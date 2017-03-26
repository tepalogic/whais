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


#include "utils/endianness.h"
#include "utils/wutf.h"
#include "utils/wunicode.h"
#include "utils/wsort.h"
#include "ps_templatetable.h"
#include "ps_serializer.h"
#include "ps_textstrategy.h"
#include "ps_arraystrategy.h"


using namespace std;

namespace whais {
namespace pastra {


PrototypeTable::PrototypeTable(DbsHandler& dbs)
  : mDbs(dbs),
    mRowsCount(0),
    mRootNode(NIL_NODE),
    mUnallocatedHead(NIL_NODE),
    mRowSize(0),
    mDescriptorsSize(0),
    mFieldsCount(0),
    mRowModified(false),
    mLockInProgress(false)
{
}


PrototypeTable::PrototypeTable(const PrototypeTable& prototype)
  : mDbs(prototype.mDbs),
    mRowsCount(0),
    mRootNode(NIL_NODE),
    mUnallocatedHead(NIL_NODE),
    mRowSize(prototype.mRowSize),
    mDescriptorsSize(prototype.mDescriptorsSize),
    mFieldsCount(prototype.mFieldsCount),
    mFieldsDescriptors(),
    mvIndexNodeMgrs(),
    mRowsSync(),
    mIndexesSync(),
    mRowModified(false),
    mLockInProgress(false)
{
  //TODO: Should be possible for the two prototypes to share the same memory
  //      for fields descriptors.
  mFieldsDescriptors.reset(new uint8_t[mDescriptorsSize]);

  memcpy(mFieldsDescriptors.get(), prototype.mFieldsDescriptors.get(), mDescriptorsSize);
}


void
PrototypeTable::Flush()
{
  DoubleLockRAII<Lock> _l(mRowsSync, mIndexesSync);

  FlushInternal();
}


void
PrototypeTable::LockTable()
{
  if (mLockInProgress)
    throw DBSException(_EXTRA(DBSException::TABLE_ALREADY_LOCKED));

  mLockInProgress = true;

  mRowsSync.Acquire();
  mIndexesSync.Acquire();

  FlushInternal();
}


void
PrototypeTable::UnlockTable()
{
  if ( ! mLockInProgress)
    throw DBSException(_EXTRA(DBSException::TABLE_NOT_LOCKED));

  mLockInProgress = false;

  mIndexesSync.Release();
  mRowsSync.Release();
}


FIELD_INDEX
PrototypeTable::FieldsCount()
{
  return mFieldsCount;
}


FIELD_INDEX
PrototypeTable::RetrieveField(const char* name)
{
  const char* fieldName = _RC(const char*, mFieldsDescriptors.get());

  //Initial iterator points at the at the first field's name.
  fieldName += mFieldsCount * sizeof(FieldDescriptor);
  for (FIELD_INDEX index = 0; index < mFieldsCount; ++index)
  {
    if (strcmp(fieldName, name) == 0)
      return index;

    fieldName += strlen(fieldName) + 1;
  }

  throw DBSException(_EXTRA(DBSException::FIELD_NOT_FOUND), "Cannot find table field '%s'.", name);
}


DBSFieldDescriptor
PrototypeTable::DescribeField(const FIELD_INDEX field)
{
  if (field >= mFieldsCount)
  {
    throw DBSException(_EXTRA(DBSException::FIELD_NOT_FOUND),
                       "Table field index is invalid %u(count %u).",
                       field,
                       mFieldsCount);
  }
  const FieldDescriptor* const desc = _RC(const FieldDescriptor*, mFieldsDescriptors.get());

  DBSFieldDescriptor result;
  result.isArray = (desc[field].Type() & PS_TABLE_ARRAY_MASK) != 0;
  result.type = _SC(DBS_FIELD_TYPE, desc[field].Type() & PS_TABLE_FIELD_TYPE_MASK);
  result.name = _RC(const char*, desc) + desc[field].NameOffset();

  return result;
}


ROW_INDEX
PrototypeTable::AllocatedRows()
{
  return mRowsCount;
}


template<class T> static void
insert_null_field_value(BTree& tree, const ROW_INDEX row)
{
  NODE_INDEX dummyNode;
  KEY_INDEX dummyKey;

  const T nullValue;

  T_BTreeKey<T> keyValue(nullValue, row);
  tree.InsertKey(keyValue, &dummyNode, &dummyKey);
}


ROW_INDEX
PrototypeTable::AddRow(const bool skipThreadSafety)
{
  MarkRowModification();
  LockRAII<Lock> _l(mRowsSync, skipThreadSafety);

  uint64_t lastRowPosition = mRowsCount * mRowSize;
  uint_t toWrite = mRowSize;

  uint8_t dummyValue[128];
  memset(dummyValue, 0xFF, sizeof dummyValue);

  mRowCache.FlushItem(mRowsCount - 1);

  while (toWrite > 0)
  {
    const uint_t writeChunk = MIN(toWrite, sizeof(dummyValue));

    RowsContainer().Write(lastRowPosition, writeChunk, dummyValue);

    toWrite -= writeChunk, lastRowPosition += writeChunk;
  }

  NODE_INDEX dummyNode;
  KEY_INDEX dummyKey;
  BTree removedRows( *this);

  removedRows.InsertKey(TableRmKey(mRowsCount), &dummyNode, &dummyKey);

  LockRAII<Lock> syncHolder2(mIndexesSync);

  for (uint_t f = 0; f < mvIndexNodeMgrs.size(); f++)
  {
    if (mvIndexNodeMgrs[f] == nullptr)
      continue;

    const FieldDescriptor& fd = GetFieldDescriptorInternal(f);
    while (true)
    {
      if ( !fd.IsAcquired())
      {
        BTree fieldIndexTree( *mvIndexNodeMgrs[f]);

        switch (GET_BASIC_TYPE(fd.Type()))
        {
        case T_BOOL:
        {
          insert_null_field_value<DBool>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_CHAR:
        {
          insert_null_field_value<DChar>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_DATE:
        {
          insert_null_field_value<DDate>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_DATETIME:
        {
          insert_null_field_value<DDateTime>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_HIRESTIME:
        {
          insert_null_field_value<DHiresTime>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_INT8:
        {
          insert_null_field_value<DInt8>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_INT16:
        {
          insert_null_field_value<DInt16>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_INT32:
        {
          insert_null_field_value<DInt32>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_INT64:
        {
          insert_null_field_value<DInt64>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_UINT8:
        {
          insert_null_field_value<DUInt8>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_UINT16:
        {
          insert_null_field_value<DUInt16>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_UINT32:
        {
          insert_null_field_value<DUInt32>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_UINT64:
        {
          insert_null_field_value<DUInt64>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_REAL:
        {
          insert_null_field_value<DReal>(fieldIndexTree, mRowsCount);
          break;
        }
        case T_RICHREAL:
        {
          insert_null_field_value<DRichReal>(fieldIndexTree, mRowsCount);
          break;
        }
        default:
          assert(false);
        }

        break; //Out of while
      }
      wh_yield();
    }
  }

  // The rows count has to be update before the procedure code is executed.
  // Other way the new content will not be refreshed.
  mRowCache.RefreshItem(mRowsCount++);
  return mRowsCount - 1;
}


ROW_INDEX
PrototypeTable::GetReusableRow(const bool forceAdd)
{
  uint64_t result = INVALID_ROW_INDEX;
  NODE_INDEX node;
  KEY_INDEX keyIndex;
  TableRmKey key(0);
  BTree removedRows( *this);

  LockRAII<Lock> syncHolder(mRowsSync);
  if (removedRows.FindBiggerOrEqual(key, &node, &keyIndex) == false)
  {
    if (forceAdd)
    {
      syncHolder.Release();
      return PrototypeTable::AddRow();
    }
  }
  else
  {
    auto keyNode = RetrieveNode(node);
    TableRmNode* const node = _SC(TableRmNode*, keyNode.get());

    assert(keyNode->IsLeaf());
    assert(keyIndex < keyNode->KeysCount());

    const ROW_INDEX* const rows = _RC(const ROW_INDEX*, node->DataForRead());
    result = Serializer::LoadRow(rows + keyIndex);
  }

  return result;
}


ROW_INDEX
PrototypeTable::ReusableRowsCount()
{
  uint64_t result = 0;
  NODE_INDEX nodeId;
  KEY_INDEX keyIndex;
  TableRmKey key(0);
  BTree removedRows( *this);

  LockRAII<Lock> syncHolder(mRowsSync);
  if (removedRows.FindBiggerOrEqual(key, &nodeId, &keyIndex) == false)
    return 0;

  else do
  {
    auto keyNode = RetrieveNode(nodeId);
    TableRmNode* const node = _SC(TableRmNode*, keyNode.get());

    result += node->KeysCount();
    nodeId = node->Next();
  } while (nodeId != NIL_NODE);

  assert(result >= 2);

  return result - 1; //Don't count the sentinel of the last node.
}


void
PrototypeTable::MarkRowForReuse(const ROW_INDEX row)
{
  FIELD_INDEX fieldsCount = mFieldsCount;

  while (fieldsCount-- > 0)
  {
    const FieldDescriptor& field = GetFieldDescriptorInternal(fieldsCount);

    if (IS_ARRAY(field.Type()))
      Set(row, fieldsCount, DArray());

    else
    {
      switch (GET_BASIC_TYPE(field.Type()))
      {
      case T_BOOL:
        Set(row, fieldsCount, DBool());
        break;

      case T_CHAR:
        Set(row, fieldsCount, DChar());
        break;

      case T_DATE:
        Set(row, fieldsCount, DDate());
        break;

      case T_DATETIME:
        Set(row, fieldsCount, DDateTime());
        break;

      case T_HIRESTIME:
        Set(row, fieldsCount, DHiresTime());
        break;

      case T_INT8:
        Set(row, fieldsCount, DInt8());
        break;

      case T_INT16:
        Set(row, fieldsCount, DInt16());
        break;

      case T_INT32:
        Set(row, fieldsCount, DInt32());
        break;

      case T_INT64:
        Set(row, fieldsCount, DInt64());
        break;

      case T_UINT8:
        Set(row, fieldsCount, DUInt8());
        break;

      case T_UINT16:
        Set(row, fieldsCount, DUInt16());
        break;

      case T_UINT32:
        Set(row, fieldsCount, DUInt32());
        break;

      case T_UINT64:
        Set(row, fieldsCount, DUInt64());
        break;

      case T_REAL:
        Set(row, fieldsCount, DReal());
        break;

      case T_RICHREAL:
        Set(row, fieldsCount, DRichReal());
        break;

      case T_TEXT:
        Set(row, fieldsCount, DText());
        break;

      default:
        assert(false);
      }
    }
  }
}


template<class T> static void
insert_row_field(PrototypeTable& table,
                 BTree& tree,
                 const ROW_INDEX row,
                 const FIELD_INDEX field)
{
  NODE_INDEX dummyNode;
  KEY_INDEX dummyKey;

  T rowValue;
  table.Get(row, field, rowValue, true);

  T_BTreeKey<T> keyValue(rowValue, row);

  tree.InsertKey(keyValue, &dummyNode, &dummyKey);
}


void
PrototypeTable::CreateIndex(const FIELD_INDEX field,
                            CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                            CreateIndexCallbackContext* const cbContext)
{
  if ((cbFunc == nullptr) && (cbContext != nullptr))
    throw DBSException(_EXTRA(DBSException::INVALID_PARAMETERS));

  if (field >= mFieldsCount)
  {
    throw DBSException(_EXTRA(DBSException::FIELD_NOT_FOUND),
                       "Table field index is invalid %u(%u),",
                       field,
                       mFieldsCount);
  }

  LockRAII<Lock> syncHolder(mRowsSync);

  assert(mvIndexNodeMgrs.size() == mFieldsCount);

  if (mvIndexNodeMgrs[field] != nullptr)
    throw DBSException(_EXTRA(DBSException::FIELD_INDEXED));

  FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  if ((desc.Type() == T_TEXT) || (desc.Type() & PS_TABLE_ARRAY_MASK) != 0)
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID),
                       "This implementation does not support indexing text "
                       "or array fields.");
  }

  const uint_t nodeSizeKB  = 16; //16KB

  unique_ptr<IDataContainer> indexContainer(CreateIndexContainer(field));
  unique_ptr<FieldIndexNodeManager> nodeMgr(new FieldIndexNodeManager(indexContainer,
                                                                      nodeSizeKB * 1024,
                                                                      0x400000, //4MB
                                                                      _SC(DBS_FIELD_TYPE,
                                                                          desc.Type()),
                                                                      true));

  BTree fieldTree( *nodeMgr.get());
  for (ROW_INDEX row = 0; row < mRowsCount; ++row)
  {
    switch (desc.Type())
    {
    case T_BOOL:
      insert_row_field<DBool>( *this, fieldTree, row, field);
      break;

    case T_CHAR:
      insert_row_field<DChar>( *this, fieldTree, row, field);
      break;

    case T_DATE:
      insert_row_field<DDate>( *this, fieldTree, row, field);
      break;

    case T_DATETIME:
      insert_row_field<DDateTime>( *this, fieldTree, row, field);
      break;

    case T_HIRESTIME:
      insert_row_field<DHiresTime>( *this, fieldTree, row, field);
      break;

    case T_UINT8:
      insert_row_field<DUInt8>( *this, fieldTree, row, field);
      break;

    case T_UINT16:
      insert_row_field<DUInt16>( *this, fieldTree, row, field);
      break;

    case T_UINT32:
      insert_row_field<DUInt32>( *this, fieldTree, row, field);
      break;

    case T_UINT64:
      insert_row_field<DUInt64>( *this, fieldTree, row, field);
      break;

    case T_INT8:
      insert_row_field<DInt8>( *this, fieldTree, row, field);
      break;

    case T_INT16:
      insert_row_field<DInt16>( *this, fieldTree, row, field);
      break;

    case T_INT32:
      insert_row_field<DInt32>( *this, fieldTree, row, field);
      break;

    case T_INT64:
      insert_row_field<DInt64>( *this, fieldTree, row, field);
      break;

    case T_REAL:
      insert_row_field<DReal>( *this, fieldTree, row, field);
      break;

    case T_RICHREAL:
      insert_row_field<DRichReal>( *this, fieldTree, row, field);
      break;

    default:
      assert(false);
    }

    if (cbFunc != nullptr)
    {
      if (cbContext != nullptr)
      {
        cbContext->mRowsCount = mRowsCount;
        cbContext->mRowIndex = row;
      }
      cbFunc(cbContext);
    }
  }

  desc.IndexNodeSizeKB(nodeSizeKB);
  desc.IndexUnitsCount(1);

  MakeHeaderPersistent();

  assert(mvIndexNodeMgrs[field] == nullptr);

  mvIndexNodeMgrs[field] = nodeMgr.release();
}


void
PrototypeTable::RemoveIndex(const FIELD_INDEX field)
{
  if (field >= mFieldsCount)
  {
    throw DBSException(_EXTRA(DBSException::FIELD_NOT_FOUND),
                       "Table field index is invalid %u(%u),",
                       field,
                       mFieldsCount);
  }

  assert(mvIndexNodeMgrs.size() == mFieldsCount);

  LockRAII<Lock> syncHolder(mRowsSync);

  FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  if (mvIndexNodeMgrs[field] == nullptr)
    throw DBSException(_EXTRA(DBSException::FIELD_NOT_INDEXED));

  AcquireFieldIndex( &desc);
  assert(desc.IndexNodeSizeKB() > 0);
  assert(desc.IndexUnitsCount() > 0);

  desc.IndexNodeSizeKB(0);
  desc.IndexUnitsCount(0);

  unique_ptr<FieldIndexNodeManager> fieldMgr(mvIndexNodeMgrs[field]);
  fieldMgr->MarkForRemoval();

  mvIndexNodeMgrs[field] = nullptr;
  ReleaseIndexField( &desc);

  MakeHeaderPersistent();
}


bool
PrototypeTable::IsIndexed(const FIELD_INDEX field) const
{
  LockRAII<Lock> syncHolder(_CC(Lock&, mRowsSync));

  if (field >= mFieldsCount)
  {
    throw DBSException(_EXTRA(DBSException::FIELD_NOT_FOUND),
                       "Table field index is invalid %u(%u),",
                       field,
                       mFieldsCount);
  }

  assert(mvIndexNodeMgrs.size() == mFieldsCount);

  return mvIndexNodeMgrs[field] != nullptr;
}


uint_t
PrototypeTable::RowSize() const
{
  return mRowSize;
}


FieldDescriptor&
PrototypeTable::GetFieldDescriptorInternal(const FIELD_INDEX field) const
{
  const auto desc = _RC(FieldDescriptor*, mFieldsDescriptors.get());

  if (field >= mFieldsCount)
  {
    throw DBSException(_EXTRA(DBSException::FIELD_NOT_FOUND),
                       "Cannot get table field %u(of %u) description.",
                       field,
                       mFieldsCount);
  }

  return desc[field];
}


uint64_t
PrototypeTable::NodeRawSize() const
{
  return TableRmNode::RAW_NODE_SIZE;
}


NODE_INDEX PrototypeTable::AllocateNode(const NODE_INDEX parent, const KEY_INDEX parentKey)
{
  NODE_INDEX nodeIndex = mUnallocatedHead;

  assert((parent == NIL_NODE) || mRowModified);

  if (nodeIndex != NIL_NODE)
  {
    auto freeNode = RetrieveNode(nodeIndex);

    mUnallocatedHead = freeNode->Next();
    MakeHeaderPersistent();
  }
  else
  {
    assert(TableContainer().Size() % NodeRawSize() == 0);

    nodeIndex = TableContainer().Size() / NodeRawSize();
  }

  if (parent != NIL_NODE)
  {
    auto parentNode = RetrieveNode(parent);
    parentNode->SetNodeOfKey(parentKey, nodeIndex);

    assert(parentNode->IsLeaf() == false);
  }

  return nodeIndex;
}


void
PrototypeTable::FreeNode(const NODE_INDEX nodeId)
{
  assert(mRowModified);

  auto node = RetrieveNode(nodeId);

  node->MarkAsRemoved();
  node->Next(mUnallocatedHead);

  mUnallocatedHead = node->NodeId();

  MakeHeaderPersistent();
}


NODE_INDEX
PrototypeTable::RootNodeId()
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
PrototypeTable::RootNodeId(const NODE_INDEX node)
{
  mRootNode = node;

  MakeHeaderPersistent();
}


uint_t
PrototypeTable::MaxCachedNodes()
{
  return 128;
}


shared_ptr<IBTreeNode>
PrototypeTable::LoadNode(const NODE_INDEX nodeId)
{
  shared_ptr<IBTreeNode> node(shared_make(TableRmNode, *this, nodeId));

  assert(TableContainer().Size() % NodeRawSize() == 0);

  if (TableContainer().Size() > nodeId * NodeRawSize())
    TableContainer().Read(node->NodeId() * NodeRawSize(), NodeRawSize(), node->RawData());

  else
  {
    MarkRowModification();

    //Reserve space for this node
    assert(TableContainer().Size() == (nodeId * NodeRawSize()));

    TableContainer().Write(TableContainer().Size(), NodeRawSize(), node->RawData());
  }

  node->MarkClean();
  assert(node->NodeId() == nodeId);

  return node;
}


void
PrototypeTable::SaveNode(IBTreeNode* const node)
{
  if (node->IsDirty() == false)
    return;

  assert(mRowModified);

  TableContainer().Write(node->NodeId() * NodeRawSize(), NodeRawSize(), node->RawData());
  node->MarkClean();
}


void
PrototypeTable::StoreItems(uint64_t firstItem, uint_t itemsCount, const uint8_t* const from)
{
  assert(mRowModified);

  if (itemsCount + firstItem > mRowsCount)
    itemsCount = mRowsCount - firstItem;

  RowsContainer().Write(firstItem * mRowSize, itemsCount * mRowSize, from);
}


void
PrototypeTable::RetrieveItems(uint64_t firstItem, uint_t itemsCount, uint8_t* const to)
{
  if (itemsCount + firstItem > mRowsCount)
    itemsCount = mRowsCount - firstItem;

  RowsContainer().Read(firstItem * mRowSize, itemsCount * mRowSize, to);
}


void
PrototypeTable::CheckRowToDelete(const ROW_INDEX row)
{
  assert(mRowModified);

  bool allFieldsNull = true;

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  uint8_t* const rowData = cachedItem.GetDataForUpdate();

  for (FIELD_INDEX index = 0; index < mFieldsCount; index += 8)
  {
    const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal(index);
    const uint8_t bitsSet = ~0;

    if (rowData[fieldDesc.NullBitIndex() / 8] != bitsSet)
    {
      allFieldsNull = false;
      break;
    }
  }

  if (allFieldsNull)
  {
    NODE_INDEX dummyNode;
    KEY_INDEX dummyKey;
    BTree removedNodes( *this);
    TableRmKey key(row);

    removedNodes.InsertKey(key, &dummyNode, &dummyKey);
  }
}


void
PrototypeTable::CheckRowToReuse(const ROW_INDEX row)
{
  assert(mRowModified);

  bool allFieldsNull = true;

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  uint8_t* const rowData = cachedItem.GetDataForUpdate();

  for (FIELD_INDEX index = 0; index < mFieldsCount; index += 8)
  {
    const FieldDescriptor& fieldDesc = GetFieldDescriptorInternal(index);
    const uint8_t bitsSet = ~0;

    if (rowData[fieldDesc.NullBitIndex() / 8] != bitsSet)
    {
      allFieldsNull = false;
      break;
    }
  }

  if (allFieldsNull)
  {
    BTree removedNodes( *this);
    TableRmKey key(row);

    removedNodes.RemoveKey(key);
  }
}


void
PrototypeTable::AcquireFieldIndex(FieldDescriptor* const field)
{
  while (true)
  {
    LockRAII<Lock> syncHolder(mIndexesSync);

    if ( !field->IsAcquired())
    {
      field->Acquire();
      break;
    }

    syncHolder.Release();
    wh_yield();
  }
}


void
PrototypeTable::ReleaseIndexField(FieldDescriptor* const field)
{
  //Do not try to grab the lock when you release the field.
  assert(field->IsAcquired());

  field->Release();
}


template <class T> void
PrototypeTable::StoreEntry(const ROW_INDEX row,
                           const FIELD_INDEX field,
                           const bool threadSafe,
                           const T& value)
{
  T currentValue;

  MarkRowModification();
  LockRAII<Lock> syncHolder(mRowsSync, !threadSafe);
  if (row == mRowsCount)
    AddRow(true);

  else
    RetrieveEntry(row, field, false, currentValue);

  if (currentValue == value)
    return; //Nothing to change

  const uint8_t bitsSet = ~0;
  FieldDescriptor& desc = GetFieldDescriptorInternal(field);
  const uint_t byteOff = desc.NullBitIndex() / 8;
  const uint8_t bitOff = desc.NullBitIndex() % 8;

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  uint8_t * const rowData = cachedItem.GetDataForUpdate();

  if (value.IsNull())
  {
    assert((rowData[byteOff] & (1 << bitOff)) == 0);

    rowData[byteOff] |= (1 << bitOff);

    if (rowData[byteOff] == bitsSet)
      CheckRowToDelete(row);
  }
  else
  {
    if (rowData[byteOff] == bitsSet)
      CheckRowToReuse(row);

    rowData[byteOff] &= ~(1 << bitOff);

    Serializer::Store(rowData + desc.RowDataOff(), value);
  }

  //Update the field index if it exists
  if (mvIndexNodeMgrs[field] != nullptr)
  {
    NODE_INDEX dummyNode;
    KEY_INDEX dummyKey;

    if (threadSafe)
    {
      AcquireFieldIndex( &desc);
      syncHolder.Release();
    }

    try
    {
      BTree fieldIndexTree( *mvIndexNodeMgrs[field]);

      fieldIndexTree.RemoveKey(T_BTreeKey<T>(currentValue, row));
      fieldIndexTree.InsertKey(T_BTreeKey<T>(value, row), &dummyNode, &dummyKey);
    }
    catch (...)
    {
      ReleaseIndexField( &desc);
      throw;
    }

    if (threadSafe)
    {
      ReleaseIndexField( &desc);
      syncHolder.Acquire();
    }
  }
}


template<> void
PrototypeTable::StoreEntry(const ROW_INDEX        row,
                           const FIELD_INDEX      field,
                           const bool             threadSafe,
                           const DText&           value)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  if (IS_ARRAY(desc.Type()) || (GET_BASIC_TYPE(desc.Type()) != T_TEXT))
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID));
  }
  else if (row > mRowsCount)
    throw DBSException(_EXTRA(DBSException::ROW_NOT_ALLOCATED));

  assert(Serializer::Size(T_TEXT, false) == 2 * sizeof(uint64_t));

  MarkRowModification();
  LockRAII<Lock> syncHolder(mRowsSync, !threadSafe);

  DText::StrategyRAII sMgr = value.GetStrategyRAII();
  ITextStrategy& s = sMgr;
  LockRAII<Lock> _l(s.mLock);

  uint64_t newFirstEntry = ~0ull;
  uint64_t newFieldValueSize = 0;
  const bool skipVariableStore = s.Utf8CountU() < (2 * sizeof(uint64_t));

  if ( !skipVariableStore)
  {
    if (s.GetTemporalContainer().Size() == 0)
    {
      RowFieldText& r = _SC(RowFieldText&, s);
      newFieldValueSize = r.Utf8CountU() + RowFieldText::CACHE_META_DATA_SIZE;

      if ( &r.GetRowStorage() != &VSStore())
        newFirstEntry = VSStore().AddRecord(r.GetRowStorage(), r.mFirstEntry, 0, newFieldValueSize);

      else
      {
        r.GetRowStorage().IncrementRecordRef(r.mFirstEntry);
        newFirstEntry = r.mFirstEntry;
      }
    }
    else
    {
      if (s.Utf8CountU() >= RowFieldText::MAX_BYTES_COUNT)
      {
        throw DBSException(_EXTRA(DBSException::OPER_NOT_SUPPORTED),
                           "This implementation does not support text"
                           " fields with more than %lu characters.",
                           _SC(long, RowFieldText::MAX_BYTES_COUNT));
      }

      newFieldValueSize = s.Utf8CountU() + RowFieldText::CACHE_META_DATA_SIZE;

      assert(s.mCachedCharsCount <= RowFieldText::MAX_CHARS_COUNT);

      uint8_t headerData[RowFieldText::CACHE_META_DATA_SIZE];

      store_le_int32(s.mCachedCharsCount, headerData);
      store_le_int32(s.mCachedCharIndex, headerData + sizeof(uint32_t));
      store_le_int32(s.mCachedCharIndexOffset, headerData + 2 * sizeof(uint32_t));

      newFirstEntry = VSStore().AddRecord(headerData, sizeof headerData);
      VSStore().UpdateRecord(newFirstEntry,
                             sizeof headerData,
                             s.GetTemporalContainer(),
                             0,
                             s.Utf8CountU());
    }
  }

  if (row == mRowsCount)
    AddRow(true);

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  uint8_t* const rowData = cachedItem.GetDataForUpdate();
  const uint8_t bitsSet = ~0;
  bool fieldValueWasNull = false;

  uint8_t* const fieldFirstEntry = rowData + desc.RowDataOff();
  uint8_t* const fieldValueSize = rowData + desc.RowDataOff() + sizeof(uint64_t);

  const uint_t byteOff = desc.NullBitIndex() / 8;
  const uint8_t bitOff = desc.NullBitIndex() % 8;

  if ((rowData[byteOff] & (1 << bitOff)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && (s.mCachedCharsCount == 0))
    return;

  else if ((fieldValueWasNull == false) && (s.mCachedCharsCount == 0))
  {
    rowData[byteOff] |= (1 << bitOff);

    if (rowData[byteOff] == bitsSet)
      CheckRowToDelete(row);
  }
  else if (s.mCachedCharsCount != 0)
  {
    if (rowData[byteOff] == bitsSet)
    {
      assert(fieldValueWasNull == true);

      CheckRowToReuse(row);
    }
    rowData[byteOff] &= ~(1 << bitOff);
  }

  if ((fieldValueWasNull == false)
      && ((load_le_int64(fieldValueSize) & 0x8000000000000000ull) == 0))
  {
    VSStore().DecrementRecordRef(load_le_int64(fieldFirstEntry));
  }

  if (skipVariableStore)
  {
    assert(s.Utf8CountU() < _SC(uint_t, Serializer::Size(T_TEXT, false)));

    fieldValueSize[sizeof(uint64_t) - 1] = s.Utf8CountU();
    fieldValueSize[sizeof(uint64_t) - 1] |= 0x80;

    s.ReadUtf8U(0, s.Utf8CountU(), rowData + desc.RowDataOff());
  }
  else
  {
    store_le_int64(newFieldValueSize, fieldValueSize);
    store_le_int64(newFirstEntry, fieldFirstEntry);
  }
}


template<> void
PrototypeTable::StoreEntry(const ROW_INDEX        row,
                           const FIELD_INDEX      field,
                           const bool             threadSafe,
                           const DArray&          value)
{
  const FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  MarkRowModification();
  LockRAII<Lock> syncHolder(mRowsSync, !threadSafe);

  auto s = value.GetStrategy();
  LockRAII<Lock> _l(s->mLock);

  if ( ! IS_ARRAY(desc.Type())
      || (GET_BASIC_TYPE(desc.Type()) != s->Type() && s->Count() != 0))
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID));
  }
  else if (row > mRowsCount)
    throw DBSException(_EXTRA(DBSException::ROW_NOT_ALLOCATED));

  uint64_t newFirstEntry = ~0;
  uint64_t newFieldValueSize = 0;
  const uint8_t bitsSet = ~0;
  bool fieldValueWasNull = false;
  const bool skipVariableStore = s->RawSize() < (2 * sizeof(uint64_t));

  if ( !skipVariableStore)
  {
    if (s->GetTemporalContainer().Size() == 0)
    {
      VariableSizeStore& arrayStore = s->GetRowStorage();
      const uint64_t arrayFirstEntry = _SC(RowFieldArray&, *s).mFirstRecordEntry;
      if ( &arrayStore == &VSStore())
      {
        arrayStore.IncrementRecordRef(arrayFirstEntry);

        newFieldValueSize = s->RawSize() + RowFieldArray::METADATA_SIZE;
        newFirstEntry = arrayFirstEntry;
      }
      else
      {
        newFieldValueSize = s->RawSize() + RowFieldArray::METADATA_SIZE;
        newFirstEntry = VSStore().AddRecord(arrayStore, arrayFirstEntry, 0, newFieldValueSize);
      }
    }
    else
    {
      newFieldValueSize = s->RawSize();

      uint8_t elemsCount[RowFieldArray::METADATA_SIZE];
      store_le_int64(s->Count(), elemsCount);

      newFirstEntry = VSStore().AddRecord(elemsCount, sizeof elemsCount);
      VSStore().UpdateRecord(newFirstEntry,
                             sizeof elemsCount,
                             s->GetTemporalContainer(),
                             0,
                             newFieldValueSize);
      newFieldValueSize += sizeof elemsCount;
    }
  }

  if (row == mRowsCount)
    AddRow(true);

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  uint8_t * const rowData = cachedItem.GetDataForUpdate();

  uint8_t * const fieldFirstEntry = rowData + desc.RowDataOff();
  uint8_t * const fieldValueSize = rowData + desc.RowDataOff() + sizeof(uint64_t);

  const uint_t byteOff = desc.NullBitIndex() / 8;
  const uint8_t bitOff = desc.NullBitIndex() % 8;

  if ((rowData[byteOff] & (1 << bitOff)) != 0)
    fieldValueWasNull = true;

  if (fieldValueWasNull && (s->Count() == 0))
    return;

  else if ((fieldValueWasNull == false) && (s->Count() == 0))
  {
    rowData[byteOff] |= (1 << bitOff);

    if (rowData[byteOff] == bitsSet)
      CheckRowToDelete(row);
  }
  else if (s->Count() != 0)
  {
    if (rowData[byteOff] == bitsSet)
    {
      assert(fieldValueWasNull == true);

      CheckRowToReuse(row);
    }
    rowData[byteOff] &= ~(1 << bitOff);
  }

  if ((fieldValueWasNull == false)
      && ((load_le_int64(fieldValueSize) & 0x8000000000000000ull) == 0))
  {
    VSStore().DecrementRecordRef(load_le_int64(fieldFirstEntry));
  }

  if (skipVariableStore)
  {
    assert(s->RawSize() < _SC(uint_t, Serializer::Size(T_HIRESTIME, true)));

    fieldValueSize[sizeof(uint64_t) - 1] = s->RawSize();
    fieldValueSize[sizeof(uint64_t) - 1] |= 0x80;

    s->RawRead(0, s->RawSize(), rowData + desc.RowDataOff());
  }
  else
  {
    store_le_int64(newFieldValueSize, fieldValueSize);
    store_le_int64(newFirstEntry, fieldFirstEntry);
  }
}


void
PrototypeTable::Set(const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DBool&          value,
                    const bool            skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX      row,
                    const FIELD_INDEX    field,
                    const DChar&         value,
                    const bool           skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DDate&          value,
                    const bool            skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DDateTime&      value,
                    const bool            skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    const DHiresTime&      value,
                    const bool             skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    const DInt8&           value,
                    const bool             skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    const DInt16&          value,
                    const bool             skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    const DInt32&          value,
                    const bool             skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    const DInt64&          value,
                    const bool             skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    const DReal&           value,
                    const bool             skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    const DRichReal&       value,
                    const bool             skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX       row,
                    const FIELD_INDEX     field,
                    const DUInt8&         value,
                    const bool            skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX      row,
                    const FIELD_INDEX    field,
                    const DUInt16&       value,
                    const bool           skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX      row,
                    const FIELD_INDEX    field,
                    const DUInt32&       value,
                    const bool           skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX      row,
                    const FIELD_INDEX    field,
                    const DUInt64&       value,
                    const bool           skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


void
PrototypeTable::Set(const ROW_INDEX      row,
                    const FIELD_INDEX    field,
                    const DText&         value,
                    const bool           skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}

void
PrototypeTable::Set(const ROW_INDEX      row,
                    const FIELD_INDEX    field,
                    const DArray&        value,
                    const bool           skipThreadSafety)
{
  StoreEntry(row, field, ! skipThreadSafety, value);
}


static ITextStrategy*
allocate_row_field_text(VariableSizeStore&   store,
                        const uint64_t       firstRecordEntry,
                        const uint64_t       valueSize)
{
  return new RowFieldText(store, firstRecordEntry, valueSize);
}


static shared_ptr<IArrayStrategy>
allocate_row_field_array(VariableSizeStore&        store,
                         const uint64_t            firstRecordEntry,
                         const DBS_FIELD_TYPE      type)
{
  shared_ptr<IArrayStrategy> r = shared_make(RowFieldArray, store, firstRecordEntry, type);
  r->SetSelfReference(r);
  return r;
}


template <class T> void
PrototypeTable::RetrieveEntry(const ROW_INDEX   row,
                              const FIELD_INDEX field,
                              const bool        threadSafe,
                              T&                outValue)
{
  LockRAII<Lock> syncHolder(mRowsSync, !threadSafe);

  const FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  if ((desc.Type() & PS_TABLE_ARRAY_MASK)
      || ((desc.Type() & PS_TABLE_FIELD_TYPE_MASK) != _SC(uint_t, outValue.DBSType())))
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID));
  }

  const uint_t byteOff = desc.NullBitIndex() / 8;
  const uint8_t bitOff = desc.NullBitIndex() % 8;

  if (row >= mRowsCount)
    throw DBSException(_EXTRA(DBSException::ROW_NOT_ALLOCATED));

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  const uint8_t* const rowData = cachedItem.GetDataForRead();

  if (rowData[byteOff] & (1 << bitOff))
    outValue = T();

  else
  {
    outValue.~T();
    Serializer::Load(rowData + desc.RowDataOff(), &outValue);
  }
}


template <> void
PrototypeTable::RetrieveEntry(const ROW_INDEX   row,
                              const FIELD_INDEX field,
                              const bool        threadSafe,
                              DText&            outValue)
{
  LockRAII<Lock> syncHolder(mRowsSync, !threadSafe);

  const FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  if (row >= mRowsCount)
    throw DBSException(_EXTRA(DBSException::ROW_NOT_ALLOCATED));

  if (IS_ARRAY(desc.Type()) || (GET_BASIC_TYPE(desc.Type()) != T_TEXT))
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID));
  }

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  const uint8_t* const rowData = cachedItem.GetDataForRead();

