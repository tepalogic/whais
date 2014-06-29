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

#include <memory.h>
#include <assert.h>

#include "dbs/dbs_mgr.h"
#include "utils/wutf.h"

#include "dbs_exception.h"
#include "ps_varstorage.h"
#include "ps_textstrategy.h"
#include "ps_serializer.h"

using namespace std;

namespace whisper {
namespace pastra {



typedef bool (*VALUE_VALIDATOR) (const uint8_t* const);


uint_t
StoreEntry::Read (uint_t offset, uint_t count, uint8_t* buffer) const
{
  assert (IsDeleted () == false);

  if (count + offset > Size () )
    count = Size () - offset;

  memcpy (buffer, mRawData + offset, count);

  return count;
}


uint_t
StoreEntry::Write (uint_t offset, uint_t count, const uint8_t* buffer)
{
  assert (IsDeleted() == false);

  if (count + offset > Size () )
    count = Size () - offset;

  memcpy (mRawData + offset, buffer, count);

  return count;
}


VariableSizeStore::VariableSizeStore ()
  : IBlocksManager (),
    mEntriesContainer (NULL),
    mEntriesCache (),
    mFirstFreeEntry (0),
    mEntriesCount (0),
    mRefsCount (0),
    mSync ()
{
}


void
VariableSizeStore::RegisterReference ()
{
  LockRAII hold (mSync);

  ++mRefsCount;
}


void
VariableSizeStore::ReleaseReference ()
{
  {
    LockRAII hold (mSync);

    assert (mRefsCount > 0);

    if (--mRefsCount > 0)
      return ;
  }

  delete this;
}


void VariableSizeStore::Init (const char*     tempDir,
                             const uint32_t   reservedMem)
{
  mEntriesContainer.reset (new TemporalContainer ());
  mEntriesCount = 0;

  FinishInit (true);
}


void
VariableSizeStore::Init (const char*        baseName,
                         const uint64_t     containerSize,
                         const uint64_t     maxFileSize)
{
  assert (maxFileSize != 0);

  const uint64_t unitsCount = (containerSize + maxFileSize - 1) / maxFileSize;

  mEntriesContainer.reset (new FileContainer (baseName,
                                              maxFileSize,
                                              unitsCount));

  mEntriesCount = mEntriesContainer->Size () / sizeof (StoreEntry);

  FinishInit (false);
}


void
VariableSizeStore::PrepareToCheckStorage ()
{
  assert (mUsedEntries.size () == 0);

  //Disable the cache block.
  mEntriesCache.~BlockCache ();
  memset (_RC (void*, &mEntriesCache), 0, sizeof (mEntriesCache));

  assert (mEntriesCount > 0);

  mUsedEntries.resize (mEntriesCount, false);
  mUsedEntries[0] = true; //The first entry is always in use. It holds the
                          //chain head of the removed ones.

  const uint64_t containerSize = mEntriesCount * sizeof (StoreEntry);

  if (containerSize < mEntriesContainer->Size ())
    mEntriesContainer->Colapse (containerSize, mEntriesContainer->Size ());

}


static bool
is_entry_valid (const StoreEntry&    entry,
                const uint64_t       prevEntry,
                const bool           firstEntry,
                const bool           lastEntry)
{
  if (firstEntry && ! entry.IsFirstEntry ())
    return false;

  else if ( ! firstEntry && entry.IsFirstEntry ())
    return false;

  else if (lastEntry && (entry.NextEntry () != StoreEntry::LAST_CHAINED_ENTRY))
    return false;

  return firstEntry ?
          (entry.PrevEntry () >= StoreEntry::FIRST_PREV_ENTRY) :
          (entry.PrevEntry () == prevEntry);
}


bool
VariableSizeStore::CheckArrayEntry (const uint64_t         recordFirstEntry,
                                    const uint64_t         recordSize,
                                    const DBS_FIELD_TYPE   itemType)
{
  if (_SC (int64_t, recordSize) <= Serializer::Size (itemType, true) - 1)
    return false;

  const uint_t itemSize  = Serializer::Size (itemType, false);

  const Serializer::VALUE_VALIDATOR validator =
      Serializer::SelectValidator (itemType);

  uint64_t              currentEntry = recordFirstEntry;
  uint64_t              prevEntry    = 0;
  StoreEntry            vsEntry;
  vector<uint64_t>      entriesUsed;

  if ((recordFirstEntry >= mUsedEntries.size ()) || mUsedEntries[currentEntry])
    return false;

  entriesUsed.push_back (currentEntry);
  mEntriesContainer.get ()->Read (currentEntry * sizeof vsEntry,
                                  sizeof vsEntry,
                                  _RC (uint8_t*, &vsEntry));
  if (vsEntry.IsDeleted ())
    return false;

  uint8_t temp[sizeof (uint64_t)];
  vsEntry.Read (0, sizeof temp, temp);

  const uint64_t itemsCount = load_le_int64 (temp);
  if ((itemsCount == 0)
      || ((recordSize - sizeof (uint64_t)) % itemSize != 0)
      || ((recordSize - sizeof (uint64_t)) / itemSize != itemsCount))
    {
      return false;
    }

  uint64_t   actualSize     = sizeof (uint64_t);
  uint64_t   itemsChecked   = 0;
  uint_t     itemBytesRead  = 0;
  uint8_t    itemBuffer[StoreEntry::ENTRY_SIZE];
  bool       entryValidated = false;
  while (itemsChecked < itemsCount)
    {
      const bool lastEntry = (recordSize - actualSize) <=
                                  (StoreEntry::ENTRY_SIZE -
                                      actualSize % StoreEntry::ENTRY_SIZE);
      if ( ! entryValidated
          && ! is_entry_valid (vsEntry,
                               prevEntry,
                               currentEntry == recordFirstEntry,
                               lastEntry))
        {
          return false;
        }
      else
        entryValidated = true;

      if (itemBytesRead == 0)
        {
          if (itemSize >
              StoreEntry::ENTRY_SIZE - actualSize % StoreEntry::ENTRY_SIZE)
            {
              itemBytesRead = StoreEntry::ENTRY_SIZE -
                              actualSize % StoreEntry::ENTRY_SIZE;
            }
          else
            itemBytesRead = itemSize;

          vsEntry.Read (actualSize % StoreEntry::ENTRY_SIZE,
                        itemBytesRead,
                        itemBuffer);
        }
      else
        {
          assert (itemBytesRead < itemSize);
          assert (actualSize % StoreEntry::ENTRY_SIZE == 0);

          vsEntry.Read (actualSize % StoreEntry::ENTRY_SIZE,
                        itemSize - itemBytesRead,
                        itemBuffer + itemBytesRead);

          itemBytesRead = itemSize;
        }

      actualSize += itemBytesRead;
      if (itemBytesRead == itemSize)
        {
          ++itemsChecked;
          itemBytesRead = 0;

          if ( ! validator (itemBuffer))
            return false;
       }

      if (actualSize % StoreEntry::ENTRY_SIZE == 0)
        {
          prevEntry     = currentEntry;
          currentEntry  = vsEntry.NextEntry ();

          if ((currentEntry >= mEntriesCount)
              || mUsedEntries[currentEntry])
            {
              return false;
            }
          else
            entriesUsed.push_back (currentEntry);

          mEntriesContainer.get ()->Read (currentEntry * sizeof vsEntry,
                                          sizeof vsEntry,
                                          _RC (uint8_t*, &vsEntry));
          entryValidated = false;
        }
    }

  for (size_t i = 0; i < entriesUsed.size (); ++i)
    mUsedEntries[entriesUsed[i]] = true;

  return true;
}


bool
VariableSizeStore::CheckTextEntry (const uint64_t   recordFirstEntry,
                                   const uint64_t   recordSize)
{
  if (_SC (int64_t, recordSize) <= Serializer::Size (T_TEXT, false) - 1)
    return false;

  uint64_t              currentEntry = recordFirstEntry;
  uint64_t              prevEntry    = 0;
  StoreEntry            vsEntry;
  vector<uint64_t>      entriesUsed;

  if ((recordFirstEntry >= mUsedEntries.size ()) || mUsedEntries[currentEntry])
    return false;

  entriesUsed.push_back (currentEntry);
  mEntriesContainer.get ()->Read (currentEntry * sizeof vsEntry,
                                  sizeof vsEntry,
                                  _RC (uint8_t*, &vsEntry));
  if (vsEntry.IsDeleted ())
    return false;

  uint8_t temp[StoreEntry::ENTRY_SIZE];
  vsEntry.Read (0, sizeof temp, temp);

  const uint32_t charsCount  = load_le_int32 (temp);
  const uint32_t charIndex   = load_le_int32 (temp + sizeof (uint32_t));
  const uint32_t charOffset  = load_le_int32 (temp + 2 * sizeof (uint32_t));

  if (charsCount == 0)
    return false;

  uint32_t checkedChars    = 0;
  uint32_t actualSize      = RowFieldText::CACHE_META_DATA_SIZE;

  uint8_t  readCodeUnits   = 0;
  uint8_t  codeUnitsCount  = 0;
  uint8_t  codeUnits[7]; //UTF-8 has a maximum of 6 code units per
  bool     entryValidated  = false;
  while (checkedChars < charsCount)
    {
      const bool lastEntry = (recordSize - actualSize) <=
                                  (StoreEntry::ENTRY_SIZE -
                                      actualSize % StoreEntry::ENTRY_SIZE);

      if ( ! entryValidated
          && ! is_entry_valid (vsEntry,
                               prevEntry,
                               currentEntry == recordFirstEntry,
                               lastEntry))
        {
          return false;
        }
      else
        entryValidated = true;

      if (readCodeUnits == 0)
        {
          codeUnitsCount = wh_utf8_cu_count (
                                      temp[actualSize % StoreEntry::ENTRY_SIZE]
                                                      );
          if ((codeUnitsCount == 0)
              || (actualSize + codeUnitsCount > recordSize))
            {
              return false;
            }

          else if ((checkedChars == charIndex)
                   && (charOffset !=
                       (actualSize - RowFieldText::CACHE_META_DATA_SIZE)))
            {
              return false;
            }

          if (codeUnitsCount >
              StoreEntry::ENTRY_SIZE - actualSize % StoreEntry::ENTRY_SIZE)
            {
              readCodeUnits = StoreEntry::ENTRY_SIZE -
                              actualSize % StoreEntry::ENTRY_SIZE;
            }
          else
            readCodeUnits = codeUnitsCount;

          vsEntry.Read (actualSize % StoreEntry::ENTRY_SIZE,
                        readCodeUnits,
                        codeUnits);
        }
      else
        {
          assert (readCodeUnits < codeUnitsCount);
          assert (actualSize % StoreEntry::ENTRY_SIZE == 0);

          vsEntry.Read (actualSize % StoreEntry::ENTRY_SIZE,
                        codeUnitsCount - readCodeUnits,
                        codeUnits + readCodeUnits);

          readCodeUnits = codeUnitsCount;
        }

      try
      {
          actualSize += readCodeUnits;
          if (readCodeUnits == codeUnitsCount)
            {
              uint32_t codePoint;
              wh_load_utf8_cp (codeUnits, &codePoint);

              //Throw an exception if the code point is not Unicode valid.
              DChar validateCodePoint (codePoint);

              ++checkedChars;
              readCodeUnits = codeUnitsCount = 0;
            }
      }
      catch (...)
      {
          return false;
      }

      if (actualSize % StoreEntry::ENTRY_SIZE == 0)
        {
          prevEntry     = currentEntry;
          currentEntry  = vsEntry.NextEntry ();

          if ((currentEntry >= mEntriesCount)
              || mUsedEntries[currentEntry])
            {
              return false;
            }
          else
            entriesUsed.push_back (currentEntry);

          mEntriesContainer.get ()->Read (currentEntry * sizeof vsEntry,
                                          sizeof vsEntry,
                                          _RC (uint8_t*, &vsEntry));
        }
    }

  for (size_t i = 0; i < entriesUsed.size (); ++i)
    mUsedEntries[entriesUsed[i]] = true;

  return true;
}


void
VariableSizeStore::ConcludeStorageCheck ()
{
  assert (mUsedEntries[0]);
  assert (mUsedEntries.size () == mEntriesCount);

  while (mEntriesCount > 0)
    {
      if (mUsedEntries[mEntriesCount - 1])
        break;

      --mEntriesCount;
    }

  const uint64_t containerSize = mEntriesCount * sizeof (StoreEntry);

  if (containerSize <= mEntriesContainer->Size ())
    mEntriesContainer->Colapse (containerSize, mEntriesContainer->Size ());

  else
    {
      assert (false);
    }

  assert (mEntriesCount >= 1);

  mUsedEntries.resize (mEntriesCount);

  StoreEntry templateEntry;

  mEntriesContainer.get ()->Read (0,
                                  sizeof templateEntry,
                                  _RC (uint8_t*, &templateEntry));

  assert (templateEntry.IsDeleted ());
  assert ( ! templateEntry.IsFirstEntry ());
  assert (templateEntry.PrevEntry () == 0);

  templateEntry.NextEntry (StoreEntry::LAST_DELETED_ENTRY);
  mFirstFreeEntry = StoreEntry::LAST_DELETED_ENTRY;

  uint64_t lastFreeEntry = 0, currentEntry = 1;
  while (currentEntry < mUsedEntries.size ())
    {
      if (mUsedEntries[currentEntry])
        {
          currentEntry++;
          continue;
        }

      templateEntry.NextEntry (currentEntry);

      if (mFirstFreeEntry == StoreEntry::LAST_DELETED_ENTRY)
        mFirstFreeEntry = currentEntry;

      mEntriesContainer.get ()->Write (lastFreeEntry * sizeof templateEntry,
                                       sizeof templateEntry,
                                       _RC (uint8_t*, &templateEntry));

      templateEntry.PrevEntry (lastFreeEntry);
      templateEntry.NextEntry(StoreEntry::LAST_DELETED_ENTRY);

      lastFreeEntry = currentEntry++;
    }

  mEntriesContainer.get ()->Write (lastFreeEntry * sizeof templateEntry,
                                   sizeof templateEntry,
                                   _RC (uint8_t*, &templateEntry));

  _placement_new (_RC (void*, &mEntriesCache), BlockCache ());

  uint_t       blkSize  = DBSSettings ().mVLStoreCacheBlkSize;
  const uint_t blkCount = DBSSettings ().mVLStoreCacheBlkCount;

  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < sizeof (StoreEntry))
    blkSize *= 2;

