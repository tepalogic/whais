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

#include "whisper.h"

#include "../parser/parser.h"

/* careful whit this to be the same as in
 * whisper.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

/* A mark to identify global declared variables*/
#define GLOBAL_DECL    0x80000000
/* A mark to identify external declarations */
#define EXTERN_DECL    0x40000000
/* A mark to identify the unused declarations */
#define NOTREF_DECL    0x20000000

#define RETRIVE_ID(x) ((x) & ~(GLOBAL_DECL | EXTERN_DECL | NOTREF_DECL))

#define IS_GLOBAL(x)     (((x) & GLOBAL_DECL) != 0)
#define IS_EXTERNAL(x)   (((x) & EXTERN_DECL) != 0)
#define IS_REFERENCED(x) (((x) & NOTREF_DECL) == 0)

#define MARK_AS_GLOBAL(x)           ((x) |= GLOBAL_DECL)
#define MARK_AS_EXTERNAL(x)         ((x) |= EXTERN_DECL)
#define MARK_AS_NOT_REFERENCED(x)   ((x) |= NOTREF_DECL)
#define MARK_AS_REFERENCED(x)       ((x) &= ~NOTREF_DECL)


struct DeclaredVar
{
  const D_CHAR*       label;	   /* not necessarily  null terminated */
  struct DeclaredVar* extra;	   /* only for tables, list to/next field entries */
  D_UINT32            typeSpecOff;
  D_UINT32            varId;
  D_UINT32            offset;      /* offset position */
  D_UINT16            labelLength; /* label's length */
  D_UINT16            type;	   /* type of this variable */
};

YYSTYPE
add_id_to_list (YYSTYPE list, YYSTYPE id);

YYSTYPE
create_type_spec (struct ParserState *pState,
                  D_UINT16            type);

struct DeclaredVar *
install_declaration (struct ParserState* pState,
                     YYSTYPE             pVar,
                     YYSTYPE             pType,
                     D_BOOL              paramter,
                     D_BOOL              unique);

YYSTYPE
install_list_declrs (struct ParserState* pState,
		     YYSTYPE             pVars,
		     YYSTYPE             pType);

YYSTYPE
install_field_declaration (struct ParserState*       pState,
			   YYSTYPE                   pVar,
			   YYSTYPE                   pType,
			   struct DeclaredVar* const pExtra);
D_BOOL
process_table_decls (struct ParserState*     pState,
                         struct DeclaredVar* pVar,
                         void*               extra);

#endif /* VARDECL_H */