  const uint64_t fieldValueSize = load_le_int64(rowData + desc.RowDataOff() + sizeof(uint64_t));
  const uint_t byteOff = desc.NullBitIndex() / 8;
  const uint8_t bitOff = desc.NullBitIndex() % 8;

  if (rowData[byteOff] & (1 << bitOff))
    outValue = DText();

  else if ((fieldValueSize & 0x8000000000000000ull) != 0)
  {
    assert(((fieldValueSize >> 56) & 0x7F) > 0);
    assert(((fieldValueSize >> 56) & 0x7F) < Serializer::Size(T_TEXT, false));

    outValue = DText(rowData + desc.RowDataOff(), (fieldValueSize >> 56) & 0x7F);
  }
  else
  {
    const uint64_t fieldFirstEntry = load_le_int64(rowData + desc.RowDataOff());
    outValue = DText( *allocate_row_field_text(VSStore(), fieldFirstEntry, fieldValueSize));
  }
}


template <> void
PrototypeTable::RetrieveEntry(const ROW_INDEX   row,
                              const FIELD_INDEX field,
                              const bool        threadSafe,
                              DArray&           outValue)
{
  LockRAII<Lock> syncHolder(mRowsSync, !threadSafe);

  const FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  if (row >= mRowsCount)
    throw DBSException(_EXTRA(DBSException::ROW_NOT_ALLOCATED));

  if ( !IS_ARRAY(desc.Type())
      || ((GET_BASIC_TYPE(desc.Type()) != outValue.Type()) && !outValue.IsNull()))
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID));
  }

  StoredItem cachedItem = mRowCache.RetriveItem(row);
  const uint8_t * const rowData = cachedItem.GetDataForRead();

  const uint64_t fieldValueSize = load_le_int64(rowData + desc.RowDataOff() + sizeof(uint64_t));

  const uint_t byteOff = desc.NullBitIndex() / 8;
  const uint8_t bitOff = desc.NullBitIndex() % 8;

  if (rowData[byteOff] & (1 << bitOff))
  {
    switch (GET_BASIC_TYPE(desc.Type()))
    {
    case T_BOOL:
      outValue = DArray(_SC(DBool *, nullptr));
      break;

    case T_CHAR:
      outValue = DArray(_SC(DChar *, nullptr));
      break;

    case T_DATE:
      outValue = DArray(_SC(DDate *, nullptr));
      break;

    case T_DATETIME:
      outValue = DArray(_SC(DDateTime *, nullptr));
      break;

    case T_HIRESTIME:
      outValue = DArray(_SC(DHiresTime *, nullptr));
      break;

    case T_UINT8:
      outValue = DArray(_SC(DUInt8 *, nullptr));
      break;

    case T_UINT16:
      outValue = DArray(_SC(DUInt16 *, nullptr));
      break;

    case T_UINT32:
      outValue = DArray(_SC(DUInt32 *, nullptr));
      break;

    case T_UINT64:
      outValue = DArray(_SC(DUInt64 *, nullptr));
      break;

    case T_REAL:
      outValue = DArray(_SC(DReal *, nullptr));
      break;

    case T_RICHREAL:
      outValue = DArray(_SC(DRichReal *, nullptr));
      break;

    case T_INT8:
      outValue = DArray(_SC(DInt8 *, nullptr));
      break;

    case T_INT16:
      outValue = DArray(_SC(DInt16 *, nullptr));
      break;

    case T_INT32:
      outValue = DArray(_SC(DInt32 *, nullptr));
      break;

    case T_INT64:
      outValue = DArray(_SC(DInt64 *, nullptr));
      break;

    default:
      assert(false);
    }
  }
  else if ((fieldValueSize & 0x8000000000000000ull) != 0)
  {
    shared_ptr<IArrayStrategy> strategy = shared_make(TemporalArray, GET_BASIC_TYPE(desc.Type()));
    strategy->SetSelfReference(strategy);

    assert(((fieldValueSize >> 56) & 0x7F) > 0);
    assert(((fieldValueSize >> 56) & 0x7F) < Serializer::Size(T_BOOL, true));

    const uint64_t size = (fieldValueSize >> 56) & 0x7F;
    strategy->RawWrite(0, size, rowData + desc.RowDataOff());
    strategy->mElementsCount = size / strategy->mElementRawSize;
    outValue = DArray(strategy);
  }
  else
  {
    const uint64_t fieldFirstEntry = load_le_int64(rowData + desc.RowDataOff());
    outValue = DArray(allocate_row_field_array(VSStore(),
                                               fieldFirstEntry,
                                               _SC(DBS_FIELD_TYPE,
                                                   desc.Type() & PS_TABLE_FIELD_TYPE_MASK)));
  }
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DBool&                 outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX      row,
                    const FIELD_INDEX    field,
                    DChar&               outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DDate&                 outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DDateTime&             outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DHiresTime&            outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DInt8&                 outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DInt16&                outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DInt32&                outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DInt64&                outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DReal&                 outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DRichReal&             outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DUInt8&                outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DUInt16&               outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DUInt32&               outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DUInt64&               outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DText&                 outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


