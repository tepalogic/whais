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

#ifndef PS_TEMPLATETABLE_H_
#define PS_TEMPLATETABLE_H_

#include <assert.h>

#include "utils/wfile.h"
#include "utils/endianness.h"

#include "dbs_table.h"
#include "dbs_types.h"
#include "dbs_values.h"

#include "ps_dbsmgr.h"
#include "ps_container.h"
#include "ps_blockcache.h"
#include "ps_varstorage.h"
#include "ps_btree_fields.h"

namespace whais {
namespace pastra {

static const uint_t PS_TABLE_FIELD_TYPE_MASK = 0x00FF;
static const uint_t PS_TABLE_ARRAY_MASK      = 0x0100;



class FieldDescriptor
{
public:

  FieldDescriptor ()
  {
    NullBitIndex (0);
    RowDataOff (0);
    NameOffset (0);
    IndexNodeSizeKB (0);
    IndexUnitsCount (0);
    Type (0);
    mAcquired = 0;
  }

  uint_t NullBitIndex () const
  {
    return load_le_int16 (mNullBitIndex);
  }

  void NullBitIndex (const uint_t index)
  {
    store_le_int16 (index, mNullBitIndex);
  }

  uint_t RowDataOff () const
  {
    return load_le_int32 (mRowDataOff);
  }

  void RowDataOff (const uint_t off)
  {
    store_le_int32 (off, mRowDataOff);
  }

  uint_t NameOffset () const
  {
    return load_le_int32 (mNameOffset);
  }

  void NameOffset (const uint_t off)
  {
    store_le_int32 (off, mNameOffset);
  }

  uint_t Type () const
  {
    return load_le_int16 (mType);
  }

  void Type (const uint_t type)
  {
    store_le_int16 (type, mType);
  }

  bool IsAcquired () const
  {
    return mAcquired != 0;
  }

  void Acquire ()
  {
    assert (mAcquired == 0);

    mAcquired = 1;
  }

  void Release ()
  {
    assert (mAcquired > 0);

    mAcquired = 0;
  }

  uint_t IndexNodeSizeKB () const
  {
    return mIndexNodeSizeKB;
  }

  void IndexNodeSizeKB (const uint_t kb)
  {
    assert (kb <= 255);

    mIndexNodeSizeKB = kb;
  }

  uint_t IndexUnitsCount () const
  {
    return load_le_int16 (mIndexUnitsCount);
  }

  void IndexUnitsCount (const uint_t count)
  {
    store_le_int16 (count, mIndexUnitsCount);
  }

private:
  //TODO: Make sure you check no fields count bigger than 65535
  uint8_t  mNullBitIndex[2];
  uint8_t  mRowDataOff[4];
  uint8_t  mNameOffset[4];
  uint8_t  mType[2];
  uint8_t  mIndexUnitsCount[2];
  uint8_t  mAcquired;
  uint8_t  mIndexNodeSizeKB;
};


class PrototypeTable : public ITable,
                       public IBlocksManager,
                       public IBTreeNodeManager
{
public:
  PrototypeTable (DbsHandler& dbs);
  PrototypeTable (const PrototypeTable& prototype);

  virtual uint64_t NodeRawSize () const;
  virtual NODE_INDEX  AllocateNode (const NODE_INDEX parent,
                                    const KEY_INDEX  parentKey);
  virtual void FreeNode (const NODE_INDEX node);
  virtual NODE_INDEX RootNodeId ();
  virtual void RootNodeId (const NODE_INDEX node);

  virtual void StoreItems (uint64_t         firstItem,
                           uint_t           itemsCount,
                           const uint8_t*   from);
  virtual void RetrieveItems (uint64_t      firstItem,
                              uint_t        itemsCount,
                              uint8_t*      to);

  virtual FIELD_INDEX FieldsCount ();

  virtual FIELD_INDEX RetrieveField (const char* name);

  virtual DBSFieldDescriptor DescribeField (const FIELD_INDEX field);

  virtual ROW_INDEX AllocatedRows ();

  virtual ROW_INDEX AddRow (const bool skipThreadSafety = false);

  virtual ROW_INDEX GetReusableRow (const bool forceAdd);

  virtual ROW_INDEX ReusableRowsCount ();

  virtual void MarkRowForReuse (const ROW_INDEX row);

  virtual void CreateIndex (const FIELD_INDEX                 field,
                            CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                            CreateIndexCallbackContext* const cbContext);

  virtual void RemoveIndex (const FIELD_INDEX field);

  virtual bool IsIndexed (const FIELD_INDEX field) const;

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DBool&      value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DChar&      value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DDate&      value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DDateTime&  value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DHiresTime& value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt8&      value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt16&     value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt32&     value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DInt64&     value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DReal&      value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DRichReal&  value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt8&     value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt16&    value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt32&    value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DUInt64&    value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DText&      value,
                    const bool        skipThreadSafety = false);

  virtual void Set (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    const DArray&     value,
                    const bool        skipThreadSafety = false);


  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DBool&            outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DChar&            outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DDate&            outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DDateTime&        outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DHiresTime&       outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt8&            outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt16&           outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt32&           outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DInt64&           outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DReal&            outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DRichReal&        outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt8&           outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt16&          outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt32&          outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DUInt64&          outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DText&            outValue,
                    const bool        skipThreadSafety = false);

  virtual void Get (const ROW_INDEX   row,
                    const FIELD_INDEX field,
                    DArray&           outValue,
                    const bool        skipThreadSafety = false);

