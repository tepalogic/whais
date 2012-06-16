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

#include "statement.h"

#include <assert.h>
#include <string.h>

#include "vardecl.h"
#include "brlo_stmts.h"

D_BOOL
init_glbl_stmt (struct Statement* pStmt)
{
  memset (pStmt, 0, sizeof (*pStmt));
  pStmt->type = STMT_GLOBAL;

  init_outstream (0, &(pStmt->spec.glb.typesDescs));
  init_outstream (0, &(pStmt->spec.glb.constsArea));
  init_array (&pStmt->spec.glb.procsDecls, sizeof (struct Statement));
  init_array (&pStmt->decls, sizeof (struct DeclaredVar));

  return TRUE;
}

void
clear_glbl_stmt (struct Statement* pGlbStmt)
{
  D_UINT procId = get_array_count (&(pGlbStmt->spec.glb.procsDecls));

  assert (pGlbStmt->pParentStmt == NULL);
  assert (pGlbStmt->type == STMT_GLOBAL);

  while (procId-- > 0)
    {
      struct Statement* pProc = get_item (&(pGlbStmt->spec.glb.procsDecls), procId);
      clear_proc_stmt (pProc);
    }

  destroy_outstream (&pGlbStmt->spec.glb.typesDescs);
  destroy_outstream (&pGlbStmt->spec.glb.constsArea);
  destroy_array (&(pGlbStmt->spec.glb.procsDecls));
  destroy_array (&(pGlbStmt->decls));

  pGlbStmt->type = STMT_ERR;

  return;
}

D_BOOL
init_proc_stmt (struct Statement* pParentStmt,
                struct Statement* pOutStmt)
{
  static struct DeclaredVar retType = { 0, };
  assert (pParentStmt != NULL);

  memset (pOutStmt, 0, sizeof (*pOutStmt));

  pOutStmt->type                  = STMT_PROC;
  pOutStmt->pParentStmt           = pParentStmt;
  pOutStmt->spec.proc.syncTracker = 0;

  init_array (&pOutStmt->spec.proc.paramsList, sizeof (struct DeclaredVar));
  init_array (&pOutStmt->decls, sizeof (struct DeclaredVar));
  init_array (stmt_query_branch_stack (pOutStmt), sizeof (struct Branch));
  init_array (stmt_query_loop_stack (pOutStmt), sizeof (struct Loop));

  init_outstream (0, stmt_query_instrs (pOutStmt));

  /* reserve space for return type */
  if (add_item (&(pOutStmt->spec.proc.paramsList), &retType) == NULL)
    return FALSE;

  pOutStmt->localsUsed++;

  return TRUE;
}

void
clear_proc_stmt (struct Statement* pStmt)
{
  assert (pStmt->pParentStmt != NULL);
  assert (pStmt->type == STMT_PROC);
  assert ((pStmt->spec.proc.name != NULL) && (pStmt->spec.proc.nameLength != 0));

  destroy_array (&(pStmt->spec.proc.paramsList));
  destroy_array (&(pStmt->decls));
  destroy_array (stmt_query_branch_stack (pStmt));
  destroy_array (stmt_query_loop_stack (pStmt));

  destroy_outstream (stmt_query_instrs (pStmt));

  pStmt->type = STMT_ERR;

  return;
}

struct DeclaredVar*
stmt_find_declaration (struct Statement* pStmt,
                       const char*       pName,
                       const D_UINT      nameLength,
                       const D_BOOL      recursive,
                       const D_BOOL      referenced)
{
  struct DeclaredVar* result      = NULL;
  D_UINT              count       = 0;
  D_UINT              stored_vals = get_array_count (&pStmt->decls);

  while (count < stored_vals)
    {
      result = get_item (&pStmt->decls, count);
      assert (result != NULL);

      if ((IS_TABLE_FIELD (result->type) == FALSE) &&
          (nameLength == result->labelLength) &&
          (strncmp (pName, result->label, nameLength) == 0))
        {
          if (referenced && (IS_REFERENCED (result->varId) == FALSE))
            {
              if (pStmt->type == STMT_GLOBAL)
                {
                  assert (RETRIVE_ID (result->varId) == 0);
                  assert (IS_GLOBAL (result->varId));

                  result->varId |= pStmt->localsUsed++;
                  MARK_AS_REFERENCED (result->varId);
                }
              else
                MARK_AS_REFERENCED (result->varId);
            }

          return result;
        }
      count++;
    }

  if (pStmt->pParentStmt != NULL)
    {
      /* let's check if is a parameter */
      stored_vals = get_array_count (&pStmt->spec.proc.paramsList);
      /* index 0 is reserved to hold the return type */
      count = 1;
      while (count < stored_vals)
        {
          result = get_item (&pStmt->spec.proc.paramsList, count);
          assert (result != NULL);

          if ((IS_TABLE_FIELD (result->type) == FALSE) &&
              (nameLength == result->labelLength) &&
              (strncmp (pName, result->label, nameLength) == 0))
            {
              if (referenced && (IS_REFERENCED (result->varId) == FALSE))
                {
                  assert (RETRIVE_ID (result->varId) != 0);
                  assert (IS_GLOBAL (result->varId) == FALSE);
                  assert (pStmt->type = STMT_PROC);

                  MARK_AS_REFERENCED (result->varId);
                }

              return result;
            }
          count++;
        }
      /* maybe is global */
      if (recursive)
        {
          return stmt_find_declaration (pStmt->pParentStmt,
                                        pName,
                                        nameLength,
                                        recursive,
                                        referenced);
        }
    }

  return NULL;
}