void
PrototypeTable::Get(const ROW_INDEX        row,
                    const FIELD_INDEX      field,
                    DArray&                outValue,
                    const bool             skipThreadSafety)
{
  RetrieveEntry(row, field, ! skipThreadSafety, outValue);
}


template<typename T> void
PrototypeTable::table_exchange_rows(const FIELD_INDEX field,
                                    const ROW_INDEX row1,
                                    const ROW_INDEX row2)
{
  T row1Value, row2Value;

  assert(row1 < AllocatedRows());
  assert(row2 < AllocatedRows());

  Get(row1, field, row1Value, true);
  Get(row2, field, row2Value, true);

  Set(row1, field, row2Value, true);
  Set(row2, field, row1Value, true);
}


void
PrototypeTable::ExchangeRows(const ROW_INDEX    row1,
                             const ROW_INDEX    row2,
                             const bool         skipTthreadSafety)
{
  LockRAII<Lock> _l(mRowsSync, skipTthreadSafety);

  const ROW_INDEX allocatedRows = AllocatedRows();
  const FIELD_INDEX fieldsCount = FieldsCount();

  if ((allocatedRows <= row1) || (allocatedRows <= row2))
    throw DBSException(_EXTRA(DBSException::ROW_NOT_ALLOCATED));

  for (FIELD_INDEX field = 0; field < fieldsCount; ++field)
  {
    const DBSFieldDescriptor fieldDesc = DescribeField(field);

    if (fieldDesc.isArray)
      table_exchange_rows<DArray>(field, row1, row2);

    else
    {
      switch (fieldDesc.type)
      {
      case T_BOOL:
        table_exchange_rows<DBool>(field, row1, row2);
        break;

      case T_CHAR:
        table_exchange_rows<DChar>(field, row1, row2);
        break;

      case T_DATE:
        table_exchange_rows<DDate>(field, row1, row2);
        break;

      case T_DATETIME:
        table_exchange_rows<DDateTime>(field, row1, row2);
        break;

      case T_HIRESTIME:
        table_exchange_rows<DHiresTime>(field, row1, row2);
        break;

      case T_INT8:
        table_exchange_rows<DInt8>(field, row1, row2);
        break;

      case T_INT16:
        table_exchange_rows<DInt16>(field, row1, row2);
        break;

      case T_INT32:
        table_exchange_rows<DInt32>(field, row1, row2);
        break;

      case T_INT64:
        table_exchange_rows<DInt64>(field, row1, row2);
        break;

      case T_UINT8:
        table_exchange_rows<DUInt8>(field, row1, row2);
        break;

      case T_UINT16:
        table_exchange_rows<DUInt16>(field, row1, row2);
        break;

      case T_UINT32:
        table_exchange_rows<DUInt32>(field, row1, row2);
        break;

      case T_UINT64:
        table_exchange_rows<DUInt64>(field, row1, row2);
        break;

      case T_REAL:
        table_exchange_rows<DReal>(field, row1, row2);
        break;

      case T_RICHREAL:
        table_exchange_rows<DRichReal>(field, row1, row2);
        break;

      case T_TEXT:
        table_exchange_rows<DText>(field, row1, row2);
        break;

      default:
        throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
      }
    }
  }
}

