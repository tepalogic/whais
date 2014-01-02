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


#include "utils/le_converter.h"

#include "ps_templatetable.h"

#include "ps_serializer.h"
#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"

using namespace std;

namespace whisper {



ITable::ITable ()
{
}


ITable::~ITable ()
{
}



namespace pastra {



PrototypeTable::PrototypeTable (DbsHandler& dbs)
  : mDbs (dbs),
    mRowsCount (0),
    mRootNode (NIL_NODE),
    mUnallocatedHead (NIL_NODE),
    mRowSize (0),
    mDescriptorsSize (0),
    mFieldsCount (0),
    mFieldsDescriptors(NULL),
    mSync (),
    mIndexSync ()
{
}


PrototypeTable::PrototypeTable (const PrototypeTable& prototype)
  : mDbs (prototype.mDbs),
    mRowsCount (0),
    mRootNode (NIL_NODE),
    mUnallocatedHead (NIL_NODE),
    mRowSize (prototype.mRowSize),
    mDescriptorsSize (prototype.mDescriptorsSize),
    mFieldsCount (prototype.mFieldsCount),
    mFieldsDescriptors (),
    mvIndexNodeMgrs (),
    mSync (),
    mIndexSync ()
{
  //TODO: Should be possible for the two prototypes to share the same memory
  //      for fields descriptors.
  mFieldsDescriptors.reset (new uint8_t [mDescriptorsSize]);

  memcpy (mFieldsDescriptors.get (),
          prototype.mFieldsDescriptors.get (),
          mDescriptorsSize);
}


void
PrototypeTable::Flush ()
{
  LockRAII syncHolder (mSync);

  mRowCache.Flush ();
  FlushNodes ();
}


FIELD_INDEX
PrototypeTable::FieldsCount ()
{
  return mFieldsCount;
}


FIELD_INDEX
PrototypeTable::RetrieveField (const char* name)
{
  const char* fieldName = _RC (const char*, mFieldsDescriptors.get ());

  //Initial iterator points at the at the first field's name.
  fieldName += mFieldsCount * sizeof(FieldDescriptor);

  for (FIELD_INDEX index = 0; index < mFieldsCount; ++index)
    {
      if (strcmp (fieldName, name) == 0)
        return index;

      fieldName += strlen (fieldName) + 1;
    }

  throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));
}


DBSFieldDescriptor
PrototypeTable::DescribeField (const FIELD_INDEX field)
{
  if (field >= mFieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  const FieldDescriptor* const desc = _RC(const FieldDescriptor*,
                                          mFieldsDescriptors.get ());

  DBSFieldDescriptor result;

  result.isArray = (desc[field].Type () & PS_TABLE_ARRAY_MASK) != 0;
  result.type    = _SC (DBS_FIELD_TYPE,
                        desc[field].Type () & PS_TABLE_FIELD_TYPE_MASK);
  result.name    = _RC (const char*, desc) + desc[field].NameOffset ();

  return result;
}


ROW_INDEX
PrototypeTable::AllocatedRows ()
{
  return mRowsCount;
}


ROW_INDEX
PrototypeTable::AddRow ()
{
  uint64_t lastRowPosition = mRowsCount * mRowSize;
  uint_t   toWrite         = mRowSize;

  uint8_t  dummyValue[128];

  mRowCache.FlushItem (mRowsCount - 1);

  memset(dummyValue, 0xFF, sizeof(dummyValue));

  while (toWrite > 0)
    {
      const uint_t writeChunk = MIN (toWrite, sizeof (dummyValue));

      RowsContainer ().Write (lastRowPosition, writeChunk, dummyValue);

      toWrite -= writeChunk, lastRowPosition += writeChunk;
    }

  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;
  BTree      removedRows (*this);

  removedRows.InsertKey (TableRmKey (mRowsCount), &dummyNode, &dummyKey);

  // The rows count has to be update before the procedure code is executed.
  // Otherway the new content will not be refreshed.
  mRowCache.RefreshItem (mRowsCount++);

  return mRowsCount - 1;
}


ROW_INDEX
PrototypeTable::AddReusedRow ()
{
  uint64_t     result = 0;
  NODE_INDEX   node;
  KEY_INDEX    keyIndex;
  TableRmKey   key (0);
  BTree        removedRows (*this);

  if (removedRows.FindBiggerOrEqual(key, &node, &keyIndex) == false)
    result = PrototypeTable::AddRow ();

  else
    {
      BTreeNodeRAII keyNode (RetrieveNode (node));

      TableRmNode* const  node = _SC (TableRmNode *, &(*keyNode));

      assert (keyNode->IsLeaf() );
      assert (keyIndex < keyNode->KeysCount());

      const ROW_INDEX* const rows = _RC (const ROW_INDEX*,
                                         node->DataForRead ());
      result = Serializer::LoadRow (rows + keyIndex);
    }
  return result;
}


void
PrototypeTable::MarkRowForReuse (const ROW_INDEX row)
{
  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;

  FIELD_INDEX fieldsCount = mFieldsCount;

  StoredItem cachedItem (mRowCache.RetriveItem (row));
  uint8_t* const rowData = cachedItem.GetDataForUpdate();

    while (fieldsCount-- > 0)
      {
        const FieldDescriptor& field = GetFieldDescriptorInternal (fieldsCount);

        const uint8_t bitOff  = field.NullBitIndex () % 8;
        const uint_t  byteOff = field.NullBitIndex () / 8;

        rowData[byteOff] |= (1 << bitOff);
      }

    TableRmKey key (row);
    BTree      removedRows (*this);

    removedRows.InsertKey (key, &dummyNode, &dummyKey);
}


template<class T> static void
insert_row_field (PrototypeTable&         table,
                  BTree&                  tree,
                  const ROW_INDEX         row,
                  const FIELD_INDEX       field)
{
  NODE_INDEX dummyNode;
  KEY_INDEX  dummyKey;

  T rowValue;
  table.Get (row, field, rowValue);

  T_BTreeKey<T> keyValue (rowValue, row);

  tree.InsertKey (keyValue, &dummyNode, &dummyKey);
}


void
PrototypeTable::CreateIndex (const FIELD_INDEX                 field,
                             CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                             CreateIndexCallbackContext* const cbContext)
{
  if (PrototypeTable::IsIndexed (field))
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_INDEXED));

  if ((cbFunc == NULL) && (cbContext != NULL))
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if ((desc.Type () == T_TEXT)
      || (desc.Type () & PS_TABLE_ARRAY_MASK) != 0)
    {
      throw DBSException (NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
    }

  const uint_t nodeSizeKB  = 16; //16KB

  auto_ptr<IDataContainer> indexContainer (CreateIndexContainer (field));
  auto_ptr<FieldIndexNodeManager> nodeMgr (
                            new FieldIndexNodeManager (indexContainer,
                                                       nodeSizeKB * 1024,
                                                       0x400000, //4MB
                                                       _SC (DBS_FIELD_TYPE,
                                                            desc.Type ()),
                                                       true)
                                          );

  BTreeNodeRAII rootNode (nodeMgr->RetrieveNode (
                                     nodeMgr->AllocateNode (NIL_NODE, 0))
                                                );
  rootNode->Next (NIL_NODE);
  rootNode->Prev (NIL_NODE);
  rootNode->KeysCount (0);
  rootNode->Leaf (true);
  rootNode->InsertKey (rootNode->SentinelKey ());

  nodeMgr->RootNodeId (rootNode->NodeId());
  BTree fieldTree (*nodeMgr.get ());

  for (ROW_INDEX row = 0; row < mRowsCount; ++row)
    {
      switch (desc.Type ())
      {
      case T_BOOL:
        insert_row_field<DBool> (*this, fieldTree, row, field);
        break;

      case T_CHAR:
        insert_row_field<DChar> (*this, fieldTree, row, field);
        break;

      case T_DATE:
        insert_row_field<DDate> (*this, fieldTree, row, field);
        break;

      case T_DATETIME:
        insert_row_field<DDateTime> (*this, fieldTree, row, field);
        break;

      case T_HIRESTIME:
        insert_row_field<DHiresTime> (*this, fieldTree, row, field);
        break;

      case T_UINT8:
        insert_row_field<DUInt8> (*this, fieldTree, row, field);
        break;

      case T_UINT16:
        insert_row_field<DUInt16> (*this, fieldTree, row, field);
        break;

      case T_UINT32:
        insert_row_field<DUInt32> (*this, fieldTree, row, field);
        break;

      case T_UINT64:
        insert_row_field<DUInt64> (*this, fieldTree, row, field);
        break;

      case T_INT8:
        insert_row_field<DInt8> (*this, fieldTree, row, field);
        break;

      case T_INT16:
        insert_row_field<DInt16> (*this, fieldTree, row, field);
        break;

      case T_INT32:
        insert_row_field<DInt32> (*this, fieldTree, row, field);
        break;

      case T_INT64:
        insert_row_field<DInt64> (*this, fieldTree, row, field);
        break;

      case T_REAL:
        insert_row_field<DReal> (*this, fieldTree, row, field);
        break;

      case T_RICHREAL:
        insert_row_field<DRichReal> (*this, fieldTree, row, field);
        break;

      default:
        assert (false);
      }

      if (cbFunc != NULL)
        {
          if (cbContext != NULL)
            {
              cbContext->mRowsCount = mRowsCount;
              cbContext->mRowIndex  = row;
            }
          cbFunc (cbContext);
        }
    }

  desc.IndexNodeSizeKB (nodeSizeKB);
  desc.IndexUnitsCount (1);

  MakeHeaderPersistent ();

  assert (mvIndexNodeMgrs[field] == NULL);

  mvIndexNodeMgrs[field] = nodeMgr.release ();
}


