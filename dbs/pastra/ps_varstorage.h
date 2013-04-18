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

#include "ps_container.h"
#include "ps_blockcache.h"

namespace whisper
{
namespace pastra
{

class StoreEntry
{
public:
  static const uint64_t LAST_DELETED_ENTRY = 0x0FFFFFFFFFFFFFFF;
  static const uint64_t LAST_CHAINED_ENTRY = 0x0FFFFFFFFFFFFFFF;
  static const uint64_t ENTRY_DELETED_MASK = 0x8000000000000000;
  static const uint64_t FIRST_RECORD_ENTRY = 0x4000000000000000;

  static const uint_t   ENTRY_SIZE = 48;

  StoreEntry ();
  ~StoreEntry () {} ;

  void MarkAsDeleted (const bool deleted)
  {
    deleted ? (m_NextEntry |= ENTRY_DELETED_MASK) :
              (m_NextEntry &= ~ENTRY_DELETED_MASK);
  }
  void MarkAsFirstEntry (const bool first)
  {
    first ? (m_NextEntry |= FIRST_RECORD_ENTRY) :
            (m_NextEntry &= ~FIRST_RECORD_ENTRY);
  }

  bool IsDeleted () const { return (m_NextEntry& ENTRY_DELETED_MASK) != 0; }
  bool IsFirstEntry () const { return (m_NextEntry& FIRST_RECORD_ENTRY) != 0; }

  uint64_t GetPrevEntry () const { return m_PrevEntry; }
  void     SetPrevEntry (uint64_t content) { m_PrevEntry = content; }
  uint64_t GetNextEntry () const
  {
    return m_NextEntry & ~(ENTRY_DELETED_MASK | FIRST_RECORD_ENTRY);
  }
  void     SetNextEntry (uint64_t content)
  {
    m_NextEntry &= (ENTRY_DELETED_MASK | FIRST_RECORD_ENTRY);
    m_NextEntry |= content;
  }

  uint_t  Read (uint_t offset, uint_t count, uint8_t *pBuffer) const ;
  uint_t  Write (uint_t offset, uint_t count, const uint8_t *pBuffer);

  static uint8_t  Size () { return ENTRY_SIZE; }

private:
  uint64_t m_PrevEntry;
  uint64_t m_NextEntry;
  uint8_t  m_aRawData[ENTRY_SIZE];
};

class VLVarsStore : public I_BlocksManager
{
public:
  VLVarsStore ();

  void RegisterReference ();
  void ReleaseReference ();

  void Init (const char*  tempDir,
             const uint32_t reservedMem);
  void Init (const char*  pContainerBaseName,
             const uint64_t uContainerSize,
             const uint64_t uMaxFileSize);
  void Flush ();
  void MarkForRemoval ();

  uint64_t AddRecord (const uint8_t* pBuffer,
                      const uint64_t size);

  uint64_t AddRecord (VLVarsStore& sourceStore,
                      uint64_t     sourceFirstEntry,
                      uint64_t     sourceFrom,
                      uint64_t     sourceSize);

  uint64_t AddRecord (I_DataContainer& sourceContainer,
                      uint64_t         sourceFrom,
                      uint64_t         sourceSize);

  void     GetRecord (uint64_t recordFirstEntry,
                      uint64_t offset,
                      uint64_t size,
                      uint8_t* pBuffer);

  void     UpdateRecord (uint64_t       recordFirstEntry,
                         uint64_t       offset,
                         uint64_t       size,
                         const uint8_t* pBuffer);

  void     UpdateRecord (uint64_t     recordFirstEntry,
                         uint64_t     offset,
                         VLVarsStore& sourceStore,
                         uint64_t     sourceFirstEntry,
                         uint64_t     sourceOffset,
                         uint64_t     sourceCount);

  void     UpdateRecord (uint64_t         recordFirstEntry,
                         uint64_t         offset,
                         I_DataContainer& sourceContainer,
                         uint64_t         sourceOffset,
                         uint64_t         sourceCount);

  void     IncrementRecordRef (const uint64_t recordFirstEntry);
  void     DecrementRecordRef (const uint64_t recordFirstEntry);

  uint64_t Size () const;

  //Implementations for I_BlocksManager
  virtual void StoreItems (const uint8_t *pSrcBuffer,
                           uint64_t firstItem,
                           uint_t itemsCount);

  virtual void RetrieveItems (uint8_t *pDestBuffer,
                              uint64_t firstItem,
                              uint_t itemsCount);

private:
  void       FinishInit ();
  uint64_t   AllocateEntry (const uint64_t prevEntry);
  uint64_t   ExtentFreeList ();
  void       RemoveRecord (uint64_t recordFirstEntry);

  void       ExtractFromFreeList (const uint64_t entry);
  void       AddToFreeList (const uint64_t entry);

  std::auto_ptr<I_DataContainer> m_apEntriesContainer;
  BlockCache                     m_EntrysCache;
  uint64_t                       m_FirstFreeEntry;
  uint64_t                       m_EntrysCount;
  uint64_t                       m_RefsCount;
  Lock                  m_Sync;
};

} //namespace pastra
} //namespace whisper

#endif /* PS_VARTSORAGE_H_ */
