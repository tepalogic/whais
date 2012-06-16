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
                                 const D_UINT32 extra)
    : WException (message, file, line, extra)
  {
  }

  virtual ~InterpreterException ()
  {
  }

  virtual InterpreterException* Clone () const
  {
    return new InterpreterException (*this);
  }
  virtual EXPCEPTION_TYPE       Type () const { return INTERPRETER_EXCEPTION; }
  virtual const D_CHAR*         Description () const
  {
    switch (GetExtra ())
      {
        case INVALID_OP_CONVERSION:
          return "Invalid operand conversion.";
        case INVALID_OP_REQUEST:
          return "Invalid operand request.";
        case INVALID_GLOBAL_REQUEST:
          return "Invalid global value request.";
        case INVALID_TYPE_DESCRIPTION:
          return "Invalid type description.";
        case DUPLICATE_DEFINITION:
          return "Definition two global symbols with the same name.";
        case EXTERNAL_FIRST:
          return "An external declaration of a symbol encountered before "
                 "it's definition.";
        case EXTERNAL_MISMATCH:
          return "Declaration of an external symbol is different from "
                 "its definition.";
        case INVALID_PROC_REQUEST:
           return "Invalid procedure request.";
        case INVALID_PROC_LOCAL_REQUEST:
           return "Invalid local value request.";
        case ALREADY_INITED:
           return "Already initialized.";
        case NOT_INITED:
           return "Not initialized.";
         case INVALID_SESSION:
           return "Invalid session.";
         case SESSION_IN_USE:
             return "Session in use.";
         default:
             assert (false);
             return "Unknown exception.";
      }
  }

  enum
  {
    INVALID_OP_CONVERSION,
    INVALID_OP_REQUEST,
    INVALID_GLOBAL_REQUEST,
    INVALID_TYPE_DESCRIPTION,
    DUPLICATE_DEFINITION,
    EXTERNAL_FIRST,
    EXTERNAL_MISMATCH,
    INVALID_PROC_REQUEST,
    INVALID_PROC_LOCAL_REQUEST,

    ALREADY_INITED,
    NOT_INITED,
    INVALID_SESSION,
    SESSION_IN_USE
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
  virtual void LogMessage (const LOG_LEVEL level, std::string& message) = 0;
};



void
InitInterpreter ();

I_Session&
GetInstance (const D_CHAR* const name);

void
ReleaseInterpreterInstance (I_Session& hInstance);

void
CleanInterpreter (const bool force = false);

#endif /* INTERPRETER_H_ */
