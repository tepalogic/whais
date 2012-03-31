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

extern int yyparse (struct ParserState*);

WHC_HANDLER
whc_hnd_create (const char*       pBuffer,
                unsigned          bufferSize,
                WHC_MESSENGER     messenger,
                WHC_MESSENGER_ARG messengerContext)
{
  struct ParserState* pState = mem_alloc (sizeof (*pState));

  if (pState != NULL)
    {
      pState->buffer            = pBuffer;
      pState->bufferPos         = 0;
      pState->bufferSize        = bufferSize;
      pState->messengerContext  = messengerContext;
      pState->messenger         = messenger;
      pState->abortError        = FALSE;
      pState->externDeclaration = FALSE;
      pState->strings           = create_string_store ();
      init_array (&(pState->parsedValues), sizeof (struct SemValue));
      init_glbl_stmt (&(pState->globalStmt));
      pState->pCurrentStmt = &pState->globalStmt;

      /* begin the compilation of the buffer */
      if (yyparse (pState) != 0)
        {
          whc_hnd_destroy ((WHC_HANDLER) pState);
          pState = NULL;
        }
    }

  return pState;
}

void
whc_hnd_destroy (WHC_HANDLER hnd)
{
  struct ParserState* pState = (struct ParserState*)hnd;

  release_string_store (pState->strings);
  clear_glbl_stmt (&(pState->globalStmt));
  destroy_array (&pState->parsedValues);
  mem_free (pState);
}

unsigned int
whc_get_globals_count (WHC_HANDLER hnd)
{
  struct ParserState* pState = (struct ParserState*)hnd;

  return pState->globalStmt.localsUsed;
}

static struct DeclaredVar*
get_var_from_stmt (const struct Statement* pStmt, unsigned int item)
{
  const unsigned int stored_vals = get_array_count (&(pStmt->decls));
  unsigned int       it          = 0;

  for (it = 0; it < stored_vals; ++it)
    {
      struct DeclaredVar *var_it = get_item (&pStmt->decls, it);

      if (IS_TABLE_FIELD (var_it->varId))
        continue;
      else if (RETRIVE_ID (var_it->varId) == item)
        return var_it;
    }
  return NULL;
}

unsigned int
whc_get_global (WHC_HANDLER      hnd,
                unsigned int     globalId,
                WHC_GLBVAR_DESC* pOutDescript)
{
  struct ParserState* pState = (struct ParserState*)hnd;
  struct DeclaredVar* pVar   = (struct DeclaredVar*)get_var_from_stmt (&(pState->globalStmt),
                                                                       globalId);
  if (pVar == NULL)
    return 0;

  assert (IS_GLOBAL (pVar->varId));
  assert (RETRIVE_ID (pVar->varId) == globalId);
  assert (pVar->labelLength);

  pOutDescript->m_Name       = pVar->label;
  pOutDescript->m_NameLength = pVar->labelLength;
  pOutDescript->m_Defined    = (pVar->varId & EXTERN_DECL) ? 0 : ~0;
  pOutDescript->m_Type       = get_buffer_outstream (&(pState->globalStmt.spec.glb.typesDescs)) +
                               pVar->typeSpecOff;
  return ~0;
}

unsigned int
whc_get_procs_count (WHC_HANDLER hnd)
{
  struct ParserState* pState = (struct ParserState*)hnd;

  return pState->globalStmt.spec.glb.procsCount;
}

unsigned int
whc_get_proc (WHC_HANDLER    hnd,
              unsigned int   procId,
              WHC_PROC_DESC* pOutDesc)
{
  const struct Statement* pProc  = (const struct Statement*)whc_get_proc_hnd (hnd, procId);

  if (pProc == NULL)
    return 0; /* Not found */

  assert (RETRIVE_ID (pProc->spec.proc.procId) == procId);

  pOutDesc->m_Name        = pProc->spec.proc.name;
  pOutDesc->m_NameLength  = pProc->spec.proc.nameLength;
  pOutDesc->m_ParamsCount = get_array_count (&(pProc->spec.proc.paramsList)) - 1;
  pOutDesc->m_LocalsCount = pProc->localsUsed;
  pOutDesc->m_SyncsCount  = pProc->spec.proc.syncTracker;
  pOutDesc->m_CodeSize    = get_size_outstream (&(pProc->spec.proc.code));
  pOutDesc->m_Code        = get_buffer_outstream (&(pProc->spec.proc.code));

  return 1; /* Output valid */
}

