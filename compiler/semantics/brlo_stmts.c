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
/*
 * brlo_stmts.c - Implements the semantics for whisper programs statements.
 */

#include "brlo_stmts.h"
#include "expression.h"
#include "statement.h"
#include "opcodes.h"
#include "wlog.h"

void
begin_if_stmt (struct ParserState *const state,
	       YYSTYPE exp, enum BRANCH_TYPE bt)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  struct UArray *const branch_stack = stmt_query_branch_stack (stmt);
  struct BranchData branch;

  if (!translate_bool_exp (state, exp))
    {
      /* errors encountered */
      assert (state->abortError == TRUE);
      return;
    }

  /* reserve space for latter */
  if ((w_opcode_encode (instrs, W_JFC) == NULL)
      || (uint32_outstream (instrs, 0) == NULL))
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  branch.type = bt;
  branch.skipPos = get_size_outstream (instrs) - sizeof (D_UINT32) - 1;
  branch.exitPos = 0;

  if (add_item (branch_stack, &branch) == NULL)
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }
}

void
begin_else_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  struct UArray *const branch_stack = stmt_query_branch_stack (stmt);
  D_UINT it_n = get_array_count (branch_stack) - 1;
  struct BranchData *it = get_item (branch_stack, it_n);
  D_INT32 offset = 0;

  /* last if or elseif statement needs to know where to exit */
  if ((w_opcode_encode (instrs, W_JMP) == NULL)
      || (uint32_outstream (instrs, 0) == NULL))
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }
  it->exitPos = get_size_outstream (instrs) - sizeof (D_UINT32) - 1;
  offset = get_size_outstream (instrs) - it->skipPos;
  memcpy (get_buffer_outstream (instrs) + it->skipPos + 1, &offset,
	  sizeof offset);
}

void
begin_elseif_stmt (struct ParserState *const state, YYSTYPE exp)
{
  begin_else_stmt (state);

  if (!state->abortError)
    {
      begin_if_stmt (state, exp, BT_ELSEIF);
    }
}

void
finalize_if_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  struct UArray *const branch_stack = stmt_query_branch_stack (stmt);
  D_UINT it_n = get_array_count (branch_stack);
  const struct BranchData *it;

  do
    {
      D_INT32 offset = get_size_outstream (instrs);
      it = get_item (branch_stack, --it_n);

      if (it->exitPos == 0)
	{
	  /* handle the lack of an else statement */
	  assert (it->skipPos > 0);
	  offset -= it->skipPos;
	  assert (offset > 0);
	  memcpy (get_buffer_outstream (instrs) + it->skipPos + 1, &offset,
		  sizeof offset);
	}
      else
	{
	  /*handle the skip of the else or elseif statements */
	  offset -= it->exitPos;
	  assert (offset > 0);
	  memcpy (get_buffer_outstream (instrs) + it->exitPos + 1, &offset,
		  sizeof offset);
	}

    }
  while (it->type == BT_ELSEIF);

  /* update the stack */
  set_array_count (branch_stack, it_n);
}

void
begin_while_stmt (struct ParserState *const state, YYSTYPE exp)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  struct UArray *const loop_stack = stmt_query_loop_stack (stmt);
  struct LoopData loop;

  loop.type = LE_WHILE_BEGIN;
  loop.startPos = get_size_outstream (instrs);

  if (!translate_bool_exp (state, exp))
    {
      /* errors encountered */
      assert (state->abortError == TRUE);
      return;
    }

  loop.endPos = get_size_outstream (instrs);
  if ((w_opcode_encode (instrs, W_JFC) == NULL)
      || (uint32_outstream (instrs, 0) == NULL))
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  if (add_item (loop_stack, &loop) == NULL)
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }
}

void
finalize_while_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  D_UINT8 *const code = get_buffer_outstream (instrs);
  struct UArray *const loop_stack = stmt_query_loop_stack (stmt);
  D_UINT it_n = get_array_count (loop_stack);
  const struct LoopData *it = NULL;
  D_INT32 end_while_loop = get_size_outstream (instrs);
  D_INT32 end_while_stmt_pos;
  D_INT32 offset = 0;

  if ((w_opcode_encode (instrs, W_JMP) == NULL)
      || (uint32_outstream (instrs, 0) == NULL))
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  end_while_stmt_pos = get_size_outstream (instrs);

  do
    {
      it = get_item (loop_stack, --it_n);
      switch (it->type)
	{
	case LE_BREAK:
	case LE_WHILE_BEGIN:
	  offset = end_while_stmt_pos - it->endPos;
	  break;
	default:
	  /* Continue statements should be already handled in case
	   * of while statements */
	  assert (0);
	}
      /* make the jump corrections */
      memcpy (code + it->endPos + 1, &offset, sizeof offset);
    }
  while ((it->type == LE_CONTINUE) || (it->type == LE_BREAK));

  assert (it->type == LE_WHILE_BEGIN);
  /* path the loop jump */
  offset = it->startPos - end_while_loop;
  memcpy (code + end_while_loop + 1, &offset, sizeof offset);

  /* update the loop stack */
  set_array_count (loop_stack, it_n);
}

