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

#include "dbs_exception.h"

#include "ps_blockcache.h"

using namespace pastra;
using namespace std;

BlockCache::BlockCache (I_BlocksManager& rBlockManagr) :
    m_Manager (rBlockManagr),
    m_ItemSize (0),
    m_MaxBlocks (0),
    m_BlockSize (0),
    m_CachedBlocks ()
{
}

BlockCache::~BlockCache ()
{
  if (m_ItemSize == 0)
    return ; //This has not been initialized. So nothing to do here!

  map<D_UINT64, BlockEntry>::iterator it = m_CachedBlocks.begin ();
  while (it != m_CachedBlocks.end ())
    {
      assert (it->second.IsInUse() == false);
      assert (it->second.IsDirty () == false);

      delete it->second.m_BlockData;

      m_CachedBlocks.erase (it++);
    }
}

void
BlockCache::Init (D_UINT itemSize, D_UINT blockSize, D_UINT maxBlockCount)
{
  if ((itemSize && maxBlockCount) == 0)
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  if ((m_ItemSize || m_BlockSize || m_MaxBlocks) != 0)
      throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

  m_ItemSize  = itemSize;
  m_MaxBlocks = maxBlockCount;
  m_BlockSize = blockSize;

  if (m_BlockSize < m_ItemSize)
    m_BlockSize = m_ItemSize;
}

void
BlockCache::Flush ()
{
  assert (m_ItemSize != 0);

  const D_UINT itemsPerBlock = m_BlockSize / m_ItemSize;

  map<D_UINT64, BlockEntry>::iterator it = m_CachedBlocks.begin ();
  while (it != m_CachedBlocks.end ())
    {
      assert (it->second.IsInUse() == false);

      if (it->second.IsDirty ())
        m_Manager.StoreItems (it->second.m_BlockData, it->first, itemsPerBlock);

      delete it->second.m_BlockData;

      m_CachedBlocks.erase (it++);
    }
}

StoredItem
BlockCache::RetriveItem (const D_UINT64 item)
{
  const D_UINT itemsPerBlock = m_BlockSize / m_ItemSize;
  const D_UINT64 baseBlockItem = (item / itemsPerBlock) * itemsPerBlock;

  map<D_UINT64, BlockEntry>::iterator it = m_CachedBlocks.find (baseBlockItem);

  if (it != m_CachedBlocks.end ())
    return StoredItem (it->second, (item % itemsPerBlock) * m_ItemSize);

  if (m_CachedBlocks.size() < m_MaxBlocks)
    {
      std::auto_ptr <D_UINT8> apBlockData (new D_UINT8[m_BlockSize]);

      pair<D_UINT64, BlockEntry> block (baseBlockItem,
                                        BlockEntry (baseBlockItem, apBlockData.get ()));
      m_Manager.RetrieveItems (block.second.m_BlockData, baseBlockItem, itemsPerBlock);

      pair< map<D_UINT64, BlockEntry>::iterator, bool> t_it = m_CachedBlocks.insert (block);

      assert (t_it.second);
      apBlockData.release();

      return StoredItem (t_it.first->second, (item % itemsPerBlock) * m_ItemSize);
    }
  else
    {
      it = m_CachedBlocks.begin ();
      while (it != m_CachedBlocks.end ())
        {
          if (it->second.IsInUse () == false)
            break;
          else
            ++ it;
        }

      if (it != m_CachedBlocks.end ())
        {
          std::auto_ptr <D_UINT8> apBlockData (NULL);
          D_UINT8*                pBlockData = it->second.m_BlockData;

          if (it->second.IsDirty())
            m_Manager.StoreItems (it->second.m_BlockData, it->second.m_BaseItemIndex, itemsPerBlock);
          m_CachedBlocks.erase (it);

          apBlockData.reset (pBlockData);

          pair<D_UINT64, BlockEntry> block (baseBlockItem,
                                            BlockEntry (baseBlockItem, pBlockData));
          m_Manager.RetrieveItems (block.second.m_BlockData, baseBlockItem, itemsPerBlock);

          pair< map<D_UINT64, BlockEntry>::iterator, bool> t_it = m_CachedBlocks.insert (block);

          assert (t_it.second);

          apBlockData.release();

          return StoredItem (t_it.first->second, (item % itemsPerBlock) * m_ItemSize);
        }
    }

  //If we are here we have to allocate something anyway
  std::auto_ptr <D_UINT8> apBlockData (new D_UINT8[m_BlockSize]);

  pair<D_UINT64, BlockEntry> block (baseBlockItem,
                                    BlockEntry (baseBlockItem, apBlockData.get ()));
  m_Manager.RetrieveItems (block.second.m_BlockData, baseBlockItem, itemsPerBlock);

  pair< map<D_UINT64, BlockEntry>::iterator, bool> t_it = m_CachedBlocks.insert (block);

  assert (t_it.second);
  apBlockData.release();

  return StoredItem (t_it.first->second, (item % itemsPerBlock) * m_ItemSize);
}

void
BlockCache::RefreshItem (const D_UINT64 item)
{
  const D_UINT itemsPerBlock   = m_BlockSize / m_ItemSize;
  const D_UINT64 baseBlockItem = (item / itemsPerBlock) * itemsPerBlock;

  map<D_UINT64, BlockEntry>::iterator it = m_CachedBlocks.find (baseBlockItem);

  if (it == m_CachedBlocks.end ())
    return;

  D_UINT8* const itemData = it->second.m_BlockData + ((item % itemsPerBlock) * m_ItemSize);

  m_Manager.RetrieveItems (itemData, item, 1);
}

