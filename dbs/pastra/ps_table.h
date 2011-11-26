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

class PSTable:public I_PSTable, public I_BlocksManager
{
  friend class DbsHandler;
  friend class PSCacheManager;
  friend class std::auto_ptr < PSTable >;

public:
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
  virtual void SetEntry (const DBSHiresDate &rSource, const D_UINT64 rowIndex, const D_UINT fieldIndex);
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
  virtual void GetEntry (DBSHiresDate &rDestination, const D_UINT64 rowIndex, const D_UINT fieldIndex);
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
  PSTable (DbsHandler & dbsHandler, const std::string & tableName);
  PSTable (DbsHandler & dbsHandler, const std::string & tableName, const DBSFieldDescriptor * pFields, D_UINT fieldsCount);
  virtual ~ PSTable ();

  D_UINT64 IncreaseRowCount ();
  void RemoveFromDatabase ();
  void CheckRowToReuse (const D_UINT64 rowIndex);
  void CheckRowToDelete (const D_UINT64 rowIndex);

  //Data members
  D_UINT64              m_VariableStorageSize;
  D_UINT64              m_RowsCount;
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

}
#endif				/* PS_TABLE_H_ */
