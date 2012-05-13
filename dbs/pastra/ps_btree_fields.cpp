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

#include "ps_btree_fields.h"

using namespace pastra;



FieldIndexNodeManager::FieldIndexNodeManager (std::auto_ptr <I_DataContainer> &container,
                                              const D_UINT nodeSize,
                                              const D_UINT maxCacheMem,
                                              const DBS_FIELD_TYPE fieldType,
                                              const bool create) :
  m_NodeSize (nodeSize),
  m_MaxCachedMem (maxCacheMem),
  m_RootNode (NIL_NODE),
  m_FirstFreeNode (NIL_NODE),
  m_Container (container.release ()),
  m_FieldType (fieldType)
{

  if (create)
    InitContainer ();

  InitFromContainer ();
}

FieldIndexNodeManager::~FieldIndexNodeManager()
{
  FlushNodes ();
}

D_UINT
FieldIndexNodeManager::GetRawNodeSize () const
{
  return m_NodeSize;
}


void
FieldIndexNodeManager::MarkForRemoval ()
{
  m_Container->MarkForRemoval ();
}

D_UINT64
FieldIndexNodeManager::GetIndexRawSize () const
{
  return m_Container->GetContainerSize ();
}

NODE_INDEX
FieldIndexNodeManager::AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey)
{
  NODE_INDEX nodeIndex = m_FirstFreeNode;

  if (nodeIndex != NIL_NODE)
    {
      BTreeNodeHandler freeNode (RetrieveNode (nodeIndex));
      m_FirstFreeNode = freeNode->GetNext ();

      UpdateContainer ();
    }
  else
    {
      assert (m_Container->GetContainerSize() % GetRawNodeSize () == 0);
      nodeIndex = m_Container->GetContainerSize() / GetRawNodeSize ();
    }

  if (parent != NIL_NODE)
    {
      BTreeNodeHandler parentNode (RetrieveNode (parent));
      parentNode->SetChildNode (parentKey, nodeIndex);

      assert (parentNode->IsLeaf() == false);
    }

  return nodeIndex;
}

void
FieldIndexNodeManager::FreeNode (const NODE_INDEX nodeId)
{
  BTreeNodeHandler node (RetrieveNode (nodeId));

  node->MarkAsRemoved();
  node->SetNext (m_FirstFreeNode);

  m_FirstFreeNode = node->GetNodeId();
  UpdateContainer ();
}

NODE_INDEX
FieldIndexNodeManager::GetRootNodeId ()
{
  return m_RootNode;
}

void
FieldIndexNodeManager::SetRootNodeId (const NODE_INDEX node)
{

  m_RootNode = node;
  assert (m_FirstFreeNode != m_RootNode);

  UpdateContainer ();
}

D_UINT
FieldIndexNodeManager::GetMaxCachedNodes ()
{
  return m_MaxCachedMem / m_NodeSize;
}


I_BTreeNode *
FieldIndexNodeManager::GetNode (const NODE_INDEX nodeId)
{
  assert (nodeId > 0);

  std::auto_ptr <I_BTreeNode> apNode (NodeFactory (nodeId));

  assert (m_Container->GetContainerSize () % GetRawNodeSize () == 0);

  if (m_Container->GetContainerSize () > nodeId * GetRawNodeSize ())
    m_Container->RetrieveData (nodeId * GetRawNodeSize (), GetRawNodeSize (), apNode->GetRawData ());
  else
    {
      assert (m_Container->GetContainerSize () == nodeId * GetRawNodeSize ());
      //Reserve the required space
      m_Container->StoreData (nodeId * GetRawNodeSize (), GetRawNodeSize (), apNode->GetRawData ());
    }

  return apNode.release ();
}

void
FieldIndexNodeManager::StoreNode (I_BTreeNode *const pNode)
{

  assert (pNode->GetNodeId() > 0);
  assert (m_Container->GetContainerSize () > pNode->GetNodeId () * GetRawNodeSize());
  assert (m_Container->GetContainerSize() % GetRawNodeSize () == 0);

  if (pNode->IsDirty() == false)
    return;

  m_Container->StoreData (pNode->GetNodeId() * GetRawNodeSize (),
                          GetRawNodeSize (),
                          pNode->GetRawData ());
}

void
FieldIndexNodeManager::InitContainer ()
{
  std::auto_ptr <I_BTreeNode> apNode (NodeFactory (0));

  apNode->SetNext (NIL_NODE);
  apNode->SetPrev (NIL_NODE);

  m_Container->StoreData (0, GetRawNodeSize (), apNode->GetRawData ());
}

void
FieldIndexNodeManager::UpdateContainer ()
{

  std::auto_ptr <I_BTreeNode> apNode (NodeFactory (0));

  apNode->SetNext (m_FirstFreeNode);
  apNode->SetPrev (m_RootNode);

  m_Container->StoreData (0, GetRawNodeSize (), apNode->GetRawData ());
}

void
FieldIndexNodeManager::InitFromContainer ()
{
  std::auto_ptr <I_BTreeNode> apNode (NodeFactory (0));
  m_Container->RetrieveData (0, GetRawNodeSize (), apNode->GetRawData ());

  m_FirstFreeNode = apNode->GetNext ();
  m_RootNode      = apNode->GetPrev ();
}

I_BTreeNode *
FieldIndexNodeManager::NodeFactory (const NODE_INDEX nodeId)
{
  I_BTreeNode *result = NULL;
  switch (m_FieldType)
  {
  case T_BOOL:
    result = new BoolBTreeNode (*this, nodeId);
    break;
  case T_CHAR:
    result = new CharBTreeNode (*this, nodeId);
    break;
  case T_DATE:
    result = new DateBTreeNode (*this, nodeId);
    break;
  case T_DATETIME:
    result = new DateTimeBTreeNode (*this, nodeId);
    break;
  case T_HIRESTIME:
    result = new HiresTimeBTreeNode (*this, nodeId);
    break;
  case T_UINT8:
    result = new UInt8BTreeNode (*this, nodeId);
    break;
  case T_UINT16:
    result = new UInt16BTreeNode (*this, nodeId);
    break;
  case T_UINT32:
    result = new UInt32BTreeNode (*this, nodeId);
    break;
  case T_UINT64:
    result = new UInt64BTreeNode (*this, nodeId);
    break;
  case T_INT8:
    result = new Int8BTreeNode (*this, nodeId);
    break;
  case T_INT16:
    result = new Int16BTreeNode (*this, nodeId);
    break;
  case T_INT32:
    result = new Int32BTreeNode (*this, nodeId);
    break;
  case T_INT64:
    result = new Int64BTreeNode (*this, nodeId);
    break;
  case T_REAL:
    result = new RealBTreeNode (*this, nodeId);
    break;
  case T_RICHREAL:
    result = new RichRealBTreeNode (*this, nodeId);
    break;
  default:
    assert (false);
  }

  assert (result != NULL);
  return result;
}

