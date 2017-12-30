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
 * table_stmts.c - Implements the semantics for whais programs statements.
 */


#include "table_stmts.h"

#include "utils/include/warray.h"
#include "utils/include/msglog.h"

#include "wlog.h"
#include "opcodes.h"
#include "expression.h"


static void
dump_table_type (struct ParserState* const parser,
                 const struct SemValue* const table,
                 const uint_t message)
{
  assert (table->val_type == VAL_TYPE_SPEC);
  assert (IS_TABLE(table->val.u_tspec.type));
  assert (message == MSG_ROW_COPY_LTABLE_TYPE || message == MSG_ROW_COPY_RTABLE_TYPE);

  struct ExpResultType srcType = {.type = table->val.u_tspec.type,
                                  .extra = table->val.u_tspec.extra};


  char* const srcText = get_type_description(&srcType);

  if (srcText != NULL)
    log_message(parser, IGNORE_BUFFER_POS, message, srcText);

  else
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

  free_type_description(srcText);
}


static void
dump_tables (struct ParserState* const parser,
             const struct SemValue* const dstTable,
             const struct SemValue* const srcTable)
{
  assert (dstTable->val_type == VAL_TYPE_SPEC);
  assert (srcTable->val_type == VAL_TYPE_SPEC);

  assert (IS_TABLE(dstTable->val.u_tspec.type));
  assert (IS_TABLE(srcTable->val.u_tspec.type));

  struct ExpResultType srcType = {.type = srcTable->val.u_tspec.type,
                                  .extra = srcTable->val.u_tspec.extra};

  struct ExpResultType dstType = {.type = dstTable->val.u_tspec.type,
                                  .extra = dstTable->val.u_tspec.extra};

  char* const srcTypeText = get_type_description(&srcType);
  char* const dstTypeText = get_type_description(&dstType);

  if ((srcTypeText != NULL) && (dstTypeText != NULL))
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_ROW_COPY_LTABLE_TYPE, dstTypeText);
    log_message(parser, IGNORE_BUFFER_POS, MSG_ROW_COPY_RTABLE_TYPE, srcTypeText);
  }
  else
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

  free_type_description(srcTypeText);
  free_type_description(dstTypeText);
}


static bool_t
get_field_list_from_ids(struct ParserState* const parser,
                        const struct SemIdList* const list,
                        const struct SemValue* const table,
                        const bool_t leftSide,
                        struct WArray *const result)
{
  const struct SemIdList* idIt = list;

  assert (wh_array_count(result) == 0);
  assert (table->val_type == VAL_TYPE_SPEC);
  assert (IS_TABLE(table->val.u_tspec.type));

  while (idIt != NULL)
  {
    const struct DeclaredVar* fieldIt = (struct DeclaredVar*)table->val.u_tspec.extra;
    while (fieldIt != NULL && IS_TABLE_FIELD(fieldIt->type))
    {
      if (fieldIt->labelLength == idIt->id.length
          && strncmp(fieldIt->label, idIt->id.name, fieldIt->labelLength) == 0)
      {
        if (wh_array_add(result, &fieldIt) == NULL)
        {
          log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
          return FALSE;
        }
        break;
      }
      fieldIt = fieldIt->extra;
    }

    if (fieldIt == NULL || ! IS_TABLE_FIELD(fieldIt->type))
    {
      char tname[128];
      wh_copy_first(tname, idIt->id.name, sizeof tname, idIt->id.length);
      log_message(parser,
                  parser->bufferPos,
                  leftSide ? MSG_ROW_COPY_NOFIELD_L : MSG_ROW_COPY_NOFIELD_R,
                  tname);
      dump_table_type(parser,
                      table,
                      leftSide ? MSG_ROW_COPY_LTABLE_TYPE : MSG_ROW_COPY_RTABLE_TYPE);
      return FALSE;
    }

    if (idIt->next == NULL)
      idIt = NULL;

    else
    {
      assert (idIt->next->val_type == VAL_ID_LIST);
      idIt = &idIt->next->val.u_idlist;
    }
  }

  return TRUE;
}

