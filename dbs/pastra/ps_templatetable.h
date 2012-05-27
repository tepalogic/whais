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
  virtual D_UINT      RawNodeSize () const;
  virtual NODE_INDEX  AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey);
  virtual void        FreeNode (const NODE_INDEX node);
  virtual NODE_INDEX  GetRootNodeId ();
  virtual void        SetRootNodeId (const NODE_INDEX node);

  //Implementations for I_PSBlocksManager
  virtual void StoreItems (const D_UINT8* pSrcBuffer, D_UINT64 firstItem, D_UINT itemsCount);
  virtual void RetrieveItems (D_UINT8* pDestBuffer, D_UINT64 firstItem, D_UINT itemsCount);

  //Implementations for I_DBSTable
  virtual FIELD_INDEX        GetFieldsCount ();
  virtual DBSFieldDescriptor GetFieldDescriptor (const FIELD_INDEX field);
  virtual DBSFieldDescriptor GetFieldDescriptor (const D_CHAR* const pFieldName);
  virtual ROW_INDEX          GetAllocatedRows ();
  virtual ROW_INDEX          AddRow ();
  virtual ROW_INDEX          AddReusedRow ();
  virtual void               MarkRowForReuse (const ROW_INDEX row);

  virtual void CreateFieldIndex (const FIELD_INDEX                 field,
                                 CREATE_INDEX_CALLBACK_FUNC* const cbFunc,
                                 CallBackIndexData* const          pCbData);
  virtual void RemoveFieldIndex (const FIELD_INDEX field);
  virtual bool IsFieldIndexed (const FIELD_INDEX field) const;

  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSBool& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSChar& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSDate& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSDateTime& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSHiresTime& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt8& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt16& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt32& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSInt64& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSReal& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSRichReal& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt8& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt16& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt32& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSUInt64& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSText& value);
  virtual void SetEntry (const ROW_INDEX row, const FIELD_INDEX field, const DBSArray& value);

  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSBool& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSChar& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSDate& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSDateTime& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSHiresTime& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt8& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt16& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt32& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSInt64& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSReal& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSRichReal& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt8& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt16& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt32& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSUInt64& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSText& outValue);
  virtual void GetEntry (const ROW_INDEX row, const FIELD_INDEX field, DBSArray& outValue);

  virtual DBSArray GetMatchingRows (const DBSBool&    min,
                                    const DBSBool&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSChar&    min,
                                    const DBSChar&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSDate&    min,
                                    const DBSDate&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSDateTime& min,
                                    const DBSDateTime& max,
                                    const ROW_INDEX    fromRow,
                                    const ROW_INDEX    toRow,
                                    const ROW_INDEX    ignoreFirst,
                                    const ROW_INDEX    maxCount,
                                    const FIELD_INDEX  field);

  virtual DBSArray GetMatchingRows (const DBSHiresTime& min,
                                    const DBSHiresTime& max,
                                    const ROW_INDEX     fromRow,
                                    const ROW_INDEX     toRow,
                                    const ROW_INDEX     ignoreFirst,
                                    const ROW_INDEX     maxCount,
                                    const FIELD_INDEX   field);

  virtual DBSArray GetMatchingRows (const DBSUInt8&   min,
                                    const DBSUInt8&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSUInt16&  min,
                                    const DBSUInt16&  max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSUInt32&  min,
                                    const DBSUInt32&  max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSUInt64&  min,
                                    const DBSUInt64&  max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt8&    min,
                                    const DBSInt8&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt16&   min,
                                    const DBSInt16&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt32&   min,
                                    const DBSInt32&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSInt64&   min,
                                    const DBSInt64&   max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSReal&    min,
                                    const DBSReal&    max,
                                    const ROW_INDEX   fromRow,
                                    const ROW_INDEX   toRow,
                                    const ROW_INDEX   ignoreFirst,
                                    const ROW_INDEX   maxCount,
                                    const FIELD_INDEX field);

  virtual DBSArray GetMatchingRows (const DBSRichReal& min,
                                    const DBSRichReal& max,
                                    const ROW_INDEX    fromRow,
                                    const ROW_INDEX    toRow,
                                    const ROW_INDEX    ignoreFirst,
                                    const ROW_INDEX    maxCount,
                                    const FIELD_INDEX  field);

  D_UINT           GetRowSize () const;
  FieldDescriptor& GetFieldDescriptorInternal (const FIELD_INDEX fieldIndex) const;
  FieldDescriptor& GetFieldDescriptorInternal (const D_CHAR* const pFieldName) const;

