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

#include <string>

namespace whisper
{

//Base class to handle all exceptions
enum EXCEPTION_TYPE
{
  COMPILER_CMD_LINE_EXCEPTION,
  DBS_EXCEPTION,
  DUMP_EXCEPTION,
  DUMP_CMD_LINE_EXCEPTION,
  FILE_EXCEPTION,
  FILE_CONTAINER_EXCEPTION,
  FUNCTIONAL_UNIT_EXCEPTION,
  SYNC_EXCEPTION,
  THREAD_EXCEPTION,
  INTERPRETER_EXCEPTION,
  SOCKED_EXCEPTION,
  CONNECTION_EXCEPTION
};

class EXCEP_SHL Exception
{
public:
  Exception (const char*   message,
             const char*   file,
             uint32_t      line,
             uint32_t      extra);
  Exception (const Exception& source);

  virtual ~Exception ();

  uint32_t      Extra () const;
  const char*   Message () const;
  void          Message (const std::string& msg);
  const char*   File () const;
  uint32_t      Line () const;

  virtual Exception* Clone () const = 0;

  virtual EXCEPTION_TYPE Type () const = 0;

  virtual const char* Description () const = 0;

private:

#pragma warning( disable : 4251 )
  std::string       mErrorMessage;
#pragma warning( default : 4251)

  const char*       mFile;
  uint32_t          mLine;
  uint32_t          mExtra;
};

} //namespace whisper

// Macro used to expand the CMD line
#define _EXTRA(x)  __FILE__, __LINE__, (x)

#endif /* WEXCEPTION_H_ */

