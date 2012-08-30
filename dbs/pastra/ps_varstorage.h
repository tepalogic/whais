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

#include "utils/include/wthread.h"

#include "ps_container.h"
#include "ps_blockcache.h"

namespace pastra
{

class StoreEntry
{
public:
  static const D_UINT64 LAST_DELETED_ENTRY = 0x0FFFFFFFFFFFFFFF;
  static const D_UINT64 LAST_CHAINED_ENTRY = 0x0FFFFFFFFFFFFFFF;
  static const D_UINT64 ENTRY_DELETED_MASK = 0x8000000000000000;
  static const D_UINT64 FIRST_RECORD_ENTRY = 0x4000000000000000;

  static const D_UINT   ENTRY_SIZE = 48;

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

  D_UINT64 GetPrevEntry () const { return m_PrevEntry; }
  void     SetPrevEntry (D_UINT64 content) { m_PrevEntry = content; }
  D_UINT64 GetNextEntry () const
  {
    return m_NextEntry & ~(ENTRY_DELETED_MASK | FIRST_RECORD_ENTRY);
  }
  void     SetNextEntry (D_UINT64 content)
  {
    m_NextEntry &= (ENTRY_DELETED_MASK | FIRST_RECORD_ENTRY);
    m_NextEntry |= content;
  }

  D_UINT  Read (D_UINT offset, D_UINT count, D_UINT8 *pBuffer) const ;
  D_UINT  Write (D_UINT offset, D_UINT count, const D_UINT8 *pBuffer);

  static D_UINT8  Size () { return ENTRY_SIZE; }

private:
  D_UINT64 m_PrevEntry;
  D_UINT64 m_NextEntry;
  D_UINT8  m_aRawData[ENTRY_SIZE];
};

class VLVarsStore : public I_BlocksManager
{
public:
  VLVarsStore ();

  void RegisterReference ();
  void ReleaseReference ();

  void Init (const D_CHAR*  tempDir,
             const D_UINT32 reservedMem);
  void Init (const D_CHAR*  pContainerBaseName,
             const D_UINT64 uContainerSize,
             const D_UINT64 uMaxFileSize);
  void Flush ();
  void MarkForRemoval ();

  D_UINT64 AddRecord (const D_UINT8* pBuffer,
                      const D_UINT64 size);

  D_UINT64 AddRecord (VLVarsStore& sourceStore,
                      D_UINT64     sourceFirstEntry,
                      D_UINT64     sourceFrom,
                      D_UINT64     sourceSize);

  D_UINT64 AddRecord (I_DataContainer& sourceContainer,
                      D_UINT64         sourceFrom,
                      D_UINT64         sourceSize);

  void     GetRecord (D_UINT64 recordFirstEntry,
                      D_UINT64 offset,
                      D_UINT64 size,
                      D_UINT8* pBuffer);

  void     UpdateRecord (D_UINT64       recordFirstEntry,
                         D_UINT64       offset,
                         D_UINT64       size,
                         const D_UINT8* pBuffer);

  void     UpdateRecord (D_UINT64     recordFirstEntry,
                         D_UINT64     offset,
                         VLVarsStore& sourceStore,
                         D_UINT64     sourceFirstEntry,
                         D_UINT64     sourceOffset,
                         D_UINT64     sourceCount);

  void     UpdateRecord (D_UINT64         recordFirstEntry,
                         D_UINT64         offset,
                         I_DataContainer& sourceContainer,
                         D_UINT64         sourceOffset,
                         D_UINT64         sourceCount);

  void     IncrementRecordRef (const D_UINT64 recordFirstEntry);
  void     DecrementRecordRef (const D_UINT64 recordFirstEntry);

  D_UINT64 Size () const;

  //Implementations for I_BlocksManager
  virtual void StoreItems (const D_UINT8 *pSrcBuffer,
                           D_UINT64 firstItem,
                           D_UINT itemsCount);

  virtual void RetrieveItems (D_UINT8 *pDestBuffer,
                              D_UINT64 firstItem,
                              D_UINT itemsCount);

private:
  void       FinishInit ();
  D_UINT64   AllocateEntry (const D_UINT64 prevEntry);
  D_UINT64   ExtentFreeList ();
  void       RemoveRecord (D_UINT64 recordFirstEntry);

  void       ExtractFromFreeList (const D_UINT64 entry);
  void       AddToFreeList (const D_UINT64 entry);

  std::auto_ptr<I_DataContainer> m_apEntriesContainer;
  BlockCache                     m_EntrysCache;
  D_UINT64                       m_FirstFreeEntry;
  D_UINT64                       m_EntrysCount;
  D_UINT64                       m_RefsCount;
  WSynchronizer                  m_Sync;
};

}

#endif /* PS_VARTSORAGE_H_ */
