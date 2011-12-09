/******************************************************************************
 PASTRA - A light database one file system and more.
 Copyright (C) 2008  Iulian Popa

 Address: Str Olimp nr. 6
 Pantelimon Ilfov,
 Romania
 Phone:   +40721939650
 e-mail:  popaiulian@gmail.com

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
#include <assert.h>

#include "dbs_exception.h"

#include "ps_btree_index.h"

using namespace pastra;
using namespace std;

I_BTreeNode::I_BTreeNode (I_BTreeNodeManager &nodesManager, const NODE_INDEX node) :
    m_Header (NULL),
    m_NodesManager (nodesManager),
    m_NodeIndex (node)
{
}

I_BTreeNode::~I_BTreeNode ()
{
}

bool
I_BTreeNode::FindBiggerOrEqual (I_BTreeKey &key, KEY_INDEX &outIndex)
{
  KEY_INDEX top_key    = 0;
  KEY_INDEX bottom_key = GetKeysCount() - 1;

  assert (bottom_key < GetKeysPerNode());

  if (IsBigger (key, top_key))
    return false;

  do
    {
      const KEY_INDEX median = (bottom_key + top_key) / 2;

      if (IsLess (key, median))
        top_key = median;
      else
        bottom_key = median;
    }
  while (bottom_key > top_key + 1);

  outIndex = top_key;
  return true;
}

void
I_BTreeNode::Release ()
{
  m_NodesManager.ReleaseNode (m_NodeIndex);
}

I_BTreeNodeManager::I_BTreeNodeManager () :
    m_Sync (),
    m_NodesKeeper ()
{
}

I_BTreeNodeManager::~I_BTreeNodeManager ()
{
}

I_BTreeNode*
I_BTreeNodeManager::RetrieveNode (const NODE_INDEX node)
{
  WSynchronizerHolder syncHnd (m_Sync);
  map <NODE_INDEX, CachedData>::iterator it = m_NodesKeeper.find (node);

  if (it == m_NodesKeeper.end ())
    {
      auto_ptr <I_BTreeNode> apNode (GetNode (node));
      m_NodesKeeper.insert ( pair <NODE_INDEX, CachedData> (node, CachedData (apNode.get (), 0)));

      apNode.release();

      it = m_NodesKeeper.find (node);
    }

  it->second.m_ReferenceCount++;
  return it->second.m_pNode;
}

void
I_BTreeNodeManager::ReleaseNode (const NODE_INDEX node)
{
  WSynchronizerHolder syncHnd (m_Sync);
  map <NODE_INDEX, CachedData>::iterator it = m_NodesKeeper.find (node);

  assert (it != m_NodesKeeper.end ());
  assert (it->second.m_ReferenceCount > 0);

  if (it->second.m_ReferenceCount == 0)
    m_NodesKeeper.erase (it);
}

BTree::BTree (I_BTreeNodeManager &nodesManager) :
    m_NodesManager (nodesManager)
{
}

BTree::~BTree ()
{
}

bool
BTree::FindBiggerOrEqual (I_BTreeKey &key,
                          NODE_INDEX &outNode,
                          KEY_INDEX &outKeyIndex)
{
  outNode = m_NodesManager.GetRootNodeId();

  BTreeNodeHandler node (m_NodesManager.RetrieveNode (outNode));
  bool             found = true;

  do
    {
      found = node->FindBiggerOrEqual (key, outKeyIndex);

      if ( (found == false) || node->IsLeaf())
        break;

      outNode = node->GetKeyNode (outKeyIndex);
      node    = m_NodesManager.RetrieveNode (outNode);
    }
  while (found);

  return found;
}

void
BTree::InsertKey (I_BTreeKey &key, NODE_INDEX &outNode, KEY_INDEX &outKeyIndex)
{
  NODE_INDEX       keyNode  = m_NodesManager.GetRootNodeId ();
  KEY_INDEX        keyIndex;

  do
    {
      BTreeNodeHandler node (m_NodesManager.RetrieveNode (keyNode));

      if (node->NeedsSpliting ())
        {
          keyNode = node->Split (m_NodesManager);
          node    = m_NodesManager.RetrieveNode (keyNode);

          continue;
        }

      if (node->FindBiggerOrEqual (key, keyIndex) == false)
        {
          keyIndex = node->InsertKey (key);
          if (node->IsLeaf () == false)
            {
              keyNode  = m_NodesManager.AllocateNode (keyNode, keyIndex);
              node->SetChild (keyIndex, keyNode);
            }
        }
      else if (node->IsEqual (key, keyIndex))
        {
          if (node->IsLeaf ())
            break;

          assert (node->GetKeyNode (keyIndex) == NIL_NODE);

          keyNode  = m_NodesManager.AllocateNode (keyNode, keyIndex);
          node->SetChild (keyIndex, keyNode);
        }
      else
        {
          assert (node->IsLess (key, keyIndex));

          if (node->IsLeaf() == true)
            {
              keyIndex = node->InsertKey (key);
              break;
            }

          keyNode = node->GetKeyNode (keyIndex);
        }
    }
  while (true);

  outKeyIndex = keyIndex;
  outNode     = keyNode;
}

void
BTree::RemoveKey (I_BTreeKey &key)
{
  NODE_INDEX keyNode;
  KEY_INDEX  keyIndex;

  if (FindBiggerOrEqual (key, keyNode, keyIndex) == false)
    throw DBSException (NULL, _EXTRA( DBSException::GENERAL_CONTROL_ERROR));

  BTreeNodeHandler node (m_NodesManager.RetrieveNode (keyNode));

  if (node->IsEqual(key, keyIndex) == false)
    throw DBSException (NULL, _EXTRA( DBSException::GENERAL_CONTROL_ERROR));

  node->RemoveKey (keyIndex);

  if (node->NeedsJoining())
    node->Join (m_NodesManager);
}



