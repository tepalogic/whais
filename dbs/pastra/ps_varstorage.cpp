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

#include "dbs_exception.h"
#include "ps_varstorage.h"


using namespace pastra;
using namespace std;

StoreEntry::StoreEntry () :
    m_PrevEntry (0),
    m_NextEntry (0)
{
}

D_UINT
StoreEntry::ReadEntryData (D_UINT offset, D_UINT count, D_UINT8 *pBuffer) const
{

  assert (IsDeleted() == false);

  if (count + offset > GetRawDataSize() )
    count = GetRawDataSize() - offset;

  memcpy (pBuffer, m_aRawData + offset, count);
  return count;
}

D_UINT
StoreEntry::UpdateEntryData (D_UINT offset, D_UINT count, const D_UINT8 *pBuffer)
{

  assert (IsDeleted() == false);

  if (count + offset > GetRawDataSize() )
    count = GetRawDataSize() - offset;

  memcpy (m_aRawData + offset, pBuffer, count);
  return count;
}



VaribaleLenghtStore::VaribaleLenghtStore () :
    I_BlocksManager (),
    m_apEntriesContainer (NULL),
    m_EntrysCache (*this),
    m_FirstFreeEntry (0),
    m_EntrysCount (0),
    m_Sync ()
{
}

VaribaleLenghtStore::~VaribaleLenghtStore ()
{
}

void
VaribaleLenghtStore::Init (
                           const D_CHAR * pContainerBaseName,
                           D_UINT64 uContainerSize, D_UINT uMaxFileSize)
{

  assert (uMaxFileSize != 0);

  const D_UINT64 uUnitsCount = (uContainerSize + uMaxFileSize- 1) / uMaxFileSize;
  m_apEntriesContainer.reset (new FileContainer (pContainerBaseName, uMaxFileSize, uUnitsCount));

  m_EntrysCount = m_apEntriesContainer->GetContainerSize () / sizeof (StoreEntry);

  if (m_EntrysCount == 0)
    {
      StoreEntry sEntry;

      sEntry.MarkAsDeleted();
      sEntry.MarkFirstAsPrev ();

      sEntry.SetPrevEntry (0);
      sEntry.SetNextEntry (StoreEntry::LAST_DELETED_ENTRY);

      m_apEntriesContainer.get ()->StoreData (0, sizeof sEntry, _RC(D_UINT8*, &sEntry));
      m_EntrysCount++;
    }
  m_EntrysCache.Init (sizeof (StoreEntry), 1024, 1024);

  StoredItem cachedItem = m_EntrysCache.RetriveItem (0);
  const StoreEntry *const pEntryHdr = _RC(const StoreEntry *, cachedItem.GetDataForRead ());

  assert (pEntryHdr->IsDeleted());
  assert (pEntryHdr->IsIndexFirst() == false);

  m_FirstFreeEntry = pEntryHdr->GetNextEntry ();

  assert ((m_apEntriesContainer->GetContainerSize() % sizeof (StoreEntry)) == 0);

  assert (m_EntrysCount > 0);
}

void
VaribaleLenghtStore::MarkForRemoval ()
{
  m_apEntriesContainer.get ()->MarkForRemoval ();
}

D_UINT64
VaribaleLenghtStore::AddRecord (
                                const D_UINT64 rowIndex,
                                const D_UINT8 *pBuffer,
                                const D_UINT64 count)
{
  D_UINT64 resultEntry = 0;
  {
    WSynchronizerRAII synchHolder(m_Sync);

    resultEntry = AllocateEntry (0);
    StoredItem cachedItem = m_EntrysCache.RetriveItem (resultEntry);
    StoreEntry *pEntryHdr = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

    pEntryHdr->MarkAsUsed();
    pEntryHdr->MarkFirstAsIndex();

    pEntryHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    pEntryHdr->SetPrevEntry (rowIndex);
  }

  if ((resultEntry != 0) && (count > 0))
    {
      assert (pBuffer != NULL);
      UpdateRecord (resultEntry, 0, count, pBuffer);
    }

  return resultEntry;
}