static bool_t
get_field_list_from_left_selection(struct ParserState* const parser,
                                   const struct SemValue* const table,
                                   const bool_t fixMissingFields,
                                   struct WArray *const leftList,
                                   struct WArray *const result)
{
  uint_t i;

  assert (table->val_type == VAL_TYPE_SPEC);
  assert (wh_array_count(result) == 0);

  for (i = 0; i < wh_array_count(leftList); ++i)
  {
    const struct DeclaredVar* srcField = *(struct DeclaredVar**)wh_array_get(leftList, i);
    const struct DeclaredVar* fieldIt = (struct DeclaredVar*)table->val.u_tspec.extra;

    while (fieldIt && IS_TABLE_FIELD(fieldIt->type))
    {
      if ((srcField->labelLength == fieldIt->labelLength)
          && (strncmp(srcField->label, fieldIt->label, srcField->labelLength) == 0))
      {
        break;
      }
      fieldIt = fieldIt->extra;
    }

    if ((fieldIt == NULL) || ! IS_TABLE_FIELD(fieldIt->type))
    {
      if (fixMissingFields)
      {
        int j;
        for (j = i + 1; j < wh_array_count(leftList); ++j)
        {
          *(struct DeclaredVar**)wh_array_get(leftList, j - 1) =
              *(struct DeclaredVar**)wh_array_get(leftList, j);
        }
        wh_array_resize(leftList, wh_array_count(leftList) - 1);
        --i;
        continue;
      }
      else
      {
        char tname[128];

        wh_copy_first(tname, srcField->label, sizeof tname, srcField->labelLength);
        log_message(parser, parser->bufferPos, MSG_ROW_COPY_NOFIELD_R, tname);
        dump_table_type(parser, table, MSG_ROW_COPY_RTABLE_TYPE);

        return FALSE;
      }
    }
    else if (wh_array_add(result, &fieldIt) == NULL)
    {
      log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return FALSE;
    }
  }

  return TRUE;
}


static bool_t
get_table_fields(struct ParserState* const parser,
                 const struct SemValue* const table,
                 struct WArray *const result)
{
  const struct DeclaredVar* fieldIt = (const struct DeclaredVar*)table->val.u_tspec.extra;

  assert (table->val_type == VAL_TYPE_SPEC);
  assert (wh_array_count(result) == 0);

  if ( ! IS_TABLE(table->val.u_tspec.type))
  {
    log_message(parser,
                parser->bufferPos,
                MSG_ROW_COPY_NOTABLE_L,
                type_to_text(table->val.u_tspec.type));

    return FALSE;
  }

  while (fieldIt && IS_TABLE_FIELD(fieldIt->type))
  {
    const struct DeclaredVar* next = fieldIt->extra;
    if (wh_array_add(result, &fieldIt) == NULL)
    {
      log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return FALSE;
    }
    fieldIt = next;
  }

  return TRUE;
}


static int
translate_all_expression (struct ParserState* const parser,
                          struct SemValue* leftIndex,
                          struct SemValue* rightIndex,
                          struct SemValue** ioLeftTable,
                          struct SemValue** ioRightTable)
{
  assert ((*ioLeftTable)->val_type == VAL_EXP_LINK);
  assert (leftIndex->val_type == VAL_EXP_LINK);
  assert ((*ioRightTable)->val_type == VAL_EXP_LINK);
  assert (rightIndex->val_type == VAL_EXP_LINK);

  *ioLeftTable = translate_exp(parser, *ioLeftTable, FALSE, TRUE);
  if ((*ioLeftTable)->val.u_tspec.type == T_UNKNOWN)
  {
    assert (parser->abortError != FALSE);
    return -1;
  }
  else if (! IS_TABLE((*ioLeftTable)->val.u_tspec.type))
  {
    log_message(parser,
                parser->bufferPos,
                MSG_ROW_COPY_NOTABLE_L,
                type_to_text((*ioLeftTable)->val.u_tspec.type));
    return -1;
  }

  leftIndex = translate_exp(parser, leftIndex, FALSE, TRUE);
  if (leftIndex->val.u_tspec.type == T_UNKNOWN)
  {
    assert (parser->abortError != FALSE);
    return -1;
  }
  else if ( ! is_integer(leftIndex->val.u_tspec.type))
  {
    log_message(parser,
                parser->bufferPos,
                MSG_ROW_COPY_NOINDEX_L,
                type_to_text(leftIndex->val.u_tspec.type));
    return -1;
  }