void
PrototypeTable::RemoveIndex (const FIELD_INDEX field)
{
  if (PrototypeTable::IsIndexed (field) == false)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_INDEXED));

  FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  assert (desc.IndexNodeSizeKB () > 0);
  assert (desc.IndexUnitsCount () > 0);

  desc.IndexNodeSizeKB (0);
  desc.IndexUnitsCount (0);

  MakeHeaderPersistent ();

  auto_ptr<FieldIndexNodeManager> fieldMgr (mvIndexNodeMgrs[field]);
  mvIndexNodeMgrs[field] = NULL;

  fieldMgr->MarkForRemoval ();
}


bool
PrototypeTable::IsIndexed (const FIELD_INDEX field) const
{
  if (field >= mFieldsCount)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  assert (mvIndexNodeMgrs.size () == mFieldsCount);

  return (mvIndexNodeMgrs[field] != NULL);
}


uint_t
PrototypeTable::RowSize () const
{
  return mRowSize;
}


FieldDescriptor&
PrototypeTable::GetFieldDescriptorInternal(const FIELD_INDEX field) const
{
  FieldDescriptor* const desc = _RC(FieldDescriptor*,
                                    mFieldsDescriptors.get ());

  if (field >= mFieldsCount)
    throw DBSException(NULL, _EXTRA (DBSException::FIELD_NOT_FOUND));

  return desc[field];
}


uint64_t
PrototypeTable::NodeRawSize () const
{
  return TableRmNode::RAW_NODE_SIZE;
}


NODE_INDEX
PrototypeTable::AllocateNode (const NODE_INDEX parent,
                              const KEY_INDEX  parentKey)
{
  NODE_INDEX nodeIndex = mUnallocatedHead;

  if (nodeIndex != NIL_NODE)
    {
      BTreeNodeRAII freeNode (RetrieveNode (nodeIndex));

      mUnallocatedHead = freeNode->Next ();
      MakeHeaderPersistent ();
    }
  else
    {
      assert (TableContainer ().Size () % NodeRawSize () == 0);

      nodeIndex = TableContainer ().Size () / NodeRawSize ();
    }

  if (parent != NIL_NODE)
    {
      BTreeNodeRAII parentNode (RetrieveNode (parent));
      parentNode->SetNodeOfKey (parentKey, nodeIndex);

      assert (parentNode->IsLeaf() == false);
    }

  return nodeIndex;
}


void
PrototypeTable::FreeNode (const NODE_INDEX nodeId)
{
  BTreeNodeRAII node (RetrieveNode (nodeId));

  node->MarkAsRemoved();
  node->Next (mUnallocatedHead);

  mUnallocatedHead = node->NodeId();

  MakeHeaderPersistent ();
}


NODE_INDEX
PrototypeTable::RootNodeId ()
{
  if (mRootNode == NIL_NODE)
    {
      BTreeNodeRAII rootNode (RetrieveNode (AllocateNode (NIL_NODE, 0)));

      rootNode->Next (NIL_NODE);
      rootNode->Prev (NIL_NODE);
      rootNode->KeysCount (0);
      rootNode->Leaf (true);
      rootNode->InsertKey (rootNode->SentinelKey ());

      RootNodeId (rootNode->NodeId());
    }

  return mRootNode;
}


void
PrototypeTable::RootNodeId (const NODE_INDEX node)
{
  mRootNode = node;

  MakeHeaderPersistent ();
}


uint_t
PrototypeTable::MaxCachedNodes ()
{
  return 128;
}


IBTreeNode*
PrototypeTable::LoadNode (const NODE_INDEX nodeId)
{
  auto_ptr<TableRmNode> node (new TableRmNode (*this, nodeId));

  assert (TableContainer ().Size () % NodeRawSize () == 0);

  if (TableContainer ().Size () > nodeId * NodeRawSize ())
    {
      TableContainer ().Read (node->NodeId () * NodeRawSize (),
                              NodeRawSize (),
                              node->RawData ());
    }
  else
    {
      //Reserve space for this node
      assert (TableContainer ().Size () == (nodeId * NodeRawSize ()));

      TableContainer ().Write (TableContainer ().Size (),
                               NodeRawSize (),
                               node->RawData ());
    }

  node->MarkClean ();
  assert (node->NodeId () == nodeId);

  return node.release ();
}


void
PrototypeTable::SaveNode (IBTreeNode* const node)
{
  if (node->IsDirty () == false)
    return ;

  TableContainer ().Write (node->NodeId() * NodeRawSize (),
                           NodeRawSize (),
                           node->RawData ());
  node->MarkClean ();
}


void
PrototypeTable::StoreItems (uint64_t       firstItem,
                            uint_t         itemsCount,
                            const uint8_t* from)
{
  if (itemsCount + firstItem > mRowsCount)
    itemsCount = mRowsCount - firstItem;

  RowsContainer ().Write (firstItem * mRowSize, itemsCount * mRowSize, from);
}


void
PrototypeTable::RetrieveItems (uint64_t  firstItem,
                               uint_t    itemsCount,
                               uint8_t*  to)
{
  if (itemsCount + firstItem > mRowsCount)
    itemsCount = mRowsCount - firstItem;

  RowsContainer ().Read (firstItem * mRowSize, itemsCount * mRowSize, to);
}


