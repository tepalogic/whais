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
/*
 * brlo_stmts.c - Implements the semantics for whais programs statements.
 */

#include "brlo_stmts.h"
#include "expression.h"
#include "statement.h"
#include "opcodes.h"
#include "wlog.h"


void
check_for_dead_statement(struct ParserState* const parser)
{
  struct Statement* const stmt = parser->pCurrentStmt;

  struct WArray* const branchStack = stmt_query_branch_stack(stmt);
  const uint_t stackSize           = wh_array_count(branchStack);

  assert(parser->pCurrentStmt->type == STMT_PROC);

  if (stackSize > 0)
  {
    struct Branch* const branchIt = wh_array_get(branchStack, stackSize - 1);

    if ( ! branchIt->returnDetected || branchIt->deadCodeWarned)
        return ;

    branchIt->deadCodeWarned = TRUE;
  }
  else if (parser->pCurrentStmt->spec.proc.returnDetected)
  {
    if (parser->pCurrentStmt->spec.proc.deadCodeWarned)
      return ;

    parser->pCurrentStmt->spec.proc.deadCodeWarned = TRUE;
  }
  else
    return ;

  log_message(parser, parser->bufferPos, MSG_DEAD_STMT);
}

static uint_t
get_iterator_local_index(struct ParserState* const parser,
                         const char* const         itName,
                         const uint_t              itNameLength)
{
  struct Statement* const stmt = parser->pCurrentStmt;
  struct Statement* const glb  = stmt->parent;

  uint_t i, v;

  assert(stmt->type == STMT_PROC);
  assert(glb->type == STMT_GLOBAL);

  for (v = 0; v < wh_array_count(&glb->decls); ++v)
  {
    struct DeclaredVar* const var = (struct DeclaredVar*)wh_array_get(&glb->decls, v);

    if (var->labelLength == itNameLength
        && strncmp(var->label, itName, itNameLength) == 0
        && ! IS_TABLE_FIELD(var->type))
    {
      char tname[128];

      wh_copy_first(tname, itName, sizeof tname, itNameLength);

      log_message(parser, parser->bufferPos, MSG_IT_VARIABLE, tname);
      log_message(parser, var->declarationPos, MSG_DECL_PREV);
    }
  }

  for (v = 0; v < wh_array_count(&stmt->spec.proc.paramsList); ++v)
  {
    struct DeclaredVar* const var = (struct DeclaredVar*)
                                    wh_array_get(&stmt->spec.proc.paramsList, v);
    if (var->labelLength == itNameLength
        && strncmp(var->label, itName, itNameLength) == 0
        && ! IS_TABLE_FIELD(var->type))
    {
      char tname[128];

      wh_copy_first(tname, itName, sizeof tname, itNameLength);

      log_message(parser, parser->bufferPos, MSG_IT_VARIABLE, tname);
      log_message(parser, var->declarationPos, MSG_DECL_PREV);
    }
  }

  for (v = 0; v < wh_array_count(&stmt->decls); ++v)
  {
    struct DeclaredVar* const var = (struct DeclaredVar*)wh_array_get(&stmt->decls, v);

    if (var->labelLength == itNameLength
        && strncmp(var->label, itName, itNameLength) == 0
        && ! IS_TABLE_FIELD(var->type))
    {
      char tname[128];

      wh_copy_first(tname, itName, sizeof tname, itNameLength);

      log_message(parser, parser->bufferPos, MSG_IT_VARIABLE, tname);
      log_message(parser, var->declarationPos, MSG_DECL_PREV);
    }
  }

  struct WArray* const iterators = stmt_query_loop_iterators_stack(stmt);
  for (i = 0; i < wh_array_count(iterators); ++i)
  {
    struct LoopIterator* const it = (struct LoopIterator*)wh_array_get(iterators, i);

    if (it->nameLen == itNameLength
        && strncmp(it->name, itName, itNameLength) == 0)
    {
      char tname[128];

      wh_copy_first(tname, itName, sizeof tname, itNameLength);

      log_message(parser, parser->bufferPos, MSG_IT_ALREADY, tname);
      return 0;
    }
  }
  return wh_array_count(iterators);
}

