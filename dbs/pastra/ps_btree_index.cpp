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

I_BTreeNode::I_BTreeNode (I_BTreeNodeManager& nodesManager,
                          const NODE_INDEX    node) :
    m_NodesMgr (nodesManager),
    m_Header (_RC (NodeHeader*, new D_UINT8 [m_NodesMgr.RawNodeSize ()]))
{
  assert (node != NIL_NODE);

  m_Header->m_NodeId = node;
  SetNullKeysCount (0);
}

I_BTreeNode::~I_BTreeNode ()
{
}

bool
I_BTreeNode::FindBiggerOrEqual (const I_BTreeKey& key,
                                KEY_INDEX&        outIndex) const
{
  if (GetKeysCount () == 0)
    return false;

  KEY_INDEX top_key    = 0;
  KEY_INDEX bottom_key = GetKeysCount() - 1;

  assert (bottom_key < KeysPerNode());

  if (IsBigger (key, top_key))
    return false;

  while (bottom_key > top_key + 1)
    {
      const KEY_INDEX median = (bottom_key + top_key) / 2;

      if (IsLess (key, median))
        top_key = median;
      else
        bottom_key = median;
    }

  if (IsEqual (key, bottom_key))
    outIndex = bottom_key;
  else if (IsBigger (key, bottom_key))
    outIndex = top_key;
  else
    outIndex = bottom_key;


  return true;
}

bool
I_BTreeNode::NeedsJoining () const
{
  if (GetKeysCount() < (KeysPerNode() / 3 - 1))
    return true;

  return false;
}

bool
I_BTreeNode::NeedsSpliting () const
{
  if (GetKeysCount () >= (2 * KeysPerNode () / 3 - 1))
    return true;

  return false;
}

NODE_INDEX
I_BTreeNode::GetChildNode (const I_BTreeKey& key) const
{
  KEY_INDEX outKey;

  if ((FindBiggerOrEqual (key, outKey) == false) ||
      (IsEqual (key, outKey) == false))
    {
      throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
    }

  return GetNodeOfKey (outKey);
}