  mEntriesCache.Init (*this, sizeof (StoreEntry), blkSize, blkCount, false);
}

void
VariableSizeStore::FinishInit (const bool nonPersitentData)
{
  if (mEntriesCount == 0)
    {
      StoreEntry entry;

      entry.MarkAsDeleted (true);
      entry.MarkAsFirstEntry (false);

      entry.PrevEntry (0);
      entry.NextEntry (StoreEntry::LAST_DELETED_ENTRY);

      mEntriesContainer.get ()->Write (0, sizeof entry, _RC (uint8_t*, &entry));

      mEntriesCount++;
    }

  uint_t       blkSize  = DBSSettings ().mVLStoreCacheBlkSize;
  const uint_t blkCount = DBSSettings ().mVLStoreCacheBlkCount;

  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < sizeof (StoreEntry))
    blkSize *= 2;

  mEntriesCache.Init (*this,
                      sizeof (StoreEntry),
                      blkSize,
                      blkCount,
                      nonPersitentData);

  StoredItem              cachedItem = mEntriesCache.RetriveItem (0);
  const StoreEntry* const entry      = _RC (const StoreEntry*,
                                            cachedItem.GetDataForRead ());
  assert (entry->IsDeleted ());
  assert (entry->IsFirstEntry () == false);

  mFirstFreeEntry = entry->NextEntry ();

