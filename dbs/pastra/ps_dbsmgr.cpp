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

using namespace pastra;
using namespace std;


static const D_CHAR DBS_FILE_EXT[] = ".pd";
static const D_CHAR DBS_FILE_SIGNATURE[] =
{
  0x50, 0x41, 0x53, 0x54,
  0x52, 0x41, 0x20, 0x44
};

static const D_UINT16 PS_DBS_VER_MAJ = 0;
static const D_UINT16 PS_DBS_VER_MIN = 10;

static const D_UINT PS_DBS_SIGNATURE_OFF = 0;
static const D_UINT PS_DBS_SIGNATURE_LEN = 8;
static const D_UINT PS_DBS_VER_MAJ_OFF = 8;
static const D_UINT PS_DBS_VER_MAJ_LEN = 2;
static const D_UINT PS_DBS_VER_MIN_OFF = 10;
static const D_UINT PS_DBS_VER_MIN_LEN = 2;
static const D_UINT PS_DBS_NUM_TABLES_OFF = 12;
static const D_UINT PS_DBS_NUM_TABLES_LEN = 2;

static const D_UINT PS_DBS_HEADER_SIZE = 14;

struct DbsElement
{
  D_UINT64 mRefCount;
  DbsHandler mDbs;

  DbsElement (const DbsHandler & rDbs):mRefCount (0), mDbs (rDbs)
  {
  }
}
;

typedef map < string, DbsElement > DATABASES_MAP;

class DbsManager
{
  friend void DBSInit (const D_CHAR * const, const D_CHAR * const, D_UINT64 maxFileSize);
  friend void DBSShoutdown ();
  friend class auto_ptr < DbsManager >;
  friend const D_CHAR *DBSGetWorkingDir ();
  friend const D_CHAR *DBSGetTempDir ();
  friend D_UINT64 DBSGetMaxFileSize ();
  friend I_DBSHandler & DBSRetrieveDatabase (const D_CHAR * const);
  friend void DBSReleaseDatabase (I_DBSHandler &);
  friend void DBSRemoveDatabase (const D_CHAR * const pName);

private:
  DbsManager (const D_CHAR * const pDBSDirectory, const D_CHAR * const pTempDir, D_UINT64 maxFileSize) :
      mSync (),
      mWorkingDir (pDBSDirectory),
      mTempDir (pTempDir),
      mDatabases (),
      m_MaxFileSize (maxFileSize)
  {

    if (pDBSDirectory[strlen (pDBSDirectory) - 1] !=
        whc_get_directory_delimiter ()[0])
      mWorkingDir += whc_get_directory_delimiter ();

    if (pTempDir[strlen (pTempDir) - 1] != whc_get_directory_delimiter ()[0])
      mTempDir += whc_get_directory_delimiter ();
  }

  ~DbsManager ()
  {
  }

  WSynchronizer mSync;
  string mWorkingDir;
  string mTempDir;
  DATABASES_MAP mDatabases;
  D_UINT64 m_MaxFileSize;
};

static auto_ptr < DbsManager > apDbsManager_;

void
DBSInit (const D_CHAR * const pDBSDirectory, const D_CHAR * const pTempDir, D_UINT64 maxFileSize)
{
  if (apDbsManager_.get () != NULL)
    throw DBSException (NULL, _EXTRA (DBSException::ALLREADY_INITED));

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

const D_CHAR *
DBSGetWorkingDir ()
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  return apDbsManager_->mWorkingDir.c_str ();
}

const D_CHAR *
DBSGetTempDir ()
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  return apDbsManager_->mTempDir.c_str ();
}

D_UINT64
DBSGetMaxFileSize ()
{
  return apDbsManager_->m_MaxFileSize;
}