void
I_BTreeNode::SetData (const KEY_INDEX, const D_UINT8 *)
{
  throw DBSException (NULL, _EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

void
I_BTreeNode::RemoveKey (const I_BTreeKey& key)
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
  m_NodesMgr.ReleaseNode (NodeId ());
}

//////////////////////////////////////////////////////////////////////////////////////////////////



I_BTreeNodeManager::I_BTreeNodeManager () :
    m_Sync (),
    m_NodesKeeper ()
{
}

I_BTreeNodeManager::~I_BTreeNodeManager ()
{
  map <NODE_INDEX, CachedData>::iterator it = m_NodesKeeper.begin ();

  while (it != m_NodesKeeper.end ())
    {
      assert (it->second.m_RefsCount == 0);
      delete it->second.m_pNode;

      ++it;
    }
}

void
I_BTreeNodeManager::Split (NODE_INDEX parentId, const NODE_INDEX nodeId)
{
  BTreeNodeRAI node (RetrieveNode (nodeId));

  if (node->NeedsSpliting() == false)
    return;

  if (parentId == NIL_NODE )
    {
      assert (nodeId == GetRootNodeId ());
      assert (node->IsEqual (node->SentinelKey(), 0));

      parentId = AllocateNode (NIL_NODE, 0);
      BTreeNodeRAI parentNode (RetrieveNode (parentId));

      parentNode->SetLeaf (false);
      parentNode->SetKeysCount (0);
      parentNode->SetNext (NIL_NODE);
      parentNode->SetPrev (NIL_NODE);
      parentNode->InsertKey (node->SentinelKey ());
      parentNode->SetNodeOfKey (0, nodeId);
      SetRootNodeId (parentId);

      assert (parentNode->GetKeysCount() == 1);
    }

  node->Split (parentId);
}

void
I_BTreeNodeManager::Join  (const NODE_INDEX parentId, const NODE_INDEX nodeId)
{
  NODE_INDEX splitNode = NIL_NODE;

  if (parentId == NIL_NODE)
    {
      assert (nodeId == GetRootNodeId ());
      return ;
    }
  else
    {
      assert (nodeId != GetRootNodeId ());
      BTreeNodeRAI parentNode (RetrieveNode (parentId));

      assert (parentNode->IsLeaf() == false);

      BTreeNodeRAI node (RetrieveNode (nodeId));
      const KEY_INDEX  keyIndex = node->GetParentKeyIndex (*parentNode);

      if (keyIndex != 0)
        {
          splitNode = node->GetNext ();
          assert (splitNode != NIL_NODE);

          assert ((parentNode->NeedsJoining () == false) ||
                  (parentNode->NodeId () == GetRootNodeId()));

          parentNode->RemoveKey (keyIndex);
          node->Join (true);
          FreeNode (node->NodeId());
        }
      else
        {
          const NODE_INDEX leftNode = node->GetPrev ();

          splitNode = node->NodeId ();

          assert (leftNode == parentNode->GetNodeOfKey (keyIndex + 1));

          node->Join (false);
          parentNode->RemoveKey (keyIndex + 1);
          FreeNode (leftNode);
        }
    }

  Split (parentId, splitNode);
}

I_BTreeNode*
I_BTreeNodeManager::RetrieveNode (const NODE_INDEX node)
{
  assert (node != NIL_NODE);
  WSynchronizerRAII syncHolder (m_Sync);
  map <NODE_INDEX, CachedData>::iterator it = m_NodesKeeper.find (node);

  if (it == m_NodesKeeper.end ())
    {
      auto_ptr<I_BTreeNode>        apNode (LoadNode (node));
      pair<NODE_INDEX, CachedData> cachedNode (node,
                                               CachedData (apNode.get ()));
      m_NodesKeeper.insert (cachedNode);

      apNode.release();

      it = m_NodesKeeper.find (node);
      assert (it != m_NodesKeeper.end ());
    }

  it->second.m_RefsCount++;

  I_BTreeNode* const result = it->second.m_pNode;

  if (m_NodesKeeper.size () > MaxCachedNodes())
    {
      it = m_NodesKeeper.begin ();
      while (it != m_NodesKeeper.end ())
        {
          assert (it->second.m_pNode->NodeId() == it->first);

          if (it->second.m_RefsCount == 0)
            {
              SaveNode (it->second.m_pNode);
              delete it->second.m_pNode;
              m_NodesKeeper.erase (it++);
            }
          else
            ++it;
        }
    }

  assert (m_NodesKeeper.find (node)->second.m_RefsCount > 0);
  assert (m_NodesKeeper.find (node)->second.m_pNode == result);
  assert (result->NodeId () == node);

  return result;
}

void
I_BTreeNodeManager::ReleaseNode (const NODE_INDEX node)
{
  WSynchronizerRAII syncHolder (m_Sync);
  map <NODE_INDEX, CachedData>::iterator it = m_NodesKeeper.find (node);

  assert (it != m_NodesKeeper.end ());
  assert (it->second.m_RefsCount > 0);

  it->second.m_RefsCount--;
}

void
I_BTreeNodeManager::FlushNodes ()
{
  WSynchronizerRAII syncHolder (m_Sync);
  map <NODE_INDEX, CachedData>::iterator it = m_NodesKeeper.begin ();

  while (it != m_NodesKeeper.end ())
    {
      SaveNode (it->second.m_pNode);
      it->second.m_pNode->MarkClean ();
      ++it;
    }
}

BTree::BTree (I_BTreeNodeManager& nodesManager) :
    m_NodesManager (nodesManager)
{
}

BTree::~BTree ()
{
}

bool
BTree::FindBiggerOrEqual (const I_BTreeKey& key,
                          NODE_INDEX& outNode,
                          KEY_INDEX& outKeyIndex)
{
  outNode = m_NodesManager.GetRootNodeId();

  BTreeNodeRAI node (m_NodesManager.RetrieveNode (outNode));

  do
    {
      const bool found = node->FindBiggerOrEqual (key, outKeyIndex);

      assert (found != false); //The shall be the sentinel if nothing is found.

      if ( node->IsLeaf())
        break;

      outNode = node->GetNodeOfKey (outKeyIndex);
      node    = m_NodesManager.RetrieveNode (outNode);
    }
  while (true);

  if ((outKeyIndex == 0) && node->IsEqual (node->SentinelKey(), 0))
    return false;

  return true;
}

void
BTree::InsertKey (const I_BTreeKey& key, NODE_INDEX& outNode, KEY_INDEX& outKeyIndex)
{
  bool tryAgain = false;
  do
    {
      tryAgain = RecursiveInsertNodeKey (NIL_NODE,
                                         m_NodesManager.GetRootNodeId (),
                                         key,
                                         outNode,
                                         outKeyIndex);
    }
  while (tryAgain);
}

void
BTree::RemoveKey (const I_BTreeKey& key)
{
  BTreeNodeRAI node (m_NodesManager.RetrieveNode (m_NodesManager.GetRootNodeId ()));
  RecursiveDeleteNodeKey (*node, key);

  if (node->IsLeaf () == false)
    {
      if (node->GetNodeOfKey( node->GetKeysCount () - 1) == node->GetNodeOfKey (0))
        {
          m_NodesManager.SetRootNodeId (node->GetNodeOfKey (0));
          m_NodesManager.FreeNode (node->NodeId ());
        }
    }
}

bool
BTree::RecursiveInsertNodeKey (const NODE_INDEX parentId,
                               const NODE_INDEX nodeId,
                               const I_BTreeKey& key,
                               NODE_INDEX       &outNode,
                               KEY_INDEX        &outKeyIndex)
{
  BTreeNodeRAI node (m_NodesManager.RetrieveNode (nodeId));

  if (node->NeedsSpliting ())
    {
      m_NodesManager.Split (parentId, nodeId);
      return true;
    }

  if (node->IsLeaf ())
    {
      if (node->FindBiggerOrEqual (key, outKeyIndex) == false)
        {
          // The sentinel shall be here!
          assert (0);
        }
      else if (node->IsEqual (key, outKeyIndex))
        throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      outKeyIndex ++;

      assert (outKeyIndex < node->KeysPerNode () - 1);

      node->InsertKey (key);

      outNode = nodeId;
      return false;
    }

  bool tryAgain = false;
  do
    {
      if (node->FindBiggerOrEqual (key, outKeyIndex) == false)
        {
          //There is no chance this key to be the biggest
          //The sentinel shall be the biggest.
          assert (false);
        }
        tryAgain = RecursiveInsertNodeKey (nodeId,
                                           node->GetNodeOfKey (outKeyIndex),
                                           key,
                                           outNode,
                                           outKeyIndex);
    }
  while (tryAgain);

  return false;
}

bool
BTree::RecursiveDeleteNodeKey (I_BTreeNode& node, const I_BTreeKey& key)
{
  KEY_INDEX  keyIndex;

  if (node.FindBiggerOrEqual (key, keyIndex) == false)
    {
      assert (false);
    }

  if (node.IsLeaf ())
    {
      if (node.IsEqual (key, keyIndex))
        node.RemoveKey (keyIndex);
      else
        return false;
    }
  else
    {
      BTreeNodeRAI childNode (m_NodesManager.RetrieveNode (node.GetNodeOfKey (keyIndex)));

      if (childNode->NeedsJoining ())
        {
          m_NodesManager.Join (node.NodeId (), childNode->NodeId ());
          return RecursiveDeleteNodeKey (node, key);
        }
      else if (RecursiveDeleteNodeKey (*childNode, key))
        {
          assert (node.IsEqual (key, keyIndex));
          node.AdjustKeyNode (*childNode, keyIndex);
        }
      else if (node.IsEqual (key, keyIndex))
        {
          assert (false);
          node.RemoveKey (keyIndex);
        }
      else
        return false;
    }

  return (keyIndex == 0);
}

