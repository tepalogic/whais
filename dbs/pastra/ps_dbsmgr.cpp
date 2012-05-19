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


static const D_CHAR DBS_FILE_EXT[]       = ".pd";
static const D_CHAR DBS_FILE_SIGNATURE[] = {
                                              0x50, 0x41, 0x53, 0x54,
                                              0x52, 0x41, 0x20, 0x44
                                            };

static const D_UINT16 PS_DBS_VER_MAJ      = 0;
static const D_UINT16 PS_DBS_VER_MIN      = 10;

static const D_UINT PS_DBS_SIGNATURE_OFF  = 0;
static const D_UINT PS_DBS_SIGNATURE_LEN  = 8;
static const D_UINT PS_DBS_VER_MAJ_OFF    = 8;
static const D_UINT PS_DBS_VER_MAJ_LEN    = 2;
static const D_UINT PS_DBS_VER_MIN_OFF    = 10;
static const D_UINT PS_DBS_VER_MIN_LEN    = 2;
static const D_UINT PS_DBS_DIRECTORY_OFF  = 12;
static const D_UINT PS_DBS_DIRECTORY_LEN  = 2;
static const D_UINT PS_DBS_NUM_TABLES_OFF = 14;
static const D_UINT PS_DBS_NUM_TABLES_LEN = 2;
static const D_UINT PS_DBS_MAX_FILE_OFF   = 16;
static const D_UINT PS_DBS_MAX_FILE_LEN   = 8;

static const D_UINT PS_DBS_HEADER_SIZE = 24;

struct DbsElement
{
  D_UINT64   m_RefCount;
  DbsHandler m_Dbs;

  DbsElement (const DbsHandler& rDbs) :
    m_RefCount (0),
    m_Dbs (rDbs)
  {
  }
}
;

typedef map<string, DbsElement> DATABASES_MAP;

class DbsManager
{
  friend void          DBSInit (const D_CHAR* const, const D_CHAR* const, D_UINT64 maxFileSize);
  friend void          DBSShoutdown ();
  friend const D_CHAR* DBSGetWorkingDir ();
  friend const D_CHAR* DBSGetTempDir ();
  friend D_UINT64      DBSGetMaxFileSize ();
  friend I_DBSHandler& DBSRetrieveDatabase (const D_CHAR* const);
  friend void          DBSReleaseDatabase (I_DBSHandler&);
  friend void          DBSRemoveDatabase (const D_CHAR* const pName);
  friend class auto_ptr<DbsManager>;

private:
  DbsManager (const D_CHAR* const pDBSDirectory,
              const D_CHAR* const pTempDir,
              D_UINT64            maxFileSize) :
      m_Sync (),
      m_WorkingDir (pDBSDirectory),
      m_TempDir (pTempDir),
      m_Databases (),
      m_MaxFileSize (maxFileSize)
  {

    if (pDBSDirectory[strlen (pDBSDirectory) - 1] != whc_get_directory_delimiter ()[0])
      m_WorkingDir += whc_get_directory_delimiter ();

    if (pTempDir[strlen (pTempDir) - 1] != whc_get_directory_delimiter ()[0])
      m_TempDir += whc_get_directory_delimiter ();
  }

  ~DbsManager ()
  {
  }

  WSynchronizer m_Sync;
  string        m_WorkingDir;
  string        m_TempDir;
  DATABASES_MAP m_Databases;
  D_UINT64      m_MaxFileSize;
};

static auto_ptr<DbsManager> apDbsManager_;

void
DBSInit (const D_CHAR* const pDBSDirectory, const D_CHAR* const pTempDir, D_UINT64 maxFileSize)
{
  if (apDbsManager_.get () != NULL)
    throw DBSException (NULL, _EXTRA (DBSException::ALLREADY_INITED));

  if (maxFileSize < MINIMUM_MAX_FILE_SIZE)
    maxFileSize = MINIMUM_MAX_FILE_SIZE;

  apDbsManager_.reset (new DbsManager (pDBSDirectory, pTempDir, maxFileSize));
}

void
DBSShoutdown ()
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //~DbsManager() will be called automatically!
  apDbsManager_.reset (NULL);
}

const D_CHAR*
DBSGetWorkingDir ()
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  return apDbsManager_->m_WorkingDir.c_str ();
}