  *ioRightTable = translate_exp(parser, *ioRightTable, FALSE, TRUE);
  if ((*ioRightTable)->val.u_tspec.type == T_UNKNOWN)
  {
    assert (parser->abortError != FALSE);
    return -1;
  }
  else if (! IS_TABLE((*ioRightTable)->val.u_tspec.type))
  {
    log_message(parser,
                parser->bufferPos,
                MSG_ROW_COPY_NOTABLE_R,
                type_to_text((*ioRightTable)->val.u_tspec.type));
    return -1;
  }

  rightIndex = translate_exp(parser, rightIndex, FALSE, TRUE);
  if (rightIndex->val.u_tspec.type == T_UNKNOWN)
  {
    assert (parser->abortError != FALSE);
    return -1;
  }
  else if ( ! is_integer(rightIndex->val.u_tspec.type))
  {
    log_message(parser,
                parser->bufferPos,
                MSG_ROW_COPY_NOINDEX_R,
                type_to_text(rightIndex->val.u_tspec.type));
    return -1;
  }

  return 1;
}


static bool_t
clear_all_expression (struct ParserState* const parser, uint_t fieldsCount)
{
  struct WOutputStream* const instrs  = stmt_query_instrs(parser->pCurrentStmt);
  if (encode_opcode(instrs, W_CTS) == NULL
      || wh_ostream_wint8(instrs, 4 + fieldsCount) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    return FALSE;
  }

  return TRUE;
}


static bool_t
print_unmatched_field_types_errs(struct ParserState* const parser,
                                 const struct DeclaredVar* const leftField,
                                 const struct DeclaredVar* const rightField)
{
  char srcField[128], destField[128];

  wh_copy_first(srcField, rightField->label, sizeof srcField, rightField->labelLength);
  wh_copy_first(destField, leftField->label, sizeof destField, leftField->labelLength);

  log_message(parser,
              parser->bufferPos,
              MSG_ROW_COPY_TYPEFIELD_NA,
              srcField,
              type_to_text(rightField->type),
              destField,
              type_to_text(leftField->type));

  return FALSE;
}


static struct WOutputStream*
encode_local_load(struct Statement* const stmt,
                  struct WOutputStream* const instrs,
                  const uint32_t localIndex)
{
  if (encode_opcode(instrs, W_LDLO32) == NULL)
    return NULL;

  const uint_t offset = wh_ostream_size(instrs);
  if (wh_array_add(&stmt->spec.proc.iteratorsUsage, &offset) == NULL
     || wh_ostream_wint32(instrs, localIndex) == NULL)
  {
    return NULL;
  }

  return instrs;
}


