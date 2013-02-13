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

///////////////////////////////////////////////////////////////////////////////
I_DBSTable::I_DBSTable ()
{
}

I_DBSTable::~I_DBSTable ()
{
}

///////////////////////////////////////////////////////////////////////////////

PrototypeTable::PrototypeTable (DbsHandler& dbs)
  : m_Dbs (dbs),
    m_RowsCount (0),
    m_RootNode (NIL_NODE),
    m_FirstUnallocatedRoot (NIL_NODE),
    m_RowSize (0),
    m_DescriptorsSize (0),
    m_FieldsCount (0),
    m_FieldsDescriptors(NULL),
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
    m_Sync (),
    m_IndexSync ()
{
  //TODO: In future make sure the two prototypes are
  //      sharing the same memory for fields.
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

FIELD_INDEX
PrototypeTable::GetFieldsCount ()
{
  return m_FieldsCount;
}

FIELD_INDEX
PrototypeTable::GetFieldIndex (const D_CHAR* pFieldName)
{
  const FieldDescriptor* const pDesc = _RC (const FieldDescriptor*,
                                            m_FieldsDescriptors.get ());
  D_UINT64 iterator = m_FieldsCount * sizeof(FieldDescriptor);

  for (FIELD_INDEX index = 0; index < m_FieldsCount; ++index)
    {
      if (strcmp(_RC (const D_CHAR*,
                 m_FieldsDescriptors.get() + iterator),
                 pFieldName) == 0)
        {
          return index;
        }
      iterator += strlen(_RC(const D_CHAR*, pDesc) + iterator) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}


DBSFieldDescriptor
PrototypeTable::GetFieldDescriptor (const FIELD_INDEX field)
{
  const FieldDescriptor* const pDesc = _RC(const FieldDescriptor*,
                                           m_FieldsDescriptors.get ());

  if (field >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  DBSFieldDescriptor result;

  result.isArray      = (pDesc[field].m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0;
  result.m_FieldType  = _SC (
                           DBS_FIELD_TYPE,
                           pDesc[field].m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK
                            );
  result.m_pFieldName = _RC (const D_CHAR*, pDesc) + pDesc[field].m_NameOffset;

  return result;
}

ROW_INDEX
PrototypeTable::GetAllocatedRows ()
{
  return m_RowsCount;
}

ROW_INDEX
PrototypeTable::AddRow ()
{
  D_UINT64 lastRowPosition = m_RowsCount * m_RowSize;
  D_UINT   toWrite         = m_RowSize;
  D_UINT8  dummyValue[128];

  m_RowCache.FlushItem (m_RowsCount - 1);

  memset(dummyValue, 0xFF, sizeof(dummyValue));

  while (toWrite > 0)
    {
      D_UINT writeChunk = MIN (toWrite, sizeof (dummyValue));

      FixedFieldsContainer ().Write (lastRowPosition, writeChunk, dummyValue);

      toWrite         -= writeChunk;
      lastRowPosition += writeChunk;
    }

  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;
  BTree      removedRows (*this);

  removedRows.InsertKey (TableRmKey (m_RowsCount), dummyNode, dummyKey);

  m_RowCache.RefreshItem (m_RowsCount++);

  assert (m_RowsCount > 0);

  return m_RowsCount - 1;
}

ROW_INDEX
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
      BTreeNodeRAI       keyNode (RetrieveNode (node));
      TableRmNode* const pNode = _SC (TableRmNode *, &(*keyNode));

      assert (keyNode->IsLeaf() );
      assert (keyIndex < keyNode->GetKeysCount());

      const D_UINT64 *const pRows = _RC (const D_UINT64*,
                                         pNode->DataToRead ());
      result = pRows [keyIndex];
    }

  return result;
}

void
PrototypeTable::MarkRowForReuse (const ROW_INDEX row)
{
  FIELD_INDEX fieldsCount = m_FieldsCount;

  StoredItem cachedItem (m_RowCache.RetriveItem (row));
  D_UINT8* const pRawData = cachedItem.GetDataForUpdate();

    while (fieldsCount-- > 0)
      {
        const FieldDescriptor& field = GetFieldDescriptorInternal (
                                                                fieldsCount);

        D_UINT  byte_off = field.m_NullBitIndex / 8;
        D_UINT8 bit_off  = field.m_NullBitIndex % 8;

        pRawData[byte_off] |= (1 << bit_off);
      }

    NODE_INDEX   node;
    KEY_INDEX    keyIndex;
    TableRmKey   key (row);
    BTree        removedRows (*this);

    removedRows.InsertKey (key, node, keyIndex);
}

template <class T> static void
insert_row_field (PrototypeTable&  table,
                  BTree&            tree,
                  const ROW_INDEX   row,
                  const FIELD_INDEX field)
{
  T rowValue;
  table.GetEntry (row, field, rowValue);


  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;

  T_BTreeKey<T> keyValue (rowValue, row);
  tree.InsertKey (keyValue, dummyNode, dummyKey);
}

void
PrototypeTable::CreateFieldIndex (const FIELD_INDEX                 field,
                                  CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                                  CallBackIndexData* const          pCbData)
{
  if (PrototypeTable::IsFieldIndexed (field))
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_INDEXED));

  if ((cbFunc == NULL) && (pCbData != NULL))
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if ((desc.m_TypeDesc == T_TEXT) ||
      (desc.m_TypeDesc & PS_TABLE_ARRAY_MASK) != 0)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));

  const D_UINT nodeSizeKB  = 16; //16KB

  auto_ptr<I_DataContainer> apIndexContainer ( CreateIndexContainer (field));
  auto_ptr<FieldIndexNodeManager> apFieldMgr (
                            new FieldIndexNodeManager (apIndexContainer,
                                                       nodeSizeKB * 1024,
                                                       0x400000, //4MB
                                                       _SC (DBS_FIELD_TYPE,
                                                            desc.m_TypeDesc),
                                                       true)
                                             );

  BTreeNodeRAI rootNode (apFieldMgr->RetrieveNode (
                                     apFieldMgr->AllocateNode (NIL_NODE, 0))
                                                  );
  rootNode->SetNext (NIL_NODE);
  rootNode->SetPrev (NIL_NODE);
  rootNode->SetKeysCount (0);
  rootNode->SetLeaf (true);
  rootNode->InsertKey (rootNode->SentinelKey ());

  apFieldMgr->SetRootNodeId (rootNode->NodeId());
  BTree fieldTree (*apFieldMgr.get ());

  for (ROW_INDEX row = 0; row < m_RowsCount; ++row)
    {
      switch (desc.m_TypeDesc)
      {
      case T_BOOL:
        insert_row_field<DBSBool> (*this, fieldTree, row, field);
        break;
      case T_CHAR:
        insert_row_field<DBSChar> (*this, fieldTree, row, field);
        break;
      case T_DATE:
        insert_row_field<DBSDate> (*this, fieldTree, row, field);
        break;
      case T_DATETIME:
        insert_row_field<DBSDateTime> (*this, fieldTree, row, field);
        break;
      case T_HIRESTIME:
        insert_row_field<DBSHiresTime> (*this, fieldTree, row, field);
        break;
      case T_UINT8:
        insert_row_field<DBSUInt8> (*this, fieldTree, row, field);
        break;
      case T_UINT16:
        insert_row_field<DBSUInt16> (*this, fieldTree, row, field);
        break;
      case T_UINT32:
        insert_row_field<DBSUInt32> (*this, fieldTree, row, field);
        break;
      case T_UINT64:
        insert_row_field<DBSUInt64> (*this, fieldTree, row, field);
        break;
      case T_INT8:
        insert_row_field<DBSInt8> (*this, fieldTree, row, field);
        break;
      case T_INT16:
        insert_row_field<DBSInt16> (*this, fieldTree, row, field);
        break;
      case T_INT32:
        insert_row_field<DBSInt32> (*this, fieldTree, row, field);
        break;
      case T_INT64:
        insert_row_field<DBSInt64> (*this, fieldTree, row, field);
        break;
      case T_REAL:
        insert_row_field<DBSReal> (*this, fieldTree, row, field);
        break;
      case T_RICHREAL:
        insert_row_field<DBSRichReal> (*this, fieldTree, row, field);
        break;
      default:
        assert (false);
      }

      if (cbFunc != NULL)
        {
          if (pCbData != NULL)
            {
              pCbData->m_RowsCount = m_RowsCount;
              pCbData->m_RowIndex  = row;
            }
          cbFunc (pCbData);
        }
    }

  desc.m_IndexNodeSizeKB = nodeSizeKB;
  desc.m_IndexUnitsCount = 1;
  MakeHeaderPersistent ();

  assert (m_vIndexNodeMgrs[field] == NULL);
  m_vIndexNodeMgrs[field] = apFieldMgr.release ();
}