struct DeclaredVar*
stmt_add_declaration (struct Statement*   pStmt,
                      struct DeclaredVar* pVar,
                      D_BOOL              parameter)
{
  struct OutputStream *pOutStream = NULL;

  if (IS_TABLE_FIELD (pVar->type))
      pVar->varId = ~0; /* Set the id to a generic value! */
  else if (pStmt->type == STMT_GLOBAL)
    {
      pVar->varId = 0; /* The real id will be assigned when is refered */

      MARK_AS_GLOBAL (pVar->varId);
      MARK_AS_NOT_REFERENCED (pVar->varId);

      pOutStream   = &(pStmt->spec.glb.typesDescs);
    }
  else
    {
      assert (pStmt->pParentStmt->type == STMT_GLOBAL);

      pVar->varId = pStmt->localsUsed++;
      MARK_AS_NOT_REFERENCED (pVar->varId);

      pOutStream  = &(pStmt->pParentStmt->spec.glb.typesDescs);
    }

  pVar->typeSpecOff = type_spec_fill (pOutStream, pVar);

  if (parameter)
    {
      assert (pStmt->pParentStmt != NULL);
      pVar = add_item (&pStmt->spec.proc.paramsList, pVar);
    }
  else
    pVar = add_item (&pStmt->decls, pVar);

  return (struct DeclaredVar*)pVar;
}

const struct DeclaredVar*
stmt_get_param (const struct Statement* const pStmt, D_UINT param)
{
  assert (pStmt->type == STMT_PROC);

  return (struct DeclaredVar *)
    get_item (&pStmt->spec.proc.paramsList, param);
}

D_UINT
stmt_get_param_count (const struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);

  return get_array_count (&pStmt->spec.proc.paramsList) - 1;
}

D_UINT32
stmt_get_import_id (const struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);

  return RETRIVE_ID (pStmt->spec.proc.procId);
}

/*****************************Type specification section ***************/

D_BOOL
is_type_spec_valid (const struct TypeSpec* pType)
{
  D_BOOL result = TRUE;

  if (((pType->type == T_UNKNOWN) || (pType->type > T_UNDETERMINED)) &&
      (IS_FIELD (pType->type) == FALSE) &&
      (IS_ARRAY (pType->type) == FALSE) &&
      (IS_TABLE (pType->type) == FALSE))
    {
      result = FALSE;
    }
  else if ((pType->data[pType->dataSize - 2] != TYPE_SPEC_END_MARK) ||
           (pType->data[pType->dataSize - 1] != 0))
    {
      result = FALSE;
    }
  else if (IS_FIELD (pType->type))
    {
      const D_UINT16 fieldType = GET_FIELD_TYPE (pType->type);
      if (pType->dataSize != 2)
        result = FALSE;
      else if (IS_ARRAY (fieldType))
        {
          if (GET_BASIC_TYPE (fieldType) == T_UNKNOWN ||
              GET_BASIC_TYPE (fieldType) >= T_UNDETERMINED)
            {
              result = FALSE;
            }
        }
      else
        {
          if (GET_BASIC_TYPE (fieldType) == T_UNKNOWN ||
              GET_BASIC_TYPE (fieldType) > T_UNDETERMINED)
            {
              result = FALSE;
            }
        }
    }
  else if (IS_ARRAY (pType->type))
    {
      if ( (pType->dataSize != 2) ||
           (GET_BASIC_TYPE (pType->type) == T_UNKNOWN) ||
           (GET_BASIC_TYPE (pType->type) > T_UNDETERMINED) )
        {
          result = FALSE;
        }
    }
  else if (IS_TABLE (pType->type))
    {
      D_UINT index = 0;

      while ((index < (D_UINT) (pType->dataSize - 2)) && (result != FALSE))
        {
          D_UINT16 type;
          D_UINT   identifierLength = strlen ((char *) &pType->data[index]);

          /* don't check for zero here, because of strlen */
          index += identifierLength + 1;
          type  = ((D_UINT16 *) & (pType->data[index]))[0];

          /* Ignore an eventual array mask */
          if ( (GET_BASIC_TYPE (type) == T_UNKNOWN) ||
               (GET_BASIC_TYPE (type) > T_UNDETERMINED))
            {
              result = FALSE;
              break;
            }
          index += 2;
        }
    }

  return result;
}

