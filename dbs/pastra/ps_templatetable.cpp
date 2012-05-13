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

#include "ps_templatetable.h"

#include "ps_valintep.h"
#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"

using namespace pastra;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////

PrototypeTable::PrototypeTable (DbsHandler& dbs)
  : m_Dbs (dbs),
    m_RowsCount (0),
    m_RootNode (NIL_NODE),
    m_FirstUnallocatedRoot (NIL_NODE),
    m_RowSize (0),
    m_DescriptorsSize (0),
    m_FieldsCount (0),
    m_FieldsDescriptors(NULL),
    m_RowCache (*this),
    m_Sync (),
    m_IndexSync ()
{
}

PrototypeTable::PrototypeTable (const PrototypeTable& prototype)
  : m_Dbs (prototype.m_Dbs),
    m_RowsCount (0),
    m_RootNode (NIL_NODE),
    m_FirstUnallocatedRoot (NIL_NODE),
    m_RowSize (prototype.m_RowSize),
    m_DescriptorsSize (prototype.m_DescriptorsSize),
    m_FieldsCount (prototype.m_FieldsCount),
    m_FieldsDescriptors(),
    m_vIndexNodeMgrs (),
    m_RowCache (*this),
    m_Sync (),
    m_IndexSync ()
{
  //TODO: In future make the two prototypes to share the same memory for fields.
  m_FieldsDescriptors.reset (new D_UINT8 [m_DescriptorsSize]);
  memcpy (m_FieldsDescriptors.get (),
          prototype.m_FieldsDescriptors.get (),
          m_DescriptorsSize);
}

PrototypeTable::~PrototypeTable ()
{
}

void
PrototypeTable::Flush ()
{
  WSynchronizerRAII syncHolder (m_Sync);

  m_RowCache.Flush ();
  FlushNodes ();
}

D_UINT
PrototypeTable::GetFieldsCount ()
{
  return m_FieldsCount;
}