private:
  template <class T> void     StoreEntry (const ROW_INDEX,
                                          const FIELD_INDEX,
                                          const T&);
  template <class T> void     RetrieveEntry (const ROW_INDEX,
                                             const FIELD_INDEX,
                                             T&);
  template <class T> DBSArray MatchRowsWithIndex (const T&          min,
                                                  const T&          max,
                                                  const ROW_INDEX   fromRow,
                                                  ROW_INDEX         toRow,
                                                  ROW_INDEX         ignoreFirst,
                                                  ROW_INDEX         maxCount,
                                                  const FIELD_INDEX fieldIndex);

  template <class T> DBSArray MatchRows (const T&          min,
                                         const T&          max,
                                         const ROW_INDEX   fromRow,
                                         ROW_INDEX         toRow,
                                         ROW_INDEX         ignoreFirst,
                                         ROW_INDEX         maxCount,
                                         const FIELD_INDEX filedIndex);

  void      CheckRowToReuse (const ROW_INDEX row);
  void      CheckRowToDelete (const ROW_INDEX row);
  void      AquireIndexField (FieldDescriptor* const pFieldDesc);
  void      ReleaseIndexField (FieldDescriptor* const pFieldDesc);

  //Implementations for I_BTreeNodeManager
  virtual D_UINT       MaxCachedNodes ();
  virtual I_BTreeNode* LoadNode (const NODE_INDEX node);
  virtual void         SaveNode (I_BTreeNode* const pNode);

protected:
  virtual void                 Flush ();
  virtual void                 MakeHeaderPersistent () = 0;
  virtual I_DataContainer*     CreateIndexContainer (const FIELD_INDEX field) = 0;
  virtual I_DataContainer&     FixedFieldsContainer () = 0;
  virtual I_DataContainer&     MainTableContainer () = 0;
  virtual VLVarsStore&         VariableFieldsStore () = 0;

  //Data members
  DbsHandler                            m_Dbs;
  ROW_INDEX                             m_RowsCount;
  NODE_INDEX                            m_RootNode;
  NODE_INDEX                            m_FirstUnallocatedRoot;
  D_UINT32                              m_RowSize;
  D_UINT32                              m_DescriptorsSize;
  FIELD_INDEX                           m_FieldsCount;
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
  virtual KEY_INDEX  GetParentKeyIndex (const I_BTreeNode& parent) const;
  virtual NODE_INDEX GetNodeOfKey (const KEY_INDEX keyIndex) const;
  virtual void       AdjustKeyNode (const I_BTreeNode& childNode, const KEY_INDEX keyIndex);
  virtual void       SetNodeOfKey (const KEY_INDEX keyIndex, const NODE_INDEX childNode);
  virtual KEY_INDEX  InsertKey (const I_BTreeKey& key);
  virtual void       RemoveKey (const KEY_INDEX keyIndex);
  virtual void       Split (const NODE_INDEX parentId);
  virtual void       Join (bool toRight);
  virtual bool       IsLess (const I_BTreeKey& key, KEY_INDEX keyIndex) const;
  virtual bool       IsEqual (const I_BTreeKey& key, KEY_INDEX keyIndex) const;
  virtual bool       IsBigger (const I_BTreeKey& key, KEY_INDEX keyIndex) const;

  virtual const I_BTreeKey& SentinelKey () const;
};

}
#endif /* PS_TEMPLATETABLE_H_ */
