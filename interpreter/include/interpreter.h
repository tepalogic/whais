/******************************************************************************
 PRIMA - A language interpretor for wshiper source objects
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
 *****************************************************************************/

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <string>

#include "whisper.h"

#include "dbs/include/dbs_mgr.h"
#include "compiler/include/whisperc/compiledunit.h"

#include "operands.h"

class InterException : public WException
{
public:
  explicit InterException (const D_CHAR*  message,
                           const D_CHAR*  file,
                           const D_UINT32 line,
                           const D_UINT32 extra)
    : WException (message, file, line, extra)
  {
  }

  virtual ~InterException ()
  {
  }

  virtual InterException* Clone () { return new InterException (*this); }
  virtual EXPCEPTION_TYPE Type () { return INTERPRETER_EXCEPTION; }

  enum
  {
    INVALID_OP_CONVERSION,
    INVALID_OP_REQ,
    INVALID_GLOBAL_REQ,
    INVALID_TYPE_DESC,
    DUPLICATE_DEFINITION,
    EXTERNAL_FIRST,
    EXTERNAL_MISMATCH,
    INVALID_PROC_REQ,
    INVALID_LOCAL_REQ,

    ALREADY_INITED,
    NOT_INITED,
    INVALID_SESSION,
    SESSION_IN_USE,

    STACK_CORRUPTED,

    INVALID_UNIT_GLB_INDEX,
    INVALID_UNIT_PROC_INDEX,
    INVALID_UNIT_DATA_OFF,

    FIELD_TYPE_MISMATCH,
    INVALID_ROW_INDEX,
    INVALID_ARRAY_INDEX,

    INTERNAL_ERROR
  };
};

enum LOG_LEVEL
{
  LOG_GENERAL_INFO  = 0x01,
  LOG_EXCEPION_DESC = 0x02,
  LOG_INT_WARNING   = 0x04,
  LOG_INT_ERROR     = 0x08,
  LOG_INT_CRITICAL  = 0x10
};

class I_Session
{
public:
  I_Session ()
  {
  }

  virtual ~I_Session ()
  {
  }

  virtual void LoadCompiledUnit (WICompiledUnit& unit) = 0;
  virtual void ExecuteProcedure (const D_CHAR* const pProcName,
                                 StackValue&         stack) = 0;
  virtual void LogMessage (const LOG_LEVEL level, std::string& message) = 0;
};

void
InitInterpreter ();

I_Session&
GetInstance (const D_CHAR* pName);

void
ReleaseInstance (I_Session& hInstance);

void
CleanInterpreter (const bool force = false);

#endif /* INTERPRETER_H_ */

