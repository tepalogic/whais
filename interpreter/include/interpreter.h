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



class INTERP_SHL InterException : public Exception
{
public:
  explicit InterException (const uint32_t  code,
                           const char*     file,
                           uint32_t        line,
                           const char*     fmtMsg = NULL,
                           ...);
  virtual Exception* Clone () const;

  virtual EXCEPTION_TYPE Type () const;

  virtual const char* Description () const;

  enum
  {
    INVALID_OP_REQ,
    INVALID_NATIVE_OP_REQ,
    NATIVE_NULL_DEREFERENCE,
    INVALID_PARAMETER_TYPE,
    INVALID_PARAMETER_VALUE,
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

    INVALID_SESSION,
    SESSION_IN_USE,

    STACK_CORRUPTED,
    NATIVE_CALL_FAILED,

    TEXT_INDEX_NULL,
    ARRAY_INDEX_NULL,
    TEXT_ARRAY_NOT_SUPP,
    ROW_INDEX_NULL,

    FIELD_TYPE_MISMATCH,
    FIELD_NAME_TOO_LONG,

    DIVIDE_BY_ZERO,

    //The exception codes below this line cause an application stop.
    __CRITICAL_EXCEPTIONS,
    ALREADY_INITED,
    NOT_INITED,
    INVALID_UNIT_GLB_INDEX,
    INVALID_UNIT_PROC_INDEX,
    INVALID_UNIT_DATA_OFF,
    INTERNAL_ERROR
  };
};



class INTERP_SHL ISession
{
public:
  ISession (Logger& log);
  virtual ~ISession ();

  virtual void LoadCompiledUnit (WIFunctionalUnit& unit) = 0;
  virtual bool LoadSharedLib (WH_SHLIB shl) = 0;

  virtual void ExecuteProcedure (const char* const   name,
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
  virtual const char* GlobalValueFieldName (const char* const   name,
                                            const uint32_t      field) = 0;

  virtual uint_t GlobalValueFieldType (const uint32_t index,
                                       const uint32_t field) = 0;
  virtual uint_t GlobalValueFieldType (const char* const   name,
                                       const uint32_t      field) = 0;

  virtual uint_t ProcedureParametersCount (const uint_t id) const = 0;
  virtual uint_t ProcedureParametersCount (const char* const name) const = 0;

  virtual uint_t ProcedurePameterRawType (const uint_t id,
                                          const uint_t param) = 0;
  virtual uint_t ProcedurePameterRawType (const char* const   name,
                                          const uint_t        param) = 0;

  virtual uint_t ProcedurePameterFieldsCount (const uint_t id,
                                              const uint_t param) = 0;
  virtual uint_t ProcedurePameterFieldsCount (const char* const   name,
                                              const uint_t        param ) = 0;

  virtual const char* ProcedurePameterFieldName (const uint_t id,
                                                 const uint_t param,
                                                 const uint_t field ) = 0;
  virtual const char* ProcedurePameterFieldName (const char* const   name,
                                                 const uint_t        param,
                                                 const uint_t        field) = 0;

  virtual uint_t ProcedurePameterFieldType (const uint_t id,
                                            const uint_t param,
                                            const uint_t field) = 0;
  virtual uint_t ProcedurePameterFieldType (const char* const   name,
                                            const uint_t        param,
                                            const uint_t        field) = 0;
  Logger& GetLogger ()
  {
    return mLog;
  }

protected:
  Logger& mLog;
};



INTERP_SHL void
InitInterpreter (const char* adminDbsDir = NULL);


INTERP_SHL ISession&
GetInstance (const char* name, Logger* log = NULL);


INTERP_SHL void
ReleaseInstance (ISession& instance);


INTERP_SHL void
CleanInterpreter (const bool force = false);


} // namespace whisper

#endif /* INTERPRETER_H_ */

