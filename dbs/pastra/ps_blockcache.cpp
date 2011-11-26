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

BlockCache::BlockCache (I_BlocksManager &rBlockManagr) :
    mManager (rBlockManagr),
    mItemSize (0),
    mMaxBlocks (0),
    mBlockSize (0),
    mCachedBlocks (),
    mNextToClear (0)
{
}

BlockCache::~BlockCache ()
{

  if (mItemSize == 0)
    return ; //This has not been initialized. So nothing to do here!

  map<D_UINT64, BlockEntry>::iterator it = mCachedBlocks.begin ();
  const D_UINT itemsPerBlock = mBlockSize / mItemSize;

  while (it != mCachedBlocks.end ())
    {
      assert (it->second.IsInUse() == false);

      if (it->second.IsDirty ())
        mManager.StoreItems (it->second.m_BlockData, it->first, itemsPerBlock);


      delete it->second.m_BlockData;

      mCachedBlocks.erase (it++);
    }
}

void
BlockCache::Init (D_UINT itemSize, D_UINT blockSize, D_UINT maxBlockCount)
{

  if ((itemSize && maxBlockCount) == 0)
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_PARAMETERS));

  if ((mItemSize || mBlockSize || mMaxBlocks) != 0)
      throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

  mItemSize = itemSize;
  mMaxBlocks = maxBlockCount;
  mBlockSize = blockSize;

  if (mBlockSize < mItemSize)
    mBlockSize = mItemSize;
}

StoredItem
BlockCache::RetriveItem (const D_UINT64 item)
{
  const D_UINT itemsPerBlock = mBlockSize / mItemSize;
  const D_UINT64 baseBlockItem = (item / itemsPerBlock) * itemsPerBlock;

  map<D_UINT64, BlockEntry>::iterator it = mCachedBlocks.find (baseBlockItem);

  if (it != mCachedBlocks.end ())
    return StoredItem (it->second, (item % itemsPerBlock) * mItemSize);

  if (mCachedBlocks.size() < mMaxBlocks)
    {
      std::auto_ptr <D_UINT8> apBlockData (new D_UINT8[mBlockSize]);

      pair<D_UINT64, BlockEntry> block (baseBlockItem, BlockEntry (baseBlockItem, apBlockData.get ()));
      mManager.RetrieveItems (block.second.m_BlockData, baseBlockItem, itemsPerBlock);

      pair< map<D_UINT64, BlockEntry>::iterator, bool> t_it = mCachedBlocks.insert (block);

      assert (t_it.second);
      apBlockData.release();

      return StoredItem (t_it.first->second, (item % itemsPerBlock) * mItemSize);
    }
  else
    {
      it = mCachedBlocks.begin ();
      while (it != mCachedBlocks.end ())
        {
          if (it->second.IsInUse () == false)
            break;
          else
            ++ it;
        }

      if (it != mCachedBlocks.end ())
        {

          std::auto_ptr <D_UINT8> apBlockData (NULL);
          D_UINT8 *pBlockData = it->second.m_BlockData;

          if (it->second.IsDirty())
            mManager.StoreItems (it->second.m_BlockData, it->second.m_BaseItemIndex, itemsPerBlock);
          mCachedBlocks.erase (it);

          apBlockData.reset (pBlockData);

          pair<D_UINT64, BlockEntry> block (baseBlockItem, BlockEntry (baseBlockItem, pBlockData));
          mManager.RetrieveItems (block.second.m_BlockData, baseBlockItem, itemsPerBlock);
          pair< map<D_UINT64, BlockEntry>::iterator, bool> t_it = mCachedBlocks.insert (block);

          assert (t_it.second);

          apBlockData.release();

          return StoredItem (t_it.first->second, (item % itemsPerBlock) * mItemSize);
        }
    }

  //If we are here we have to allocate something anyway
  std::auto_ptr <D_UINT8> apBlockData (new D_UINT8[mBlockSize]);

  pair<D_UINT64, BlockEntry> block (baseBlockItem, BlockEntry (baseBlockItem, apBlockData.get ()));
  mManager.RetrieveItems (block.second.m_BlockData, baseBlockItem, itemsPerBlock);

  pair< map<D_UINT64, BlockEntry>::iterator, bool> t_it = mCachedBlocks.insert (block);

  assert (t_it.second);
  apBlockData.release();

  return StoredItem (t_it.first->second, (item % itemsPerBlock) * mItemSize);
}

void
BlockCache::ForceItemUpdate (const D_UINT64 item)
{
  const D_UINT itemsPerBlock = mBlockSize / mItemSize;
  const D_UINT64 baseBlockItem = (item / itemsPerBlock) * itemsPerBlock;

  map<D_UINT64, BlockEntry>::iterator it = mCachedBlocks.find (baseBlockItem);

  if (it == mCachedBlocks.end ())
    return;

  D_UINT8* const itemData = it->second.m_BlockData + ((item % itemsPerBlock) * mItemSize);

  mManager.RetrieveItems (itemData, item, 1);
}

