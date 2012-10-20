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
******************************************************************************/

#ifndef DBS_EXCEPTION_H_
#define DBS_EXCEPTION_H_

#include <assert.h>

#include "whisper.h"

class DBS_SHL DBSException : public WException
{
public:
  DBSException (const D_CHAR* pMessage,
                const D_CHAR* pFile,
                D_UINT32 uLine,
                D_UINT32 extra)
    : WException (pMessage, pFile, uLine, extra)
  {
  }

  virtual ~DBSException ()
  {
  }

  enum ExceptionCodes
  {
    OPER_NOT_SUPPORTED           = 1,
    ALREADY_INITED,
    NOT_INITED,
    DATABASE_NOT_SUPP,
    INAVLID_DATABASE,
    DATABASE_NOT_FOUND,
    DATABASE_IN_USE,
    TABLE_INCONSITENCY,
    TABLE_NOT_FOUND,
    TABLE_EXISTS,
    TABLE_IN_USE,
    TABLE_INVALID,
    FIELD_NAME_INVALID,
    FIELD_NAME_DUPLICATED,
    FIELD_TYPE_INVALID,
    FIELD_NOT_FOUND,
    FIELD_NOT_INDEXED,
    FIELD_INDEXED,
    GENERAL_CONTROL_ERROR,
    INVALID_PARAMETERS,
    INVALID_UTF8_STRING,
    STRING_INDEX_TOO_BIG,
    ARRAY_INDEX_TOO_BIG,
    INVALID_ARRAY_TYPE,
    NULL_ARRAY_ELEMENT,
    INVALID_DATE,
    INVALID_DATETIME,
    INVALID_HIRESTIME,
    ROW_NOT_ALLOCATED,
  };

  virtual WException* Clone () const
    {
      return new DBSException (*this);
    }
  virtual EXPCEPTION_TYPE Type () const
    {
      return DBS_EXCEPTION;
    }

  virtual const D_CHAR* Description () const
    {
      switch (GetExtra ())
        {
        case OPER_NOT_SUPPORTED:
          return "Operation not supported.";
        case ALREADY_INITED:
          return  "Already initialised.";
        case NOT_INITED:
          return  "Not initialised.";
        case DATABASE_NOT_SUPP:
          return "Database not supplied.";
        case INAVLID_DATABASE:
          return "Invalid database.";
        case DATABASE_NOT_FOUND:
          return "Database not found.";
        case DATABASE_IN_USE:
          return "Database is in use.";
        case TABLE_INCONSITENCY:
          return "Tables is inconsistent.";
        case TABLE_NOT_FOUND:
          return "Table not found.";
        case TABLE_EXISTS:
          return "Table already exists.";
        case TABLE_IN_USE:
          return "Table is in use.";
        case TABLE_INVALID:
          return "Invalid table.";
        case FIELD_NAME_INVALID:
          return "Invalid field name.";
        case FIELD_NAME_DUPLICATED:
          return "Field name duplication.";
        case FIELD_TYPE_INVALID:
          return "Invalid field type.";
        case FIELD_NOT_FOUND:
          return "Field not found.";
        case FIELD_NOT_INDEXED:
          return "Field is not indexed.";
        case FIELD_INDEXED:
          return "Field is indexed";
        case GENERAL_CONTROL_ERROR:
          return "Internal control error detected.";
        case INVALID_PARAMETERS:
          return "Invalid parameters";
        case INVALID_UTF8_STRING:
          return "String is not UTF8 encoded.";
        case STRING_INDEX_TOO_BIG:
          return "Text string has less elements.";
        case ARRAY_INDEX_TOO_BIG:
          return "Array has less elements.";
        case INVALID_ARRAY_TYPE:
          return "Array of invalid type elements.";
        case NULL_ARRAY_ELEMENT:
          return "An array could not hold a NULL element.";
        case INVALID_DATE:
          return "Invalid representation of a DATE.";
        case INVALID_DATETIME:
          return "Invalid representation of a DATETIME.";
        case INVALID_HIRESTIME:
          return "Invalid representation of a HIRESTIME.";
        default:
          assert (false);
          return "Unknown exception code.";
        }
    }
};

#endif /* DBS_EXCEPTION_H_ */