template<typename TF> class
SortTableContainer
{
public:
  SortTableContainer(PrototypeTable& table, const FIELD_INDEX field)
    : mTable(table),
      mField(field)
  {
    mRowsPermutation.resize(table.AllocatedRows());
    for (ROW_INDEX i = 0; i < mRowsPermutation.size(); ++i)
      mRowsPermutation[i] = i;
  }

  ~SortTableContainer()
  {
    ROW_INDEX row = 0;
    while (row < mRowsPermutation.size())
    {
      if (mRowsPermutation[row] == row)
      {
        ++row;
        continue;
      }

      ROW_INDEX currentRow = row;
      do
      {
        const ROW_INDEX correctRow = mRowsPermutation[currentRow];
        if (correctRow == row)
        {
          mRowsPermutation[currentRow] = currentRow;
          break;
        }

        mTable.ExchangeRows(currentRow, correctRow, true);
        mRowsPermutation[currentRow] = currentRow;

        currentRow = correctRow;
      } while (true);
    }
  }
  const TF operator[] (const int64_t index) const
  {
    TF value;
    mTable.Get(mRowsPermutation[index], mField, value, true);

    return value;
  }
  void Exchange(const int64_t pos1, const int64_t pos2)
  {
    const ROW_INDEX t      = mRowsPermutation[pos1];
    mRowsPermutation[pos1] = mRowsPermutation[pos2];
    mRowsPermutation[pos2] = t;
  }

  uint64_t Count() const { return mRowsPermutation.size(); }
  void Pivot(const uint64_t index) { mTable.Get(mRowsPermutation[index], mField, mPivot, true); }
  const TF& Pivot() const { return mPivot; }

private:
  PrototypeTable&           mTable;
  std::vector<ROW_INDEX>    mRowsPermutation;
  const FIELD_INDEX         mField;
  TF                        mPivot;
};