void
PrototypeTable::CheckRowToDelete (const ROW_INDEX row)
{
  bool allFieldsNull = true;

  StoredItem     cachedItem = mRowCache.RetriveItem (row);
  uint8_t* const rowData    = cachedItem.GetDataForUpdate();

  for (FIELD_INDEX index = 0; index < mFieldsCount; index += 8)
    {
      const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
      const uint8_t          bitsSet   = ~0;

      if ( rowData[fieldDesc.NullBitIndex () / 8] != bitsSet)
        {
          allFieldsNull = false;
          break;
        }
    }

  if (allFieldsNull)
    {
      NODE_INDEX   dumyNode;
      KEY_INDEX    dummyKey;
      BTree        removedNodes (*this);
      TableRmKey   key (row);

      removedNodes.InsertKey (key, &dumyNode, &dummyKey);
    }
}


void
PrototypeTable::CheckRowToReuse (const ROW_INDEX row)
{
  bool allFieldsNull = true;

  StoredItem     cachedItem = mRowCache.RetriveItem (row);
  uint8_t* const rowData    = cachedItem.GetDataForUpdate();

  for (FIELD_INDEX index = 0; index < mFieldsCount; index += 8)
    {
      const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal (index);
      const uint8_t          bitsSet   = ~0;

      if ( rowData[fieldDesc.NullBitIndex () / 8] != bitsSet)
        {
          allFieldsNull = false;
          break;
        }
    }

  if (allFieldsNull)
    {
      BTree        removedNodes (*this);
      TableRmKey   key (row);

      removedNodes.RemoveKey (key);
    }
}


void
PrototypeTable::AcquireFieldIndex (FieldDescriptor* const field)
{
  while (true)
    {
      LockRAII syncHolder (mIndexSync);

      if (! field->IsAcquired ())
        {
          field->Acquire ();
          break;
        }

      syncHolder.Release ();
      wh_yield ();
    }
}


void
PrototypeTable::ReleaseIndexField (FieldDescriptor* const field)
{
  LockRAII syncHolder (mIndexSync);

  assert (field->IsAcquired ());

  field->Release ();
}


void
PrototypeTable::Set (const ROW_INDEX      row,
                     const FIELD_INDEX    field,
                     const DChar&         value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX       row,
                     const FIELD_INDEX     field,
                     const DBool&          value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX       row,
                     const FIELD_INDEX     field,
                     const DDate&          value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX       row,
                     const FIELD_INDEX     field,
                     const DDateTime&      value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DHiresTime&      value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DInt8&           value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DInt16&          value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DInt32&          value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DInt64&          value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DReal&           value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DRichReal&       value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX       row,
                     const FIELD_INDEX     field,
                     const DUInt8&         value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX      row,
                     const FIELD_INDEX    field,
                     const DUInt16&       value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX      row,
                     const FIELD_INDEX    field,
                     const DUInt32&       value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX      row,
                     const FIELD_INDEX    field,
                     const DUInt64&       value)
{
  StoreEntry (row, field, value);
}


