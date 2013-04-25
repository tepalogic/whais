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
init_glbl_stmt (struct Statement* stmt)
{
  memset (stmt, 0, sizeof (*stmt));

  stmt->type = STMT_GLOBAL;

  wh_ostream_init (0, &(stmt->spec.glb.typesDescs));
  wh_ostream_init (0, &(stmt->spec.glb.constsArea));
  wh_array_init (&stmt->spec.glb.procsDecls, sizeof (struct Statement));
  wh_array_init (&stmt->decls, sizeof (struct DeclaredVar));

  return TRUE;
}


void
clear_glbl_stmt (struct Statement* stmt)
{
  uint_t procId = wh_array_count (&(stmt->spec.glb.procsDecls));

  assert (stmt->parent == NULL);
  assert (stmt->type == STMT_GLOBAL);

  while (procId-- > 0)
    {
      struct Statement* proc = wh_array_get (&(stmt->spec.glb.procsDecls),
                                             procId);
      clear_proc_stmt (proc);
    }

  wh_ostream_clean (&stmt->spec.glb.typesDescs);
  wh_ostream_clean (&stmt->spec.glb.constsArea);
  wh_array_clean (&(stmt->spec.glb.procsDecls));
  wh_array_clean (&(stmt->decls));

  stmt->type = STMT_ERR;

  return;
}


bool_t
init_proc_stmt (struct Statement* parent,
                struct Statement* outStmt)
{
  static struct DeclaredVar retType = { 0, };

  assert (parent != NULL);

  memset (outStmt, 0, sizeof (*outStmt));

  outStmt->type                  = STMT_PROC;
  outStmt->parent                = parent;
  outStmt->spec.proc.syncTracker = 0;

  wh_array_init (&outStmt->spec.proc.paramsList, sizeof (struct DeclaredVar));
  wh_array_init (&outStmt->decls, sizeof (struct DeclaredVar));
  wh_array_init (stmt_query_branch_stack (outStmt), sizeof (struct Branch));
  wh_array_init (stmt_query_loop_stack (outStmt), sizeof (struct Loop));

  wh_ostream_init (0, stmt_query_instrs (outStmt));

  /* Reserve space for return type */
  if (wh_array_add (&(outStmt->spec.proc.paramsList), &retType) == NULL)
    return FALSE;

  outStmt->localsUsed++;

  return TRUE;
}


void
clear_proc_stmt (struct Statement* stmt)
{
  assert (stmt->parent != NULL);
  assert (stmt->type == STMT_PROC);
  assert ((stmt->spec.proc.name != NULL) && (stmt->spec.proc.nameLength != 0));

  wh_array_clean (&(stmt->spec.proc.paramsList));
  wh_array_clean (&(stmt->decls));
  wh_array_clean (stmt_query_branch_stack (stmt));
  wh_array_clean (stmt_query_loop_stack (stmt));

  wh_ostream_clean (stmt_query_instrs (stmt));

  stmt->type = STMT_ERR;
}


struct DeclaredVar*
stmt_find_declaration (struct Statement* stmt,
                       const char*       name,
                       const uint_t      nameLength,
                       const bool_t      recursive,
                       const bool_t      reffered)
{
  struct DeclaredVar* result      = NULL;
  uint_t              i           = 0;
  uint_t              stored_vals = wh_array_count (&stmt->decls);

  while (i < stored_vals)
    {
      result = wh_array_get (&stmt->decls, i);

      assert (result != NULL);

      if ((IS_TABLE_FIELD (result->type) == FALSE) &&
          (nameLength == result->labelLength) &&
          (strncmp (name, result->label, nameLength) == 0))
        {
          if (reffered &&  ! IS_REFERRED (result->varId))
            {
              if (stmt->type == STMT_GLOBAL)
                {
                  assert (RETRIVE_ID (result->varId) == 0);
                  assert (IS_GLOBAL (result->varId));

                  result->varId |= stmt->localsUsed++;
                  MARK_AS_REFERENCED (result->varId);
                }
              else
                MARK_AS_REFERENCED (result->varId);
            }
          return result;
        }
      i++;
    }

  if (stmt->parent != NULL)
    {
      /* Let's check if is a parameter. */
      stored_vals = wh_array_count (&stmt->spec.proc.paramsList);

      /* index 0 is reserved to hold the return type */
      i = 1;
      while (i < stored_vals)
        {
          result = wh_array_get (&stmt->spec.proc.paramsList, i);
          assert (result != NULL);

          if ((IS_TABLE_FIELD (result->type) == FALSE) &&
              (nameLength == result->labelLength) &&
              (strncmp (name, result->label, nameLength) == 0))
            {
              if (reffered && ! IS_REFERRED (result->varId))
                {
                  assert (RETRIVE_ID (result->varId) != 0);
                  assert (IS_GLOBAL (result->varId) == FALSE);
                  assert (stmt->type = STMT_PROC);

                  MARK_AS_REFERENCED (result->varId);
                }

              return result;
            }
          i++;
        }

      /* Maybe is global */
      if (recursive)
        {
          return stmt_find_declaration (stmt->parent,
                                        name,
                                        nameLength,
                                        recursive,
                                        reffered);
        }
    }

  return NULL;
}


