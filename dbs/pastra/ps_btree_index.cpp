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

using namespace std;



namespace whais {
namespace pastra {



IBTreeNode::IBTreeNode (IBTreeNodeManager&  nodesManager,
                        const NODE_INDEX    nodeId)
  : mNodesMgr (nodesManager),
    mRawNodeSize (nodesManager.NodeRawSize ()),
    mHeader (_RC (NodeHeader*, new uint8_t [mNodesMgr.NodeRawSize ()]))
{
  assert (nodeId != NIL_NODE);

  Serializer::StoreNode (nodeId, &mHeader->mNodeId);
  NullKeysCount (0);
}


IBTreeNode::~IBTreeNode ()
{
}


bool
IBTreeNode::FindBiggerOrEqual (const IBTreeKey&   key,
                              KEY_INDEX* const    outIndex) const
{
  if (KeysCount () == 0)
    return false;

  KEY_INDEX topKey    = 0;
  KEY_INDEX bottomKey = KeysCount () - 1;

  assert (bottomKey < KeysPerNode ());

  if (CompareKey (key, topKey) > 0)
    return false;

  while (bottomKey > topKey + 1)
    {
      const KEY_INDEX median = (bottomKey + topKey) / 2;

      if (CompareKey (key, median) < 0)
        topKey = median;

      else
        bottomKey = median;
    }

  const int compare = CompareKey (key, bottomKey);
  if (compare == 0)
    *outIndex = bottomKey;

  else if (compare > 0)
    *outIndex = topKey;

  else
    *outIndex = bottomKey;

  return true;
}


bool
IBTreeNode::NeedsJoining () const
{
  /* A node with less than 1/3 of it's capacity should be joined. */

  if (KeysCount () < (KeysPerNode () / 3 - 1))
    return true;

  return false;
}


bool
IBTreeNode::NeedsSpliting () const
{
  /* Do not allow one node to have more than 2/3 of it's capacity filed.
     It needs to safely allow joining of one of it's neighbors. */

  if (KeysCount () >= (2 * KeysPerNode () / 3 - 1))
    return true;

  return false;
}


NODE_INDEX
IBTreeNode::GetChildNode (const IBTreeKey& key) const
{
  KEY_INDEX outKey;

  if ( ! FindBiggerOrEqual (key, &outKey)
      ||  (CompareKey (key, outKey) != 0))
    {
      throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
    }

  return NodeIdOfKey (outKey);
}


void
IBTreeNode::SetData (const KEY_INDEX, const uint8_t* )
{
  throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


void
IBTreeNode::RemoveKey (const IBTreeKey& key)
{
  KEY_INDEX keyIndex = 0;

  if (FindBiggerOrEqual (key, &keyIndex) == false)
    {
      assert (false);
    }
  else if (CompareKey (key, keyIndex) == 0)
    {
      assert (false);
    }

  RemoveKey (keyIndex);
}

void
IBTreeNode::Release ()
{
  mNodesMgr.ReleaseNode (NodeId ());
}



IBTreeNodeManager::IBTreeNodeManager ()
  : mSync (),
    mNodesKeeper ()
{
}


IBTreeNodeManager::~IBTreeNodeManager ()
{
  map<NODE_INDEX, CachedData>::iterator it = mNodesKeeper.begin ();

  while (it != mNodesKeeper.end ())
    {
      assert (it->second.mRefsCount == 0);

      delete it->second.mNode;

      ++it;
    }
}


void
IBTreeNodeManager::Split (NODE_INDEX parentId, const NODE_INDEX nodeId)
{
  BTreeNodeRAII node (RetrieveNode (nodeId));

  if (node->NeedsSpliting () == false)
    return;

  if (parentId == NIL_NODE )
    {
      assert (nodeId == RootNodeId ());
      assert (node->CompareKey (node->SentinelKey (), 0) == 0);

      parentId = AllocateNode (NIL_NODE, 0);

      BTreeNodeRAII parentNode (RetrieveNode (parentId));

      parentNode->Leaf (false);
      parentNode->KeysCount (0);
      parentNode->Next (NIL_NODE);
      parentNode->Prev (NIL_NODE);
      parentNode->InsertKey (node->SentinelKey ());
      parentNode->SetNodeOfKey (0, nodeId);
      RootNodeId (parentId);

      assert (parentNode->KeysCount () == 1);
    }

  node->Split (parentId);
}


void
IBTreeNodeManager::Join (const NODE_INDEX parentId, const NODE_INDEX nodeId)
{
  NODE_INDEX splitNode = NIL_NODE;

  if (parentId == NIL_NODE)
    {
      assert (nodeId == RootNodeId ());

      return ;
    }
  else
    {
      assert (nodeId != RootNodeId ());

      BTreeNodeRAII parentNode (RetrieveNode (parentId));

      assert (parentNode->IsLeaf () == false);

      BTreeNodeRAII node (RetrieveNode (nodeId));

      const KEY_INDEX keyIndex = node->GetParentKeyIndex (*parentNode);

      if (keyIndex != 0)
        {
          splitNode = node->Next ();

          assert (splitNode != NIL_NODE);
          assert ((parentNode->NeedsJoining () == false)
                  || (parentNode->NodeId () == RootNodeId ()));

          parentNode->RemoveKey (keyIndex);
          node->Join (true);
          FreeNode (node->NodeId ());
        }
      else
        {
          const NODE_INDEX leftNode = node->Prev ();

          splitNode = node->NodeId ();

          assert (leftNode == parentNode->NodeIdOfKey (keyIndex + 1));

          node->Join (false);
          parentNode->RemoveKey (keyIndex + 1);
          FreeNode (leftNode);
        }
    }
  Split (parentId, splitNode);
}


IBTreeNode*
IBTreeNodeManager::RetrieveNode (const NODE_INDEX nodeId)
{
  assert (nodeId != NIL_NODE);

  LockRAII<Lock> syncHolder (mSync);
  map<NODE_INDEX, CachedData>::iterator it = mNodesKeeper.find (nodeId);

  if (it == mNodesKeeper.end ())
    {
      auto_ptr<IBTreeNode>         node (LoadNode (nodeId));
      pair<NODE_INDEX, CachedData> cachedNode (nodeId,
                                               CachedData (node.get ()));
      mNodesKeeper.insert (cachedNode);
      node.release ();

      it = mNodesKeeper.find (nodeId);

      assert (it != mNodesKeeper.end ());
    }

  it->second.mRefsCount++;

  IBTreeNode* const result = it->second.mNode;

  if (mNodesKeeper.size () > MaxCachedNodes ())
    {
      it = mNodesKeeper.begin ();
      while (it != mNodesKeeper.end ())
        {
          assert (it->second.mNode->NodeId () == it->first);

          if ((it->second.mRefsCount == 0) && (it->first != RootNodeId ()))
            {
              SaveNode (it->second.mNode);

              delete it->second.mNode;

              mNodesKeeper.erase (it++);
            }
          else
            ++it;
        }
    }

  assert (mNodesKeeper.find (nodeId)->second.mRefsCount > 0);
  assert (mNodesKeeper.find (nodeId)->second.mNode == result);
  assert (result->NodeId () == nodeId);

  return result;
}


void
IBTreeNodeManager::ReleaseNode (const NODE_INDEX nodeId)
{
  LockRAII<Lock> syncHolder (mSync);
  map<NODE_INDEX, CachedData>::iterator it = mNodesKeeper.find (nodeId);

  assert (it != mNodesKeeper.end ());
  assert (it->second.mRefsCount > 0);

  it->second.mRefsCount--;
}


void
IBTreeNodeManager::FlushNodes ()
{
  LockRAII<Lock> syncHolder (mSync);
  map <NODE_INDEX, CachedData>::iterator it = mNodesKeeper.begin ();

  while (it != mNodesKeeper.end ())
    {
      SaveNode (it->second.mNode);
      it->second.mNode->MarkClean ();

      ++it;
    }
}



BTree::BTree (IBTreeNodeManager& nodesManager)
  : mNodesManager (nodesManager)
{
}


bool
BTree::FindBiggerOrEqual (const IBTreeKey& key,
                          NODE_INDEX*      outNode,
                          KEY_INDEX*       outKeyIndex)
{
  *outNode = mNodesManager.RootNodeId ();

  BTreeNodeRAII node (mNodesManager.RetrieveNode (*outNode));

  do
    {
      const bool found = node->FindBiggerOrEqual (key, outKeyIndex);

      (void)found;
      assert (found != false);

      if ( node->IsLeaf ())
        break;

      *outNode = node->NodeIdOfKey (*outKeyIndex);
      node     = mNodesManager.RetrieveNode (*outNode);
    }
  while (true);

  if ((*outKeyIndex == 0)
      && (node->CompareKey (node->SentinelKey (), 0) == 0))
    {
      return false;
    }

  return true;
}


void
BTree::InsertKey (const IBTreeKey&  key,
                  NODE_INDEX*       outNode,
                  KEY_INDEX*        outKeyIndex)
{
  bool tryAgain = false;
  do
    {
      tryAgain = RecursiveInsertNodeKey (NIL_NODE,
                                         mNodesManager.RootNodeId (),
                                         key,
                                         outNode,
                                         outKeyIndex);
    }
  while (tryAgain);
}


void
BTree::RemoveKey (const IBTreeKey& key)
{
  BTreeNodeRAII node (mNodesManager.RetrieveNode (mNodesManager.RootNodeId ()));

  RecursiveDeleteNodeKey (*node, key);

  if (node->IsLeaf () == false)
    {
      if (node->NodeIdOfKey ( node->KeysCount () - 1) == node->NodeIdOfKey (0))
        {
          mNodesManager.RootNodeId (node->NodeIdOfKey (0));
          mNodesManager.FreeNode (node->NodeId ());
        }
    }
}


bool
BTree::RecursiveInsertNodeKey (const NODE_INDEX         parentId,
                               const NODE_INDEX         nodeId,
                               const IBTreeKey&         key,
                               NODE_INDEX*              outNode,
                               KEY_INDEX*               outKeyIndex)
{
  BTreeNodeRAII node (mNodesManager.RetrieveNode (nodeId));

  if (node->NeedsSpliting ())
    {
      mNodesManager.Split (parentId, nodeId);
      return true;
    }

  if (node->IsLeaf ())
    {
      if (node->FindBiggerOrEqual (key, outKeyIndex) == false)
        {
          //The sentinel shall be here!
          assert (0);
        }
      else if (node->CompareKey (key, *outKeyIndex) == 0)
        throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      *outKeyIndex += 1;

      assert (*outKeyIndex < node->KeysPerNode () - 1);

      node->InsertKey (key);
      *outNode = nodeId;

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
                                           node->NodeIdOfKey (*outKeyIndex),
                                           key,
                                           outNode,
                                           outKeyIndex);
    }
  while (tryAgain);

  return false;
}


bool
BTree::RecursiveDeleteNodeKey (IBTreeNode& node, const IBTreeKey& key)
{
  KEY_INDEX keyIndex = ~0;

  if (node.FindBiggerOrEqual (key, &keyIndex) == false)
    {
      assert (false);
    }

  if (node.IsLeaf ())
    {
      if (node.CompareKey (key, keyIndex) == 0)
        node.RemoveKey (keyIndex);

      else
        return false;
    }
  else
    {
      const NODE_INDEX childNodeId = node.NodeIdOfKey (keyIndex);
      BTreeNodeRAII childNode (mNodesManager.RetrieveNode (childNodeId));

      if (childNode->NeedsJoining ())
        {
          mNodesManager.Join (node.NodeId (), childNode->NodeId ());

          return RecursiveDeleteNodeKey (node, key);
        }
      else if (RecursiveDeleteNodeKey (*childNode, key))
        {
          assert (node.CompareKey (key, keyIndex) == 0);

          node.AdjustKeyNode (*childNode, keyIndex);
        }
      else if (node.CompareKey (key, keyIndex) == 0)
        {
          assert (false);

          node.RemoveKey (keyIndex);
        }
      else
        return false;
    }

  return (keyIndex == 0);
}

} //namespace pastra
} //namespace whais

