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

#include "../include/wfile.h"
#include "whisper.h"



WFile::WFile (const D_CHAR * fname, D_UINT mode) : mHandle (0)
{
  mHandle = whc_fopen (fname, mode);
  if (mHandle == 0)
    throw WFileException (NULL, _EXTRA(whc_fgetlasterror()));
}

WFile::WFile (const WFile &rSource)
:mHandle (whc_fdup (rSource.mHandle))
{
  if (mHandle == 0)
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
}

WFile::~WFile ()
{
  /* Close it only if is not already closed */
  if (mHandle != 0)
    whc_fclose (mHandle);
}

void
WFile::Read (D_UINT8 * buffer, D_UINT size)
{
  if (!whc_fread (mHandle, buffer, size))
    throw WFileException (NULL, _EXTRA(whc_fgetlasterror ()));
}

void
WFile::Write (const D_UINT8 * buffer, D_UINT size)
{
  if (!whc_fwrite (mHandle, buffer, size))
    throw WFileException (NULL, _EXTRA(whc_fgetlasterror ()));
}

void
WFile::Seek (D_INT64 where, D_INT whence)
{
  if (!whc_fseek (mHandle, where, whence))
    throw WFileException (NULL, _EXTRA(whc_fgetlasterror ()));
}

D_UINT64 WFile::Tell ()
{
  D_UINT64
    position;
  if (!whc_ftell (mHandle, &position))
    throw
    WFileException (NULL, _EXTRA(whc_fgetlasterror ()));

  return position;
}

void
WFile::Sync ()
{
  if (!whc_fsync (mHandle))
    throw WFileException (NULL,_EXTRA(whc_fgetlasterror ()));
}

D_UINT64 WFile::GetSize () const
{
  D_UINT64
    size;

  if (!whc_ftellsize (mHandle, &size))
    throw
    WFileException (NULL, _EXTRA(whc_fgetlasterror ()));
  return size;
}

void
WFile::SetSize (D_UINT64 size)
{
  if (!whc_fsetsize (mHandle, size))
    throw WFileException (NULL, _EXTRA(whc_fgetlasterror ()));
}

void
WFile::Close ()
{
  assert (mHandle != 0);

  if (whc_fclose (mHandle))
    mHandle = 0;
  else
    {
      mHandle = 0;
      throw WFileException (NULL, _EXTRA(whc_fgetlasterror ()));
    }
}

WFile&
WFile::operator= (const WFile &rSource)
{
  if (&rSource == this)
    return *this;

  Close(); // Close the old handler
  mHandle = whc_fdup (rSource.mHandle);
  if (mHandle == 0)
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));

  return *this;
}