void
PrototypeTable::RemoveFieldIndex (const FIELD_INDEX field)
{
  if (PrototypeTable::IsFieldIndexed(field) == false)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_INDEXED));

  FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  assert (desc.m_IndexNodeSizeKB > 0);
  assert (desc.m_IndexUnitsCount > 0);

  desc.m_IndexNodeSizeKB = 0;
  desc.m_IndexUnitsCount = 0;
  MakeHeaderPersistent ();

  auto_ptr <FieldIndexNodeManager> apFieldMgr (m_vIndexNodeMgrs [field]);
  m_vIndexNodeMgrs[field] = NULL;

  apFieldMgr->MarkForRemoval ();
}

bool
PrototypeTable::IsFieldIndexed (const FIELD_INDEX field) const
{
  if (field >= m_FieldsCount)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  assert (m_vIndexNodeMgrs.size () == m_FieldsCount);

  return (m_vIndexNodeMgrs[field] != NULL);
}

D_UINT
PrototypeTable::GetRowSize() const
{
  return m_RowSize;
}

FieldDescriptor&
PrototypeTable::GetFieldDescriptorInternal(const FIELD_INDEX field) const
{
  FieldDescriptor* const pDesc = _RC(FieldDescriptor*,
                                     m_FieldsDescriptors.get ());

  if (field >= m_FieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  return pDesc[field];
}

D_UINT64
PrototypeTable::RawNodeSize () const
{
  return TableRmNode::RAW_NODE_SIZE;
}

NODE_INDEX
PrototypeTable::AllocateNode (const NODE_INDEX parent,
                              const KEY_INDEX  parentKey)
{
  NODE_INDEX nodeIndex = m_FirstUnallocatedRoot;

  if (nodeIndex != NIL_NODE)
    {
      BTreeNodeRAI freeNode (RetrieveNode (nodeIndex));

      m_FirstUnallocatedRoot = freeNode->GetNext ();
      MakeHeaderPersistent ();
    }
  else
    {
      assert (MainTableContainer ().Size () % RawNodeSize () == 0);

      nodeIndex = MainTableContainer ().Size () / RawNodeSize ();
    }

  if (parent != NIL_NODE)
    {
      BTreeNodeRAI parentNode (RetrieveNode (parent));
      parentNode->SetNodeOfKey (parentKey, nodeIndex);

      assert (parentNode->IsLeaf() == false);
    }

  return nodeIndex;
}

void
PrototypeTable::FreeNode (const NODE_INDEX nodeId)
{
  BTreeNodeRAI node (RetrieveNode (nodeId));

  node->MarkAsRemoved();
  node->SetNext (m_FirstUnallocatedRoot);

  m_FirstUnallocatedRoot = node->NodeId();
  MakeHeaderPersistent ();
}

NODE_INDEX
PrototypeTable::GetRootNodeId ()
{
  if (m_RootNode == NIL_NODE)
    {
      BTreeNodeRAI rootNode (RetrieveNode (AllocateNode (NIL_NODE, 0)));

      rootNode->SetNext (NIL_NODE);
      rootNode->SetPrev (NIL_NODE);
      rootNode->SetKeysCount (0);
      rootNode->SetLeaf (true);
      rootNode->InsertKey (rootNode->SentinelKey ());

      SetRootNodeId (rootNode->NodeId());
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
PrototypeTable::MaxCachedNodes ()
{
  return 128;
}

I_BTreeNode*
PrototypeTable::LoadNode (const NODE_INDEX node)
{
  std::auto_ptr<TableRmNode> apNode (new TableRmNode (*this, node));

  assert (MainTableContainer ().Size () % RawNodeSize () == 0);

  if (MainTableContainer ().Size () > node * RawNodeSize ())
    {
      MainTableContainer ().Read (apNode->NodeId () * RawNodeSize (),
                                  RawNodeSize (),
                                  apNode->RawData ());
    }
  else
    {
      //Reserve space for this node
      assert (MainTableContainer ().Size () == (node * RawNodeSize ()));
      MainTableContainer ().Write (MainTableContainer ().Size (),
                                   RawNodeSize (),
                                   apNode->RawData ());
    }

  apNode->MarkClean ();
  assert (apNode->NodeId () == node);

  return apNode.release ();
}


void
PrototypeTable::SaveNode (I_BTreeNode* const pNode)
{
  if (pNode->IsDirty () == false)
    return ;

  MainTableContainer ().Write (pNode->NodeId() * RawNodeSize (),
                                   RawNodeSize (),
                                   pNode->RawData ());
  pNode->MarkClean ();
}

void
PrototypeTable::StoreItems (const D_UINT8* pSrcBuffer,
                            D_UINT64       firstItem,
                            D_UINT         itemsCount)
{
  if (itemsCount + firstItem > m_RowsCount)
    itemsCount = m_RowsCount - firstItem;

  FixedFieldsContainer ().Write (firstItem * m_RowSize,
                                 itemsCount * m_RowSize,
                                 pSrcBuffer);
}

void
PrototypeTable::RetrieveItems (D_UINT8* pDestBuffer,
                               D_UINT64  firstItem,
                               D_UINT    itemsCount)
{
  if (itemsCount + firstItem > m_RowsCount)
    itemsCount = m_RowsCount - firstItem;

  FixedFieldsContainer ().Read (firstItem * m_RowSize,
                                itemsCount * m_RowSize,
                                pDestBuffer);
}

void
PrototypeTable::CheckRowToDelete (const ROW_INDEX row)
{
  bool isRowDeleted = true;

  StoredItem     cachedItem   = m_RowCache.RetriveItem (row);
  D_UINT8 *const pRawData     = cachedItem.GetDataForUpdate();

  for (FIELD_INDEX index = 0; index < m_FieldsCount; index += 8)
    {
      const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
      const D_UINT8          bitsSet   = ~0;

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
      TableRmKey   key (row);

      removedNodes.InsertKey (key, dumyNode, dummyKey);
    }
}

void
PrototypeTable::CheckRowToReuse (const ROW_INDEX row)
{
  bool wasRowDeleted = true;

  StoredItem     cachedItem = m_RowCache.RetriveItem (row);
  D_UINT8 *const pRawData   = cachedItem.GetDataForUpdate();

  for (FIELD_INDEX index = 0; index < m_FieldsCount; index += 8)
    {
      const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
      const D_UINT8          bitsSet   = ~0;

      if ( pRawData [fieldDesc.m_NullBitIndex / 8] != bitsSet)
        {
          wasRowDeleted = false;
          break;
        }
    }

  if (wasRowDeleted)
    {
      BTree        removedNodes (*this);
      TableRmKey   key (row);

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
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSChar&    value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSBool&    value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSDate&    value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX    row,
                          const FIELD_INDEX  field,
                          const DBSDateTime& value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX     row,
                          const FIELD_INDEX   field,
                          const DBSHiresTime& value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSInt8&    value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSInt16&   value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSInt32&   value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSInt64&   value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSReal&    value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX    row,
                          const FIELD_INDEX  field,
                          const DBSRichReal& value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSUInt8&   value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSUInt16&  value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSUInt32&  value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSUInt64&  value)
{
  StoreEntry (row, field, value);
}

void
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSText&    value)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row >= m_RowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if ((desc.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  D_UINT64                 newFirstEntry     = ~0;
  D_UINT64                 newFieldValueSize = 0;
  const D_UINT8            bitsSet           = ~0;
  bool                     fieldValueWasNull = false;

  if (value.IsNull() == false)
    {
      I_TextStrategy& text = value;
      if (text.IsRowValue ())
        {
          RowFieldText& value = text.GetRowValue();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = VariableFieldsStore ().AddRecord (
                                                      value.m_Storage,
                                                      value.m_FirstEntry,
                                                      0,
                                                      value.m_BytesSize
                                                                  );
        }
      else
        {
          TemporalText& value = text.GetTemporal();
          newFieldValueSize   = value.m_BytesSize;
          newFirstEntry       = VariableFieldsStore ().AddRecord (
                                                        value.m_Storage,
                                                        0,
                                                        newFieldValueSize
                                                                  );
        }
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem     cachedItem = m_RowCache.RetriveItem (row);
  D_UINT8* const pRawData   = cachedItem.GetDataForUpdate();

  D_UINT64* const fieldFirstEntry = _RC (D_UINT64*,
                                         pRawData + desc.m_StoreIndex + 0);
  D_UINT64* const fieldValueSize  = _RC (
                            D_UINT64*,
                            pRawData + desc.m_StoreIndex + sizeof (D_UINT64)
                                         );


  assert (desc.m_NullBitIndex > 0);

  const D_UINT  byte_off = desc.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = desc.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && value.IsNull ())
    return ;
  else if ( (fieldValueWasNull == false) && value.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (row);

      return;
    }
  else if (value.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (row);
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
PrototypeTable::SetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const DBSArray&   value)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row >= m_RowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if ( ((desc.m_TypeDesc & PS_TABLE_ARRAY_MASK) == 0) ||
          ((desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) !=
            _SC(D_UINT, value.ElementsType())))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  D_UINT64      newFirstEntry     = ~0;
  D_UINT64      newFieldValueSize = 0;
  const D_UINT8 bitsSet           = ~0;
  bool          fieldValueWasNull = false;

  if (value.IsNull() == false)
    {
      I_ArrayStrategy& array = value;
      if (array.IsRowValue())
        {
          RowFieldArray& value = array.GetRowValue();
          newFieldValueSize    = value.RawSize();
          newFirstEntry        = VariableFieldsStore ().AddRecord (
                                                  value.m_Storage,
                                                  value.m_FirstRecordEntry,
                                                  0,
                                                  newFieldValueSize
                                                                  );
        }
      else
        {
          TemporalArray& value = array.GetTemporal ();
          newFieldValueSize    = value.RawSize();

          const D_UINT64 elemsCount = value.Count();
          newFirstEntry             = VariableFieldsStore ().AddRecord (
                                              _RC (const D_UINT8*, &elemsCount),
                                              sizeof elemsCount
                                                                       );
          VariableFieldsStore ().UpdateRecord (newFirstEntry,
                                               sizeof elemsCount,
                                               value.m_Storage,
                                               0,
                                               newFieldValueSize);
          newFieldValueSize += sizeof elemsCount;
        }
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem cachedItem           = m_RowCache.RetriveItem (row);
  D_UINT8 *const pRawData         = cachedItem.GetDataForUpdate();

  D_UINT64 *const fieldFirstEntry = _RC (D_UINT64*,
                                         pRawData + desc.m_StoreIndex + 0);
  D_UINT64 *const fieldValueSize  = _RC (
                            D_UINT64*,
                            pRawData + desc.m_StoreIndex + sizeof (D_UINT64)
                                        );

  assert (desc.m_NullBitIndex > 0);

  const D_UINT  byte_off = desc.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = desc.m_NullBitIndex % 8;

  if ((pRawData [byte_off] & (1 << bit_off)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && value.IsNull ())
    return ;
  else if ( (fieldValueWasNull == false) && value.IsNull())
    {
      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (row);

      return ;
    }
  else if (value.IsNull() == false)
    {
      if (pRawData [byte_off] == bitsSet)
        {
          assert (fieldValueWasNull == true);
          CheckRowToReuse (row);
        }
      pRawData [byte_off] &= ~(1 << bit_off);
    }

  if (fieldValueWasNull == false)
    {
      //Postpone the removal of the actual record entries
      //to allow other threads gain access to 'm_Sync' faster.
      RowFieldArray   oldEntryRAII (VariableFieldsStore (),
                                    *fieldFirstEntry,
                                    value.ElementsType ());

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
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSChar&          outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSBool&          outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSDate&          outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSDateTime&      outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSHiresTime&     outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSInt8&          outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSInt16&         outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSInt32&         outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSInt64&         outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSReal&          outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSRichReal&      outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSUInt8&         outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSUInt16&        outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSUInt32&        outValue)
{
  RetrieveEntry (row, field, outValue);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSUInt64&        outValue)
{
  RetrieveEntry (row, field, outValue);
}

static I_TextStrategy*
allocate_row_field_text (VLVarsStore&   store,
                         const D_UINT64 firstRecordEntry,
                         const D_UINT64 valueSize)
{
  I_TextStrategy* const pTextStrategy = new RowFieldText (store,
                                                          firstRecordEntry,
                                                          valueSize);
  return pTextStrategy;
}

static RowFieldArray*
allocate_row_field_array (VLVarsStore&         store,
                          const D_UINT64       firstRecordEntry,
                          const DBS_FIELD_TYPE type)
{
  return new RowFieldArray (store, firstRecordEntry, type);
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSText&          outValue)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row >= m_RowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if ((desc.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
      ((desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) != _SC(D_UINT, T_TEXT)))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (row);
  const D_UINT8* const pRawData   = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (
                                      const D_UINT64*,
                                      pRawData + desc.m_StoreIndex + 0
                                         );
  const D_UINT64& fieldValueSize  = *_RC (
                              const D_UINT64*,
                              pRawData + desc.m_StoreIndex + sizeof (D_UINT64)
                                         );
  const D_UINT  byte_off = desc.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = desc.m_NullBitIndex % 8;

  outValue.~DBSText ();
  if (pRawData[byte_off] & (1 << bit_off))
    _placement_new (&outValue, DBSText ());
  else
    {
      _placement_new (
                       &outValue,
                       DBSText (
                           *allocate_row_field_text (
                                                     VariableFieldsStore (),
                                                     fieldFirstEntry,
                                                     fieldValueSize
                                                     )
                               )
                      );
    }
}

void
PrototypeTable::GetEntry (const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          DBSArray&         outValue)
{

  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row >= m_RowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if (((desc.m_TypeDesc & PS_TABLE_ARRAY_MASK) == 0)
      ||
          (((desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) !=
              _SC(D_UINT, outValue.ElementsType()))
          &&
          (outValue.ElementsType () != T_UNDETERMINED)))

    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (row);
  const D_UINT8 *const pRawData   = cachedItem.GetDataForRead();

  const D_UINT64& fieldFirstEntry = *_RC (
                                          const D_UINT64*,
                                          pRawData + desc.m_StoreIndex + 0
                                         );

  const D_UINT  byte_off = desc.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = desc.m_NullBitIndex % 8;

  outValue.~DBSArray ();
  if (pRawData[byte_off] & (1 << bit_off))
    {
      switch (desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK)
      {
      case T_BOOL:
        _placement_new (&outValue, DBSArray(_SC(DBSBool *, NULL)));
        break;
      case T_CHAR:
        _placement_new (&outValue, DBSArray(_SC(DBSChar *, NULL)));
        break;
      case T_DATE:
        _placement_new (&outValue, DBSArray(_SC(DBSDate *, NULL)));
        break;
      case T_DATETIME:
        _placement_new (&outValue, DBSArray(_SC(DBSDateTime *, NULL)));
        break;
      case T_HIRESTIME:
        _placement_new (&outValue, DBSArray(_SC(DBSHiresTime *, NULL)));
        break;
      case T_UINT8:
        _placement_new (&outValue, DBSArray(_SC(DBSUInt8 *, NULL)));
        break;
      case T_UINT16:
        _placement_new (&outValue, DBSArray(_SC(DBSUInt16 *, NULL)));
        break;
      case T_UINT32:
        _placement_new (&outValue, DBSArray(_SC(DBSUInt32 *, NULL)));
        break;
      case T_UINT64:
        _placement_new (&outValue, DBSArray(_SC(DBSUInt64 *, NULL)));
        break;
      case T_REAL:
        _placement_new (&outValue, DBSArray(_SC(DBSReal *, NULL)));
        break;
      case T_RICHREAL:
        _placement_new (&outValue, DBSArray(_SC(DBSRichReal *, NULL)));
        break;
      case T_INT8:
        _placement_new (&outValue, DBSArray(_SC(DBSInt8 *, NULL)));
        break;
      case T_INT16:
        _placement_new (&outValue, DBSArray(_SC(DBSInt16 *, NULL)));
        break;
      case T_INT32:
        _placement_new (&outValue, DBSArray(_SC(DBSInt32 *, NULL)));
        break;
      case T_INT64:
        _placement_new (&outValue, DBSArray(_SC(DBSInt64 *, NULL)));
        break;
      default:
        assert (0);
      }
    }
  else
    {
      I_ArrayStrategy& rowArray = *allocate_row_field_array (
                               VariableFieldsStore (),
                               fieldFirstEntry,
                              _SC (
                                   DBS_FIELD_TYPE,
                                   desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK
                                  )
                                                            );
      _placement_new ( &outValue, DBSArray (rowArray));
    }
}


DBSArray
PrototypeTable::GetMatchingRows (const DBSBool&    min,
                                 const DBSBool&    max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSChar&    min,
                                 const DBSChar&    max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSDate&    min,
                                 const DBSDate&    max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSDateTime& min,
                                 const DBSDateTime& max,
                                 const ROW_INDEX    fromRow,
                                 const ROW_INDEX    toRow,
                                 const ROW_INDEX    ignoreFirst,
                                 const ROW_INDEX    maxCount,
                                 const FIELD_INDEX  field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSHiresTime& min,
                                 const DBSHiresTime& max,
                                 const ROW_INDEX     fromRow,
                                 const ROW_INDEX     toRow,
                                 const ROW_INDEX     ignoreFirst,
                                 const ROW_INDEX     maxCount,
                                 const FIELD_INDEX   field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt8&   min,
                                 const DBSUInt8&   max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt16&  min,
                                 const DBSUInt16&  max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt32&  min,
                                 const DBSUInt32&  max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSUInt64&  min,
                                 const DBSUInt64&  max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt8&    min,
                                 const DBSInt8&    max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt16&   min,
                                 const DBSInt16&   max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt32&   min,
                                 const DBSInt32&   max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSInt64&   min,
                                 const DBSInt64&   max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSReal&    min,
                                 const DBSReal&    max,
                                 const ROW_INDEX   fromRow,
                                 const ROW_INDEX   toRow,
                                 const ROW_INDEX   ignoreFirst,
                                 const ROW_INDEX   maxCount,
                                 const FIELD_INDEX field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

DBSArray
PrototypeTable::GetMatchingRows (const DBSRichReal& min,
                                 const DBSRichReal& max,
                                 const ROW_INDEX    fromRow,
                                 const ROW_INDEX    toRow,
                                 const ROW_INDEX    ignoreFirst,
                                 const ROW_INDEX    maxCount,
                                 const FIELD_INDEX  field)
{
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      return MatchRowsWithIndex (min,
                                 max,
                                 fromRow,
                                 toRow,
                                 ignoreFirst,
                                 maxCount,
                                 field);
    }

  return MatchRows (min, max, fromRow, toRow, ignoreFirst, maxCount, field);
}

template <class T> void
PrototypeTable::StoreEntry (const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const T&          value)
{

  //Check if we are trying to write a different value
  T currentValue;
  RetrieveEntry (row, field, currentValue);

  if (currentValue == value)
    return; //Nothing to change

  const D_UINT8      bitsSet  = ~0;
  FieldDescriptor&   desc     = GetFieldDescriptorInternal (field);
  const D_UINT       byte_off = desc.m_NullBitIndex / 8;
  const D_UINT8      bit_off  = desc.m_NullBitIndex % 8;

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem     cachedItem = m_RowCache.RetriveItem (row);
  D_UINT8 *const pRawData   = cachedItem.GetDataForUpdate();

  assert (desc.m_NullBitIndex > 0);

  if (value.IsNull ())
    {
      assert ((pRawData [byte_off] & (1 << bit_off)) == 0);

      pRawData [byte_off] |= (1 << bit_off);

      if (pRawData [byte_off] == bitsSet)
        CheckRowToDelete (row);
    }
  else
    {
      if (pRawData [byte_off] == bitsSet)
        CheckRowToReuse (row);

      pRawData [byte_off] &= ~(1 << bit_off);

      PSValInterp::Store (pRawData + desc.m_StoreIndex, value);
    }

  syncHolder.Leave ();

  //Update the field index if it exists
  if (m_vIndexNodeMgrs[field] != NULL)
    {
      NODE_INDEX        dummyNode;
      KEY_INDEX         dummyKey;

      AquireIndexField (&desc);

      try
        {
          BTree fieldIndexTree (*m_vIndexNodeMgrs[field]);

          fieldIndexTree.RemoveKey (T_BTreeKey<T> (currentValue, row));
          fieldIndexTree.InsertKey (T_BTreeKey<T> (value, row),
                                    dummyNode,
                                    dummyKey);
        }
      catch (...)
        {
          ReleaseIndexField (&desc);
          throw ;
        }
      ReleaseIndexField (&desc);
    }
}

template <class T> void
PrototypeTable::RetrieveEntry (const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               T&                outValue)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row >= m_RowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if ((desc.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
        ((desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) !=
        _SC(D_UINT,  outValue.GetDBSType ())))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  T* const      pDest    = &outValue;
  const D_UINT  byte_off = desc.m_NullBitIndex / 8;
  const D_UINT8 bit_off  = desc.m_NullBitIndex % 8;

  WSynchronizerRAII syncHolder (m_Sync);

  StoredItem           cachedItem = m_RowCache.RetriveItem (row);
  const D_UINT8* const pRawData   = cachedItem.GetDataForRead ();

  if (pRawData[byte_off] & (1 << bit_off))
    {
      pDest->~T ();
      _placement_new (pDest, T ());
    }
  else
    PSValInterp::Retrieve (pRawData + desc.m_StoreIndex, pDest);
}


template <class T> DBSArray
PrototypeTable::MatchRowsWithIndex (const T&          min,
                                    const T&          max,
                                    const ROW_INDEX   fromRow,
                                    ROW_INDEX         toRow,
                                    ROW_INDEX         ignoreFirst,
                                    ROW_INDEX         maxCount,
                                    const FIELD_INDEX field)
{
  FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if ((desc.m_TypeDesc & PS_TABLE_ARRAY_MASK) ||
        ((desc.m_TypeDesc & PS_TABLE_FIELD_TYPE_MASK) !=
        _SC(D_UINT, min.GetDBSType ())))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  FieldIndexNodeManager* const pNodeMgr = m_vIndexNodeMgrs[field];
  DBSArray                     result (_SC (DBSUInt64*, NULL));
  NODE_INDEX                   node;
  KEY_INDEX                    key;
  const T_BTreeKey<T>          firstKey (min, fromRow);
  const T_BTreeKey<T>          lastKey (max, toRow);

  assert (pNodeMgr != NULL);

  AquireIndexField (&desc);

  try
    {
      BTree fieldIndexTree (*pNodeMgr);

      if ( (fieldIndexTree.FindBiggerOrEqual (firstKey, node, key) == false) ||
           (maxCount == 0))
        {
          goto force_return;
        }

      BTreeNodeRAI currentNode (pNodeMgr->RetrieveNode (node));

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
          I_BTreeFieldIndexNode* pNode    = _SC (
                                                 I_BTreeFieldIndexNode*,
                                                 &*currentNode
                                                );
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

          pNode->GetRows (key, toKey, result);

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
      ReleaseIndexField (&desc);
      throw;
    }

force_return:
  ReleaseIndexField (&desc);
  return result;
}


template <class T> DBSArray
PrototypeTable::MatchRows (const T&          min,
                           const T&          max,
                           const ROW_INDEX   fromRow,
                           ROW_INDEX         toRow,
                           ROW_INDEX         ignoreFirst,
                           ROW_INDEX         maxCount,
                           const FIELD_INDEX field)
{
  toRow = MIN (toRow, ((m_RowsCount > 0) ? m_RowsCount - 1 : 0));

  DBSArray result (_SC (DBSUInt64*, NULL));
  T        rowValue;

  for (ROW_INDEX row = fromRow; (row <= toRow) && (maxCount > 0); ++row)
    {
      GetEntry (field, row, rowValue);

      if ( (rowValue < min) || (max < rowValue))
        continue;

      if (ignoreFirst > 0)
        {
          --ignoreFirst;
          continue;
        }

      --maxCount;

      result.AddElement (DBSUInt64 (row));
    }

  return result;
}

///////////////////////////////////////////////////////////////////////////////

TableRmNode::TableRmNode (PrototypeTable& table, const NODE_INDEX nodeId)
  : I_BTreeNode (table, nodeId)
{
  assert ((sizeof (NodeHeader) % ( 2 * sizeof  (D_UINT64)) == 0));
  assert (RAW_NODE_SIZE == m_NodesMgr.RawNodeSize ());
}

TableRmNode::~TableRmNode ()
{
}

D_UINT
TableRmNode::KeysPerNode () const
{
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);

  D_UINT result = m_NodesMgr.RawNodeSize () - sizeof (NodeHeader);

  if (IsLeaf ())
    result /= sizeof (D_UINT64);
  else
    result /= sizeof (D_UINT64) + sizeof (NODE_INDEX);

  return result;
}

KEY_INDEX
TableRmNode::GetParentKeyIndex (const I_BTreeNode& parent) const
{
  assert (GetKeysCount() > 0);

  KEY_INDEX               result;
  const NODE_INDEX* const pKeys = _RC ( const NODE_INDEX*, DataToRead ());

  const TableRmKey key (pKeys[0]);

  parent.FindBiggerOrEqual (key, result);

  assert (parent.IsEqual (key, result));
  assert (NodeId () == parent.GetNodeOfKey (result));

  return result;
}

NODE_INDEX
TableRmNode::GetNodeOfKey (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT            firstKeyOff = KeysPerNode () * sizeof (D_UINT64);
  const NODE_INDEX* const cpFirstKey = _RC (
                                            const NODE_INDEX*,
                                            DataToRead() + firstKeyOff
                                           );

  return cpFirstKey [keyIndex];
}

void
TableRmNode::AdjustKeyNode (const I_BTreeNode& childNode, const KEY_INDEX keyIndex)
{
  assert (childNode.NodeId () == GetNodeOfKey (keyIndex));
  assert (IsLeaf() == false);

  const TableRmNode& child         = _SC (const TableRmNode&, childNode);
  NODE_INDEX* const pKeys          = _RC (NODE_INDEX*, DataToWrite ());
  const NODE_INDEX* const pSrcKeys = _RC (
                                          const NODE_INDEX*,
                                          child.DataToRead ()
                                         );

  pKeys [keyIndex] = pSrcKeys [0];
}

void
TableRmNode::SetNodeOfKey (const KEY_INDEX keyIndex, const NODE_INDEX childNode)
{

  assert (keyIndex < GetKeysCount ());
  assert (sizeof (NodeHeader) % sizeof (D_UINT64) == 0);
  assert (IsLeaf () == false);

  const D_UINT      keysOffset = KeysPerNode() * sizeof (D_UINT64);
  NODE_INDEX* const cpFirstKey = _RC (NODE_INDEX*, DataToWrite () + keysOffset);

  cpFirstKey[keyIndex] = childNode;
}

KEY_INDEX
TableRmNode::InsertKey (const I_BTreeKey& key)
{
  if (GetKeysCount () > 0)
    {
      KEY_INDEX keyIndex;
      KEY_INDEX lastKey = GetKeysCount () - 1;

      assert (lastKey < (KeysPerNode() - 1));

      if (FindBiggerOrEqual (key, keyIndex) == false)
        keyIndex = 0;
      else
        keyIndex ++;

      assert ((keyIndex == 0) || IsLess (key, keyIndex - 1));

      D_UINT64* const pRows = _RC (D_UINT64 *, DataToWrite ());

      make_array_room (_RC (D_UINT8*, pRows),
                       lastKey,
                       keyIndex,
                       sizeof (D_UINT64));
      SetKeysCount (GetKeysCount() + 1);
      pRows[keyIndex] = *( _SC(const TableRmKey *, &key));

      if (IsLeaf () == false)
        {
          NODE_INDEX* const pNodes = _RC (NODE_INDEX*, pRows + KeysPerNode());
          make_array_room (_RC (D_UINT8*, pNodes),
                           lastKey,
                           keyIndex,
                           sizeof (NODE_INDEX));
        }

      return keyIndex;
    }

  D_UINT64* const pRows = _RC (D_UINT64 *, DataToWrite ());
  pRows [0]             = *( _SC(const TableRmKey *, &key));

  SetKeysCount (1);

  return 0;
}

void
TableRmNode::RemoveKey (const KEY_INDEX keyIndex)
{
  D_UINT lastKey = GetKeysCount () - 1;

  assert (lastKey < (KeysPerNode() - 1));

  D_UINT64* const pRows = _RC (D_UINT64 *, DataToWrite ());
  remove_array_elemes (_RC (D_UINT8*, pRows),
                       lastKey,
                       keyIndex,
                       sizeof (D_UINT64));

  if (IsLeaf () == false)
    {
      NODE_INDEX* const pNodes = _RC (
                                      NODE_INDEX*,
                                      pRows + TableRmNode::KeysPerNode()
                                     );
      remove_array_elemes (_RC (D_UINT8*, pNodes),
                           lastKey,
                           keyIndex,
                           sizeof (NODE_INDEX));
    }

  SetKeysCount (GetKeysCount () - 1);
}

void
TableRmNode::Split ( const NODE_INDEX parentId)
{
  assert (NeedsSpliting ());

  const D_UINT64* const pRows    = _RC (const D_UINT64 *, DataToRead ());
  const KEY_INDEX       splitKey = GetKeysCount() / 2;

  BTreeNodeRAI     parentNode (m_NodesMgr.RetrieveNode (parentId));
  const TableRmKey key (pRows[splitKey]);
  const KEY_INDEX  insertionPos    = parentNode->InsertKey (key);
  const NODE_INDEX allocatedNodeId = m_NodesMgr.AllocateNode (
                                                              parentId,
                                                              insertionPos
                                                             );
  BTreeNodeRAI allocatedNode (m_NodesMgr.RetrieveNode (allocatedNodeId));

  allocatedNode->SetLeaf (IsLeaf ());
  allocatedNode->MarkAsUsed();

  D_UINT64* const pSplitRows = _RC (D_UINT64*, allocatedNode->DataToWrite ());

  for (D_UINT index = splitKey; index < GetKeysCount (); ++index)
    pSplitRows [index - splitKey] = pRows [index];

  if (IsLeaf () == false)
    {
      const NODE_INDEX* const pNodes = _RC (const NODE_INDEX*,
                                            pRows + KeysPerNode ());
      NODE_INDEX* const pSplitNodes = _RC (NODE_INDEX*,
                                           pSplitRows + KeysPerNode ());

      for (D_UINT index = splitKey; index < GetKeysCount (); ++index)
        pSplitNodes [index - splitKey] = pNodes [index];
    }

  allocatedNode->SetKeysCount (GetKeysCount() - splitKey);
  SetKeysCount (splitKey);

  allocatedNode->SetNext (NodeId());
  allocatedNode->SetPrev (GetPrev());
  SetPrev (allocatedNodeId);
  if (allocatedNode->GetPrev() != NIL_NODE)
    {
      BTreeNodeRAI prevNode (m_NodesMgr.RetrieveNode (allocatedNode->GetPrev()));
      prevNode->SetNext (allocatedNodeId);
    }
}

void
TableRmNode::Join (bool toRight)
{
  assert (NeedsJoining ());

  D_UINT64 *const   pRows  = _RC (D_UINT64 *, DataToWrite ());
  NODE_INDEX* const pNodes = _RC (
                                  NODE_INDEX*,
                                  pRows + TableRmNode::KeysPerNode ()
                                 );
  if (toRight)
    {
      assert (GetNext () != NIL_NODE);
      BTreeNodeRAI       nextNode (m_NodesMgr.RetrieveNode (GetNext ()));
      TableRmNode* const pNextNode = _SC (TableRmNode*, &(*nextNode));
      D_UINT64 *const    pDestRows = _RC (
                                          D_UINT64*,
                                          pNextNode->DataToWrite ()
                                         );

      for (D_UINT index = 0; index < GetKeysCount (); ++index)
        pDestRows [index + pNextNode->GetKeysCount ()] = pRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX* const pDestNodes = _RC (
                                      NODE_INDEX*,
                                      pDestRows + TableRmNode::KeysPerNode ()
                                             );

          for (D_UINT index = 0; index < GetKeysCount(); ++index)
            pDestNodes [index + pNextNode->GetKeysCount ()] = pNodes [index];
        }

      nextNode->SetKeysCount (nextNode->GetKeysCount () + GetKeysCount ());
      nextNode->SetPrev (GetPrev ());

      if (GetPrev () != NIL_NODE)
        {
          BTreeNodeRAI prevNode (m_NodesMgr.RetrieveNode (GetPrev ()));
          prevNode->SetNext (GetNext ());
        }
    }
  else
    {
      assert (GetPrev () != NIL_NODE);

      BTreeNodeRAI       prevNode (m_NodesMgr.RetrieveNode (GetPrev ()));
      TableRmNode* const pPrevNode = _SC (TableRmNode*, &(*prevNode));
      D_UINT64 *const    pSrcRows  = _RC (
                                          D_UINT64*,
                                          pPrevNode->DataToWrite ()
                                         );

      for (D_UINT index = 0; index < prevNode->GetKeysCount(); ++index)
        pRows [index + GetKeysCount ()] = pSrcRows [index];

      if (IsLeaf () == false)
        {
          NODE_INDEX* const pSrcNodes = _RC (
                                       NODE_INDEX*,
                                       pSrcRows + TableRmNode::KeysPerNode ()
                                            );

          for (D_UINT index = 0; index < prevNode->GetKeysCount (); ++index)
            pNodes [index + GetKeysCount ()] = pSrcNodes [index];
        }

      SetKeysCount (GetKeysCount () + prevNode->GetKeysCount());
      SetPrev (prevNode->GetPrev ());
      if (GetPrev () != NIL_NODE)
        {
          BTreeNodeRAI prevNode (m_NodesMgr.RetrieveNode (GetPrev ()));
          prevNode->SetNext (NodeId ());
        }
    }
}

bool
TableRmNode::IsLess (const I_BTreeKey& key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64*, DataToRead ());
  const TableRmKey    tKey (*_SC (const TableRmKey*, &key));

  return tKey < pRows [keyIndex];
}

bool
TableRmNode::IsEqual (const I_BTreeKey& key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64*, DataToRead ());
  const TableRmKey    tKey (*_SC (const TableRmKey*, &key));

  return tKey == pRows [keyIndex];
}

bool
TableRmNode::IsBigger (const I_BTreeKey& key, KEY_INDEX keyIndex) const
{
  const D_UINT64 *const pRows = _RC ( const D_UINT64*, DataToRead ());
  const TableRmKey    tKey (*_SC (const TableRmKey*, &key));

  return tKey > pRows [keyIndex];
}

const I_BTreeKey&
TableRmNode::SentinelKey () const
{
  static TableRmKey _key (~0);

  return _key;
}