void
PrototypeTable::Sort(const FIELD_INDEX     field,
                     const ROW_INDEX       fromRow,
                     const ROW_INDEX       toRow,
                     const bool            reverse)
{
  const DBSFieldDescriptor fd = DescribeField(field);

  if (fd.isArray)
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID),
                       "This implementation does not sort array fields.");
  }

  LockRAII<Lock> _l(mRowsSync);

  const ROW_INDEX from = MIN(fromRow, toRow);
  const ROW_INDEX to = MIN(MAX(fromRow, toRow), AllocatedRows() - 1);

  if (from > to)
    throw DBSException(_EXTRA(DBSException::INVALID_PARAMETERS));

  else if (from == to)
    return;

  switch (fd.type)
  {
  case T_BOOL:
  {
    SortTableContainer<DBool> temp( *this, field);
    quick_sort<DBool, SortTableContainer<DBool> >(from, to, reverse, temp);
  }
    break;

  case T_CHAR:
  {
    SortTableContainer<DChar> temp( *this, field);
    quick_sort<DChar, SortTableContainer<DChar> >(from, to, reverse, temp);
  }
    break;

  case T_DATE:
  {
    SortTableContainer<DDate> temp( *this, field);
    quick_sort<DDate, SortTableContainer<DDate> >(from, to, reverse, temp);
  }
    break;

  case T_DATETIME:
  {
    SortTableContainer<DDateTime> temp( *this, field);
    quick_sort<DDateTime, SortTableContainer<DDateTime> >(from, to, reverse, temp);
  }
    break;

  case T_HIRESTIME:
  {
    SortTableContainer<DHiresTime> temp( *this, field);
    quick_sort<DHiresTime, SortTableContainer<DHiresTime> >(from, to, reverse, temp);
  }
    break;

  case T_UINT8:
  {
    SortTableContainer<DUInt8> temp( *this, field);
    quick_sort<DUInt8, SortTableContainer<DUInt8> >(from, to, reverse, temp);
  }
    break;

  case T_UINT16:
  {
    SortTableContainer<DUInt16> temp( *this, field);
    quick_sort<DUInt16, SortTableContainer<DUInt16> >(from, to, reverse, temp);
  }
    break;

  case T_UINT32:
  {
    SortTableContainer<DUInt32> temp( *this, field);
    quick_sort<DUInt32, SortTableContainer<DUInt32> >(from, to, reverse, temp);
  }
    break;

  case T_UINT64:
  {
    SortTableContainer<DUInt64> temp( *this, field);
    quick_sort<DUInt64, SortTableContainer<DUInt64> >(from, to, reverse, temp);
  }
    break;

  case T_REAL:
  {
    SortTableContainer<DReal> temp( *this, field);
    quick_sort<DReal, SortTableContainer<DReal> >(from, to, reverse, temp);
  }
    break;

  case T_RICHREAL:
  {
    SortTableContainer<DRichReal> temp( *this, field);
    quick_sort<DRichReal, SortTableContainer<DRichReal> >(from, to, reverse, temp);
  }
    break;

  case T_INT8:
  {
    SortTableContainer<DInt8> temp( *this, field);
    quick_sort<DInt8, SortTableContainer<DInt8> >(from, to, reverse, temp);
  }
    break;

  case T_INT16:
  {
    SortTableContainer<DInt16> temp( *this, field);
    quick_sort<DInt16, SortTableContainer<DInt16> >(from, to, reverse, temp);
  }
    break;

  case T_INT32:
  {
    SortTableContainer<DInt32> temp( *this, field);
    quick_sort<DInt32, SortTableContainer<DInt32> >(from, to, reverse, temp);
  }
    break;

  case T_INT64:
  {
    SortTableContainer<DInt64> temp( *this, field);
    quick_sort<DInt64, SortTableContainer<DInt64> >(from, to, reverse, temp);
  }
    break;
  case T_TEXT:
  {
    SortTableContainer<DText> temp( *this, field);
    quick_sort<DText, SortTableContainer<DText> >(from, to, reverse, temp);
  }
    break;

  default:
    throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
  }
}