DBSFieldDescriptor
PrototypeTable::GetFieldDescriptor (D_UINT fieldIndex)
{
  const FieldDescriptor * const pDesc = _RC(const FieldDescriptor*, m_FieldsDescriptors.get ());

  if (fieldIndex >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  DBSFieldDescriptor result;

  result.isArray      = (pDesc[fieldIndex].m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0;
  result.m_FieldType  = _SC (DBS_FIELD_TYPE, pDesc[fieldIndex].m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK);
  result.m_pFieldName = _RC (const D_CHAR *, pDesc) + pDesc[fieldIndex].m_NameOffset;

  return result;
}

DBSFieldDescriptor
PrototypeTable::GetFieldDescriptor (const D_CHAR* const pFieldName)
{

  const FieldDescriptor* const pDesc = _RC(const FieldDescriptor*, m_FieldsDescriptors.get ());
  D_UINT iterator = m_FieldsCount * sizeof(FieldDescriptor);

  for (D_UINT index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp(_RC (const D_CHAR *, m_FieldsDescriptors.get() + iterator), pFieldName) == 0)
        {
          DBSFieldDescriptor result;

          result.m_pFieldName = _RC (const D_CHAR*, pDesc) + iterator;
          result.isArray      = (pDesc[index].m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0;
          result.m_FieldType  = _SC (DBS_FIELD_TYPE, pDesc[index].m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK);

          return result;
        }
      iterator += strlen(_RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}

D_UINT64
PrototypeTable::GetAllocatedRows ()
{
  return m_RowsCount;
}

D_UINT64
PrototypeTable::AddRow ()
{
  D_UINT64 lastRowPosition = m_RowsCount * m_RowSize;
  D_UINT   toWrite         = m_RowSize;
  D_UINT8  dummyValue[128];

  memset(dummyValue, 0xFF, sizeof(dummyValue));

  while (toWrite)
    {
      D_UINT writeChunk = MIN (toWrite, sizeof (dummyValue));

      FixedFieldsContainer ().StoreData (lastRowPosition, writeChunk, dummyValue);

      toWrite         -= writeChunk;
      lastRowPosition += writeChunk;
    }

  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;
  BTree      removedRows (*this);

  removedRows.InsertKey (TableRmKey (m_RowsCount), dummyNode, dummyKey);

  IncreaseRowCount ();

  assert (m_RowsCount > 0);

  return m_RowsCount - 1;
}

D_UINT64
PrototypeTable::AddReusedRow ()
{
  D_UINT64     result = 0;
  NODE_INDEX   node;
  KEY_INDEX    keyIndex;
  TableRmKey   key (0);
  BTree        removedRows (*this);

  if (removedRows.FindBiggerOrEqual(key, node, keyIndex) == false)
    result = PrototypeTable::AddRow ();
  else
    {
      BTreeNodeHandler keyNode (RetrieveNode (node));
      TableRmNode* const pNode = _SC (TableRmNode *, &(*keyNode));

      assert (keyNode->IsLeaf() );
      assert (keyIndex < keyNode->GetKeysCount());

      const D_UINT64 *const pRows = _RC (D_UINT64*,
                                         pNode->GetRawData () + sizeof (TableRmNode::NodeHeader));
      result = pRows [keyIndex];
    }

  assert (result > 0);

  return result;
}

void
PrototypeTable::MarkRowForReuse (D_UINT64 rowIndex)
{
  D_UINT fieldsCount = m_FieldsCount;

  StoredItem cachedItem (m_RowCache.RetriveItem (rowIndex));
  D_UINT8* const pRawData = cachedItem.GetDataForUpdate();

    while (fieldsCount-- > 0)
      {
        const FieldDescriptor&   field    = GetFieldDescriptorInternal (fieldsCount);
        D_UINT                   byte_off = field.m_NullBitIndex / 8;
        D_UINT8                  bit_off  = field.m_NullBitIndex % 8;

        pRawData[byte_off] |= (1 << bit_off);
      }

    NODE_INDEX   node;
    KEY_INDEX    keyIndex;
    TableRmKey   key (rowIndex);
    BTree        removedRows (*this);

    removedRows.InsertKey (key, node, keyIndex);
}

template <class T> static void
insert_row_field (PrototypeTable &table,
                  BTree &tree,
                  const D_UINT64 rowIndex,
                  const D_UINT fieldIndex)
{
  T rowValue;
  table.GetEntry (rowValue, rowIndex, fieldIndex);


  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;

  T_BTreeKey<T> keyValue (rowValue, rowIndex);
  tree.InsertKey (keyValue, dummyNode, dummyKey);
}

void
PrototypeTable::CreateFieldIndex (const D_UINT                      fieldIndex,
                                  CREATE_INDEX_CALLBACK_FUNC* const cb_func,
                                  CallBackIndexData* const          pCbData)
{
  if (PrototypeTable::IsFieldIndexed(fieldIndex))
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_INDEXED));

  if ((cb_func == NULL) && (pCbData != NULL))
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc == T_TEXT) ||
      (field.m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));

  const D_UINT nodeSizeKB  = 16; //16KB

  auto_ptr<I_DataContainer>       apIndexContainer (CreateIndexContainer (fieldIndex));
  auto_ptr<FieldIndexNodeManager> apFieldMgr (new FieldIndexNodeManager (apIndexContainer,
                                                                         nodeSizeKB * 1024,
                                                                         0x400000, //4MB
                                                                         _SC (DBS_FIELD_TYPE,
                                                                              field.m_TypeDesc),
                                                                         true));

  BTreeNodeHandler rootNode (apFieldMgr->RetrieveNode (apFieldMgr->AllocateNode (NIL_NODE, 0)));
  rootNode->SetNext (NIL_NODE);
  rootNode->SetPrev (NIL_NODE);
  rootNode->SetKeysCount (0);
  rootNode->SetLeaf (true);
  rootNode->InsertKey (rootNode->GetSentinelKey ());

  apFieldMgr->SetRootNodeId (rootNode->GetNodeId());
  BTree fieldTree (*apFieldMgr.get ());

  for (D_UINT64 rowIndex = 0; rowIndex < m_RowsCount; ++rowIndex)
    {
      switch (field.m_TypeDesc)
      {
      case T_BOOL:
        insert_row_field<DBSBool> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_CHAR:
        insert_row_field<DBSChar> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_DATE:
        insert_row_field<DBSDate> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_DATETIME:
        insert_row_field<DBSDateTime> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_HIRESTIME:
        insert_row_field<DBSHiresTime> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT8:
        insert_row_field<DBSUInt8> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT16:
        insert_row_field<DBSUInt16> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT32:
        insert_row_field<DBSUInt32> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_UINT64:
        insert_row_field<DBSUInt64> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT8:
        insert_row_field<DBSInt8> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT16:
        insert_row_field<DBSInt16> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT32:
        insert_row_field<DBSInt32> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_INT64:
        insert_row_field<DBSInt64> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_REAL:
        insert_row_field<DBSReal> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      case T_RICHREAL:
        insert_row_field<DBSRichReal> (*this, fieldTree, rowIndex, fieldIndex);
        break;
      default:
        assert (false);
      }

      if (cb_func != NULL)
        {
          if (pCbData != NULL)
            {
              pCbData->m_RowsCount = m_RowsCount;
              pCbData->m_RowIndex  = rowIndex;
            }
          cb_func (pCbData);
        }
    }

  field.m_IndexNodeSizeKB = nodeSizeKB;
  field.m_IndexUnitsCount = 1;
  MakeHeaderPersistent ();

  assert (m_vIndexNodeMgrs[fieldIndex] == NULL);
  m_vIndexNodeMgrs[fieldIndex] = apFieldMgr.release ();

}

void
PrototypeTable::RemoveFieldIndex (const D_UINT fieldIndex)
{
  if (PrototypeTable::IsFieldIndexed(fieldIndex) == false)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_INDEXED));

  FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  assert (field.m_IndexNodeSizeKB > 0);
  assert (field.m_IndexUnitsCount > 0);

  field.m_IndexNodeSizeKB = 0;
  field.m_IndexUnitsCount = 0;
  MakeHeaderPersistent ();

  auto_ptr <FieldIndexNodeManager> apFieldMgr (m_vIndexNodeMgrs [fieldIndex]);
  m_vIndexNodeMgrs[fieldIndex] = NULL;

  apFieldMgr->MarkForRemoval ();
}

bool
PrototypeTable::IsFieldIndexed (const D_UINT fieldIndex) const
{
  if (fieldIndex >= m_FieldsCount)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  assert (m_vIndexNodeMgrs.size () == m_FieldsCount);

  return (m_vIndexNodeMgrs[fieldIndex] != NULL);
}

D_UINT
PrototypeTable::GetRowSize() const
{
  return m_RowSize;
}