void
DBSCreateDatabase (const D_CHAR * const pName)
{
  string fileName (DBSGetWorkingDir ());
  fileName += pName;
  fileName += DBS_FILE_EXT;

  WFile dbsFile (fileName.c_str (), WHC_FILECREATE_NEW | WHC_FILEWRITE);
  auto_ptr < D_UINT8 > apBufferHeader (new D_UINT8[PS_DBS_HEADER_SIZE]);
  D_UINT8 *const cpHeader = apBufferHeader.get ();

  memcpy (cpHeader + PS_DBS_SIGNATURE_OFF,
          DBS_FILE_SIGNATURE, PS_DBS_SIGNATURE_LEN);
  *_RC (D_UINT16 *, cpHeader + PS_DBS_VER_MAJ_OFF) = PS_DBS_VER_MAJ;
  *_RC (D_UINT16 *, cpHeader + PS_DBS_VER_MIN_OFF) = PS_DBS_VER_MIN;

  *_RC (D_UINT16 *, cpHeader + PS_DBS_NUM_TABLES_OFF) = 0;

  dbsFile.Write (cpHeader, PS_DBS_HEADER_SIZE);
}

I_DBSHandler & DBSRetrieveDatabase (const D_CHAR * const pName)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII syncHolder (apDbsManager_->mSync);

  DATABASES_MAP & rMap = apDbsManager_->mDatabases;
  DATABASES_MAP::iterator it = rMap.find (pName);

  if (it == rMap.end ())
    {
      rMap.insert (pair < string, DbsElement > (pName, DbsElement (DbsHandler (string (pName)))));
      it = rMap.find (pName);
      assert (it != rMap.end ());
    }

  it->second.mRefCount++;
  return it->second.mDbs;
}

void
DBSReleaseDatabase (I_DBSHandler & hndDatabase)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII  syncHolder (apDbsManager_->mSync);

  DATABASES_MAP & rMap = apDbsManager_->mDatabases;
  DATABASES_MAP::iterator it;

  for (it = rMap.begin (); it != rMap.end (); ++it)
    {
      if (_SC (I_DBSHandler *, &it->second.mDbs) == &hndDatabase)
        {
          assert (it->second.mRefCount > 0);

          if (--it->second.mRefCount == 0)
            {
              it->second.mDbs.Discard ();
              rMap.erase (it);
            }
          break;
        }
    }
}

void
DBSRemoveDatabase (const D_CHAR * const pName)
{
  if (apDbsManager_.get () == NULL)
    throw DBSException (NULL, _EXTRA (DBSException::NOT_INITED));

  //Acquire the DBS's manager lock!
  WSynchronizerRAII syncHolder (apDbsManager_->mSync);

  DATABASES_MAP & rMap = apDbsManager_->mDatabases;
  DATABASES_MAP::iterator it = rMap.find (pName);

  if (it == rMap.end ())
    {
      rMap.insert (pair < string, DbsElement > (pName, DbsElement (DbsHandler (string (pName)))));
      it = rMap.find (pName);
      assert (it != rMap.end ());
    }

  if (it->second.mRefCount != 0)
    throw DBSException (NULL, _EXTRA (DBSException::DATABASE_IN_USE));

  it->second.mDbs.RemoveFromStorage ();
  rMap.erase (it);
}


DbsHandler::DbsHandler (const string & name):
    I_DBSHandler (),
    mName (name),
    mTables ()
{
  string fileName (DBSGetWorkingDir () + name + DBS_FILE_EXT);

  WFile inputFile (fileName.c_str (), WHC_FILEOPEN_EXISTING | WHC_FILEREAD);

  auto_ptr < D_UINT8 >
  apBuffer (new D_UINT8[inputFile.GetSize ()]);
  D_UINT8 *pBuffer = apBuffer.get ();

  inputFile.Seek (0, WHC_SEEK_BEGIN);
  inputFile.Read (pBuffer, inputFile.GetSize ());

  if (memcmp (pBuffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
    throw DBSException (NULL, _EXTRA (DBSException::INAVLID_DATABASE));

  const D_UINT16 versionMaj = *_RC (D_UINT16 *, pBuffer + PS_DBS_VER_MAJ_OFF);
  const D_UINT16 versionMin = *_RC (D_UINT16 *, pBuffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ) ||
      ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
    throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));

  D_UINT16 tablesCount = *_RC (D_UINT16 *, pBuffer + PS_DBS_NUM_TABLES_OFF);
  pBuffer += PS_DBS_HEADER_SIZE;
  while (tablesCount-- > 0)
    {
      mTables.insert (pair < string, PSTable * >(_RC (D_CHAR *, pBuffer), NULL));
      pBuffer += strlen (_RC (D_CHAR *, pBuffer));
    }

}

DbsHandler::~DbsHandler ()
{
  Discard ();
}

