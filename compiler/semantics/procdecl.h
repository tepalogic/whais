/******************************************************************************
WHAISC - A compiler for whais programs
Copyright(C) 2009  Iulian Popa

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

/* careful with this to be the same as in
 * whais.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

struct Statement*
find_proc_decl(struct ParserState  *parser,
               const char          *name,
               const uint_t         nameLength,
               const bool_t         refer);

YYSTYPE
add_proc_param_decl(YYSTYPE   paramsList,
                    YYSTYPE   id,
                    YYSTYPE   type);

void
install_proc_args(struct ParserState* const   parser,
                  struct SemValue            *paramsList);

void
install_proc_decl(struct ParserState* const   parser,
                  struct SemValue* const      identifier);

void
set_proc_rettype(struct ParserState* const   parser,
                 struct SemValue* const      type);

void
finish_proc_decl(struct ParserState* const parser);

#endif /* PROCDECL_H */


