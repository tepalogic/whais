/******************************************************************************
WHISPER - An advanced database system
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
#include "whisper.h"



namespace whisper {


File::File (const char* name, uint_t mode)
  : mHandle (0)
{
  mHandle = whf_open (name, mode);
  if (mHandle == _SC (WH_FILE, -1))
    throw FileException (name, _EXTRA (whf_last_error ()));
}


File::File (const File &src) :
  mHandle (whf_dup (src.mHandle))
{
  if (mHandle == _SC (WH_FILE, -1))
    throw FileException (NULL, _EXTRA (whf_last_error ()));
}


File::~File ()
{
  /* Close it only if is not already closed */
  if (mHandle != _SC (WH_FILE, -1))
    whf_close (mHandle);
}


void
File::Read (uint8_t* pBuffer, uint_t size)
{
  if ( ! whf_read (mHandle, pBuffer, size))
    throw FileException (NULL, _EXTRA (whf_last_error ()));
}


void
File::Write (const uint8_t* pBuffer, uint_t size)
{
  if ( ! whf_write (mHandle, pBuffer, size))
    throw FileException (NULL, _EXTRA (whf_last_error ()));
}


void
File::Seek (const int64_t where, const int whence)
{
  if ( ! whf_seek (mHandle, where, whence))
    throw FileException (NULL, _EXTRA (whf_last_error ()));
}


uint64_t File::Tell ()
{
  uint64_t position;

  if ( ! whf_tell (mHandle, &position))
    throw FileException (NULL, _EXTRA(whf_last_error ()));

  return position;
}


void
File::Sync ()
{
  if ( ! whf_sync (mHandle))
    throw FileException (NULL,_EXTRA(whf_last_error ()));
}


uint64_t File::GetSize () const
{
  uint64_t size;

  if ( ! whf_tell_size (mHandle, &size))
    throw FileException (NULL, _EXTRA (whf_last_error ()));

  return size;
}


void
File::SetSize (const uint64_t size)
{
  if ( ! whf_set_size (mHandle, size))
    throw FileException (NULL, _EXTRA (whf_last_error ()));
}


void
File::Close ()
{
  assert (mHandle != _SC (WH_FILE, -1));

  if ( ! whf_close (mHandle))
    throw FileException (NULL, _EXTRA (whf_last_error ()));

  mHandle = _SC (WH_FILE, -1);
}


File&
File::operator= (const File &src)
{
  if (&src == this)
    return *this;

  Close(); // Close the old handler

  mHandle = whf_dup (src.mHandle);
  if (mHandle == _SC (WH_FILE, -1))
    throw FileException (NULL, _EXTRA (whf_last_error ()));

  return *this;
}


} //namespace whisper

