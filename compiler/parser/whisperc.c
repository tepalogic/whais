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


#include "whisper.h"

#include "parser.h"
#include "../semantics/statement.h"
#include "../../utils/include/array.h"
#include "../../utils/include/outstream.h"
#include "../semantics/vardecl.h"

extern int yyparse (struct ParserState *);

WHC_HANDLER
whc_hnd_create (const char *buffer,
		unsigned buffer_len,
		WHC_MESSENGER msg_func, WHC_MESSENGER_ARG data)
{
  struct ParserState *state = mem_alloc (sizeof (struct ParserState));

  if (state != NULL)
    {
      state->buffer = buffer;
      state->bufferPos = 0;
      state->bufferSize = buffer_len;
      state->messengerContext = data;
      state->messenger = msg_func;
      state->abortError = FALSE;
      state->externDeclaration = FALSE;
      state->strings = create_string_store ();
      init_array (&(state->parsedValues), sizeof (struct SemValue));
      init_glbl_stmt (&(state->globalStmt));
      state->pCurrentStmt = &state->globalStmt;

      /* begin the compilation of the buffer */
      if (yyparse (state) != 0)
	{
	  whc_hnd_destroy ((WHC_HANDLER) state);
	  state = NULL;
	}
    }

  return state;
}

void
whc_hnd_destroy (WHC_HANDLER hnd)
{
  struct ParserState *state = (struct ParserState *) hnd;

  release_string_store (state->strings);
  clear_glbl_stmt (&(state->globalStmt));
  destroy_array (&state->parsedValues);
  mem_free (state);
}

unsigned int
whc_get_globals_count (WHC_HANDLER hnd)
{
  struct ParserState *state = (struct ParserState *) hnd;

  return state->globalStmt.localsUsed;
}

static struct DeclaredVar *
get_var_from_stmt (const struct Statement *stmt, unsigned int item)
{
  const unsigned int stored_vals = get_array_count (&(stmt->decls));
  unsigned int it = 0;

  for (it = 0; it < stored_vals; ++it)
    {
      struct DeclaredVar *var_it = get_item (&stmt->decls, it);
      if ((var_it->var_id & T_FIELD_MASK) != 0)
	continue;
      else if (RETRIVE_ID (var_it->var_id) == item)
	return var_it;
    }
  return NULL;
}

unsigned int
whc_get_global (WHC_HANDLER hnd, unsigned int item, WHC_GLBVAR_DESC * output)
{
  struct ParserState *state = (struct ParserState *) hnd;
  struct DeclaredVar *var = (struct DeclaredVar *)
    get_var_from_stmt (&(state->globalStmt), item);

  if (var == NULL)
    return 0;

  assert (var->var_id & GLOBAL_DECL);
  assert (RETRIVE_ID (var->var_id) == item);
  assert (var->l_label);

  output->m_Name = var->label;
  output->m_NameLength = var->l_label;
  output->m_Defined = (var->var_id & EXTERN_DECL) ? 0 : ~0;
  output->m_Type = get_buffer_outstream (&(state->globalStmt.spec.glb.typesDescs)) + var->type_spec_pos;
  return ~0;
}

unsigned int
whc_get_procs_count (WHC_HANDLER hnd)
{
  struct ParserState* pState = (struct ParserState*) hnd;

  return pState->globalStmt.spec.glb.procsCount;
}

unsigned int
whc_get_proc (WHC_HANDLER hnd, unsigned int item, WHC_PROC_DESC* pOutDesc)
{
  const struct Statement* pProc  = (const struct Statement*) whc_get_proc_hnd (hnd, item);

  if (pProc == NULL)
    return 0; /* Not found */

  assert ((pProc->spec.proc.procId & ~EXTERN_DECL) == item);

  pOutDesc->m_Name = pProc->spec.proc.name;
  pOutDesc->m_NameLength = pProc->spec.proc.nameLength;
  pOutDesc->m_ParamsCount = get_array_count (&(pProc->spec.proc.paramsList)) - 1;
  pOutDesc->m_LocalsCount = pProc->localsUsed;
  pOutDesc->m_SyncsCount = pProc->spec.proc.syncTracker;
  pOutDesc->m_CodeSize = get_size_outstream (&(pProc->spec.proc.code));
  pOutDesc->m_Code = get_buffer_outstream (&(pProc->spec.proc.code));

  return 1; /* Output valid */
}

