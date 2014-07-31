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

#include "dbs/dbs_exception.h"
#include "utils/wfile.h"
#include "utils/wthread.h"
#include "utils/endianness.h"

#include "ps_dbsmgr.h"
#include "ps_table.h"

using namespace std;

namespace whisper {
namespace pastra {

static const char DBS_FILE_EXT[]       = ".db";
static const char DBS_FILE_SIGNATURE[] = { 0x50, 0x41, 0x53, 0x54,
                                           0x52, 0x41, 0x20, 0x44 };

static const uint16_t PS_DBS_VER_MAJ          = 1;
static const uint16_t PS_DBS_VER_MIN          = 0;

static const uint_t PS_DBS_SIGNATURE_OFF      = 0;
static const uint_t PS_DBS_SIGNATURE_LEN      = 8;
static const uint_t PS_DBS_VER_MAJ_OFF        = 8;
static const uint_t PS_DBS_VER_MAJ_LEN        = 2;
static const uint_t PS_DBS_VER_MIN_OFF        = 10;
static const uint_t PS_DBS_VER_MIN_LEN        = 2;
static const uint_t PS_DBS_NUM_TABLES_OFF     = 14;
static const uint_t PS_DBS_NUM_TABLES_LEN     = 2;
static const uint_t PS_DBS_MAX_FILE_OFF       = 16;
static const uint_t PS_DBS_MAX_FILE_LEN       = 8;
static const uint_t PS_DBS_FLAGS_OFF          = 24;
static const uint_t PS_DBS_FLAGS_LEN          = 8;

static const uint_t PS_DBS_HEADER_SIZE        = 32;

static const uint64_t PS_FLAG_NOT_CLOSED      = 1;
static const uint64_t PS_FLAG_TO_REPAIR       = 2;


static auto_ptr<DbsManager> dbsMgrs_;

DbsHandler::DbsHandler (const DBSSettings&    settings,
                        const std::string&    locationDir,
                        const std::string&    name)
  : IDBSHandler (),
    mGlbSettings (settings),
    mSync (),
    mDbsLocationDir (locationDir),
    mName (name),
    mTables (),
    mCreatedTemporalTables (0)
{
  string fileName = mDbsLocationDir + mName + DBS_FILE_EXT;
  File   inputFile (fileName.c_str (), WH_FILEOPEN_EXISTING | WH_FILERDWR);

  const uint_t      fileSize = inputFile.Size ();
  auto_ptr<uint8_t> fileContent (new uint8_t[fileSize]);
  uint8_t*          buffer = fileContent.get ();

  inputFile.Seek (0, WH_SEEK_BEGIN);
  inputFile.Read (buffer, fileSize);

  if (memcmp (buffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
    {
      throw DBSException (_EXTRA (DBSException::INAVLID_DATABASE),
                          "File '%s' does not contains a valid signature.",
                          fileName.c_str ());
    }

  uint64_t headerFlags = load_le_int64 (buffer + PS_DBS_FLAGS_OFF);
  if ((headerFlags & PS_FLAG_NOT_CLOSED)
      && ! (headerFlags & PS_FLAG_TO_REPAIR))
    {
      throw DBSException (_EXTRA (DBSException::DATABASE_IN_USE),
                          "Cannot open database '%s'. Either it is already in"
                           " use or it was not properly closed last time.",
                          name.c_str ());
    }
  headerFlags &= ~PS_FLAG_TO_REPAIR;
  headerFlags |= PS_FLAG_NOT_CLOSED;
  store_le_int64 (headerFlags, buffer + PS_DBS_FLAGS_OFF);

  const uint16_t versionMaj = load_le_int16 (buffer + PS_DBS_VER_MAJ_OFF);
  const uint16_t versionMin = load_le_int16 (buffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ)
      || ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
    {
      throw DBSException (_EXTRA (DBSException::OPER_NOT_SUPPORTED),
                          "Cannot open a database with version %d.%d. "
                            "Maximum supported by this implementation is "
                            "%d.%d",
                          versionMaj,
                          versionMin,
                          PS_DBS_VER_MAJ,
                          PS_DBS_VER_MIN);
    }

  const uint64_t maxFileSize = load_le_int64 (buffer + PS_DBS_MAX_FILE_OFF);

  if (maxFileSize != mGlbSettings.mMaxFileSize)
    {
      throw DBSException (_EXTRA (DBSException::INAVLID_DATABASE),
                          "Database uses a different maximum file size "
                             "than what is parameterized (%lu vs %lu).",
                          _SC (long, maxFileSize),
                          _SC (long, mGlbSettings.mMaxFileSize));
    }

  //Before we continue set the 'in use' flag.
  inputFile.Seek (0, WH_SEEK_BEGIN);
  inputFile.Write (buffer, fileSize);

  uint16_t tablesCount = load_le_int16 (buffer + PS_DBS_NUM_TABLES_OFF);

  buffer += PS_DBS_HEADER_SIZE;

  while (tablesCount-- > 0)
    {
      mTables.insert (
          pair<string, PersistentTable*> (_RC (char*, buffer),
                                          _RC (PersistentTable*, NULL))
                     );
      buffer += strlen (_RC (char*, buffer)) + 1;
    }
}


DbsHandler::DbsHandler (const DbsHandler& source)
  : IDBSHandler (),
    mGlbSettings (source.mGlbSettings),
    mSync (),
    mDbsLocationDir (source.mDbsLocationDir),
    mName (source.mName),
    mTables (source.mTables),
    mCreatedTemporalTables (source.mCreatedTemporalTables)
{
  assert (mCreatedTemporalTables == 0);
}


DbsHandler::~DbsHandler ()
{
  Discard ();
}


TABLE_INDEX
DbsHandler::PersistentTablesCount ()
{
  return mTables.size ();
}


ITable&
DbsHandler::RetrievePersistentTable (const TABLE_INDEX index)
{
  TABLE_INDEX iterator = index;

  LockRAII syncHolder (mSync);

  if (iterator >= mTables.size ())
    {
      throw DBSException (_EXTRA (DBSException::TABLE_NOT_FUND),
                          "Cannot retrieve table by index %u (count %u).",
                          index,
                          mTables.size ());
    }

  TABLES::iterator it = mTables.begin ();

  while (iterator-- > 0)
    {
      assert (it != mTables.end ());

      ++it;
    }

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  else
    {
      throw DBSException (_EXTRA (DBSException::TABLE_IN_USE),
                          "Table with index %u needs to be released before it"
                          " can be retrieved again.",
                          index);
    }

  return *it->second;

}


ITable&
DbsHandler::RetrievePersistentTable (const char* const name)
{
  LockRAII syncHolder (mSync);

  TABLES::iterator it = mTables.find (name);

  if (it == mTables.end ())
    {
      throw DBSException (_EXTRA (DBSException::TABLE_NOT_FUND),
                          "Cannot retrieve table '%s'.",
                          name);
    }

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  else
    {
      throw DBSException (_EXTRA (DBSException::TABLE_IN_USE),
                          "Table '%s' needs to be released before it can be"
                          " retrieved again.",
                          name);
    }

  return *it->second;
}


void
DbsHandler::AddTable (const char* const   name,
                      const FIELD_INDEX   fieldsCount,
                      DBSFieldDescriptor* inoutFields)
{
  LockRAII syncHolder (mSync);

  if ((name == NULL) || (inoutFields == NULL) || (fieldsCount == 0))
    {
      throw DBSException (_EXTRA(DBSException::INVALID_PARAMETERS),
                          "Cannot create persistent table '%s' with %u fields.",
                          (name == NULL) ? "(no name)" : name,
                          (inoutFields == NULL) ? 0 : fieldsCount);
    }

  const string     tableName (name);
  TABLES::iterator it = mTables.find (tableName);

  if (it != mTables.end ())
    {
      throw DBSException (_EXTRA (DBSException::TABLE_EXISTS),
                          "Cannot create table '%s'. It already exists.",
                          name);
    }

  mTables.insert (pair<string, PersistentTable*> (
                                        tableName,
                                        new PersistentTable (*this,
                                                             tableName,
                                                             inoutFields,
                                                             fieldsCount)
                                                 ));
  SyncToFile ();

  //Make sure we can retrieve the table later.
  it = mTables.find (tableName);

  assert (it != mTables.end ());

  delete it->second;
  it->second = NULL;
}


void
DbsHandler::ReleaseTable (ITable& hndTable)
{
  assert (mCreatedTemporalTables >= 0);

  if (& _SC (PrototypeTable&, hndTable).GetDBSHandler () != this)
    {
      throw DBSException (_EXTRA (DBSException::TABLE_INVALID),
                          "Cannot release a table that was created on a"
                          " different database.");
    }

  LockRAII syncHolder (mSync);

  if (hndTable.IsTemporal ())
    {
      assert (mCreatedTemporalTables > 0);

      --mCreatedTemporalTables;

      delete &_SC (TemporalTable&, hndTable);

      return;
    }

  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    {
      if (&hndTable == _SC (ITable*, it->second))
        {
          delete it->second;
          it->second = NULL;

          return;
        }
    }
}


const char*
DbsHandler::TableName (const TABLE_INDEX index)
{
  TABLE_INDEX iterator = index;

  LockRAII syncHolder (mSync);

  if (iterator >= mTables.size ())
    {
      throw DBSException (_EXTRA (DBSException::TABLE_NOT_FUND),
                          "Cannot retrieve table by index %u (count %u).",
                          index,
                          mTables.size ());
    }

  TABLES::iterator it = mTables.begin ();

  while (iterator-- > 0)
    {
      assert (it != mTables.end ());

      ++it;
    }

  return it->first.c_str ();
}


void
DbsHandler::DeleteTable (const char* const name)
{
  LockRAII syncHolder (mSync);

  TABLES::iterator it = mTables.find (name);

  if (it == mTables.end ())
    {
      throw DBSException (_EXTRA (DBSException::TABLE_NOT_FUND),
                          "Cannot delete table '%s'. It was not found.",
                          name);
    }

  if (it->second == NULL)
    it->second = new PersistentTable (*this, it->first);

  PersistentTable* const table = it->second;
  table->RemoveFromDatabase ();
  delete table;

  mTables.erase (it);

  SyncToFile ();
}


ITable&
DbsHandler::CreateTempTable (const FIELD_INDEX   fieldsCount,
                             DBSFieldDescriptor* inoutFields)
{
  ITable* const result = new TemporalTable (*this, inoutFields, fieldsCount);

  LockRAII syncHolder (mSync);

  ++mCreatedTemporalTables;

  return *result;
}


void
DbsHandler::Discard ()
{
  LockRAII syncHolder (mSync);

  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    {
      delete it->second;
      it->second = NULL;
    }
}


void
DbsHandler::SyncToFile ()
{
  const uint8_t zero = 0;

  uint8_t header[PS_DBS_HEADER_SIZE];

  memcpy (header, DBS_FILE_SIGNATURE, PS_DBS_SIGNATURE_LEN);

  store_le_int16 (PS_DBS_VER_MAJ, header + PS_DBS_VER_MAJ_OFF);
  store_le_int16 (PS_DBS_VER_MIN, header + PS_DBS_VER_MIN_OFF);
  store_le_int16 (mTables.size (), header + PS_DBS_NUM_TABLES_OFF);
  store_le_int64 (MaxFileSize (), header + PS_DBS_MAX_FILE_OFF);
  store_le_int64 (PS_FLAG_NOT_CLOSED, header + PS_DBS_FLAGS_OFF);

  const string fileName (mDbsLocationDir + mName + DBS_FILE_EXT);
  File outFile (fileName.c_str (), WH_FILECREATE | WH_FILEWRITE);

  outFile.Size (0);
  outFile.Write (header, sizeof header);

  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    {
      outFile.Write (_RC (const uint8_t*, it->first.c_str ()),
                     it->first.length () + 1);
    }

  outFile.Write (&zero, 1);
}


bool
DbsHandler::HasUnreleasedTables ()
{
  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    {
      if (it->second != NULL)
        return true;
    }

  return mCreatedTemporalTables > 0;
}


void
DbsHandler::RegisterTableSpawn ()
{
  LockRAII syncHolder (mSync);

  ++mCreatedTemporalTables;
}


void
DbsHandler::RemoveFromStorage ()
{
  Discard ();

  for (TABLES::iterator it = mTables.begin (); it != mTables.end (); ++it)
    {
      assert (it->first.c_str () != NULL);
      assert (it->second == NULL);

      auto_ptr<PersistentTable> table (new PersistentTable (*this, it->first));

      table->RemoveFromDatabase ();
    }

  const string fileName (mDbsLocationDir + mName + DBS_FILE_EXT);

  whf_remove (fileName.c_str ());
}



} //namespace pastra



using namespace pastra;



DBS_SHL void
DBSInit (const DBSSettings& settings)
{
  if (dbsMgrs_.get () != NULL)
    {
      throw DBSException (_EXTRA (DBSException::ALREADY_INITED),
                          "DBS framework was already initialized.");
    }

  dbsMgrs_.reset (new DbsManager (settings));
}


DBS_SHL void
DBSShoutdown ()
{
  if (dbsMgrs_.get () == NULL)
    {
      throw DBSException (_EXTRA (DBSException::NOT_INITED),
                          "DBS framework is not initialized.");
    }

  //~DbsManager() will be called automatically!
  dbsMgrs_.reset (NULL);
}


DBS_SHL const DBSSettings&
DBSGetSeettings ()
{
  if (dbsMgrs_.get () == NULL)
    {
      throw DBSException (_EXTRA (DBSException::NOT_INITED),
                          "DBS framework is not initialized.");
    }

  return dbsMgrs_->mDBSSettings;
}


DBS_SHL void
DBSCreateDatabase (const char* const name,
                   const char*       path)
{
  if (dbsMgrs_.get () == NULL)
    {
      throw DBSException (_EXTRA (DBSException::NOT_INITED),
                          "DBS framework is not initialized.");
    }

  if (path == NULL)
    path = dbsMgrs_->mDBSSettings.mWorkDir.c_str ();

  string fileName = path;
  fileName += name;
  fileName += DBS_FILE_EXT;

  File dbsFile (fileName.c_str (), WH_FILECREATE_NEW | WH_FILEWRITE);

  auto_ptr<uint8_t> header(new uint8_t[PS_DBS_HEADER_SIZE]);
  uint8_t* const    buffer = header.get ();

  memset (buffer, 0, PS_DBS_HEADER_SIZE);
  memcpy (buffer + PS_DBS_SIGNATURE_OFF,
          DBS_FILE_SIGNATURE,
          PS_DBS_SIGNATURE_LEN);

  store_le_int16 (PS_DBS_VER_MAJ, buffer + PS_DBS_VER_MAJ_OFF);
  store_le_int16 (PS_DBS_VER_MIN, buffer + PS_DBS_VER_MIN_OFF);
  store_le_int16 (0, buffer + PS_DBS_NUM_TABLES_OFF);
  store_le_int64 (dbsMgrs_->mDBSSettings.mMaxFileSize,
                  buffer + PS_DBS_MAX_FILE_OFF);

  dbsFile.Write (buffer, PS_DBS_HEADER_SIZE);
}


DBS_SHL bool
DBSValidateDatabase (const char* const name,
                     const char*       path)
{
  if (path == NULL)
    path = dbsMgrs_->mDBSSettings.mWorkDir.c_str ();

  const string fileName = string (path) + name + DBS_FILE_EXT;
  File         inputFile (fileName.c_str (),
                          WH_FILEOPEN_EXISTING | WH_FILERDWR);

  const uint_t      fileSize = inputFile.Size ();
  auto_ptr<uint8_t> fileContent (new uint8_t[fileSize]);
  uint8_t* const    buffer = fileContent.get ();

  inputFile.Seek (0, WH_SEEK_BEGIN);
  inputFile.Read (buffer, fileSize);

  if (memcmp (buffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
    return false;

  uint64_t headerFlags = load_le_int64 (buffer + PS_DBS_FLAGS_OFF);
  if (headerFlags & PS_FLAG_NOT_CLOSED)
    return false;

  const uint16_t versionMaj = load_le_int16 (buffer + PS_DBS_VER_MAJ_OFF);
  const uint16_t versionMin = load_le_int16 (buffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ)
      || ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
    {
      return false;
    }

  uint16_t       actualCount  = 0;
  const uint16_t tablesCount  = load_le_int16 (buffer + PS_DBS_NUM_TABLES_OFF);
  const char*    tableName    = _RC (const char*, buffer + PS_DBS_HEADER_SIZE);
  while (*tableName != 0)
   {
      ++actualCount;

      if ( ! PersistentTable::ValidateTable (path, tableName))
        return false;

      tableName += strlen (tableName) + 1;
    }

  if (tablesCount != actualCount)
    return false;

  return true;
}


DBS_SHL bool
DBSRepairDatabase (const char* const            name,
                   const char*                  path,
                   FIX_ERROR_CALLBACK           fixCallback)
{
  if (path == NULL)
    path = dbsMgrs_->mDBSSettings.mWorkDir.c_str ();

  const string fileName = string (path) + name + DBS_FILE_EXT;
  File         inputFile (fileName.c_str (),
                          WH_FILEOPEN_EXISTING | WH_FILERDWR);

  const uint_t      fileSize = inputFile.Size ();
  auto_ptr<uint8_t> fileContent (new uint8_t[fileSize]);
  uint8_t* const    buffer = fileContent.get ();

  inputFile.Seek (0, WH_SEEK_BEGIN);
  inputFile.Read (buffer, fileSize);

  if (memcmp (buffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
    {
      fixCallback (CRITICAL, "Cannot find the signature of the database file!");

      return false;
    }

  const uint16_t versionMaj = load_le_int16 (buffer + PS_DBS_VER_MAJ_OFF);
  const uint16_t versionMin = load_le_int16 (buffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ)
      || ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
    {
      fixCallback (CRITICAL,
                   "Database '%s' format version (%u,%u) is not supported."
                   " Cannot continue to check!",
                   name);
      return false;
    }

  uint64_t headerFlags = load_le_int64 (buffer + PS_DBS_FLAGS_OFF);
  if ((headerFlags & PS_FLAG_TO_REPAIR)
      || (headerFlags & PS_FLAG_NOT_CLOSED))
    {
      fixCallback (INFORMATION,
                   "Database '%s' was not closed properly.",
                   name);
    }

  //Before we continue set the 'in use' flag.
  store_le_int64 (headerFlags | PS_FLAG_TO_REPAIR, buffer + PS_DBS_FLAGS_OFF);
  inputFile.Seek (0, WH_SEEK_BEGIN);
  inputFile.Write (buffer, fileSize);
  inputFile.Close ();

  IDBSHandler&   dbs          = DBSRetrieveDatabase (name, path);
  uint16_t       tablesCount  = load_le_int16 (buffer + PS_DBS_NUM_TABLES_OFF);
  uint16_t       actualCount  = 0;
  const char*    tableName    = _RC (const char*, buffer + PS_DBS_HEADER_SIZE);
  while ((*tableName != 0)
         && ((_RC (const uint8_t*, tableName) - buffer) < fileSize))

    {
      --tablesCount, ++actualCount;

      fixCallback (INFORMATION,
                   "Checking database table '%s' ...",
                   tableName);
      try
      {
          if (! PersistentTable::RepairTable (_SC (DbsHandler&, dbs),
                                              tableName,
                                              path,
                                              fixCallback))
            {
              return false;
            }
      }
      catch (...)
      {
          DBSReleaseDatabase (dbs);
          return false;
      }

      tableName += strlen (tableName) + 1;
    }
  DBSReleaseDatabase (dbs);

  if (tablesCount > 0)
    {
      bool fixError = fixCallback (FIX_QUESTION,
                                   "The database's tables count is not correct."
                                   " It should be set to '%u'.",
                                   actualCount);
      if ( ! fixError)
        return false;

      else
        store_le_int16 (actualCount, buffer + PS_DBS_NUM_TABLES_OFF);
    }

  return true;
}


DBS_SHL IDBSHandler&
DBSRetrieveDatabase (const char* const name, const char* path)
{
  if (dbsMgrs_.get () == NULL)
    {
      throw DBSException (_EXTRA (DBSException::NOT_INITED),
                          "DBS framework is not initialized.");
    }

  LockRAII syncHolder (dbsMgrs_->mSync);

  DbsManager::DATABASES_MAP&          dbses = dbsMgrs_->mDatabases;
  DbsManager::DATABASES_MAP::iterator it    = dbses.find (name);

  if (it == dbses.end ())
    {
      if (path == NULL)
        path = dbsMgrs_->mDBSSettings.mWorkDir.c_str ();

      dbses.insert (
          pair<string, DbsElement> (
                        name,
                        DbsElement (DbsHandler (dbsMgrs_->mDBSSettings,
                                                string (path),
                                                string (name)))
                                   )
                  );
      it = dbses.find (name);

      assert (it != dbses.end ());
    }

  it->second.mRefCount++;

  return it->second.mDbs;
}


DBS_SHL void
DBSReleaseDatabase (IDBSHandler& hnd)
{
  if (dbsMgrs_.get () == NULL)
    {
      throw DBSException (_EXTRA (DBSException::NOT_INITED),
                          "DBS framework is not initialized.");
    }

  LockRAII syncHolder (dbsMgrs_->mSync);

  DbsManager::DATABASES_MAP&          dbses = dbsMgrs_->mDatabases;
  DbsManager::DATABASES_MAP::iterator it;

  for (it = dbses.begin (); it != dbses.end (); ++it)
    {
      if (_SC (IDBSHandler*, &it->second.mDbs) == &hnd)
        {
          if (it->second.mDbs.HasUnreleasedTables ())
            {
              throw DBSException (_EXTRA (DBSException::DATABASE_IN_USE),
                                  "Could not release a database handler with"
                                  " associated persistent tables unreleased.");
            }

          if (--it->second.mRefCount == 0)
            {
              const string fileName (it->second.mDbs.WorkingDir () +
                                       it->first                   +
                                       DBS_FILE_EXT);

              it->second.mDbs.Discard ();

              dbses.erase (it);

              uint8_t header[PS_DBS_HEADER_SIZE];

              File dbFile (fileName.c_str (),
                           WH_FILEOPEN_EXISTING | WH_FILERDWR);

              dbFile.Seek (0, WH_SEEK_BEGIN);
              dbFile.Read (header, sizeof header);

              uint64_t flags = load_le_int64 (header + PS_DBS_FLAGS_OFF);
              flags &= ~(PS_FLAG_NOT_CLOSED | PS_FLAG_TO_REPAIR);
              store_le_int64 (flags, header + PS_DBS_FLAGS_OFF);

              dbFile.Seek (0, WH_SEEK_BEGIN);
              dbFile.Write (header, sizeof header);
            }
          break;
        }
    }
}


DBS_SHL  void
DBSRemoveDatabase (const char* const name, const char* path)
{
  if (dbsMgrs_.get () == NULL)
    {
      throw DBSException (_EXTRA (DBSException::NOT_INITED),
                          "DBS framework is not initialized.");
    }

  //Acquire the DBS's manager lock!
  LockRAII syncHolder (dbsMgrs_->mSync);

  DbsManager::DATABASES_MAP&          dbses = dbsMgrs_->mDatabases;
  DbsManager::DATABASES_MAP::iterator it    = dbses.find (name);

  if (it == dbses.end ())
    {
      if (path == NULL)
        path = dbsMgrs_->mDBSSettings.mWorkDir.c_str ();

      dbses.insert (
          pair<string, DbsElement> (
                        name,
                        DbsElement (DbsHandler (dbsMgrs_->mDBSSettings,
                                                string (path),
                                                string (name)))
                                   )
                  );
      it = dbses.find (name);

      assert (it != dbses.end ());
    }

  if (it->second.mRefCount != 0)
    {
      throw DBSException (
                    _EXTRA (DBSException::DATABASE_IN_USE),
                    "Cannot remove database '%s' because is still in use.",
                    name
                         );
    }

  it->second.mDbs.RemoveFromStorage ();
  dbses.erase (it);
}




} //namespace whisper



#if  defined (ENABLE_MEMORY_TRACE) && defined (USE_DBS_SHL)
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "PASTRA";
#endif

