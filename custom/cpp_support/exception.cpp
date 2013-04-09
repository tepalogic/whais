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
#include "utils/include/wfile.h"
#include "utils/include/wthread.h"
#include "utils/include/wsocket.h"

WException::WException (const char* pMessage,
                        const char* pFile,
                        uint32_t      line,
                        uint32_t      extra)
  : m_ErrorMessage ((pMessage == NULL) ? "" : pMessage),
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

uint32_t
WException::GetExtra () const
{
  return m_Extra;
}

const char*
WException::Message () const
{
  return m_ErrorMessage.c_str ();
}

const char*
WException::GetFile () const
{
  return m_File;
}

uint32_t
WException::GetLine () const
{
  return m_Line;
}


///////////////////////////////////////////////////////////////////////////////
WFileException::WFileException (const char* pMessage,
                                const char* pFile,
                                uint32_t      line,
                                uint32_t      extra)
  : WException (pMessage, pFile, line, extra)
{
}

WException*
WFileException::Clone () const
{
  return new WFileException (*this);
}

EXPCEPTION_TYPE
WFileException::Type () const
{
  return FILE_EXCEPTION;
}

const char*
WFileException::Description () const
{
  return "File IO error.";
}

//////////////////////////WSynchException/////////////////////////////////////

WSynchException::WSynchException (const char* message,
                                  const char* file,
                                  uint32_t      line,
                                  uint32_t      extra)
  : WException (message, file, line, extra)
{
}

WException*
WSynchException::Clone () const
{
  return new WSynchException (*this);
}

EXPCEPTION_TYPE
WSynchException::Type () const
{
  return SYNC_EXCEPTION;
}

const char*
WSynchException::Description () const
{
  return "A mutex related exception.";
}

//////////////////////////WThreadException/////////////////////////////////////

WThreadException::WThreadException (const char* message,
                                    const char* file,
                                    uint32_t      line,
                                    uint32_t      extra)
  : WException (message, file, line, extra)
{
}

WException*
WThreadException::Clone () const
{
  return new WThreadException (*this);
}

EXPCEPTION_TYPE
WThreadException::Type () const
{
  return THREAD_EXCEPTION;
}

const char*
WThreadException::Description () const
{
  return "Thread related exception.";
}

//////////////////////////WSocketException////////////////////////////////////

WSocketException::WSocketException (const char* message,
                                    const char* file,
                                    uint32_t      line,
                                    uint32_t      extra)
  : WException (message, file, line, extra)
{
}

WException*
WSocketException::Clone () const
{
  return new WSocketException (*this);
}

EXPCEPTION_TYPE
WSocketException::Type () const
{
  return SOCKED_EXCEPTION;
}

const char*
WSocketException::Description () const
{

  return "Network socket related exception";
}
