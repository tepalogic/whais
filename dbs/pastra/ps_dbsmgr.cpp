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

#include <map>
#include <memory>
#include <memory.h>
#include <assert.h>

#include "../include/dbs_exception.h"
#include "utils/include/wfile.h"
#include "utils/include/wthread.h"

#include "ps_dbsmgr.h"
#include "ps_table.h"

using namespace pastra;
using namespace std;

static const char DBS_FILE_EXT[]       = ".pd";
static const char DBS_FILE_SIGNATURE[] = {
                                              0x50, 0x41, 0x53, 0x54,
                                              0x52, 0x41, 0x20, 0x44
                                            };

static const uint16_t PS_DBS_VER_MAJ      = 0;
static const uint16_t PS_DBS_VER_MIN      = 10;

static const uint_t PS_DBS_SIGNATURE_OFF  = 0;
static const uint_t PS_DBS_SIGNATURE_LEN  = 8;
static const uint_t PS_DBS_VER_MAJ_OFF    = 8;
static const uint_t PS_DBS_VER_MAJ_LEN    = 2;
static const uint_t PS_DBS_VER_MIN_OFF    = 10;
static const uint_t PS_DBS_VER_MIN_LEN    = 2;
static const uint_t PS_DBS_NUM_TABLES_OFF = 14;
static const uint_t PS_DBS_NUM_TABLES_LEN = 2;
static const uint_t PS_DBS_MAX_FILE_OFF   = 16;
static const uint_t PS_DBS_MAX_FILE_LEN   = 8;

static const uint_t PS_DBS_HEADER_SIZE = 24;

struct DbsElement
{
  uint64_t   m_RefCount;
  DbsHandler m_Dbs;

  DbsElement (const DbsHandler& rDbs) :
    m_RefCount (0),
    m_Dbs (rDbs)
  {
  }
};

typedef map<string, DbsElement> DATABASES_MAP;

struct DbsManager
{
  DbsManager (const DBSSettings& settings)
    : m_Sync (),
      m_DBSSettings (settings),
      m_Databases ()
  {
    if ((m_DBSSettings.m_WorkDir.length () == 0)
        || (m_DBSSettings.m_TempDir.length () == 0)
        || (m_DBSSettings.m_TableCacheBlkSize == 0)
        || (m_DBSSettings.m_TableCacheBlkCount == 0)
        || (m_DBSSettings.m_VLStoreCacheBlkSize == 0)
        || (m_DBSSettings.m_VLStoreCacheBlkCount == 0)
        || (m_DBSSettings.m_VLValueCacheSize == 0))
      {

        throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));
      }

    if (m_DBSSettings.m_WorkDir [m_DBSSettings.m_WorkDir.length () - 1] !=
        whf_dir_delim ()[0])
      {
        m_DBSSettings.m_WorkDir += whf_dir_delim ();
      }

    if (m_DBSSettings.m_TempDir [m_DBSSettings.m_TempDir.length () - 1] !=
        whf_dir_delim ()[0])
      {
        m_DBSSettings.m_TempDir += whf_dir_delim ();
      }
  }

  WSynchronizer m_Sync;
  DBSSettings   m_DBSSettings;
  DATABASES_MAP m_Databases;
};

static auto_ptr<DbsManager> apDbsManager_;

DBS_SHL void
DBSInit (const DBSSettings& setup)
{
  if (apDbsManager_.get () != NULL)
    throw DBSException (NULL, _EXTRA (DBSException::ALREADY_INITED));

  apDbsManager_.reset (new DbsManager (setup));
}

DBS_SHL void
DBSShoutdown ()
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //~DbsManager() will be called automatically!
  apDbsManager_.reset (NULL);
}

DBS_SHL const DBSSettings&
DBSGetSeettings ()
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  return apDbsManager_->m_DBSSettings;
}