  assert ((mEntriesContainer->Size () % sizeof (StoreEntry)) == 0);
  assert (mEntriesCount > 0);
}


void
VariableSizeStore::Flush ()
{
  LockRAII sync (mSync);

  mEntriesCache.Flush ();
}


void
VariableSizeStore::MarkForRemoval ()
{
  mEntriesContainer.get ()->MarkForRemoval ();
}


uint64_t
VariableSizeStore::AddRecord (const uint8_t*    buffer,
                              const uint64_t    size)
{
  assert (mUsedEntries.size () == 0);

  uint64_t resultEntry = 0;

  {
    LockRAII sync (mSync);

    resultEntry = AllocateEntry (0);

    StoredItem  cachedItem = mEntriesCache.RetriveItem (resultEntry);
    StoreEntry* entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate ());

    entry->MarkAsDeleted (false);
    entry->MarkAsFirstEntry (true);

    entry->NextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    entry->PrevEntry (StoreEntry::FIRST_PREV_ENTRY);
  }

  if ((resultEntry != 0) && (size > 0))
    {
      assert (buffer != NULL);

      UpdateRecord (resultEntry, 0, size, buffer);
    }

  return resultEntry;
}


uint64_t
VariableSizeStore::AddRecord (VariableSizeStore& sourceStore,
                              uint64_t           sourceFirstEntry,
                              uint64_t           sourceOffset,
                              uint64_t           sourceSize)
{
  assert (mUsedEntries.size () == 0);

  uint64_t resultEntry = 0;

  {
    LockRAII sync (mSync);

    resultEntry = AllocateEntry (0);

    StoredItem  cachedItem = mEntriesCache.RetriveItem (resultEntry);
    StoreEntry* entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

    entry->MarkAsDeleted (false);
    entry->MarkAsFirstEntry (true);

    entry->NextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    entry->PrevEntry (StoreEntry::FIRST_PREV_ENTRY);
  }

  if ((resultEntry != 0) && (sourceSize > 0))
    {
      UpdateRecord (resultEntry,
                    0,
                    sourceStore,
                    sourceFirstEntry,
                    sourceOffset,
                    sourceSize);
    }

  return resultEntry;
}


