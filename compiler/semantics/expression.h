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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../parser/parser.h"

/* careful whit this to be the same as in
 * whisper.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

enum EXP_OPERATION
{
  OP_NULL = 0,

  OP_INC,
  OP_DEC,
  OP_NOT,

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

  OP_GROUP,			/* (...) */
  OP_INDEX,			/* [...] */
  OP_FIELD,			/* table */
  OP_ATTR,
  OP_C_ATTR,			/* { ... } */
  OP_CALL,

  OP_MAX
};

YYSTYPE
create_exp_link (struct ParserState* pState,
		 YYSTYPE             firstOp,
		 YYSTYPE             secondOp,
		 YYSTYPE             thirdOp,
		 enum EXP_OPERATION  op);

YYSTYPE
translate_exp (struct ParserState* pState, YYSTYPE exp);

YYSTYPE
translate_return_exp (struct ParserState* pState, YYSTYPE exp);

D_BOOL
translate_bool_exp (struct ParserState* pState, YYSTYPE exp);

YYSTYPE
create_arg_link (struct ParserState* pState, YYSTYPE arg, YYSTYPE next);

#endif /* EXPRESSION_H */