DBS_SHL void
DBSCreateDatabase (const char* const pName,
                   const char*       pDbsDirectory)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  if (pDbsDirectory == NULL)
    pDbsDirectory = apDbsManager_->m_DBSSettings.m_WorkDir.c_str ();

  string fileName = pDbsDirectory;
  fileName += pName;
  fileName += DBS_FILE_EXT;

  WFile dbsFile (fileName.c_str (), WHC_FILECREATE_NEW | WHC_FILEWRITE);

  auto_ptr<uint8_t> apBufferHeader (new uint8_t[PS_DBS_HEADER_SIZE]);
  uint8_t* const    cpHeader = apBufferHeader.get ();

  memcpy (cpHeader + PS_DBS_SIGNATURE_OFF,
          DBS_FILE_SIGNATURE,
          PS_DBS_SIGNATURE_LEN);

  *_RC (uint16_t*, cpHeader + PS_DBS_VER_MAJ_OFF)    = PS_DBS_VER_MAJ;
  *_RC (uint16_t*, cpHeader + PS_DBS_VER_MIN_OFF)    = PS_DBS_VER_MIN;
  *_RC (uint16_t*, cpHeader + PS_DBS_NUM_TABLES_OFF) = 0;
  *_RC (uint64_t*, cpHeader + PS_DBS_MAX_FILE_OFF)   =
                                  apDbsManager_->m_DBSSettings.m_MaxFileSize;

  dbsFile.Write (cpHeader, PS_DBS_HEADER_SIZE);
}

DBS_SHL  I_DBSHandler&
DBSRetrieveDatabase (const char* const pName, const char* pDatabaseDir)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII syncHolder (apDbsManager_->m_Sync);

  DATABASES_MAP&          rMap = apDbsManager_->m_Databases;
  DATABASES_MAP::iterator it   = rMap.find (pName);

  if (it == rMap.end ())
    {
      if (pDatabaseDir == NULL)
        pDatabaseDir = apDbsManager_->m_DBSSettings.m_WorkDir.c_str ();

      rMap.insert (
          pair<string, DbsElement> (
                        pName,
                        DbsElement (DbsHandler (apDbsManager_->m_DBSSettings,
                                                string (pDatabaseDir),
                                                string (pName)))
                                   )
                  );
      it = rMap.find (pName);
      assert (it != rMap.end ());
    }

  it->second.m_RefCount++;

  return it->second.m_Dbs;
}

DBS_SHL void
DBSReleaseDatabase (I_DBSHandler& hndDatabase)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII syncHolder (apDbsManager_->m_Sync);

  DATABASES_MAP&          rMap = apDbsManager_->m_Databases;
  DATABASES_MAP::iterator it;

  for (it = rMap.begin (); it != rMap.end (); ++it)
    {
      if (_SC (I_DBSHandler *, &it->second.m_Dbs) == &hndDatabase)
        {
          assert (it->second.m_RefCount > 0);

          if (--it->second.m_RefCount == 0)
            {
              it->second.m_Dbs.Discard ();
              rMap.erase (it);
            }
          break;
        }
    }
}

DBS_SHL  void
DBSRemoveDatabase (const char* const pName, const char* pDatabaseDir)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII syncHolder (apDbsManager_->m_Sync);

  DATABASES_MAP&          rMap = apDbsManager_->m_Databases;
  DATABASES_MAP::iterator it   = rMap.find (pName);

  if (it == rMap.end ())
    {
      if (pDatabaseDir == NULL)
        pDatabaseDir = apDbsManager_->m_DBSSettings.m_WorkDir.c_str ();

      rMap.insert (
          pair<string, DbsElement> (
                        pName,
                        DbsElement (DbsHandler (apDbsManager_->m_DBSSettings,
                                                string (pDatabaseDir),
                                                string (pName)))
                                   )
                  );
      it = rMap.find (pName);
      assert (it != rMap.end ());
    }

  if (it->second.m_RefCount != 0)
    throw DBSException (NULL, _EXTRA (DBSException::DATABASE_IN_USE));

  it->second.m_Dbs.RemoveFromStorage ();
  rMap.erase (it);
}


