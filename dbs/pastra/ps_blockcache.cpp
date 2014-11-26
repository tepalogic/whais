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

using namespace std;

namespace whais {
namespace pastra {

IBlocksManager::~IBlocksManager()
{
}



BlockCache::BlockCache()
  : mManager( NULL),
    mItemSize( 0),
    mBlockSize( 0),
    mMaxCachedBlocks( 0),
    mCachedBlocks()
{
}


BlockCache::~BlockCache()
{
  if (mItemSize == 0)
    return ; //This has not been initialized. So nothing to do here!

  map<uint64_t, BlockEntry>::iterator it = mCachedBlocks.begin();
  while( it != mCachedBlocks.end ())
    {
      assert( it->second.IsInUse() == false);
      assert( mSkipFlush || it->second.IsDirty() == false);

      delete [] it->second.Data();
      ++it;
    }
}


void
BlockCache::Init( IBlocksManager&  blocksMgr,
                  const uint_t     itemSize,
                  const uint_t     blockSize,
                  const uint_t     maxCachedBlocks,
                  const bool       nonPersitentData)
{
  assert( itemSize > 0);
  assert( blockSize > 0);
  assert( maxCachedBlocks > 0);

  mManager = &blocksMgr;

  if ((itemSize == 0) || (maxCachedBlocks == 0))
    throw DBSException( _EXTRA( DBSException::BAD_PARAMETERS));

  /* Expect a clean initialization. */
  if ((mItemSize | mBlockSize | mMaxCachedBlocks) != 0)
    throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));

  mItemSize         = itemSize;
  mMaxCachedBlocks  = maxCachedBlocks;
  mBlockSize        = blockSize;
  mSkipFlush        = nonPersitentData;

  if (mBlockSize < mItemSize)
    mBlockSize = mItemSize;

  else if (mBlockSize % mItemSize != 0)
    mBlockSize -= mBlockSize % mItemSize;

  assert( mBlockSize > 0);
}

void
BlockCache::Flush()
{
  assert( mItemSize != 0);

  if (mSkipFlush)
    return;

  map<uint64_t, BlockEntry>::iterator it = mCachedBlocks.begin();
  while( it != mCachedBlocks.end ())
    {
      FlushItem( it->first);
      ++it;
    }
}

StoredItem
BlockCache::RetriveItem( const uint64_t item)
{
  const uint_t   itemsPerBlock = mBlockSize / mItemSize;
  const uint64_t baseBlockItem = (item / itemsPerBlock) * itemsPerBlock;

  map<uint64_t, BlockEntry>::iterator it = mCachedBlocks.find( baseBlockItem);

  /* Check if the item is in cache. */
  if (it != mCachedBlocks.end ())
    return StoredItem( it->second, (item % itemsPerBlock) * mItemSize);

  /* Make sure you have room to cache the new item. */
  if (mCachedBlocks.size() >= mMaxCachedBlocks)
    {
      it = mCachedBlocks.begin();
      while( it != mCachedBlocks.end ())
        {
          if (it->second.IsInUse())
            continue ;

          uint8_t* const data_ = it->second.Data();

          if (it->second.IsDirty())
            mManager->StoreItems( it->first, itemsPerBlock, data_);

          delete [] data_;

          mCachedBlocks.erase( it++);
        }
    }

  auto_ptr<uint8_t> block( new uint8_t[mBlockSize]);
  uint8_t* const    data_ = block.get ();

  mCachedBlocks.insert( pair<uint64_t, BlockEntry> (baseBlockItem,
                                                    BlockEntry( data_)));
  block.release();

  mManager->RetrieveItems( baseBlockItem, itemsPerBlock, data_);

  return StoredItem( mCachedBlocks.find( baseBlockItem)->second,
                     (item % itemsPerBlock) * mItemSize);
}

void
BlockCache::FlushItem( const uint64_t item)
{
  const uint_t   itemsPerBlock = mBlockSize / mItemSize;
  const uint64_t baseBlockItem = (item / itemsPerBlock) * itemsPerBlock;

  map<uint64_t, BlockEntry>::iterator it = mCachedBlocks.find( baseBlockItem);
  if (it == mCachedBlocks.end ())
    return;

  if (it->second.IsDirty())
    {
      mManager->StoreItems( baseBlockItem, itemsPerBlock, it->second.Data());
      it->second.MarkClean();
    }
}

void
BlockCache::RefreshItem( const uint64_t item)
{
  const uint_t   itemsPerBlock = mBlockSize / mItemSize;
  const uint64_t baseBlockItem = (item / itemsPerBlock) * itemsPerBlock;

  map<uint64_t, BlockEntry>::iterator it = mCachedBlocks.find( baseBlockItem);

  if (it == mCachedBlocks.end ())
    return;

  assert( it->second.IsDirty() == false);

  mManager->RetrieveItems( baseBlockItem, itemsPerBlock, it->second.Data());
}

} //namespace pastra
} //namespace whais

