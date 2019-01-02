/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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

#ifndef EXT_EXCEPTION_H_
#define EXT_EXCEPTION_H_

#include "whais.h"

namespace whais
{


class ExtException : public Exception
{
public:
  ExtException(const uint32_t  code,
               const char     *file,
               uint32_t        line,
               const char     *fmtMsg = nullptr,
               ...);
  virtual ~ExtException() = default;

  enum ExceptionCodes
  {
    OPER_NOT_SUPPORTED           = 1,
    FILTER_FIELD_NOT_EXISTENT,
    FILTER_FIELD_INVALID_TYPE,
    ARRAY_TYPES_MISSMATCH,
    BAD_PARAMETERS,
    GENERAL_CONTROL_ERROR
  };

  virtual Exception* Clone() const override;
  virtual EXCEPTION_TYPE Type() const override;
  virtual const char* Description() const override;
};


} //namespace whais




#endif /* EXT_EXCEPTION_H_ */