static D_UINT32
find_type_spec (const D_UINT8*         pTypeBuffer,
                D_UINT32               typeBufferSize,
                const struct TypeSpec* pSpec)
{
  D_UINT                 position = 0;
  const struct TypeSpec* pIterator;

  while (position < typeBufferSize)
    {
      pIterator = (struct TypeSpec*)(pTypeBuffer + position);

      if ( ! is_type_spec_valid (pIterator))
        {
          /* internal error */
          assert (0);
          return TYPE_SPEC_ERROR;
        }

      if (type_spec_cmp (pIterator, pSpec) != FALSE)
        return position;

      position += pIterator->dataSize + 2 * sizeof (D_UINT16);
    }

  return TYPE_SPEC_INVALID_POS;
}

D_BOOL
type_spec_cmp (const struct TypeSpec* const pSpec_1,
               const struct TypeSpec* const pSpec_2)
{
  return (pSpec_1->type == pSpec_2->type) &&
         (pSpec_1->dataSize == pSpec_2->dataSize) &&
         (memcmp (&pSpec_1->data[0],
                  &pSpec_2->data[0],
                  pSpec_1->dataSize) == 0);
}

static D_UINT
type_spec_fill_table_field (struct OutputStream* const pStream,
                            const struct DeclaredVar*  pFieldList)
{
  D_UINT result = 0;

  while (pFieldList && IS_TABLE_FIELD (pFieldList->type))
    {
      if ((output_data (pStream,
                        (D_UINT8*)pFieldList->label,
                        pFieldList->labelLength) == NULL) ||
          (output_uint8 (pStream, 0) == NULL))
        {
          result = TYPE_SPEC_ERROR;
          break;
        }

      result += pFieldList->labelLength + 1;
      if (output_uint16 (pStream, GET_BASIC_TYPE (pFieldList->type)) == NULL)
        {
          result = TYPE_SPEC_ERROR;
          break;
        }
      result     += 2;
      pFieldList = pFieldList->extra;
    }

  return result;
}

