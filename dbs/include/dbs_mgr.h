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

#ifndef DBS_MGR_H_
#define DBS_MGR_H_

#include <string>

#include "dbs_types.h"
#include "dbs_table.h"

struct DBSFieldDescriptor;
class I_DBSTable;

static const D_UINT64 DEFAULT_MAX_FILE_SIZE           = 2147483648ul; //2GB
static const D_UINT32 DEFAULT_TABLE_CACHE_BLK_SIZE    = 16384u;       //16KB
static const D_UINT32 DEFAULT_TABLE_CACHE_BLK_COUNT   = 1024u;        //16KB
static const D_UINT32 DEFAULT_VLSTORE_CACHE_BLK_SIZE  = 16384u;       //16KB
static const D_UINT32 DEFAULT_VLSTORE_CACHE_BLK_COUNT = 1024u;        //16KB
static const D_UINT32 DEFAULT_VLVALUE_CACHE_SIZE      = 512u;

class DBS_SHL I_DBSHandler
{
public:
  I_DBSHandler () {}
  virtual ~I_DBSHandler () {}

  virtual TABLE_INDEX   PersistentTablesCount () = 0;
  virtual I_DBSTable&   RetrievePersistentTable (const TABLE_INDEX index) = 0;
  virtual I_DBSTable&   RetrievePersistentTable (const D_CHAR* pTableName) = 0;
  virtual void          AddTable (const D_CHAR* const pTableName,
                                  const FIELD_INDEX   fieldsCount,
                                  DBSFieldDescriptor* pInOutFields) = 0;
  virtual void          DeleteTable (const D_CHAR* const pTableName) = 0;

  virtual I_DBSTable&   CreateTempTable (const FIELD_INDEX   fieldsCount,
                                         DBSFieldDescriptor* pInOutFields) = 0;
  virtual void          ReleaseTable (I_DBSTable&) = 0;

  virtual const D_CHAR* TableName (const TABLE_INDEX index) = 0;
};

struct DBSSettings
{
  DBSSettings ()
    : m_WorkDir (whc_get_current_directory ()),
      m_TempDir (whc_get_current_directory ()),
      m_MaxFileSize (DEFAULT_MAX_FILE_SIZE),
      m_TableCacheBlkSize (DEFAULT_TABLE_CACHE_BLK_SIZE),
      m_TableCacheBlkCount (DEFAULT_TABLE_CACHE_BLK_COUNT),
      m_VLStoreCacheBlkSize (DEFAULT_VLSTORE_CACHE_BLK_SIZE),
      m_VLStoreCacheBlkCount (DEFAULT_VLSTORE_CACHE_BLK_COUNT),
      m_VLValueCacheSize (DEFAULT_VLVALUE_CACHE_SIZE)
  {
  }

  std::string   m_WorkDir;
  std::string   m_TempDir;
  D_UINT64      m_MaxFileSize;
  D_UINT32      m_TableCacheBlkSize;
  D_UINT32      m_TableCacheBlkCount;
  D_UINT32      m_VLStoreCacheBlkSize;
  D_UINT32      m_VLStoreCacheBlkCount;
  D_UINT32      m_VLValueCacheSize;
};

DBS_SHL void
DBSInit (const DBSSettings& setup);

DBS_SHL void
DBSShoutdown ();

DBS_SHL const DBSSettings&
DBSGetSeettings ();

DBS_SHL void
DBSCreateDatabase (const D_CHAR* const pName,
                   const D_CHAR*      pDbsDirectory = NULL);

DBS_SHL I_DBSHandler&
DBSRetrieveDatabase (const D_CHAR* const pName,
                     const D_CHAR*        pDbsDirectory = NULL);

DBS_SHL void
DBSReleaseDatabase (I_DBSHandler& hndDatabase);

DBS_SHL void
DBSRemoveDatabase (const D_CHAR* const pName,
                   const D_CHAR*        pDbsDirectory = NULL);

#endif /* DBS_MGR_H_ */