DbsHandler::DbsHandler (const DBSSettings& globalSettings,
                        const std::string& dbsDirectory,
                        const std::string& dbsName)
  : I_DBSHandler (),
    m_GlbSettings (globalSettings),
    m_Sync (),
    m_DbsWorkDir (dbsDirectory),
    m_Name (dbsName),
    m_Tables ()
{
  string fileName = m_DbsWorkDir + m_Name + DBS_FILE_EXT;
  WFile  inputFile (fileName.c_str (), WHC_FILEOPEN_EXISTING | WHC_FILEREAD);

  auto_ptr<uint8_t> apBuffer (new uint8_t[inputFile.GetSize ()]);
  uint8_t*          pBuffer = apBuffer.get ();

  inputFile.Seek (0, WHC_SEEK_BEGIN);
  inputFile.Read (pBuffer, inputFile.GetSize ());

  if (memcmp (pBuffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
    throw DBSException (NULL, _EXTRA (DBSException::INAVLID_DATABASE));

  const uint16_t versionMaj = *_RC (uint16_t*, pBuffer + PS_DBS_VER_MAJ_OFF);
  const uint16_t versionMin = *_RC (uint16_t*, pBuffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ) ||
      ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
    {
      throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));
    }

  uint64_t maxFileSize  = *_RC (uint64_t*, pBuffer + PS_DBS_MAX_FILE_OFF);

  if (maxFileSize != m_GlbSettings.m_MaxFileSize)
    throw DBSException (NULL, _EXTRA (DBSException::INAVLID_DATABASE));

  uint16_t tablesCount = *_RC (uint16_t*, pBuffer + PS_DBS_NUM_TABLES_OFF);

  pBuffer += PS_DBS_HEADER_SIZE;

  while (tablesCount-- > 0)
    {
      m_Tables.insert (
          pair<string, PersistentTable*> (_RC (char*, pBuffer),
                                          _RC (PersistentTable*, NULL))
                      );
      pBuffer += strlen (_RC (char*, pBuffer)) + 1;
    }

}

DbsHandler::DbsHandler (const DbsHandler& source)
  : I_DBSHandler (),
    m_GlbSettings (source.m_GlbSettings),
    m_Sync (),
    m_DbsWorkDir (source.m_DbsWorkDir),
    m_Name (source.m_Name),
    m_Tables (source.m_Tables)
{
}

DbsHandler::~DbsHandler ()
{
  Discard ();
}

TABLE_INDEX
DbsHandler::PersistentTablesCount ()
{
  return m_Tables.size ();
}