uint64_t
VariableSizeStore::AddRecord (IDataContainer& sourceContainer,
                              uint64_t        sourceOffset,
                              uint64_t        sourceSize)
{
  assert (mUsedEntries.size () == 0);

  uint64_t resultEntry = 0;

  {
    LockRAII sync (mSync);

    resultEntry = AllocateEntry (0);

    StoredItem  cachedItem = mEntriesCache.RetriveItem (resultEntry);
    StoreEntry* entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

    entry->MarkAsDeleted (false);
    entry->MarkAsFirstEntry (true);

    entry->NextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    entry->PrevEntry (StoreEntry::FIRST_PREV_ENTRY);
  }

  if ((resultEntry != 0) && (sourceSize > 0))
    UpdateRecord (resultEntry, 0, sourceContainer, sourceOffset, sourceSize);

  return resultEntry;
}


void
VariableSizeStore::GetRecord (uint64_t  recordFirstEntry,
                              uint64_t  offset,
                              uint64_t  size,
                              uint8_t*  buffer)
{
  assert (mUsedEntries.size () == 0);

  do
    {
      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());

      assert (entry->IsDeleted() == false);

      if (offset < entry->Size())
        break;

      offset           -= entry->Size();
      recordFirstEntry  = entry->NextEntry();
    }
  while (true);

  while (size > 0)
    {
      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());

      assert (entry->IsDeleted() == false);

      const uint64_t chunkSize = min (size, entry->Size() - offset);

      entry->Read (offset, chunkSize, buffer);

      size -= chunkSize, buffer += chunkSize;

      offset = (offset + chunkSize) % entry->Size ();

      recordFirstEntry = entry->NextEntry ();
    }
}

