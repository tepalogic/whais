/******************************************************************************
 WHAISC - A compiler for whais programs
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
/*
 * brlo_stmts.c - Implements the semantics for whais programs statements.
 */

#include "brlo_stmts.h"
#include "expression.h"
#include "statement.h"
#include "opcodes.h"
#include "wlog.h"

void
begin_if_stmt (struct ParserState* const parser,
               YYSTYPE                   expression,
               enum BRANCH_TYPE          branchType)
{
  struct Statement* const     stmt = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct WArray* const branchStack = stmt_query_branch_stack (stmt);
  struct Branch        branch;

  if ( ! translate_bool_exp (parser, expression))
    {
      assert (parser->abortError == TRUE);

      return;
    }

  /* Reserve space for latter when we have to put the correct offset. */
  if ((encode_opcode( code, W_JFC) == NULL)
      || (wh_ostream_wint32 (code, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  branch.type     = branchType;
  branch.startPos = wh_ostream_size (code) - sizeof (uint32_t) - 1;
  branch.elsePos  = 0;

  if (wh_array_add( branchStack, &branch) == NULL)
    log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
}


void
begin_else_stmt (struct ParserState* const parser)
{
  struct Statement* const     stmt = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct WArray* const branchStack = stmt_query_branch_stack (stmt);
  uint_t               branchId    = wh_array_count (branchStack) - 1;
  struct Branch*       branchIt    = wh_array_get (branchStack, branchId);
  int32_t              jumpOffset  = 0;

  /* Last if or elseif statement needs to know where to exit */
  if ((encode_opcode( code, W_JMP) == NULL)
      || (wh_ostream_wint32 (code, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  branchIt->elsePos = wh_ostream_size (code) - sizeof (uint32_t) - 1;
  jumpOffset        = wh_ostream_size (code) - branchIt->startPos;

  store_le_int32 (jumpOffset, wh_ostream_data (code) + branchIt->startPos + 1);
}


void
begin_elseif_stmt (struct ParserState *const state, YYSTYPE exp)
{
  begin_else_stmt (state);

  if ( ! state->abortError)
    begin_if_stmt (state, exp, BT_ELSEIF);
}


void
finalize_if_stmt (struct ParserState* const parser)
{
  struct Statement* const    stmt  = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct WArray* const  branchStack = stmt_query_branch_stack (stmt);
  uint_t                branchId    = wh_array_count (branchStack);
  const struct Branch*  branchIt    = NULL;

  do
    {
      int32_t jumpOffset = wh_ostream_size (code);

      branchIt = wh_array_get (branchStack, --branchId);

      if (branchIt->elsePos == 0)
        {
          /* Handle the lack of an else statement. */
          assert (branchIt->startPos > 0);

          jumpOffset -= branchIt->startPos;

          assert (jumpOffset > 0);

          store_le_int32 (jumpOffset,
                          wh_ostream_data (code) + branchIt->startPos + 1);
        }
      else
        {
          /* Skip the else or elseif statements. */
          jumpOffset -= branchIt->elsePos;

          assert (jumpOffset > 0);

          store_le_int32 (jumpOffset,
                          wh_ostream_data (code) + branchIt->elsePos + 1);
        }

    }
  while (branchIt->type == BT_ELSEIF);

  wh_array_resize (branchStack, branchId);
}


void
begin_for_stmt (struct ParserState* const parser,
                YYSTYPE                   exp1,
                YYSTYPE                   exp2,
                YYSTYPE                   exp3)
{
  translate_exp (parser, exp1);
  if (parser->abortError)
    return ;

  struct Statement* const     stmt = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct Loop loop;
  loop.type = LE_FOR_BEGIN;

  const int32_t boolExpMark = wh_ostream_size (code);
  if ( ! translate_bool_exp (parser, exp2))
    {
      assert (parser->abortError == TRUE);

      return;
    }

  loop.endPos = wh_ostream_size (code);
  if ((encode_opcode( code, W_JFC) == NULL)
      || (wh_ostream_wint32 (code, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  const int32_t stepExpMark = wh_ostream_size (code);
  if ((encode_opcode( code, W_JMP) == NULL)
      || (wh_ostream_wint32 (code, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  loop.startPos = wh_ostream_size (code);
  translate_exp (parser, exp3);
  if (parser->abortError)
    return ;

  const int32_t offset = wh_ostream_size (code);
  if ((encode_opcode( code, W_JMP) == NULL)
      || (wh_ostream_wint32 (code, boolExpMark - offset) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  store_le_int32 (wh_ostream_size( code) - stepExpMark,
                  wh_ostream_data (code) + stepExpMark + 1);

  struct WArray* const loopsStack = stmt_query_loop_stack (stmt);
  if (wh_array_add( loopsStack, &loop) == NULL)
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }
}

void
finalize_for_stmt (struct ParserState* const parser)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const stream  = stmt_query_instrs (stmt);
  uint8_t* const              code    = wh_ostream_data (stream);

  struct WArray* const loopsStack = stmt_query_loop_stack (stmt);
  uint_t               loopId     = wh_array_count (loopsStack);
  const struct Loop*   loopIt     = NULL;

  int32_t  endForLoopPos = wh_ostream_size (stream);
  int32_t  endForStmtPos = 0;
  int32_t  offset        = 0;

  if ((encode_opcode( stream, W_JMP) == NULL)
      || (wh_ostream_wint32 (stream, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  endForStmtPos = wh_ostream_size (stream);
  do
    {
      loopIt = wh_array_get (loopsStack, --loopId);
      switch (loopIt->type)
        {
        case LE_BREAK:
        case LE_FOR_BEGIN:
          offset = endForStmtPos - loopIt->endPos;
          break;

        default:
          /* Continue statements should be already handled in case
           * of FOR statements */
          assert (FALSE);
        }
      /* Make the jump corrections. */
      store_le_int32 (offset, code + loopIt->endPos + 1);
    }
  while ((loopIt->type == LE_CONTINUE) || (loopIt->type == LE_BREAK));

  assert (loopIt->type == LE_FOR_BEGIN);

  offset = loopIt->startPos - endForLoopPos;
  store_le_int32 (offset, code + endForLoopPos + 1);

  wh_array_resize (loopsStack, loopId);
}

void
begin_while_stmt (struct ParserState* const parser, YYSTYPE exp)
{
  struct Statement* const     stmt = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct WArray* const loopsStack  = stmt_query_loop_stack (stmt);
  struct Loop          loop;

  loop.type     = LE_WHILE_BEGIN;
  loop.startPos = wh_ostream_size (code);

  if ( ! translate_bool_exp (parser, exp))
    {
      assert (parser->abortError == TRUE);

      return;
    }

  loop.endPos = wh_ostream_size (code);
  if ((encode_opcode( code, W_JFC) == NULL)
      || (wh_ostream_wint32 (code, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  if (wh_array_add( loopsStack, &loop) == NULL)
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }
}

void
finalize_while_stmt (struct ParserState* const parser)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const stream  = stmt_query_instrs (stmt);
  uint8_t* const              code    = wh_ostream_data (stream);

  struct WArray* const loopsStack = stmt_query_loop_stack (stmt);
  uint_t               loopId     = wh_array_count (loopsStack);
  const struct Loop*   loopIt     = NULL;

  int32_t  endWhileLoopPos = wh_ostream_size (stream);
  int32_t  endWhileStmtPos = 0;
  int32_t  offset          = 0;

  if ((encode_opcode( stream, W_JMP) == NULL)
      || (wh_ostream_wint32 (stream, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  endWhileStmtPos = wh_ostream_size (stream);

  do
    {
      loopIt = wh_array_get (loopsStack, --loopId);
      switch (loopIt->type)
        {
        case LE_BREAK:
        case LE_WHILE_BEGIN:
          offset = endWhileStmtPos - loopIt->endPos;
          break;

        default:
          /* Continue statements should be already handled in case
           * of while statements */
          assert (FALSE);
        }
      /* Make the jump corrections. */
      store_le_int32 (offset, code + loopIt->endPos + 1);
    }
  while ((loopIt->type == LE_CONTINUE) || (loopIt->type == LE_BREAK));

  assert (loopIt->type == LE_WHILE_BEGIN);

  offset = loopIt->startPos - endWhileLoopPos;
  store_le_int32 (offset, code + endWhileLoopPos + 1);

  wh_array_resize (loopsStack, loopId);
}


void
begin_until_stmt (struct ParserState* const parser)
{
  struct Statement* const     stmt       = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct WArray* const loopsStack  = stmt_query_loop_stack (stmt);
  struct Loop          loop;

  loop.type     = LE_UNTIL_BEGIN;
  loop.startPos = wh_ostream_size (code);
  loop.endPos   = 0;

  if (wh_array_add( loopsStack, &loop) == NULL)
    log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
}


void
finalize_until_stmt (struct ParserState* const parser, YYSTYPE exp)
{
  struct Statement* const     stmt   = parser->pCurrentStmt;
  struct WOutputStream* const stream = stmt_query_instrs (stmt);
  uint8_t *const              code   = wh_ostream_data (stream);

  struct WArray* const loopsStack = stmt_query_loop_stack (stmt);
  uint_t               loopId     = wh_array_count (loopsStack);
  struct Loop*         loopIt     = NULL;

  int32_t untilExpPos     = wh_ostream_size (stream);
  int32_t endUntilStmtPos = 0;
  int32_t offset          = 0;

  if ( ! translate_bool_exp (parser, exp))
    {
      assert (parser->abortError == TRUE);

      return;
    }

  if ((encode_opcode( stream, W_JTC) == NULL)
      || (wh_ostream_wint32 (stream, 0) == NULL))
    {
      assert (parser->abortError == TRUE);

      return;
    }

  endUntilStmtPos = wh_ostream_size (stream);

  do
    {
      loopIt = wh_array_get (loopsStack, --loopId);
      switch (loopIt->type)
        {
        case LE_CONTINUE:
          offset = untilExpPos - loopIt->endPos;
          break;

        case LE_BREAK:
          offset = endUntilStmtPos - loopIt->endPos;
          break;

        case LE_UNTIL_BEGIN:
          /* JCT 0x01020304 should be encoded with 5 bytes */
          loopIt->endPos = endUntilStmtPos - 5;
          offset         = loopIt->startPos - loopIt->endPos;
          break;

        default:
          assert (0);
        }
      store_le_int32 (offset, code + loopIt->endPos + 1);
    }
  while ((loopIt->type == LE_CONTINUE) || (loopIt->type == LE_BREAK));

  wh_array_resize (loopsStack, loopId);
}


void
handle_break_stmt (struct ParserState* const parser)
{
  struct Statement* const     stmt  = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct WArray* const loopsStack = stmt_query_loop_stack (stmt);
  struct Loop          loop;

  if (wh_array_count( loopsStack) == 0)
    {
      log_message (parser, parser->bufferPos, MSG_BREAK_NOLOOP);

      return;
    }

  loop.type     = LE_BREAK;
  loop.startPos = 0;
  loop.endPos   = wh_ostream_size (code);

  if (wh_array_add( loopsStack, &loop) == NULL)
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  if ((encode_opcode( code, W_JMP) == NULL) ||
      (wh_ostream_wint32 (code, 0) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    }
}


void
handle_continue_stmt (struct ParserState* const parser)
{
  struct Statement* const     stmt = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs (stmt);

  struct WArray* const loopsStack = stmt_query_loop_stack (stmt);
  uint_t               loopId     = wh_array_count (loopsStack);
  const struct Loop*   loopIt     = NULL;
  struct Loop          loop;

  while (loopId != 0)
    {
      loopIt = wh_array_get (loopsStack, --loopId);
      if ((loopIt->type != LE_CONTINUE) && (loopIt->type != LE_BREAK))
        break;

      else
        loopIt = NULL;
    }

  if (loopIt == NULL)
    {
      log_message (parser, parser->bufferPos, MSG_CONTINUE_NOLOOP);

      return;
    }

  loop.type   = LE_CONTINUE;
  loop.endPos = wh_ostream_size (code);
  switch (loopIt->type)
    {
    case LE_UNTIL_BEGIN:
      if ((encode_opcode( code, W_JMP) == NULL) ||
          (wh_ostream_wint32 (code, 0) == NULL))
        {
          log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

          return;
        }

      if (wh_array_add( loopsStack, &loop) == NULL)
        log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      break;

    case LE_WHILE_BEGIN:
    case LE_FOR_BEGIN:
      if ((encode_opcode( code, W_JMP) == NULL) ||
          (wh_ostream_wint32 (code, loopIt->startPos - loop.endPos) == NULL))
        {
          log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

          return;
        }
      /* This statement doesn't need jump correction. */
      break;

    default:
      assert (FALSE);
    }
}


void
begin_sync_stmt (struct ParserState* const parser)
{
  struct Statement *const     stmt       = parser->pCurrentStmt;
  struct WOutputStream *const code       = stmt_query_instrs (stmt);
  const uint_t                stmtsCount = stmt->spec.proc.syncTracker / 2;

  if (stmt->spec.proc.syncTracker & 1)
    {
      log_message (parser, parser->bufferPos, MSG_SYNC_NA);

      return;
    }
  else if (stmtsCount > 255)
    {
      log_message (parser, parser->bufferPos, MSG_SYNC_MANY);

      return;
    }

  if ((encode_opcode( code, W_BSYNC) == NULL)
      || (wh_ostream_wint8 (code, stmtsCount) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  stmt->spec.proc.syncTracker++;
}


void
finalize_sync_stmt (struct ParserState* const parser)
{
  struct Statement *const     stmt       = parser->pCurrentStmt;
  struct WOutputStream *const code       = stmt_query_instrs (stmt);
  const uint_t                stmtsCount = stmt->spec.proc.syncTracker / 2;

  assert (stmt->spec.proc.syncTracker & 1);

  if ((encode_opcode( code, W_ESYNC) == NULL)
      || (wh_ostream_wint8 (code, stmtsCount) == NULL))
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  stmt->spec.proc.syncTracker++;
}