FieldDescriptor&
PrototypeTable::GetFieldDescriptorInternal(D_UINT fieldIndex) const
{
  FieldDescriptor* const pDesc = _RC(FieldDescriptor*, m_FieldsDescriptors.get ());

  if (fieldIndex >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  return pDesc[fieldIndex];
}

FieldDescriptor&
PrototypeTable::GetFieldDescriptorInternal(const D_CHAR* const pFieldName) const
{

  FieldDescriptor* const   pDesc    = _RC (FieldDescriptor*, m_FieldsDescriptors.get ());
  D_UINT                   iterator = m_FieldsCount * sizeof(FieldDescriptor);

  for (D_UINT index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp ( _RC(const D_CHAR *, m_FieldsDescriptors.get() + iterator), pFieldName) == 0)
        return pDesc[index];

      iterator += strlen( _RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}

D_UINT
PrototypeTable::GetRawNodeSize () const
{
  return TableRmNode::RAW_NODE_SIZE;
}

NODE_INDEX
PrototypeTable::AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey)
{
  NODE_INDEX nodeIndex = m_FirstUnallocatedRoot;

  if (nodeIndex != NIL_NODE)
    {
      BTreeNodeHandler freeNode (RetrieveNode (nodeIndex));

      m_FirstUnallocatedRoot = freeNode->GetNext ();
      MakeHeaderPersistent ();
    }
  else
    {
      assert (MainTableContainer ().GetContainerSize () % GetRawNodeSize () == 0);

      //TODO: Rename the GetContainerSize method to Size ()
      nodeIndex = MainTableContainer ().GetContainerSize () / GetRawNodeSize ();
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
PrototypeTable::FreeNode (const NODE_INDEX nodeId)
{
  BTreeNodeHandler node (RetrieveNode (nodeId));

  node->MarkAsRemoved();
  node->SetNext (m_FirstUnallocatedRoot);

  m_FirstUnallocatedRoot = node->GetNodeId();
  MakeHeaderPersistent ();
}

NODE_INDEX
PrototypeTable::GetRootNodeId ()
{
  if (m_RootNode == NIL_NODE)
    {
      BTreeNodeHandler rootNode (RetrieveNode (AllocateNode (NIL_NODE, 0)));
      rootNode->SetNext (NIL_NODE);
      rootNode->SetPrev (NIL_NODE);
      rootNode->SetKeysCount (0);
      rootNode->SetLeaf (true);
      rootNode->InsertKey (rootNode->GetSentinelKey ());

      SetRootNodeId (rootNode->GetNodeId());
    }

  return m_RootNode;
}

void
PrototypeTable::SetRootNodeId (const NODE_INDEX node)
{
  m_RootNode = node;
  MakeHeaderPersistent ();
}


D_UINT
PrototypeTable::GetMaxCachedNodes ()
{
  return 128;
}

I_BTreeNode*
PrototypeTable::GetNode (const NODE_INDEX node)
{
  std::auto_ptr <TableRmNode> apNode (new TableRmNode (*this, node));

  assert (MainTableContainer ().GetContainerSize () % GetRawNodeSize () == 0);

  if (MainTableContainer ().GetContainerSize () > node * GetRawNodeSize ())
    {
      MainTableContainer ().RetrieveData (apNode->GetNodeId () * GetRawNodeSize (),
                                          GetRawNodeSize (),
                                          apNode->GetRawData ());
    }
  else
    {
      //Reserve space for this node
      assert (MainTableContainer ().GetContainerSize () == (node * GetRawNodeSize ()));
      MainTableContainer ().StoreData (MainTableContainer ().GetContainerSize (),
                                       GetRawNodeSize (),
                                       apNode->GetRawData ());
    }

  return apNode.release ();
}


void
PrototypeTable::StoreNode (I_BTreeNode* const pNode)
{
  if (pNode->IsDirty() == false)
    return ;

  MainTableContainer ().StoreData (pNode->GetNodeId() * GetRawNodeSize (),
                                   GetRawNodeSize (),
                                   pNode->GetRawData ());

  return;
}

void
PrototypeTable::StoreItems (const D_UINT8* pSrcBuffer,
                            D_UINT64       firstItem,
                            D_UINT         itemsCount)
{
  if (itemsCount + firstItem > m_RowsCount)
    itemsCount = m_RowsCount - firstItem;

  FixedFieldsContainer ().StoreData( firstItem * m_RowSize, itemsCount * m_RowSize, pSrcBuffer);
}

void
PrototypeTable::RetrieveItems (D_UINT8* pDestBuffer,
                              D_UINT64  firstItem,
                              D_UINT    itemsCount)
{
  if (itemsCount + firstItem > m_RowsCount)
    itemsCount = m_RowsCount - firstItem;

  FixedFieldsContainer ().RetrieveData( firstItem * m_RowSize, itemsCount * m_RowSize, pDestBuffer);
}



D_UINT64
PrototypeTable::IncreaseRowCount ()
{
  m_RowCache.RefreshItem (m_RowsCount++);
  return m_RowsCount;
}

void
PrototypeTable::CheckRowToDelete (const D_UINT64 rowIndex)
{
  bool isRowDeleted = true;

  StoredItem cachedItem   = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData = cachedItem.GetDataForUpdate();

  for (D_UINT64 index = 0; index < m_FieldsCount; index += 8)
    {
      const FieldDescriptor&   fieldDesc = GetFieldDescriptorInternal (index);
      const D_UINT8            bitsSet   = ~0;

      if ( pRawData [fieldDesc.m_NullBitIndex / 8] != bitsSet)
        {
          isRowDeleted = false;
          break;
        }
    }

  if (isRowDeleted)
    {
      NODE_INDEX   dumyNode;
      KEY_INDEX    dummyKey;
      BTree        removedNodes (*this);
      TableRmKey    key (rowIndex);

      removedNodes.InsertKey (key, dumyNode, dummyKey);
    }
}

void
PrototypeTable::CheckRowToReuse (const D_UINT64 rowIndex)
{
  bool wasRowDeleted = true;

  StoredItem     cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData   = cachedItem.GetDataForUpdate();

  for (D_UINT64 index = 0; index < m_FieldsCount; index += 8)
    {
      const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
      const D_UINT8            bitsSet   = ~0;

      if ( pRawData [fieldDesc.m_NullBitIndex / 8] != bitsSet)
        {
          wasRowDeleted = false;
          break;
        }
    }

  if (wasRowDeleted)
    {
      BTree        removedNodes (*this);
      TableRmKey   key (rowIndex);

      removedNodes.RemoveKey (key);
    }
}

void
PrototypeTable::AquireIndexField (FieldDescriptor* const pFieldDesc)
{
  bool aquired = false;
  while ( ! aquired)
    {
      WSynchronizerRAII syncHolder (m_IndexSync);

      if (pFieldDesc->m_Aquired == 0)
        {
          pFieldDesc->m_Aquired = 1;
          aquired = true;
        }

      syncHolder.Leave ();

      if ( ! aquired)
        wh_yield ();
    }
}

void
PrototypeTable::ReleaseIndexField (FieldDescriptor* const pFieldDesc)
{
  WSynchronizerRAII syncHolder (m_IndexSync);

  assert (pFieldDesc->m_Aquired != 0);

  pFieldDesc->m_Aquired = 0;

}

void
PrototypeTable::SetEntry (const DBSChar& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSBool& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSDate& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSDateTime& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSHiresTime& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSInt8& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSInt16& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSInt32& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSInt64& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSReal& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSRichReal& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSUInt8& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSUInt16& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSUInt32& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSUInt64& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  StoreEntry (rSource, rowIndex, fieldIndex);
}

void
PrototypeTable::SetEntry (const DBSText& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  D_UINT64                 newFirstEntry     = ~0;
  D_UINT64                 newFieldValueSize = 0;
  const D_UINT8            bitsSet           = ~0;
  bool                     fieldValueWasNull = false;

  if (rSource.IsNull() == false)
    {
      I_TextStrategy& text = rSource;
      if (text.IsRowValue ())
        {
          RowFieldText& value = text.GetRowValue();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = VariableFieldsStore ().AddRecord (value.m_Storage,
                                                                  value.m_FirstEntry,
                                                                  0,
                                                                  value.m_BytesSize);
        }
      else
        {
          TemporalText& value = text.GetTemporal();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = VariableFieldsStore ().AddRecord (value.m_Storage,
                                                                  0,
                                                                  newFieldValueSize);
        }
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem     cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8* const pRawData   = cachedItem.GetDataForUpdate();

  D_UINT64* const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.m_StoreIndex + 0);
  D_UINT64* const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.m_StoreIndex + sizeof (D_UINT64));

  assert (field.m_NullBitIndex > 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && rSource.IsNull ())
    return ;
  else if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);

      return;
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (rowIndex);
        }
      pRawData [byte_off] &= ~(1 << bit_off);
    }

  if (fieldValueWasNull == false)
    {
      //Postpone the removal of the actual record entries
      //to allow other threads gain access to 'm_Sync' faster.
      RowFieldText   oldEntryRAII (VariableFieldsStore (),
                                   *fieldFirstEntry,
                                   *fieldValueSize);

      VariableFieldsStore ().DecrementRecordRef (*fieldFirstEntry);
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;

      syncHolder.Leave ();
    }
  else
    {
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;
    }
}

