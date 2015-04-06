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


#include "whais.h"

#include "utils/warray.h"
#include "utils/woutstream.h"

#include "parser.h"
#include "../semantics/statement.h"
#include "../semantics/vardecl.h"

extern int yyparse (struct ParserState*);

WH_COMPILED_UNIT
wh_compiler_load (const char*         sourceCode,
                  unsigned            sourceSize,
                  WH_MESSENGER        messenger,
                  WH_MESSENGER_CTXT   messengerContext)
{
  struct ParserState* state = mem_alloc (sizeof( *state));

  if (state != NULL)
    {
      state->buffer            = sourceCode;
      state->bufferPos         = 0;
      state->bufferSize        = sourceSize;
      state->messenger         = messenger;
      state->messengerCtxt     = messengerContext;
      state->abortError        = FALSE;
      state->externDecl        = FALSE;
      state->strings           = create_string_store ();

      wh_array_init (&(state->values), sizeof (struct SemValue));
      init_glbl_stmt (&(state->globalStmt));
      state->pCurrentStmt = &state->globalStmt;

      /* begin the compilation of the buffer */
      if (yyparse( state) != 0)
        {
          wh_compiler_discard ((WH_COMPILED_UNIT) state);
          state = NULL;
        }
    }

  return state;
}


void
wh_compiler_discard (WH_COMPILED_UNIT hnd)
{
  struct ParserState* state = (struct ParserState*)hnd;

  release_string_store (state->strings);
  clear_glbl_stmt (&(state->globalStmt));
  wh_array_clean (&state->values);
  mem_free (state);
}


uint_t
wh_unit_globals_count (WH_COMPILED_UNIT hnd)
{
  struct ParserState* state = (struct ParserState*)hnd;

  return state->globalStmt.localsUsed;
}


static struct DeclaredVar*
get_var_from_stmt (const struct Statement* stmt, uint_t item)
{
  const uint_t varsCount = wh_array_count (&(stmt->decls));
  uint_t       it        = 0;

  for (it = 0; it < varsCount; ++it)
    {
      struct DeclaredVar* var = wh_array_get (&stmt->decls, it);

      if (IS_TABLE_FIELD( var->type))
        continue;

      else if (((stmt->type == STMT_PROC) || IS_REFERRED (var->varId))
               && (RETRIVE_ID( var->varId) == item))
        {
          return var;
        }
    }

  return NULL;
}


uint_t
wh_unit_global (WH_COMPILED_UNIT           hnd,
                const uint_t               id,
                WCompilerGlobalDesc* const outDescription)
{
  struct ParserState* state = (struct ParserState*)hnd;
  struct DeclaredVar* var   = get_var_from_stmt (&(state->globalStmt), id);

  if (var == NULL)
    return 0;

  assert (IS_GLOBAL( var->varId));
  assert (RETRIVE_ID( var->varId) == id);
  assert (var->labelLength);

  outDescription->name       = var->label;
  outDescription->nameLength = var->labelLength;
  outDescription->defined    = (var->varId & EXTERN_DECL) ? 0 : ~0;

  outDescription->type = wh_ostream_data(
                                  &(state->globalStmt.spec.glb.typesDescs)
                                         ) +
                         var->typeSpecOff;

  /* TODO: return value is misleading. */
  return -1;
}


uint_t
wh_unit_procedures_count (WH_COMPILED_UNIT hnd)
{
  struct ParserState* state = (struct ParserState*)hnd;

  return state->globalStmt.spec.glb.procsCount;
}


uint_t
wh_unit_procedure (WH_COMPILED_UNIT              hnd,
                   const uint_t                  id,
                   WCompilerProcedureDesc* const outDescription)
{
  const struct Statement* stmt  = (const struct Statement*)
                                  wh_unit_procedure_get (hnd, id);

  if (stmt == NULL)
    return 0; /* Not found */

  assert (RETRIVE_ID( stmt->spec.proc.procId) == id);

  outDescription->name        = stmt->spec.proc.name;
  outDescription->nameLength  = stmt->spec.proc.nameLength;
  outDescription->paramsCount = wh_array_count (&(stmt->spec.proc.paramsList))
                                - 1;
  outDescription->localsCount = stmt->localsUsed;
  outDescription->syncsCount  = stmt->spec.proc.syncTracker;
  outDescription->codeSize    = wh_ostream_size (&(stmt->spec.proc.code));
  outDescription->code        = wh_ostream_data (&(stmt->spec.proc.code));

  return 1; /* Output valid */
}


