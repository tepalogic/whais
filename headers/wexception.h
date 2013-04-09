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
#ifndef __cplusplus
#error "This header file should not be included from a C source file!"
#endif

#ifndef WEXCEPTION_H_
#define WEXCEPTION_H_

#include "string"

//Base class to handle all exceptions
enum EXPCEPTION_TYPE
{
  COMPILER_CMD_LINE_EXCEPTION,
  DBS_EXCEPTION,
  DUMP_EXCEPTION,
  DUMP_CMD_LINE_EXCEPTION,
  FILE_EXCEPTION,
  FILE_CONTAINER_EXCEPTION,
  UNIT_COMPILE_EXCEPTION,
  SYNC_EXCEPTION,
  THREAD_EXCEPTION,
  INTERPRETER_EXCEPTION,
  SOCKED_EXCEPTION,
  CONNECTION_EXCEPTION
};

class EXCEP_SHL WException
{
public:
  WException (const char* pMessage,
              const char* pFile,
              uint32_t      line,
              uint32_t      extra);
  WException (const WException& source);

  virtual ~WException ();

  uint32_t      GetExtra () const;
  const char* Message () const;
  const char* GetFile () const;
  uint32_t      GetLine () const;

  virtual WException*     Clone () const = 0;
  virtual EXPCEPTION_TYPE Type () const = 0;
  virtual const char*   Description () const = 0;

private:

#pragma warning( disable : 4251 )
  const std::string m_ErrorMessage;
#pragma warning( default : 4251)

  const char*     m_File;
  uint32_t          m_Line;
  uint32_t          m_Extra;
};

// Macro used to expand the CMD line
#define _EXTRA(x)  __FILE__, __LINE__, (x)

#endif /* WEXCEPTION_H_ */