void
PrototypeTable::SetEntry (const DBSArray& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ( ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) == 0) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, rSource.GetElementsType())))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  D_UINT64      newFirstEntry     = ~0;
  D_UINT64      newFieldValueSize = 0;
  const D_UINT8 bitsSet           = ~0;
  bool          fieldValueWasNull = false;

  if (rSource.IsNull() == false)
    {
      I_ArrayStrategy &array = rSource;
      if (array.IsRowValue())
        {
          RowFieldArray& value = array.GetRowValue();
          newFieldValueSize    = value.GetRawDataSize();
          newFirstEntry        = VariableFieldsStore ().AddRecord (value.m_Storage,
                                                                   value.m_FirstRecordEntry,
                                                                   0,
                                                                   newFieldValueSize);
        }
      else
        {
          TemporalArray& value = array.GetTemporal ();
          newFieldValueSize    = value.GetRawDataSize();

          const D_UINT64 elemsCount = value.GetElementsCount();
          newFirstEntry             = VariableFieldsStore ().AddRecord (_RC(const D_UINT8*, &elemsCount),
                                                                        sizeof elemsCount);
          VariableFieldsStore ().UpdateRecord (newFirstEntry,
                                               sizeof elemsCount,
                                               value.m_Storage,
                                               0,
                                            newFieldValueSize);
          newFieldValueSize += sizeof elemsCount;
        }
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem cachedItem           = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData         = cachedItem.GetDataForUpdate();

  D_UINT64 *const fieldFirstEntry = _RC (D_UINT64*, pRawData + field.m_StoreIndex + 0);
  D_UINT64 *const fieldValueSize  = _RC (D_UINT64*,
                                         pRawData + field.m_StoreIndex + sizeof (D_UINT64));

  assert (field.m_NullBitIndex > 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && rSource.IsNull ())
    return ;
  else if ( (fieldValueWasNull == false) && rSource.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);

      return ;
    }
  else if (rSource.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (rowIndex);
        }
      pRawData [byte_off] &= ~(1 << bit_off);
    }

  if (fieldValueWasNull == false)
    {
      //Postpone the removal of the actual record entries
      //to allow other threads gain access to 'm_Sync' faster.
      RowFieldArray   oldEntryRAII (VariableFieldsStore (),
                                    *fieldFirstEntry,
                                    rSource.GetElementsType ());

      VariableFieldsStore ().DecrementRecordRef (*fieldFirstEntry);
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;

      syncHolder.Leave ();
    }
  else
    {
      *fieldFirstEntry = newFirstEntry;
      *fieldValueSize  = newFieldValueSize;
    }
}

