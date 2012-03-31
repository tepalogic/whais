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
  DBSException (const D_CHAR * pMessage,
                const D_CHAR * pFile,
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
    ALLREADY_INITED                             = 2,
    NOT_INITED                                  = 3,
    DATABASE_NOT_SUPP                           = 4,
    INAVLID_DATABASE                            = 5,
    DATABASE_NOT_FOUND                          = 6,
    DATABASE_IN_USE                             = 7,
    TABLE_NOT_FOUND                             = 8,
    TABLE_EXISTS                                = 9,
    TABLE_IN_USE                                = 10,
    TABLE_INVALID                               = 11,
    FIELD_NAME_INVALID                          = 12,
    FIELD_NAME_DUPLICATED                       = 13,
    FIELD_TYPE_INVALID                          = 14,
    FIELD_NOT_FOUND                             = 15,
    FIELD_NOT_INDEXED                           = 16,
    FIELD_INDEXED                               = 17,
    GENERAL_CONTROL_ERROR                       = 18,
    INVALID_PARAMETERS                          = 19,
    INVALID_UTF8_STRING                         = 20,
    STRING_INDEX_TOO_BIG                        = 21,
    ARRAY_INDEX_TOO_BIG                         = 22,
    INVALID_ARRAY_TYPE                          = 23,
    NULL_ARRAY_ELEMENT                          = 24,
    INVALID_DATE                                = 25,
    INVALID_DATETIME                            = 26,
    INVALID_HIRESTIME                           = 27
  };

  virtual WException*     Clone () { return new DBSException (*this); }
  virtual EXPCEPTION_TYPE GetType () { return DBS_EXCEPTION; }
};

#endif /* DBS_EXCEPTION_H_ */
