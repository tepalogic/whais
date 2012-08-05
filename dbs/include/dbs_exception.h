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

#ifndef DBS_EXCEPTION_H_
#define DBS_EXCEPTION_H_

#include "whisper.h"

class DBSException:public WException
{
public:
  DBSException (const D_CHAR* pMessage,
                const D_CHAR* pFile,
                D_UINT32 uLine,
                D_UINT32 extra) :
    WException (pMessage, pFile, uLine, extra)
  {
  }

  virtual ~ DBSException ()
  {
  }

  enum ExceptionCodes
  {
    OPER_NOT_SUPPORTED                          = 1,
    ALLREADY_INITED,
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

  virtual WException*     Clone () { return new DBSException (*this); }
  virtual EXPCEPTION_TYPE Type () { return DBS_EXCEPTION; }
};

#endif /* DBS_EXCEPTION_H_ */