DArray
PrototypeTable::MatchRows(const DBool&       min,
                          const DBool&       max,
                          const ROW_INDEX    fromRow,
                          const ROW_INDEX    toRow,
                          const FIELD_INDEX  field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DChar&         min,
                           const DChar&         max,
                           const ROW_INDEX      fromRow,
                           const ROW_INDEX      toRow,
                           const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DDate&         min,
                          const DDate&         max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DDateTime&     min,
                          const DDateTime&     max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DHiresTime&     min,
                          const DHiresTime&     max,
                          const ROW_INDEX       fromRow,
                          const ROW_INDEX       toRow,
                          const FIELD_INDEX     field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DUInt8&        min,
                          const DUInt8&        max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DUInt16&       min,
                          const DUInt16&       max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DUInt32&       min,
                          const DUInt32&       max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DUInt64&       min,
                          const DUInt64&       max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DInt8&         min,
                          const DInt8&         max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DInt16&        min,
                          const DInt16&        max,
                          const ROW_INDEX      fromRow,
                          const ROW_INDEX      toRow,
                          const FIELD_INDEX    field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DInt32&       min,
                          const DInt32&       max,
                          const ROW_INDEX     fromRow,
                          const ROW_INDEX     toRow,
                          const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DInt64&       min,
                          const DInt64&       max,
                          const ROW_INDEX     fromRow,
                          const ROW_INDEX     toRow,
                          const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DReal&        min,
                          const DReal&        max,
                          const ROW_INDEX     fromRow,
                          const ROW_INDEX     toRow,
                          const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


DArray
PrototypeTable::MatchRows(const DRichReal&    min,
                          const DRichReal&    max,
                          const ROW_INDEX     fromRow,
                          const ROW_INDEX     toRow,
                          const FIELD_INDEX   field)
{
  if (mvIndexNodeMgrs[field] != nullptr)
    return MatchRowsWithIndex(min, max, fromRow, toRow, field);

  return MatchRowsNoIndex(min, max, fromRow, toRow, field);
}


template <class T> DArray
PrototypeTable::MatchRowsWithIndex(const T&          min,
                                   const T&          max,
                                   const ROW_INDEX   fromRow,
                                   ROW_INDEX         toRow,
                                   const FIELD_INDEX field)
{
  DArray result;
  if (mRowsCount == 0)
    return result;

  FieldDescriptor& desc = GetFieldDescriptorInternal(field);

  if ((desc.Type() & PS_TABLE_ARRAY_MASK)
      || ((desc.Type() & PS_TABLE_FIELD_TYPE_MASK) != _SC(uint_t, min.DBSType())))
  {
    throw DBSException(_EXTRA(DBSException::FIELD_TYPE_INVALID));
  }

  toRow = MIN(toRow, mRowsCount - 1);

  FieldIndexNodeManager* const nodeMgr = mvIndexNodeMgrs[field];

  NODE_INDEX nodeId;
  KEY_INDEX keyIndex;
  const T_BTreeKey<T> firstKey(min, fromRow);
  const T_BTreeKey<T> lastKey(max, toRow);

  assert(nodeMgr != nullptr);

  AcquireFieldIndex( &desc);

  try
  {
    BTree fieldIndexTree( *nodeMgr);

    if ( !fieldIndexTree.FindBiggerOrEqual(firstKey, &nodeId, &keyIndex))
      goto force_return;

    auto currentNode = nodeMgr->RetrieveNode(nodeId);

    assert(keyIndex < currentNode->KeysCount());

    while (true)
    {
      IBTreeFieldIndexNode* node = _SC(IBTreeFieldIndexNode*, &*currentNode);
      KEY_INDEX toKey = ~0;

      bool lastNode = false;

      if (node->FindBiggerOrEqual(lastKey, &toKey))
      {
        lastNode = true;

        if (node->CompareKey(lastKey, toKey) < 0)
          toKey++;
      }
      else
        toKey = 0;

      node->GetRows(keyIndex, toKey, result);

      if (lastNode || (node->Next() == NIL_NODE))
        break;

      else
      {
        currentNode = nodeMgr->RetrieveNode(node->Next());

        assert(currentNode->KeysCount() > 0);

        keyIndex = currentNode->KeysCount() - 1;
      }
    }
  }
  catch (...)
  {
    ReleaseIndexField( &desc);

    throw;
  }

  force_return:

  ReleaseIndexField( &desc);

  return result;
}


template <class T> DArray
PrototypeTable::MatchRowsNoIndex(const T&          min,
                                 const T&          max,
                                 const ROW_INDEX   fromRow,
                                 ROW_INDEX         toRow,
                                 const FIELD_INDEX field)
{
  DArray result;

  if (mRowsCount == 0)
    return result;

  toRow = MIN(toRow, mRowsCount - 1);
  for (ROW_INDEX row = fromRow; row <= toRow; ++row)
  {
    T rowValue;
    Get(row, field, rowValue);

    if ((rowValue < min) || (max < rowValue))
      continue;

    result.Add(DROW_INDEX(row));
  }

  return result;
}


void
PrototypeTable::MarkRowModification()
{
  if ( ! mRowModified)
  {
    mRowModified = true;
    mDbs.NotifyDatabaseUpdate();
    MakeHeaderPersistent();
  }
}


void
PrototypeTable::FlushInternal()
{
  mRowCache.Flush();
  FlushNodes();

  for (int field = 0; field < mFieldsCount; ++field)
  {
    if (mvIndexNodeMgrs[field] == nullptr)
      continue;

    FieldDescriptor& fd = GetFieldDescriptorInternal(field);

    while (fd.IsAcquired())
      wh_yield();

    mvIndexNodeMgrs[field]->FlushNodes();
  }

  FlushEpilog();

  mRowModified = false;

  MakeHeaderPersistent();
}




TableRmNode::TableRmNode(PrototypeTable& table, const NODE_INDEX nodeId)
  : IBTreeNode(table, nodeId)
{
  assert((sizeof(NodeHeader) % ( 2 * sizeof(uint64_t)) == 0));
  assert(RAW_NODE_SIZE == mNodesMgr.NodeRawSize());
}


uint_t
TableRmNode::KeysPerNode() const
{
  assert(sizeof(NodeHeader) % sizeof(uint64_t) == 0);

  uint_t result = mNodesMgr.NodeRawSize() - sizeof(NodeHeader);

  result /= sizeof(ROW_INDEX) + (IsLeaf() ? 0 : sizeof (NODE_INDEX));

  return result;
}


KEY_INDEX
TableRmNode::GetParentKeyIndex(const IBTreeNode& parent) const
{
  assert(KeysCount() > 0);

  KEY_INDEX result;
  const ROW_INDEX* const keys = _RC(const ROW_INDEX*, DataForRead());

  const TableRmKey key(Serializer::LoadRow(keys));

  parent.FindBiggerOrEqual(key, &result);

  assert(parent.CompareKey(key, result) == 0);
  assert(NodeId() == parent.NodeIdOfKey(result));

  return result;
}


NODE_INDEX
TableRmNode::NodeIdOfKey(const KEY_INDEX keyIndex) const
{
  assert(keyIndex < KeysCount());
  assert(sizeof(NodeHeader) % sizeof(uint64_t) == 0);
  assert(IsLeaf() == false);

  const uint_t firstNodeOff = TableRmNode::KeysPerNode() * sizeof(ROW_INDEX);
  const auto nodes = _RC(const NODE_INDEX*, DataForRead() + firstNodeOff);
  return Serializer::LoadNode(nodes + keyIndex);
}


void
TableRmNode::AdjustKeyNode(const IBTreeNode& childNode, const KEY_INDEX keyIndex)
{
  assert(childNode.NodeId() == NodeIdOfKey(keyIndex));
  assert(IsLeaf() == false);

  const TableRmNode& child = _SC(const TableRmNode&, childNode);
  const auto keys = _RC(ROW_INDEX*, DataForWrite());
  const auto childKeys = _RC(const ROW_INDEX*, child.DataForRead());

  Serializer::StoreRow(Serializer::LoadRow(childKeys), keys + keyIndex);
}


void
TableRmNode::SetNodeOfKey(const KEY_INDEX keyIndex, const NODE_INDEX childNode)
{

  assert(keyIndex < KeysCount());
  assert(sizeof(NodeHeader) % sizeof(uint64_t) == 0);
  assert(IsLeaf() == false);

  const uint_t nodesOff = TableRmNode::KeysPerNode() * sizeof(ROW_INDEX);
  const auto nodes = _RC(NODE_INDEX*, DataForWrite() + nodesOff);

  Serializer::StoreNode(childNode, nodes + keyIndex);
}


KEY_INDEX
TableRmNode::InsertKey(const IBTreeKey& key)
{
  if (KeysCount() > 0)
  {
    KEY_INDEX keyIndex;
    KEY_INDEX lastKey = KeysCount() - 1;

    assert(lastKey < (KeysPerNode() - 1));

    if (FindBiggerOrEqual(key, &keyIndex) == false)
      keyIndex = 0;

    else
      keyIndex++;

    assert((keyIndex == 0) || (CompareKey(key, keyIndex - 1) < 0));

    const auto rows = _RC(ROW_INDEX*, DataForWrite());
    make_array_room(lastKey, keyIndex, sizeof(rows[0]), _RC(uint8_t*, rows));

    KeysCount(KeysCount() + 1);
    Serializer::StoreRow( *_SC(const TableRmKey*, &key), rows + keyIndex);
    if ( ! IsLeaf())
    {
      NODE_INDEX* const nodes = _RC(NODE_INDEX*, rows + KeysPerNode());
      make_array_room(lastKey, keyIndex, sizeof(NODE_INDEX), _RC(uint8_t*, nodes));
    }

    return keyIndex;
  }

  const auto rows = _RC(ROW_INDEX*, DataForWrite());
  Serializer::StoreRow( *_SC(const TableRmKey*, &key), rows);

  KeysCount(1);

  return 0;
}


void
TableRmNode::RemoveKey(const KEY_INDEX keyIndex)
{
  uint_t lastKey = KeysCount() - 1;

  assert(lastKey < (KeysPerNode() - 1));

  const auto rows = _RC(NODE_INDEX*, DataForWrite());
  remove_array_elemes(lastKey, keyIndex, sizeof(rows[0]), _RC(uint8_t*, rows));

  if (IsLeaf() == false)
  {
    NODE_INDEX* const nodes = _RC(NODE_INDEX*, rows + KeysPerNode());
    remove_array_elemes(lastKey, keyIndex, sizeof(nodes[0]), _RC(uint8_t*, nodes));
  }

  KeysCount(KeysCount() - 1);
}


void
TableRmNode::Split( const NODE_INDEX parentId)
{
  assert(NeedsSpliting());

  const auto rows = _RC(const ROW_INDEX*, DataForRead());
  const KEY_INDEX splitKey = KeysCount() / 2;

  auto parentNode = mNodesMgr.RetrieveNode(parentId);

  const TableRmKey key(Serializer::LoadRow(rows + splitKey));
  const KEY_INDEX insertionPos = parentNode->InsertKey(key);
  const NODE_INDEX allocatedNodeId = mNodesMgr.AllocateNode(parentId, insertionPos);

  auto allocatedNode = mNodesMgr.RetrieveNode(allocatedNodeId);

  allocatedNode->Leaf(IsLeaf());
  allocatedNode->MarkAsUsed();

  const auto newRows = _RC(ROW_INDEX*, allocatedNode->DataForWrite());
  for (uint_t index = splitKey; index < KeysCount(); ++index)
  {
    const ROW_INDEX r = Serializer::LoadRow(rows + index);
    Serializer::StoreRow(r, newRows + index - splitKey);
  }

  if ( ! IsLeaf())
  {
    const auto nodes = _RC(const NODE_INDEX*, rows + KeysPerNode());
    const auto newNodes = _RC(NODE_INDEX*, newRows + KeysPerNode());

    for (uint_t index = splitKey; index < KeysCount(); ++index)
    {
      const NODE_INDEX n = Serializer::LoadNode(nodes + index);
      Serializer::StoreNode(n, newNodes + index - splitKey);
    }
  }

  allocatedNode->KeysCount(KeysCount() - splitKey);
  KeysCount(splitKey);

  allocatedNode->Next(NodeId());
  allocatedNode->Prev(Prev());
  Prev(allocatedNodeId);
  if (allocatedNode->Prev() != NIL_NODE)
  {
    auto prevNode = mNodesMgr.RetrieveNode(allocatedNode->Prev());
    prevNode->Next(allocatedNodeId);
  }
}


void
TableRmNode::Join(const bool toRight)
{
  assert(NeedsJoining());

  const auto rows  = _RC(ROW_INDEX*,  DataForWrite());
  const auto nodes = _RC(NODE_INDEX*, rows + KeysPerNode());

  if (toRight)
  {
    assert(Next() != NIL_NODE);

    auto next = mNodesMgr.RetrieveNode(Next());

    const auto nextNode = _SC(TableRmNode*, next.get());
    const auto destRows = _RC(ROW_INDEX*, nextNode->DataForWrite());

    for (uint_t index = 0; index < KeysCount(); ++index)
    {
      const ROW_INDEX r = Serializer::LoadRow(rows + index);
      Serializer::StoreRow(r, destRows + index + nextNode->KeysCount());
    }

    if (IsLeaf() == false)
    {
      NODE_INDEX* const destNodes = _RC(NODE_INDEX*, destRows + KeysPerNode());

      for (uint_t index = 0; index < KeysCount(); ++index)
      {
        const NODE_INDEX n = Serializer::LoadNode(nodes + index);

        Serializer::StoreNode(n, destNodes + index + nextNode->KeysCount());
      }
    }

    nextNode->KeysCount(nextNode->KeysCount() + KeysCount());
    nextNode->Prev(Prev());

    if (Prev() != NIL_NODE)
      mNodesMgr.RetrieveNode(Prev())->Next(Next());
  }
  else
  {
    assert(Prev() != NIL_NODE);

    auto prev(mNodesMgr.RetrieveNode(Prev()));

    const auto prevNode = _SC(TableRmNode*, prev.get());
    const auto srcRows = _RC(ROW_INDEX*, prevNode->DataForWrite());

    for (uint_t index = 0; index < prevNode->KeysCount(); ++index)
    {
      const ROW_INDEX r = Serializer::LoadRow(srcRows + index);

      Serializer::StoreRow(r, rows + index + KeysCount());
    }

    if ( ! IsLeaf())
    {
      const auto srcNodes = _RC(NODE_INDEX*, srcRows + KeysPerNode());
      for (uint_t index = 0; index < prevNode->KeysCount(); ++index)
      {
        const NODE_INDEX n = Serializer::LoadNode(srcNodes + index);

        Serializer::StoreNode(n, nodes + index + KeysCount());
      }
    }

    KeysCount(KeysCount() + prevNode->KeysCount());
    Prev(prevNode->Prev());
    if (Prev() != NIL_NODE)
      mNodesMgr.RetrieveNode(Prev())->Next(NodeId());
  }
}


int TableRmNode::CompareKey(const IBTreeKey& key, const KEY_INDEX keyIndex) const
{
  auto rows = _RC(const ROW_INDEX*, DataForRead());
  const TableRmKey tKey( *_SC(const TableRmKey*, &key));
  const int64_t row = Serializer::LoadRow(rows + keyIndex);

  if (tKey < row)
    return -1;

  else if (tKey == row)
    return 0;

  return 1;
}

const IBTreeKey&
TableRmNode::SentinelKey() const
{
  static TableRmKey _key(~_SC(ROW_INDEX, 0));

  return _key;
}


} //namespace pastra
} //namespace whais
