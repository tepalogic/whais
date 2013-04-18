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

#include "utils/le_converter.h"

#include "vardecl.h"
#include "brlo_stmts.h"

bool_t
init_glbl_stmt (struct Statement* pStmt)
{
  memset (pStmt, 0, sizeof (*pStmt));
  pStmt->type = STMT_GLOBAL;

  wh_ostream_init (0, &(pStmt->spec.glb.typesDescs));
  wh_ostream_init (0, &(pStmt->spec.glb.constsArea));
  wh_array_init (&pStmt->spec.glb.procsDecls, sizeof (struct Statement));
  wh_array_init (&pStmt->decls, sizeof (struct DeclaredVar));

  return TRUE;
}

void
clear_glbl_stmt (struct Statement* pGlbStmt)
{
  uint_t procId = wh_array_count (&(pGlbStmt->spec.glb.procsDecls));

  assert (pGlbStmt->pParentStmt == NULL);
  assert (pGlbStmt->type == STMT_GLOBAL);

  while (procId-- > 0)
    {
      struct Statement* pProc = wh_array_get (&(pGlbStmt->spec.glb.procsDecls), procId);
      clear_proc_stmt (pProc);
    }

  wh_ostream_clean (&pGlbStmt->spec.glb.typesDescs);
  wh_ostream_clean (&pGlbStmt->spec.glb.constsArea);
  wh_array_clean (&(pGlbStmt->spec.glb.procsDecls));
  wh_array_clean (&(pGlbStmt->decls));

  pGlbStmt->type = STMT_ERR;

  return;
}

bool_t
init_proc_stmt (struct Statement* pParentStmt,
                struct Statement* pOutStmt)
{
  static struct DeclaredVar retType = { 0, };
  assert (pParentStmt != NULL);

  memset (pOutStmt, 0, sizeof (*pOutStmt));

  pOutStmt->type                  = STMT_PROC;
  pOutStmt->pParentStmt           = pParentStmt;
  pOutStmt->spec.proc.syncTracker = 0;

  wh_array_init (&pOutStmt->spec.proc.paramsList, sizeof (struct DeclaredVar));
  wh_array_init (&pOutStmt->decls, sizeof (struct DeclaredVar));
  wh_array_init (stmt_query_branch_stack (pOutStmt), sizeof (struct Branch));
  wh_array_init (stmt_query_loop_stack (pOutStmt), sizeof (struct Loop));

  wh_ostream_init (0, stmt_query_instrs (pOutStmt));

  /* reserve space for return type */
  if (wh_array_add (&(pOutStmt->spec.proc.paramsList), &retType) == NULL)
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

  wh_array_clean (&(pStmt->spec.proc.paramsList));
  wh_array_clean (&(pStmt->decls));
  wh_array_clean (stmt_query_branch_stack (pStmt));
  wh_array_clean (stmt_query_loop_stack (pStmt));

  wh_ostream_clean (stmt_query_instrs (pStmt));

  pStmt->type = STMT_ERR;

  return;
}

