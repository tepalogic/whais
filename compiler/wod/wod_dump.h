/******************************************************************************
WHISPERC - A compiler for whisper programs
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

#include "whisper.h"

#include "compiler/wopcodes.h"
#include "compiler/compiledunit.h"

#include "utils/wfile.h"

using namespace whisper;

void
wod_dump_header (WFile& rInObj, std::ostream& rOutStream);

void
wod_dump_const_area (WICompiledUnit& rUnit, std::ostream& rOutStream);

void
wod_dump_globals_tables (WICompiledUnit& rUnit, std::ostream& rOutStream);

void
wod_dump_procs (WICompiledUnit& rUnit, std::ostream& rOutStream, bool_t showCode);

class WDumpException : public Exception
{
public:
  WDumpException (const char* pMessage,
                  const char* pFile,
                  uint32_t      line,
                  uint32_t      extra)
    : Exception (pMessage, pFile, line, extra)
  {
  }
  virtual ~ WDumpException ()
  {
  };

  virtual Exception*     Clone () const { return new WDumpException (*this); }
  virtual EXPCEPTION_TYPE Type () const { return DUMP_EXCEPTION; }
  virtual const char*   Description () const
  {
    return "General exception by object dumper.";
  }
};

typedef uint_t (*FDECODE_OPCODE) (const uint8_t* pInArgs,
                                  char*        pOp1,
                                  char*        pOp2);

extern FDECODE_OPCODE wod_decode_table[];
extern const char*  wod_str_table[];

#define MAX_OP_STRING 128

#endif /* WOD_DUMP_H_ */
