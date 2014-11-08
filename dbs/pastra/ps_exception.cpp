/******************************************************************************
 PASTRA - A light database one file system and more.
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

#include <cassert>

#include "dbs_exception.h"


using namespace whais;

DBSException::DBSException( const uint32_t  code,
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


DBSException::~DBSException( )
{
}


Exception*
DBSException::Clone( ) const
{
  return new DBSException( *this);
}


EXCEPTION_TYPE
DBSException::Type( ) const
{
  return DBS_EXCEPTION;
}


const char*
DBSException::Description( ) const
{

  switch( Code( ))
  {
  case OPER_NOT_SUPPORTED:
    return "A requested operation is not supported.";

  case ALREADY_INITED:
    return "Requested an initialization of an already initialized object.";

  case NOT_INITED:
    return "Tried to use an object without been initialized first.";

  case INAVLID_DATABASE:
    return "Tried to use a database with unsupported parameters or format.";

  case DATABASE_IN_USE:
    return "A requested operation on a database failed because is still in use.";

  case DATABASE_EXISTS:
    return "A database with the same name already exists.";

  case TABLE_INCONSITENCY:
    return "Tried to use a database table using unsupported parameters.";

  case TABLE_NOT_FUND:
    return "The requested database table cannot be fund.";

  case TABLE_EXISTS:
    return "Tried to create a table with a name already used.";

  case TABLE_IN_USE:
    return "A requested operation on a database table failed because is still in use.";

  case TABLE_INVALID:
    return "Tried to use a database table with unsupported parameters or format.";

  case FIELD_NAME_INVALID:
    return "Tried to create a table using an invalid name.";

  case FIELD_TYPE_INVALID:
    return "Tried to create a table using an invalid or unsupported field type.";

  case FIELD_NOT_FOUND:
    return "Tried to select an inexistent table field.";

  case FIELD_NOT_INDEXED:
    return "Tried to remove the index associated with a table field though was not indexed.";

  case FIELD_INDEXED:
    return "Tried to associate an index with a table field though it is already indexed.";

  case INVALID_PARAMETERS:
    return "An operation failed due to invalid parameters.";

  case INVALID_UTF8_STRING:
    return "Tried to use an invalid UTF8 encoded string.";

  case STRING_INDEX_TOO_BIG:
    return "Tried to access a character outside of a string boundaries.";

  case ARRAY_INDEX_TOO_BIG:
    return "Tried to access an array element outside of its bounds.";

  case INVALID_ARRAY_TYPE:
    return "The requested operation failed due to unmatched array type.";

  case NULL_ARRAY_ELEMENT:
    return "Tried to assign a null value to an array element.";

  case INVALID_UNICODE_CHAR:
    return "Specified code point is not Unicode valid.";

  case INVALID_DATE:
    return "Tried to create a date value with invalid parameters.";

  case INVALID_DATETIME:
    return "Tried to create a time value with invalid parameters.";

  case INVALID_HIRESTIME:
    return "Tried to create a high resolution time value with invalid parameters.";

  case ROW_NOT_ALLOCATED:
    return "Tried to access a table row that was not allocated.";

  case TABLE_ALREADY_LOCKED:
    return "Tried to lock a table content, but it was already locked.";

  case NUMERIC_FAULT:
    return "A numeric fault was encountered.";

  case TABLE_NOT_LOCKED:
    return "Tried to unlock a table though it was not locked  locked.";

  case TABLE_RECOVER_FAILED:
    return "Failed to recover a database table.";

  case BAD_PARAMETERS:
    return "Program execution could not continue due to invalid parameters.";

  case GENERAL_CONTROL_ERROR:
    return "GENERAL FAILURE: Program execution must be stopped due to a DBS internal error.";
  }

  assert( false);
  return "An unknown DBS framework exception was thrown";
};
