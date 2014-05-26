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

#ifndef PS_VARSTORAGE_H_
#define PS_VARSTORAGE_H_

#include "whisper.h"

#include "utils/wthread.h"
#include "utils/le_converter.h"

#include "ps_container.h"
#include "ps_blockcache.h"

namespace whisper
{
namespace pastra
{



class StoreEntry
{
public:
  static const uint64_t LAST_DELETED_ENTRY = 0x0FFFFFFFFFFFFFFFull;
  static const uint64_t LAST_CHAINED_ENTRY = 0x0FFFFFFFFFFFFFFFull;
  static const uint64_t ENTRY_DELETED_MASK = 0x8000000000000000ull;
  static const uint64_t FIRST_RECORD_ENTRY = 0x4000000000000000ull;

  static const uint_t   ENTRY_SIZE = 48;

  void MarkAsDeleted (const bool deleted)
  {
    uint64_t entry = load_le_int64 (mNextEntry);

    deleted ? (entry |= ENTRY_DELETED_MASK) : (entry &= ~ENTRY_DELETED_MASK);

    store_le_int64 (entry, mNextEntry);
  }

  void MarkAsFirstEntry (const bool first)
  {
    uint64_t entry = load_le_int64 (mNextEntry);

    first ? (entry |= FIRST_RECORD_ENTRY) : (entry &= ~FIRST_RECORD_ENTRY);

    store_le_int64 (entry, mNextEntry);
  }

  bool IsDeleted () const
  {
    return (load_le_int64 (mNextEntry) & ENTRY_DELETED_MASK) != 0;
  }

  bool IsFirstEntry () const
  {
    return (load_le_int64 (mNextEntry) & FIRST_RECORD_ENTRY) != 0;
  }

  uint64_t PrevEntry () const
  {
    return load_le_int64 (mPrevEntry);
  }

  void PrevEntry (const uint64_t content)
  {
    store_le_int64 (content, mPrevEntry);
  }

  uint64_t NextEntry () const
  {
    return load_le_int64 (mNextEntry) &
             ~(ENTRY_DELETED_MASK | FIRST_RECORD_ENTRY);
  }

  void NextEntry (const uint64_t content)
  {
    uint64_t entry = load_le_int64 (mNextEntry);

    entry &= (ENTRY_DELETED_MASK | FIRST_RECORD_ENTRY);
    entry |= content;

    store_le_int64 (entry, mNextEntry);
  }

  uint_t Read (uint_t offset, uint_t count, uint8_t* buffer) const;

  uint_t Write (uint_t offset, uint_t count, const uint8_t* buffer);

  static uint8_t Size ()
  {
    return ENTRY_SIZE;
  }

private:
  uint8_t  mPrevEntry[8];
  uint8_t  mNextEntry[8];
  uint8_t  mRawData[ENTRY_SIZE];
};



class VariableSizeStore : public IBlocksManager
{
public:
  VariableSizeStore ();

  void RegisterReference ();

  void ReleaseReference ();

  void Init (const char*       tempDir,
             const uint32_t    reservedMem);

  void Init (const char*        baseName,
             const uint64_t     storeSize,
             const uint64_t     maxFileSize);

  void Flush ();

  void MarkForRemoval ();

  uint64_t AddRecord (const uint8_t*      buffer,
                      const uint64_t      size);

  uint64_t AddRecord (VariableSizeStore& sourceStore,
                      uint64_t           sourceFirstEntry,
                      uint64_t           sourceFrom,
                      uint64_t           sourceSize);

  uint64_t AddRecord (IDataContainer&  sourceContainer,
                      uint64_t         sourceFrom,
                      uint64_t         sourceSize);

  void GetRecord (uint64_t    recordFirstEntry,
                  uint64_t    offset,
                  uint64_t    size,
                  uint8_t*    buffer);

  void UpdateRecord (uint64_t          recordFirstEntry,
                     uint64_t          offset,
                     uint64_t          size,
                     const uint8_t*    source);

  void UpdateRecord (uint64_t           recordFirstEntry,
                     uint64_t           offset,
                     VariableSizeStore& sourceStore,
                     uint64_t           sourceFirstEntry,
                     uint64_t           sourceOffset,
                     uint64_t           sourceCount);

  void UpdateRecord (uint64_t           recordFirstEntry,
                     uint64_t           offset,
                     IDataContainer&    sourceContainer,
                     uint64_t           sourceOffset,
                     uint64_t           sourceCount);

  void IncrementRecordRef (const uint64_t recordFirstEntry);

  void DecrementRecordRef (const uint64_t recordFirstEntry);

  uint64_t Size () const;

  virtual void StoreItems (uint64_t          firstItem,
                           uint_t            itemsCount,
                           const uint8_t*    from);

  virtual void RetrieveItems (uint64_t       firstItem,
                              uint_t         itemsCount,
                              uint8_t*       to);

  void PrepareToCheckStorage ();

  bool CheckArrayEntry (const uint64_t   recordFirstEntry,
                        const uint64_t   recordSize,
                        const uint_t     itemSize);

  bool CheckTextEntry (const uint64_t   recordFirstEntry,
                       const uint64_t   recordSize);

  void ConcludeStorageCheck ();

private:
  void FinishInit ();

  uint64_t AllocateEntry (const uint64_t prevEntryId);

  uint64_t ExtentFreeList ();

  void RemoveRecord (uint64_t recordFirstEntry);

  void ExtractFromFreeList (const uint64_t entryId);

  void AddToFreeList (const uint64_t entryId);

  std::auto_ptr<IDataContainer> mEntriesContainer;
  BlockCache                    mEntriesCache;
  uint64_t                      mFirstFreeEntry;
  uint64_t                      mEntriesCount;
  uint64_t                      mRefsCount;
  Lock                          mSync;

  std::vector<bool>             mUsedEntries;
};



} //namespace pastra
} //namespace whisper

#endif /* PS_VARTSORAGE_H_ */