void
PrototypeTable::GetEntry (DBSChar& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSBool& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSDate& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSDateTime& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSHiresTime& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSInt8& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSInt16& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSInt32& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSInt64& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSReal& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSRichReal& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSUInt8& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSUInt16& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSUInt32& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

void
PrototypeTable::GetEntry (DBSUInt64& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  RetrieveEntry (rDestination, rowIndex, fieldIndex);
}

static RowFieldText*
allocate_row_field_text (VariableLengthStore& store,
                         const D_UINT64       firstRecordEntry,
                         const D_UINT64       valueSize)
{
  return new RowFieldText (store, firstRecordEntry, valueSize);
}

static RowFieldArray*
allocate_row_field_array (VariableLengthStore& store,
                          const D_UINT64       firstRecordEntry,
                          const DBS_FIELD_TYPE type)
{
  return new RowFieldArray (store, firstRecordEntry, type);
}

void
PrototypeTable::GetEntry (DBSText& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8* const pRawData   = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*, pRawData + field.m_StoreIndex + 0);
  const D_UINT64& fieldValueSize  = *_RC (const D_UINT64*,
                                          pRawData + field.m_StoreIndex + sizeof (D_UINT64));

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  rDestination.~DBSText ();
  if (pRawData[byte_off] & (1 << bit_off))
    _placement_new (&rDestination, DBSText (NULL));
  else
    {
      _placement_new (&rDestination,
                      DBSText (*allocate_row_field_text (VariableFieldsStore (),
                                                         fieldFirstEntry,
                                                         fieldValueSize)));
    }
}

void
PrototypeTable::GetEntry (DBSArray& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{

  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ( ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) == 0) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, rDestination.GetElementsType())))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData   = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (const D_UINT64*,
                                          pRawData + field.m_StoreIndex + 0);

  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  rDestination.~DBSArray ();
  if (pRawData[byte_off] & (1 << bit_off))
    {
      switch (field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK)
      {
      case T_BOOL:
        _placement_new (&rDestination, DBSArray(_SC(DBSBool *, NULL)));
        break;
      case T_CHAR:
        _placement_new (&rDestination, DBSArray(_SC(DBSChar *, NULL)));
        break;
      case T_DATE:
        _placement_new (&rDestination, DBSArray(_SC(DBSDate *, NULL)));
        break;
      case T_DATETIME:
        _placement_new (&rDestination, DBSArray(_SC(DBSDateTime *, NULL)));
        break;
      case T_HIRESTIME:
        _placement_new (&rDestination, DBSArray(_SC(DBSHiresTime *, NULL)));
        break;
      case T_UINT8:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt8 *, NULL)));
        break;
      case T_UINT16:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt16 *, NULL)));
        break;
      case T_UINT32:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt32 *, NULL)));
        break;
      case T_UINT64:
        _placement_new (&rDestination, DBSArray(_SC(DBSUInt64 *, NULL)));
        break;
      case T_REAL:
        _placement_new (&rDestination, DBSArray(_SC(DBSReal *, NULL)));
        break;
      case T_RICHREAL:
        _placement_new (&rDestination, DBSArray(_SC(DBSRichReal *, NULL)));
        break;
      case T_INT8:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt8 *, NULL)));
        break;
      case T_INT16:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt16 *, NULL)));
        break;
      case T_INT32:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt32 *, NULL)));
        break;
      case T_INT64:
        _placement_new (&rDestination, DBSArray(_SC(DBSInt64 *, NULL)));
        break;
      default:
        assert (0);
      }
    }
  else
    {
      _placement_new (&rDestination,
                      DBSArray (*allocate_row_field_array (VariableFieldsStore (),
                                                           fieldFirstEntry,
                                                           _SC (DBS_FIELD_TYPE,
                                                                field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK))));
    }
}


DBSArray
PrototypeTable::GetMatchingRows (const DBSBool&  min,
                          const DBSBool&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSChar&  min,
                          const DBSChar&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSDate&  min,
                          const DBSDate&  max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSDateTime&  min,
                          const DBSDateTime&  max,
                          const D_UINT64      fromRow,
                          const D_UINT64      toRow,
                          const D_UINT64      ignoreFirst,
                          const D_UINT64      maxCount,
                          const D_UINT        fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSHiresTime& min,
                          const DBSHiresTime& max,
                          const D_UINT64      fromRow,
                          const D_UINT64      toRow,
                          const D_UINT64      ignoreFirst,
                          const D_UINT64      maxCount,
                          const D_UINT        fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt8& min,
                          const DBSUInt8& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt16& min,
                          const DBSUInt16& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt32& min,
                          const DBSUInt32& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt64& min,
                          const DBSUInt64& max,
                          const D_UINT64   fromRow,
                          const D_UINT64   toRow,
                          const D_UINT64   ignoreFirst,
                          const D_UINT64   maxCount,
                          const D_UINT     fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt8& min,
                          const DBSInt8& max,
                          const D_UINT64 fromRow,
                          const D_UINT64 toRow,
                          const D_UINT64 ignoreFirst,
                          const D_UINT64 maxCount,
                          const D_UINT   fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt16& min,
                          const DBSInt16& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt32& min,
                          const DBSInt32& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt64& min,
                          const DBSInt64& max,
                          const D_UINT64  fromRow,
                          const D_UINT64  toRow,
                          const D_UINT64  ignoreFirst,
                          const D_UINT64  maxCount,
                          const D_UINT    fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSReal& min,
                          const DBSReal& max,
                          const D_UINT64 fromRow,
                          const D_UINT64 toRow,
                          const D_UINT64 ignoreFirst,
                          const D_UINT64 maxCount,
                          const D_UINT   fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSRichReal& min,
                          const DBSRichReal& max,
                          const D_UINT64     fromRow,
                          const D_UINT64     toRow,
                          const D_UINT64     ignoreFirst,
                          const D_UINT64     maxCount,
                          const D_UINT       fieldIndex)
{
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    return MatchRowsWithIndex (fieldIndex, min, max, fromRow, toRow, ignoreFirst, maxCount);

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, fieldIndex);
}