void
VariableSizeStore::UpdateRecord (uint64_t       recordFirstEntry,
                                 uint64_t       offset,
                                 uint64_t       size,
                                 const uint8_t* buffer)
{
  assert (mUsedEntries.size () == 0);

  uint64_t prevEntry = recordFirstEntry;

  do
    {
      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

          else
            break;
        }

      StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());

      assert (entry->IsDeleted() == false);

      if (offset < entry->Size())
        break;

      offset           -= entry->Size();
      prevEntry         = recordFirstEntry;
      recordFirstEntry  = entry->NextEntry();
    }
  while (true);

  while (size > 0)
    {
      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        recordFirstEntry = AllocateEntry (prevEntry);

      StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

      StoreEntry* const entry = _RC (StoreEntry*,
                                     cachedItem.GetDataForUpdate ());

      assert (entry->IsDeleted() == false);

      const uint64_t chunkSize = entry->Write (offset, size, buffer);
      assert (chunkSize > 0);

      size -= chunkSize, buffer += chunkSize;

      offset = (offset + chunkSize) % entry->Size();

      prevEntry        = recordFirstEntry;
      recordFirstEntry = entry->NextEntry();
    }
}


void
VariableSizeStore::UpdateRecord (uint64_t           recordFirstEntry,
                                 uint64_t           offset,
                                 VariableSizeStore& sourceStore,
                                 uint64_t           sourceFirstEntry,
                                 uint64_t           sourceOffset,
                                 uint64_t           sourceSize)
{
  assert (mUsedEntries.size () == 0);

  uint64_t prevEntry       = recordFirstEntry;
  uint64_t sourcePrevEntry = sourceFirstEntry;

  do
    {
      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

          else
            break;
        }

      StoredItem  cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());

      assert (entry->IsDeleted() == false);

      if (offset < entry->Size())
        break;

      offset           -= entry->Size();
      prevEntry         = recordFirstEntry;
      recordFirstEntry  = entry->NextEntry();

    }
  while (true);

  do
    {
      LockRAII sourceSyncHolder (sourceStore.mSync);

      if (sourceFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (sourceOffset != 0)
            throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

          else
            break;
        }

      StoredItem cachedItem = sourceStore.mEntriesCache.RetriveItem (
                                                          recordFirstEntry
                                                                    );
      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());

      assert (entry->IsDeleted() == false);

      if (sourceOffset < entry->Size())
        break;

      sourceOffset     -= entry->Size();
      sourceFirstEntry  = entry->NextEntry();
    }
  while (true);

  while (sourceSize > 0)
    {
      if (sourceFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        recordFirstEntry = AllocateEntry (prevEntry);

      sync.Release ();

      uint8_t tembuffer [64];
      uint_t  tempValid = 0;

      {
        LockRAII sourceSynchHolder (sourceStore.mSync);

        StoredItem cachedItem = sourceStore.mEntriesCache.RetriveItem (
                                                             sourceFirstEntry
                                                                      );
        StoreEntry* entry = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

        assert (entry->Size() <= sizeof tembuffer);

        tempValid        = entry->Read(sourceOffset, sourceSize, tembuffer);
        sourcePrevEntry  = sourceFirstEntry;
        sourceFirstEntry = entry->NextEntry();
      }

      sync.Acquire ();

      StoredItem  cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);
      StoreEntry* entry      = _RC (StoreEntry*,
                                    cachedItem.GetDataForUpdate());

      assert (entry->IsDeleted() == false);

      const uint64_t chunkSize = entry->Write (offset, tempValid, tembuffer);

      assert (chunkSize > 0);

      sourceSize -= chunkSize, offset += chunkSize;

      if (offset >= entry->Size())
        {
          prevEntry        = recordFirstEntry;
          recordFirstEntry = entry->NextEntry();

          offset -= entry->Size();
        }

      sourceOffset += chunkSize;
      if (sourceOffset < entry->Size())
        sourceFirstEntry = sourcePrevEntry;

      else
        sourceOffset -= entry->Size ();
    }
}


