/******************************************************************************
WHAISC - A compiler for whais programs
Copyright (C) 2009  Iulian Popa

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

#ifndef WOD_DUMP_H_
#define WOD_DUMP_H_

#include <iostream>

#include "whais.h"

#include "compiler/wopcodes.h"
#include "compiler/compiledunit.h"

#include "utils/wfile.h"

namespace whais {
namespace wod {


#define MAX_OP_STRING 128


class DumpException : public Exception
{
public:
  DumpException( const uint32_t  code,
                 const char*     file,
                 uint32_t        line,
                 const char*     fmtMsg = NULL,
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

  virtual ~DumpException()
  {
  };

  virtual Exception* Clone() const { return new DumpException( *this); }

  virtual EXCEPTION_TYPE Type() const { return DUMP_EXCEPTION; }

  virtual const char* Description() const
  {
    return "General exception by object dumper.";
  }
};


typedef uint_t( *FDECODE_OPCODE) (const uint8_t*      args,
                                  char* const         op1,
                                  char* const         op2);


extern FDECODE_OPCODE wod_decode_table[];
extern const char*    wod_str_table[];




void
wod_dump_header( File& obj, std::ostream& output);


void
wod_dump_const_area( WIFunctionalUnit& unit, std::ostream& output);


void
wod_dump_globals_tables( WIFunctionalUnit& unit, std::ostream& output);


void
wod_dump_procs( WIFunctionalUnit&     unit,
                std::ostream&         output,
                bool_t                showCode);



} //namespace wod
} //namespace whais

#endif /* WOD_DUMP_H_ */

