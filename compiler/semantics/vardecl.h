/******************************************************************************
WHAISC - A compiler for Whais programs
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

#ifndef VARDECL_H
#define VARDECL_H

#include "whais.h"
#include "../parser/parser.h"

/* careful whit this to be the same as in
 * whais.y */
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
#define IS_REFERRED(x)   (((x) & NOTREF_DECL) == 0)

#define MARK_AS_GLOBAL(x)           ((x) |= GLOBAL_DECL)
#define MARK_AS_EXTERNAL(x)         ((x) |= EXTERN_DECL)
#define MARK_AS_NOT_REFERENCED(x)   ((x) |= NOTREF_DECL)
#define MARK_AS_REFERENCED(x)       ((x) &= ~NOTREF_DECL)


struct DeclaredVar
{
  const char*          label;
  struct DeclaredVar  *extra;        /* Field entries list for tables. */
  uint32_t             typeSpecOff;  /* Offset in the type spec pool. */
  uint32_t             varId;        /* Internal id to refer it. */
  uint32_t             offset;       /* Code offset for argumenting errors. */
  uint16_t             labelLength;  /* Label's length */
  uint16_t             type;         /* Type of this variable */
  uint32_t             declarationPos;
  uint32_t             definitionPos;
};

YYSTYPE
add_id_to_list(YYSTYPE   list,
               YYSTYPE   id);

YYSTYPE
create_type_spec(struct ParserState* const   parser,
                 const uint16_t              type);

struct DeclaredVar*
add_declaration(struct ParserState* const   parser,
                YYSTYPE                     var,
                YYSTYPE                     type,
                const bool_t                paramter,
                const bool_t                unique);

YYSTYPE
add_list_declaration(struct ParserState* const parser,
                     YYSTYPE                   vars,
                     YYSTYPE                   type);

YYSTYPE
add_auto_declaration(struct ParserState* const parser,
                     YYSTYPE                   vars,
                     YYSTYPE                   type);

YYSTYPE
add_field_declaration(struct ParserState* const   parser,
                      YYSTYPE                     var,
                      YYSTYPE                     type,
                      struct DeclaredVar* const   extra);

bool_t
compare_extern_table_types(const struct DeclaredVar* const   firstTable,
                           const struct DeclaredVar* const   secondFields);

#endif /* VARDECL_H */