void
VariableSizeStore::UpdateRecord (uint64_t         recordFirstEntry,
                                 uint64_t         offset,
                                 IDataContainer&  sourceContainer,
                                 uint64_t         sourceOffset,
                                 uint64_t         sourceSize)
{
  assert (mUsedEntries.size () == 0);

  uint64_t prevEntry = recordFirstEntry;

  do
    {
      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));

          else
            break;
        }

      StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());

      assert (entry->IsDeleted() == false);

      if (offset < entry->Size())
        break;

      offset           -= entry->Size();
      prevEntry         = recordFirstEntry;
      recordFirstEntry  = entry->NextEntry();

    }
  while (true);

  while (sourceSize > 0)
    {
      LockRAII sync (mSync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        recordFirstEntry = AllocateEntry (prevEntry);

      StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

      StoreEntry* entry = _RC (StoreEntry*, cachedItem.GetDataForUpdate ());

      assert (entry->IsDeleted() == false);

      uint8_t tembuffer[64];
      uint_t  tempValid = MIN (entry->Size() - offset, sourceSize);

      assert (entry->Size() < sizeof tembuffer);

      sourceContainer.Read (sourceOffset, tempValid, tembuffer);

      const uint64_t chunkSize = entry->Write (offset, tempValid, tembuffer);

      (void)chunkSize;
      assert (chunkSize == tempValid);

      sourceSize -= tempValid, sourceOffset += tempValid;

      offset = (offset + tempValid) % entry->Size();

      prevEntry        = recordFirstEntry;
      recordFirstEntry = entry->NextEntry();
    }
}


void
VariableSizeStore::IncrementRecordRef (const uint64_t recordFirstEntry)
{
  LockRAII sync (mSync);

  StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

  StoreEntry* const entry = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  assert (entry->IsFirstEntry ());
  assert (entry->IsDeleted () == false);
  assert (entry->PrevEntry () > 0);

  entry->PrevEntry (entry->PrevEntry () + 1);
}


void
VariableSizeStore::DecrementRecordRef (const uint64_t recordFirstEntry)
{
  LockRAII sync (mSync);

  StoredItem cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);

  StoreEntry* const entry = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  assert (entry->IsFirstEntry ());
  assert (entry->IsDeleted () == false);

  uint64_t refCount = entry->PrevEntry ();

  assert (refCount > 0);

  entry->PrevEntry (--refCount);

  if (refCount == 0)
    RemoveRecord (recordFirstEntry);
}


uint64_t
VariableSizeStore::Size () const
{
  if (mEntriesContainer.get () == NULL)
    return 0;

  return mEntriesContainer->Size ();
}


void
VariableSizeStore::StoreItems (uint64_t       firstItem,
                               uint_t         itemsCount,
                               const uint8_t* from)
{
  if (firstItem + itemsCount > mEntriesCount)
    itemsCount = mEntriesCount - firstItem;

  const uint64_t start = firstItem * sizeof (StoreEntry);
  const uint64_t count = itemsCount * sizeof (StoreEntry);

  mEntriesContainer->Write (start, count, from);
}


void
VariableSizeStore::RetrieveItems (uint64_t    firstItem,
                                  uint_t      itemsCount,
                                  uint8_t*    to)
{
  if (firstItem + itemsCount > mEntriesCount)
    itemsCount = mEntriesCount - firstItem;

  const uint64_t start = firstItem * sizeof (StoreEntry);
  const uint64_t count = itemsCount * sizeof (StoreEntry);

  mEntriesContainer->Read (start, count, to);
}


