/******************************************************************************
 PASTRA - A light database one file system and more.
 Copyright(C) 2008  Iulian Popa

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

#include "ps_btree_fields.h"


using namespace std;

namespace whais {
namespace pastra {


FieldIndexNodeManager::FieldIndexNodeManager(unique_ptr<IDataContainer>&   container,
                                             const uint_t                  nodeSize,
                                             const uint_t                  maxCacheMem,
                                             const DBS_FIELD_TYPE          fieldType,
                                             const bool                    create)
  : mNodeSize(nodeSize),
    mMaxCachedMem(maxCacheMem),
    mRootNode(NIL_NODE),
    mFirstFreeNode(NIL_NODE),
    mContainer(container.release()),
    mFieldType(fieldType)
{

  if (create)
    InitContainer();

  InitFromContainer();
}

FieldIndexNodeManager::~FieldIndexNodeManager()
{
  FlushNodes();
}

uint64_t
FieldIndexNodeManager::NodeRawSize() const
{
  return mNodeSize;
}

void
FieldIndexNodeManager::MarkForRemoval()
{
  mContainer->MarkForRemoval();
}

uint64_t
FieldIndexNodeManager::IndexRawSize() const
{
  return mContainer->Size();
}

NODE_INDEX
FieldIndexNodeManager::AllocateNode(const NODE_INDEX parent, const KEY_INDEX parentKey)
{
  NODE_INDEX nodeIndex = mFirstFreeNode;

  if (nodeIndex != NIL_NODE)
  {
    auto freeNode = RetrieveNode(nodeIndex);
    mFirstFreeNode = freeNode->Next();

    UpdateContainer();
  }
  else
  {
    assert(mContainer->Size() % NodeRawSize() == 0);

    nodeIndex = mContainer->Size() / NodeRawSize();
  }

  if (parent != NIL_NODE)
  {
    auto parentNode = RetrieveNode(parent);

    parentNode->SetNodeOfKey(parentKey, nodeIndex);

    assert(parentNode->IsLeaf() == false);
  }

  assert(nodeIndex > 0);
  assert(nodeIndex != mFirstFreeNode);

  return nodeIndex;
}

void
FieldIndexNodeManager::FreeNode(const NODE_INDEX nodeId)
{
  auto node = RetrieveNode(nodeId);

  node->MarkAsRemoved();
  node->Next(mFirstFreeNode);

  mFirstFreeNode = node->NodeId();

  UpdateContainer();
}

NODE_INDEX
FieldIndexNodeManager::RootNodeId()
{
  if (mRootNode == NIL_NODE)
  {
    auto rootNode = RetrieveNode(AllocateNode(NIL_NODE, 0));

    rootNode->Next(NIL_NODE);
    rootNode->Prev(NIL_NODE);
    rootNode->KeysCount(0);
    rootNode->Leaf(true);
    rootNode->InsertKey(rootNode->SentinelKey());

    RootNodeId(rootNode->NodeId());
  }

  return mRootNode;
}

void
FieldIndexNodeManager::RootNodeId(const NODE_INDEX nodeId)
{
  assert(nodeId != NIL_NODE);

  mRootNode = nodeId;
  assert(mFirstFreeNode != mRootNode);

  UpdateContainer();
}

uint_t
FieldIndexNodeManager::MaxCachedNodes()
{
  return mMaxCachedMem / mNodeSize;
}

std::shared_ptr<IBTreeNode>
FieldIndexNodeManager::LoadNode(const NODE_INDEX nodeId)
{
  assert(nodeId > 0);

  std::shared_ptr<IBTreeNode> node(NodeFactory(nodeId));

  assert(mContainer->Size() % NodeRawSize() == 0);

  if (mContainer->Size() > nodeId * NodeRawSize())
    mContainer->Read(nodeId * NodeRawSize(), NodeRawSize(), node->RawData());

  else
  {
    assert(mContainer->Size() == nodeId * NodeRawSize());

    //Reserve the required space
    mContainer->Write(nodeId * NodeRawSize(), NodeRawSize(), node->RawData());
  }

  node->MarkClean();
  assert(node->NodeId() == nodeId);

  return node;
}

void
FieldIndexNodeManager::SaveNode(IBTreeNode* const node)
{
  assert(node->NodeId() > 0);
  assert(mContainer->Size() > node->NodeId() * NodeRawSize());
  assert(mContainer->Size() % NodeRawSize() == 0);

  if (node->IsDirty() == false)
    return;

  mContainer->Write(node->NodeId() * NodeRawSize(), NodeRawSize(), node->RawData());

  node->MarkClean();
}

void
FieldIndexNodeManager::InitContainer()
{
  unique_ptr<IBTreeNode> node(NodeFactory(0));

  node->Next(NIL_NODE);
  node->Prev(NIL_NODE);

  mContainer->Write(0, NodeRawSize(), node->RawData());
}

void
FieldIndexNodeManager::UpdateContainer()
{
  unique_ptr<IBTreeNode> node(NodeFactory(0));

  node->Next(mFirstFreeNode);
  node->Prev(mRootNode);

  mContainer->Write(0, NodeRawSize(), node->RawData());
}

void
FieldIndexNodeManager::InitFromContainer()
{
  unique_ptr<IBTreeNode> node(NodeFactory(0));

  mContainer->Read(0, NodeRawSize(), node->RawData());

  mFirstFreeNode = node->Next();
  mRootNode = node->Prev();
}

IBTreeNode *
FieldIndexNodeManager::NodeFactory(const NODE_INDEX nodeId)
{
  IBTreeNode* result = nullptr;

  switch(mFieldType)
    {
  case T_BOOL:
    result = new BoolBTreeNode(*this, nodeId);
    break;

  case T_CHAR:
    result = new CharBTreeNode(*this, nodeId);
    break;

  case T_DATE:
    result = new DateBTreeNode(*this, nodeId);
    break;

  case T_DATETIME:
    result = new DateTimeBTreeNode(*this, nodeId);
    break;

  case T_HIRESTIME:
    result = new HiresTimeBTreeNode(*this, nodeId);
    break;

  case T_UINT8:
    result = new UInt8BTreeNode(*this, nodeId);
    break;

  case T_UINT16:
    result = new UInt16BTreeNode(*this, nodeId);
    break;

  case T_UINT32:
    result = new UInt32BTreeNode(*this, nodeId);
    break;

  case T_UINT64:
    result = new UInt64BTreeNode(*this, nodeId);
    break;

  case T_INT8:
    result = new Int8BTreeNode(*this, nodeId);
    break;

  case T_INT16:
    result = new Int16BTreeNode(*this, nodeId);
    break;

  case T_INT32:
    result = new Int32BTreeNode(*this, nodeId);
    break;

  case T_INT64:
    result = new Int64BTreeNode(*this, nodeId);
    break;

  case T_REAL:
    result = new RealBTreeNode(*this, nodeId);
    break;

  case T_RICHREAL:
    result = new RichRealBTreeNode(*this, nodeId);
    break;

  default:
    assert(false);
    }

  assert(result != nullptr);
  return result;
}


} //namespace pastra
} //namespace whais