struct DeclaredVar*
stmt_add_declaration (struct Statement* const   stmt,
                      struct DeclaredVar*       var,
                      const bool_t              procPram)
{
  struct WOutputStream *typeStream = NULL;

  if (IS_TABLE_FIELD (var->type))
      var->varId = ~0; /* Set the id to a generic value! */
  else if (stmt->type == STMT_GLOBAL)
    {
      var->varId = 0; /* The real id will be assigned when is refered */

      MARK_AS_GLOBAL (var->varId);
      MARK_AS_NOT_REFERENCED (var->varId);

      typeStream = &(stmt->spec.glb.typesDescs);
    }
  else
    {
      assert (stmt->parent->type == STMT_GLOBAL);

      var->varId = stmt->localsUsed++;
      MARK_AS_NOT_REFERENCED (var->varId);

      typeStream = &(stmt->parent->spec.glb.typesDescs);
    }

  var->typeSpecOff = fill_type_spec (typeStream, var);

  if (procPram)
    {
      assert (stmt->parent != NULL);
      var = wh_array_add (&stmt->spec.proc.paramsList, var);
    }
  else
    var = wh_array_add (&stmt->decls, var);

  return (struct DeclaredVar*)var;
}


const struct DeclaredVar*
stmt_get_param (const struct Statement* const stmt, const uint_t param)
{
  assert (stmt->type == STMT_PROC);

  return (struct DeclaredVar*)
    wh_array_get (&stmt->spec.proc.paramsList, param);
}


uint_t
stmt_get_param_count (const struct Statement* const stmt)
{
  assert (stmt->type == STMT_PROC);

  return wh_array_count (&stmt->spec.proc.paramsList) - 1;
}

uint32_t
stmt_get_import_id (const struct Statement* const stmt)
{
  assert (stmt->type == STMT_PROC);

  return RETRIVE_ID (stmt->spec.proc.procId);
}

/*****************************Type specification section ***************/

