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

#include "whisper.h"

WException::WException (const D_CHAR* pMessage,
                        const D_CHAR* pFile,
                        D_UINT32      line,
                        D_UINT32      extra)
  : m_ErrorMessage (pMessage),
    m_File (pFile),
    m_Line (line),
    m_Extra (extra)
{
}

WException::WException (const WException& source)
  : m_ErrorMessage (source.m_ErrorMessage),
    m_File (source.m_File),
    m_Line (source.m_Line),
    m_Extra (source.m_Extra)
{
}

WException::~WException ()
{
}

D_UINT32
WException::GetExtra () const
{
  return m_Extra;
}

const D_CHAR*
WException::Message () const
{
  return m_ErrorMessage;
}

const D_CHAR*
WException::GetFile () const
{
  return m_File;
}

D_UINT32
WException::GetLine () const
{
  return m_Line;
}
