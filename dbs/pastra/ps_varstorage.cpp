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

#include "dbs_exception.h"
#include "ps_varstorage.h"

using namespace std;

namespace whisper {
namespace pastra {

StoreEntry::StoreEntry () :
    m_PrevEntry (0),
    m_NextEntry (0)
{
}

uint_t
StoreEntry::Read (uint_t offset, uint_t count, uint8_t *pBuffer) const
{

  assert (IsDeleted() == false);

  if (count + offset > Size() )
    count = Size() - offset;

  memcpy (pBuffer, m_aRawData + offset, count);
  return count;
}

uint_t
StoreEntry::Write (uint_t offset, uint_t count, const uint8_t *pBuffer)
{
  assert (IsDeleted() == false);

  if (count + offset > Size() )
    count = Size() - offset;

  memcpy (m_aRawData + offset, pBuffer, count);
  return count;
}


VLVarsStore::VLVarsStore ()
  : I_BlocksManager (),
    m_apEntriesContainer (NULL),
    m_FirstFreeEntry (0),
    m_EntrysCount (0),
    m_RefsCount (0),
    m_Sync ()
{
}

void
VLVarsStore::RegisterReference ()
{
  LockRAII hold (m_Sync);

  ++m_RefsCount;
}

void
VLVarsStore::ReleaseReference ()
{
  {
    LockRAII hold (m_Sync);

    assert (m_RefsCount > 0);

    if (--m_RefsCount > 0)
      return ; //Nothing for us to do here!
  }

  //We are the last ones that hold this object
  delete this;
}

void VLVarsStore::Init (const char*  tempDir,
                        const uint32_t reservedMem)
{
  m_apEntriesContainer.reset (new TempContainer (tempDir, reservedMem));
  m_EntrysCount = 0;

  FinishInit ();
}

void
VLVarsStore::Init (const char*  pContainerBaseName,
                   const uint64_t containerSize,
                   const uint64_t maxFileSize)
{
  assert (maxFileSize != 0);

  const uint64_t unitsCount = (containerSize + maxFileSize- 1) / maxFileSize;

  m_apEntriesContainer.reset (new FileContainer (pContainerBaseName,
                                                 maxFileSize,
                                                 unitsCount));
  m_EntrysCount = m_apEntriesContainer->Size () / sizeof (StoreEntry);

  FinishInit ();
}

void
VLVarsStore::FinishInit ()
{
  if (m_EntrysCount == 0)
    {
      StoreEntry sEntry;

      sEntry.MarkAsDeleted (true);
      sEntry.MarkAsFirstEntry (false);

      sEntry.SetPrevEntry (0);
      sEntry.SetNextEntry (StoreEntry::LAST_DELETED_ENTRY);

      m_apEntriesContainer.get ()->Write (0,
                                          sizeof sEntry,
                                          _RC(uint8_t*, &sEntry));
      m_EntrysCount++;
    }
  uint_t       blkSize  = DBSSettings ().m_VLStoreCacheBlkSize;
  const uint_t blkCount = DBSSettings ().m_VLStoreCacheBlkCount;
  assert ((blkSize != 0) && (blkCount != 0));

  while (blkSize < sizeof (StoreEntry))
    blkSize *= 2;

  m_EntrysCache.Init (*this, sizeof (StoreEntry), blkSize, blkCount);

  StoredItem              cachedItem = m_EntrysCache.RetriveItem (0);
  const StoreEntry* const pEntryHdr  = _RC(const StoreEntry*,
                                           cachedItem.GetDataForRead ());

  assert (pEntryHdr->IsDeleted());
  assert (pEntryHdr->IsFirstEntry() == false);

  m_FirstFreeEntry = pEntryHdr->GetNextEntry ();

  assert ((m_apEntriesContainer->Size() % sizeof (StoreEntry)) == 0);

  assert (m_EntrysCount > 0);
}

void
VLVarsStore::Flush ()
{
  LockRAII syncHolder (m_Sync);

  m_EntrysCache.Flush ();
}

void
VLVarsStore::MarkForRemoval ()
{
  m_apEntriesContainer.get ()->MarkForRemoval ();
}

uint64_t
VLVarsStore::AddRecord (const uint8_t* pBuffer,
                        const uint64_t size)
{
  uint64_t resultEntry = 0;
  {
    LockRAII synchHolder(m_Sync);

    resultEntry            = AllocateEntry (0);
    StoredItem  cachedItem = m_EntrysCache.RetriveItem (resultEntry);
    StoreEntry* pEntryHdr  = _RC (StoreEntry*, cachedItem.GetDataForUpdate ());

    pEntryHdr->MarkAsDeleted (false);
    pEntryHdr->MarkAsFirstEntry (true);

    pEntryHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    pEntryHdr->SetPrevEntry (1);
  }

  if ((resultEntry != 0) && (size > 0))
    {
      assert (pBuffer != NULL);
      UpdateRecord (resultEntry, 0, size, pBuffer);
    }

  return resultEntry;
}

uint64_t
VLVarsStore::AddRecord (VLVarsStore& sourceStore,
                        uint64_t     sourceFirstEntry,
                        uint64_t     sourceOffset,
                        uint64_t     sourceSize)

{
  uint64_t resultEntry = 0;
  {
    LockRAII synchHolder(m_Sync);

    resultEntry            = AllocateEntry (0);
    StoredItem  cachedItem = m_EntrysCache.RetriveItem (resultEntry);
    StoreEntry* pEntryHdr  = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

    pEntryHdr->MarkAsDeleted (false);
    pEntryHdr->MarkAsFirstEntry (true);

    pEntryHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    pEntryHdr->SetPrevEntry (1);
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
VLVarsStore::AddRecord (I_DataContainer& sourceContainer,
                        uint64_t         sourceOffset,
                        uint64_t         sourceSize)

{
  uint64_t resultEntry = 0;
  {
    LockRAII synchHolder(m_Sync);

    resultEntry            = AllocateEntry (0);
    StoredItem  cachedItem = m_EntrysCache.RetriveItem (resultEntry);
    StoreEntry* pEntryHdr  = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

    pEntryHdr->MarkAsDeleted (false);
    pEntryHdr->MarkAsFirstEntry (true);

    pEntryHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    pEntryHdr->SetPrevEntry (1);
  }

  if ((resultEntry != 0) && (sourceSize > 0))
    UpdateRecord (resultEntry, 0, sourceContainer, sourceOffset, sourceSize);

  return resultEntry;
}

void
VLVarsStore::GetRecord (uint64_t recordFirstEntry,
                        uint64_t offset,
                        uint64_t size,
                        uint8_t* pBuffer)
{
  do
    {
      LockRAII synchHolder(m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

      const StoreEntry* cpEntry = _RC (const StoreEntry*,
                                       cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->Size())
        break;

      offset           -= cpEntry->Size();
      recordFirstEntry = cpEntry->GetNextEntry();
    }
  while (true);

  while (size > 0)
    {
      LockRAII synchHolder(m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          throw DBSException (NULL,
                              _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
        }

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

      const StoreEntry* cpEntry = _RC (const StoreEntry*,
                                       cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      const uint64_t chunkSize = min (size, cpEntry->Size() - offset);

      cpEntry->Read (offset, chunkSize, pBuffer);

      size -= chunkSize, pBuffer += chunkSize;
      offset = (offset + chunkSize) % cpEntry->Size();

      recordFirstEntry = cpEntry->GetNextEntry();
    }
}

void
VLVarsStore::UpdateRecord (uint64_t       recordFirstEntry,
                           uint64_t       offset,
                           uint64_t       size,
                           const uint8_t* pBuffer)
{
  uint64_t prevEntry = recordFirstEntry;

  do
    {
      LockRAII synchHolder(m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            {
              throw DBSException (
                              NULL,
                              _EXTRA (DBSException::GENERAL_CONTROL_ERROR)
                                  );
            }
          else
            break;
        }

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

      const StoreEntry* cpEntry = _RC (const StoreEntry*,
                                       cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->Size())
        break;

      offset           -= cpEntry->Size();
      prevEntry        = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();

    }
  while (true);

  while (size > 0)
    {
      LockRAII synchHolder(m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
              recordFirstEntry = AllocateEntry (prevEntry);

      StoredItem  cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

      StoreEntry* cpEntry = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

      assert (cpEntry->IsDeleted() == false);

      const uint64_t chunkSize = cpEntry->Write (offset, size, pBuffer);
      assert (chunkSize > 0);


      size  -= chunkSize, pBuffer += chunkSize;
      offset = (offset + chunkSize) % cpEntry->Size();

      prevEntry = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();
    }
}

void
VLVarsStore::UpdateRecord (uint64_t     recordFirstEntry,
                           uint64_t     offset,
                           VLVarsStore& sourceStore,
                           uint64_t     sourceFirstEntry,
                           uint64_t     sourceOffset,
                           uint64_t     sourceSize)
{
  uint64_t prevEntry       = recordFirstEntry;
  uint64_t sourcePrevEntry = sourceFirstEntry;

  do
    {
      LockRAII synchHolder (m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            {
              throw DBSException (
                            NULL,
                            _EXTRA (DBSException::GENERAL_CONTROL_ERROR)
                                  );
            }
          else
            break;
        }

      StoredItem  cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

      const StoreEntry* cpEntry = _RC (const StoreEntry*,
                                       cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->Size())
        break;

      offset           -= cpEntry->Size();
      prevEntry        = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();

    }
  while (true);

  do
    {
      LockRAII sourceSyncHolder (sourceStore.m_Sync);

      if (sourceFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (sourceOffset != 0)
            {
              throw DBSException (
                            NULL,
                            _EXTRA (DBSException::GENERAL_CONTROL_ERROR)
                                 );
            }
          else
            break;
        }

      StoredItem cachedItem = sourceStore.m_EntrysCache.RetriveItem (
                                                          recordFirstEntry
                                                                    );

      const StoreEntry* cpEntry = _RC (const StoreEntry*,
                                       cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (sourceOffset < cpEntry->Size())
        break;

      sourceOffset     -= cpEntry->Size();
      sourceFirstEntry = cpEntry->GetNextEntry();
    }
  while (true);

  while (sourceSize > 0)
    {
      if (sourceFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      LockRAII synchHolder (m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
              recordFirstEntry = AllocateEntry (prevEntry);

      synchHolder.Release ();

      uint8_t tempBuffer [64];
      uint_t  tempValid = 0;
      {
        LockRAII sourceSynchHolder (sourceStore.m_Sync);

        StoredItem  cachedItem = sourceStore.m_EntrysCache.RetriveItem (
                                                              sourceFirstEntry
                                                                        );
        StoreEntry* cpEntry = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

        assert (cpEntry->Size() <= sizeof tempBuffer);

        tempValid        = cpEntry->Read(sourceOffset, sourceSize, tempBuffer);
        sourcePrevEntry  = sourceFirstEntry;
        sourceFirstEntry = cpEntry->GetNextEntry();
      }

      synchHolder.Acquire ();

      StoredItem  cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      StoreEntry* cpEntry    = _RC (StoreEntry*,
                                    cachedItem.GetDataForUpdate());


      assert (cpEntry->IsDeleted() == false);

      const uint64_t chunkSize = cpEntry->Write (offset,
                                                 tempValid,
                                                 tempBuffer);
      assert (chunkSize > 0);

      sourceSize -= chunkSize;

      offset += chunkSize;
      if (offset >= cpEntry->Size())
        {
          prevEntry = recordFirstEntry;
          recordFirstEntry = cpEntry->GetNextEntry();

          offset -= cpEntry->Size();
        }

      sourceOffset += chunkSize;
      if (sourceOffset < cpEntry->Size())
        {
          /* Revert the sourceFirstEntry! */
          sourceFirstEntry = sourcePrevEntry;
        }
      else
        sourceOffset -= cpEntry->Size(); //Bogus! Is not its 'cpEntry'!

    }
}

void
VLVarsStore::UpdateRecord (uint64_t         recordFirstEntry,
                           uint64_t         offset,
                           I_DataContainer& sourceContainer,
                           uint64_t         sourceOffset,
                           uint64_t         sourceSize)
{

  uint64_t prevEntry = recordFirstEntry;

  do
    {
      LockRAII synchHolder(m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            {
              throw DBSException (
                            NULL,
                            _EXTRA (DBSException::GENERAL_CONTROL_ERROR)
                                 );
            }
          else
            break;
        }

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

      const StoreEntry* cpEntry = _RC (const StoreEntry*,
                                       cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->Size())
        break;

      offset           -= cpEntry->Size();
      prevEntry        = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();

    }
  while (true);

  while (sourceSize > 0)
    {
      LockRAII synchHolder(m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
              recordFirstEntry = AllocateEntry (prevEntry);

      StoredItem  cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

      StoreEntry* cpEntry = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

      assert (cpEntry->IsDeleted() == false);

      uint8_t tempBuffer[64];
      uint_t  tempValid = MIN (cpEntry->Size() - offset, sourceSize);

      assert (cpEntry->Size() < sizeof tempBuffer);

      sourceContainer.Read (sourceOffset, tempValid, tempBuffer);

      const uint64_t chunkSize = cpEntry->Write (offset, tempValid, tempBuffer);
      assert (chunkSize == tempValid);

      sourceSize -= tempValid, sourceOffset += tempValid;
      offset      = (offset + tempValid) % cpEntry->Size();

      prevEntry        = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();
    }
}

void
VLVarsStore::IncrementRecordRef (const uint64_t recordFirstEntry)
{
  LockRAII synchHolder(m_Sync);

  StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

  StoreEntry* const cpEntry = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  assert (cpEntry->IsFirstEntry ());
  assert (cpEntry->IsDeleted () == false);
  assert (cpEntry->GetPrevEntry () > 0);

  cpEntry->SetPrevEntry (cpEntry->GetPrevEntry () + 1);

}

void
VLVarsStore::DecrementRecordRef (const uint64_t recordFirstEntry)
{
  LockRAII synchHolder(m_Sync);

  StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);

  StoreEntry* const cpEntry = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  assert (cpEntry->IsFirstEntry ());
  assert (cpEntry->IsDeleted () == false);

  uint64_t refCount = cpEntry->GetPrevEntry ();

  assert (refCount > 0);

  cpEntry->SetPrevEntry (--refCount);

  if (refCount == 0)
    RemoveRecord (recordFirstEntry);
}

uint64_t
VLVarsStore::Size() const
{
  if (m_apEntriesContainer.get() == NULL)
    return 0;

  return m_apEntriesContainer->Size();
}

void
VLVarsStore::StoreItems (const uint8_t* pSrcBuffer,
                         uint64_t       firstItem,
                         uint_t         itemsCount)
{
  if (firstItem + itemsCount > m_EntrysCount)
    itemsCount = m_EntrysCount - firstItem;

  const uint64_t start = firstItem * sizeof (StoreEntry);
  const uint64_t count = itemsCount * sizeof (StoreEntry);

  m_apEntriesContainer->Write (start, count, pSrcBuffer);
}

void
VLVarsStore::RetrieveItems (uint8_t* pDestBuffer,
                            uint64_t firstItem,
                            uint_t   itemsCount)
{
  if (firstItem + itemsCount > m_EntrysCount)
    itemsCount = m_EntrysCount - firstItem;

  const uint64_t start = firstItem * sizeof (StoreEntry);
  const uint64_t count = itemsCount * sizeof (StoreEntry);

  m_apEntriesContainer->Read (start, count, pDestBuffer);
}

uint64_t
VLVarsStore::AllocateEntry (const uint64_t prevEntry)
{
  uint64_t foundFree = m_FirstFreeEntry;

  //Try to find a neighbor for of the previous entry;
  if ((prevEntry + 1) < m_EntrysCount)
    {
      StoredItem cachedItem = m_EntrysCache.RetriveItem (prevEntry + 1);
      const StoreEntry* pEntHdr = _RC( const StoreEntry*,
                                       cachedItem.GetDataForRead());

      if (pEntHdr->IsDeleted())
        foundFree = prevEntry + 1;
    }
  else if (prevEntry > 1)
    {
      StoredItem cachedItem = m_EntrysCache.RetriveItem (prevEntry - 1);
      const StoreEntry* pEntHdr = _RC(const StoreEntry*,
                                      cachedItem.GetDataForRead());

      if (pEntHdr->IsDeleted())
        foundFree = prevEntry - 1;
    }

  if (foundFree == StoreEntry::LAST_DELETED_ENTRY)
    foundFree = ExtentFreeList ();

  ExtractFromFreeList (foundFree);

  StoredItem  cachedItem = m_EntrysCache.RetriveItem (foundFree);
  StoreEntry* pEntHdr    = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  if (prevEntry > 0)
    {
      StoredItem  prevCachedItem = m_EntrysCache.RetriveItem (prevEntry);
      StoreEntry* pPrevEntHdr    = _RC (StoreEntry*,
                                        prevCachedItem.GetDataForUpdate());


      const uint64_t prevNext = pPrevEntHdr->GetNextEntry();

      assert (pPrevEntHdr->IsDeleted() == false);
      assert (pPrevEntHdr->GetNextEntry() == StoreEntry::LAST_CHAINED_ENTRY);

      pPrevEntHdr->SetNextEntry(foundFree);

      pEntHdr->SetPrevEntry (prevEntry);
      pEntHdr->MarkAsFirstEntry (false);
      pEntHdr->SetNextEntry (prevNext);

      if (pEntHdr->GetNextEntry() != StoreEntry::LAST_CHAINED_ENTRY)
        {
          StoredItem nextCachedItem = m_EntrysCache.RetriveItem (
                                                      pEntHdr->GetNextEntry()
                                                                );
          pEntHdr = _RC (StoreEntry*, nextCachedItem.GetDataForUpdate());

          pEntHdr->SetPrevEntry (foundFree);
        }
    }
  else
    {
      pEntHdr->MarkAsFirstEntry (true);
      pEntHdr->SetPrevEntry (0);
      pEntHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    }

  return foundFree;
}

uint64_t
VLVarsStore::ExtentFreeList ()
{
  assert (m_FirstFreeEntry == StoreEntry::LAST_DELETED_ENTRY);
  StoreEntry addEntry;

  addEntry.MarkAsDeleted (true);
  addEntry.SetPrevEntry (0);
  addEntry.SetNextEntry (StoreEntry::LAST_DELETED_ENTRY);

  addEntry.MarkAsFirstEntry (false);

  uint64_t insertPos = m_apEntriesContainer.get ()->Size();

  m_FirstFreeEntry   = insertPos / sizeof (addEntry);

  //Flush the current content
  m_EntrysCache.FlushItem (m_FirstFreeEntry - 1);

  assert ((insertPos % sizeof (addEntry)) == 0);
  m_apEntriesContainer->Write (insertPos,
                               sizeof (addEntry),
                               _RC(uint8_t*, &addEntry));

  ++m_EntrysCount;

  //Reload the content of item's block.
  m_EntrysCache.RefreshItem (m_FirstFreeEntry);

  StoredItem  cachedItem = m_EntrysCache.RetriveItem (0);
  StoreEntry* pEntHdr    = _RC (StoreEntry*, cachedItem.GetDataForUpdate());

  pEntHdr->SetNextEntry (m_FirstFreeEntry);

  return m_FirstFreeEntry;
}

void
VLVarsStore::RemoveRecord (uint64_t recordFirstEntry)
{
  StoredItem        cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
  const StoreEntry* cpEntry    = _RC (const StoreEntry*,
                                      cachedItem.GetDataForRead());

  assert (cpEntry->IsDeleted () == false);
  assert (cpEntry->IsFirstEntry ());

  while (recordFirstEntry != StoreEntry::LAST_CHAINED_ENTRY)
    {
      StoredItem        cachedItem = m_EntrysCache.RetriveItem (
                                                            recordFirstEntry
                                                               );
      const StoreEntry* cpEntry    = _RC (const StoreEntry*,
                                          cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      const uint64_t currentEntry = recordFirstEntry;
      recordFirstEntry            = cpEntry->GetNextEntry();

      AddToFreeList (currentEntry);
    }
}

void
VLVarsStore::ExtractFromFreeList (const uint64_t freeEntry)
{
  StoredItem  cachedItem = m_EntrysCache.RetriveItem (freeEntry);
  StoreEntry* pEntHdr    = _RC( StoreEntry*, cachedItem.GetDataForUpdate());

  assert (freeEntry != 0);
  assert (pEntHdr->IsDeleted());
  assert (pEntHdr->IsFirstEntry () == false);

  const uint64_t prevEntry = pEntHdr->GetPrevEntry();
  const uint64_t nextEntry = pEntHdr->GetNextEntry();

  assert (prevEntry < m_EntrysCount);
  assert ((nextEntry == StoreEntry::LAST_DELETED_ENTRY)
          || (nextEntry < m_EntrysCount));

  pEntHdr->MarkAsDeleted (false);
  pEntHdr->SetPrevEntry (0);
  pEntHdr->SetNextEntry (0);

  cachedItem = m_EntrysCache.RetriveItem (prevEntry);
  pEntHdr    = _RC( StoreEntry*, cachedItem.GetDataForUpdate());

  assert (pEntHdr->IsDeleted());
  assert (pEntHdr->IsFirstEntry() == false);

  pEntHdr->SetNextEntry (nextEntry);

  if (freeEntry == m_FirstFreeEntry)
    m_FirstFreeEntry = nextEntry;

  if (nextEntry != StoreEntry::LAST_DELETED_ENTRY)
    {
      cachedItem = m_EntrysCache.RetriveItem (nextEntry);
      pEntHdr = _RC( StoreEntry *,cachedItem.GetDataForUpdate());

      assert (pEntHdr->IsDeleted());
      assert (pEntHdr->IsFirstEntry() == false);

      pEntHdr->SetPrevEntry(prevEntry);
    }
}

void
VLVarsStore::AddToFreeList (const uint64_t entry)
{
  assert (m_EntrysCount > entry);

  StoredItem  cachedItem = m_EntrysCache.RetriveItem (entry);
  StoreEntry* pEntHdr    = _RC( StoreEntry *, cachedItem.GetDataForUpdate());

  //Just to have a valid initialization
  StoredItem  neighborCachedItem = cachedItem;
  StoreEntry* pNeighborEntHdr    = NULL;

  assert (entry > 0);
  assert (pEntHdr->IsDeleted() == false);

  pEntHdr->MarkAsDeleted (true);
  pEntHdr->MarkAsFirstEntry (false);

  //Maybe we are lucky! Check to see if we can link to one of our neighbors!
  if ((entry + 1) < m_EntrysCount)
    {
      neighborCachedItem = m_EntrysCache.RetriveItem (entry + 1);
      pNeighborEntHdr     = _RC (StoreEntry*,
                                 neighborCachedItem.GetDataForUpdate());

      if (pNeighborEntHdr->IsDeleted())
        {
          assert (pNeighborEntHdr->IsFirstEntry() == false);

          const uint64_t prevEntry = pNeighborEntHdr->GetPrevEntry();
          pNeighborEntHdr->SetPrevEntry (entry);
          pEntHdr->SetPrevEntry (prevEntry);
          pEntHdr->SetNextEntry (entry + 1);

          neighborCachedItem = m_EntrysCache.RetriveItem (prevEntry);
          pNeighborEntHdr    = _RC (StoreEntry*,
                                    neighborCachedItem.GetDataForUpdate());

          assert (pNeighborEntHdr->IsDeleted());
          assert (pNeighborEntHdr->IsFirstEntry() == false);

          pNeighborEntHdr->SetNextEntry (entry);

          if (m_FirstFreeEntry == (entry + 1))
            {
              assert (prevEntry == 0);
              m_FirstFreeEntry = entry;
            }

          return ;
        }
    }
  else if (entry > 1)
    {
      neighborCachedItem = m_EntrysCache.RetriveItem (entry - 1);
      pNeighborEntHdr    = _RC (StoreEntry*,
                                neighborCachedItem.GetDataForUpdate());

      if (pNeighborEntHdr->IsDeleted())
        {
          assert (pNeighborEntHdr->IsFirstEntry() == false);

          const uint64_t nextEntry = pNeighborEntHdr->GetNextEntry();
          pNeighborEntHdr->SetNextEntry (entry);
          pEntHdr->SetNextEntry (nextEntry);
          pEntHdr->SetPrevEntry (entry - 1);

          if (nextEntry == StoreEntry::LAST_CHAINED_ENTRY)
            return ;

          neighborCachedItem = m_EntrysCache.RetriveItem (nextEntry);
          pNeighborEntHdr    = _RC (StoreEntry*,
                                    neighborCachedItem.GetDataForUpdate());

          assert (pNeighborEntHdr->IsDeleted());
          assert (pNeighborEntHdr->IsFirstEntry() == false);

          pNeighborEntHdr->SetPrevEntry (entry);

          return ;
        }
    }

  //Nothing found close by! Let's add it at the begin.
  if (m_FirstFreeEntry != StoreEntry::LAST_CHAINED_ENTRY)
    {
      neighborCachedItem = m_EntrysCache.RetriveItem (m_FirstFreeEntry);
      pNeighborEntHdr    = _RC (StoreEntry*,
                                neighborCachedItem.GetDataForUpdate());

      assert (pNeighborEntHdr->IsDeleted ());
      assert (pNeighborEntHdr->IsFirstEntry() == false);

     pNeighborEntHdr->SetPrevEntry (entry);
    }

  pEntHdr->SetPrevEntry (0);
  pEntHdr->SetNextEntry( m_FirstFreeEntry);

  m_FirstFreeEntry = entry;
  cachedItem       = m_EntrysCache.RetriveItem (0);
  pEntHdr          = _RC( StoreEntry *, cachedItem.GetDataForUpdate());

  assert (pEntHdr->IsDeleted());
  assert (pEntHdr->IsFirstEntry() == false);

  pEntHdr->SetNextEntry (entry);
}

} //namespace pastra
} //namespace whisper

