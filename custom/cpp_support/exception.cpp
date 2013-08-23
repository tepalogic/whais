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
#include "utils/wfile.h"
#include "utils/wthread.h"
#include "utils/wsocket.h"


namespace whisper {

Exception::Exception (const char*     message,
                      const char*     file,
                      uint32_t        line,
                      uint32_t        extra)
  : mErrorMessage ((message == NULL) ? "" : message),
    mFile (file),
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

void
Exception::Message (const std::string& msg)
{
  mErrorMessage = msg;
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




FileException::FileException (const char*   message,
                              const char*   file,
                              uint32_t      line,
                              uint32_t      extra)
  : Exception (message, file, line, extra)
{
}


Exception*
FileException::Clone () const
{
  return new FileException (*this);
}


EXCEPTION_TYPE
FileException::Type () const
{
  return FILE_EXCEPTION;
}


const char*
FileException::Description () const
{
  return "File IO error.";
}



LockException::LockException (const char*   message,
                              const char*   file,
                              uint32_t      line,
                              uint32_t      extra)
  : Exception (message, file, line, extra)
{
}


Exception*
LockException::Clone () const
{
  return new LockException (*this);
}


EXCEPTION_TYPE
LockException::Type () const
{
  return SYNC_EXCEPTION;
}


const char*
LockException::Description () const
{
  return "Thread synchronize error.";
}



ThreadException::ThreadException (const char*   message,
                                  const char*   file,
                                  uint32_t      line,
                                  uint32_t      extra)
  : Exception (message, file, line, extra)
{
}


Exception*
ThreadException::Clone () const
{
  return new ThreadException (*this);
}


EXCEPTION_TYPE
ThreadException::Type () const
{
  return THREAD_EXCEPTION;
}


const char*
ThreadException::Description () const
{
  return "Thread execution error.";
}




SocketException::SocketException (const char*   message,
                                  const char*   file,
                                  uint32_t      line,
                                  uint32_t      extra)
  : Exception (message, file, line, extra)
{
}


Exception*
SocketException::Clone () const
{
  return new SocketException (*this);
}


EXCEPTION_TYPE
SocketException::Type () const
{
  return SOCKED_EXCEPTION;
}


const char*
SocketException::Description () const
{

  return "Network IO error.";
}


} //namespace whisper

