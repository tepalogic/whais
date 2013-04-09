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

Exception::Exception (const char* pMessage,
                        const char* pFile,
                        uint32_t      line,
                        uint32_t      extra)
  : mErrorMessage ((pMessage == NULL) ? "" : pMessage),
    mFile (pFile),
    mLine (line),
    mExtra (extra)
{
}

Exception::Exception (const Exception& source)
  : mErrorMessage (source.mErrorMessage),
    mFile (source.mFile),
    mLine (source.mLine),
    mExtra (source.mExtra)
{
}

Exception::~Exception ()
{
}

uint32_t
Exception::Extra () const
{
  return mExtra;
}

const char*
Exception::Message () const
{
  return mErrorMessage.c_str ();
}

const char*
Exception::File () const
{
  return mFile;
}

uint32_t
Exception::Line () const
{
  return mLine;
}


///////////////////////////////////////////////////////////////////////////////
WFileException::WFileException (const char* pMessage,
                                const char* pFile,
                                uint32_t      line,
                                uint32_t      extra)
  : Exception (pMessage, pFile, line, extra)
{
}

Exception*
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
  : Exception (message, file, line, extra)
{
}

Exception*
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
  : Exception (message, file, line, extra)
{
}

Exception*
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
  : Exception (message, file, line, extra)
{
}

Exception*
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