template <class T> void
PrototypeTable::StoreEntry (const T& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{

  //Check if we are trying to write a different value
  T currentValue;
  RetrieveEntry (currentValue, rowIndex, fieldIndex);

  if (currentValue == rSource)
    return; //Nothing to change

  const D_UINT8      bitsSet  = ~0;
  FieldDescriptor& field    = GetFieldDescriptorInternal (fieldIndex);
  const D_UINT       byte_off = field.m_NullBitIndex / 8;
  const D_UINT8      bit_off  = field.m_NullBitIndex % 8;

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem     cachedItem = m_RowCache.RetriveItem (rowIndex);
  D_UINT8 *const pRawData   = cachedItem.GetDataForUpdate();

  assert (field.m_NullBitIndex > 0);

  if (rSource.IsNull ())
    {
      assert ((pRawData [byte_off] & (1 << bit_off)) == 0);

      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (rowIndex);
    }
  else
    {
      if (pRawData [byte_off] == bitsSet)
        CheckRowToReuse (rowIndex);

      pRawData [byte_off] &= ~(1 << bit_off);

      PSValInterp::Store (rSource, pRawData + field.m_StoreIndex);
    }

  syncHolder.Leave ();

  //Update the field index if it exists
  if (m_vIndexNodeMgrs[fieldIndex] != NULL)
    {
      NODE_INDEX        dummyNode;
      KEY_INDEX         dummyKey;

      AquireIndexField (&field);

      try
        {
          BTree fieldIndexTree (*m_vIndexNodeMgrs[fieldIndex]);

          fieldIndexTree.RemoveKey (T_BTreeKey<T> (currentValue, rowIndex));
          fieldIndexTree.InsertKey (T_BTreeKey<T> (rSource, rowIndex), dummyNode, dummyKey);
        }
      catch (...)
        {
          ReleaseIndexField (&field);
          throw ;
        }
      ReleaseIndexField (&field);
    }
}

template <class T> void
PrototypeTable::RetrieveEntry (T& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex)
{
  const FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, _SC(DBS_FIELD_TYPE, rDestination))))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  T *const      pDest    = &rDestination;
  const D_UINT  byte_off = field.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = field.m_NullBitIndex % 8;

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (rowIndex);
  const D_UINT8 *const pRawData   = cachedItem.GetDataForRead();

  if (pRawData[byte_off] & (1 << bit_off))
    {
      pDest->~T ();
      _placement_new (pDest, T ());
    }
  else
    PSValInterp::Retrieve (pDest, pRawData + field.m_StoreIndex);
}


template <class T> DBSArray
PrototypeTable::MatchRowsWithIndex (const D_UINT   fieldIndex,
                             const T&       min,
                             const T&       max,
                             const D_UINT64 fromRow,
                             D_UINT64       toRow,
                             D_UINT64       ignoreFirst,
                             D_UINT64       maxCount)
{
  FieldDescriptor& field = GetFieldDescriptorInternal (fieldIndex);

  if ((field.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((field.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, _SC(DBS_FIELD_TYPE, min))))
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));

  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  FieldIndexNodeManager *const pNodeMgr = m_vIndexNodeMgrs[fieldIndex];
  DBSArray                     result (_SC (DBSUInt64*, NULL));
  NODE_INDEX                   node;
  KEY_INDEX                    key;
  const T_BTreeKey<T>          firstKey (min, fromRow);
  const T_BTreeKey<T>          lastKey (max, MIN (toRow, m_RowsCount));

  assert (pNodeMgr != NULL);

  AquireIndexField (&field);

  try
    {
      BTree fieldIndexTree (*pNodeMgr);

      if ( (fieldIndexTree.FindBiggerOrEqual (firstKey, node, key) == false) ||
           (maxCount == 0))
        goto force_return;

      BTreeNodeHandler currentNode (pNodeMgr->RetrieveNode (node));

      while (ignoreFirst > 0)
        {
          if (ignoreFirst <= key)
            {
              key         -= ignoreFirst;
              ignoreFirst = 0;
            }
          else
            {
              NODE_INDEX nextNode = currentNode->GetNext ();

              if (nextNode == NIL_NODE)
                goto force_return;

              node        = nextNode;
              currentNode = pNodeMgr->RetrieveNode (node);
              assert (currentNode->GetKeysCount() > 0);

              ignoreFirst -= (key + 1);
              key          = currentNode->GetKeysCount () - 1;
            }
        }


      assert (key < currentNode->GetKeysCount ());

      while (true)
        {
          I_BTreeFieldIndexNode* pNode    = _SC (I_BTreeFieldIndexNode*, &*currentNode);
          KEY_INDEX              toKey    = ~0;;
          bool                   lastNode = false;

          if (pNode->FindBiggerOrEqual (lastKey, toKey) == false)
            toKey = 0;
          else
            {
              lastNode = true;
              toKey ++;
            }

          assert (key >= toKey);

          if (maxCount <= (key - toKey + 1))
            {
              toKey += maxCount;
              maxCount = 0;
            }
          else
            maxCount -= (key - toKey + 1);

          pNode->GetKeysRows (result, key, toKey);

          if (lastNode || (pNode->GetNext() == NIL_NODE) || (maxCount == 0))
            break;
          else
            {
              currentNode = pNodeMgr->RetrieveNode (pNode->GetNext ());

              assert (currentNode->GetKeysCount() > 0);
              key = currentNode->GetKeysCount () - 1;
            }
        }
    }
  catch (...)
    {
      ReleaseIndexField (&field);
      throw;
    }

