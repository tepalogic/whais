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

#ifndef PROCDECL_H
#define PROCDECL_H

#include "../parser/parser.h"
#include "statement.h"

/* careful whit this to be the same as in
 * whisper.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

struct Statement*
find_proc_decl (struct ParserState* pState,
                const D_CHAR*       pName,
                const D_UINT        nameLength,
                const D_BOOL        referenced);

YYSTYPE
add_prcdcl_list (YYSTYPE pList,
                 YYSTYPE pIdentifier,
                 YYSTYPE pType);

void
install_proc_args (struct ParserState* const pState, struct SemValue* pArgList);

void
install_proc_decl (struct ParserState* const pState, struct SemValue* const pIdentifier);

void
set_proc_rettype (struct ParserState* const pState, struct SemValue* const pType);

void
finish_proc_decl (struct ParserState* const pState);

#endif /* PROCDECL_H */