struct DeclaredVar*
stmt_find_declaration (struct Statement* pStmt,
                       const char*       pName,
                       const uint_t      nameLength,
                       const bool_t      recursive,
                       const bool_t      referenced)
{
  struct DeclaredVar* result      = NULL;
  uint_t              count       = 0;
  uint_t              stored_vals = wh_array_count (&pStmt->decls);

  while (count < stored_vals)
    {
      result = wh_array_get (&pStmt->decls, count);
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
      stored_vals = wh_array_count (&pStmt->spec.proc.paramsList);
      /* index 0 is reserved to hold the return type */
      count = 1;
      while (count < stored_vals)
        {
          result = wh_array_get (&pStmt->spec.proc.paramsList, count);
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
stmt_add_declaration (struct Statement* const   pStmt,
                      struct DeclaredVar*       pVar,
                      const bool_t              parameter)
{
  struct WOutputStream *pOutStream = NULL;

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
      pVar = wh_array_add (&pStmt->spec.proc.paramsList, pVar);
    }
  else
    pVar = wh_array_add (&pStmt->decls, pVar);

  return (struct DeclaredVar*)pVar;
}

const struct DeclaredVar*
stmt_get_param (const struct Statement* const pStmt, uint_t param)
{
  assert (pStmt->type == STMT_PROC);

  return (struct DeclaredVar *)
    wh_array_get (&pStmt->spec.proc.paramsList, param);
}

uint_t
stmt_get_param_count (const struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);

  return wh_array_count (&pStmt->spec.proc.paramsList) - 1;
}

uint32_t
stmt_get_import_id (const struct Statement* const pStmt)
{
  assert (pStmt->type == STMT_PROC);

  return RETRIVE_ID (pStmt->spec.proc.procId);
}

/*****************************Type specification section ***************/

bool_t
is_type_spec_valid (const struct TypeSpec* pType)
{
  bool_t result = TRUE;

  const uint16_t htype = load_le_int16 ((uint8_t*)&pType->type);
  const uint16_t hsize = load_le_int16 ((uint8_t*)&pType->dataSize);

  if (((htype == T_UNKNOWN) || (htype > T_UNDETERMINED)) &&
      (IS_FIELD (htype) == FALSE) &&
      (IS_ARRAY (htype) == FALSE) &&
      (IS_TABLE (htype) == FALSE))
    {
      result = FALSE;
    }
  else if ((pType->data[hsize - 2] != TYPE_SPEC_END_MARK) ||
           (pType->data[hsize - 1] != 0))
    {
      result = FALSE;
    }
  else if (IS_FIELD (htype))
    {
      const uint16_t fieldType = GET_FIELD_TYPE (htype);
      if (hsize != 2)
        result = FALSE;
      else if (IS_ARRAY (fieldType))
        {
          if (GET_BASIC_TYPE (fieldType) == T_UNKNOWN ||
              GET_BASIC_TYPE (fieldType) > T_UNDETERMINED)
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
  else if (IS_ARRAY (htype))
    {
      if ( (hsize != 2) ||
           (GET_BASIC_TYPE (htype) == T_UNKNOWN) ||
           (GET_BASIC_TYPE (htype) > T_UNDETERMINED) )
        {
          result = FALSE;
        }
    }
  else if (IS_TABLE (htype))
    {
      uint_t index = 0;

      while ((index < (uint_t) (hsize - 2)) && (result != FALSE))
        {
          uint16_t type;
          uint_t   identifierLength = strlen ((char *) &pType->data[index]);

          /* don't check for zero here, because of strlen */
          index += identifierLength + 1;
          type  = ((uint16_t *) & (pType->data[index]))[0];

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

static uint32_t
find_type_spec (const uint8_t*         pTypeBuffer,
                uint32_t               typeBufferSize,
                const struct TypeSpec* pSpec)
{
  uint_t                 position = 0;
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

      position += pIterator->dataSize + 2 * sizeof (uint16_t);
    }

  return TYPE_SPEC_INVALID_POS;
}

bool_t
type_spec_cmp (const struct TypeSpec* const pSpec_1,
               const struct TypeSpec* const pSpec_2)
{
  return (pSpec_1->type == pSpec_2->type) &&
         (pSpec_1->dataSize == pSpec_2->dataSize) &&
         (memcmp (&pSpec_1->data[0],
                  &pSpec_2->data[0],
                  pSpec_1->dataSize) == 0);
}

static uint_t
type_spec_fill_table_field (struct WOutputStream* const pStream,
                            const struct DeclaredVar*  pFieldList)
{
  uint_t   result = 0;
  uint16_t le_type;

  while (pFieldList && IS_TABLE_FIELD (pFieldList->type))
    {
      if ((wh_ostream_write (pStream,
                        (uint8_t*)pFieldList->label,
                        pFieldList->labelLength) == NULL) ||
          (wh_ostream_wint8 (pStream, 0) == NULL))
        {
          result = TYPE_SPEC_ERROR;
          break;
        }

      result += pFieldList->labelLength + 1;
      store_le_int16 (GET_TYPE (pFieldList->type), (uint8_t*)&le_type);
      if (wh_ostream_wint16 (pStream, le_type) == NULL)
        {
          result = TYPE_SPEC_ERROR;
          break;
        }

      result     += sizeof (uint16_t);
      pFieldList  = pFieldList->extra;
    }

  return result;
}

static uint_t
type_spec_fill_table (struct WOutputStream* const     pStream,
                      const struct DeclaredVar*const pVar)
{
  uint_t   result  = 0;
  uint_t   specOff = wh_ostream_size (pStream);
  uint16_t le_type;

  assert (IS_TABLE (pVar->type));

  store_le_int16 (pVar->type, (uint8_t*)&le_type);

  /* output the type and a dummy length to fill
   * after fields are output */
  if ((wh_ostream_wint16 (pStream, le_type) != NULL) &&
      (wh_ostream_wint16 (pStream, 0) != NULL))
    {
      result = type_spec_fill_table_field (pStream, pVar->extra);
    }
  else
    result = TYPE_SPEC_ERROR;

  if ((result != TYPE_SPEC_ERROR) &&
      (wh_ostream_wint8 (pStream, TYPE_SPEC_END_MARK) != NULL) &&
      (wh_ostream_wint8 (pStream, 0) != NULL))
    {
      struct TypeSpec *ts = (struct TypeSpec*)
                            (wh_ostream_data (pStream) + specOff);
      result += 2 * sizeof (uint8_t);
      store_le_int16 (result, (uint8_t*)&ts->dataSize);
    }
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

static uint_t
type_spec_fill_array (struct WOutputStream* const      pStream,
                      const struct DeclaredVar* const pVar)
{
  uint_t          result = 0;
  struct TypeSpec spec;

  assert (IS_ARRAY (GET_TYPE (pVar->type)));

  store_le_int16 (GET_TYPE (pVar->type), (uint8_t*)&spec.type);
  store_le_int16 (2, (uint8_t*)&spec.dataSize);
  spec.data[0] = TYPE_SPEC_END_MARK;
  spec.data[1] = 0;

  if (wh_ostream_write (pStream, (uint8_t *)&spec, sizeof spec) != NULL)
    result = sizeof spec;
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

static uint_t
type_spec_fill_field (struct WOutputStream* const      pStream,
                      const struct DeclaredVar* const pVar)
{
  uint_t          result = 0;
  struct TypeSpec spec;

  assert (IS_FIELD (GET_TYPE (pVar->type)));

  store_le_int16 (GET_TYPE (pVar->type), (uint8_t*)&spec.type);
  store_le_int16 (2, (uint8_t*)&spec.dataSize);
  spec.data[0] = TYPE_SPEC_END_MARK;
  spec.data[1] = 0;

  if (wh_ostream_write (pStream, (uint8_t *)&spec, sizeof spec) != NULL)
    result = sizeof spec;
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

static uint_t
type_spec_fill_basic (struct WOutputStream* const      pStream,
                      const struct DeclaredVar* const pVar)
{
  uint_t          result = 0;
  struct TypeSpec spec;

  assert ((IS_ARRAY (pVar->varId) == FALSE)
          && (IS_TABLE (pVar->varId) == FALSE)
          && (IS_FIELD (pVar->varId) == FALSE));
  assert (pVar->type != T_UNKNOWN);
  assert (pVar->type <= T_TEXT);

  store_le_int16 (GET_BASIC_TYPE (pVar->type), (uint8_t*)&spec.type);
  store_le_int16 (2, (uint8_t*)&spec.dataSize);
  spec.data[0] = TYPE_SPEC_END_MARK;
  spec.data[1] = 0;


  if (wh_ostream_write (pStream, (uint8_t *)&spec, sizeof spec) != NULL)
    result = sizeof spec;
  else
    result = TYPE_SPEC_ERROR;

  return result;
}

uint_t
type_spec_fill (struct WOutputStream* const      pStream,
                const struct DeclaredVar* const pVar)
{

  uint_t              result = 0;
  struct WOutputStream temporalStream;

  if (IS_TABLE_FIELD (pVar->type))
    {
      /* Field types are not present in the local/global
       * variable table of type descriptors */
      return 0;
    }

  wh_ostream_init (OUTSTREAM_INCREMENT_SIZE, &temporalStream);

  if ( ! (IS_FIELD (pVar->type)
          || IS_ARRAY (pVar->type)
          || IS_TABLE (pVar->type)))
    {
      result = type_spec_fill_basic (&temporalStream, pVar);
    }
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
      result = find_type_spec (wh_ostream_data (pStream),
                               wh_ostream_size (pStream),
                               (struct TypeSpec*)
                               wh_ostream_data (&temporalStream));
    }

  if (result == TYPE_SPEC_INVALID_POS)
    {
      result = wh_ostream_size (pStream);
      if (wh_ostream_write (pStream,
                          wh_ostream_data (&temporalStream),
                          wh_ostream_size (&temporalStream)) == NULL)
        {
          return TYPE_SPEC_INVALID_POS;
        }
    }

  wh_ostream_clean (&temporalStream);

  return result;
}

int
add_text_const (struct Statement* const pStmt,
                const uint8_t* const    pText,
                const uint_t            testSize)
{
  struct WOutputStream *pStream  = (pStmt->type == STMT_GLOBAL) ?
                                   &pStmt->spec.glb.constsArea :
                                   &pStmt->pParentStmt->spec.glb.constsArea;
  const uint8_t* pStreamBuff = wh_ostream_data (pStream);
  const uint_t   streamSize  = wh_ostream_size (pStream);
  int          iterator;

  assert ((pStmt->type == STMT_GLOBAL) || (pStmt->type == STMT_PROC));

  for (iterator = 0; iterator + testSize <= streamSize; ++iterator)
    if ((memcmp (pStreamBuff + iterator, pText, testSize) == 0) &&
        ((pText[testSize - 1] == 0) || (pStreamBuff[iterator + testSize] == 0)))
      {
        return iterator;
      }

  if (wh_ostream_write (pStream, pText, testSize) == NULL)
    return -1;

  /* make sure you add a zero just in case */
  if ((pText[testSize - 1] != 0) && (wh_ostream_wint8 (pStream, 0) == NULL))
    return -1;

  return streamSize;
}
