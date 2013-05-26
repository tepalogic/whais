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

#include <assert.h>
#include <string.h>
#include <memory>

#include "dbs/dbs_mgr.h"

#include "ps_container.h"

using namespace std;

namespace whisper {
namespace pastra {


static inline void
safe_memcpy (uint8_t* to, uint8_t* from, uint64_t count)
{
  while (count-- > 0)
    *to++ = *from++;
}


void
append_int_to_str (uint64_t number, string& inoutStr)
{
  const uint_t bitsCount = sizeof (number) * 8;

  char   buffer[bitsCount];
  char*  conv = &buffer[bitsCount - 1];

  *conv = 0;
  do
    {
      conv--;

      *conv   = _SC (char, ('0' + (number % 10)));
      number /= 10;
    }
  while (number != 0);

  inoutStr += conv;
}



Exception*
WFileContainerException::Clone () const
{
  return new WFileContainerException (*this);
}


EXCEPTION_TYPE
WFileContainerException::Type () const
{
  return FILE_CONTAINER_EXCEPTION;
}


const char*
WFileContainerException::Description () const
{
  switch (Extra ())
    {
    case INVALID_PARAMETERS:
      return "Invalid parameters.";

    case CONTAINTER_INVALID:
      return "Container inconsistency detected.";

    case INVALID_ACCESS_POSITION:
      return "Container accessed outside bounds.";

    case FILE_OS_IO_ERROR:
      return "Container internal file IO error.";

    default:
      assert (false);

      return "Unknown container exception";
    }
  return NULL;
}



IDataContainer::~IDataContainer ()
{
}



FileContainer::FileContainer (const char*       baseName,
                              const uint64_t    maxFileSize,
                              const uint64_t    unitsCount)
  : mMaxFileUnitSize (maxFileSize),
    mFilesHandles (),
    mFileNamePrefix (baseName),
    mToRemove (false)
{
  uint_t openMode;

  openMode  = (unitsCount > 0) ? WHC_FILEOPEN_EXISTING : WHC_FILECREATE_NEW;
  openMode |= WHC_FILERDWR;

  for (uint_t unit = 0; unit < unitsCount; ++unit)
    {
      string baseName = mFileNamePrefix;

      if (unit != 0)
        append_int_to_str (unit, baseName);

      File container (baseName.c_str (), openMode);
      mFilesHandles.push_back (container);
    }

  assert (mFilesHandles.size () == unitsCount);

  // Check for structural consistency
  for (uint_t unit = 0; unit < unitsCount; ++unit)
    {
      File& file = mFilesHandles[unit];

      if ((file.GetSize () != maxFileSize)
          && ((unit != (unitsCount - 1)) || (file.GetSize () > maxFileSize)))
        {
          throw WFileContainerException (
                         "Inconsistent container!",
                         _EXTRA (WFileContainerException::CONTAINTER_INVALID)
                                        );
        }
    }
}


FileContainer::~FileContainer ()
{
  if (mToRemove)
    Colapse (0, Size () );
}


void
FileContainer::Write (uint64_t to, uint64_t size, const uint8_t* buffer)
{
  const uint_t unitsCount   = mFilesHandles.size ();
  uint64_t     unitIndex    = to / mMaxFileUnitSize;
  uint64_t     unitPosition = to % mMaxFileUnitSize;

  if (unitIndex > unitsCount)
    {
      throw WFileContainerException (
                   NULL,
                   _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION)
                                    );
    }
  else if (unitIndex == unitsCount)
    {
      if (unitPosition != 0)
        {
          throw WFileContainerException (
                   NULL,
                   _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION)
                                        );
        }
      else
        ExtendContainer ();
    }

  uint64_t actualSize = size;

  if ((actualSize + unitPosition) > mMaxFileUnitSize)
    actualSize = mMaxFileUnitSize - unitPosition;

  assert (actualSize <= size);

  File& file = mFilesHandles[unitIndex];

  if (file.GetSize () < unitPosition)
    {
      throw WFileContainerException (
                   NULL,
                   _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION)
                                    );
    }

  file.Seek (unitPosition, WHC_SEEK_BEGIN);
  file.Write (buffer, actualSize);

  //Write the rest
  if (actualSize < size)
    Write (to + actualSize, size - actualSize, buffer + actualSize);
}


