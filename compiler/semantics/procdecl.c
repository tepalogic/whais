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
  const struct UArray *procs = &(state->global_stmt.spec.glb.proc_decls);
  D_UINT count = get_array_count (procs);
  while (count-- > 0)
    {
      struct Statement *result = NULL;
      result = (struct Statement *) get_item (procs, count);
      assert (result->type == STMT_PROC);
      assert (result->spec.proc.name != NULL);
      assert (result->spec.proc.nlength != 0);

      if ((result->spec.proc.nlength == nlength) &&
	  (strncmp (result->spec.proc.name, name, nlength) == 0))
	{
          if (referenced && ((result->spec.proc.proc_id & NOTREF_DECL) != 0))
            {
              assert (RETRIVE_ID (result->spec.proc.proc_id) == 0);
              assert ((result->spec.proc.proc_id & EXTERN_DECL) != 0);

              result->spec.proc.proc_id &= ~NOTREF_DECL;
              result->spec.proc.proc_id |= state->global_stmt.spec.glb.procs_count++;
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
  struct UArray *const procs = &(state->global_stmt.spec.glb.proc_decls);
  struct Statement stmt;

  assert (state->current_stmt->type == STMT_GLOBAL);
  assert (val_id->val_type == VAL_ID);

  if (init_proc_stmt (state->current_stmt, &stmt) == FALSE)
    {
      /* not enough memory */
      w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);

    }

  stmt.spec.proc.name = val_id->val.u_id.text;
  stmt.spec.proc.nlength = val_id->val.u_id.length;

  if (find_proc_decl (state, stmt.spec.proc.name, stmt.spec.proc.nlength, FALSE) != NULL)
    {
      D_CHAR tname[128];
      copy_text_truncate (tname, stmt.spec.proc.name,
			  sizeof tname, stmt.spec.proc.nlength);
      w_log_msg (state, state->buffer_pos, MSG_PROC_ADECL, tname);

      clear_proc_stmt (&stmt);
    }
  else
    {
      void *nstmt = NULL;

      if (state->extern_decl)
	stmt.spec.proc.proc_id = EXTERN_DECL | NOTREF_DECL;
      else
        stmt.spec.proc.proc_id = state->global_stmt.spec.glb.procs_count++;

      nstmt = add_item (procs, &stmt);
      if (nstmt == NULL)
	{
	  /* not enough memory */
	  w_log_msg (state, IGNORE_BUFFER_POS, MSG_NO_MEM);
	  state->err_sem = TRUE;
	  clear_proc_stmt (&stmt);
	}
      else
	state->current_stmt = nstmt;
    }

  val_id->val_type = VAL_REUSE;

  return;
}

void
set_proc_rettype (struct ParserState *state, struct SemValue *rtype)
{
  struct DeclaredVar *retv = (struct DeclaredVar *)
    get_item (&(state->current_stmt->spec.proc.param_list), 0);

  assert (rtype->val_type == VAL_TYPE_SPEC);

  memset (retv, 0, sizeof (retv));
  retv->type = rtype->val.u_tspec.type;
  retv->extra = rtype->val.u_tspec.extra;

  if (retv->type & T_CONTAINER_MASK)
    {
      if (process_container_decls (state, retv, retv->extra))
	{
	  if (retv->type & (T_TABLE_MASK | T_RECORD_MASK))
	    {
	      struct DeclaredVar *it = retv->extra;

	      if (it == NULL)
		retv->extra = retv;
	      else
		{
		  while (it->extra && ((it->extra->type & T_FIELD_MASK) != 0))
		    it = it->extra;

		  it->extra = retv;
		}
	    }

	}
      else
	return;			/* An error occurred! */
    }

  retv->type_spec_pos =
    type_spec_fill (&state->global_stmt.spec.glb.type_desc, retv);
  rtype->val_type = VAL_REUSE;
  return;
}

void
finish_proc_decl (struct ParserState *state)
{
  assert (state->current_stmt->type = STMT_PROC);
  assert (&(state->global_stmt) == state->current_stmt->parent);

  state->current_stmt = &(state->global_stmt);

  return;
}
