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

  virtual void StoreItems (const D_UINT8* pSrcBuffer,
                           D_UINT64       firstItem,
                           D_UINT         itemsCount) = 0;
  virtual void RetrieveItems (D_UINT8* pDestBuffer,
                              D_UINT64 firstItem,
                              D_UINT   itemsCount) = 0;
};


class BlockEntry
{
public:
  explicit BlockEntry (D_UINT8* const pData) :
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

  D_UINT8* Data () { return m_pData; }

private:
  static const D_UINT32 BLOCK_ENTRY_DIRTY = 0x00000001;

  D_UINT8* const m_pData;
  D_UINT32       m_ReferenceCount;
  D_UINT32       m_Flags;
};

class StoredItem
{
public:
  StoredItem (BlockEntry& blockEntry, D_UINT itemOffset) :
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

  D_UINT8* GetDataForUpdate () const
  {
    m_BlockEntry->MarkDirty ();
    return m_BlockEntry->Data () + m_ItemOffset;
  }

  const D_UINT8* GetDataForRead () const
  {
    return m_BlockEntry->Data () + m_ItemOffset;
  }

protected:
  BlockEntry* m_BlockEntry;
  D_UINT      m_ItemOffset;
};

class BlockCache
{
public:
  BlockCache ();
  ~BlockCache ();

  void       Init (I_BlocksManager& blocksMgr,
                   const D_UINT     itemSize,
                   const D_UINT     blockSize,
                   const D_UINT     maxBlockCount);
  void       Flush ();
  void       FlushItem (const D_UINT64 item);
  void       RefreshItem (const D_UINT64 item);
  StoredItem RetriveItem (const D_UINT64 item);

private:
  I_BlocksManager* m_pManager;
  D_UINT           m_ItemSize;
  D_UINT           m_MaxBlocks;
  D_UINT           m_BlockSize;

  std::map <D_UINT64, class BlockEntry> m_CachedBlocks;
};

}

#endif /* PS_BLOCKCACHE_H_ */