I_DBSTable&
DbsHandler::RetrievePersistentTable (const TABLE_INDEX index)
{
  TABLE_INDEX       iterator = index;
  WSynchronizerRAII syncHolder (m_Sync);

  if (iterator >= m_Tables.size ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  TABLES::iterator it = m_Tables.begin ();

  while (iterator-- > 0)
    {
      assert (it != m_Tables.end ());
      ++it;
    }

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  return *it->second;

}

I_DBSTable&
DbsHandler::RetrievePersistentTable (const char* pTableName)
{
  WSynchronizerRAII syncHolder (m_Sync);

  TABLES::iterator it = m_Tables.find (pTableName);

  if (it == m_Tables.end ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  return *it->second;
}

void
DbsHandler::AddTable (const char* const pTableName,
                      const FIELD_INDEX   fieldsCount,
                      DBSFieldDescriptor* pInOutFields)
{
  WSynchronizerRAII syncHolder (m_Sync);

  if ((pTableName == NULL) || (pInOutFields == NULL) || (fieldsCount == 0))
    throw DBSException (NULL, _EXTRA(DBSException::INVALID_PARAMETERS));

  string           tableName (pTableName);
  TABLES::iterator it = m_Tables.find (tableName);

  if (it != m_Tables.end ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_EXISTS));

  m_Tables.insert (pair<string, PersistentTable*> (
                                        tableName,
                                        _RC (PersistentTable*, NULL))
                                                  );
  it = m_Tables.find (tableName);
  try
    {
      it->second = new PersistentTable (*this,
                                        it->first,
                                        pInOutFields,
                                        fieldsCount);
    }
  catch (...)
    {
      m_Tables.erase (it);
      throw;
    }

  SyncToFile ();
}

void
DbsHandler::ReleaseTable (I_DBSTable& hndTable)
{

  if (hndTable.IsTemporal ())
    {
      delete &_SC (TemporalTable&, hndTable);
      return;
    }

  WSynchronizerRAII syncHolder (m_Sync);

  for (TABLES::iterator it = m_Tables.begin (); it != m_Tables.end (); ++it)
    if (&hndTable == _SC (I_DBSTable*, it->second))
      {
        delete it->second;
        it->second = NULL;

        return;
      }
}

const char*
DbsHandler::TableName (const TABLE_INDEX index)
{
  TABLE_INDEX       iterator = index;
  WSynchronizerRAII syncHolder (m_Sync);

  if (iterator >= m_Tables.size ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  TABLES::iterator it = m_Tables.begin ();

  while (iterator-- > 0)
    {
      assert (it != m_Tables.end ());
      ++it;
    }

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  return it->first.c_str ();
}

void
DbsHandler::DeleteTable (const char* const pTableName)
{
  WSynchronizerRAII syncHolder (m_Sync);

  TABLES::iterator it = m_Tables.find (pTableName);

  if (it == m_Tables.end ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  PersistentTable* const cpTable = it->second;
  cpTable->RemoveFromDatabase ();
  delete cpTable;

  m_Tables.erase (it);
  SyncToFile ();
}

I_DBSTable&
DbsHandler::CreateTempTable (const FIELD_INDEX   fieldsCount,
                             DBSFieldDescriptor* pInOutFields)
{
  return *(new TemporalTable (*this,
                               pInOutFields,
                               fieldsCount));
}

void
DbsHandler::Discard ()
{
  WSynchronizerRAII syncHolder (m_Sync);

  for (TABLES::iterator it = m_Tables.begin (); it != m_Tables.end (); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
}

void
DbsHandler::SyncToFile ()
{
  uint8_t aBuffer[PS_DBS_HEADER_SIZE];

  memcpy (aBuffer, DBS_FILE_SIGNATURE, PS_DBS_SIGNATURE_LEN);

  *_RC (uint16_t*, aBuffer + PS_DBS_VER_MAJ_OFF)    = PS_DBS_VER_MAJ;
  *_RC (uint16_t*, aBuffer + PS_DBS_VER_MIN_OFF)    = PS_DBS_VER_MIN;
  *_RC (uint16_t*, aBuffer + PS_DBS_NUM_TABLES_OFF) = m_Tables.size ();
  *_RC (uint64_t*, aBuffer + PS_DBS_MAX_FILE_OFF)   = MaxFileSize ();

  string fileName (WorkingDir () + m_Name + DBS_FILE_EXT);
  WFile outFile (fileName.c_str (), WHC_FILECREATE | WHC_FILEWRITE);
  outFile.SetSize (0);
  outFile.Write (aBuffer, sizeof aBuffer);

  for (TABLES::iterator it = m_Tables.begin (); it != m_Tables.end (); ++it)
    {
      outFile.Write (_RC (const uint8_t*, it->first.c_str ()),
                     it->first.length () + 1);
    }
}

void
DbsHandler::RemoveFromStorage ()
{
  //Discard all tables first!
  Discard ();

  //Remove all tables from database
  for (TABLES::iterator it = m_Tables.begin (); it != m_Tables.end (); ++it)
    {
      assert (it->first.c_str () != NULL);
      assert (it->second == NULL);

      auto_ptr<PersistentTable> table (new PersistentTable (*this, it->first));
      table->RemoveFromDatabase ();
    }

  //Remove the database file
  const string fileName (WorkingDir () + m_Name + DBS_FILE_EXT);
  whf_remove (fileName.c_str ());
}

#if  defined (ENABLE_MEMORY_TRACE) && defined (USE_DBS_SHL)
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "PASTRA";
#endif
