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

#include "utils/include/wfile.h"
#include "whisper.h"

using namespace whisper;

WFile::WFile (const char* pFileName, uint_t mode)
  : m_Handle (0)
{
  m_Handle = whf_open (pFileName, mode);
  if (m_Handle == 0)
    throw WFileException (pFileName, _EXTRA (whf_last_error ()));
}

WFile::WFile (const WFile &rSource) :
  m_Handle (whf_dup (rSource.m_Handle))
{
  if (m_Handle == 0)
    throw WFileException (NULL, _EXTRA (whf_last_error ()));
}

WFile::~WFile ()
{
  /* Close it only if is not already closed */
  if (m_Handle != 0)
    whf_close (m_Handle);
}

void
WFile::Read (uint8_t* pBuffer, uint_t size)
{
  if (!whf_read (m_Handle, pBuffer, size))
    throw WFileException (NULL, _EXTRA (whf_last_error ()));
}

void
WFile::Write (const uint8_t* pBuffer, uint_t size)
{
  if (!whf_write (m_Handle, pBuffer, size))
    throw WFileException (NULL, _EXTRA (whf_last_error ()));
}

void
WFile::Seek (const int64_t where, const int whence)
{
  if (!whf_seek (m_Handle, where, whence))
    throw WFileException (NULL, _EXTRA (whf_last_error ()));
}

uint64_t WFile::Tell ()
{
  uint64_t position;

  if (!whf_tell (m_Handle, &position))
    throw WFileException (NULL, _EXTRA(whf_last_error ()));

  return position;
}

void
WFile::Sync ()
{
  if (!whf_sync (m_Handle))
    throw WFileException (NULL,_EXTRA(whf_last_error ()));
}

uint64_t WFile::GetSize () const
{
  uint64_t size;

  if (!whf_tell_size (m_Handle, &size))
    throw WFileException (NULL, _EXTRA (whf_last_error ()));

  return size;
}

void
WFile::SetSize (const uint64_t size)
{
  if (!whf_set_size (m_Handle, size))
    throw WFileException (NULL, _EXTRA (whf_last_error ()));
}

void
WFile::Close ()
{
  assert (m_Handle != 0);

  if (whf_close (m_Handle))
    m_Handle = 0;
  else
    {
      m_Handle = 0;
      throw WFileException (NULL, _EXTRA (whf_last_error ()));
    }
}

WFile&
WFile::operator= (const WFile &rSource)
{
  if (&rSource == this)
    return *this;

  Close(); // Close the old handler

  m_Handle = whf_dup (rSource.m_Handle);
  if (m_Handle == 0)
    throw WFileException (NULL, _EXTRA (whf_last_error ()));

  return *this;
}
