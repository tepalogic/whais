/******************************************************************************
WHAIS - An advanced database system
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
******************************************************************************/

#include <assert.h>

#include "utils/wfile.h"
#include "whais.h"



namespace whais {


File::File (const char* name, uint_t mode)
  : mHandle (0),
    mFileSize (UNKNOWN_SIZE)
{
  mHandle = whf_open (name, mode);

  if (mHandle == FILE_LOCKED)
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Cannot be open file '%s'. It's already opened.",
                           name);
    }
  else if (mHandle == INVALID_FILE)
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Could not open file '%s'.",
                           name);
    }

  Size ();
}


File::File (const File &src) :
  mHandle (src.mHandle),
  mFileSize (src.mFileSize)
{
  _CC (File&, src).mHandle = INVALID_FILE;
}


File::~File ()
{
  /* Close it only if is not already closed */
  if ((mHandle != INVALID_FILE) && (mHandle != FILE_LOCKED))
    whf_close (mHandle);
}


void
File::Read (uint8_t* pBuffer, uint_t size)
{
  if ( ! whf_read (mHandle, pBuffer, size))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to read file (%d) content.",
                           mHandle);
    }
}


void
File::Write (const uint8_t* pBuffer, uint_t size)
{
  if (mFileSize != UNKNOWN_SIZE)
    {
      uint64_t currPos = Tell ();

      if (mFileSize < currPos + size)
        mFileSize = currPos + size;
    }

  if ( ! whf_write (mHandle, pBuffer, size))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to update file (%d).",
                           mHandle);
    }
}


void
File::Seek (const int64_t where, const int whence)
{
  if ( ! whf_seek (mHandle, where, whence))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to seek in file (%d).",
                           mHandle);
    }
}


uint64_t File::Tell ()
{
  uint64_t position;

  if ( ! whf_tell (mHandle, &position))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to to get file (%d) current position.",
                           mHandle);
    }

  return position;
}


void
File::Sync ()
{
  if ( ! whf_sync (mHandle))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to flush file (%d) content.",
                           mHandle);
    }
}


uint64_t File::Size () const
{
  if (mFileSize != UNKNOWN_SIZE)
    return mFileSize;

  if ( ! whf_tell_size (mHandle, &_CC (uint64_t&, mFileSize)))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to get file (%d) size.",
                           mHandle);
    }

  return mFileSize;
}


void
File::Size (const uint64_t size)
{
  if ( ! whf_set_size (mHandle, size))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to update file (%d) size.",
                           mHandle);
    }

  mFileSize = UNKNOWN_SIZE;
}


void
File::Close ()
{
  if ((mHandle == INVALID_FILE) || (mHandle == FILE_LOCKED))
    return ;

  if ( ! whf_close (mHandle))
    {
      throw FileException (_EXTRA (whf_last_error ()),
                           "Failed to close file (%d).",
                           mHandle);
    }

  mHandle = INVALID_FILE;
}


File&
File::operator= (const File &src)
{
  if (&src == this)
    return *this;

  Close (); // Close the old handler

  mHandle = src.mHandle;
  _CC (File&, src).mHandle = INVALID_FILE;

  return *this;
}


} //namespace whais

