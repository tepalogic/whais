/******************************************************************************
  PASTRA - A light database one file system and more.
  Copyright(C) 2008  Iulian Popa

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

#include "whais.h"


namespace whais
{


class DBS_SHL DBSException : public Exception
{
public:
  DBSException(const uint32_t  code,
               const char     *file,
               uint32_t        line,
               const char     *fmtMsg = nullptr,
               ...);
  virtual ~DBSException() = default;

  enum ExceptionCodes
  {
    OPER_NOT_SUPPORTED           = 1,
    ALREADY_INITED,
    NOT_INITED,
    INAVLID_DATABASE,
    DATABASE_IN_USE,
    DATABASE_EXISTS,
    TABLE_INCONSITENCY,
    TABLE_NOT_FUND,
    TABLE_EXISTS,
    TABLE_IN_USE,
    TABLE_INVALID,
    FIELD_NAME_INVALID,
    FIELD_TYPE_INVALID,
    FIELD_NOT_FOUND,
    FIELD_NOT_INDEXED,
    FIELD_INDEXED,
    INVALID_PARAMETERS,
    INVALID_UTF8_STRING,
    STRING_INDEX_TOO_BIG,
    ARRAY_INDEX_TOO_BIG,
    INVALID_ARRAY_TYPE,
    NULL_ARRAY_ELEMENT,
    INVALID_UNICODE_CHAR,
    INVALID_DATE,
    INVALID_DATETIME,
    INVALID_HIRESTIME,
    ROW_NOT_ALLOCATED,
    TABLE_ALREADY_LOCKED,
    TABLE_NOT_LOCKED,
    NUMERIC_FAULT,

    //The exception codes below this line cause an application stop.
    __CRITICAL_EXCEPTIONS,
    TABLE_RECOVER_FAILED,
    BAD_PARAMETERS,
    GENERAL_CONTROL_ERROR
  };

  virtual Exception* Clone() const override;
  virtual EXCEPTION_TYPE Type() const override;
  virtual const char* Description() const override;
};


} //namespace whais


#endif /* DBS_EXCEPTION_H_ */
