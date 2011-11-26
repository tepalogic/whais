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

#ifndef VARDECL_H
#define VARDECL_H

#include "../parser/parser.h"

/* careful whit this to be the same as in
 * whisper.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

/* A mark to identify global declared variables*/
#define GLOBAL_DECLARED 0x80000000
/* A mark to identify external declarations */
#define EXTERN_DECLARED 0x40000000

struct DeclaredVar
{
  const D_CHAR *label;		/* not necessarily null terminated */
  struct DeclaredVar *extra;	/* only for rows, tables and records it's
				   list to/next field entries */
  D_UINT32 type_spec_pos;
  D_UINT32 var_id;

  D_UINT32 offset;		/* offset position */

  D_UINT16 l_label;		/* label's length */
  D_UINT16 type;		/* type of this variable */
};

YYSTYPE
add_idlist (YYSTYPE list, YYSTYPE id);

YYSTYPE
create_type_spec (struct ParserState *state, D_UINT16 type);

struct DeclaredVar *
install_declaration (struct ParserState *state,
                     YYSTYPE sem_var,
                     YYSTYPE sem_type, D_BOOL paramter, D_BOOL unique);

YYSTYPE
install_list_declrs (struct ParserState *state,
		     YYSTYPE sem_vars, YYSTYPE sem_type);

YYSTYPE
install_field_declaration (struct ParserState *state,
			   YYSTYPE sem_var,
			   YYSTYPE sem_type, struct DeclaredVar *const extra);
D_BOOL
process_container_decls (struct ParserState *state,
                         struct DeclaredVar *var, void *extra);

#endif /* VARDECL_H */
