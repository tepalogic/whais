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

#ifndef PS_TABLE_H_
#define PS_TABLE_H_

#include "utils/include/wfile.h"

#include "dbs_table.h"
#include "dbs_types.h"

#include "ps_container.h"
#include "ps_blockcache.h"
#include "ps_varstorage.h"
#include "ps_btree_index.h"
#include "ps_btree_fields.h"

namespace pastra
{

static const D_UINT PS_TABLE_FIELD_TYPE_MASK = 0xFF;
static const D_UINT PS_TABLE_ARRAY_MASK = 0x0100;

class DbsHandler;

struct PSFieldDescriptor
{
  D_UINT32 mNullBitIndex;
  D_UINT32 mTypeDesc;
  D_UINT32 mStoreIndex;
  D_UINT32 mNameOffset;
};

class I_PSTable : public I_DBSTable
{
public:
  virtual ~I_PSTable() {};
  virtual D_UINT GetRowSize () const = 0;

  virtual PSFieldDescriptor GetFieldDescriptorInternal (D_UINT fieldIndex) const = 0;
  virtual PSFieldDescriptor GetFieldDescriptorInternal (const D_CHAR * const pFieldName) const = 0;
};

class PSTable:public I_PSTable, public I_BlocksManager, public I_BTreeNodeManager
{
  friend class DbsHandler;
  friend class PSCacheManager;
  friend class std::auto_ptr < PSTable >;

public:
  //Implementations for I_BTreeNodeManager
  virtual NODE_INDEX  AllocateNode (const NODE_INDEX parent, KEY_INDEX parentKey);
  virtual void        FreeNode (const NODE_INDEX node);
  virtual NODE_INDEX  GetRootNodeId ();
  virtual void        SetRootNodeId (const NODE_INDEX node);

  //Implementations for I_PSBlocksManager
  virtual void StoreItems (const D_UINT8 *pSrcBuffer, D_UINT64 firstItem, D_UINT itemsCount);
  virtual void RetrieveItems (D_UINT8 *pDestBuffer, D_UINT64 firstItem, D_UINT itemsCount);

  //Implementations for I_DBSTable
  virtual D_UINT             GetFieldsCount ();
  virtual DBSFieldDescriptor GetFieldDescriptor (D_UINT fieldIndex);
  virtual DBSFieldDescriptor GetFieldDescriptor (const D_CHAR * const pFieldName);
  virtual D_UINT64 GetAllocatedRows ();

  virtual D_UINT64 AddRow ();
  virtual D_UINT64 AddReusedRow ();
  virtual void     MarkRowForReuse (D_UINT64 rowindex);

  virtual void SetEntry (const DBSChar &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSBool &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSDate &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSDateTime &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSHiresTime &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt8 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt16 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt32 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSInt64 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);

  virtual void SetEntry (const DBSReal &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSRichReal &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt8 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt16 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt32 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSUInt64 &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSText &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void SetEntry (const DBSArray &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);

  virtual void GetEntry (DBSChar &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSBool &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSDate &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSDateTime &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSHiresTime &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt8 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt16 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt32 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSInt64 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSReal &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSRichReal &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt8 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt16 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt32 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSUInt64 &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSText &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
  virtual void GetEntry (DBSArray &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);

  //Implementations for I_PSTable
  virtual D_UINT            GetRowSize () const;
  virtual PSFieldDescriptor GetFieldDescriptorInternal (D_UINT fieldIndex) const;
  virtual PSFieldDescriptor GetFieldDescriptorInternal (const D_CHAR * const pFieldName) const;

private:
  template <class T> void StoreEntry (const T &, const D_UINT64, const D_UINT);
  template <class T> void RetrieveEntry (T &, const D_UINT64, const D_UINT);

  void InitVariableStorages();
  void InitFromFile ();
  void SyncToFile ();

protected:

  //Implementations for I_BTreeNodeManager
  virtual D_UINT       GetMaxCachedNodes ();
  virtual I_BTreeNode* GetNode (const NODE_INDEX node);
  virtual void         StoreNode (I_BTreeNode *const node);

  PSTable (DbsHandler & dbsHandler, const std::string & tableName);
  PSTable (DbsHandler & dbsHandler,
           const std::string & tableName,
           const DBSFieldDescriptor * pFields,
           D_UINT fieldsCount);
  virtual ~ PSTable ();

  D_UINT64 IncreaseRowCount ();
  void RemoveFromDatabase ();
  void CheckRowToReuse (const D_UINT64 rowIndex);
  void CheckRowToDelete (const D_UINT64 rowIndex);

  //Data members
  D_UINT64              m_VariableStorageSize;
  D_UINT64              m_RowsCount;
  NODE_INDEX            m_RootNode;
  NODE_INDEX            m_FirstUnallocatedRoot;
  D_UINT                m_ReferenceCount;
  D_UINT32              m_RowSize;
  D_UINT32              m_DescriptorsSize;
  D_UINT16              m_FieldsCount;
  std::string           m_BaseFileName;
  std::auto_ptr <const D_UINT8>         m_FieldsDescriptors;
  WFile                                 m_MainTableFile;
  std::auto_ptr <FileContainer>         m_apFixedFields;
  std::auto_ptr <VaribaleLenghtStore>   m_apVariableFields;
  BlockCache            m_RowCache;
  bool                  m_Removed;
};

class PSTableRmKey : public I_BTreeKey
{
  friend class PSTableRmNode;
  friend class PSTable;

  PSTableRmKey (const D_UINT64 row) : m_Row (row) {};
  operator D_UINT64 () const { return m_Row; }

  const D_UINT64 m_Row;
};
class PSTableRmNode : public I_BTreeNode
{
public:

  static const D_UINT RAW_NODE_SIZE = 16384;

  PSTableRmNode (PSTable &table, const NODE_INDEX nodeId);
  virtual ~PSTableRmNode ();

  //Implementation of I_BTreeNode
  virtual D_UINT GetKeysPerNode () const;

  virtual KEY_INDEX  GetFirstKey (const I_BTreeNode &parent) const;
  virtual NODE_INDEX GetChildNode (const KEY_INDEX keyIndex) const;
  virtual void       ResetKeyNode (const I_BTreeNode &childNode, const KEY_INDEX keyIndex);
  virtual void       SetChildNode (const KEY_INDEX keyIndex, const NODE_INDEX childNode);

  virtual KEY_INDEX InsertKey (const I_BTreeKey &key);
  virtual void      RemoveKey (const KEY_INDEX keyIndex);

  virtual void Split (const NODE_INDEX parentId);
  virtual void Join (bool toRight);

  virtual bool IsLess (const I_BTreeKey &key, KEY_INDEX keyIndex) const;
  virtual bool IsEqual (const I_BTreeKey &key, KEY_INDEX keyIndex) const;
  virtual bool IsBigger (const I_BTreeKey &key, KEY_INDEX keyIndex) const;

  virtual const I_BTreeKey& GetSentinelKey () const;

protected:
  friend class PSTable;

  D_UINT8 * const m_cpNodeData;
};

}
#endif				/* PS_TABLE_H_ */