WH_COMPILED_UNIT_PROC
wh_unit_procedure_get (WH_COMPILED_UNIT hnd, const uint_t id)
{
  struct ParserState*     state = (struct ParserState*)hnd;
  const struct Statement* stmt  = NULL;

  const uint_t totalProcs = wh_array_count(
                                    &state->globalStmt.spec.glb.procsDecls
                                           );
  uint_t procIndex;

  for (procIndex = 0; procIndex < totalProcs; ++procIndex)
    {
      const struct Statement* it = wh_array_get(
                                      &state->globalStmt.spec.glb.procsDecls,
                                      procIndex
                                                );
      assert (it->type & STMT_PROC);

      if (IS_REFERRED (it->spec.proc.procId)
          && (RETRIVE_ID( it->spec.proc.procId) == id))
        {
          stmt = it;
          break;
        }
    }

  return (WH_COMPILED_UNIT_PROC) stmt;
}


void
wh_unit_procedure_release (WH_COMPILED_UNIT         hnd,
                           WH_COMPILED_UNIT_PROC    proc)
{
  /* Do nothing for now! */
  return;
}


uint_t
wh_unit_type_descriptors (WH_COMPILED_UNIT     hnd,
                          const uint8_t**      ppTypePool)
{
  struct ParserState* const         state     = (struct ParserState*) hnd;
  const struct Statement* const     glbStmt   = &state->globalStmt;
  const struct WOutputStream* const types     = &glbStmt->spec.glb.typesDescs;
  const unsigned                    typesSize = wh_ostream_size (types);

  assert (glbStmt->type == STMT_GLOBAL);

  *ppTypePool = (typesSize != 0) ? wh_ostream_data (types) : NULL;
  return typesSize;
}


uint_t
wh_unit_constants (WH_COMPILED_UNIT     hnd,
                   const uint8_t**      ppConstPool)
{
  struct ParserState* const         state      = (struct ParserState*) hnd;
  const struct Statement* const     glbStmt    = &state->globalStmt;
  const struct WOutputStream* const consts     = &glbStmt->spec.glb.constsArea;
  const unsigned                    constsSize = wh_ostream_size (consts);

  assert (glbStmt->type == STMT_GLOBAL);

  *ppConstPool = (constsSize != 0) ? wh_ostream_data (consts) : NULL;
  return constsSize;
}


const uint8_t*
wh_procedure_return_type (WH_COMPILED_UNIT      hnd,
                          WH_COMPILED_UNIT_PROC proc)
{
  struct ParserState* const state  = (struct ParserState*) hnd;
  struct Statement* const   stmt   = (struct Statement*) proc;
  struct DeclaredVar* const retVar = (struct DeclaredVar*)
                                     wh_array_get(
                                            &(stmt->spec.proc.paramsList),
                                            0
                                                  );

  assert (retVar != NULL);

  return wh_ostream_data (&(state->globalStmt.spec.glb.typesDescs)) +
         retVar->typeSpecOff;
}


const uint8_t*
wh_procedure_local_type (WH_COMPILED_UNIT               hnd,
                         WH_COMPILED_UNIT_PROC          proc,
                         const uint_t                   id)
{
  struct ParserState* state = (struct ParserState*) hnd;
  struct Statement*   stmt  = (struct Statement*) proc;
  struct DeclaredVar* local = NULL;

  const uint_t paramsCount = wh_array_count (&(stmt->spec.proc.paramsList));

  assert (stmt->type == STMT_PROC);

  if (id < paramsCount)
    local = wh_array_get (&(stmt->spec.proc.paramsList), id);

  else
    local = get_var_from_stmt (stmt, id);


  if (local == NULL)
    return NULL;

  else
    return wh_ostream_data (&(state->globalStmt.spec.glb.typesDescs)) +
           local->typeSpecOff;
}


void
wh_compiler_libver (uint_t* const     outMajor,
                    uint_t* const     outMinor)
{
  *outMajor = 1;
  *outMinor = 0;
}


void
wh_compiler_language_ver (uint_t* const    outMajor,
                          uint_t* const    outMinor)
{
  *outMajor = 1;
  *outMinor = 0;
}