uint64_t
VariableSizeStore::AllocateEntry (const uint64_t prevEntryId)
{
  uint64_t foundFree = mFirstFreeEntry;

  //Try to find a free neighbor of the previous entry;
  if ((prevEntryId + 1) < mEntriesCount)
    {
      StoredItem cachedItem   = mEntriesCache.RetriveItem (prevEntryId + 1);
      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());
      if (entry->IsDeleted())
        foundFree = prevEntryId + 1;
    }
  else if (prevEntryId > 1)
    {
      StoredItem cachedItem   = mEntriesCache.RetriveItem (prevEntryId - 1);
      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());
      if (entry->IsDeleted())
        foundFree = prevEntryId - 1;
    }

  if (foundFree == StoreEntry::LAST_DELETED_ENTRY)
    foundFree = ExtentFreeList ();

  ExtractFromFreeList (foundFree);

  StoredItem  cachedItem = mEntriesCache.RetriveItem (foundFree);
  StoreEntry* entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  if (prevEntryId > 0)
    {
      StoredItem  prevCachedItem = mEntriesCache.RetriveItem (prevEntryId);
      StoreEntry* prevEntry      = _RC (StoreEntry*,
                                        prevCachedItem.GetDataForUpdate());

      const uint64_t prevNext = prevEntry->NextEntry();

      assert (prevEntry->IsDeleted() == false);
      assert (prevEntry->NextEntry() == StoreEntry::LAST_CHAINED_ENTRY);

      prevEntry->NextEntry(foundFree);

      entry->PrevEntry (prevEntryId);
      entry->MarkAsFirstEntry (false);
      entry->NextEntry (prevNext);

      if (entry->NextEntry() != StoreEntry::LAST_CHAINED_ENTRY)
        {
          StoredItem nextCachedItem = mEntriesCache.RetriveItem (
                                                      entry->NextEntry()
                                                                );
          entry = _RC (StoreEntry*, nextCachedItem.GetDataForUpdate());

          entry->PrevEntry (foundFree);
        }
    }
  else
    {
      entry->MarkAsFirstEntry (true);
      entry->PrevEntry (0);
      entry->NextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    }

  return foundFree;
}


uint64_t
VariableSizeStore::ExtentFreeList ()
{
  assert (mFirstFreeEntry == StoreEntry::LAST_DELETED_ENTRY);
  StoreEntry newEntry;

  newEntry.MarkAsDeleted (true);
  newEntry.PrevEntry (0);
  newEntry.NextEntry (StoreEntry::LAST_DELETED_ENTRY);

  newEntry.MarkAsFirstEntry (false);

  const uint64_t insertPos = mEntriesContainer.get ()->Size ();

  mFirstFreeEntry = insertPos / sizeof (newEntry);

  //Flush the current content
  mEntriesCache.FlushItem (mFirstFreeEntry - 1);

  assert ((insertPos % sizeof (newEntry)) == 0);
  mEntriesContainer->Write (insertPos,
                            sizeof (newEntry),
                            _RC(uint8_t*, &newEntry));
  ++mEntriesCount;

  //Reload the content of item's block.
  mEntriesCache.RefreshItem (mFirstFreeEntry);

  StoredItem  cachedItem = mEntriesCache.RetriveItem (0);
  StoreEntry* entry      = _RC (StoreEntry*,
                                cachedItem.GetDataForUpdate());

  entry->NextEntry (mFirstFreeEntry);

  return mFirstFreeEntry;
}


void
VariableSizeStore::RemoveRecord (uint64_t recordFirstEntry)
{
  assert (mUsedEntries.size () == 0);

  StoredItem        cachedItem = mEntriesCache.RetriveItem (recordFirstEntry);
  const StoreEntry* entry      = _RC (const StoreEntry*,
                                      cachedItem.GetDataForRead());

  (void)entry;
  assert (entry->IsDeleted () == false);
  assert (entry->IsFirstEntry ());

  while (recordFirstEntry != StoreEntry::LAST_CHAINED_ENTRY)
    {
      StoredItem cachedItem   = mEntriesCache.RetriveItem (recordFirstEntry);
      const StoreEntry* entry = _RC (const StoreEntry*,
                                     cachedItem.GetDataForRead());

      assert (entry->IsDeleted() == false);

      const uint64_t currentEntry = recordFirstEntry;
      recordFirstEntry            = entry->NextEntry();

      AddToFreeList (currentEntry);
    }
}