WHC_PROC_HANDLER
whc_get_proc_hnd (WHC_HANDLER hnd, unsigned int item)
{
  struct ParserState*     pState     = (struct ParserState*) hnd;
  const struct Statement* pProc      = NULL;
  const D_UINT            totalProcs = get_array_count (&pState->globalStmt.spec.glb.procsDecls);
  D_UINT                  procIndex;

  for (procIndex = 0; procIndex < totalProcs; ++procIndex)
    {
      const struct Statement* pProcIt = get_item (&pState->globalStmt.spec.glb.procsDecls,
                                                  procIndex);
      assert (pProcIt->type & STMT_PROC);

      if (RETRIVE_ID (pProcIt->spec.proc.procId) == item)
        {
          pProc = pProcIt;
          break;
        }
    }

  return (WHC_PROC_HANDLER) pProc;
}

void
whc_release_proc_hnd (WHC_HANDLER hnd, WHC_PROC_HANDLER h_proc)
{
  return;
}

const unsigned char *
whc_get_proc_rettype (WHC_HANDLER hnd, WHC_PROC_HANDLER h_proc)
{
  struct ParserState *state = (struct ParserState *) hnd;
  struct Statement *proc = (struct Statement *) h_proc;
  struct DeclaredVar *var = (struct DeclaredVar *)
    get_item (&(proc->spec.proc.paramsList), 0);

  assert (var != NULL);
  return get_buffer_outstream (&(state->globalStmt.spec.glb.typesDescs))
    + var->type_spec_pos;
}

const unsigned char *
whc_get_local_type (WHC_HANDLER hnd,
		    WHC_PROC_HANDLER h_proc, unsigned int local)
{

  struct ParserState *state = (struct ParserState *) hnd;
  struct Statement *proc = (struct Statement *) h_proc;
  struct DeclaredVar *var = NULL;
  const D_UINT param_count = get_array_count (&(proc->spec.proc.paramsList));

  assert (proc->type == STMT_PROC);
  if (local < param_count)
    var = get_item (&(proc->spec.proc.paramsList), local);
  else
    var = get_var_from_stmt (proc, local);

  if (var == NULL)
    return NULL;
  else
    return get_buffer_outstream (&(state->globalStmt.spec.glb.typesDescs))
      + var->type_spec_pos;
}

unsigned int
whc_get_typedec_pool (WHC_HANDLER hnd, const unsigned char **type_info)
{

  struct ParserState *state = (struct ParserState *) hnd;
  const struct Statement *const glb_stm = &state->globalStmt;
  const struct OutStream *const type_desc = &glb_stm->spec.glb.typesDescs;
  const unsigned type_desc_size = get_size_outstream (type_desc);

  assert (glb_stm->type == STMT_GLOBAL);

  *type_info = (type_desc_size != 0) ?
    get_buffer_outstream (type_desc) : NULL;
  return type_desc_size;

}

unsigned int
whc_get_const_area (WHC_HANDLER hnd, const unsigned char **const_area)
{
  struct ParserState *state = (struct ParserState *) hnd;
  const struct Statement *const glb_stm = &state->globalStmt;
  const struct OutStream *const consts_outs = &glb_stm->spec.glb.constsArea;
  const unsigned const_areas_size = get_size_outstream (consts_outs);

  assert (glb_stm->type == STMT_GLOBAL);

  *const_area = (const_areas_size != 0) ?
    get_buffer_outstream (consts_outs) : NULL;
  return const_areas_size;
}

void
whc_get_libver (unsigned int *major, unsigned int *minor)
{
  *major = 0;
  *minor = 1;
}

void
whc_get_lang_ver (unsigned int *major, unsigned int *minor)
{
  *major = 1;
  *minor = 0;
}