void
FileContainer::Read (uint64_t from, uint64_t size, uint8_t* buffer)
{
  const uint_t unitsCount   = mFilesHandles.size ();
  uint64_t     unitIndex    = from / mMaxFileUnitSize;
  uint64_t     unitPosition = from % mMaxFileUnitSize;

  if ((unitIndex > unitsCount) || (from + size > Size ()))
    {
      throw WFileContainerException (
                     NULL,
                     _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION)
                                    );
    }

  File& file = mFilesHandles[unitIndex];

  uint64_t actualSize = size;

  if (actualSize + unitPosition > file.GetSize ())
    actualSize = file.GetSize () - unitPosition;

  file.Seek (unitPosition, WHC_SEEK_BEGIN);
  file.Read (buffer, actualSize);

  //Read the rest
  if (actualSize < size)
    Read (from + actualSize, size - actualSize, buffer + actualSize);

}


void
FileContainer::Colapse (uint64_t from, uint64_t to)
{
  const uint_t   bufferSize    = 4096;        //4KB
  const uint64_t intervalSize  = to - from;
  const uint64_t containerSize = Size ();

  if ((to < from) || (containerSize < to))
    {
      throw WFileContainerException (
                         NULL,
                         _EXTRA (WFileContainerException::INVALID_PARAMETERS)
                                    );
    }
  else if (intervalSize == 0)
    return;

  auto_ptr<uint8_t> buffer (new uint8_t[bufferSize]);

  while (to < containerSize)
    {
      uint64_t stepSize = bufferSize;

      if (stepSize + to > containerSize)
        stepSize = containerSize - to;

      Read (to, stepSize, buffer.get ());
      Write (from, stepSize, buffer.get ());

      to += stepSize, from += stepSize;
    }

  //Delete the remaining content.
  const uint64_t newSize      = containerSize - intervalSize;
  int            lastUnit     = newSize / mMaxFileUnitSize;
  const int      lastUnitSize = newSize % mMaxFileUnitSize;

  if (newSize == 0)
    --lastUnit;

  else
    mFilesHandles[lastUnit].SetSize (lastUnitSize);

  for (int unit = mFilesHandles.size () - 1; unit > lastUnit; --unit)
    {
      mFilesHandles[unit].Close ();

      string baseName = mFileNamePrefix;

      if (unit != 0)
        append_int_to_str (unit, baseName);

      if ( ! whf_remove (baseName.c_str ()))
        {
          throw WFileContainerException (
                           NULL,
                           _EXTRA (WFileContainerException::FILE_OS_IO_ERROR)
                                        );
        }
      mFilesHandles.pop_back ();
    }
}


uint64_t
FileContainer::Size () const
{
  if (mFilesHandles.size () == 0)
    return 0;

  const File&  lastUnitFile = mFilesHandles[mFilesHandles.size () - 1];
  uint64_t     result       = (mFilesHandles.size () - 1) * mMaxFileUnitSize;

  result += lastUnitFile.GetSize ();

  return result;
}


void
FileContainer::MarkForRemoval()
{
  mToRemove = true;
}


void
FileContainer::ExtendContainer ()
{
  uint_t count    = mFilesHandles.size ();
  string baseName = mFileNamePrefix;

  if (count != 0)
    append_int_to_str (count, baseName);

  File unitFile (baseName.c_str (), WHC_FILECREATE_NEW | WHC_FILERDWR);
  mFilesHandles.push_back (unitFile);
}



TemporalFileContainer::TemporalFileContainer (const char*    baseName,
                                              const uint32_t maxFileSize)
  : FileContainer (baseName, maxFileSize, 0)
{
  MarkForRemoval ();
}



TemporalContainer::TemporalContainer (const uint_t reservedMemory)
  : IDataContainer (),
    mFileContainer (NULL),
    mCache (new uint8_t[reservedMemory]),
    mCacheStartPos (0),
    mCacheEndPos (0),
    mCacheSize (reservedMemory),
    mDirtyCache (false)
{
}


void
TemporalContainer::Write (uint64_t to, uint64_t size, const uint8_t* buffer)
{
  if (to > Size ())
    {
      throw WFileContainerException (
                   NULL,
                   _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION)
                                    );
    }

  while (size > 0)
    {
      assert ((mCacheStartPos % mCacheSize) == 0);

      if ((to >= mCacheStartPos) && (to < (mCacheStartPos + mCacheSize)))
        {
          const uint_t toWrite = MIN (size, mCacheStartPos + mCacheSize - to);

          memcpy (mCache.get () + (to - mCacheStartPos), buffer, toWrite);

          if (to + toWrite > mCacheEndPos)
            mCacheEndPos = to + toWrite;

          mDirtyCache = true;
          to += toWrite, buffer += toWrite, size -= toWrite;
        }
      else
        FillCache (to);
    }
}

