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

#ifndef PS_BLOCKCACHE_H_
#define PS_BLOCKCACHE_H_

#include <map>
#include <assert.h>
#include <string.h>

#include "whisper.h"

namespace pastra
{

class I_BlocksManager
{
public:
  I_BlocksManager () {}
  virtual ~I_BlocksManager () {}

  virtual void StoreItems (const uint8_t* pSrcBuffer,
                           uint64_t       firstItem,
                           uint_t         itemsCount) = 0;
  virtual void RetrieveItems (uint8_t* pDestBuffer,
                              uint64_t firstItem,
                              uint_t   itemsCount) = 0;
};


class BlockEntry
{
public:
  explicit BlockEntry (uint8_t* const pData) :
      m_pData (pData),
      m_ReferenceCount (0),
      m_Flags (0)
  {
    assert (pData != NULL);
  }

  bool  IsDirty () const { return (m_Flags & BLOCK_ENTRY_DIRTY) != 0;}
  bool  IsInUse () const { return m_ReferenceCount > 0;  }

  void  MarkDirty () { m_Flags |= BLOCK_ENTRY_DIRTY; }
  void  MarkClean () { m_Flags &= ~BLOCK_ENTRY_DIRTY; }

  void  RegisterUser () { m_ReferenceCount++; }
  void  ReleaseUser () {  assert ( m_ReferenceCount > 0); m_ReferenceCount--; }

  uint8_t* Data () { return m_pData; }

private:
  static const uint32_t BLOCK_ENTRY_DIRTY = 0x00000001;

  uint8_t* const m_pData;
  uint32_t       m_ReferenceCount;
  uint32_t       m_Flags;
};

class StoredItem
{
public:
  StoredItem (BlockEntry& blockEntry, uint_t itemOffset) :
    m_BlockEntry (&blockEntry),
    m_ItemOffset (itemOffset)
  {
    m_BlockEntry->RegisterUser ();
  }

  StoredItem (const StoredItem& rSource) :
    m_BlockEntry (rSource.m_BlockEntry),
    m_ItemOffset (rSource.m_ItemOffset)
  {
    m_BlockEntry->RegisterUser ();
  }

  ~StoredItem ()
  {
    m_BlockEntry->ReleaseUser ();
  }

  StoredItem& operator= (const StoredItem& rSource)
  {
    if (this == &rSource)
      return *this;

    rSource.m_BlockEntry->RegisterUser ();
    m_BlockEntry->ReleaseUser ();

    m_BlockEntry = rSource.m_BlockEntry;
    m_ItemOffset = rSource.m_ItemOffset;

    return *this;
  }

  uint8_t* GetDataForUpdate () const
  {
    m_BlockEntry->MarkDirty ();
    return m_BlockEntry->Data () + m_ItemOffset;
  }

  const uint8_t* GetDataForRead () const
  {
    return m_BlockEntry->Data () + m_ItemOffset;
  }

protected:
  BlockEntry* m_BlockEntry;
  uint_t      m_ItemOffset;
};

class BlockCache
{
public:
  BlockCache ();
  ~BlockCache ();

  void       Init (I_BlocksManager& blocksMgr,
                   const uint_t     itemSize,
                   const uint_t     blockSize,
                   const uint_t     maxBlockCount);
  void       Flush ();
  void       FlushItem (const uint64_t item);
  void       RefreshItem (const uint64_t item);
  StoredItem RetriveItem (const uint64_t item);

private:
  I_BlocksManager* m_pManager;
  uint_t           m_ItemSize;
  uint_t           m_MaxBlocks;
  uint_t           m_BlockSize;

  std::map <uint64_t, class BlockEntry> m_CachedBlocks;
};

}

#endif /* PS_BLOCKCACHE_H_ */