force_return:
  ReleaseIndexField (&field);
  return result;
}


template <class T> DBSArray
PrototypeTable::MatchRows (const T&       min,
                    const T&       max,
                    const D_UINT64 fromRow,
                    D_UINT64       toRow,
                    D_UINT64       ignoreFirst,
                    D_UINT64       maxCount,
                    const D_UINT   fieldIndex)
{
  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  DBSArray result (_SC (DBSUInt64*, NULL));
  T        rowValue;

  for (D_UINT64 rowIndex = fromRow; (rowIndex <= toRow) && (maxCount > 0); ++rowIndex)
    {
      GetEntry (rowValue, fieldIndex, rowIndex);

      if ( (rowValue < min) || (max < rowValue))
        continue;

      if (ignoreFirst > 0)
        {
          --ignoreFirst;
          continue;
        }

      --maxCount;

      result.AddElement (DBSUInt64 (rowIndex));
    }

  return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

TableRmNode::TableRmNode (PrototypeTable &table, const NODE_INDEX nodeId) :
    I_BTreeNode (table),
    m_cpNodeData (new D_UINT8 [table.GetRawNodeSize ()])
{
  m_Header           = _RC ( NodeHeader*, m_cpNodeData.get ());
  m_Header->m_NodeId = nodeId;

  SetNullKeysCount (0);

  assert ((sizeof (NodeHeader) % ( 2 * sizeof  (D_UINT64)) == 0));
  assert (RAW_NODE_SIZE == m_NodesManager.GetRawNodeSize ());
}

TableRmNode::~TableRmNode ()
{
}

D_UINT
TableRmNode::GetKeysPerNode () const
{
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  D_UINT result = m_NodesManager.GetRawNodeSize () - sizeof (NodeHeader);

  if (IsLeaf ())
    result /= sizeof (D_UINT64);
  else
    result /= sizeof (D_UINT64) + sizeof (NODE_INDEX);

  return result;
}

KEY_INDEX
TableRmNode::GetFirstKey (const I_BTreeNode& parent) const
{
  assert (GetKeysCount() > 0);

  KEY_INDEX               result;
  const NODE_INDEX *const pKeys = _RC ( const NODE_INDEX*, GetRawData () + sizeof (NodeHeader));

  const TableRmKey key (pKeys[0]);

  parent.FindBiggerOrEqual (key, result);

  assert (parent.IsEqual (key, result));
  assert (GetNodeId () == parent.GetChildNode (result));

  return result;
}

NODE_INDEX
TableRmNode::GetChildNode (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT firstKeyOff = sizeof (NodeHeader) + TableRmNode::GetKeysPerNode() * sizeof (D_UINT64);
  const NODE_INDEX *const cpFirstKey = _RC (const NODE_INDEX *, GetRawData () + firstKeyOff);

  return cpFirstKey [keyIndex];
}

void
TableRmNode::ResetKeyNode (const I_BTreeNode &childNode, const KEY_INDEX keyIndex)
{
  assert (childNode.GetNodeId () == GetChildNode (keyIndex));

  const TableRmNode &child       = _SC (const TableRmNode&, childNode);
  NODE_INDEX *const pKeys          = _RC (NODE_INDEX*, GetRawData () + sizeof (NodeHeader));
  const NODE_INDEX *const pSrcKeys = _RC ( const NODE_INDEX*,
                                           child.GetRawData () + sizeof (NodeHeader));

  pKeys [keyIndex] = pSrcKeys [0];
}

void
TableRmNode::SetChildNode (const KEY_INDEX keyIndex, const NODE_INDEX childNode)
{

  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT firstKeyOff = sizeof (NodeHeader) + TableRmNode::GetKeysPerNode() * sizeof (D_UINT64);
  NODE_INDEX *const cpFirstKey = _RC (NODE_INDEX *, GetRawData () + firstKeyOff);

  cpFirstKey [keyIndex] = childNode;
}

KEY_INDEX
TableRmNode::InsertKey (const I_BTreeKey& key)
{
  if (GetKeysCount () > 0)
    {
      KEY_INDEX keyIndex;
      KEY_INDEX lastKey = GetKeysCount () - 1;

      assert (lastKey < (GetKeysPerNode() - 1));

      if (FindBiggerOrEqual (key, keyIndex) == false)
        keyIndex = 0;
      else
        keyIndex ++;

      assert ((keyIndex == 0) || IsLess (key, keyIndex - 1));

      D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));

      make_array_room (pRows, lastKey, keyIndex, 1);
      SetKeysCount (GetKeysCount() + 1);
      pRows[keyIndex] = *( _SC(const TableRmKey *, &key));

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pNodes = _RC (NODE_INDEX*, pRows + TableRmNode::GetKeysPerNode());
          make_array_room (pNodes, lastKey, keyIndex, 1);
        }

      return keyIndex;
    }

  D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  pRows [0]             = *( _SC(const TableRmKey *, &key));

  SetKeysCount (1);

  return 0;
}

