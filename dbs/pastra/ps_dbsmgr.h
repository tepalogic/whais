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

//Some classes forward declarations
class PersistentTable;

class DbsHandler : public I_DBSHandler
{
public:
  DbsHandler (const DBSSettings& globalSettings,
              const std::string& dbsDirectory,
              const std::string& dbsName);
  DbsHandler (const DbsHandler& source);
  virtual ~DbsHandler ();

  virtual TABLE_INDEX  PersistentTablesCount ();
  virtual I_DBSTable&  RetrievePersistentTable (const TABLE_INDEX index);
  virtual I_DBSTable&  RetrievePersistentTable (const char* pTableName);
  virtual void         ReleaseTable (I_DBSTable&);
  virtual void         AddTable (const char* const pTableName,
                                 const FIELD_INDEX   fieldsCount,
                                 DBSFieldDescriptor* pInOutFields);
  virtual void         DeleteTable (const char* const pTableName);

  virtual I_DBSTable&  CreateTempTable (const FIELD_INDEX   fieldsCount,
                                        DBSFieldDescriptor* pInOutFields);
  virtual const char* TableName (const TABLE_INDEX index);

  void Discard ();
  void RemoveFromStorage ();

  const std::string& WorkingDir () const
  {
    return m_DbsWorkDir;
  }
  const std::string& TemporalDir () const
  {
    return m_GlbSettings.m_TempDir;
  }

  uint64_t MaxFileSize () const
  {
    return m_GlbSettings.m_MaxFileSize;
  }

private:
  typedef std::map<std::string, PersistentTable*> TABLES;

  void SyncToFile ();

  const DBSSettings& m_GlbSettings;
  Lock      m_Sync;
  const std::string  m_DbsWorkDir;
  const std::string  m_Name;
  TABLES             m_Tables;
};

} //namespace pastra
} //namespace whisper

#endif                                /* PS_DBSMGR_H_ */
