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

#include "utils/include/wfile.h"

#include "dbs_table.h"
#include "dbs_types.h"
#include "dbs_values.h"

#include "ps_dbsmgr.h"
#include "ps_container.h"
#include "ps_blockcache.h"
#include "ps_varstorage.h"
#include "ps_btree_fields.h"

namespace pastra
{

static const D_UINT PS_TABLE_FIELD_TYPE_MASK = 0x00FF;
static const D_UINT PS_TABLE_ARRAY_MASK      = 0x0100;

struct FieldDescriptor
{
  D_UINT32 m_NullBitIndex;
  D_UINT32 m_StoreIndex;
  D_UINT32 m_NameOffset;
  D_UINT32 m_TypeDesc        : 12;
  D_UINT32 m_Aquired         : 1;
  D_UINT32 m_IndexNodeSizeKB : 9;
  D_UINT32 m_IndexUnitsCount : 10;
};


class PrototypeTable : public I_DBSTable, public I_BlocksManager, public I_BTreeNodeManager
{

public:
  PrototypeTable (DbsHandler& dbs);
  PrototypeTable (const PrototypeTable& prototype);

  virtual ~PrototypeTable ();

  //Implementations for I_BTreeNodeManager
  virtual D_UINT      GetRawNodeSize () const;
  virtual NODE_INDEX  AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey);
  virtual void        FreeNode (const NODE_INDEX node);
  virtual NODE_INDEX  GetRootNodeId ();
  virtual void        SetRootNodeId (const NODE_INDEX node);

  //Implementations for I_PSBlocksManager
  virtual void StoreItems (const D_UINT8* pSrcBuffer, D_UINT64 firstItem, D_UINT itemsCount);
  virtual void RetrieveItems (D_UINT8* pDestBuffer, D_UINT64 firstItem, D_UINT itemsCount);

  //Implementations for I_DBSTable
  virtual D_UINT             GetFieldsCount ();
  virtual DBSFieldDescriptor GetFieldDescriptor (D_UINT fieldIndex);
  virtual DBSFieldDescriptor GetFieldDescriptor (const D_CHAR* const pFieldName);
  virtual D_UINT64           GetAllocatedRows ();
  virtual D_UINT64           AddRow ();
  virtual D_UINT64           AddReusedRow ();
  virtual void               MarkRowForReuse (D_UINT64 rowindex);

  virtual void CreateFieldIndex (const D_UINT fieldIndex,
                                 CREATE_INDEX_CALLBACK_FUNC* const cb_func,
                                 CallBackIndexData* const pCbData);
  virtual void RemoveFieldIndex (const D_UINT fieldIndex);
  virtual bool IsFieldIndexed (const D_UINT fieldIndex) const;

  virtual void SetEntry (const DBSChar& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSBool& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSDate& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSDateTime& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSHiresTime& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt8& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt16& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt32& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt64& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSReal& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSRichReal& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt8& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt16& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt32& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt64& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSText& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSArray& rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);