void
TableRmNode::RemoveKey (const KEY_INDEX keyIndex)
{
  D_UINT lastKey = GetKeysCount () - 1;

  assert (lastKey < (GetKeysPerNode() - 1));

  D_UINT64* const pRows = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  remove_array_elemes (pRows, lastKey, keyIndex, 1);

  if (IsLeaf () == false)
    {
      NODE_INDEX *const pNodes = _RC (NODE_INDEX*, pRows + TableRmNode::GetKeysPerNode());
      remove_array_elemes (pNodes, lastKey, keyIndex, 1);
    }

  SetKeysCount (GetKeysCount () - 1);
}

void
TableRmNode::Split ( const NODE_INDEX parentId)
{
  assert (NeedsSpliting ());

  D_UINT64* const  pRows    = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const KEY_INDEX  splitKey = GetKeysCount() / 2;

  BTreeNodeHandler   parentNode (m_NodesManager.RetrieveNode (parentId));
  const TableRmKey key (pRows[splitKey]);
  const KEY_INDEX    insertionPos    = parentNode->InsertKey (key);
  const NODE_INDEX   allocatedNodeId = m_NodesManager.AllocateNode (parentId, insertionPos);
  BTreeNodeHandler   allocatedNode (m_NodesManager.RetrieveNode (allocatedNodeId));

  allocatedNode->SetLeaf (IsLeaf ());
  allocatedNode->MarkAsUsed();


  TableRmNode *const pRawAllocNode = _SC (TableRmNode*, &(*allocatedNode));
  D_UINT64* const pSplitRows         = _RC (D_UINT64*,
                                            pRawAllocNode->GetRawData () + sizeof (NodeHeader));

  for (D_UINT index = splitKey; index < GetKeysCount (); ++index)
    pSplitRows [index - splitKey] = pRows [index];

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pNodes      = _RC (NODE_INDEX*, pRows + GetKeysPerNode ());
      NODE_INDEX* const pSplitNodes = _RC (NODE_INDEX*, pSplitRows + GetKeysPerNode ());

      for (D_UINT index = splitKey; index < GetKeysCount (); ++index)
        pSplitNodes [index - splitKey] = pNodes [index];
    }

  allocatedNode->SetKeysCount (GetKeysCount() - splitKey);
  SetKeysCount (splitKey);

  allocatedNode->SetNext (GetNodeId());
  allocatedNode->SetPrev (GetPrev());
  SetPrev (allocatedNodeId);
  if (allocatedNode->GetPrev() != NIL_NODE)
    {
      BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (allocatedNode->GetPrev()));
      prevNode->SetNext (allocatedNodeId);
    }
}

void
TableRmNode::Join (bool toRight)
{
  assert (NeedsJoining ());

  D_UINT64 *const   pRows  = _RC (D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  NODE_INDEX *const pNodes = _RC (NODE_INDEX *, pRows + TableRmNode::GetKeysPerNode ());

  if (toRight)
    {
      assert (GetNext () != NIL_NODE);
      BTreeNodeHandler     nextNode (m_NodesManager.RetrieveNode (GetNext ()));
      TableRmNode *const pNextNode = _SC (TableRmNode*, &(*nextNode));
      D_UINT64 *const      pDestRows = _RC (D_UINT64 *, pNextNode->GetRawData () + sizeof (NodeHeader));

      for (D_UINT index = 0; index < GetKeysCount (); ++index)
        pDestRows [index + pNextNode->GetKeysCount ()] = pRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pDestNodes = _RC (NODE_INDEX*,
                                              pDestRows + TableRmNode::GetKeysPerNode ());

          for (D_UINT index = 0; index < GetKeysCount(); ++index)
            pDestNodes [index + pNextNode->GetKeysCount ()] = pNodes [index];
        }

      nextNode->SetKeysCount (nextNode->GetKeysCount () + GetKeysCount ());
      nextNode->SetPrev (GetPrev ());

      if (GetPrev () != NIL_NODE)
        {
          BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
          prevNode->SetNext (GetNext ());
        }
    }
  else
    {
      assert (GetPrev () != NIL_NODE);

      BTreeNodeHandler     prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
      TableRmNode *const pPrevNode = _SC (TableRmNode*, &(*prevNode));
      D_UINT64 *const      pSrcRows  = _RC (D_UINT64 *, pPrevNode->GetRawData () + sizeof (NodeHeader));

      for (D_UINT index = 0; index < prevNode->GetKeysCount(); ++index)
        pRows [index + GetKeysCount ()] = pSrcRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX *const pSrcNodes = _RC (NODE_INDEX*,
                                             pSrcRows + TableRmNode::GetKeysPerNode ());

          for (D_UINT index = 0; index < prevNode->GetKeysCount (); ++index)
            pNodes [index + GetKeysCount ()] = pSrcNodes [index];
        }

      SetKeysCount (GetKeysCount () + prevNode->GetKeysCount());
      SetPrev (prevNode->GetPrev ());
      if (GetPrev () != NIL_NODE)
        {
          BTreeNodeHandler prevNode (m_NodesManager.RetrieveNode (GetPrev ()));
          prevNode->SetNext (GetNodeId ());
        }
    }
}

bool
TableRmNode::IsLess (const I_BTreeKey& key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const TableRmKey    tKey ( * _SC( const  TableRmKey*, &key));

  return tKey < pRows [keyIndex];
}

bool
TableRmNode::IsEqual (const I_BTreeKey& key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const TableRmKey    tKey ( * _SC( const  TableRmKey*, &key));

  return tKey == pRows [keyIndex];
}

bool
TableRmNode::IsBigger (const I_BTreeKey &key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64 *, GetRawData () + sizeof (NodeHeader));
  const TableRmKey    tKey ( * _SC( const  TableRmKey*, &key));

  return tKey > pRows [keyIndex];
}

const I_BTreeKey&
TableRmNode::GetSentinelKey () const
{
  static TableRmKey _key (~0);

  return _key;
}





