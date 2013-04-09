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

#include <assert.h>
#include <string.h>

#include "procdecl.h"
#include "wlog.h"
#include "vardecl.h"

struct Statement*
find_proc_decl(struct ParserState* pState,
               const char*       pName,
               const uint_t        nameLength,
               const bool_t        referenced)
{
  const struct UArray* pProcsList = &(pState->globalStmt.spec.glb.procsDecls);
  uint_t               procIt     = get_array_count (pProcsList);

  while (procIt-- > 0)
    {
      struct Statement* result = (struct Statement*) get_item (pProcsList,
          procIt);

      assert(result->type == STMT_PROC);
      assert(result->spec.proc.name != NULL);
      assert(result->spec.proc.nameLength != 0);

      if ((result->spec.proc.nameLength == nameLength)
          && (strncmp (result->spec.proc.name, pName, nameLength) == 0))
        {
          if (referenced && (IS_REFERENCED (result->spec.proc.procId) == FALSE))
            {
              assert(RETRIVE_ID (result->spec.proc.procId) == 0);
              assert(IS_EXTERNAL (result->spec.proc.procId));

              result->spec.proc.procId |= pState->globalStmt.spec.glb.procsCount++;
              MARK_AS_REFERENCED(result->spec.proc.procId);
            }
          return result;
        }
    }

  return NULL;
}

YYSTYPE add_prcdcl_list(YYSTYPE pList, YYSTYPE pIdentifier, YYSTYPE pType)
{
  struct SemTypeSpec typeDesc = pType->val.u_tspec;

  assert(pList == NULL || pList->val_type == VAL_PRCDCL_LIST);
  assert(pIdentifier->val_type == VAL_ID);
  assert(pType->val_type == VAL_TYPE_SPEC);

  /* reconvert s_type to a VAL_PRCDCL_LIST */
  pType->val_type         = VAL_PRCDCL_LIST;
  pType->val.u_prdcl.type = typeDesc;
  pType->val.u_prdcl.id   = pIdentifier->val.u_id;
  pType->val.u_prdcl.next = pList;

  /* recycle some things */
  free_sem_value (pIdentifier);

  return pType;
}

void
install_proc_args(struct ParserState* const pState, struct SemValue* pArgList)
{
  struct SemValue identifier;
  struct SemValue type;

  identifier.val_type = VAL_ID;
  type.val_type       = VAL_TYPE_SPEC;

  while (pArgList != NULL)
    {
      assert(pArgList->val_type == VAL_PRCDCL_LIST);

      identifier.val.u_id = pArgList->val.u_prdcl.id;
      type.val.u_tspec    = pArgList->val.u_prdcl.type;

      install_declaration (pState, &identifier, &type, TRUE, TRUE);

      pArgList->val_type = VAL_REUSE;
      pArgList           = pArgList->val.u_prdcl.next;
    }
}

void
install_proc_decl(struct ParserState* const pState,
                  struct SemValue* const    pIdentifier)
{
  struct UArray* const procs = &(pState->globalStmt.spec.glb.procsDecls);
  struct Statement     stmt;

  assert(pState->pCurrentStmt->type == STMT_GLOBAL);
  assert(pIdentifier->val_type == VAL_ID);

  if (init_proc_stmt (pState->pCurrentStmt, &stmt) == FALSE)
    w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);

  stmt.spec.proc.name       = pIdentifier->val.u_id.text;
  stmt.spec.proc.nameLength = pIdentifier->val.u_id.length;

  if (find_proc_decl (pState,
                      stmt.spec.proc.name,
                      stmt.spec.proc.nameLength,
                      FALSE) != NULL)
    {
      char tname[128];
      copy_text_truncate (tname,
                          stmt.spec.proc.name,
                          sizeof tname,
                          stmt.spec.proc.nameLength);
      w_log_msg (pState, pState->bufferPos, MSG_PROC_ADECL, tname);

      clear_proc_stmt (&stmt);
    }
  else
    {
      void* check = NULL;

      if (pState->externDeclaration)
        {
          stmt.spec.proc.procId = 0;

          MARK_AS_EXTERNAL(stmt.spec.proc.procId);
          MARK_AS_NOT_REFERENCED(stmt.spec.proc.procId);
        }
      else
        stmt.spec.proc.procId = pState->globalStmt.spec.glb.procsCount++;

      check = add_item (procs, &stmt);
      if (check == NULL)
        {
          w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
          pState->abortError = TRUE;
          clear_proc_stmt (&stmt);
        }
      else
        pState->pCurrentStmt = check;
    }

  free_sem_value (pIdentifier);

  return;
}

void
set_proc_rettype(struct ParserState* const pState, struct SemValue* const pType)
{
  struct DeclaredVar* pRetVar = (struct DeclaredVar*)
      get_item (&(pState->pCurrentStmt->spec.proc.paramsList), 0);

  assert(pType->val_type == VAL_TYPE_SPEC);

  memset (pRetVar, 0, sizeof(pRetVar));
  pRetVar->type  = pType->val.u_tspec.type;
  pRetVar->extra = pType->val.u_tspec.extra;

  if (IS_TABLE (pRetVar->type))
    {
      struct DeclaredVar* pVarIt = pRetVar->extra;

      if (pVarIt == NULL)
        pRetVar->extra = pRetVar;
      else
        {
          while (pVarIt->extra != NULL)
            {
              assert(IS_TABLE_FIELD (pVarIt->type));
              pVarIt = pVarIt->extra;
            }
          pVarIt->extra = pRetVar;
        }
    }

  pRetVar->typeSpecOff = type_spec_fill (&pState->globalStmt.spec.glb.typesDescs,
                                         pRetVar);
  free_sem_value (pType);

  return;
}

void
finish_proc_decl(struct ParserState* const pState)
{
  assert(pState->pCurrentStmt->type = STMT_PROC);
  assert(&(pState->globalStmt) == pState->pCurrentStmt->pParentStmt);

  pState->pCurrentStmt = &pState->globalStmt;

  return;
}