void
PrototypeTable::Set (const ROW_INDEX      row,
                     const FIELD_INDEX    field,
                     const DText&         value)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row == mRowsCount)
    AddRow ();

  else if (row > mRowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if ((desc.Type () & PS_TABLE_ARRAY_MASK)
      || ((desc.Type () & PS_TABLE_FIELD_TYPE_MASK) != _SC(uint_t, T_TEXT)))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  uint64_t newFirstEntry     = ~0ull;
  uint64_t newFieldValueSize = 0;

  if (value.IsNull() == false)
    {
      ITextStrategy& text = value;
      if (text.IsRowValue ())
        {
          RowFieldText& value = text.GetRow ();

          newFieldValueSize = value.mBytesSize +
                                RowFieldText::CACHE_META_DATA_SIZE;

          if (&value.mStorage != &VSStore ())
            {
              newFirstEntry = VSStore ().AddRecord (value.mStorage,
                                                    value.mFirstEntry,
                                                    0,
                                                    newFieldValueSize);
            }
          else
            {
              value.mStorage.IncrementRecordRef (value.mFirstEntry);

              newFirstEntry = value.mFirstEntry;
            }
        }
      else
        {
          TemporalText& value = text.GetTemporal();

          assert (value.mBytesSize > 0);

          if (value.mBytesSize >= RowFieldText::MAX_BYTES_COUNT)
            {
              throw DBSException (NULL,
                                  _EXTRA (DBSException::OPER_NOT_SUPPORTED));
            }

          newFieldValueSize = value.mBytesSize +
                                RowFieldText::CACHE_META_DATA_SIZE;

          assert (value.CharsCount () <= RowFieldText::MAX_CHARS_COUNT);

          uint8_t headerData[RowFieldText::CACHE_META_DATA_SIZE];

          store_le_int32 (value.CharsCount (), headerData);
          store_le_int32 (value.mCachedCharIndex,
                          headerData + sizeof (uint32_t));
          store_le_int32 (value.mCachedCharIndexOffset,
                          headerData + 2 * sizeof (uint32_t));

          newFirstEntry = VSStore ().AddRecord (
                                                          headerData,
                                                          sizeof (headerData)
                                                            );
          VSStore ().UpdateRecord (newFirstEntry,
                                               sizeof (headerData),
                                               value.mStorage,
                                               0,
                                               value.mBytesSize);
        }
    }

  LockRAII syncHolder (mSync);

  StoredItem     cachedItem        = mRowCache.RetriveItem (row);
  uint8_t* const rowData           = cachedItem.GetDataForUpdate();
  const uint8_t  bitsSet           = ~0;
  bool           fieldValueWasNull = false;

  uint8_t* const fieldFirstEntry = rowData + desc.RowDataOff ();
  uint8_t* const fieldValueSize  = rowData +
                                     desc.RowDataOff () + sizeof (uint64_t);

  const uint_t  byteOff = desc.NullBitIndex () / 8;
  const uint8_t bitOff  = desc.NullBitIndex () % 8;

  if ((rowData[byteOff] & (1 << bitOff)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && value.IsNull ())
    return ;

  else if ( (fieldValueWasNull == false) && value.IsNull())
    {
      rowData[byteOff] |= (1 << bitOff);

      if (rowData[byteOff] == bitsSet)
        CheckRowToDelete (row);

      return;
    }
  else if (value.IsNull() == false)
    {
      if (rowData[byteOff] == bitsSet)
        {
          assert (fieldValueWasNull == true);

          CheckRowToReuse (row);
        }
      rowData[byteOff] &= ~(1 << bitOff);
    }

  if (fieldValueWasNull == false)
    {
      //Postpone the removal of the actual record entries
      //to allow other threads gain access to 'mSync' faster.
      RowFieldText oldEntryRAII (VSStore (),
                                 load_le_int64 (fieldFirstEntry),
                                 load_le_int64 (fieldValueSize));
      syncHolder.Release ();

      VSStore ().DecrementRecordRef (load_le_int64 (fieldFirstEntry));
    }

  store_le_int64 (newFirstEntry,     fieldFirstEntry);
  store_le_int64 (newFieldValueSize, fieldValueSize);
}


void
PrototypeTable::Set (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     const DArray&          value)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row == mRowsCount)
    AddRow ();

  else if (row > mRowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if (((desc.Type () & PS_TABLE_ARRAY_MASK) == 0)
      || ((desc.Type () & PS_TABLE_FIELD_TYPE_MASK) !=
           _SC (uint_t, value.Type ())))
    {
      throw DBSException (NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
    }

  uint64_t      newFirstEntry     = ~0;
  uint64_t      newFieldValueSize = 0;
  const uint8_t bitsSet           = ~0;
  bool          fieldValueWasNull = false;

  if (value.IsNull() == false)
    {
      IArrayStrategy& array = value;
      if (array.IsRowValue())
        {
          RowFieldArray& value = array.GetRow();

          if (&value.mStorage == &VSStore ())
            {
              value.mStorage.IncrementRecordRef (value.mFirstRecordEntry);

              newFieldValueSize = value.RawSize ();
              newFirstEntry     = value.mFirstRecordEntry;
            }
          else
            {
              newFieldValueSize = value.RawSize();
              newFirstEntry     = VSStore ().AddRecord (
                                                      value.mStorage,
                                                      value.mFirstRecordEntry,
                                                      0,
                                                      newFieldValueSize
                                                                   );
            }
        }
      else
        {
          TemporalArray& value = array.GetTemporal ();
          newFieldValueSize    = value.RawSize();

          uint8_t elemsCount[RowFieldArray::METADATA_SIZE];

          store_le_int64 (value.Count(), elemsCount);

          newFirstEntry = VSStore ().AddRecord (elemsCount, sizeof elemsCount);
          VSStore ().UpdateRecord (newFirstEntry,
                                   sizeof elemsCount,
                                   value.mStorage,
                                   0,
                                   newFieldValueSize);
          newFieldValueSize += sizeof elemsCount;
        }
    }

  LockRAII syncHolder (mSync);

  StoredItem     cachedItem = mRowCache.RetriveItem (row);
  uint8_t *const rowData    = cachedItem.GetDataForUpdate();

  uint8_t *const fieldFirstEntry = rowData + desc.RowDataOff ();
  uint8_t *const fieldValueSize  = rowData +
                                     desc.RowDataOff () + sizeof (uint64_t);


  const uint_t  byteOff = desc.NullBitIndex () / 8;
  const uint8_t bitOff  = desc.NullBitIndex () % 8;

  if ((rowData[byteOff] & (1 << bitOff)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && value.IsNull ())
    return ;

  else if ( (fieldValueWasNull == false) && value.IsNull())
    {
      rowData[byteOff] |= (1 << bitOff);

      if (rowData[byteOff] == bitsSet)
        CheckRowToDelete (row);

      return ;
    }
  else if (value.IsNull() == false)
    {
      if (rowData[byteOff] == bitsSet)
        {
          assert (fieldValueWasNull == true);

          CheckRowToReuse (row);
        }
      rowData[byteOff] &= ~(1 << bitOff);
    }

  if (fieldValueWasNull == false)
    {
      //Postpone the removal of the actual record entries
      //to allow other threads gain access to 'mSync' faster.
      RowFieldArray   oldEntryRAII (VSStore (),
                                    load_le_int64 (fieldFirstEntry),
                                    value.Type ());

      VSStore ().DecrementRecordRef (load_le_int64 (fieldFirstEntry));

      store_le_int64 (newFirstEntry,     fieldFirstEntry);
      store_le_int64 (newFieldValueSize, fieldValueSize);

      syncHolder.Release ();
    }
  else
    {
      store_le_int64 (newFirstEntry,     fieldFirstEntry);
      store_le_int64 (newFieldValueSize, fieldValueSize);
    }
}



void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX       field,
                     DChar&                  outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DBool&                 outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DDate&                 outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DDateTime&             outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DHiresTime&            outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DInt8&                 outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DInt16&                outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DInt32&                outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DInt64&                outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DReal&                 outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DRichReal&             outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DUInt8&                outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DUInt16&               outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DUInt32&               outValue)
{
  RetrieveEntry (row, field, outValue);
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DUInt64&               outValue)
{
  RetrieveEntry (row, field, outValue);
}


static ITextStrategy*
allocate_row_field_text (VariableSizeStore&   store,
                         const uint64_t       firstRecordEntry,
                         const uint64_t       valueSize)
{
  ITextStrategy* const strategy = new RowFieldText (store,
                                                    firstRecordEntry,
                                                    valueSize);
  return strategy;
}


static RowFieldArray*
allocate_row_field_array (VariableSizeStore&        store,
                          const uint64_t            firstRecordEntry,
                          const DBS_FIELD_TYPE      type)
{
  return new RowFieldArray (store, firstRecordEntry, type);
}


void
PrototypeTable::Get (const ROW_INDEX   row,
                     const FIELD_INDEX field,
                     DText&            outValue)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row >= mRowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if ((desc.Type () & PS_TABLE_ARRAY_MASK)
      || ((desc.Type () & PS_TABLE_FIELD_TYPE_MASK) != _SC(uint_t, T_TEXT)))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  LockRAII syncHolder (mSync);

  StoredItem           cachedItem = mRowCache.RetriveItem (row);
  const uint8_t* const rowData    = cachedItem.GetDataForRead();

  const uint64_t fieldFirstEntry = load_le_int64 (rowData + desc.RowDataOff ());
  const uint64_t fieldValueSize  = load_le_int64 (
                              rowData + desc.RowDataOff () + sizeof (uint64_t)
                                                 );

  const uint_t  byteOff = desc.NullBitIndex () / 8;
  const uint8_t bitOff  = desc.NullBitIndex () % 8;

  outValue.~DText ();
  if (rowData[byteOff] & (1 << bitOff))
    _placement_new (&outValue, DText ());

  else
    {
      _placement_new (&outValue,
                      DText (*allocate_row_field_text (VSStore (),
                                                       fieldFirstEntry,
                                                       fieldValueSize)));
    }
}


