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

#ifndef BRLO_STMTS_H
#define BRLO_STMTS_H

#include "whisper.h"
#include "../parser/parser.h"

/* careful whit this to be the same as in
 * whisper.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

enum BRANCH_TYPE
{
  BT_UNKNOWN,
  BT_IF,
  BT_ELSEIF
};

struct Branch
{
  enum BRANCH_TYPE type;
  int32_t          startPos;
  int32_t          elsePos;
};

void
begin_if_stmt (struct ParserState* const parser,
               YYSTYPE                   expression,
               enum BRANCH_TYPE          branchType);

void
begin_else_stmt (struct ParserState* const parser);

void
begin_elseif_stmt (struct ParserState* const parser, YYSTYPE exp);

void
finalize_if_stmt (struct ParserState* const parser);



enum LOOP_ELEMENT_TYPE
{
  LE_UNKNOWN,
  LE_WHILE_BEGIN,
  LE_UNTIL_BEGIN,
  LE_FOREACH_BEGIN,
  LE_BREAK,
  LE_CONTINUE
};

struct Loop
{
  enum LOOP_ELEMENT_TYPE type;
  uint32_t               startPos;
  int32_t                endPos;
};

void
begin_while_stmt (struct ParserState* const parser, YYSTYPE exp);

void
finalize_while_stmt (struct ParserState* const parser);

void
begin_until_stmt (struct ParserState* const parser);

void
finalize_until_stmt (struct ParserState* const parser, YYSTYPE exp);

void
handle_break_stmt (struct ParserState* const parser);

void
handle_continue_stmt (struct ParserState* const parser);


void
begin_sync_stmt (struct ParserState* const parser);

void
finalize_sync_stmt (struct ParserState* const parser);

#endif /* BRLO_STMTS_H */

