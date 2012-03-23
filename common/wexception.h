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
  THREAD_EXCEPTION,
  INTERPRETER_EXCEPTION
};

class WException
{
public:
  WException (const D_CHAR* pMessage,
              const D_CHAR* pFile,
              D_UINT32      line,
              D_UINT32      extra)
  : m_ErrorMessage (pMessage),
    m_File (pFile),
    m_Line (line),
    m_Extra (extra)
  {
  }
  virtual ~WException () {};

  D_UINT32 GetExtra ()
  {
    return m_Extra;
  };

  const D_CHAR* GetDescription ()
  {
    return m_ErrorMessage;
  }

  const D_CHAR* GetFile ()
  {
    return m_File;
  }

  D_UINT32 GetLine ()
  {
    return m_Line;
  }

  virtual WException*     Clone () = 0;
  virtual EXPCEPTION_TYPE GetType () = 0;

private:
  const D_CHAR* m_ErrorMessage;
  const D_CHAR* m_File;
  D_UINT32      m_Line;
  D_UINT32      m_Extra;
};

// Macro used to expand the CMD line
#define _EXTRA(x)  __FILE__, __LINE__, (x)

#endif /* WEXCEPTION_H_ */
