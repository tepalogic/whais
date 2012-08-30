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



WFile::WFile (const D_CHAR* pFileName, D_UINT mode)
  : m_Handle (0)
{
  m_Handle = whc_fopen (pFileName, mode);
  if (m_Handle == 0)
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
}

WFile::WFile (const WFile &rSource) :
  m_Handle (whc_fdup (rSource.m_Handle))
{
  if (m_Handle == 0)
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
}

WFile::~WFile ()
{
  /* Close it only if is not already closed */
  if (m_Handle != 0)
    whc_fclose (m_Handle);
}

void
WFile::Read (D_UINT8* pBuffer, D_UINT size)
{
  if (!whc_fread (m_Handle, pBuffer, size))
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
}

void
WFile::Write (const D_UINT8* pBuffer, D_UINT size)
{
  if (!whc_fwrite (m_Handle, pBuffer, size))
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
}

void
WFile::Seek (const D_INT64 where, const D_INT whence)
{
  if (!whc_fseek (m_Handle, where, whence))
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
}

D_UINT64 WFile::Tell ()
{
  D_UINT64 position;

  if (!whc_ftell (m_Handle, &position))
    throw WFileException (NULL, _EXTRA(whc_fgetlasterror ()));

  return position;
}

void
WFile::Sync ()
{
  if (!whc_fsync (m_Handle))
    throw WFileException (NULL,_EXTRA(whc_fgetlasterror ()));
}

D_UINT64 WFile::GetSize () const
{
  D_UINT64 size;

  if (!whc_ftellsize (m_Handle, &size))
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));

  return size;
}

void
WFile::SetSize (const D_UINT64 size)
{
  if (!whc_fsetsize (m_Handle, size))
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
}

void
WFile::Close ()
{
  assert (m_Handle != 0);

  if (whc_fclose (m_Handle))
    m_Handle = 0;
  else
    {
      m_Handle = 0;
      throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));
    }
}

WFile&
WFile::operator= (const WFile &rSource)
{
  if (&rSource == this)
    return *this;

  Close(); // Close the old handler

  m_Handle = whc_fdup (rSource.m_Handle);
  if (m_Handle == 0)
    throw WFileException (NULL, _EXTRA (whc_fgetlasterror ()));

  return *this;
}
