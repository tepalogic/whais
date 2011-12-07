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

static inline bool
find_bigger_or_equal (I_BTreeNode &node, I_BTreeKey &key, D_UINT &outIndex)
{
  D_UINT top_key    = 0;
  D_UINT bottom_key = node.GetKeysCount() - 1;

  assert (bottom_key < node.GetKeysPerNode());

  if (node.IsBigger (key, top_key))
    return false;

  do
    {
      const D_UINT median = (bottom_key + top_key) / 2;

      if (node.IsLess (key, median))
        top_key = median;
      else
        bottom_key = median;
    }
  while (bottom_key > top_key + 1);

  outIndex = top_key;
  return true;
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
                          D_UINT &outKeyIndex,
                          const bool splitCheck)
{
  I_BTreeNode *pTreeNode   = &m_NodesManager.GetRoot ();
  bool         found       = true;

  outNode = m_NodesManager.GetRootNodeId();
  do
    {
      if (splitCheck &&
          pTreeNode->NeedsSpliting())
        {
          outNode = pTreeNode->Split (m_NodesManager);

          //Search the the key again!
          pTreeNode = &m_NodesManager.GetNode (outNode);
          continue;
        }

      found = find_bigger_or_equal (*pTreeNode, key, outKeyIndex);

      if ( (found == false) || pTreeNode->IsLeaf())
        break;

      outNode   = pTreeNode->GetKeyNode (outKeyIndex);
      pTreeNode = &m_NodesManager.GetNode (outNode);
    }
  while (found);

  return found;
}

void
BTree::InsertKey (I_BTreeKey &key, NODE_INDEX &outNode, D_UINT &outKeyIndex)
{
  D_UINT       keyNode  = m_NodesManager.GetRootNodeId ();
  NODE_INDEX   keyIndex;
  I_BTreeNode *pNode;

  do
    {
      pNode = &m_NodesManager.GetNode (keyNode);

      if (pNode->NeedsSpliting ())
        {
          keyNode = pNode->Split (m_NodesManager);
          pNode   = &m_NodesManager.GetNode (keyNode);

          continue;
        }

      if (find_bigger_or_equal (*pNode, key, keyIndex) == false)
        {
          keyIndex = pNode->InsertKey (key);
          if (pNode->IsLeaf () == false)
            {
              keyNode  = m_NodesManager.AllocateNode (keyNode, keyIndex);
              pNode->SetChild (keyIndex, keyNode);
            }
        }
      else if (pNode->IsEqual (key, keyIndex))
        {
          if (pNode->IsLeaf ())
            break;

          assert (pNode->GetKeyNode (keyIndex) == NIL_NODE);

          keyNode  = m_NodesManager.AllocateNode (keyNode, keyIndex);
          pNode->SetChild (keyIndex, keyNode);
        }
      else
        {
          assert (pNode->IsLess (key, keyIndex));

          if (pNode->IsLeaf() == true)
            {
              keyIndex = pNode->InsertKey (key);
              break;
            }

          keyNode  = pNode->GetKeyNode (keyIndex);
        }
    }
  while (pNode->IsLeaf() == false);

  outKeyIndex = keyIndex;
  outNode     = keyNode;
}

void
BTree::RemoveKey (I_BTreeKey &key)
{
  NODE_INDEX keyNode;
  D_UINT     keyIndex;

  if (FindBiggerOrEqual (key, keyNode, keyIndex, false) == false)
    throw DBSException (NULL, _EXTRA( DBSException::GENERAL_CONTROL_ERROR));

  I_BTreeNode &node = m_NodesManager.GetNode (keyNode);

  if (node.IsEqual(key, keyNode) == false)
    throw DBSException (NULL, _EXTRA( DBSException::GENERAL_CONTROL_ERROR));

  node.RemoveKey (keyIndex);

  if (node.NeedsJoining())
    node.Join (m_NodesManager);
}