bool_t
is_type_spec_valid (const struct TypeSpec* spec)
{
  bool_t result = TRUE;

  const uint16_t htype = load_le_int16 ((uint8_t*)&spec->type);
  const uint16_t hsize = load_le_int16 ((uint8_t*)&spec->dataSize);

  if (((htype == T_UNKNOWN) || (htype > T_UNDETERMINED))
      && (IS_FIELD (htype) == FALSE)
      && (IS_ARRAY (htype) == FALSE)
      && (IS_TABLE (htype) == FALSE))
    {
      result = FALSE;
    }
  else if ((spec->data[hsize - 2] != TYPE_SPEC_END_MARK)
           || (spec->data[hsize - 1] != 0))
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
          if ((GET_BASIC_TYPE (fieldType) == T_UNKNOWN)
              || (GET_BASIC_TYPE (fieldType) > T_UNDETERMINED))
            {
              result = FALSE;
            }
        }
      else
        {
          if ((GET_BASIC_TYPE (fieldType) == T_UNKNOWN)
              || (GET_BASIC_TYPE (fieldType) > T_UNDETERMINED))
            {
              result = FALSE;
            }
        }
    }
  else if (IS_ARRAY (htype))
    {
      if ((hsize != 2)
          || (GET_BASIC_TYPE (htype) == T_UNKNOWN)
          || (GET_BASIC_TYPE (htype) > T_UNDETERMINED) )
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
          uint_t   identifierLength = strlen ((char *) &spec->data[index]);

          /* Don't check for zero here, because of strlen () */
          index += identifierLength + 1;
          type   = ((uint16_t*)&(spec->data[index]))[0];

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
find_type_spec (const uint8_t*         typeBuff,
                uint32_t               typeBuffSize,
                const struct TypeSpec* spec)
{
  uint_t position = 0;

  const struct TypeSpec* it;

  while (position < typeBuffSize)
    {
      it = (struct TypeSpec*)(typeBuff + position);

      if ( ! is_type_spec_valid (it))
        {
          /* internal error */
          assert (0);
          return TYPE_SPEC_ERROR;
        }

      if (compare_type_spec (it, spec) != FALSE)
        return position;

      position += it->dataSize + 2 * sizeof (uint16_t);
    }

  return TYPE_SPEC_INVALID_POS;
}


bool_t
compare_type_spec (const struct TypeSpec* const spec1,
               const struct TypeSpec* const spec2)
{
  return (spec1->type == spec2->type)
          && (spec1->dataSize == spec2->dataSize)
          && (memcmp (&spec1->data[0], &spec2->data[0], spec1->dataSize) == 0);
}


static uint_t
type_spec_fill_table_field (struct WOutputStream* const typeStream,
                            const struct DeclaredVar*   fields)
{
  uint_t   result = 0;
  uint16_t leType;

  while (fields && IS_TABLE_FIELD (fields->type))
    {
      if ((wh_ostream_write (typeStream,
                        (uint8_t*)fields->label,
                        fields->labelLength) == NULL)
          || (wh_ostream_wint8 (typeStream, 0) == NULL))
        {
          result = TYPE_SPEC_ERROR;
          break;
        }

      result += fields->labelLength + 1;
      store_le_int16 (GET_TYPE (fields->type), (uint8_t*)&leType);
      if (wh_ostream_wint16 (typeStream, leType) == NULL)
        {
          result = TYPE_SPEC_ERROR;
          break;
        }

      result += sizeof (uint16_t), fields = fields->extra;
    }

  return result;
}


static uint_t
type_spec_fill_table (struct WOutputStream* const     typeStream,
                      const struct DeclaredVar* const var)
{
  uint_t   result  = 0;
  uint_t   specOff = wh_ostream_size (typeStream);
  uint16_t leType;

  assert (IS_TABLE (var->type));

  store_le_int16 (var->type, (uint8_t*)&leType);

  /* output the type and a dummy length to fill
   * after fields are output */
  if ((wh_ostream_wint16 (typeStream, leType) != NULL)
      && (wh_ostream_wint16 (typeStream, 0) != NULL))
    {
      result = type_spec_fill_table_field (typeStream, var->extra);
    }
  else
    result = TYPE_SPEC_ERROR;

  if ((result != TYPE_SPEC_ERROR)
      && (wh_ostream_wint8 (typeStream, TYPE_SPEC_END_MARK) != NULL)
      && (wh_ostream_wint8 (typeStream, 0) != NULL))
    {
      struct TypeSpec *ts = (struct TypeSpec*)
                            (wh_ostream_data (typeStream) + specOff);
      result += 2 * sizeof (uint8_t);
      store_le_int16 (result, (uint8_t*)&ts->dataSize);
    }
  else
    result = TYPE_SPEC_ERROR;

  return result;
}


static uint_t
type_spec_fill_array (struct WOutputStream* const     typeStream,
                      const struct DeclaredVar* const var)
{
  uint_t result = 0;

  struct TypeSpec spec;

  assert (IS_ARRAY (GET_TYPE (var->type)));

  store_le_int16 (GET_TYPE (var->type), (uint8_t*)&spec.type);
  store_le_int16 (2, (uint8_t*)&spec.dataSize);
  spec.data[0] = TYPE_SPEC_END_MARK;
  spec.data[1] = 0;

  if (wh_ostream_write (typeStream, (uint8_t *)&spec, sizeof spec) != NULL)
    result = sizeof spec;

  else
    result = TYPE_SPEC_ERROR;

  return result;
}


static uint_t
type_spec_fill_field (struct WOutputStream* const     typeStream,
                      const struct DeclaredVar* const var)
{
  uint_t result = 0;

  struct TypeSpec spec;

  assert (IS_FIELD (GET_TYPE (var->type)));

  store_le_int16 (GET_TYPE (var->type), (uint8_t*)&spec.type);
  store_le_int16 (2, (uint8_t*)&spec.dataSize);
  spec.data[0] = TYPE_SPEC_END_MARK;
  spec.data[1] = 0;

  if (wh_ostream_write (typeStream, (uint8_t *)&spec, sizeof spec) != NULL)
    result = sizeof spec;

  else
    result = TYPE_SPEC_ERROR;

  return result;
}


static uint_t
type_spec_fill_basic (struct WOutputStream* const      typeStream,
                      const struct DeclaredVar* const var)
{
  uint_t result = 0;

  struct TypeSpec spec;

  assert ((IS_ARRAY (var->varId) == FALSE)
          && (IS_TABLE (var->varId) == FALSE)
          && (IS_FIELD (var->varId) == FALSE));
  assert (var->type != T_UNKNOWN);
  assert (var->type <= T_TEXT);

  store_le_int16 (GET_BASIC_TYPE (var->type), (uint8_t*)&spec.type);
  store_le_int16 (2, (uint8_t*)&spec.dataSize);
  spec.data[0] = TYPE_SPEC_END_MARK;
  spec.data[1] = 0;


  if (wh_ostream_write (typeStream, (uint8_t *)&spec, sizeof spec) != NULL)
    result = sizeof spec;

  else
    result = TYPE_SPEC_ERROR;

  return result;
}


uint_t
fill_type_spec (struct WOutputStream* const     typeStream,
                const struct DeclaredVar* const var)
{

  uint_t result = 0;

  struct WOutputStream temporalStream;

  if (IS_TABLE_FIELD (var->type))
    {
      /* Field types are not present in the local/global
       * variable table of type descriptors */
      return 0;
    }

  wh_ostream_init (OUTSTREAM_INCREMENT_SIZE, &temporalStream);

  if ( ! (IS_FIELD (var->type)
          || IS_ARRAY (var->type)
          || IS_TABLE (var->type)))
    {
      result = type_spec_fill_basic (&temporalStream, var);
    }
  else if (IS_TABLE (var->type))
    result = type_spec_fill_table (&temporalStream, var);

  else if (IS_FIELD (var->type))
    result = type_spec_fill_field (&temporalStream, var);

  else if (IS_ARRAY (var->type))
    result = type_spec_fill_array (&temporalStream, var);

  else
    {
      assert (0);
      result = TYPE_SPEC_ERROR;
    }

  if ((result == TYPE_SPEC_ERROR) || (result == TYPE_SPEC_INVALID_POS))
    return result;

  else
    {
      result = find_type_spec (wh_ostream_data (typeStream),
                               wh_ostream_size (typeStream),
                               (struct TypeSpec*)
                               wh_ostream_data (&temporalStream));
    }

  if (result == TYPE_SPEC_INVALID_POS)
    {
      result = wh_ostream_size (typeStream);
      if (wh_ostream_write (typeStream,
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
add_constant_text (struct Statement* const stmt,
                const uint8_t* const       text,
                const uint_t               testSize)
{
  struct WOutputStream *stream  = (stmt->type == STMT_GLOBAL) ?
                                    &stmt->spec.glb.constsArea :
                                    &stmt->parent->spec.glb.constsArea;

  const uint8_t* streamBuff = wh_ostream_data (stream);
  const uint_t   streamSize  = wh_ostream_size (stream);

  int iterator;

  assert ((stmt->type == STMT_GLOBAL) || (stmt->type == STMT_PROC));

  for (iterator = 0; iterator + testSize <= streamSize; ++iterator)
    if ((memcmp (streamBuff + iterator, text, testSize) == 0) &&
        ((text[testSize - 1] == 0) || (streamBuff[iterator + testSize] == 0)))
      {
        return iterator;
      }

  if (wh_ostream_write (stream, text, testSize) == NULL)
    return -1;

  /* Add a ending 0 just in case. */
  if ((text[testSize - 1] != 0) && (wh_ostream_wint8 (stream, 0) == NULL))
    return -1;

  return streamSize;
}