D_UINT64
VaribaleLenghtStore::AddRecord (
                                const D_UINT64 rowIndex,
                                VaribaleLenghtStore &sourceStore,
                                D_UINT64 sourceFirstEntry,
                                D_UINT64 sourceOffset,
                                D_UINT64 sourceCount)

{
  D_UINT64 resultEntry = 0;
  {
    WSynchronizerRAII synchHolder(m_Sync);

    resultEntry = AllocateEntry (0);
    StoredItem cachedItem = m_EntrysCache.RetriveItem (resultEntry);
    StoreEntry *pEntryHdr = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

    pEntryHdr->MarkAsUsed();
    pEntryHdr->MarkFirstAsIndex();

    pEntryHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    pEntryHdr->SetPrevEntry (rowIndex);
  }

  if ((resultEntry != 0) && (sourceCount > 0))
    UpdateRecord (resultEntry, 0, sourceStore, sourceFirstEntry, sourceOffset, sourceCount);

  return resultEntry;
}


D_UINT64
VaribaleLenghtStore::AddRecord (
                                const D_UINT64 rowIndex,
                                I_DataContainer &sourceContainer,
                                D_UINT64 sourceOffset,
                                D_UINT64 sourceCount)

{
  D_UINT64 resultEntry = 0;
  {
    WSynchronizerRAII synchHolder(m_Sync);

    resultEntry = AllocateEntry (0);
    StoredItem cachedItem = m_EntrysCache.RetriveItem (resultEntry);
    StoreEntry *pEntryHdr = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

    pEntryHdr->MarkAsUsed();
    pEntryHdr->MarkFirstAsIndex();

    pEntryHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    pEntryHdr->SetPrevEntry (rowIndex);
  }

  if ((resultEntry != 0) && (sourceCount > 0))
    UpdateRecord (resultEntry, 0, sourceContainer, sourceOffset, sourceCount);

  return resultEntry;
}

void
VaribaleLenghtStore::GetRecord (
                                D_UINT64 recordFirstEntry,
                                D_UINT64 offset,
                                D_UINT64 count,
                                D_UINT8 *pBuffer)
{
  WSynchronizerRAII synchHolder(m_Sync);

  do
    {
      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->GetRawDataSize())
        break;

      offset -= cpEntry->GetRawDataSize();
      recordFirstEntry = cpEntry->GetNextEntry();
    }
  while (true);

  while (count > 0)
    {
      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
              throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      const D_UINT64 chunkSize = min (count, cpEntry->GetRawDataSize() - offset);

      cpEntry->ReadEntryData (offset, chunkSize, pBuffer);

      count -= chunkSize, pBuffer += chunkSize;
      offset = (offset + chunkSize) % cpEntry->GetRawDataSize();

      recordFirstEntry = cpEntry->GetNextEntry();
    }
}

void
VaribaleLenghtStore::UpdateRecord (
                                   D_UINT64 recordFirstEntry,
                                   D_UINT64 offset,
                                   D_UINT64 count,
                                   const D_UINT8 *pBuffer)
{
  WSynchronizerRAII synchHolder(m_Sync);

  D_UINT64 prevEntry = recordFirstEntry;

  do
    {
      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
          else
            break;
        }

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->GetRawDataSize())
        break;

      offset -= cpEntry->GetRawDataSize();
      prevEntry = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();

    }
  while (true);

  while (count > 0)
    {
      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
              recordFirstEntry = AllocateEntry (prevEntry);

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      StoreEntry *cpEntry = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

      assert (cpEntry->IsDeleted() == false);

      const D_UINT64 chunkSize = cpEntry->UpdateEntryData (offset, count, pBuffer);
      assert (chunkSize > 0);


      count -= chunkSize, pBuffer += chunkSize;
      offset = (offset + chunkSize) % cpEntry->GetRawDataSize();

      prevEntry = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();
    }

}