void
PrototypeTable::Get (const ROW_INDEX        row,
                     const FIELD_INDEX      field,
                     DArray&                outValue)
{

  const FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if (row >= mRowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if (((desc.Type () & PS_TABLE_ARRAY_MASK) == 0)
      || (((desc.Type () & PS_TABLE_FIELD_TYPE_MASK) !=
             _SC(uint_t, outValue.Type ()))
          && (outValue.Type () != T_UNDETERMINED)))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  LockRAII syncHolder (mSync);

  StoredItem           cachedItem = mRowCache.RetriveItem (row);
  const uint8_t *const rowData   = cachedItem.GetDataForRead();

  const uint64_t fieldFirstEntry = load_le_int64 (rowData + desc.RowDataOff ());

  const uint_t  byteOff = desc.NullBitIndex () / 8;
  const uint8_t bitOff  = desc.NullBitIndex () % 8;

  outValue.~DArray ();
  if (rowData[byteOff] & (1 << bitOff))
    {
      switch (desc.Type () & PS_TABLE_FIELD_TYPE_MASK)
      {
      case T_BOOL:
        _placement_new (&outValue, DArray(_SC(DBool *, NULL)));
        break;

      case T_CHAR:
        _placement_new (&outValue, DArray(_SC(DChar *, NULL)));
        break;

      case T_DATE:
        _placement_new (&outValue, DArray(_SC(DDate *, NULL)));
        break;

      case T_DATETIME:
        _placement_new (&outValue, DArray(_SC(DDateTime *, NULL)));
        break;

      case T_HIRESTIME:
        _placement_new (&outValue, DArray(_SC(DHiresTime *, NULL)));
        break;

      case T_UINT8:
        _placement_new (&outValue, DArray(_SC(DUInt8 *, NULL)));
        break;

      case T_UINT16:
        _placement_new (&outValue, DArray(_SC(DUInt16 *, NULL)));
        break;

      case T_UINT32:
        _placement_new (&outValue, DArray(_SC(DUInt32 *, NULL)));
        break;

      case T_UINT64:
        _placement_new (&outValue, DArray(_SC(DUInt64 *, NULL)));
        break;

      case T_REAL:
        _placement_new (&outValue, DArray(_SC(DReal *, NULL)));
        break;

      case T_RICHREAL:
        _placement_new (&outValue, DArray(_SC(DRichReal *, NULL)));
        break;

      case T_INT8:
        _placement_new (&outValue, DArray(_SC(DInt8 *, NULL)));
        break;

      case T_INT16:
        _placement_new (&outValue, DArray(_SC(DInt16 *, NULL)));
        break;

      case T_INT32:
        _placement_new (&outValue, DArray(_SC(DInt32 *, NULL)));
        break;

      case T_INT64:
        _placement_new (&outValue, DArray(_SC(DInt64 *, NULL)));
        break;

      default:
        assert (false);
      }
    }
  else
    {
      IArrayStrategy& rowArray = *allocate_row_field_array (
                               VSStore (),
                               fieldFirstEntry,
                              _SC (DBS_FIELD_TYPE,
                                   desc.Type () & PS_TABLE_FIELD_TYPE_MASK)
                                                            );
      _placement_new (&outValue, DArray (rowArray));
    }
}


template<typename T>
void table_exchange_rows (ITable&             table,
                          const FIELD_INDEX   field,
                          const ROW_INDEX     row1,
                          const ROW_INDEX     row2)
{
  T row1Value, row2Value;

  assert (row1 < table.AllocatedRows ());
  assert (row2 < table.AllocatedRows ());

  table.Get (row1, field, row1Value);
  table.Get (row2, field, row2Value);

  if (row1Value == row2Value)
    return ;

  table.Set (row1, field, row2Value);
  table.Set (row2, field, row1Value);
}


template<>
void table_exchange_rows<DArray> (ITable&             table,
                                 const FIELD_INDEX   field,
                                 const ROW_INDEX     row1,
                                 const ROW_INDEX     row2)
{
  DArray row1Value, row2Value;

  assert (row1 < table.AllocatedRows ());
  assert (row2 < table.AllocatedRows ());

  table.Get (row1, field, row1Value);
  table.Get (row2, field, row2Value);

  table.Set (row1, field, row2Value);
  table.Set (row2, field, row1Value);
}


template<>
void table_exchange_rows<DText> (ITable&             table,
                                 const FIELD_INDEX   field,
                                 const ROW_INDEX     row1,
                                 const ROW_INDEX     row2)
{
  DText row1Value, row2Value;

  assert (row1 < table.AllocatedRows ());
  assert (row2 < table.AllocatedRows ());

  table.Get (row1, field, row1Value);
  table.Get (row2, field, row2Value);

  table.Set (row1, field, row2Value);
  table.Set (row2, field, row1Value);
}


void
PrototypeTable::ExchangeRows (const ROW_INDEX    row1,
                              const ROW_INDEX    row2)
{
  const ROW_INDEX     allocatedRows = AllocatedRows ();
  const FIELD_INDEX   fieldsCount   = FieldsCount ();

  if ((allocatedRows <= row1) || (allocatedRows <= row2))
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  for (FIELD_INDEX field = 0; field < fieldsCount; ++field)
    {
      const DBSFieldDescriptor fieldDesc = DescribeField (field);

      if (fieldDesc.isArray)
        table_exchange_rows<DArray> (*this, field, row1, row2);

      else
        {
          switch (fieldDesc.type)
            {
            case T_BOOL:
              table_exchange_rows<DBool> (*this, field, row1, row2);
              break;

            case T_CHAR:
              table_exchange_rows<DChar> (*this, field, row1, row2);
              break;

            case T_DATE:
              table_exchange_rows<DDate> (*this, field, row1, row2);
              break;

            case T_DATETIME:
              table_exchange_rows<DDateTime> (*this, field, row1, row2);
              break;

            case T_HIRESTIME:
              table_exchange_rows<DHiresTime> (*this, field, row1, row2);
              break;

            case T_INT8:
              table_exchange_rows<DInt8> (*this, field, row1, row2);
              break;

            case T_INT16:
              table_exchange_rows<DInt16> (*this, field, row1, row2);
              break;

            case T_INT32:
              table_exchange_rows<DInt32> (*this, field, row1, row2);
              break;

            case T_INT64:
              table_exchange_rows<DInt64> (*this, field, row1, row2);
              break;

            case T_UINT8:
              table_exchange_rows<DUInt8> (*this, field, row1, row2);
              break;

            case T_UINT16:
              table_exchange_rows<DUInt16> (*this, field, row1, row2);
              break;

            case T_UINT32:
              table_exchange_rows<DUInt32> (*this, field, row1, row2);
              break;

            case T_UINT64:
              table_exchange_rows<DUInt64> (*this, field, row1, row2);
              break;

            case T_REAL:
              table_exchange_rows<DReal> (*this, field, row1, row2);
              break;

            case T_RICHREAL:
              table_exchange_rows<DRichReal> (*this, field, row1, row2);
              break;

            case T_TEXT:
              table_exchange_rows<DText> (*this, field, row1, row2);
              break;

            default:
              throw DBSException (
                              NULL,
                              _EXTRA (DBSException::GENERAL_CONTROL_ERROR)
                                 );
            }
        }
    }
}


static bool
operator< (const DText& text1, const DText& text2)
{
  const uint64_t text1Count = text1.Count ();
  const uint64_t text2Count = text2.Count ();

  uint64_t i, j;

  for (i = 0, j = 0;
       ((i < text1Count) && (j < text2Count));
       ++i, ++j)
    {
      const DChar c1 = text1.CharAt (i);
      const DChar c2 = text2.CharAt (j);

      if (c1 < c2)
        return true;

      else if (c1 > c2)
        return false;
    }

  if (i < j)
    return true;

  return false;
}

//Keep this here! Other way g++ compiler fails to notice the operator<'s
//definition.
#include "utils/wsort.h"

template<typename TF> class
SortTableContainer
{
public:
  SortTableContainer (ITable& table, const FIELD_INDEX field)
    : mTable (table),
      mField (field)
  {
  }

  const TF operator[] (const int64_t position) const
  {
    TF value;
    mTable.Get (position, mField, value);

    return value;
  }

  void Exchange (const int64_t pos1, const int64_t pos2)
  {
    mTable.ExchangeRows (pos1, pos2);
  }

  uint64_t Count () const
  {
    return mTable.AllocatedRows ();
  }

  void Pivot (const uint64_t from, const uint64_t to)
  {
    mTable.Get ((from + to) / 2, mField, mPivot);
  }

  const TF& Pivot () const
  {
    return mPivot;
  }

private:
  ITable&             mTable;
  const FIELD_INDEX   mField;
  TF                  mPivot;
};


void
PrototypeTable::Sort (const FIELD_INDEX     field,
                      const ROW_INDEX       fromRow,
                      const ROW_INDEX       toRow,
                      const bool            reverse)
{
  const DBSFieldDescriptor fd = DescribeField (field);

  if (fd.isArray)
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));

  else if (fromRow > toRow)
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  else if (toRow >= AllocatedRows ())
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  switch (fd.type)
    {
      case T_BOOL:
          {
            SortTableContainer<DBool> temp (*this, field);
            quick_sort<DBool, SortTableContainer<DBool> > (fromRow,
                                                           toRow,
                                                           reverse,
                                                           temp);
          }
        break;

      case T_CHAR:
         {
            SortTableContainer<DChar> temp (*this, field);
            quick_sort<DChar, SortTableContainer<DChar> > (fromRow,
                                                           toRow,
                                                           reverse,
                                                           temp);
          }
        break;

      case T_DATE:
         {
            SortTableContainer<DDate> temp (*this, field);
            quick_sort<DDate, SortTableContainer<DDate> > (fromRow,
                                                           toRow,
                                                           reverse,
                                                           temp);
          }
        break;

      case T_DATETIME:
         {
            SortTableContainer<DDateTime> temp (*this, field);
            quick_sort<DDateTime, SortTableContainer<DDateTime> > (fromRow,
                                                                   toRow,
                                                                   reverse,
                                                                   temp);
          }
        break;

      case T_HIRESTIME:
         {
            SortTableContainer<DHiresTime> temp (*this, field);
            quick_sort<DHiresTime, SortTableContainer<DHiresTime> > (fromRow,
                                                                     toRow,
                                                                     reverse,
                                                                     temp);
          }
        break;

      case T_UINT8:
         {
            SortTableContainer<DUInt8> temp (*this, field);
            quick_sort<DUInt8, SortTableContainer<DUInt8> > (fromRow,
                                                             toRow,
                                                             reverse,
                                                             temp);
          }
        break;

      case T_UINT16:
         {
            SortTableContainer<DUInt16> temp (*this, field);
            quick_sort<DUInt16, SortTableContainer<DUInt16> > (fromRow,
                                                               toRow,
                                                               reverse,
                                                               temp);
          }
        break;

      case T_UINT32:
         {
            SortTableContainer<DUInt32> temp (*this, field);
            quick_sort<DUInt32, SortTableContainer<DUInt32> > (fromRow,
                                                               toRow,
                                                               reverse,
                                                               temp);
          }
        break;

      case T_UINT64:
         {
            SortTableContainer<DUInt64> temp (*this, field);
            quick_sort<DUInt64, SortTableContainer<DUInt64> > (fromRow,
                                                               toRow,
                                                               reverse,
                                                               temp);
          }
        break;

      case T_REAL:
         {
            SortTableContainer<DReal> temp (*this, field);
            quick_sort<DReal, SortTableContainer<DReal> > (fromRow,
                                                           toRow,
                                                           reverse,
                                                           temp);
          }
        break;

      case T_RICHREAL:
         {
            SortTableContainer<DRichReal> temp (*this, field);
            quick_sort<DRichReal, SortTableContainer<DRichReal> > (fromRow,
                                                                   toRow,
                                                                   reverse,
                                                                   temp);
          }
        break;

      case T_INT8:
         {
            SortTableContainer<DInt8> temp (*this, field);
            quick_sort<DInt8, SortTableContainer<DInt8> > (fromRow,
                                                           toRow,
                                                           reverse,
                                                           temp);
          }
        break;

      case T_INT16:
         {
            SortTableContainer<DInt16> temp (*this, field);
            quick_sort<DInt16, SortTableContainer<DInt16> > (fromRow,
                                                             toRow,
                                                             reverse,
                                                             temp);
          }
        break;

      case T_INT32:
         {
            SortTableContainer<DInt32> temp (*this, field);
            quick_sort<DInt32, SortTableContainer<DInt32> > (fromRow,
                                                             toRow,
                                                             reverse,
                                                             temp);
          }
        break;

      case T_INT64:
         {
            SortTableContainer<DInt64> temp (*this, field);
            quick_sort<DInt64, SortTableContainer<DInt64> > (fromRow,
                                                             toRow,
                                                             reverse,
                                                             temp);
          }
        break;
      case T_TEXT:
         {
            SortTableContainer<DText> temp (*this, field);
            quick_sort<DText, SortTableContainer<DText> > (fromRow,
                                                           toRow,
                                                           reverse,
                                                           temp);
          }
        break;

    default:
      throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
    }
}


