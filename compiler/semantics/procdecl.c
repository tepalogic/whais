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

struct Statement *
find_proc_decl (struct ParserState* state,
		const D_CHAR* name,
		const D_UINT  nlength,
		const D_BOOL  referenced)
{
  const struct UArray *procs = &(state->globalStmt.spec.glb.procsDecls);
  D_UINT count = get_array_count (procs);
  while (count-- > 0)
    {
      struct Statement *result = NULL;
      result = (struct Statement *) get_item (procs, count);
      assert (result->type == STMT_PROC);
      assert (result->spec.proc.name != NULL);
      assert (result->spec.proc.nameLength != 0);

      if ((result->spec.proc.nameLength == nlength) &&
	  (strncmp (result->spec.proc.name, name, nlength) == 0))
	{
          if (referenced && ((result->spec.proc.procId & NOTREF_DECL) != 0))
            {
              assert (RETRIVE_ID (result->spec.proc.procId) == 0);
              assert ((result->spec.proc.procId & EXTERN_DECL) != 0);

              result->spec.proc.procId &= ~NOTREF_DECL;
              result->spec.proc.procId |= state->globalStmt.spec.glb.procsCount++;
            }

	  return result;
	}
    }

  /* not found */
  return NULL;

}

YYSTYPE
add_prcdcl_list (YYSTYPE list, YYSTYPE s_id, YYSTYPE s_type)
{
  struct SemTypeSpec tmp_tspec;

  assert (list == NULL || list->val_type == VAL_PRCDCL_LIST);
  assert (s_id->val_type == VAL_ID);
  assert (s_type->val_type == VAL_TYPE_SPEC);

  tmp_tspec = s_type->val.u_tspec;

  /* reconvert s_type to a VAL_PRCDCL_LIST */
  s_type->val_type = VAL_PRCDCL_LIST;
  s_type->val.u_prdcl.type = tmp_tspec;
  s_type->val.u_prdcl.id = s_id->val.u_id;
  s_type->val.u_prdcl.next = list;

  /* recycle some things */
  s_id->val_type = VAL_REUSE;

  return s_type;

}

void
install_proc_args (struct ParserState *state, struct SemValue *arg_list)
{
  struct SemValue s_id;
  struct SemValue s_type;

  s_id.val_type = VAL_ID;
  s_type.val_type = VAL_TYPE_SPEC;

  while (arg_list != NULL)
    {
      assert (arg_list->val_type == VAL_PRCDCL_LIST);

      s_id.val.u_id = arg_list->val.u_prdcl.id;
      s_type.val.u_tspec = arg_list->val.u_prdcl.type;

      install_declaration (state, &s_id, &s_type, TRUE, TRUE);

      arg_list->val_type = VAL_REUSE;
      arg_list = arg_list->val.u_prdcl.next;
    }
}

void
install_proc_decl (struct ParserState *state, struct SemValue *val_id)
{
  struct UArray *const procs = &(state->globalStmt.spec.glb.procsDecls);
  struct Statement stmt;

  assert (state->pCurrentStmt->type == STMT_GLOBAL);
  assert (val_id->val_type == VAL_ID);

  if (init_proc_stmt (state->pCurrentStmt, &stmt) == FALSE)
    {
      /* not enough memory */
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

    }

  stmt.spec.proc.name = val_id->val.u_id.text;
  stmt.spec.proc.nameLength = val_id->val.u_id.length;

  if (find_proc_decl (state, stmt.spec.proc.name, stmt.spec.proc.nameLength, FALSE) != NULL)
    {
      D_CHAR tname[128];
      copy_text_truncate (tname, stmt.spec.proc.name,
			  sizeof tname, stmt.spec.proc.nameLength);
      w_log_msg (state, state->bufferPos, MSG_PROC_ADECL, tname);

      clear_proc_stmt (&stmt);
    }
  else
    {
      void *nstmt = NULL;

      if (state->externDeclaration)
	stmt.spec.proc.procId = EXTERN_DECL | NOTREF_DECL;
      else
        stmt.spec.proc.procId = state->globalStmt.spec.glb.procsCount++;

      nstmt = add_item (procs, &stmt);
      if (nstmt == NULL)
	{
	  /* not enough memory */
	  w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);
	  state->abortError = TRUE;
	  clear_proc_stmt (&stmt);
	}
      else
	state->pCurrentStmt = nstmt;
    }

  val_id->val_type = VAL_REUSE;

  return;
}

void
set_proc_rettype (struct ParserState* pState, struct SemValue* pRettype)
{
  struct DeclaredVar* pRetVar = (struct DeclaredVar*)
    get_item (&(pState->pCurrentStmt->spec.proc.paramsList), 0);

  assert (pRettype->val_type == VAL_TYPE_SPEC);

  memset (pRetVar, 0, sizeof (pRetVar));
  pRetVar->type  = pRettype->val.u_tspec.type;
  pRetVar->extra = pRettype->val.u_tspec.extra;

  if (pRetVar->type & T_TABLE_MASK)
    {
      struct DeclaredVar *it = pRetVar->extra;
      if (it == NULL)
        pRetVar->extra = pRetVar;
      else
        {
          while (it->extra != NULL)
            {
              assert (it->type & T_FIELD_MASK);
              it = it->extra;
            }
          it->extra = pRetVar;
        }
    }

  pRetVar->type_spec_pos =
    type_spec_fill (&pState->globalStmt.spec.glb.typesDescs, pRetVar);
  pRettype->val_type = VAL_REUSE;
  return;
}

void
finish_proc_decl (struct ParserState *state)
{
  assert (state->pCurrentStmt->type = STMT_PROC);
  assert (&(state->globalStmt) == state->pCurrentStmt->pParentStmt);

  state->pCurrentStmt = &(state->globalStmt);

  return;
}