void
VaribaleLenghtStore::UpdateRecord (
                                   D_UINT64 recordFirstEntry,
                                   D_UINT64 offset,
                                   VaribaleLenghtStore &sourceStore,
                                   D_UINT64 sourceFirstEntry,
                                   D_UINT64 sourceOffset,
                                   D_UINT64 sourceCount)
{
  D_UINT64 prevEntry = recordFirstEntry;
  D_UINT64 sourcePrevEntry = sourceFirstEntry;

  do
    {
      WSynchronizerRAII synchHolder (m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
          else
            break;
        }

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->GetRawDataSize())
        break;

      offset -= cpEntry->GetRawDataSize();
      prevEntry = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();

    }
  while (true);

  do
    {
      WSynchronizerRAII sourceSyncHolder (sourceStore.m_Sync);

      if (sourceFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (sourceOffset != 0)
            throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
          else
            break;
        }

      StoredItem cachedItem = sourceStore.m_EntrysCache.RetriveItem (recordFirstEntry);
      const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (sourceOffset < cpEntry->GetRawDataSize())
        break;

      sourceOffset -= cpEntry->GetRawDataSize();
      sourceFirstEntry = cpEntry->GetNextEntry();
    }
  while (true);

  while (sourceCount > 0)
    {
      if (sourceFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

      WSynchronizerRAII synchHolder (m_Sync);

      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
              recordFirstEntry = AllocateEntry (prevEntry);

      synchHolder.Leave();

      D_UINT8 tempBuffer [64];
      D_UINT  tempValid = 0;
      {
        WSynchronizerRAII sourceSynchHolder (sourceStore.m_Sync);

        StoredItem cachedItem = sourceStore.m_EntrysCache.RetriveItem (sourceFirstEntry);
        StoreEntry *cpEntry = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

        assert (cpEntry->GetRawDataSize() <= sizeof tempBuffer);
        tempValid = cpEntry->ReadEntryData(sourceOffset, sourceCount, tempBuffer);
        sourcePrevEntry = sourceFirstEntry;
        sourceFirstEntry = cpEntry->GetNextEntry();
      }

      synchHolder.Enter();

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      StoreEntry *cpEntry = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

      assert (cpEntry->IsDeleted() == false);

      const D_UINT64 chunkSize = cpEntry->UpdateEntryData (offset, tempValid, tempBuffer);
      assert (chunkSize > 0);

      sourceCount -= chunkSize;

      offset += chunkSize;
      if (offset >= cpEntry->GetRawDataSize())
        {
          prevEntry = recordFirstEntry;
          recordFirstEntry = cpEntry->GetNextEntry();

          offset -= cpEntry->GetRawDataSize();
        }

      sourceOffset += chunkSize;
      if (sourceOffset < cpEntry->GetRawDataSize())
        {
          /* Revert the sourceFirstEntry! */
          sourceFirstEntry = sourcePrevEntry;
        }
      else
        sourceOffset -= cpEntry->GetRawDataSize(); //Bogus! Is not its 'cpEntry'!

    }
}

void
VaribaleLenghtStore::UpdateRecord (
                                   D_UINT64 recordFirstEntry,
                                   D_UINT64 offset,
                                   I_DataContainer &sourceContainer,
                                   D_UINT64 sourceOffset,
                                   D_UINT64 sourceCount)
{
  WSynchronizerRAII synchHolder(m_Sync);

  D_UINT64 prevEntry = recordFirstEntry;

  do
    {
      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
        {
          if (offset != 0)
            throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
          else
            break;
        }

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      if (offset < cpEntry->GetRawDataSize())
        break;

      offset -= cpEntry->GetRawDataSize();
      prevEntry = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();

    }
  while (true);

  while (sourceCount > 0)
    {
      if (recordFirstEntry == StoreEntry::LAST_CHAINED_ENTRY)
              recordFirstEntry = AllocateEntry (prevEntry);

      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      StoreEntry *cpEntry = _RC (StoreEntry *, cachedItem.GetDataForUpdate());

      assert (cpEntry->IsDeleted() == false);

      D_UINT8 tempBuffer[64];
      D_UINT  tempValid = MIN (cpEntry->GetRawDataSize() - offset, sourceCount);

      assert (cpEntry->GetRawDataSize() < sizeof tempBuffer);

      sourceContainer.RetrieveData (sourceOffset, tempValid, tempBuffer);

      const D_UINT64 chunkSize = cpEntry->UpdateEntryData (offset, tempValid, tempBuffer);
      assert (chunkSize == tempValid);

      sourceCount -= tempValid, sourceOffset += tempValid;
      offset = (offset + tempValid) % cpEntry->GetRawDataSize();

      prevEntry = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();
    }
}