DArray
PrototypeTable::MatchRows (const DBool&       min,
                           const DBool&       max,
                           const ROW_INDEX    fromRow,
                           const ROW_INDEX    toRow,
                           const FIELD_INDEX  field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DChar&         min,
                           const DChar&         max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DDate&         min,
                           const DDate&         max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DDateTime&     min,
                           const DDateTime&     max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DHiresTime&     min,
                           const DHiresTime&     max,
                           const ROW_INDEX       fromRow,
                           const ROW_INDEX       toRow,
                           const FIELD_INDEX     field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DUInt8&        min,
                           const DUInt8&        max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DUInt16&       min,
                           const DUInt16&       max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DUInt32&       min,
                           const DUInt32&       max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DUInt64&       min,
                           const DUInt64&       max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DInt8&         min,
                           const DInt8&         max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DInt16&        min,
                           const DInt16&        max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DInt32&       min,
                           const DInt32&       max,
                           const ROW_INDEX     fromRow,
                           const ROW_INDEX     toRow,
                           const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DInt64&       min,
                           const DInt64&       max,
                           const ROW_INDEX     fromRow,
                           const ROW_INDEX     toRow,
                           const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DReal&        min,
                           const DReal&        max,
                           const ROW_INDEX     fromRow,
                           const ROW_INDEX     toRow,
                           const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows (const DRichReal&    min,
                           const DRichReal&    max,
                           const ROW_INDEX     fromRow,
                           const ROW_INDEX     toRow,
                           const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != NULL)
    return MatchRowsWithIndex (min, max, fromRow, toRow, field);

  return MatchRowsNoIndex (min, max, fromRow, toRow, field);
}