  virtual void ExchangeRows (const ROW_INDEX   row1,
                             const ROW_INDEX   row2,
                             const bool        skipThreadSafety = false);

  virtual void Sort (const FIELD_INDEX  field,
                     const ROW_INDEX    from,
                     const ROW_INDEX    to,
                     const bool         reverse);

  virtual DArray MatchRows (const DBool&          min,
                            const DBool&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DChar&          min,
                            const DChar&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DDate&          min,
                            const DDate&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DDateTime&      min,
                            const DDateTime&      max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DHiresTime&     min,
                            const DHiresTime&     max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt8&         min,
                            const DUInt8&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt16&        min,
                            const DUInt16&        max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt32&        min,
                            const DUInt32&        max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt64&        min,
                            const DUInt64&        max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt8&          min,
                            const DInt8&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt16&         min,
                            const DInt16&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt32&         min,
                            const DInt32&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt64&         min,
                            const DInt64&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DReal&          min,
                            const DReal&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DRichReal&      min,
                            const DRichReal&      max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const FIELD_INDEX     field);
  virtual void LockTable ();

  virtual void UnlockTable ();

  //Followings declarations shouldn't be public,
  //but kept here to ease the testing procedures.
  uint_t RowSize () const;

  FieldDescriptor& GetFieldDescriptorInternal(
                           const FIELD_INDEX fieldIndex
                                              ) const;

  virtual void Flush ();

  DbsHandler& GetDBSHandler ()
  {
    return mDbs;
  }

private:
  template<class T> void StoreEntry (const ROW_INDEX,
                                     const FIELD_INDEX,
                                     const bool,
                                     const T&);


  template<class T> void RetrieveEntry (const ROW_INDEX,
                                        const FIELD_INDEX,
                                        const bool,
                                        T&);

  template<typename T> void table_exchange_rows (const FIELD_INDEX   field,
                                                 const ROW_INDEX     row1,
                                                 const ROW_INDEX     row2);

  template<class T> DArray MatchRowsWithIndex (const T&          min,
                                               const T&          max,
                                               const ROW_INDEX   fromRow,
                                               ROW_INDEX         toRow,
                                               const FIELD_INDEX fieldIndex);

  template<class T> DArray MatchRowsNoIndex (const T&          min,
                                             const T&          max,
                                             const ROW_INDEX   fromRow,
                                             ROW_INDEX         toRow,
                                             const FIELD_INDEX filedIndex);
  void CheckRowToReuse (const ROW_INDEX row);

  void CheckRowToDelete (const ROW_INDEX row);

  void AcquireFieldIndex (FieldDescriptor* const field);

  void ReleaseIndexField (FieldDescriptor* const field);

  virtual uint_t MaxCachedNodes ();

  virtual IBTreeNode* LoadNode (const NODE_INDEX nodeId);

  virtual void SaveNode (IBTreeNode* const node);

protected:

  virtual void MakeHeaderPersistent () = 0;

  virtual IDataContainer* CreateIndexContainer (const FIELD_INDEX field) = 0;

  virtual IDataContainer& RowsContainer () = 0;

  virtual IDataContainer& TableContainer () = 0;

  virtual VariableSizeStore& VSStore () = 0;

  virtual void FlushEpilog () = 0;

  void MarkRowModification ();

  void FlushInternal ();

  //Data members
  DbsHandler&                           mDbs;
  ROW_INDEX                             mRowsCount;
  NODE_INDEX                            mRootNode;
  NODE_INDEX                            mUnallocatedHead;
  uint32_t                              mRowSize;
  uint32_t                              mDescriptorsSize;
  FIELD_INDEX                           mFieldsCount;
  std::unique_ptr<uint8_t>                mFieldsDescriptors;
  std::vector<FieldIndexNodeManager*>   mvIndexNodeMgrs;
  BlockCache                            mRowCache;
  Lock                                  mRowsSync;
  Lock                                  mIndexesSync;
  bool                                  mRowModified;
  bool                                  mLockInProgress;
};



class TableRmKey : public IBTreeKey
{
public:
  TableRmKey (const ROW_INDEX row)
    : mRow (row)
  {
  };

  operator ROW_INDEX () const
  {
    return mRow;
  }

private:
  const ROW_INDEX mRow;
};



class TableRmNode : public IBTreeNode
{
public:

  TableRmNode (PrototypeTable& table, const NODE_INDEX nodeId);

  virtual uint_t KeysPerNode () const;

  virtual KEY_INDEX GetParentKeyIndex (const IBTreeNode& parent) const;

  virtual NODE_INDEX NodeIdOfKey (const KEY_INDEX keyIndex) const;

  virtual void AdjustKeyNode (const IBTreeNode& childNode,
                              const KEY_INDEX   keyIndex);

  virtual void SetNodeOfKey (const KEY_INDEX  keyIndex,
                             const NODE_INDEX childNode);

  virtual KEY_INDEX InsertKey (const IBTreeKey& key);

  virtual void RemoveKey (const KEY_INDEX keyIndex);

  virtual void Split (const NODE_INDEX parentId);

  virtual void Join (const bool toRight);

  virtual int CompareKey (const IBTreeKey&      key,
                          const KEY_INDEX       nodeKeyIndex) const;

  virtual const IBTreeKey& SentinelKey () const;

  static const uint_t RAW_NODE_SIZE = 16384;
};

} //namespace pastra
} //namespace whais

#endif /* PS_TEMPLATETABLE_H_ */
