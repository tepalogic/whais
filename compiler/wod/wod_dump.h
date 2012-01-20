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

#include "../include/whisperc/wopcodes.h"
#include "../include/whisperc/compiledunit.h"

#include "../../utils/include/wfile.h"

void wod_dump_header (WFile & wobj, std::ostream & outs);

void wod_dump_const_area (WICompiledUnit & unit, std::ostream & outs);

void wod_dump_globals_tables (WICompiledUnit & unit, std::ostream & outs);

void
wod_dump_procs (WICompiledUnit & unit, std::ostream & outs, D_BOOL show_code);

class WDumpException:public WException
{
public:
  WDumpException (const D_CHAR *message,
      const D_CHAR *file, D_UINT32 line, D_UINT32 extra)
  : WException (message, file, line, extra) {}
  virtual ~ WDumpException () {};

  virtual WException*     Clone () { return new WDumpException (*this); }
  virtual EXPCEPTION_TYPE GetType () { return DUMP_EXCEPTION; }
};

typedef D_UINT (*FDECODE_OPCODE) (const D_UINT8 * in_args,
				  D_CHAR * operand_1, D_CHAR * operand_2);

extern FDECODE_OPCODE wod_decode_table[];
extern const D_CHAR *wod_str_table[];

#endif /* WOD_DUMP_H_ */
