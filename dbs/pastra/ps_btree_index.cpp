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
    m_NodesManager (nodesManager)
{
    (void)node;
}

I_BTreeNode::~I_BTreeNode ()
{
}

bool
I_BTreeNode::FindBiggerOrEqual (const I_BTreeKey &key, KEY_INDEX &outIndex) const
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

bool
I_BTreeNode::NeedsJoining () const
{
  if (GetKeysCount() < (GetKeysPerNode() / 3))
    return true;

  return false;
}

bool
I_BTreeNode::NeedsSpliting () const
{
  if (GetKeysCount () > (2 * GetKeysPerNode () / 3))
    return true;

  return false;
}

NODE_INDEX
I_BTreeNode::GetChildNode (const I_BTreeKey &key) const
{
  KEY_INDEX outKey;

  if ((FindBiggerOrEqual (key, outKey) == false) ||
      (IsEqual (key, outKey) == false))
    throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

  return GetChildNode (outKey);
}

void
I_BTreeNode::SetData (const KEY_INDEX, const D_UINT8 *)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

void
I_BTreeNode::RemoveKey (const I_BTreeKey &key)
{
  KEY_INDEX keyIndex;

  if (FindBiggerOrEqual (key, keyIndex) == false)
    {
      assert (0);
    }
  else if (IsEqual (key, keyIndex) == false )
    {
      assert (0);
    }

  RemoveKey (keyIndex);
}

void
I_BTreeNode::Release ()
{
  m_NodesManager.ReleaseNode (GetNodeId ());
}



//////////////////////////////////////////////////////////////////////////////////////////////////



I_BTreeNodeManager::I_BTreeNodeManager () :
    m_Sync (),
    m_NodesKeeper ()
{
}

I_BTreeNodeManager::~I_BTreeNodeManager ()
{
}

void
I_BTreeNodeManager::Split (NODE_INDEX parentId, const NODE_INDEX nodeId)
{
  BTreeNodeHandler node (RetrieveNode (nodeId));

  assert (node->NeedsSpliting ());

  if (parentId == NIL_NODE )
    {
      assert (nodeId == GetRootNodeId ());
      assert (node->IsEqual (node->GetSentinelKey(), 0));

      parentId = AllocateNode (NIL_NODE, 0);
      BTreeNodeHandler parentNode (RetrieveNode (parentId));

      parentNode->SetLeaf (false);
      parentNode->SetKeysCount (0);
      parentNode->InsertKey (node->GetSentinelKey ());
      parentNode->SetChildNode (0, nodeId);
      SetRootNodeId (parentId);

      assert (parentNode->GetKeysCount() == 0);
    }
  else
    {
      assert (nodeId != GetRootNodeId ());
    }

  node->Split (parentId);
}

void
I_BTreeNodeManager::Join  (const NODE_INDEX parentId, const NODE_INDEX nodeId)
{
  if (parentId == NIL_NODE)
    {
      assert (nodeId == GetRootNodeId ());
      return ;
    }
  else
    {
      assert (nodeId != GetRootNodeId ());
      BTreeNodeHandler parentNode (RetrieveNode (parentId));

      assert (parentNode->IsLeaf() == false);

      BTreeNodeHandler node (RetrieveNode (nodeId));
      const KEY_INDEX  keyIndex = node->GetParentKey (*parentNode);

      if (keyIndex != 0)
        {
          assert ((parentNode->NeedsJoining() == false) ||
                  (parentNode->GetNodeId () == GetRootNodeId()));
          parentNode->RemoveKey (keyIndex);
          node->Join (true);
        }
      else
        {
          const NODE_INDEX leftNode = node->GetPrev ();
          assert (leftNode == parentNode->GetChildNode (keyIndex + 1));

          node->Join (false);

          parentNode->SetChildNode (keyIndex, leftNode);
          parentNode->RemoveKey (keyIndex + 1);
        }
    }
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
BTree::FindBiggerOrEqual (const I_BTreeKey &key,
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

      outNode = node->GetChildNode (outKeyIndex);
      node    = m_NodesManager.RetrieveNode (outNode);
    }
  while (found);

  return found;
}

void
BTree::InsertKey (const I_BTreeKey &key, NODE_INDEX &outNode, KEY_INDEX &outKeyIndex)
{
  NODE_INDEX       parentId = NIL_NODE;
  BTreeNodeHandler itNode (m_NodesManager.RetrieveNode (m_NodesManager.GetRootNodeId ()));

  do
    {
      if (itNode->NeedsSpliting ())
        {
          m_NodesManager.Split (parentId, itNode->GetNodeId());

          //We don't know where the key could reside here.
          //So search the parent again! Do not worry we won't need to split again.
          itNode = m_NodesManager.RetrieveNode ((parentId == NIL_NODE) ?
                                                 m_NodesManager.GetRootNodeId () : parentId);
          continue;
        }

      if (itNode->IsLeaf ())
        {
          outKeyIndex ++;
          outNode = itNode->GetNodeId ();
          itNode->InsertKey (key);
          break;
        }
      else
        {
          if (itNode->FindBiggerOrEqual (key, outKeyIndex) == false)
            {
              //There is no chance this key to be the biggest
              //The sentinel shall be the biggest.
              assert (false);
            }
          else
            {
              assert (outNode == itNode->GetNodeId ());
              assert (itNode->IsLess (key, outKeyIndex));
            }
          parentId = itNode->GetNodeId ();
          itNode   = m_NodesManager.RetrieveNode (itNode->GetChildNode (outKeyIndex));
        }
    }
  while (true);
}

void
BTree::RemoveKey (I_BTreeKey &key)
{
  BTreeNodeHandler node (m_NodesManager.RetrieveNode (m_NodesManager.GetRootNodeId ()));
  DeleteNodeKey (*node, key);

  if (node->IsLeaf () == false)
    {
      if (node->GetChildNode( node->GetKeysCount () - 1) == node->GetChildNode (0));
      {
        m_NodesManager.SetRootNodeId (node->GetChildNode (0));
        m_NodesManager.FreeNode (node->GetNodeId ());
      }
    }
}

bool
BTree::DeleteNodeKey (I_BTreeNode &node, const I_BTreeKey &key)
{
  KEY_INDEX  keyIndex;

  node.FindBiggerOrEqual (key, keyIndex);

  if (node.IsLeaf ())
    {
      if (node.IsEqual (key, keyIndex))
        node.RemoveKey (keyIndex);
      else
        return false;
    }
  else
    {
      BTreeNodeHandler childNode (m_NodesManager.RetrieveNode (node.GetChildNode (keyIndex)));

      if (childNode->NeedsJoining ())
        {
          m_NodesManager.Join (node.GetNodeId (), childNode->GetNodeId ());
          return DeleteNodeKey (node, key);
        }
      else if (DeleteNodeKey (*childNode, key))
        {
          assert (node.IsEqual (key, keyIndex));
          node.ResetKeyNode (*childNode, keyIndex);
        }
      else if (node.IsEqual (key, keyIndex))
        node.RemoveKey (keyIndex);
      else
        return false;
    }

  return (keyIndex == 0);
}