void
begin_until_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  struct UArray *const loop_stack = stmt_query_loop_stack (stmt);
  struct LoopData loop;

  loop.type = LE_UNTIL_BEGIN;
  loop.startPos = get_size_outstream (instrs);
  loop.endPos = 0;

  if (add_item (loop_stack, &loop) == NULL)
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

    }
}

void
finalize_until_stmt (struct ParserState *const state, YYSTYPE exp)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  D_UINT8 *const code = get_buffer_outstream (instrs);
  struct UArray *const loop_stack = stmt_query_loop_stack (stmt);
  D_UINT it_n = get_array_count (loop_stack);
  struct LoopData *it = NULL;
  D_INT32 until_exp_pos = get_size_outstream (instrs);
  D_INT32 offset = 0;
  D_INT32 end_until_stmt_pos;

  if (!translate_bool_exp (state, exp))
    {
      /* errors encountered */
      assert (state->abortError == TRUE);
      return;
    }
  if ((w_opcode_encode (instrs, W_JTC) == NULL)
      || (uint32_outstream (instrs, 0) == NULL))
    {
      /* errors encountered */
      assert (state->abortError == TRUE);
      return;
    }
  end_until_stmt_pos = get_size_outstream (instrs);

  do
    {
      it = get_item (loop_stack, --it_n);
      switch (it->type)
	{
	case LE_CONTINUE:
	  offset = until_exp_pos - it->endPos;
	  break;
	case LE_BREAK:
	  offset = end_until_stmt_pos - it->endPos;
	  break;
	case LE_UNTIL_BEGIN:
	  /* JCT 0x01020304 should be encoded on 5 bytes */
	  it->endPos = end_until_stmt_pos - 5;
	  offset = it->startPos - it->endPos;
	  break;
	default:
	  assert (0);
	}
      memcpy (code + it->endPos + 1, &offset, sizeof offset);
    }
  while ((it->type == LE_CONTINUE) || (it->type == LE_BREAK));

  /* update the loop stack */
  set_array_count (loop_stack, it_n);
}

void
handle_break_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  struct UArray *const loop_stack = stmt_query_loop_stack (stmt);
  struct LoopData loop;

  if (get_array_count (loop_stack) == 0)
    {
      w_log_msg (state, state->bufferPos, MSG_BREAK_NOLOOP);

      return;
    }

  loop.type = LE_BREAK;
  loop.startPos = 0;
  loop.endPos = get_size_outstream (instrs);

  if (add_item (loop_stack, &loop) == NULL)
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

  if ((w_opcode_encode (instrs, W_JMP) == NULL)
      || (uint32_outstream (instrs, 0) == NULL))
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

    }
}

void
handle_continue_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  struct UArray *const loop_stack = stmt_query_loop_stack (stmt);
  D_UINT it_n = get_array_count (loop_stack);
  const struct LoopData *it = NULL;
  struct LoopData loop;

  while (it_n != 0)
    {
      it = get_item (loop_stack, --it_n);
      if ((it->type != LE_CONTINUE) && (it->type != LE_BREAK))
	{
	  break;
	}
      else
	{
	  it = NULL;
	}
    }

  if (it == NULL)
    {
      w_log_msg (state, state->bufferPos, MSG_CONTINUE_NOLOOP);

      return;
    }

  loop.type = LE_CONTINUE;
  loop.endPos = get_size_outstream (instrs);
  switch (it->type)
    {
    case LE_UNTIL_BEGIN:
      if ((w_opcode_encode (instrs, W_JMP) == NULL)
	  || (uint32_outstream (instrs, 0) == NULL))
	{
	  w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

	  return;
	}
      if (add_item (loop_stack, &loop) == NULL)
	{
	  w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

	}
      break;
    case LE_WHILE_BEGIN:
      if ((w_opcode_encode (instrs, W_JMP) == NULL)
	  || (uint32_outstream (instrs, it->startPos - loop.endPos) ==
	      NULL))
	{
	  w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

	  return;
	}
      /* this statement won't need jump correction latter */
      break;
    default:
      assert (0);
    }
}

void
begin_sync_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  const D_UINT stmts_count = stmt->spec.proc.syncTracker / 2;

  if (stmt->spec.proc.syncTracker & 1)
    {
      w_log_msg (state, state->bufferPos, MSG_SYNC_NA);

      return;
    }
  else if (stmts_count > 255)
    {
      w_log_msg (state, state->bufferPos, MSG_SYNC_MANY);

      return;
    }

  if ((w_opcode_encode (instrs, W_BSYNC) == NULL)
      || (uint8_outstream (instrs, stmts_count) == NULL))
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }
  /* update the keeper */
  stmt->spec.proc.syncTracker++;
}

void
finalize_sync_stmt (struct ParserState *const state)
{
  struct Statement *const stmt = state->pCurrentStmt;
  struct OutStream *const instrs = stmt_query_instrs (stmt);
  const D_UINT stmts_count = stmt->spec.proc.syncTracker / 2;

  assert (stmt->spec.proc.syncTracker & 1);

  if ((w_opcode_encode (instrs, W_ESYNC) == NULL)
      || (uint8_outstream (instrs, stmts_count) == NULL))
    {
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }
  /* update the keeper */
  stmt->spec.proc.syncTracker++;
}
