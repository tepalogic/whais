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

#include "utils/wfile.h"

#include "dbs_table.h"
#include "dbs_types.h"
#include "dbs_values.h"

#include "ps_dbsmgr.h"
#include "ps_container.h"
#include "ps_blockcache.h"
#include "ps_varstorage.h"
#include "ps_btree_fields.h"

namespace whisper {
namespace pastra {

static const uint_t PS_TABLE_FIELD_TYPE_MASK = 0x00FF;
static const uint_t PS_TABLE_ARRAY_MASK      = 0x0100;

struct FieldDescriptor
{
  uint32_t mNullBitIndex;
  uint32_t mRowDataOff;
  uint32_t mNameOffset;
  uint32_t mTypeDesc        : 12;
  uint32_t mAcquired        : 1;
  uint32_t mIndexNodeSizeKB : 9;
  uint32_t mIndexUnitsCount : 10;
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

  virtual ROW_INDEX AddRow ();

  virtual ROW_INDEX AddReusedRow ();

  virtual void MarkRowForReuse (const ROW_INDEX row);

  virtual void CreateIndex (const FIELD_INDEX                 field,
                            CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                            CreateIndexCallbackContext* const cbContext);

  virtual void RemoveIndex (const FIELD_INDEX field);

  virtual bool IsIndexed (const FIELD_INDEX field) const;

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DBool&              value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DChar&              value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DDate&              value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DDateTime&          value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DHiresTime&         value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DInt8&              value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DInt16&             value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DInt32&             value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DInt64&             value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DReal&              value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DRichReal&          value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DUInt8&             value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DUInt16&            value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DUInt32&            value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DUInt64&            value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DText&              value);

  virtual void Set (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    const DArray&             value);



  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DBool&                    outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DChar&                    outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DDate&                    outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DDateTime&                outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DHiresTime&               outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DInt8&                    outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DInt16&                   outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DInt32&                   outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DInt64&                   outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DReal&                    outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DRichReal&                outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DUInt8&                   outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DUInt16&                  outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DUInt32&                  outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DUInt64&                  outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DText&                    outValue);

  virtual void Get (const ROW_INDEX           row,
                    const FIELD_INDEX         field,
                    DArray&                   outValue);


  virtual DArray MatchRows (const DBool&          min,
                            const DBool&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DChar&          min,
                            const DChar&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DDate&          min,
                            const DDate&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DDateTime&      min,
                            const DDateTime&      max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DHiresTime&     min,
                            const DHiresTime&     max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt8&         min,
                            const DUInt8&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt16&        min,
                            const DUInt16&        max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt32&        min,
                            const DUInt32&        max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DUInt64&        min,
                            const DUInt64&        max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt8&          min,
                            const DInt8&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt16&         min,
                            const DInt16&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt32&         min,
                            const DInt32&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DInt64&         min,
                            const DInt64&         max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DReal&          min,
                            const DReal&          max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);

  virtual DArray MatchRows (const DRichReal&      min,
                            const DRichReal&      max,
                            const ROW_INDEX       fromRow,
                            const ROW_INDEX       toRow,
                            const ROW_INDEX       ignoreFirst,
                            const ROW_INDEX       maxCount,
                            const FIELD_INDEX     field);



  //Followings declarations shouldn't be public,
  //but kept here to ease the testing procedures.
  uint_t RowSize () const;
  FieldDescriptor& GetFieldDescriptorInternal (
                                         const FIELD_INDEX fieldIndex
                                              ) const;

private:
  template<class T> void StoreEntry (const ROW_INDEX,
                                     const FIELD_INDEX,
                                     const T&);

  template<class T> void RetrieveEntry (const ROW_INDEX,
                                        const FIELD_INDEX,
                                        T&);

  template<class T> DArray MatchRowsWithIndex (const T&          min,
                                               const T&          max,
                                               const ROW_INDEX   fromRow,
                                               ROW_INDEX         toRow,
                                               ROW_INDEX         ignoreFirst,
                                               ROW_INDEX         maxCount,
                                               const FIELD_INDEX fieldIndex);

  template<class T> DArray MatchRows (const T&          min,
                                      const T&          max,
                                      const ROW_INDEX   fromRow,
                                      ROW_INDEX         toRow,
                                      ROW_INDEX         ignoreFirst,
                                      ROW_INDEX         maxCount,
                                      const FIELD_INDEX filedIndex);

  void CheckRowToReuse (const ROW_INDEX row);

  void CheckRowToDelete (const ROW_INDEX row);

  void AcquireFieldIndex (FieldDescriptor* const field);

  void ReleaseIndexField (FieldDescriptor* const field);


  virtual uint_t MaxCachedNodes ();

  virtual IBTreeNode* LoadNode (const NODE_INDEX nodeId);

  virtual void SaveNode (IBTreeNode* const node);

protected:
  virtual void Flush ();

  virtual void MakeHeaderPersistent () = 0;

  virtual IDataContainer* CreateIndexContainer (const FIELD_INDEX field) = 0;

  virtual IDataContainer& RowsContainer () = 0;

  virtual IDataContainer& TableContainer () = 0;

  virtual VariableSizeStore& VSStore () = 0;

  //Data members
  DbsHandler&                           mDbs;
  ROW_INDEX                             mRowsCount;
  NODE_INDEX                            mRootNode;
  NODE_INDEX                            mUnallocatedHead;
  uint32_t                              mRowSize;
  uint32_t                              mDescriptorsSize;
  FIELD_INDEX                           mFieldsCount;
  std::auto_ptr<uint8_t>                mFieldsDescriptors;
  std::vector<FieldIndexNodeManager*>   mvIndexNodeMgrs;
  BlockCache                            mRowCache;
  Lock                                  mSync;
  Lock                                  mIndexSync;
};

class TableRmKey : public IBTreeKey
{
public:
  TableRmKey (const uint64_t row) : mRow (row)
  {
  };

  operator uint64_t () const { return mRow; }

  const uint64_t mRow;
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

  virtual bool IsLess (const IBTreeKey& key, const KEY_INDEX keyIndex) const;

  virtual bool IsEqual (const IBTreeKey& key, const KEY_INDEX keyIndex) const;

  virtual bool IsBigger (const IBTreeKey& key, const KEY_INDEX keyIndex) const;

  virtual const IBTreeKey& SentinelKey () const;

  static const uint_t RAW_NODE_SIZE = 16384;
};

} //namespace pastra
} //namespace whisper

#endif /* PS_TEMPLATETABLE_H_ */