void
TemporalContainer::Read (uint64_t from, uint64_t size, uint8_t* buffer)
{
  if (from + size > Size ())
    {
      throw WFileContainerException (
                   NULL,
                   _EXTRA (WFileContainerException::INVALID_ACCESS_POSITION)
                                    );
    }

  while (size > 0)
    {
      assert ((mCacheStartPos % mCacheSize) == 0);

      if ((from >= mCacheStartPos) && (from < mCacheEndPos))
        {
          const uint_t toRead = MIN (size, mCacheEndPos - from);

          memcpy (buffer, mCache.get () + (from - mCacheStartPos), toRead);

          from += toRead, buffer += toRead, size -= toRead;
        }
      else
        FillCache (from);
    }
}


void
TemporalContainer::Colapse (uint64_t from, uint64_t to)
{
  if ((to < from) || (Size () < to))
    {
      throw WFileContainerException (
                         NULL,
                         _EXTRA (WFileContainerException::INVALID_PARAMETERS)
                                    );
    }

  if (mFileContainer.get () != NULL)
    {
      if (mDirtyCache)
        {
          mFileContainer->Write (mCacheStartPos,
                                 mCacheEndPos - mCacheStartPos,
                                 mCache.get ());
          mDirtyCache = false;
        }

      mFileContainer->Colapse (from, to);

      if (mFileContainer->Size () < from)
        FillCache (0);

      else
        FillCache (from);
    }
  else
    {
      uint8_t* const cache_      = mCache.get();
      const uint_t   colapseSize = mCacheEndPos - to;

      safe_memcpy (cache_ + from, cache_ + to, colapseSize);
      mCacheEndPos -= (to - from);
    }

  if ((mFileContainer.get () != NULL) && (mCacheSize > mFileContainer->Size()))
    mFileContainer.reset (NULL);
}

void
TemporalContainer::MarkForRemoval ()
{
  return ; //This will be deleted automatically. Nothing to do here!
}

uint64_t
TemporalContainer::Size () const
{
  assert ((mCacheStartPos % mCacheSize) == 0);
  assert (mCacheStartPos <= mCacheEndPos);

  if (mFileContainer.get () != NULL)
    {
      const uint64_t result = mFileContainer->Size ();

      assert (result >= mCacheStartPos);

      return MAX (result, mCacheEndPos);
    }

  assert (mCacheStartPos == 0);
  assert (mCacheEndPos <= mCacheSize);

  return mCacheEndPos;
}

void
TemporalContainer::FillCache (uint64_t position)
{
  position -= (position % mCacheSize);

  assert ((mCacheStartPos % mCacheSize) == 0);
  assert ((position % mCacheSize) == 0);

  if (mCacheStartPos == position)
    return;

  if (mFileContainer.get () == NULL)
    {
      smSync.Acquire ();
      const uint64_t currentId = smTemporalsCount++;
      smSync.Release ();

      assert (mCacheStartPos == 0);
      assert (mCacheEndPos == mCacheSize);
      assert (position == mCacheSize);

      const DBSSettings& settings = DBSGetSeettings ();

      string baseName (settings.mTempDir);
      baseName += "wtemp";
      append_int_to_str (currentId, baseName);
      baseName += ".tmp";

      mFileContainer.reset (new TemporalFileContainer (baseName.c_str (),
                                                       settings.mMaxFileSize));

      mFileContainer->Write (0, mCacheEndPos, mCache.get ());

      mCacheStartPos = mCacheEndPos = position;
      mDirtyCache    = false;

      return;
    }
  else
    {
      if (mDirtyCache)
        {
          mFileContainer->Write (mCacheStartPos,
                                  mCacheEndPos - mCacheStartPos,
                                  mCache.get ());
          mDirtyCache = false;
        }

      const uint_t toRead = MIN (mCacheSize,
                                 mFileContainer->Size() - position);

      mFileContainer->Read (position, toRead, mCache.get ());

      mCacheStartPos = position;
      mCacheEndPos   = mCacheStartPos + toRead;
    }
}


uint64_t  TemporalContainer::smTemporalsCount = 0;
Lock      TemporalContainer::smSync;

} //namespace pastra
} //namespace whisper

