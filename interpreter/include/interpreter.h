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


class InterpreterException : public WException
{
public:
  explicit InterpreterException (const D_CHAR*  message,
                                 const D_CHAR*  file,
                                 const D_UINT32 line,
                                 const D_UINT32 extra) :
     WException (message, file, line, extra)
  {
  }

  virtual ~InterpreterException ()
  {
  }

  virtual InterpreterException* Clone () { return new InterpreterException (*this); }
  virtual EXPCEPTION_TYPE       GetType () { return INTERPRETER_EXCEPTION; }

  enum
  {
    INVALID_OP_CONVERSION,
    INVALID_OP_REQUEST,
    INVALID_GLOBAL_REQUEST,
    INVALID_TYPE_DESCRIPTION,
    DUPLICATE_DEFINITION,
    EXTERNAL_FIRST,
    EXTERNAL_MISMATCH
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

class I_InterpreterLogger
{
public:

  I_InterpreterLogger ()
  {
  }

  virtual ~I_InterpreterLogger ()
  {
  }

  virtual void LogMessage (const LOG_LEVEL level, std::string& message) = 0;
  virtual void Flush () = 0;
};

class I_InterpreterSession
{
public:
  I_InterpreterSession ()
  {
  }

  virtual ~I_InterpreterSession ()
  {
  }

  virtual void LoadCompiledUnit (WICompiledUnit& unit) = 0;
  virtual void LogMessage (const LOG_LEVEL level, std::string& message) = 0;
};



void
InitInterpreter ();

I_InterpreterSession&
GetInterpreterInstance ();

I_InterpreterSession&
GetInterpreterInstance (I_DBSHandler& dbsHandler);

void
ReleaseInterpreterInstance (I_InterpreterSession& dbsHandler);

void
CleanInterpreter ();

#endif /* INTERPRETER_H_ */
