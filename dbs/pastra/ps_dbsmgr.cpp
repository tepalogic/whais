/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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
******************************************************************************/

#include <map>
#include <memory>
#include <memory.h>
#include <assert.h>
#include <stdio.h>

#include "dbs/dbs_exception.h"
#include "utils/wfile.h"
#include "utils/wthread.h"
#include "utils/endianness.h"
#include "ps_dbsmgr.h"
#include "ps_table.h"


using namespace std;

namespace whais {
namespace pastra {


static const char DBS_FILE_EXT[]       = ".db";
static const char DBS_FILE_SIGNATURE[] = { 0x50, 0x41, 0x53, 0x54, 0x52, 0x41, 0x20, 0x44 };

static const uint16_t PS_DBS_VER_MAJ   = 1;
static const uint16_t PS_DBS_VER_MIN   = 1;

static const uint_t PS_DBS_SIGNATURE_OFF    = 0;
static const uint_t PS_DBS_SIGNATURE_LEN    = 8;
static const uint_t PS_DBS_VER_MAJ_OFF      = 8;
static const uint_t PS_DBS_VER_MAJ_LEN      = 2;
static const uint_t PS_DBS_VER_MIN_OFF      = 10;
static const uint_t PS_DBS_VER_MIN_LEN      = 2;
static const uint_t PS_DBS_NUM_TABLES_OFF   = 14;
static const uint_t PS_DBS_NUM_TABLES_LEN   = 2;
static const uint_t PS_DBS_MAX_FILE_OFF     = 16;
static const uint_t PS_DBS_MAX_FILE_LEN     = 8;
static const uint_t PS_DBS_FLAGS_OFF        = 24;
static const uint_t PS_DBS_FLAGS_LEN        = 8;
static const uint_t PS_DBS_HEADER_SIZE      = 32;

static const uint64_t PS_FLAG_NOT_CLOSED   = 1;
static const uint64_t PS_FLAG_TO_REPAIR    = 2;

static unique_ptr<DbsManager> dbsMgrs_;


DbsHandler::DbsHandler(const DBSSettings&   settings,
                       const std::string&   locationDir,
                       const std::string&   name)
  : mGlbSettings(settings),
    mDbsLocationDir(locationDir),
    mFileName(mDbsLocationDir + name + DBS_FILE_EXT),
    mFile(mFileName.c_str(), WH_FILEOPEN_EXISTING | WH_FILERDWR | WH_FILESYNC),
    mCreatedTemporalTables(0),
    mNeedsSync(false)
{
  const uint_t fileSize = mFile.Size();
  unique_ptr<uint8_t[]> fileContent(unique_array_make(uint8_t, fileSize));
  uint8_t* buffer = fileContent.get();

  mFile.Seek(0, WH_SEEK_BEGIN);
  mFile.Read(buffer, fileSize);

  if (memcmp(buffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
  {
    throw DBSException(_EXTRA(DBSException::INAVLID_DATABASE),
                       "File '%s' does not contains a valid signature.",
                       mFileName.c_str());
  }

  uint64_t headerFlags = load_le_int64(buffer + PS_DBS_FLAGS_OFF);
  if ((headerFlags & PS_FLAG_NOT_CLOSED)
      && ! (headerFlags & PS_FLAG_TO_REPAIR))
  {
    throw DBSException(_EXTRA(DBSException::DATABASE_IN_USE),
                        "Cannot open database '%s'. Either it is already in use or it was not"
                          " properly closed last time.",
                        name.c_str());
  }
  headerFlags &= ~PS_FLAG_TO_REPAIR;
  store_le_int64(headerFlags, buffer + PS_DBS_FLAGS_OFF);

  const uint16_t versionMaj = load_le_int16(buffer + PS_DBS_VER_MAJ_OFF);
  const uint16_t versionMin = load_le_int16(buffer + PS_DBS_VER_MIN_OFF);

  if (versionMaj > PS_DBS_VER_MAJ
      || (versionMaj == PS_DBS_VER_MAJ && versionMin > PS_DBS_VER_MIN))
  {
    throw DBSException(_EXTRA(DBSException::OPER_NOT_SUPPORTED),
                       "Cannot open a database with version %d.%d. Maximum supported by this"
                         " implementation is %d.%d",
                       versionMaj,
                       versionMin,
                       PS_DBS_VER_MAJ,
                       PS_DBS_VER_MIN);
  }

  const uint64_t maxFileSize = load_le_int64(buffer + PS_DBS_MAX_FILE_OFF);
  if (maxFileSize != mGlbSettings.mMaxFileSize)
  {
    throw DBSException(_EXTRA(DBSException::INAVLID_DATABASE),
                       "Database uses a different maximum file size than what is"
                         " parameterized(%lu vs %lu).",
                       _SC(long, maxFileSize),
                       _SC(long, mGlbSettings.mMaxFileSize));
  }

  //Before we continue set the 'in use' flag.
  mFile.Seek(0, WH_SEEK_BEGIN);
  mFile.Write(buffer, fileSize);

  uint16_t tablesCount = load_le_int16(buffer + PS_DBS_NUM_TABLES_OFF);
  buffer += PS_DBS_HEADER_SIZE;
  while (tablesCount-- > 0)
  {
    mTables.insert(pair<string, TABLE_DATA>(_RC(char*, buffer),
                                            make_tuple<PersistentTable*, uint32_t>(nullptr, 0)));
    buffer += strlen(_RC(char*, buffer)) + 1;
  }
}

DbsHandler::DbsHandler(DbsHandler&& source)
  : mGlbSettings(move(source.mGlbSettings)),
    mDbsLocationDir(move(source.mDbsLocationDir)),
    mFileName(move(source.mFileName)),
    mFile(move(source.mFile)),
    mTables(move(source.mTables)),
    mCreatedTemporalTables(move(source.mCreatedTemporalTables)),
    mNeedsSync(move(source.mNeedsSync))
{
  assert(mCreatedTemporalTables == 0);
}

DbsHandler::~DbsHandler()
{
  Discard();
}

TABLE_INDEX
DbsHandler::PersistentTablesCount()
{
  return mTables.size();
}

ITable&
DbsHandler::RetrievePersistentTable(const TABLE_INDEX index)
{
  TABLE_INDEX iterator = index;

  LockGuard<Lock> syncHolder(mSync);

  if (iterator >= mTables.size())
  {
    throw DBSException(_EXTRA(DBSException::TABLE_NOT_FUND),
                       "Cannot retrieve table by index %u(count %u).",
                       index,
                       mTables.size());
  }

  auto it = mTables.begin();
  while (iterator-- > 0)
  {
    assert(it != mTables.end());
    ++it;
  }

  if (get<0>(it->second) == nullptr)
  {
    assert(get<1>(it->second) == 0);

    get<0>(it->second) = new PersistentTable(*this, it->first);
    get<1>(it->second) = 1;
  }
  else
    ++get<1>(it->second) += 1;

  return *get<0>(it->second);
}

ITable&
DbsHandler::RetrievePersistentTable(const char* const name)
{
  LockGuard<Lock> syncHolder(mSync);

  auto it = mTables.find(name);
  if (it == mTables.end())
    throw DBSException(_EXTRA(DBSException::TABLE_NOT_FUND), "Cannot retrieve table '%s'.", name);

  if (get<0>(it->second) == nullptr)
  {
    assert(get<1>(it->second) == 0);

    get<0>(it->second) = new PersistentTable(*this, it->first);
    get<1>(it->second) = 1;
  }
  else
    ++get<1>(it->second) += 1;

  return *get<0>(it->second);
}


void
DbsHandler::AddTable(const char* const           name,
                     const FIELD_INDEX           fieldsCount,
                     DBSFieldDescriptor* const   inoutFields)
{
  LockGuard<Lock> syncHolder(mSync);

  if (name == nullptr
      || inoutFields == nullptr
      || fieldsCount == 0)
  {
    throw DBSException(_EXTRA(DBSException::INVALID_PARAMETERS),
                        "Cannot create persistent table '%s' with %u fields.",
                        (name == nullptr) ? "(no name)" : name,
                        (inoutFields == nullptr) ? 0 : fieldsCount);
  }

  const string tableName(name);
  auto it = mTables.find(tableName);

  if (it != mTables.end())
  {
    throw DBSException(_EXTRA(DBSException::TABLE_EXISTS),
                       "Cannot create table '%s'. It already exists.",
                       name);
  }

  unique_ptr<PersistentTable> table(new PersistentTable(*this,
                                                        tableName,
                                                        inoutFields,
                                                        fieldsCount));
  const auto tableData = make_tuple<PersistentTable*, uint32_t>(table.get(), 1);
  mTables.insert(pair<string, TABLE_DATA>(tableName, tableData));
  table.release();
  SyncToFile();

  //Make sure we can retrieve the table later.
  it = mTables.find(tableName);

  assert(it != mTables.end());
  assert(get<0>(it->second) != nullptr);
  assert(get<1>(it->second) == 1);

  delete get<0>(it->second);
  get<0>(it->second) = nullptr;
  get<1>(it->second) = 0;
}


void
DbsHandler::ReleaseTable(ITable& hndTable)
{
  assert(mCreatedTemporalTables >= 0);

  if (&_SC(PrototypeTable&, hndTable).GetDbsHandler() != this)
  {
    throw DBSException(_EXTRA(DBSException::TABLE_INVALID),
                       "Cannot release a table that was created on a different database.");
  }

  LockGuard<Lock> syncHolder(mSync);
  if (hndTable.IsTemporal())
  {
    assert(mCreatedTemporalTables > 0);

    --mCreatedTemporalTables;
    delete &hndTable;
    return;
  }

  for (auto it = mTables.begin(); it != mTables.end(); ++it)
  {
    if (&hndTable == _SC(ITable*, get<0>(it->second)))
    {
      assert (get<1>(it->second) > 0);

      if (--get<1>(it->second) == 0)
      {
        delete get<0>(it->second);
        get<0>(it->second) = nullptr;
        return;
      }
    }
  }
}

const char*
DbsHandler::TableName(const TABLE_INDEX index)
{
  TABLE_INDEX iterator = index;

  LockGuard<Lock> syncHolder(mSync);

  if (iterator >= mTables.size())
  {
    throw DBSException(_EXTRA(DBSException::TABLE_NOT_FUND),
                       "Cannot retrieve table by index %u(count %u).",
                       index,
                       mTables.size());
  }

  auto it = mTables.begin();
  while (iterator-- > 0)
  {
    assert(it != mTables.end());
    ++it;
  }
  return it->first.c_str();
}


void
DbsHandler::DeleteTable(const char* const name)
{
  LockGuard<Lock> syncHolder(mSync);

  auto it = mTables.find(name);
  if (it == mTables.end())
  {
    throw DBSException(_EXTRA(DBSException::TABLE_NOT_FUND),
                       "Cannot delete table '%s'. It was not found.",
                       name);
  }

  if (get<0>(it->second) == nullptr)
  {
    assert (get<1>(it->second) == 0);

    get<0>(it->second) = new PersistentTable(*this, it->first);
    get<1>(it->second) = 1;
  }
  else
  {
    if (get<1>(it->second) > 1)
    {
      throw DBSException(_EXTRA(DBSException::TABLE_IN_USE),
                         "Cannot delete table '%s'. It is still in use.",
                         name);
    }
  }

  assert (get<1>(it->second) == 1);

  unique_ptr<PersistentTable> table {get<0>(it->second)};
  table->RemoveFromDatabase();
  table.reset();
  mTables.erase(it);

  SyncToFile();
}


void
DbsHandler::SyncAllTablesContent()
{
  LockGuard<Lock> _l(mSync);

  if ( ! mNeedsSync)
    return;

  for (auto& table: mTables)
  {
    if (get<0>(table.second) != nullptr)
      get<0>(table.second)->Flush();
  }

  mNeedsSync = false;

  uint8_t flags[sizeof(uint64_t)];

  mFile.Seek(PS_DBS_FLAGS_OFF, WH_SEEK_BEGIN);
  mFile.Read(flags, sizeof flags);

  store_le_int64(load_le_int64(flags) & ~PS_FLAG_NOT_CLOSED, flags);

  mFile.Seek(PS_DBS_FLAGS_OFF, WH_SEEK_BEGIN);
  mFile.Write(flags, sizeof flags);
}

void
DbsHandler::SyncTableContent(const TABLE_INDEX index)
{
  LockGuard<Lock> syncHolder(mSync);

  if (index >= mTables.size())
  {
    throw DBSException(_EXTRA(DBSException::TABLE_NOT_FUND),
                       "Cannot retrieve table by index %u(count %u).",
                       index,
                       mTables.size());
  }

  TABLE_INDEX iterator = index;
  auto it = mTables.begin();
  while (iterator-- > 0)
  {
    assert(it != mTables.end());
    ++it;
  }

  if (get<0>(it->second) != nullptr)
    get<0>(it->second)->Flush();
}

bool
DbsHandler::NotifyDatabaseUpdate(const bool tryDbLock)
{
  LockGuard<Lock> _l(mSync, true);
  if (tryDbLock)
  {
    if (! _l.try_lock())
      return false;
  }
  else
    _l.lock();

  if (mNeedsSync)
    return true;

  mNeedsSync = true;

  uint8_t flags[sizeof(uint64_t)];

  mFile.Seek(PS_DBS_FLAGS_OFF, WH_SEEK_BEGIN);
  mFile.Read(flags, sizeof flags);

  store_le_int64(load_le_int64(flags) | PS_FLAG_NOT_CLOSED, flags);

  mFile.Seek(PS_DBS_FLAGS_OFF, WH_SEEK_BEGIN);
  mFile.Write(flags, sizeof flags);

  return true;
}

ITable&
DbsHandler::CreateTempTable(const FIELD_INDEX   fieldsCount,
                             DBSFieldDescriptor* inoutFields)
{
  ITable* const result = new TemporalTable(*this, inoutFields, fieldsCount);

  LockGuard<Lock> syncHolder(mSync);

  ++mCreatedTemporalTables;

  return *result;
}

void
DbsHandler::Discard()
{
  LockGuard<Lock> syncHolder(mSync);

  for (auto& table : mTables)
  {
    delete get<0>(table.second);
    get<0>(table.second) = nullptr;
    get<1>(table.second) = 0;
  }
}

void
DbsHandler::SyncToFile()
{
  mNeedsSync = true;

  const uint8_t zero = 0;

  uint8_t header[PS_DBS_HEADER_SIZE];

  memcpy(header, DBS_FILE_SIGNATURE, PS_DBS_SIGNATURE_LEN);

  store_le_int16(PS_DBS_VER_MAJ, header + PS_DBS_VER_MAJ_OFF);
  store_le_int16(PS_DBS_VER_MIN, header + PS_DBS_VER_MIN_OFF);
  store_le_int16(mTables.size(), header + PS_DBS_NUM_TABLES_OFF);
  store_le_int64(MaxFileSize(), header + PS_DBS_MAX_FILE_OFF);
  store_le_int64(PS_FLAG_NOT_CLOSED, header + PS_DBS_FLAGS_OFF);

  mFile.Size(0);
  mFile.Seek(0, WH_SEEK_BEGIN);
  mFile.Write(header, sizeof header);

  for (auto& t : mTables)
    mFile.Write(_RC(const uint8_t*, t.first.c_str()), t.first.length() + 1);

  mFile.Write(&zero, 1);
}


bool
DbsHandler::HasUnreleasedTables()
{
  for (TABLES::iterator it = mTables.begin(); it != mTables.end(); ++it)
  {
    if (get<0>(it->second) != nullptr)
    {
      assert (get<1>(it->second) > 0);
      return true;
    }
    else
    {
      assert (get<1>(it->second) == 0);
    }
  }

  return mCreatedTemporalTables > 0;
}

void
DbsHandler::RegisterTableSpawn()
{
  LockGuard<Lock> syncHolder(mSync);

  ++mCreatedTemporalTables;
}

void
DbsHandler::RemoveFromStorage()
{
  Discard();

  for (TABLES::iterator it = mTables.begin(); it != mTables.end(); ++it)
    {
      unique_ptr<PersistentTable> table(unique_make(PersistentTable, *this, it->first));
      table->RemoveFromDatabase();
    }

  mTables.clear();
  whf_remove(mFileName.c_str());
}


} //namespace pastra


using namespace pastra;


DBS_SHL void
DBSInit(const DBSSettings& settings)
{
  if (dbsMgrs_.get() != nullptr)
  {
    throw DBSException(_EXTRA(DBSException::ALREADY_INITED),
                       "DBS framework was already initialized.");
  }
  dbsMgrs_ = unique_make(DbsManager, settings);
}


DBS_SHL void
DBSShoutdown()
{
  if (dbsMgrs_.get() == nullptr)
    throw DBSException(_EXTRA(DBSException::NOT_INITED), "DBS framework is not initialized.");

   dbsMgrs_.release();
}


DBS_SHL const DBSSettings&
DBSGetSeettings()
{
  if (dbsMgrs_.get() == nullptr)
  {
    throw DBSException(_EXTRA(DBSException::NOT_INITED), "DBS framework is not initialized.");
  }

  return dbsMgrs_->mDBSSettings;
}


DBS_SHL void
DBSCreateDatabase(const char* const name, const char* path)
{
  if (dbsMgrs_.get() == nullptr)
    throw DBSException(_EXTRA(DBSException::NOT_INITED), "DBS framework is not initialized.");

  if (path == nullptr)
    path = dbsMgrs_->mDBSSettings.mWorkDir.c_str();

  const string fileName = string(path) + name + DBS_FILE_EXT;
  File dbsFile(fileName.c_str(), WH_FILECREATE_NEW | WH_FILEWRITE);

  unique_ptr<uint8_t[]> header(unique_array_make(uint8_t, PS_DBS_HEADER_SIZE));
  uint8_t* const buffer = header.get();

  memset(buffer, 0, PS_DBS_HEADER_SIZE);
  memcpy(buffer + PS_DBS_SIGNATURE_OFF, DBS_FILE_SIGNATURE, PS_DBS_SIGNATURE_LEN);

  store_le_int16(PS_DBS_VER_MAJ, buffer + PS_DBS_VER_MAJ_OFF);
  store_le_int16(PS_DBS_VER_MIN, buffer + PS_DBS_VER_MIN_OFF);
  store_le_int16(0, buffer + PS_DBS_NUM_TABLES_OFF);
  store_le_int64(dbsMgrs_->mDBSSettings.mMaxFileSize, buffer + PS_DBS_MAX_FILE_OFF);

  dbsFile.Write(buffer, PS_DBS_HEADER_SIZE);
}


DBS_SHL bool
DBSValidateDatabase(const char* const name, const char* path)
{
  if (path == nullptr)
    path = dbsMgrs_->mDBSSettings.mWorkDir.c_str();

  const string fileName = string(path) + name + DBS_FILE_EXT;
  File inputFile(fileName.c_str(), WH_FILEOPEN_EXISTING | WH_FILERDWR);

  const uint_t fileSize = inputFile.Size();
  unique_ptr<uint8_t[]> fileContent(unique_array_make(uint8_t, fileSize));
  uint8_t* const buffer = fileContent.get();

  inputFile.Seek(0, WH_SEEK_BEGIN);
  inputFile.Read(buffer, fileSize);

  if (memcmp(buffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
    return false;

  uint64_t headerFlags = load_le_int64(buffer + PS_DBS_FLAGS_OFF);
  if (headerFlags & PS_FLAG_NOT_CLOSED)
    return false;

  const uint16_t versionMaj = load_le_int16(buffer + PS_DBS_VER_MAJ_OFF);
  const uint16_t versionMin = load_le_int16(buffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ)
      || ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
  {
    return false;
  }

  uint16_t actualCount = 0;
  const uint16_t tablesCount = load_le_int16(buffer + PS_DBS_NUM_TABLES_OFF);
  const char* tableName = _RC(const char*, buffer + PS_DBS_HEADER_SIZE);
  while (*tableName != 0)
 {
    ++actualCount;

    if ( ! PersistentTable::ValidateTable(path, tableName))
      return false;

    tableName += strlen(tableName) + 1;
  }

  if (tablesCount != actualCount)
    return false;

  return true;
}


DBS_SHL bool
DBSRepairDatabase(const char* const name, const char* path, FIX_ERROR_CALLBACK fixCallback)
{
  if (path == nullptr)
    path = dbsMgrs_->mDBSSettings.mWorkDir.c_str();

  const string fileName = string(path) + name + DBS_FILE_EXT;
  File inputFile(fileName.c_str(), WH_FILEOPEN_EXISTING | WH_FILERDWR);

  const uint64_t fileSize = inputFile.Size();
  unique_ptr<uint8_t[]> fileContent(unique_array_make(uint8_t, fileSize));
  uint8_t* const buffer = fileContent.get();

  inputFile.Seek(0, WH_SEEK_BEGIN);
  inputFile.Read(buffer, fileSize);

  if (memcmp(buffer, DBS_FILE_SIGNATURE, sizeof PS_DBS_SIGNATURE_LEN) != 0)
  {
    fixCallback(CRITICAL, "Cannot find the signature of the database file!");

    return false;
  }

  const uint16_t versionMaj = load_le_int16(buffer + PS_DBS_VER_MAJ_OFF);
  const uint16_t versionMin = load_le_int16(buffer + PS_DBS_VER_MIN_OFF);

  if ((versionMaj > PS_DBS_VER_MAJ)
      || ((versionMaj == PS_DBS_VER_MAJ) && (versionMin > PS_DBS_VER_MIN)))
  {
    fixCallback(CRITICAL,
                "Database '%s' format version(%u,%u) is not supported. Cannot continue to check!",
                name);
    return false;
  }

  uint64_t headerFlags = load_le_int64(buffer + PS_DBS_FLAGS_OFF);
  if ((headerFlags & PS_FLAG_TO_REPAIR)
      || (headerFlags & PS_FLAG_NOT_CLOSED))
    {
      fixCallback(INFORMATION, "Database '%s' was not closed properly.", name);
    }

  //Before we continue set the 'in use' flag.
  store_le_int64(headerFlags | PS_FLAG_TO_REPAIR, buffer + PS_DBS_FLAGS_OFF);
  inputFile.Seek(0, WH_SEEK_BEGIN);
  inputFile.Write(buffer, fileSize);
  inputFile.Close();

  IDBSHandler& dbs = DBSRetrieveDatabase(name, path);
  uint16_t tablesCount = load_le_int16(buffer + PS_DBS_NUM_TABLES_OFF);
  uint16_t actualCount = 0;
  const char* tableName = _RC(const char*, buffer + PS_DBS_HEADER_SIZE);
  while ((*tableName != 0) && (_SC(uint64_t, (_RC(const uint8_t*, tableName) - buffer)) < fileSize))
  {
    --tablesCount, ++actualCount;

    fixCallback(STEP_INFO, " * Checking database table '%s' ...\n", tableName);
    try
    {
      if ( ! PersistentTable::RepairTable(_SC(DbsHandler&, dbs), tableName, path, fixCallback))
        return false;
    }
    catch (...)
    {
      DBSReleaseDatabase(dbs);
      return false;
    }

      tableName += strlen(tableName) + 1;
    }
  DBSReleaseDatabase(dbs);

  if (tablesCount > 0)
  {
    bool fixError = fixCallback(FIX_QUESTION,
                                "The database's tables count is not correct. It should be"
                                  " set to '%u'.",
                                actualCount);
    if (!fixError)
      return false;

    else
      store_le_int16(actualCount, buffer + PS_DBS_NUM_TABLES_OFF);
  }

  return true;
}


DBS_SHL IDBSHandler&
DBSRetrieveDatabase(const char* const name, const char* path)
{
  if (dbsMgrs_.get() == nullptr)
    throw DBSException(_EXTRA(DBSException::NOT_INITED), "DBS framework is not initialized.");

  LockGuard<Lock> syncHolder(dbsMgrs_->mSync);

  auto& dbses = dbsMgrs_->mDatabases;
  auto it = dbses.find(name);
  if (it == dbses.end())
  {
    if (path == nullptr)
      path = dbsMgrs_->mDBSSettings.mWorkDir.c_str();

    dbses.insert(pair<string, DbsElement>(name,
                                          DbsElement(DbsHandler(dbsMgrs_->mDBSSettings,
                                                                path,
                                                                name))));
    it = dbses.find(name);

    assert(it != dbses.end());
  }

  it->second.mRefCount++;

  return it->second.mDbs;
}


DBS_SHL void
DBSReleaseDatabase(IDBSHandler& hnd)
{
  if (dbsMgrs_.get() == nullptr)
    throw DBSException(_EXTRA(DBSException::NOT_INITED), "DBS framework is not initialized.");

  LockGuard<Lock> syncHolder(dbsMgrs_->mSync);

  auto& dbses = dbsMgrs_->mDatabases;
  for (auto it = dbses.begin(); it != dbses.end(); ++it)
  {
    if (_SC(IDBSHandler*, &it->second.mDbs) != &hnd)
      continue ;

    if (it->second.mDbs.HasUnreleasedTables())
    {
      throw DBSException(_EXTRA(DBSException::DATABASE_IN_USE),
                         "Could not release a database handler due to unreleased tables"
                           " associated with it.");
    }

    if (--it->second.mRefCount == 0)
    {
      const string fileName(it->second.mDbs.WorkingDir() + it->first + DBS_FILE_EXT);

      it->second.mDbs.Discard();

      dbses.erase(it);

      uint8_t header[PS_DBS_HEADER_SIZE];

      File dbFile(fileName.c_str(), WH_FILEOPEN_EXISTING | WH_FILERDWR);

      dbFile.Seek(0, WH_SEEK_BEGIN);
      dbFile.Read(header, sizeof header);

      uint64_t flags = load_le_int64(header + PS_DBS_FLAGS_OFF);
      flags &= ~(PS_FLAG_NOT_CLOSED | PS_FLAG_TO_REPAIR);
      store_le_int64(flags, header + PS_DBS_FLAGS_OFF);

      dbFile.Seek(0, WH_SEEK_BEGIN);
      dbFile.Write(header, sizeof header);
    }
    break;
  }
}

DBS_SHL  void
DBSRemoveDatabase(const char* const name, const char* path)
{
  if (dbsMgrs_.get() == nullptr)
    throw DBSException(_EXTRA(DBSException::NOT_INITED), "DBS framework is not initialized.");

  //Acquire the DBS's manager lock!
  LockGuard<Lock> syncHolder(dbsMgrs_->mSync);

  auto& dbses = dbsMgrs_->mDatabases;
  auto it = dbses.find(name);
  if (it == dbses.end())
  {
    if (path == nullptr)
      path = dbsMgrs_->mDBSSettings.mWorkDir.c_str();

    dbses.insert(pair<string, DbsElement>(name,
                                          DbsElement(DbsHandler(dbsMgrs_->mDBSSettings,
                                                                path,
                                                                name))));
    it = dbses.find(name);
    assert(it != dbses.end());
  }

  if (it->second.mRefCount != 0)
  {
    throw DBSException(_EXTRA(DBSException::DATABASE_IN_USE),
                       "Cannot remove database '%s' because is still in use.",
                       name);
  }

  it->second.mDbs.RemoveFromStorage();
  dbses.erase(it);
}


DBS_SHL const char*
DescribeDbsEngineVersion()
{
  static char description[128];

  if (strlen(description) == 0)
  {
    snprintf(description,
             sizeof description,
             "Data Storage Engine: PASTRA version: %u.%02u",
             WVER_MAJ,
             WVER_MIN);
  }
  return description;
}


} //namespace whais


#if  defined(ENABLE_MEMORY_TRACE) && defined(USE_DBS_SHL)
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "PASTRA";
#endif
