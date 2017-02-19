/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#include <cstdio>
#include <cstdarg>
#include <cstring>

#include "whais.h"
#include "utils/wfile.h"
#include "utils/wthread.h"
#include "utils/wsocket.h"
#include "utils/auto_array.h"


#ifndef va_copy
  #if defined __va_copy
      #define va_copy(x,y) __va_copy(x,y)
      #define _va_end_w(x) va_end(x)
  #elif defined(ARCH_PPC) && defined(_GNU_SOURCE)
      #define va_copy(x,y) (*x = *y)
      #define _va_end_w(x)
  #else
      #define va_copy(x,y) (x = y)
      #define _va_end_w(x)
  #endif
#else
  #define _va_end_w(x) va_end(x)
#endif


namespace whais {

Exception::Exception(uint32_t        code,
                      const char*     file,
                      uint32_t        line)
  : mErrorMessage(),
    mFile(file),
    mLine(line),
    mCode(code)
{
}


Exception::Exception(const Exception& source)
  : mErrorMessage(source.mErrorMessage),
    mFile(source.mFile),
    mLine(source.mLine),
    mCode(source.mCode)
{
}


Exception::~Exception()
{
}


uint32_t
Exception::Code() const
{
  return mCode;
}


const std::string&
Exception::Message() const
{
  return mErrorMessage;
}


void
Exception::Message(const char* fmtMsg, va_list vl)
{
  std::string msgHolder;
  va_list     c_vl;

  int maxSize = 128;

  while (true)
    {
      msgHolder.resize(maxSize);
      va_copy(c_vl, vl);

      const int actualSize = vsnprintf(_CC(char*, msgHolder.c_str()),
                                        maxSize,
                                        fmtMsg,
                                        c_vl);
      _va_end_w(c_vl);
      if ((0 <= actualSize) && (actualSize < maxSize))
        {
          mErrorMessage = std::string(msgHolder.c_str());
          break;
        }
      maxSize *= 2;
    }
}


void
Exception::Message(const char* fmtMsg, ... )
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start(vl, fmtMsg);
      this->Message(fmtMsg, vl);
      va_end(vl);
    }
  else
    mErrorMessage.clear();
}


const char*
Exception::File() const
{
  return mFile;
}


uint32_t
Exception::Line() const
{
  return mLine;
}




FileException::FileException(const uint32_t    code,
                              const char*       file,
                              uint32_t          line,
                              const char*       fmtMsg,
                              ...)
  : Exception(code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start(vl, fmtMsg);
      this->Message(fmtMsg, vl);
      va_end(vl);
    }
}


Exception*
FileException::Clone() const
{
  return new FileException(*this);
}


EXCEPTION_TYPE
FileException::Type() const
{
  return FILE_EXCEPTION;
}


const char*
FileException::Description() const
{
  return "File IO error.";
}



LockException::LockException(const uint32_t    code,
                              const char*       file,
                              uint32_t          line,
                              const char*       fmtMsg,
                              ...)
  : Exception(code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start(vl, fmtMsg);
      this->Message(fmtMsg, vl);
      va_end(vl);
    }
}


Exception*
LockException::Clone() const
{
  return new LockException(*this);
}


EXCEPTION_TYPE
LockException::Type() const
{
  return SYNC_EXCEPTION;
}


const char*
LockException::Description() const
{
  return "Thread synchronization error.";
}



ThreadException::ThreadException(const uint32_t    code,
                                  const char*       file,
                                  uint32_t          line,
                                  const char*       fmtMsg,
                                  ... )
  : Exception(code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start(vl, fmtMsg);
      this->Message(fmtMsg, vl);
      va_end(vl);
    }
}


Exception*
ThreadException::Clone() const
{
  return new ThreadException(*this);
}


EXCEPTION_TYPE
ThreadException::Type() const
{
  return THREAD_EXCEPTION;
}


const char*
ThreadException::Description() const
{
  return "Thread execution error.";
}




SocketException::SocketException(const uint32_t        code,
                                  const char*           file,
                                  uint32_t              line,
                                  const char*           fmtMsg,
                                  ...)
  : Exception(code, file, line)
{
    if (fmtMsg != NULL)
      {
        va_list vl;

        va_start(vl, fmtMsg);
        this->Message(fmtMsg, vl);
        va_end(vl);
      }
}


Exception*
SocketException::Clone() const
{
  return new SocketException(*this);
}


EXCEPTION_TYPE
SocketException::Type() const
{
  return SOCKED_EXCEPTION;
}


const char*
SocketException::Description() const
{

  return "Network IO error.";
}


} //namespace whais