void
VaribaleLenghtStore::RemoveRecord (D_UINT64 recordFirstEntry)
{
  WSynchronizerRAII synchHolder(m_Sync);

  {
    StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
    const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

    if (cpEntry->IsDeleted() || (cpEntry->IsIndexFirst() == false))
      throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
  }

  while (recordFirstEntry != StoreEntry::LAST_CHAINED_ENTRY)
    {
      StoredItem cachedItem = m_EntrysCache.RetriveItem (recordFirstEntry);
      const StoreEntry *cpEntry = _RC (const StoreEntry *, cachedItem.GetDataForRead());

      assert (cpEntry->IsDeleted() == false);

      const D_UINT64 currentEntry = recordFirstEntry;
      recordFirstEntry = cpEntry->GetNextEntry();

      AddToFreeList (currentEntry);
    }
}

D_UINT64
VaribaleLenghtStore::GetRawSize() const
{
  if (m_apEntriesContainer.get() == NULL)
    return 0;

  return m_apEntriesContainer->GetContainerSize();
}

void
VaribaleLenghtStore::StoreItems (const D_UINT8 *pSrcBuffer, D_UINT64 firstItem, D_UINT itemsCount)
{
  if (firstItem + itemsCount > m_EntrysCount)
    itemsCount = m_EntrysCount - firstItem;

  const D_UINT64 start = firstItem * sizeof (StoreEntry);
  const D_UINT64 count = itemsCount * sizeof (StoreEntry);

  m_apEntriesContainer->StoreData (start, count, pSrcBuffer);
}

void
VaribaleLenghtStore::RetrieveItems (D_UINT8 *pDestBuffer, D_UINT64 firstItem, D_UINT itemsCount)
{

  if (firstItem + itemsCount > m_EntrysCount)
    itemsCount = m_EntrysCount - firstItem;

  const D_UINT64 start = firstItem * sizeof (StoreEntry);
  const D_UINT64 count = itemsCount * sizeof (StoreEntry);

  m_apEntriesContainer->RetrieveData (start, count, pDestBuffer);
}

D_UINT64
VaribaleLenghtStore::AllocateEntry (const D_UINT64 prevEntry)
{
  D_UINT64 foundFree = m_FirstFreeEntry;

  //Try to find a neighbor for of the previous entry;
  if ((prevEntry + 1) < m_EntrysCount)
    {
      StoredItem cachedItem = m_EntrysCache.RetriveItem (prevEntry + 1);
      const StoreEntry *pEntHdr = _RC( const StoreEntry *, cachedItem.GetDataForRead());
      if (pEntHdr->IsDeleted())
        foundFree = prevEntry + 1;
    }
  else if (prevEntry > 1)
    {
      StoredItem cachedItem = m_EntrysCache.RetriveItem (prevEntry - 1);
      const StoreEntry *pEntHdr = _RC( const StoreEntry *, cachedItem.GetDataForRead());
      if (pEntHdr->IsDeleted())
        foundFree = prevEntry - 1;
    }

  if (foundFree == StoreEntry::LAST_DELETED_ENTRY)
    foundFree = ExtentFreeList ();

  ExtractFromFreeList (foundFree);

  StoredItem cachedItem = m_EntrysCache.RetriveItem (foundFree);
  StoreEntry *pEntHdr = _RC( StoreEntry *, cachedItem.GetDataForUpdate());

  if (prevEntry > 0)
    {
      StoredItem prevCachedItem = m_EntrysCache.RetriveItem (prevEntry);
      StoreEntry *pPrevEntHdr = _RC( StoreEntry *, prevCachedItem.GetDataForUpdate());

      const D_UINT64 prevNext = pPrevEntHdr->GetNextEntry();

      assert (pPrevEntHdr->IsDeleted() == false);
      assert (pPrevEntHdr->GetNextEntry() == StoreEntry::LAST_CHAINED_ENTRY);

      pPrevEntHdr->SetNextEntry(foundFree);

      pEntHdr->SetPrevEntry (prevEntry);
      pEntHdr->MarkFirstAsPrev();
      pEntHdr->SetNextEntry (prevNext);

      if (pEntHdr->GetNextEntry() != StoreEntry::LAST_CHAINED_ENTRY)
        {
          StoredItem nextCachedItem = m_EntrysCache.RetriveItem (pEntHdr->GetNextEntry());
          pEntHdr = _RC( StoreEntry *, nextCachedItem.GetDataForUpdate());

          pEntHdr->SetPrevEntry (foundFree);
        }
    }
  else
    {
      pEntHdr->MarkFirstAsIndex ();
      pEntHdr->SetPrevEntry (0);
      pEntHdr->SetNextEntry (StoreEntry::LAST_CHAINED_ENTRY);
    }

  return foundFree;
}

