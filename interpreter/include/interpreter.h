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

#include "dbs/dbs_mgr.h"
#include "compiler/compiledunit.h"
#include "utils/logger.h"

#include "operands.h"

namespace whisper
{


class InterException : public Exception
{
public:
  explicit InterException (const char*  message,
                           const char*  file,
                           const uint32_t line,
                           const uint32_t extra)
    : Exception (message, file, line, extra)
  {
  }

  virtual ~InterException ()
  {
  }

  virtual Exception*      Clone () const { return new InterException (*this); }
  virtual EXPCEPTION_TYPE Type () const { return INTERPRETER_EXCEPTION; }
  virtual const char*   Description () const
  {
    switch (Extra ())
      {
        case INVALID_OP_CONVERSION:
          return "Invalid operand conversion.";
        case INVALID_OP_REQ:
          return "Invalid operand request.";
        case INVALID_GLOBAL_REQ:
          return "Invalid global value request.";
        case INVALID_TYPE_DESC:
          return "Invalid type description.";
        case DUPLICATE_DEFINITION:
          return "Definition two global symbols with the same name.";
        case EXTERNAL_FIRST:
          return "An external declaration of a symbol encountered before "
                 "it's definition.";
        case EXTERNAL_MISMATCH:
          return "Declaration of an external symbol is different from "
                 "its definition.";
        case INVALID_PROC_REQ:
          return "Invalid procedure request.";
        case INVALID_LOCAL_REQ:
          return "Invalid local value request.";
        case ALREADY_INITED:
          return "Already initialized.";
        case NOT_INITED:
          return "Not initialized.";
         case INVALID_SESSION:
          return "Invalid session.";
         case SESSION_IN_USE:
          return "Session in use.";
         case TEXT_ARRAY_NOT_SUPP:
          return "The current implementation does not have support for "
                 "text arrays.";
         default:
             assert (false);
             return "Unknown exception.";
      }
  }

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
    INVALID_SYNC_REQ,
    NEESTED_SYNC_REQ,
    SYNC_NOT_AQUIRED,

    ALREADY_INITED,
    NOT_INITED,
    INVALID_SESSION,
    SESSION_IN_USE,

    STACK_CORRUPTED,

    INVALID_UNIT_GLB_INDEX,
    INVALID_UNIT_PROC_INDEX,
    INVALID_UNIT_DATA_OFF,

    TEXT_INDEX_NULL,
    ARRAY_INDEX_NULL,
    TEXT_ARRAY_NOT_SUPP,
    ROW_INDEX_NULL,

    FIELD_TYPE_MISMATCH,

    DIVIDE_BY_ZERO,

    INTERNAL_ERROR
  };
};

class INTERP_SHL I_Session
{
public:
  I_Session (Logger& log);
  virtual ~I_Session ();

  virtual void LoadCompiledUnit (WICompiledUnit& unit) = 0;
  virtual void ExecuteProcedure (const char* const pProcName,
                                 SessionStack&       stack) = 0;

  virtual uint_t GlobalValuesCount () const = 0;
  virtual uint_t ProceduresCount () const = 0;

  virtual const char* GlobalValueName (const uint_t index) const = 0;
  virtual const char* ProcedureName (const uint_t index) const = 0;

  virtual uint_t GlobalValueRawType (const uint32_t index) = 0;
  virtual uint_t GlobalValueRawType (const char* const name) = 0;
  virtual uint_t GlobalValueFieldsCount (const uint32_t index) = 0;
  virtual uint_t GlobalValueFieldsCount (const char* const name) = 0;

  virtual const char* GlobalValueFieldName (const uint32_t index,
                                              const uint32_t field) = 0;
  virtual const char* GlobalValueFieldName (const char* const name,
                                              const uint32_t      field) = 0;

  virtual uint_t GlobalValueFieldType (const uint32_t index,
                                       const uint32_t field) = 0;
  virtual uint_t GlobalValueFieldType (const char* const name,
                                       const uint32_t      field) = 0;

  virtual uint_t ProcedureParametersCount (const uint_t id) const = 0;
  virtual uint_t ProcedureParametersCount (const char* const name) const = 0;
  virtual uint_t ProcedurePameterRawType (const uint_t id,
                                          const uint_t param) = 0;
  virtual uint_t ProcedurePameterRawType (const char* const name,
                                          const uint_t        param) = 0;
  virtual uint_t ProcedurePameterFieldsCount (const uint_t id,
                                              const uint_t param) = 0;
  virtual uint_t ProcedurePameterFieldsCount (const char* const name,
                                              const uint_t        param ) = 0;
  virtual const char* ProcedurePameterFieldName (
                                                const uint_t id,
                                                const uint_t param,
                                                const uint_t field
                                                  ) = 0;
  virtual const char* ProcedurePameterFieldName (
                                                const char* const name,
                                                const uint_t        param,
                                                const uint_t        field
                                                  ) = 0;
  virtual uint_t ProcedurePameterFieldType (const uint_t id,
                                            const uint_t param,
                                            const uint_t field) = 0;
  virtual uint_t ProcedurePameterFieldType (const char* const name,
                                            const uint_t        param,
                                            const uint_t        field) = 0;
protected:
  Logger& m_Log;
};

INTERP_SHL void
InitInterpreter (const char* adminDbsDir = NULL);

INTERP_SHL I_Session&
GetInstance (const char* pName, Logger* pLog = NULL);

INTERP_SHL void
ReleaseInstance (I_Session& hInstance);

INTERP_SHL void
CleanInterpreter (const bool force = false);

} // namespace whisper

#endif /* INTERPRETER_H_ */