void
begin_if_stmt(struct ParserState* const   parser,
              YYSTYPE                     expression,
              const enum BRANCH_TYPE      branchType)
{
  struct Statement* const     stmt        = parser->pCurrentStmt;
  struct WOutputStream* const code        = stmt_query_instrs(stmt);
  struct WArray* const        branchStack = stmt_query_branch_stack(stmt);

  struct Branch branch = {0, };

  if ( ! translate_bool_exp(parser, expression))
  {
    assert(parser->abortError == TRUE);
    return;
  }

  /* Reserve space for latter when we have to put the correct offset. */
  if (encode_opcode( code, W_JFC) == NULL
      || wh_ostream_wint32(code, 0) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  branch.type     = branchType;
  branch.startPos = wh_ostream_size(code) - sizeof(uint32_t) - 1;

  if (wh_array_add( branchStack, &branch) == NULL)
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
}

void
begin_else_stmt(struct ParserState* const parser)
{
  struct Statement* const     stmt        = parser->pCurrentStmt;
  struct WOutputStream* const code        = stmt_query_instrs(stmt);
  struct WArray* const        branchStack = stmt_query_branch_stack(stmt);
  const uint_t                branchId    = wh_array_count(branchStack) - 1;
  struct Branch* const        branchIt    = wh_array_get(branchStack, branchId);

  int32_t jumpOffset  = 0;

  /* Last if or elseif statement needs to know where to exit */
  if (encode_opcode( code, W_JMP) == NULL
      || wh_ostream_wint32(code, 0) == NULL)
    {
      log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return;
    }

  jumpOffset = wh_ostream_size(code) - branchIt->startPos;
  branchIt->elsePos            = wh_ostream_size(code) - sizeof(uint32_t) - 1;
  branchIt->prevReturnDetected = branchIt->returnDetected;
  branchIt->returnDetected     = FALSE;

  store_le_int32(jumpOffset, wh_ostream_data(code) + branchIt->startPos + 1);
}

void
begin_elseif_stmt(struct ParserState *const   parser,
                  YYSTYPE                     exp)
{
  struct Statement* const stmt        = parser->pCurrentStmt;
  struct WArray* const    branchStack = stmt_query_branch_stack(stmt);
  const uint_t            branchId    = wh_array_count(branchStack) - 1;
  struct Branch* const    branchIt    = wh_array_get(branchStack, branchId);
  const bool_t            retDetected = branchIt->returnDetected;

  begin_else_stmt(parser);
  branchIt->returnDetected = retDetected;

  if ( ! parser->abortError)
    begin_if_stmt(parser, exp, BT_ELSEIF);
}

void
finalize_if_stmt(struct ParserState* const parser)
{
  struct Statement* const     stmt = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs(stmt);
  struct WArray* const        branchStack = stmt_query_branch_stack(stmt);

  struct Branch  *branchIt          = NULL;
  uint_t          branchId          = wh_array_count(branchStack);
  bool_t          allBranchesReturn = TRUE;

  do
  {
    int32_t jumpOffset = wh_ostream_size(code);

    branchIt = wh_array_get(branchStack, --branchId);

    if (branchIt->elsePos == 0)
    {
      /* Handle the lack of an else statement. */
      assert(branchIt->startPos > 0);

      jumpOffset -= branchIt->startPos;

      assert(jumpOffset > 0);

      store_le_int32(jumpOffset, wh_ostream_data(code) + branchIt->startPos + 1);

      allBranchesReturn = FALSE;
    }
    else
    {
      /* Skip the else or elseif statements. */
      jumpOffset -= branchIt->elsePos;

      assert(jumpOffset > 0);

      store_le_int32(jumpOffset, wh_ostream_data(code) + branchIt->elsePos + 1);
    }

    allBranchesReturn &= branchIt->returnDetected;
    allBranchesReturn &= branchIt->prevReturnDetected;
  }
  while (branchIt->type == BT_ELSEIF);

  wh_array_resize(branchStack, branchId);

  if (branchId == 0)
    stmt->spec.proc.returnDetected |= allBranchesReturn;

  else
  {
    branchIt = wh_array_get(branchStack, branchId - 1);
    branchIt->returnDetected |= allBranchesReturn;
  }
}


void
begin_for_stmt(struct ParserState* const   parser,
               YYSTYPE                     exp1,
               YYSTYPE                     exp2,
               YYSTYPE                     exp3)
{
  struct Statement* const     stmt = parser->pCurrentStmt;
  struct WOutputStream* const code = stmt_query_instrs(stmt);

  struct Loop loop;

  loop.type = LE_FOR_BEGIN;

  translate_exp(parser, exp1, TRUE, FALSE);
  if (parser->abortError)
    return ;

  const int32_t stepExpMark = wh_ostream_size(code);
  if (encode_opcode(code, W_JMP) == NULL
      || wh_ostream_wint32(code, 0) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  loop.continueMark = wh_ostream_size(code);
  translate_exp(parser, exp3, TRUE, FALSE);
  if (parser->abortError)
    return ;

  const int32_t boolExpOffset = wh_ostream_size(code);
  if ( ! translate_bool_exp(parser, exp2))
  {
    assert(parser->abortError == TRUE);
    return;
  }

  loop.breakMark = wh_ostream_size(code);
  if (encode_opcode(code, W_JFC) == NULL
      || wh_ostream_wint32(code, 0) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  store_le_int32(boolExpOffset - stepExpMark,
                 wh_ostream_data(code) + stepExpMark + opcode_bytes(W_JMP));

  struct WArray* const loopsStack = stmt_query_loop_stack(stmt);
  if (wh_array_add( loopsStack, &loop) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }
}

void
begin_foreach_stmt(struct ParserState* const   parser,
                   YYSTYPE                     id,
                   YYSTYPE                     exp,
                   const bool_t                reverse)
{
  struct Statement* const     stmt       = parser->pCurrentStmt;
  struct WOutputStream* const code       = stmt_query_instrs(stmt);
  struct SemId* const         identifier = (struct SemId*)&id->val.u_id;
  const enum W_OPCODE         stepOp     = reverse ? W_ITP : W_ITN;
  const enum W_OPCODE         beginOp    = reverse ? W_ITL : W_ITF;

  struct LoopIterator it = {0, };

  assert(id->val_type == VAL_ID);

  it.name       = identifier->name;
  it.nameLen    = identifier->length;
  it.localIndex = get_iterator_local_index(parser, it.name, it.nameLen);
  if (parser->abortError)
    return ;

  it.type = translate_iterable_exp(parser, exp);
  if (parser->abortError)
    return ;

  assert(it.type != T_UNKNOWN);

  if (encode_opcode(code, beginOp) == NULL
      || encode_opcode(code, W_JMP) == NULL
      || wh_ostream_wint32(code, opcode_bytes(W_JMP) + 4 + opcode_bytes(stepOp)) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  struct Loop loop;
  loop.type = LE_FOREACH_BEGIN;

  loop.continueMark = wh_ostream_size(code);
  if (encode_opcode(code, stepOp) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  loop.breakMark = wh_ostream_size(code);
  if (encode_opcode( code, W_JFC) == NULL
      || wh_ostream_wint32(code, 0) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  struct WArray* const loopsStack = stmt_query_loop_stack(stmt);
  if (wh_array_add( loopsStack, &loop) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  struct WArray* const iterators = stmt_query_loop_iterators_stack(stmt);
  if (wh_array_add(iterators, &it) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  free_sem_value(id);
}

void
finalize_for_stmt(struct ParserState* const parser)
{
  struct Statement* const     stmt       = parser->pCurrentStmt;
  struct WOutputStream* const stream     = stmt_query_instrs(stmt);
  uint8_t* const              code       = wh_ostream_data(stream);
  struct WArray* const        loopsStack = stmt_query_loop_stack(stmt);
  const struct Loop*          loopIt     = NULL;

  uint_t   loopId        = wh_array_count(loopsStack);
  int32_t  endForLoopPos = wh_ostream_size(stream);
  int32_t  endForStmtPos = 0;
  int32_t  offset        = 0;

  if (encode_opcode(stream, W_JMP) == NULL
      || wh_ostream_wint32(stream, 0) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  endForStmtPos = wh_ostream_size(stream);
  do
  {
    loopIt = wh_array_get(loopsStack, --loopId);
    switch (loopIt->type)
    {
    case LE_BREAK:
    case LE_FOR_BEGIN:
    case LE_FOREACH_BEGIN:
      offset = endForStmtPos - loopIt->breakMark;
      break;

    default:
      /* Continue statements should be already handled in case
       * of FOR statements */
      assert(FALSE);
    }
    /* Make the jump corrections. */
    store_le_int32(offset, code + loopIt->breakMark + 1);
  }
  while ((loopIt->type == LE_CONTINUE) || (loopIt->type == LE_BREAK));

  assert((loopIt->type == LE_FOR_BEGIN) || (loopIt->type == LE_FOREACH_BEGIN));

  offset = loopIt->continueMark - endForLoopPos;
  store_le_int32(offset, code + endForLoopPos + 1);

  if (loopIt->type == LE_FOREACH_BEGIN)
    {
      if (encode_opcode(stream, W_CTS) == NULL
          || wh_ostream_wint8(stream, 1) == NULL)
      {
        log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
        return;
      }

      struct WArray* const iterators = stmt_query_loop_iterators_stack(stmt);
      wh_array_resize(iterators, wh_array_count(iterators) -1 );
    }

  wh_array_resize(loopsStack, loopId);
}

void
begin_while_stmt(struct ParserState* const   parser,
                 YYSTYPE                     exp)
{
  struct Statement* const     stmt        = parser->pCurrentStmt;
  struct WOutputStream* const code        = stmt_query_instrs(stmt);
  struct WArray* const        loopsStack  = stmt_query_loop_stack(stmt);

  struct Loop loop;

  loop.type     = LE_WHILE_BEGIN;
  loop.continueMark = wh_ostream_size(code);

  if ( ! translate_bool_exp(parser, exp))
  {
    assert(parser->abortError == TRUE);

    return;
  }

  loop.breakMark = wh_ostream_size(code);
  if (encode_opcode( code, W_JFC) == NULL
      || wh_ostream_wint32(code, 0) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  if (wh_array_add(loopsStack, &loop) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }
}

void
finalize_while_stmt(struct ParserState* const parser)
{
  struct Statement* const     stmt       = parser->pCurrentStmt;
  struct WOutputStream* const stream     = stmt_query_instrs(stmt);
  uint8_t* const              code       = wh_ostream_data(stream);
  struct WArray* const        loopsStack = stmt_query_loop_stack(stmt);

  const struct Loop *loopIt          = NULL;
  uint_t             loopId          = wh_array_count(loopsStack);
  int32_t            endWhileLoopPos = wh_ostream_size(stream);
  int32_t            endWhileStmtPos = 0;
  int32_t            offset          = 0;

  if (encode_opcode(stream, W_JMP) == NULL
      || wh_ostream_wint32(stream, 0) == NULL)
    {
      log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return;
    }

  endWhileStmtPos = wh_ostream_size(stream);

  do
  {
    loopIt = wh_array_get(loopsStack, --loopId);
    switch (loopIt->type)
    {
    case LE_BREAK:
    case LE_WHILE_BEGIN:
      offset = endWhileStmtPos - loopIt->breakMark;
      break;

    default:
      /* Continue statements should be already handled in case
       * of while statements */
      assert(FALSE);
    }

    /* Make the jump corrections. */
    store_le_int32(offset, code + loopIt->breakMark + 1);

  } while ((loopIt->type == LE_CONTINUE) || (loopIt->type == LE_BREAK));

  assert(loopIt->type == LE_WHILE_BEGIN);

  offset = loopIt->continueMark - endWhileLoopPos;
  store_le_int32(offset, code + endWhileLoopPos + 1);

  wh_array_resize(loopsStack, loopId);
}

void
begin_until_stmt(struct ParserState* const parser)
{
  struct Statement* const     stmt        = parser->pCurrentStmt;
  struct WOutputStream* const code        = stmt_query_instrs(stmt);
  struct WArray* const        loopsStack  = stmt_query_loop_stack(stmt);

  struct Loop loop;

  loop.type         = LE_UNTIL_BEGIN;
  loop.continueMark = wh_ostream_size(code);
  loop.breakMark    = 0;

  if (wh_array_add( loopsStack, &loop) == NULL)
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
}

void
finalize_until_stmt(struct ParserState* const   parser,
                    YYSTYPE                     exp)
{
  struct Statement* const     stmt       = parser->pCurrentStmt;
  struct WOutputStream* const stream     = stmt_query_instrs(stmt);
  uint8_t* const              code       = wh_ostream_data(stream);
  struct WArray* const        loopsStack = stmt_query_loop_stack(stmt);

  struct Loop *loopIt          = NULL;
  uint_t       loopId          = wh_array_count(loopsStack);
  int32_t      untilExpPos     = wh_ostream_size(stream);
  int32_t      endUntilStmtPos = 0;
  int32_t      offset          = 0;

  if ( ! translate_bool_exp(parser, exp))
    {
      assert(parser->abortError == TRUE);
      return;
    }

  if (encode_opcode(stream, W_JTC) == NULL
      || wh_ostream_wint32(stream, 0) == NULL)
    {
      assert(parser->abortError == TRUE);
      return;
    }

  endUntilStmtPos = wh_ostream_size(stream);

  do
  {
    loopIt = wh_array_get(loopsStack, --loopId);
    switch(loopIt->type)
    {
    case LE_CONTINUE:
      offset = untilExpPos - loopIt->breakMark;
      break;

    case LE_BREAK:
      offset = endUntilStmtPos - loopIt->breakMark;
      break;

    case LE_UNTIL_BEGIN:
      /* JCT 0x01020304 should be encoded with 5 bytes */
      loopIt->breakMark = endUntilStmtPos - 5;
      offset = loopIt->continueMark - loopIt->breakMark;
      break;

    default:
      assert(0);
    }
    store_le_int32(offset, code + loopIt->breakMark + 1);
  }
  while ((loopIt->type == LE_CONTINUE) || (loopIt->type == LE_BREAK));

  wh_array_resize(loopsStack, loopId);
}

void
handle_break_stmt(struct ParserState* const parser)
{
  struct Statement* const     stmt       = parser->pCurrentStmt;
  struct WOutputStream* const code       = stmt_query_instrs(stmt);
  struct WArray* const        loopsStack = stmt_query_loop_stack(stmt);

  struct Loop loop;

  if (wh_array_count( loopsStack) == 0)
  {
    log_message(parser, parser->bufferPos, MSG_BREAK_NOLOOP);
    return;
  }

  loop.type         = LE_BREAK;
  loop.continueMark = 0;
  loop.breakMark    = wh_ostream_size(code);
  if (wh_array_add( loopsStack, &loop) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

    return;
  }

  if (encode_opcode( code, W_JMP) == NULL
      || wh_ostream_wint32(code, 0) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
  }
}

void
handle_continue_stmt(struct ParserState* const parser)
{
  struct Statement* const     stmt       = parser->pCurrentStmt;
  struct WOutputStream* const code       = stmt_query_instrs(stmt);
  struct WArray* const        loopsStack = stmt_query_loop_stack(stmt);

  const struct Loop *loopIt = NULL;
  uint_t             loopId = wh_array_count(loopsStack);

  struct Loop loop;

  while (loopId != 0)
  {
    loopIt = wh_array_get(loopsStack, --loopId);
    if ((loopIt->type != LE_CONTINUE) && (loopIt->type != LE_BREAK))
      break;

    else
      loopIt = NULL;
  }

  if (loopIt == NULL)
  {
    log_message(parser, parser->bufferPos, MSG_CONTINUE_NOLOOP);
    return;
  }

  loop.type      = LE_CONTINUE;
  loop.breakMark = wh_ostream_size(code);
  switch(loopIt->type)
  {
  case LE_UNTIL_BEGIN:
    if ((encode_opcode(code, W_JMP) == NULL)
        || (wh_ostream_wint32(code, 0) == NULL))
    {
      log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return;
    }

    if (wh_array_add(loopsStack, &loop) == NULL)
      log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    break;

  case LE_WHILE_BEGIN:
  case LE_FOR_BEGIN:
  case LE_FOREACH_BEGIN:
    if ((encode_opcode(code, W_JMP) == NULL)
        || wh_ostream_wint32(code, loopIt->continueMark - loop.breakMark) == NULL)
    {
      log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return;
    }
    /* This statement doesn't need jump correction. */
    break;

  default:
    assert(FALSE);
  }
}

void
begin_sync_stmt(struct ParserState* const parser)
{
  struct Statement *const     stmt       = parser->pCurrentStmt;
  struct WOutputStream *const code       = stmt_query_instrs(stmt);
  const uint_t                stmtsCount = stmt->spec.proc.syncTracker / 2;

  if (stmt->spec.proc.syncTracker & 1)
  {
    log_message(parser, parser->bufferPos, MSG_SYNC_NA);
    return;
  }
  else if (stmtsCount > 255)
  {
    log_message(parser, parser->bufferPos, MSG_SYNC_MANY);
    return;
  }

  if (encode_opcode( code, W_BSYNC) == NULL
      || wh_ostream_wint8(code, stmtsCount) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return;
  }

  stmt->spec.proc.syncTracker++;
}

void
finalize_sync_stmt(struct ParserState* const parser)
{
  struct Statement *const     stmt       = parser->pCurrentStmt;
  struct WOutputStream *const code       = stmt_query_instrs(stmt);
  const uint_t                stmtsCount = stmt->spec.proc.syncTracker / 2;

  assert(stmt->spec.proc.syncTracker & 1);

  if ((encode_opcode( code, W_ESYNC) == NULL)
      || (wh_ostream_wint8(code, stmtsCount) == NULL))
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

    return;
  }

  stmt->spec.proc.syncTracker++;
}
