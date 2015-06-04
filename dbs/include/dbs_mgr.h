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



namespace whais {



static const uint64_t DEFAULT_MAX_FILE_SIZE           = 2147483648ul; //2GB
static const uint32_t DEFAULT_TABLE_CACHE_BLK_SIZE    = 16384u;       //16KB
static const uint32_t DEFAULT_TABLE_CACHE_BLK_COUNT   = 1024u;
static const uint32_t DEFAULT_VLSTORE_CACHE_BLK_SIZE  = 16384u;       //16KB
static const uint32_t DEFAULT_VLSTORE_CACHE_BLK_COUNT = 1024u;
static const uint32_t DEFAULT_VLVALUE_CACHE_SIZE      = 512u;



class DBS_SHL IDBSHandler
{
public:
  virtual ~IDBSHandler () {}

  virtual TABLE_INDEX PersistentTablesCount () = 0;

  virtual ITable& RetrievePersistentTable (const TABLE_INDEX index) = 0;

  virtual ITable& RetrievePersistentTable (const char* const name) = 0;

  virtual void AddTable (const char* const   name,
                         const FIELD_INDEX   fieldsCount,
                         DBSFieldDescriptor* inoutFields) = 0;
  virtual void DeleteTable (const char* const name) = 0;

  virtual void SyncAllTablesContent () = 0;
  virtual void SyncTableContent (const TABLE_INDEX index) = 0;
  virtual void NotifyDatabaseUpdate () = 0;

  virtual ITable& CreateTempTable (const FIELD_INDEX   fieldsCount,
                                   DBSFieldDescriptor* inoutFields) = 0;

  virtual void ReleaseTable (ITable&) = 0;

  virtual const char* TableName (const TABLE_INDEX index) = 0;

};

struct DBSSettings
{
  DBSSettings ()
    : mWorkDir (whf_current_dir ()),
      mTempDir (whf_current_dir ()),
      mMaxFileSize (DEFAULT_MAX_FILE_SIZE),
      mTableCacheBlkSize (DEFAULT_TABLE_CACHE_BLK_SIZE),
      mTableCacheBlkCount (DEFAULT_TABLE_CACHE_BLK_COUNT),
      mVLStoreCacheBlkSize (DEFAULT_VLSTORE_CACHE_BLK_SIZE),
      mVLStoreCacheBlkCount (DEFAULT_VLSTORE_CACHE_BLK_COUNT),
      mVLValueCacheSize (DEFAULT_VLVALUE_CACHE_SIZE)
  {
  }

  std::string   mWorkDir;
  std::string   mTempDir;

  uint64_t      mMaxFileSize;
  uint32_t      mTableCacheBlkSize;
  uint32_t      mTableCacheBlkCount;
  uint32_t      mVLStoreCacheBlkSize;
  uint32_t      mVLStoreCacheBlkCount;
  uint32_t      mVLValueCacheSize;
};


enum FIX_ERROR_CALLBACK_TYPE {
  INFORMATION                   = 1,
  FIX_INFO,
  FIX_QUESTION,
  CONFIRMATION_QUESTION,
  OPTIMISE_QUESTION,
  CRITICAL
};

typedef bool (*FIX_ERROR_CALLBACK) (const FIX_ERROR_CALLBACK_TYPE type,
                                    const char* const             format,
                                    ... );

DBS_SHL void
DBSInit (const DBSSettings& setup);


DBS_SHL void
DBSShoutdown ();


DBS_SHL const DBSSettings&
DBSGetSeettings ();


DBS_SHL void
DBSCreateDatabase (const char* const name,
                   const char*       path = NULL);


DBS_SHL bool
DBSValidateDatabase (const char* const name,
                     const char*       path = NULL);

DBS_SHL bool
DBSRepairDatabase (const char* const            name,
                   const char*                  path        = NULL,
                   FIX_ERROR_CALLBACK           fixCallback = NULL);

DBS_SHL IDBSHandler&
DBSRetrieveDatabase (const char* const name,
                     const char*       path = NULL);

DBS_SHL void
DBSReleaseDatabase (IDBSHandler& hnd);


DBS_SHL void
DBSRemoveDatabase (const char* const     name,
                   const char* const     path = NULL);


} //namespace whais

#endif /* DBS_MGR_H_ */