const D_CHAR*
DBSGetTempDir ()
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  return apDbsManager_->m_TempDir.c_str ();
}

D_UINT64
DBSGetMaxFileSize ()
{
  return apDbsManager_->m_MaxFileSize;
}

void
DBSCreateDatabase (const D_CHAR* const pName,
                  const D_CHAR*        pDbsDirectory,
                  D_UINT64             maxFileSize)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  if (pDbsDirectory == NULL)
    pDbsDirectory = DBSGetWorkingDir ();

  if (maxFileSize == 0)
    maxFileSize = DBSGetMaxFileSize ();

  if (maxFileSize < MINIMUM_MAX_FILE_SIZE)
    maxFileSize = MINIMUM_MAX_FILE_SIZE;

  string fileName;

  if ((pName != NULL) && whc_is_path_absolute (pDbsDirectory))
    {
      fileName += pDbsDirectory;
      fileName += whc_get_directory_delimiter ();
    }
  else
    {
      fileName += DBSGetWorkingDir ();
      fileName += pDbsDirectory;
      fileName += whc_get_directory_delimiter ();
    }

  fileName += pName;
  fileName += DBS_FILE_EXT;

  WFile             dbsFile (fileName.c_str (), WHC_FILECREATE_NEW | WHC_FILEWRITE);
  auto_ptr<D_UINT8> apBufferHeader (new D_UINT8[PS_DBS_HEADER_SIZE]);
  D_UINT8* const    cpHeader = apBufferHeader.get ();

  memcpy (cpHeader + PS_DBS_SIGNATURE_OFF, DBS_FILE_SIGNATURE, PS_DBS_SIGNATURE_LEN);

  *_RC (D_UINT16*, cpHeader + PS_DBS_VER_MAJ_OFF)    = PS_DBS_VER_MAJ;
  *_RC (D_UINT16*, cpHeader + PS_DBS_VER_MIN_OFF)    = PS_DBS_VER_MIN;
  *_RC (D_UINT16*, cpHeader + PS_DBS_DIRECTORY_OFF)  = PS_DBS_HEADER_SIZE;
  *_RC (D_UINT16*, cpHeader + PS_DBS_NUM_TABLES_OFF) = 0;
  *_RC (D_UINT64*, cpHeader + PS_DBS_MAX_FILE_OFF)   = maxFileSize;

  dbsFile.Write (cpHeader, PS_DBS_HEADER_SIZE);
  dbsFile.Write (_RC (const D_UINT8*, pDbsDirectory), strlen (pDbsDirectory) + 1);
}

I_DBSHandler&
DBSRetrieveDatabase (const D_CHAR* const pName)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII syncHolder (apDbsManager_->m_Sync);

  DATABASES_MAP&          rMap = apDbsManager_->m_Databases;
  DATABASES_MAP::iterator it   = rMap.find (pName);

  if (it == rMap.end ())
    {
      rMap.insert (pair<string, DbsElement> (pName, DbsElement (DbsHandler (string (pName)))));
      it = rMap.find (pName);
      assert (it != rMap.end ());
    }

  it->second.m_RefCount++;

  return it->second.m_Dbs;
}

void
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

void
DBSRemoveDatabase (const D_CHAR* const pName)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII syncHolder (apDbsManager_->m_Sync);

  DATABASES_MAP&          rMap = apDbsManager_->m_Databases;
  DATABASES_MAP::iterator it   = rMap.find (pName);

  if (it == rMap.end ())
    {
      rMap.insert (pair<string, DbsElement> (pName, DbsElement (DbsHandler (string (pName)))));
      it = rMap.find (pName);
      assert (it != rMap.end ());
    }

  if (it->second.m_RefCount != 0)
    throw DBSException (NULL, _EXTRA (DBSException::DATABASE_IN_USE));

  it->second.m_Dbs.RemoveFromStorage ();
  rMap.erase (it);
}


