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

#include "ext_exception.h"

#include <cassert>

using namespace whais;

ExtException::ExtException(const uint32_t   code,
                           const char      *file,
                           uint32_t         line,
                           const char      *fmtMsg,
                            ...)
  : Exception(code, file, line)
{
  if (fmtMsg != nullptr)
  {
    va_list vl;

    va_start(vl, fmtMsg);
    this->Message(fmtMsg, vl);
    va_end(vl);
  }
}

Exception*
ExtException::Clone() const
{
  return new ExtException(*this);
}

EXCEPTION_TYPE
ExtException::Type() const
{
  return EXT_EXCEPTION;
}

const char*
ExtException::Description() const
{

  switch(Code())
  {
  case OPER_NOT_SUPPORTED:
    return "A requested operation is not supported.";

  case FILTER_FIELD_NOT_EXISTENT:
    return "Cannot filter thw rows of a table based on inexistent field.";

  case FILTER_FIELD_INVALID_TYPE:
    return "Filter is of invalid type for requested operation.";

  case BAD_PARAMETERS:
    return "Program execution could not continue due to invalid parameters.";

  case GENERAL_CONTROL_ERROR:
    return "GENERAL FAILURE: Program execution must be stopped due to a DBS internal error.";
  }

  assert(false);
  return "An unknown DBS framework exception was thrown";
};