D_UINT DbsHandler::GetPesistentTablesCount ()
{
  return mTables.size ();
}

I_DBSTable & DbsHandler::RetrievePersistentTable (D_UINT index)
{
  if (index >= mTables.size ())
    throw
    DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  TABLES::iterator it = mTables.begin ();

  while (index-- > 0)
    {
      assert (it != mTables.end ());
      ++it;
    }

  if (it->second == NULL)
    it->second = new PSTable (*this, it->first);

  it->second->m_ReferenceCount++;
  return *it->second;

}

I_DBSTable & DbsHandler::RetrievePersistentTable (const D_CHAR * pTableName)
{
  TABLES::iterator it = mTables.find (pTableName);

  if (it == mTables.end ())
    throw
    DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  if (it->second == NULL)
    it->second = new PSTable (*this, it->first);

  it->second->m_ReferenceCount++;
  return *it->second;
}

void
DbsHandler::AddTable (const D_CHAR* const       pTableName,
                      const DBSFieldDescriptor* pFields,
                      const D_UINT              fieldsCount)
{

  if ((pTableName == NULL) || (pFields == NULL) || (fieldsCount == 0))
    throw DBSException (NULL, _EXTRA(DBSException::INVALID_PARAMETERS));

  string tableName (pTableName);
  TABLES::iterator it = mTables.find (tableName);


  if (it != mTables.end ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_EXISTS));

  mTables.insert (pair < string, PSTable * >(tableName, NULL));
  it = mTables.find (tableName);
  try
    {
      it->second = new PSTable (*this, it->first, pFields, fieldsCount);
    }
  catch (...)
    {
      mTables.erase (it);
      throw;
    }

  SyncToFile ();
}

void
DbsHandler::ReleaseTable (I_DBSTable & hndTable)
{
  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    if (&hndTable == _SC (I_DBSTable *, it->second))
      {
        if (--it->second->m_ReferenceCount == 0)
          {
            delete it->second;
            it->second = NULL;
          }
        return;
      }
}

void
DbsHandler::DeleteTable (const D_CHAR * const pTableName)
{
  TABLES::iterator it = mTables.find (pTableName);

  if (it == mTables.end ())
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_NOT_FOUND));

  if (it->second == NULL)
    it->second = new PSTable (*this, it->first);

  if (it->second->m_ReferenceCount > 0)
    throw DBSException (NULL, _EXTRA (DBSException::TABLE_IN_USE));

  PSTable *const cpTable = it->second;
  cpTable->RemoveFromDatabase ();
  delete cpTable;

  mTables.erase (it);
  SyncToFile ();
}

I_DBSTable*
DbsHandler::CreateTempTable (const DBSFieldDescriptor* pFields,
                             const D_UINT              fieldsCount)
{
  return new PSTemporalTable (*this, pFields, fieldsCount);
}

void
DbsHandler::Discard ()
{
  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
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

  *_RC (D_UINT16 *, aBuffer + PS_DBS_VER_MAJ_OFF) = PS_DBS_VER_MAJ;
  *_RC (D_UINT16 *, aBuffer + PS_DBS_VER_MIN_OFF) = PS_DBS_VER_MIN;

  *_RC (D_UINT16 *, aBuffer + PS_DBS_NUM_TABLES_OFF) = mTables.size ();

  string fileName (DBSGetWorkingDir () + mName + DBS_FILE_EXT);
  WFile outFile (fileName.c_str (), WHC_FILECREATE | WHC_FILEWRITE);
  outFile.SetSize (0);
  outFile.Write (aBuffer, sizeof aBuffer);

  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    outFile.Write (_RC (const D_UINT8 *, it->first.c_str ()),
                   strlen (it->first.c_str ()) + 1);
}

void
DbsHandler::RemoveFromStorage ()
{
  //Discard all tables first!
  Discard ();

  //Remove all tables from database
  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    {
      assert (it->first.c_str () != NULL);
      assert (it->second == NULL);

      auto_ptr<PSTable> table (new PSTable (*this, it->first));
      table->RemoveFromDatabase ();
    }

  //Remove the database file
  const string fileName (DBSGetWorkingDir () + mName + DBS_FILE_EXT);
  whc_fremove (fileName.c_str ());
}