static D_UINT
type_spec_fill_table (struct OutputStream* const     pStream,
                      const struct DeclaredVar*const pVar)
{
  D_UINT result  = 0;
  D_UINT specOff = get_size_outstream (pStream);

  assert (IS_TABLE (pVar->type));

  /* output the type and a dummy length to fill
   * after fields are output */
  if ((output_uint16 (pStream, pVar->type) != NULL) &&
      (output_uint16 (pStream, 0) != NULL))
    {
      result = type_spec_fill_table_field (pStream, pVar->extra);
    }
  else
    result = TYPE_SPEC_ERROR;

  if ((result != TYPE_SPEC_ERROR) &&
      (output_uint8 (pStream, TYPE_SPEC_END_MARK) != NULL) &&
      (output_uint8 (pStream, 0) != NULL))
    {
      struct TypeSpec *ts = (struct TypeSpec*)(get_buffer_outstream (pStream) + specOff);

      result       += 2;
      ts->dataSize  = result;
    }
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

static D_UINT
type_spec_fill_array (struct OutputStream* const      pStream,
                      const struct DeclaredVar* const pVar)
{
  D_UINT          result = 0;
  struct TypeSpec spec;

  assert (IS_ARRAY (pVar->type));

  spec.type     = pVar->type;
  spec.dataSize = 2;
  spec.data[0]  = TYPE_SPEC_END_MARK;
  spec.data[1]  = 0;

  if ((output_uint16 (pStream, spec.type) != NULL) &&
      (output_uint16 (pStream, spec.dataSize) != NULL) &&
      (output_data (pStream, spec.data, sizeof spec.data) != NULL))
    {
      result = 2 * sizeof  (D_UINT16) + sizeof (spec.data);
    }
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

static D_UINT
type_spec_fill_field (struct OutputStream* const      pStream,
                      const struct DeclaredVar* const pVar)
{
  D_UINT          result = 0;
  struct TypeSpec spec;

  assert (IS_FIELD (pVar->type));

  spec.type     = pVar->type;
  spec.dataSize = 2;
  spec.data[0]  = TYPE_SPEC_END_MARK;
  spec.data[1]  = 0;

  if ((output_uint16 (pStream, spec.type) != NULL) &&
      (output_uint16 (pStream, spec.dataSize) != NULL) &&
      (output_data (pStream, spec.data, sizeof spec.data) != NULL))
    {
      result = 2 * sizeof  (D_UINT16) + sizeof (spec.data);
    }
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

static D_UINT
type_spec_fill_basic (struct OutputStream* const      pStream,
                      const struct DeclaredVar* const pVar)
{
  D_UINT          result = 0;
  struct TypeSpec spec;

  assert ((IS_ARRAY (pVar->varId) || IS_TABLE (pVar->varId)) == FALSE);
  assert (pVar->type != T_UNKNOWN);
  assert (pVar->type <= T_UINT64);

  spec.type     = pVar->type;
  spec.dataSize = 2;
  spec.data[0]  = TYPE_SPEC_END_MARK;
  spec.data[1]  = 0;

  if (output_data (pStream, (D_UINT8 *) & spec, sizeof spec) != NULL)
    result = sizeof spec;
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

D_UINT
type_spec_fill (struct OutputStream* const      pStream,
                const struct DeclaredVar* const pVar)
{

  D_UINT           result = 0;
  struct OutputStream temporalStream;

  if (IS_TABLE_FIELD (pVar->type))
    {
      /* Field types are not present in the local/global
       * variable table of type descriptors */
      return 0;
    }

  init_outstream (OUTSTREAM_INCREMENT_SIZE, &temporalStream);

  if ( (IS_FIELD (pVar->type) || IS_ARRAY (pVar->type) || IS_TABLE (pVar->type)) == FALSE)
    result = type_spec_fill_basic (&temporalStream, pVar);
  else if (IS_TABLE (pVar->type))
    result = type_spec_fill_table (&temporalStream, pVar);
  else if (IS_FIELD (pVar->type))
    result = type_spec_fill_field (&temporalStream, pVar);
  else if (IS_ARRAY (pVar->type))
    result = type_spec_fill_array (&temporalStream, pVar);
  else
    {
      assert (0);
      result = TYPE_SPEC_ERROR;
    }

  if ((result == TYPE_SPEC_ERROR) || (result == TYPE_SPEC_INVALID_POS))
    return result;
  else
    {
      result = find_type_spec (get_buffer_outstream (pStream),
                               get_size_outstream (pStream),
                               (struct TypeSpec*)
                               get_buffer_outstream (&temporalStream));
    }

  if (result == TYPE_SPEC_INVALID_POS)
    {
      result = get_size_outstream (pStream);
      if (output_data (pStream,
                          get_buffer_outstream (&temporalStream),
                          get_size_outstream (&temporalStream)) == NULL)
        {
          return TYPE_SPEC_INVALID_POS;
        }
    }

  destroy_outstream (&temporalStream);

  return result;
}

D_INT
add_text_const (struct Statement* const pStmt,
                const D_UINT8* const    pText,
                const D_UINT            testSize)
{
  struct OutputStream *pStream  = (pStmt->type == STMT_GLOBAL) ?
                                   &pStmt->spec.glb.constsArea :
                                   &pStmt->pParentStmt->spec.glb.constsArea;
  const D_UINT8* pStreamBuff = get_buffer_outstream (pStream);
  const D_UINT   streamSize  = get_size_outstream (pStream);
  D_INT          iterator;

  assert ((pStmt->type == STMT_GLOBAL) || (pStmt->type == STMT_PROC));

  for (iterator = 0; iterator + testSize <= streamSize; ++iterator)
    if ((memcmp (pStreamBuff + iterator, pText, testSize) == 0) &&
        ((pText[testSize - 1] == 0) || (pStreamBuff[iterator + testSize] == 0)))
      {
        return iterator;
      }

  if (output_data (pStream, pText, testSize) == NULL)
    return -1;

  /* make sure you add a zero just in case */
  if ((pText[testSize - 1] != 0) && (output_uint8 (pStream, 0) == NULL))
    return -1;

  return streamSize;
}
