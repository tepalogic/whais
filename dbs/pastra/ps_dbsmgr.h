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

#ifndef PS_DBSMGR_H_
#define PS_DBSMGR_H_

#include <map>
#include <string.h>

#include "utils/wthread.h"
#include "dbs/dbs_mgr.h"
#include "dbs/dbs_types.h"

namespace whisper {
namespace pastra {


//Forward declarations
class PersistentTable;



class DbsHandler : public IDBSHandler
{
public:
  DbsHandler (const DBSSettings&    settings,
              const std::string&    directory,
              const std::string&    name);

  DbsHandler (const DbsHandler&     source);

  virtual ~DbsHandler ();

  virtual TABLE_INDEX PersistentTablesCount ();

  virtual ITable& RetrievePersistentTable (const TABLE_INDEX index);

  virtual ITable& RetrievePersistentTable (const char* const name);

  virtual void ReleaseTable (ITable&);

  virtual void AddTable (const char* const      name,
                         const FIELD_INDEX      fieldsCount,
                         DBSFieldDescriptor*    inoutFields);

  virtual void DeleteTable (const char* const name);

  virtual ITable& CreateTempTable (const FIELD_INDEX   fieldsCount,
                                   DBSFieldDescriptor* inoutFields);

  virtual const char* TableName (const TABLE_INDEX index);

  void Discard ();

  void RemoveFromStorage ();

  const std::string& WorkingDir () const
  {
    return mDbsLocationDir;
  }

  const std::string& TemporalDir () const
  {
    return mGlbSettings.mTempDir;
  }

  uint64_t MaxFileSize () const
  {
    return mGlbSettings.mMaxFileSize;
  }

  bool HasUnreleasedTables ();

  void RegisterTableSpawn ();

private:
  typedef std::map<std::string, PersistentTable*> TABLES;

  void SyncToFile ();

  const DBSSettings&      mGlbSettings;
  Lock                    mSync;
  const std::string       mDbsLocationDir;
  const std::string       mName;
  TABLES                  mTables;
  int                     mCreatedTemporalTables;
};

} //namespace pastra
} //namespace whisper

#endif                                /* PS_DBSMGR_H_ */

