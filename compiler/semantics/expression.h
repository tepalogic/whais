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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../parser/parser.h"

/* careful with this to be the same as in
 * whais.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

enum EXP_OPERATION
{
  OP_NULL = 0,

  OP_OFFSET,
  OP_NOT,

  OP_INULL,
  OP_NNULL,

  OP_POW,
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  OP_LT,
  OP_LE,
  OP_GT,
  OP_GE,
  OP_EQ,
  OP_NE,
  OP_AND,
  OP_OR,
  OP_XOR,

  OP_GROUP,     /* (...) */
  OP_INDEX,     /* [...] */
  OP_TABVAL,
  OP_FIELD,

  OP_ATTR,
  OP_ATTR_AUTO,

  OP_SADD,
  OP_SSUB,
  OP_SMUL,
  OP_SDIV,
  OP_SMOD,
  OP_SAND,
  OP_SXOR,
  OP_SOR,
  OP_CALL,

  OP_EXP_SEL,
  OP_EXP_COPY_ROW,
  OP_EXP_COPY_ROW_FREE,

  OP_CREATE_ARRAY,
  OP_EXP_CAST,
  OP_NEGATIVE,

  OP_MAX
};


struct ExpResultType
{
  const struct DeclaredVar  *extra; /* only for table types */
  uint_t                     type;
};



YYSTYPE
create_exp_link(struct ParserState* const   parser,
                YYSTYPE                     firstOp,
                YYSTYPE                     secondOp,
                YYSTYPE                     thirdOp,
                const enum EXP_OPERATION    opcode);

YYSTYPE
translate_exp(struct ParserState* const   parser,
              YYSTYPE                     exp,
              const bool_t                ingoreResult,
              const bool_t                keepResult);

YYSTYPE
translate_return_exp(struct ParserState* const   parser,
                     YYSTYPE                     exp);

bool_t
translate_bool_exp(struct ParserState* const   parser,
                   YYSTYPE                     exp);

uint16_t
translate_iterable_exp(struct ParserState* const   parser,
                       YYSTYPE                     exp);

YYSTYPE
create_arg_link(struct ParserState* const   parser,
                YYSTYPE                     arg,
                YYSTYPE                     next);

const char*
type_to_text(uint_t type);

char*
get_type_description(const struct ExpResultType* const opType);

void
free_type_description(char* description);

#endif /* EXPRESSION_H */