D_UINT64
VaribaleLenghtStore::ExtentFreeList ()
{
  assert (m_FirstFreeEntry == StoreEntry::LAST_DELETED_ENTRY);
  StoreEntry addEntry;

  addEntry.MarkAsDeleted();
  addEntry.SetPrevEntry (0);
  addEntry.SetNextEntry (StoreEntry::LAST_DELETED_ENTRY);

  addEntry.MarkFirstAsPrev ();

  D_UINT64 insertPos = m_apEntriesContainer.get ()->GetContainerSize();
  m_FirstFreeEntry = insertPos / sizeof (addEntry);


  assert ((insertPos % sizeof (addEntry)) == 0);
  m_apEntriesContainer.get ()->StoreData (insertPos, sizeof (addEntry), _RC(D_UINT8*, &addEntry));

  ++m_EntrysCount;

  //Reload the content of item's block.
  m_EntrysCache.ForceItemUpdate (m_FirstFreeEntry);

  StoredItem cachedItem = m_EntrysCache.RetriveItem (0);
  StoreEntry *pEntHdr = _RC( StoreEntry *, cachedItem.GetDataForUpdate());
  pEntHdr->SetNextEntry (m_FirstFreeEntry);

  return m_FirstFreeEntry;
}

void
VaribaleLenghtStore::ExtractFromFreeList (const D_UINT64 freeEntry)
{
  StoredItem cachedItem = m_EntrysCache.RetriveItem (freeEntry);
  StoreEntry *pEntHdr = _RC( StoreEntry *, cachedItem.GetDataForUpdate());

  assert (freeEntry != 0);
  assert (pEntHdr->IsDeleted());
  assert (pEntHdr->IsIndexFirst() == false);

  const D_UINT64 prevEntry = pEntHdr->GetPrevEntry();
  const D_UINT64 nextEntry = pEntHdr->GetNextEntry();

  assert (prevEntry < m_EntrysCount);
  assert ((nextEntry == StoreEntry::LAST_DELETED_ENTRY) || (nextEntry < m_EntrysCount));

  pEntHdr->MarkAsUsed();
  pEntHdr->SetPrevEntry (0);
  pEntHdr->SetNextEntry (0);

  cachedItem = m_EntrysCache.RetriveItem (prevEntry);
  pEntHdr = _RC( StoreEntry *, cachedItem.GetDataForUpdate());
  assert (pEntHdr->IsDeleted());
  assert (pEntHdr->IsIndexFirst() == false);

  pEntHdr->SetNextEntry (nextEntry);

  if (freeEntry == m_FirstFreeEntry)
    m_FirstFreeEntry = nextEntry;

  if (nextEntry != StoreEntry::LAST_DELETED_ENTRY)
    {
      cachedItem = m_EntrysCache.RetriveItem (nextEntry);
      pEntHdr = _RC( StoreEntry *,cachedItem.GetDataForUpdate());

      assert (pEntHdr->IsDeleted());
      assert (pEntHdr->IsIndexFirst() == false);

      pEntHdr->SetPrevEntry(prevEntry);
    }
}