WHC_PROC_HANDLER
whc_get_proc_hnd (WHC_HANDLER hnd, unsigned int procId)
{
  struct ParserState*     pState     = (struct ParserState*)hnd;
  const struct Statement* pProc      = NULL;
  const D_UINT            totalProcs = get_array_count (&pState->globalStmt.spec.glb.procsDecls);
  D_UINT                  procIndex;

  for (procIndex = 0; procIndex < totalProcs; ++procIndex)
    {
      const struct Statement* pProcIt = get_item (&pState->globalStmt.spec.glb.procsDecls,
                                                  procIndex);
      assert (pProcIt->type & STMT_PROC);

      if (RETRIVE_ID (pProcIt->spec.proc.procId) == procId)
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
  /* Do nothing for now! */
  return;
}

const unsigned char *
whc_get_proc_rettype (WHC_HANDLER hnd, WHC_PROC_HANDLER hProc)
{
  struct ParserState* const pState     = (struct ParserState*) hnd;
  struct Statement* const   pProc      = (struct Statement*) hProc;
  struct DeclaredVar* const pRetrunVar = (struct DeclaredVar*)
                                          get_item (&(pProc->spec.proc.paramsList), 0);

  assert (pRetrunVar != NULL);

  return get_buffer_outstream (&(pState->globalStmt.spec.glb.typesDescs)) +
         pRetrunVar->typeSpecOff;
}

const unsigned char *
whc_get_local_type (WHC_HANDLER      hnd,
                    WHC_PROC_HANDLER hProc,
                    unsigned int     localId)
{

  struct ParserState* pState          = (struct ParserState*) hnd;
  struct Statement*   pProc           = (struct Statement*) hProc;
  struct DeclaredVar* pLocalVal       = NULL;
  const D_UINT        procParamsCount = get_array_count (&(pProc->spec.proc.paramsList));

  assert (pProc->type == STMT_PROC);

  if (localId < procParamsCount)
    pLocalVal = get_item (&(pProc->spec.proc.paramsList), localId);
  else
    pLocalVal = get_var_from_stmt (pProc, localId);

  if (pLocalVal == NULL)
    return NULL;
  else
    return get_buffer_outstream (&(pState->globalStmt.spec.glb.typesDescs)) +
           pLocalVal->typeSpecOff;
}

unsigned int
whc_get_typedec_pool (WHC_HANDLER hnd, const unsigned char** pOutPTypes)
{

  struct ParserState* const     pState       = (struct ParserState*) hnd;
  const struct Statement* const pGlbStm      = &pState->globalStmt;
  const struct OutStream* const pTypeDescs   = &pGlbStm->spec.glb.typesDescs;
  const unsigned                typeDescSize = get_size_outstream (pTypeDescs);

  assert (pGlbStm->type == STMT_GLOBAL);

  *pOutPTypes = (typeDescSize != 0) ? get_buffer_outstream (pTypeDescs) : NULL;
  return typeDescSize;

}

unsigned int
whc_get_const_area (WHC_HANDLER hnd, const unsigned char** pOutPConsts)
{
  struct ParserState* const     pState     = (struct ParserState*) hnd;
  const struct Statement* const pGlbStm    = &pState->globalStmt;
  const struct OutStream* const pConsts    = &pGlbStm->spec.glb.constsArea;
  const unsigned                constsSize = get_size_outstream (pConsts);

  assert (pGlbStm->type == STMT_GLOBAL);

  *pOutPConsts = (constsSize != 0) ? get_buffer_outstream (pConsts) : NULL;
  return constsSize;
}

void
whc_get_libver (unsigned int* pOutMajor, unsigned int* pOutMinor)
{
  *pOutMajor = 0;
  *pOutMinor = 1;
}

void
whc_get_lang_ver (unsigned int* pOutMajor, unsigned int* pOutMinor)
{
  *pOutMajor = 1;
  *pOutMinor = 0;
}
