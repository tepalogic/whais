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

WException::WException (const D_CHAR* pMessage,
                        const D_CHAR* pFile,
                        D_UINT32      line,
                        D_UINT32      extra)
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

D_UINT32
WException::GetExtra () const
{
  return m_Extra;
}

const D_CHAR*
WException::Message () const
{
  return m_ErrorMessage.c_str ();
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


///////////////////////////////////////////////////////////////////////////////
WFileException::WFileException (const D_CHAR* pMessage,
                                const D_CHAR* pFile,
                                D_UINT32      line,
                                D_UINT32      extra)
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

const D_CHAR*
WFileException::Description () const
{
  return "File IO error.";
}

//////////////////////////WSynchException/////////////////////////////////////

WSynchException::WSynchException (const D_CHAR* message,
                                  const D_CHAR* file,
                                  D_UINT32      line,
                                  D_UINT32      extra)
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

const D_CHAR*
WSynchException::Description () const
{
  return "A mutex related exception.";
}

//////////////////////////WThreadException/////////////////////////////////////

WThreadException::WThreadException (const D_CHAR* message,
                                    const D_CHAR* file,
                                    D_UINT32      line,
                                    D_UINT32      extra)
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

const D_CHAR*
WThreadException::Description () const
{
  return "Thread related exception.";
}

//////////////////////////WSocketException////////////////////////////////////

WSocketException::WSocketException (const D_CHAR* message,
                                    const D_CHAR* file,
                                    D_UINT32      line,
                                    D_UINT32      extra)
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

const D_CHAR*
WSocketException::Description () const
{

  switch (GetExtra ())
  {
  case WH_SOCK_NOTINIT:
    return "Socket framework not initialized.";
  case WH_SOCK_ENOBUF:
  case WH_SOCK_NOMEM:
    return "Insufficient memory to complete socket operation.";
  case WH_SOCK_NONAME:
    return "Host name could not be found.";
  case WH_SOCK_NOSERV:
    return "Service requested could not be identified.";
  case WH_SOCK_EPIPE:
    return "Connection with the remote peer had been interrupted.";
  case WH_SOCK_NOTRICH:
    return "The remote host is unreachable.";
  case WH_SOCK_NOPERM:
    return "Socket operation not permitted.";
  case WH_SOCK_ETIMEOUT:
    return "Socket operation has timeout.";
  case WH_SOCK_EINVAL:
    return "Invalid parameters for requested socket operation.";
  }
  return "Unknown socket exception";
}