void
VaribaleLenghtStore::AddToFreeList (const D_UINT64 entry)
{
  assert (m_EntrysCount > entry);

  StoredItem cachedItem = m_EntrysCache.RetriveItem (entry);
  StoreEntry *pEntHdr = _RC( StoreEntry *, cachedItem.GetDataForUpdate());

  StoredItem neighborCachedItem = cachedItem; //Just to have a valid initialization
  StoreEntry *pNeighborEntHdr = NULL;

  assert (entry > 0);
  assert (pEntHdr->IsDeleted() == false);

  pEntHdr->MarkAsDeleted();
  pEntHdr->MarkFirstAsPrev ();

  //Maybe we are lucky! Check to see if we can link to one of our neighbors!
  if (m_EntrysCount > (entry + 1))
    {
      neighborCachedItem = m_EntrysCache.RetriveItem (entry + 1);
      pNeighborEntHdr = _RC( StoreEntry *, neighborCachedItem.GetDataForUpdate());
    }

  if ((pNeighborEntHdr != NULL) && pNeighborEntHdr->IsDeleted())
    {
      assert (pNeighborEntHdr->IsIndexFirst() == false);

      const D_UINT64 prevEntry = pNeighborEntHdr->GetPrevEntry();
      pNeighborEntHdr->SetPrevEntry (entry);
      pEntHdr->SetPrevEntry (prevEntry);
      pEntHdr->SetNextEntry (entry + 1);

      neighborCachedItem = m_EntrysCache.RetriveItem (prevEntry);
      pNeighborEntHdr = _RC( StoreEntry *, neighborCachedItem.GetDataForUpdate());

      assert (pNeighborEntHdr->IsDeleted());
      assert (pNeighborEntHdr->IsIndexFirst() == false);

      pNeighborEntHdr->SetNextEntry (entry);

      if (m_FirstFreeEntry == (entry + 1))
        {
          assert (prevEntry == 0);
          m_FirstFreeEntry = entry;
        }

      return ;
    }

  if (entry > 1)
    {
      neighborCachedItem = m_EntrysCache.RetriveItem (entry - 1);
      pNeighborEntHdr = _RC( StoreEntry *, neighborCachedItem.GetDataForUpdate());

      if (pNeighborEntHdr->IsDeleted())
        {
          assert (pNeighborEntHdr->IsIndexFirst() == false);

          const D_UINT64 nextEntry = pNeighborEntHdr->GetNextEntry();
          pNeighborEntHdr->SetNextEntry (entry);
          pEntHdr->SetNextEntry (nextEntry);
          pEntHdr->SetPrevEntry (entry - 1);

          if (nextEntry == StoreEntry::LAST_CHAINED_ENTRY)
            return ;

          neighborCachedItem = m_EntrysCache.RetriveItem (nextEntry);
          pNeighborEntHdr = _RC( StoreEntry *, neighborCachedItem.GetDataForUpdate());

          assert (pNeighborEntHdr->IsDeleted());
          assert (pNeighborEntHdr->IsIndexFirst() == false);

          pNeighborEntHdr->SetPrevEntry (entry);

          return ;
        }
    }

  //Nothing found close by! Let's add it at the begin.
  pEntHdr->SetPrevEntry (0);
  pEntHdr->SetNextEntry( m_FirstFreeEntry);

  m_FirstFreeEntry = entry;
  cachedItem = m_EntrysCache.RetriveItem (0);
  pEntHdr = _RC( StoreEntry *, cachedItem.GetDataForUpdate());

  assert (pEntHdr->IsDeleted());
  assert (pEntHdr->IsIndexFirst() == false);

  pEntHdr->SetNextEntry (entry);

}