  virtual void GetEntry (DBSChar& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSBool& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSDate& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSDateTime& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSHiresTime& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt8& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt16& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt32& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt64& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSReal& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSRichReal& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt8& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt16& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt32& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt64& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSText& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSArray& rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSBool&  min,
                                    const DBSBool&  max,
                                    const D_UINT64  fromRow,
                                    const D_UINT64  toRow,
                                    const D_UINT64  ignoreFirst,
                                    const D_UINT64  maxCount,
                                    const D_UINT    fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSChar& min,
                                    const DBSChar& max,
                                    const D_UINT64 fromRow,
                                    const D_UINT64 toRow,
                                    const D_UINT64 ignoreFirst,
                                    const D_UINT64 maxCount,
                                    const D_UINT   fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSDate& min,
                                    const DBSDate& max,
                                    const D_UINT64 fromRow,
                                    const D_UINT64 toRow,
                                    const D_UINT64 ignoreFirst,
                                    const D_UINT64 maxCount,
                                    const D_UINT   fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSDateTime& min,
                                    const DBSDateTime& max,
                                    const D_UINT64     fromRow,
                                    const D_UINT64     toRow,
                                    const D_UINT64     ignoreFirst,
                                    const D_UINT64     maxCount,
                                    const D_UINT       fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSHiresTime& min,
                                    const DBSHiresTime& max,
                                    const D_UINT64      fromRow,
                                    const D_UINT64      toRow,
                                    const D_UINT64      ignoreFirst,
                                    const D_UINT64      maxCount,
                                    const D_UINT        fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSUInt8& min,
                                    const DBSUInt8& max,
                                    const D_UINT64  fromRow,
                                    const D_UINT64  toRow,
                                    const D_UINT64  ignoreFirst,
                                    const D_UINT64  maxCount,
                                    const D_UINT    fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSUInt16& min,
                                    const DBSUInt16& max,
                                    const D_UINT64   fromRow,
                                    const D_UINT64   toRow,
                                    const D_UINT64   ignoreFirst,
                                    const D_UINT64   maxCount,
                                    const D_UINT     fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSUInt32& min,
                                    const DBSUInt32& max,
                                    const D_UINT64   fromRow,
                                    const D_UINT64   toRow,
                                    const D_UINT64   ignoreFirst,
                                    const D_UINT64   maxCount,
                                    const D_UINT     fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSUInt64& min,
                                    const DBSUInt64& max,
                                    const D_UINT64   fromRow,
                                    const D_UINT64   toRow,
                                    const D_UINT64   ignoreFirst,
                                    const D_UINT64   maxCount,
                                    const D_UINT     fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSInt8& min,
                                    const DBSInt8& max,
                                    const D_UINT64 fromRow,
                                    const D_UINT64 toRow,
                                    const D_UINT64 ignoreFirst,
                                    const D_UINT64 maxCount,
                                    const D_UINT   fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSInt16& min,
                                    const DBSInt16& max,
                                    const D_UINT64  fromRow,
                                    const D_UINT64  toRow,
                                    const D_UINT64  ignoreFirst,
                                    const D_UINT64  maxCount,
                                    const D_UINT    fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSInt32& min,
                                    const DBSInt32& max,
                                    const D_UINT64  fromRow,
                                    const D_UINT64  toRow,
                                    const D_UINT64  ignoreFirst,
                                    const D_UINT64  maxCount,
                                    const D_UINT    fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSInt64& min,
                                    const DBSInt64& max,
                                    const D_UINT64   fromRow,
                                    const D_UINT64   toRow,
                                    const D_UINT64   ignoreFirst,
                                    const D_UINT64   maxCount,
                                    const D_UINT     fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSReal& min,
                                    const DBSReal& max,
                                    const D_UINT64 fromRow,
                                    const D_UINT64 toRow,
                                    const D_UINT64 ignoreFirst,
                                    const D_UINT64 maxCount,
                                    const D_UINT   fieldIndex);

  virtual DBSArray GetMatchingRows (const DBSRichReal& min,
                                    const DBSRichReal& max,
                                    const D_UINT64     fromRow,
                                    const D_UINT64     toRow,
                                    const D_UINT64     ignoreFirst,
                                    const D_UINT64     maxCount,
                                    const D_UINT       fieldIndex);

  //Implementations for I_PSTable
  D_UINT           GetRowSize () const;
  FieldDescriptor& GetFieldDescriptorInternal (D_UINT fieldIndex) const;
  FieldDescriptor& GetFieldDescriptorInternal (const D_CHAR* const pFieldName) const;

private:
  template <class T> void     StoreEntry (const T &, const D_UINT64, const D_UINT);
  template <class T> void     RetrieveEntry (T &, const D_UINT64, const D_UINT);
  template <class T> DBSArray MatchRowsWithIndex (const D_UINT   fieldIndex,
                                                  const T&       min,
                                                  const T&       max,
                                                  const D_UINT64 fromRow,
                                                  D_UINT64       toRow,
                                                  D_UINT64       ignoreFirst,
                                                  D_UINT64       maxCount);