static bool_t
translate_field_copy (struct ParserState* const parser,
                      const struct DeclaredVar* const leftField,
                      const struct DeclaredVar* const rightField,
                      const uint_t startLocalsIndex,
                      const enum W_OPCODE storeOpcode)
{
  struct Statement* const stmt = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs(stmt);

  const int32_t leftTextPos = add_constant_text(stmt,
                                                (const uint8_t*)leftField->label,
                                                leftField->labelLength);
  const int32_t rightTextPos = add_constant_text(stmt,
                                                 (const uint8_t*)rightField->label,
                                                 rightField->labelLength);
  if (storeOpcode == W_NA)
    return print_unmatched_field_types_errs(parser, leftField, rightField);

  if (leftTextPos < 0
      || rightTextPos < 0

      || encode_local_load(stmt, instrs, startLocalsIndex) == NULL
      || encode_opcode(instrs, W_SELF) == NULL
      || wh_ostream_wint32(instrs, leftTextPos) == NULL
      || encode_local_load(stmt, instrs, startLocalsIndex + 1) == NULL
      || encode_opcode(instrs, W_INDF) == NULL

      || encode_local_load(stmt, instrs, startLocalsIndex + 2) == NULL
      || encode_opcode(instrs, W_SELF) == NULL
      || wh_ostream_wint32(instrs, rightTextPos) == NULL
      || encode_local_load(stmt, instrs, startLocalsIndex + 3) == NULL
      || encode_opcode(instrs, W_INDF) == NULL

      || encode_opcode(instrs, storeOpcode) == NULL)
  {
    log_message(parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
    parser->abortError = TRUE;
    return FALSE;
  }

  return TRUE;
}

static bool_t
copy_rows_fields (struct ParserState* const parser,
                  struct WArray* const leftIdsList,
                  struct WArray* const rightIdsList)
{
  struct Statement* const stmt = parser->pCurrentStmt;
  const struct WArray* const iterators = stmt_query_loop_iterators_stack(stmt);
  const uint_t startLocalsIndex = wh_array_count(iterators);
  int i;

  assert (wh_array_count(leftIdsList) == wh_array_count(rightIdsList));
  assert (stmt->type == STMT_PROC);

  for (i = 0; i < wh_array_count(leftIdsList); ++i)
  {
    const struct DeclaredVar* const leftField = *(struct DeclaredVar**)wh_array_get(leftIdsList, i);
    const struct DeclaredVar* const rightField =
        *(struct DeclaredVar**)wh_array_get(rightIdsList, i);

    assert (IS_TABLE_FIELD(leftField->type));
    assert (IS_TABLE_FIELD(rightField->type));

    if (IS_ARRAY(leftField->type) && IS_ARRAY(rightField->type))
    {
      if (GET_TYPE(leftField->type) != GET_TYPE(rightField->type))
        return print_unmatched_field_types_errs(parser, leftField, rightField);

      if ( ! translate_field_copy(parser, leftField, rightField, startLocalsIndex, W_STA))
        return FALSE;

      continue ;
    }
    else if (IS_ARRAY(leftField->type) ^ IS_ARRAY(rightField->type))
        return print_unmatched_field_types_errs(parser, leftField, rightField);

    assert (T_UNKNOWN < GET_TYPE(leftField->type) && GET_TYPE(leftField->type) <= T_TEXT);
    assert (T_UNKNOWN < GET_TYPE(rightField->type) && GET_TYPE(rightField->type) <= T_TEXT);

    if ( ! translate_field_copy(parser,
                                leftField,
                                rightField,
                                startLocalsIndex,
                                store_op[GET_TYPE(leftField->type)][GET_TYPE(rightField->type)]))
    {
      return FALSE;
    }
  }
  return TRUE;
}



YYSTYPE
translate_row_copy (struct ParserState* const   parser,
                    YYSTYPE                     leftTable,
                    YYSTYPE                     leftIdsList,
                    YYSTYPE                     leftIndex,
                    YYSTYPE                     rightTable,
                    YYSTYPE                     rightIdsList,
                    YYSTYPE                     rightIndex)
{
  struct WArray leftFields, rightFields;

  assert ((leftIdsList == NULL) || (leftIdsList->val_type == VAL_ID_LIST));
  assert ((rightIdsList == NULL) || (rightIdsList->val_type == VAL_ID_LIST));

  wh_array_init(&leftFields, sizeof (struct DeclaredVar* ));
  wh_array_init(&rightFields, sizeof (struct DeclaredVar* ));

  if (translate_all_expression(parser, leftIndex, rightIndex, &leftTable, &rightTable) < 0)
  {
    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  assert(leftIndex->val_type == VAL_TYPE_SPEC);
  assert(rightIndex->val_type == VAL_TYPE_SPEC);

  free_sem_value(leftIndex);
  free_sem_value(rightIndex);

  if (leftIdsList != NULL)
  {
    if ( ! get_field_list_from_ids(parser,
                                   &leftIdsList->val.u_idlist,
                                   leftTable,
                                   TRUE,
                                   &leftFields))
    {
      wh_array_clean(&leftFields);
      wh_array_clean(&rightFields);

      return NULL;
    }
  }
  else if ( ! get_table_fields(parser, leftTable, &leftFields))
  {
    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  if (rightIdsList != NULL)
  {
    if ( ! get_field_list_from_ids(parser,
                                   &rightIdsList->val.u_idlist,
                                   rightTable,
                                   FALSE,
                                   &rightFields))
    {
      wh_array_clean(&leftFields);
      wh_array_clean(&rightFields);

      return NULL;
    }
  }
  else if ( ! get_field_list_from_left_selection(parser,
                                                 rightTable,
                                                 FALSE,
                                                 &leftFields,
                                                 &rightFields))
  {
    if (leftIdsList == NULL)
      dump_table_type(parser, leftTable, MSG_ROW_COPY_LTABLE_TYPE);

    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  if (wh_array_count(&leftFields) != wh_array_count(&rightFields))
  {
    log_message(parser, parser->bufferPos, MSG_ROW_COPY_NOFIELDS_CNT);
    dump_tables(parser, leftTable, rightTable);

    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }
  else if (wh_array_count(&leftFields) == 0)
  {
    log_message(parser, parser->bufferPos, MSG_ROW_COPY_NOFIELD_SEL);
    dump_tables(parser, leftTable, rightTable);
  }

  if (! copy_rows_fields(parser, &leftFields, &rightFields))
  {
    if (leftIdsList == NULL || rightIdsList == NULL)
      dump_tables(parser, rightTable, leftTable);

    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  while (leftIdsList != NULL)
  {
    struct SemValue* const next = leftIdsList->val.u_idlist.next;

    assert (leftIdsList->val_type == VAL_ID_LIST);
    free_sem_value(leftIdsList);
    leftIdsList = next;
  }
  while (rightIdsList != NULL)
  {
    struct SemValue* const next = rightIdsList->val.u_idlist.next;

    assert (rightIdsList->val_type == VAL_ID_LIST);
    free_sem_value(rightIdsList);
    rightIdsList = next;
  }

  assert (leftTable->val_type == VAL_TYPE_SPEC);
  assert (rightTable->val_type == VAL_TYPE_SPEC);

  free_sem_value(leftTable);
  free_sem_value(rightTable);

  if ( ! clear_all_expression(parser, wh_array_count(&leftFields)))
  {
    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  wh_array_clean(&leftFields);
  wh_array_clean(&rightFields);

  return NULL;
}


YYSTYPE
translate_row_copy_free (struct ParserState* const   parser,
                         YYSTYPE                     leftTable,
                         YYSTYPE                     leftIndex,
                         YYSTYPE                     rightTable,
                         YYSTYPE                     rightIndex)
{
  struct WArray leftFields, rightFields;

  wh_array_init(&leftFields, sizeof (struct DeclaredVar*));
  wh_array_init(&rightFields, sizeof (struct DeclaredVar*));

  if (translate_all_expression(parser, leftIndex, rightIndex, &leftTable, &rightTable) < 0)
    return NULL;

  assert(leftIndex->val_type == VAL_TYPE_SPEC);
  assert(rightIndex->val_type == VAL_TYPE_SPEC);

  free_sem_value(leftIndex);
  free_sem_value(rightIndex);

  if ( ! get_table_fields(parser, leftTable, &leftFields))
  {
    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  if ( ! get_field_list_from_left_selection(parser,
                                            rightTable,
                                            TRUE,
                                            &leftFields,
                                            &rightFields))
  {
    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  if (wh_array_count(&leftFields) != wh_array_count(&rightFields))
  {
    log_message(parser, parser->bufferPos, MSG_ROW_COPY_NOFIELDS_CNT);
    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }
  else if (wh_array_count(&leftFields) == 0)
  {
    log_message(parser, parser->bufferPos, MSG_ROW_COPY_NOFIELD_SEL);
    dump_tables(parser, rightTable, leftTable);
  }

  if (! copy_rows_fields(parser, &leftFields, &rightFields))
  {
      dump_tables(parser, rightTable, leftTable);
      wh_array_clean(&leftFields);
      wh_array_clean(&rightFields);

      return NULL;
  }

  assert (leftTable->val_type == VAL_TYPE_SPEC);
  assert (rightTable->val_type == VAL_TYPE_SPEC);

  free_sem_value(leftTable);
  free_sem_value(rightTable);

  if ( ! clear_all_expression(parser, wh_array_count(&leftFields)))
  {
    wh_array_clean(&leftFields);
    wh_array_clean(&rightFields);

    return NULL;
  }

  wh_array_clean(&leftFields);
  wh_array_clean(&rightFields);

  return NULL;
}