void
VariableSizeStore::ExtractFromFreeList (const uint64_t entryId)
{
  StoredItem  cachedItem = mEntriesCache.RetriveItem (entryId);
  StoreEntry* entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  assert (entryId != 0);
  assert (entry->IsDeleted ());
  assert (entry->IsFirstEntry () == false);

  const uint64_t prevEntry = entry->PrevEntry();
  const uint64_t nextEntry = entry->NextEntry();

  assert (prevEntry < mEntriesCount);
  assert ((nextEntry == StoreEntry::LAST_DELETED_ENTRY)
          || (nextEntry < mEntriesCount));

  entry->MarkAsDeleted (false);
  entry->PrevEntry (0);
  entry->NextEntry (0);

  cachedItem = mEntriesCache.RetriveItem (prevEntry);
  entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate ());

  assert (entry->IsDeleted ());
  assert (entry->IsFirstEntry () == false);

  entry->NextEntry (nextEntry);

  if (entryId == mFirstFreeEntry)
    mFirstFreeEntry = nextEntry;

  if (nextEntry != StoreEntry::LAST_DELETED_ENTRY)
    {
      cachedItem = mEntriesCache.RetriveItem (nextEntry);
      entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate ());

      assert (entry->IsDeleted ());
      assert (entry->IsFirstEntry () == false);

      entry->PrevEntry (prevEntry);
    }
}


void
VariableSizeStore::AddToFreeList (const uint64_t entryId)
{
  assert (mEntriesCount > entryId);

  StoredItem  cachedItem = mEntriesCache.RetriveItem (entryId);
  StoreEntry* entry      = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  //Just to have a valid initialization
  StoredItem  neighborCachedItem = cachedItem;
  StoreEntry* neighborEntry      = NULL;

  assert (entry > 0);
  assert (entry->IsDeleted () == false);

  entry->MarkAsDeleted (true);
  entry->MarkAsFirstEntry (false);

  //Maybe we are lucky! Check to see if we can link to one of our neighbors!
  if ((entryId + 1) < mEntriesCount)
    {
      neighborCachedItem = mEntriesCache.RetriveItem (entryId + 1);
      neighborEntry      = _RC (StoreEntry*,
                                neighborCachedItem.GetDataForUpdate());

      if (neighborEntry->IsDeleted ())
        {
          assert (neighborEntry->IsFirstEntry() == false);

          const uint64_t prevEntry = neighborEntry->PrevEntry();

          neighborEntry->PrevEntry (entryId);
          entry->PrevEntry (prevEntry);
          entry->NextEntry (entryId + 1);

          neighborCachedItem = mEntriesCache.RetriveItem (prevEntry);
          neighborEntry      = _RC (StoreEntry*,
                                    neighborCachedItem.GetDataForUpdate());

          assert (neighborEntry->IsDeleted ());
          assert (neighborEntry->IsFirstEntry () == false);

          neighborEntry->NextEntry (entryId);

          if (mFirstFreeEntry == (entryId + 1))
            {
              assert (prevEntry == 0);

              mFirstFreeEntry = entryId;
            }

          return ;
        }
    }
  else if (entryId > 1)
    {
      neighborCachedItem = mEntriesCache.RetriveItem (entryId - 1);
      neighborEntry      = _RC (StoreEntry*,
                                neighborCachedItem.GetDataForUpdate());

      if (neighborEntry->IsDeleted ())
        {
          assert (neighborEntry->IsFirstEntry() == false);

          const uint64_t nextEntry = neighborEntry->NextEntry();

          neighborEntry->NextEntry (entryId);
          entry->NextEntry (nextEntry);
          entry->PrevEntry (entryId - 1);

          if (nextEntry == StoreEntry::LAST_CHAINED_ENTRY)
            return ;

          neighborCachedItem = mEntriesCache.RetriveItem (nextEntry);
          neighborEntry      = _RC (StoreEntry*,
                                    neighborCachedItem.GetDataForUpdate());

          assert (neighborEntry->IsDeleted());
          assert (neighborEntry->IsFirstEntry() == false);

          neighborEntry->PrevEntry (entryId);

          return ;
        }
    }

  //No neighbors! Let's add it at the front of the free entries chain.
  if (mFirstFreeEntry != StoreEntry::LAST_CHAINED_ENTRY)
    {
      neighborCachedItem = mEntriesCache.RetriveItem (mFirstFreeEntry);
      neighborEntry      = _RC (StoreEntry*,
                                neighborCachedItem.GetDataForUpdate());

      assert (neighborEntry->IsDeleted ());
      assert (neighborEntry->IsFirstEntry() == false);

     neighborEntry->PrevEntry (entryId);
    }

  entry->PrevEntry (0);
  entry->NextEntry( mFirstFreeEntry);

  mFirstFreeEntry = entryId;
  cachedItem      = mEntriesCache.RetriveItem (0);
  entry           = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  assert (entry->IsDeleted());
  assert (entry->IsFirstEntry() == false);

  entry->NextEntry (entryId);
}

} //namespace pastra
} //namespace whisper

