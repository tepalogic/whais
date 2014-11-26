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

#include "interpreter.h"


using namespace whais;


InterException::InterException( const uint32_t  code,
                                const char*     file,
                                uint32_t        line,
                                const char*     fmtMsg,
                                ...)
  : Exception( code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start( vl, fmtMsg);
      this->Message( fmtMsg, vl);
      va_end( vl);
    }
}


Exception*
InterException::Clone() const
{
  return new InterException( *this);
}


EXCEPTION_TYPE
InterException::Type() const
{
  return INTERPRETER_EXCEPTION;
}


const char*
InterException::Description() const
{
  switch( Code())
    {
  case INVALID_OP_REQ:
    return "An operation was requested that was not support by a stack operand.";

  case INVALID_NATIVE_OP_REQ:
    return "An operation was requested on a undefined object that turns out it cannot handle.";

  case NATIVE_NULL_DEREFERENCE:
    return "Tried to derefence an undefined object that turns out to be null.";

  case INVALID_PARAMETER_TYPE:
    return "A requested operation could not be completed due of the invalid type of an operand.";

  case INVALID_PARAMETER_VALUE:
    return "A requested operation could not be completed due of the invalid value of an operand.";

  case INVALID_GLOBAL_REQ:
    return "Could not fund a global value.";

  case INVALID_TYPE_DESC:
    return "A request to define a symbol failed due of an invalid type descriptor.";

  case DUPLICATE_DEFINITION:
    return "Failed to define a symbol as it was already defined.";

  case EXTERNAL_FIRST:
    return "Failed to bind an external symbol because its definition was not fund.";

  case EXTERNAL_MISMATCH:
    return "Failed to bind an external symbol because its definition has a different type.";

  case INVALID_PROC_REQ:
    return "Could not fund a procedure.";

  case INVALID_LOCAL_REQ:
    return "A procedure local value description cannot be fund.";

  case INVALID_SYNC_REQ:
    return "Failed to acquire or release a procedure synchronized statement.";

  case NEESTED_SYNC_REQ:
    return "Detected an acquiring of a nested procedure synchronized statement.";

  case SYNC_NOT_AQUIRED:
    return "Tried to release a procedure synchronized statement that was not acquired priori.";

  case INVALID_SESSION:
    return "Tried to use an unknown session.";

  case SESSION_IN_USE:
    return "On requested failed because of a session is still in use.";

  case STACK_CORRUPTED:
    return "The interpreter detected a stack corruption";

  case NATIVE_CALL_FAILED:
    return "Failed to execute a native procedure.";

  case TEXT_INDEX_NULL:
    return "Cannot retrieve a text character due to null index operand.";

  case ARRAY_INDEX_NULL:
    return "Cannot retrieve an array value due to null index operand.";

  case ROW_INDEX_NULL:
    return "Cannot retrieve an table row due to null index operand.";

  case FIELD_TYPE_MISMATCH:
    return "A field operation failed due to invalid field type.";

  case FIELD_NAME_TOO_LONG:
    return "Cannot use a field name because is too long.";

  case DIVIDE_BY_ZERO:
    return "Cannot divide by zero.";

  case ALREADY_INITED:
    return "Cannot initialize the interpreter as it was already initialized.";

  case NOT_INITED:
    return "Cannot use the interpreter because it was not initialized.";

  case INVALID_UNIT_GLB_INDEX:
    return "Cannot find a global value unit index.";

  case INVALID_UNIT_PROC_INDEX:
    return "Cannot find a procedure unit index.";

  case INVALID_UNIT_DATA_OFF:
    return "Cannot find a unit constant data.";

  case INTERNAL_ERROR:
    return "GENERAL FAILURE: Program execution must stop due to a interpreter internal error.";
    }

  assert( false);
  return "An unknown interpreter exception was thrown.";
}