template <class T> void
PrototypeTable::StoreEntry (const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const T&          value)
{

  //Check if we are trying to write a different value
  T currentValue;

  if (row == mRowsCount)
    AddRow ();

  else
    RetrieveEntry (row, field, currentValue);

  if (currentValue == value)
    return; //Nothing to change

  const uint8_t      bitsSet  = ~0;
  FieldDescriptor&   desc     = GetFieldDescriptorInternal (field);
  const uint_t       byteOff = desc.NullBitIndex () / 8;
  const uint8_t      bitOff  = desc.NullBitIndex () % 8;

  LockRAII syncHolder (mSync);

  StoredItem     cachedItem = mRowCache.RetriveItem (row);
  uint8_t *const rowData   = cachedItem.GetDataForUpdate();

  if (value.IsNull ())
    {
      assert ((rowData[byteOff] & (1 << bitOff)) == 0);

      rowData[byteOff] |= (1 << bitOff);

      if (rowData[byteOff] == bitsSet)
        CheckRowToDelete (row);
    }
  else
    {
      if (rowData[byteOff] == bitsSet)
        CheckRowToReuse (row);

      rowData[byteOff] &= ~(1 << bitOff);

      Serializer::Store (rowData + desc.RowDataOff (), value);
    }

  syncHolder.Release ();

  //Update the field index if it exists
  if (mvIndexNodeMgrs[field] != NULL)
    {
      NODE_INDEX        dummyNode;
      KEY_INDEX         dummyKey;

      AcquireFieldIndex (&desc);

      try
        {
          BTree fieldIndexTree (*mvIndexNodeMgrs[field]);

          fieldIndexTree.RemoveKey (T_BTreeKey<T> (currentValue, row));
          fieldIndexTree.InsertKey (T_BTreeKey<T> (value, row),
                                    &dummyNode,
                                    &dummyKey);
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

  if (row >= mRowsCount)
    throw DBSException (NULL, _EXTRA (DBSException::ROW_NOT_ALLOCATED));

  if ((desc.Type () & PS_TABLE_ARRAY_MASK)
      || ((desc.Type () & PS_TABLE_FIELD_TYPE_MASK) !=
            _SC(uint_t,  outValue.DBSType ())))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  const uint_t  byteOff = desc.NullBitIndex () / 8;
  const uint8_t bitOff  = desc.NullBitIndex () % 8;

  LockRAII syncHolder (mSync);

  StoredItem           cachedItem = mRowCache.RetriveItem (row);
  const uint8_t* const rowData    = cachedItem.GetDataForRead ();

  if (rowData[byteOff] & (1 << bitOff))
    {
      outValue.~T();
      _placement_new (&outValue, T ());
    }
  else
    Serializer::Load (rowData + desc.RowDataOff (), &outValue);
}


template <class T> DArray
PrototypeTable::MatchRowsWithIndex (const T&          min,
                                    const T&          max,
                                    const ROW_INDEX   fromRow,
                                    ROW_INDEX         toRow,
                                    const FIELD_INDEX field)
{
  FieldDescriptor& desc = GetFieldDescriptorInternal (field);

  if ((desc.Type () & PS_TABLE_ARRAY_MASK)
      || ((desc.Type () & PS_TABLE_FIELD_TYPE_MASK) !=
            _SC(uint_t, min.DBSType ())))
    {
      throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
    }

  toRow = MIN (toRow, ((mRowsCount > 0) ? mRowsCount - 1 : 0));

  FieldIndexNodeManager* const nodeMgr = mvIndexNodeMgrs[field];

  DArray                       result;
  NODE_INDEX                   nodeId;
  KEY_INDEX                    keyIndex;
  const T_BTreeKey<T>          firstKey (min, fromRow);
  const T_BTreeKey<T>          lastKey (max, toRow);

  assert (nodeMgr != NULL);

  AcquireFieldIndex (&desc);

  try
    {
      BTree fieldIndexTree (*nodeMgr);

      if ( ! fieldIndexTree.FindBiggerOrEqual (firstKey, &nodeId, &keyIndex))
        goto force_return;

      BTreeNodeRAII currentNode (nodeMgr->RetrieveNode (nodeId));

      assert (keyIndex < currentNode->KeysCount ());

      while (true)
        {
          IBTreeFieldIndexNode* node = _SC (IBTreeFieldIndexNode*,
                                            &*currentNode);

          KEY_INDEX toKey    = ~0;;
          bool      lastNode = false;

          if (node->FindBiggerOrEqual (lastKey, &toKey))
            {
              lastNode = true;

              if (node->IsLess (lastKey, toKey))
                toKey++;
            }
          else
            toKey = 0;

          node->GetRows (keyIndex, toKey, result);

          if (lastNode || (node->Next () == NIL_NODE))
            break;

          else
            {
              currentNode = nodeMgr->RetrieveNode (node->Next ());

              assert (currentNode->KeysCount() > 0);

              keyIndex = currentNode->KeysCount () - 1;
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


template <class T> DArray
PrototypeTable::MatchRowsNoIndex (const T&          min,
                                  const T&          max,
                                  const ROW_INDEX   fromRow,
                                  ROW_INDEX         toRow,
                                  const FIELD_INDEX field)
{
  toRow = MIN (toRow, ((mRowsCount > 0) ? mRowsCount - 1 : 0));

  DArray result;
  T      rowValue;

  for (ROW_INDEX row = fromRow; row <= toRow; ++row)
    {
      Get (row, field, rowValue);

      if ((rowValue < min) || (max < rowValue))
        continue;

      result.Add (DROW_INDEX (row));
    }

  return result;
}


TableRmNode::TableRmNode (PrototypeTable& table, const NODE_INDEX nodeId)
  : IBTreeNode (table, nodeId)
{
  assert ((sizeof (NodeHeader) % ( 2 * sizeof  (uint64_t)) == 0));
  assert (RAW_NODE_SIZE == mNodesMgr.NodeRawSize ());
}


uint_t
TableRmNode::KeysPerNode () const
{
  assert (sizeof (NodeHeader) % sizeof (uint64_t) == 0);

  uint_t result = mNodesMgr.NodeRawSize () - sizeof (NodeHeader);

  if (IsLeaf ())
    result /= sizeof (ROW_INDEX);

  else
    result /= sizeof (ROW_INDEX) + sizeof (NODE_INDEX);

  return result;
}


KEY_INDEX
TableRmNode::GetParentKeyIndex (const IBTreeNode& parent) const
{
  assert (KeysCount() > 0);

  KEY_INDEX               result;
  const ROW_INDEX* const  keys = _RC (const ROW_INDEX*, DataForRead ());

  const TableRmKey key (Serializer::LoadRow (keys));

  parent.FindBiggerOrEqual (key, &result);

  assert (parent.IsEqual (key, result));
  assert (NodeId () == parent.NodeIdOfKey (result));

  return result;
}


NODE_INDEX
TableRmNode::NodeIdOfKey (const KEY_INDEX keyIndex) const
{
  assert (keyIndex < KeysCount ());
  assert (sizeof (NodeHeader) % sizeof (uint64_t) == 0);
  assert (IsLeaf () == false);

  const uint_t            firstNodeOff = KeysPerNode () * sizeof (ROW_INDEX);
  const NODE_INDEX* const nodes        = _RC (const NODE_INDEX*,
                                              DataForRead() + firstNodeOff);
  return Serializer::LoadNode (nodes + keyIndex);
}


void
TableRmNode::AdjustKeyNode (const IBTreeNode&   childNode,
                            const KEY_INDEX     keyIndex)
{
  assert (childNode.NodeId () == NodeIdOfKey (keyIndex));
  assert (IsLeaf() == false);

  const TableRmNode&     child     = _SC (const TableRmNode&, childNode);
  ROW_INDEX* const       keys      = _RC (ROW_INDEX*, DataForWrite ());
  const ROW_INDEX* const childKeys = _RC (const ROW_INDEX*,
                                          child.DataForRead ());

  Serializer::StoreRow (Serializer::LoadRow (childKeys), keys + keyIndex);
}


void
TableRmNode::SetNodeOfKey (const KEY_INDEX keyIndex, const NODE_INDEX childNode)
{

  assert (keyIndex < KeysCount ());
  assert (sizeof (NodeHeader) % sizeof (uint64_t) == 0);
  assert (IsLeaf () == false);

  const uint_t      nodesOff = KeysPerNode() * sizeof (ROW_INDEX);
  NODE_INDEX* const nodes    = _RC (NODE_INDEX*, DataForWrite () + nodesOff);

  Serializer::StoreNode (childNode, nodes + keyIndex);
}


KEY_INDEX
TableRmNode::InsertKey (const IBTreeKey& key)
{
  if (KeysCount () > 0)
    {
      KEY_INDEX keyIndex;
      KEY_INDEX lastKey = KeysCount () - 1;

      assert (lastKey < (KeysPerNode() - 1));

      if (FindBiggerOrEqual (key, &keyIndex) == false)
        keyIndex = 0;

      else
        keyIndex++;

      assert ((keyIndex == 0) || IsLess (key, keyIndex - 1));

      ROW_INDEX* const rows = _RC (ROW_INDEX*, DataForWrite ());

      make_array_room (lastKey,
                       keyIndex,
                       sizeof (rows[0]),
                       _RC (uint8_t*, rows));
      KeysCount (KeysCount() + 1);
      Serializer::StoreRow ( *_SC (const TableRmKey*, &key), rows + keyIndex);

      if (IsLeaf () == false)
        {
          NODE_INDEX* const nodes = _RC (NODE_INDEX*, rows + KeysPerNode());
          make_array_room (lastKey,
                           keyIndex,
                           sizeof (NODE_INDEX),
                           _RC (uint8_t*, nodes));
        }

      return keyIndex;
    }

  ROW_INDEX* const rows = _RC (ROW_INDEX*, DataForWrite ());

  Serializer::StoreRow ( *_SC (const TableRmKey*, &key), rows);

  KeysCount (1);

  return 0;
}


void
TableRmNode::RemoveKey (const KEY_INDEX keyIndex)
{
  uint_t lastKey = KeysCount () - 1;

  assert (lastKey < (KeysPerNode() - 1));

  NODE_INDEX* const rows = _RC (NODE_INDEX*, DataForWrite ());
  remove_array_elemes (lastKey,
                       keyIndex,
                       sizeof (rows[0]),
                       _RC (uint8_t*, rows));

  if (IsLeaf () == false)
    {
      NODE_INDEX* const nodes = _RC (NODE_INDEX*, rows + KeysPerNode());
      remove_array_elemes (lastKey,
                           keyIndex,
                           sizeof (nodes[0]),
                           _RC (uint8_t*, nodes));
    }

  KeysCount (KeysCount () - 1);
}


void
TableRmNode::Split ( const NODE_INDEX parentId)
{
  assert (NeedsSpliting ());

  const ROW_INDEX* const rows     = _RC (const ROW_INDEX*, DataForRead ());
  const KEY_INDEX        splitKey = KeysCount() / 2;

  BTreeNodeRAII parentNode (mNodesMgr.RetrieveNode (parentId));

  const TableRmKey key (Serializer::LoadRow (rows + splitKey));
  const KEY_INDEX  insertionPos    = parentNode->InsertKey (key);
  const NODE_INDEX allocatedNodeId = mNodesMgr.AllocateNode (parentId,
                                                             insertionPos);

  BTreeNodeRAII allocatedNode (mNodesMgr.RetrieveNode (allocatedNodeId));

  allocatedNode->Leaf (IsLeaf ());
  allocatedNode->MarkAsUsed();

  ROW_INDEX* const newRows = _RC (ROW_INDEX*, allocatedNode->DataForWrite ());

  for (uint_t index = splitKey; index < KeysCount (); ++index)
    {
      const ROW_INDEX r = Serializer::LoadRow (rows + index);

      Serializer::StoreRow (r, newRows + index - splitKey);
    }

  if (IsLeaf () == false)
    {
      const NODE_INDEX* const nodes = _RC (const NODE_INDEX*,
                                           rows + KeysPerNode ());
      NODE_INDEX* const newNodes = _RC (NODE_INDEX*,
                                        newRows + KeysPerNode ());

      for (uint_t index = splitKey; index < KeysCount (); ++index)
        {
          const NODE_INDEX n = Serializer::LoadNode (nodes + index);

          Serializer::StoreNode (n, newNodes + index - splitKey);
        }
    }

  allocatedNode->KeysCount (KeysCount() - splitKey);
  KeysCount (splitKey);

  allocatedNode->Next (NodeId());
  allocatedNode->Prev (Prev());
  Prev (allocatedNodeId);
  if (allocatedNode->Prev() != NIL_NODE)
    {
      BTreeNodeRAII prevNode (mNodesMgr.RetrieveNode (allocatedNode->Prev()));
      prevNode->Next (allocatedNodeId);
    }
}


void
TableRmNode::Join (const bool toRight)
{
  assert (NeedsJoining ());

  ROW_INDEX* const  rows  = _RC (ROW_INDEX*,  DataForWrite ());
  NODE_INDEX* const nodes = _RC (NODE_INDEX*, rows + KeysPerNode ());

  if (toRight)
    {
      assert (Next () != NIL_NODE);

      BTreeNodeRAII next (mNodesMgr.RetrieveNode (Next ()));

      TableRmNode* const nextNode = _SC (TableRmNode*, &(*next));
      ROW_INDEX* const   destRows = _RC (ROW_INDEX*, nextNode->DataForWrite ());

      for (uint_t index = 0; index < KeysCount (); ++index)
        {
          const ROW_INDEX r = Serializer::LoadRow (rows + index);

          Serializer::StoreRow (r, destRows + index + nextNode->KeysCount ());
        }

      if (IsLeaf () == false)
        {
          NODE_INDEX* const destNodes = _RC (NODE_INDEX*,
                                             destRows + KeysPerNode ());

          for (uint_t index = 0; index < KeysCount (); ++index)
            {
              const NODE_INDEX n = Serializer::LoadNode (nodes + index);

              Serializer::StoreNode (
                                n,
                                destNodes + index + nextNode->KeysCount ()
                                    );
            }
        }

      nextNode->KeysCount (nextNode->KeysCount () + KeysCount ());
      nextNode->Prev (Prev ());

      if (Prev () != NIL_NODE)
        {
          BTreeNodeRAII prevNode (mNodesMgr.RetrieveNode (Prev ()));
          prevNode->Next (Next ());
        }
    }
  else
    {
      assert (Prev () != NIL_NODE);

      BTreeNodeRAII prev (mNodesMgr.RetrieveNode (Prev ()));

      TableRmNode* const prevNode = _SC (TableRmNode*, &(*prev));
      ROW_INDEX *const   srcRows  = _RC (ROW_INDEX*, prevNode->DataForWrite ());

      for (uint_t index = 0; index < prevNode->KeysCount(); ++index)
        {
          const ROW_INDEX r = Serializer::LoadRow (srcRows + index);

          Serializer::StoreRow (r, rows + index + KeysCount ());
        }

      if (IsLeaf () == false)
        {
          NODE_INDEX* const srcNodes = _RC (NODE_INDEX*,
                                            srcRows + KeysPerNode ());

          for (uint_t index = 0; index < prevNode->KeysCount (); ++index)
            {
              const NODE_INDEX n = Serializer::LoadNode (srcNodes + index);

              Serializer::StoreNode (n, nodes + index + KeysCount ());
            }
        }

      KeysCount (KeysCount () + prevNode->KeysCount());
      Prev (prevNode->Prev ());
      if (Prev () != NIL_NODE)
        {
          BTreeNodeRAII prevNode (mNodesMgr.RetrieveNode (Prev ()));
          prevNode->Next (NodeId ());
        }
    }
}


bool
TableRmNode::IsLess (const IBTreeKey& key, const KEY_INDEX keyIndex) const
{
  const ROW_INDEX* const rows = _RC ( const ROW_INDEX*, DataForRead ());

  const TableRmKey tKey (*_SC (const TableRmKey*, &key));

  return tKey < Serializer::LoadRow (rows + keyIndex);
}


bool
TableRmNode::IsEqual (const IBTreeKey& key, const KEY_INDEX keyIndex) const
{
  const ROW_INDEX* const rows = _RC ( const ROW_INDEX*, DataForRead ());

  const TableRmKey tKey (*_SC (const TableRmKey*, &key));

  return tKey == Serializer::LoadRow (rows + keyIndex);
}


bool
TableRmNode::IsBigger (const IBTreeKey& key, KEY_INDEX const keyIndex) const
{
  const ROW_INDEX* const rows = _RC ( const ROW_INDEX*, DataForRead ());

  const TableRmKey tKey (*_SC (const TableRmKey*, &key));

  return tKey > Serializer::LoadRow (rows + keyIndex);
}


const IBTreeKey&
TableRmNode::SentinelKey () const
{
  static TableRmKey _key (~0);

  return _key;
}


} //namespace pastra
} //namespace whisper