DbsHandler::DbsHandler (const string& name) :
    I_DBSHandler (),
    m_MaxFileSize (0),
    m_Sync (),
    m_Name (name),
    m_DbsDirectory (),
    m_Tables ()
{
  string fileName (DBSGetWorkingDir () + name + DBS_FILE_EXT);
  WFile  inputFile (fileName.c_str (), WHC_FILEOPEN_EXISTING | WHC_FILEREAD);

  auto_ptr<D_UINT8> apBuffer (new D_UINT8[inputFile.GetSize ()]);
  D_UINT8*          pBuffer = apBuffer.get ();

  inputFile.Seek (0, WHC_SEEK_BEGIN);
  inputFile.Read (pBuffer, inputFile.GetSize ());

  if (memcmp (pBuffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
    throw DBSException (NULL, _EXTRA (DBSException::INAVLID_DATABASE));

  const D_UINT16 versionMaj = *_RC (D_UINT16*, pBuffer + PS_DBS_VER_MAJ_OFF);
  const D_UINT16 versionMin = *_RC (D_UINT16*, pBuffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ) ||
      ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
    throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));

  m_MaxFileSize  = *_RC (D_UINT64*, pBuffer + PS_DBS_MAX_FILE_OFF);
  m_DbsDirectory = string (_RC (D_CHAR*, pBuffer) +
                           *_RC (D_UINT16*, pBuffer + PS_DBS_DIRECTORY_OFF));

  D_UINT16 tablesCount = *_RC (D_UINT16*, pBuffer + PS_DBS_NUM_TABLES_OFF);

  pBuffer += *_RC (D_UINT16*, pBuffer + PS_DBS_DIRECTORY_OFF) + m_DbsDirectory.size () + 1;
  while (tablesCount-- > 0)
    {
      m_Tables.insert (pair < string, PersistentTable* >(_RC (D_CHAR*, pBuffer), NULL));
      pBuffer += strlen (_RC (D_CHAR*, pBuffer));
    }

}

DbsHandler::DbsHandler (const DbsHandler& source) :
    m_MaxFileSize (source.m_MaxFileSize),
    m_Sync (),
    m_Name (source.m_Name),
    m_DbsDirectory (source.m_DbsDirectory),
    m_Tables (source.m_Tables)
{
}

DbsHandler::~DbsHandler ()
{
  Discard ();
}

TABLE_INDEX
DbsHandler::PesistentTablesCount ()
{
  return m_Tables.size ();
}

I_DBSTable&
DbsHandler::RetrievePersistentTable (TABLE_INDEX index)
{
  WSynchronizerRAII syncHolder (m_Sync);

  if (index >= m_Tables.size ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  TABLES::iterator it = m_Tables.begin ();

  while (index-- > 0)
    {
      assert (it != m_Tables.end ());
      ++it;
    }

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  return *it->second;

}

I_DBSTable&
DbsHandler::RetrievePersistentTable (const D_CHAR* pTableName)
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
DbsHandler::AddTable (const D_CHAR* const pTableName,
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

  m_Tables.insert (pair<string, PersistentTable*> (tableName, NULL));
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

void
DbsHandler::DeleteTable (const D_CHAR* const pTableName)
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
  D_UINT8 aBuffer[PS_DBS_HEADER_SIZE];

  memcpy (aBuffer, DBS_FILE_SIGNATURE, PS_DBS_SIGNATURE_LEN);

  *_RC (D_UINT16*, aBuffer + PS_DBS_VER_MAJ_OFF)    = PS_DBS_VER_MAJ;
  *_RC (D_UINT16*, aBuffer + PS_DBS_VER_MIN_OFF)    = PS_DBS_VER_MIN;
  *_RC (D_UINT16*, aBuffer + PS_DBS_DIRECTORY_OFF)  = PS_DBS_HEADER_SIZE;
  *_RC (D_UINT16*, aBuffer + PS_DBS_NUM_TABLES_OFF) = m_Tables.size ();
  *_RC (D_UINT64*, aBuffer + PS_DBS_MAX_FILE_OFF)   = MaxFileSize ();

  string fileName (m_DbsDirectory + m_Name + DBS_FILE_EXT);
  WFile outFile (fileName.c_str (), WHC_FILECREATE | WHC_FILEWRITE);
  outFile.SetSize (0);
  outFile.Write (aBuffer, sizeof aBuffer);
  outFile.Write (_RC (const D_UINT8*, m_DbsDirectory.c_str ()), m_DbsDirectory.size () + 1);

  for (TABLES::iterator it = m_Tables.begin (); it != m_Tables.end (); ++it)
    outFile.Write (_RC (const D_UINT8 *, it->first.c_str ()), it->first.size () + 1);
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
  const string fileName (m_DbsDirectory + m_Name + DBS_FILE_EXT);
  whc_fremove (fileName.c_str ());
}