  template <class T> DBSArray MatchRows (const T&       min,
                                         const T&       max,
                                         const D_UINT64 fromRow,
                                         D_UINT64       toRow,
                                         D_UINT64       ignoreFirst,
                                         D_UINT64       maxCount,
                                         const D_UINT   filedIndex);

  D_UINT64 IncreaseRowCount ();
  void     CheckRowToReuse (const D_UINT64 rowIndex);
  void     CheckRowToDelete (const D_UINT64 rowIndex);
  void     AquireIndexField (FieldDescriptor* const pFieldDesc);
  void     ReleaseIndexField (FieldDescriptor* const pFieldDesc);

  //Implementations for I_BTreeNodeManager
  virtual D_UINT       GetMaxCachedNodes ();
  virtual I_BTreeNode* GetNode (const NODE_INDEX node);
  virtual void         StoreNode (I_BTreeNode* const pNode);

protected:
  virtual void                 Flush ();
  virtual void                 MakeHeaderPersistent () = 0;
  virtual I_DataContainer*     CreateIndexContainer (const D_UINT fieldIndex) = 0;
  virtual I_DataContainer&     FixedFieldsContainer () = 0;
  virtual I_DataContainer&     MainTableContainer () = 0;
  virtual VLVarsStore&         VariableFieldsStore () = 0;

  //Data members
  DbsHandler                            m_Dbs;
  D_UINT64                              m_RowsCount;
  NODE_INDEX                            m_RootNode;
  NODE_INDEX                            m_FirstUnallocatedRoot;
  D_UINT32                              m_RowSize;
  D_UINT32                              m_DescriptorsSize;
  D_UINT16                              m_FieldsCount;
  std::auto_ptr<D_UINT8>                m_FieldsDescriptors;
  std::vector<FieldIndexNodeManager*>   m_vIndexNodeMgrs;
  BlockCache                            m_RowCache;
  WSynchronizer                         m_Sync;
  WSynchronizer                         m_IndexSync;
};

class TableRmKey : public I_BTreeKey
{
public:
  TableRmKey (const D_UINT64 row) : m_Row (row) {};

  operator D_UINT64 () const { return m_Row; }
  const D_UINT64 m_Row;
};

class TableRmNode : public I_BTreeNode
{
public:
  static const D_UINT RAW_NODE_SIZE = 16384;

  TableRmNode (PrototypeTable& table, const NODE_INDEX nodeId);
  virtual ~TableRmNode ();

  //Implementation of I_BTreeNode
  virtual D_UINT     KeysPerNode () const;
  virtual KEY_INDEX  GetFirstKey (const I_BTreeNode& parent) const;
  virtual NODE_INDEX GetChildNode (const KEY_INDEX keyIndex) const;
  virtual void       ResetKeyNode (const I_BTreeNode& childNode, const KEY_INDEX keyIndex);
  virtual void       SetChildNode (const KEY_INDEX keyIndex, const NODE_INDEX childNode);
  virtual KEY_INDEX  InsertKey (const I_BTreeKey& key);
  virtual void       RemoveKey (const KEY_INDEX keyIndex);
  virtual void       Split (const NODE_INDEX parentId);
  virtual void       Join (bool toRight);
  virtual bool       IsLess (const I_BTreeKey& key, KEY_INDEX keyIndex) const;
  virtual bool       IsEqual (const I_BTreeKey& key, KEY_INDEX keyIndex) const;
  virtual bool       IsBigger (const I_BTreeKey& key, KEY_INDEX keyIndex) const;

  virtual const I_BTreeKey& SentinelKey () const;

protected:
  std::auto_ptr <D_UINT8> m_cpNodeData;
};

}
#endif /* PS_TEMPLATETABLE_H_ */
