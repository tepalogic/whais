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

#include "whisper.h"
#include "whisperc/wopcodes.h"

#include "expression.h"
#include "wlog.h"
#include "statement.h"
#include "vardecl.h"
#include "opcodes.h"
#include "procdecl.h"


YYSTYPE
create_exp_link (struct ParserState* pState,
                 YYSTYPE             firstOp,
                 YYSTYPE             secondOp,
                 YYSTYPE             thirdOp,
                 enum EXP_OPERATION  opcode)
{
  struct SemValue* const result = alloc_sem_value (pState);

  if (result == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return NULL;
    }

  result->val_type            = VAL_EXP_LINK;
  result->val.u_exp.pFirstOp  = firstOp;
  result->val.u_exp.pSecondOp = secondOp;
  result->val.u_exp.pThirdOp  = thirdOp;
  result->val.u_exp.opcode    = opcode;

  return result;
}

static D_BOOL
is_unsigned (D_UINT type)
{
  return ((type >= T_UINT8) && (type <= T_UINT64));
}

static D_BOOL
is_signed (D_UINT type)
{
  return ((type >= T_INT8) && (type <= T_INT64));
}

static D_BOOL
is_integer (D_UINT type)
{
  return is_unsigned (type) || is_signed (type);
}

struct ExpResultType
{
  const struct DeclaredVar* extra; /* only for table types */
  D_UINT                    type;
};

static const struct ExpResultType gResultUnk = { NULL, T_UNKNOWN };


static struct ExpResultType
translate_exp_tree (struct ParserState* const   pState,
                    struct Statement* const     pStmt,
                    struct SemExpression* const pTreeHead);

static D_BOOL
is_leaf_exp (struct SemExpression *exp)
{
  return (exp->opcode == OP_NULL) && (exp->pSecondOp == NULL);
}

static const struct DeclaredVar*
find_field (const D_CHAR* const       pLabel,
            const D_UINT              labelLen,
            const struct DeclaredVar* pFieldList)
{
  assert (pFieldList != NULL);

  while (IS_TABLE_FIELD (pFieldList->type))
    {
      if ((pFieldList->labelLength == labelLen) &&
          (memcmp (pFieldList->label, pLabel, labelLen) == 0))
        {
          break;
        }
      pFieldList = pFieldList->extra;
    }

  if (IS_TABLE_FIELD (pFieldList->type) == FALSE)
    {
      assert (IS_TABLE (pFieldList->type));

      /* the field was not found */
      return NULL;
    }

  return pFieldList;
}

static D_BOOL
are_compatible_fields (const struct DeclaredVar* const pFirstField,
                       const struct DeclaredVar* const pSecondField)
{
  const D_UINT firstBaseType  = GET_BASIC_TYPE (pFirstField->type);
  const D_UINT secondBaseType = GET_BASIC_TYPE (pSecondField->type);

  assert (IS_TABLE_FIELD (pFirstField->type));
  assert (IS_TABLE_FIELD (pSecondField->type));

  assert (firstBaseType < T_UNDETERMINED);
  assert (secondBaseType < T_UNDETERMINED);

  if ( IS_ARRAY (GET_FIELD_TYPE (pFirstField->type)) !=
       IS_ARRAY (GET_FIELD_TYPE (pSecondField->type)) )
    {
      return FALSE;
    }
  else if (W_NA == store_op[firstBaseType][secondBaseType])
    return FALSE;

  return TRUE;
}

static const D_CHAR*
array_to_text (D_UINT type)
{
  assert (IS_ARRAY (type));
  type = GET_BASIC_TYPE (type);

  assert (type > T_UNKNOWN || type <= T_UNDETERMINED);

  if (type == T_BOOL)
    return "ARRAY OF BOOL";
  else if (type == T_CHAR)
    return "ARRAY OF CHARACTER";
  else if (type == T_DATE)
    return "ARRAY OF DATE";
  else if (type == T_DATETIME)
    return "ARRAY OF DATETIME";
  else if (type == T_HIRESTIME)
    return "ARRAY OF HIRESTIME";
  else if (type == T_INT8)
    return "ARRAY OF INT8";
  else if (type == T_INT16)
    return "ARRAY OF INT16";
  else if (type == T_INT32)
    return "ARRAY OF INT32";
  else if (type == T_INT64)
    return "ARRAY OF INT64";
  else if (type == T_REAL)
    return "ARRAY OF REAL";
  else if (type == T_RICHREAL)
    return "ARRAY OF RICHREAL";
  else if (type == T_TEXT)
    return "ARRAY OF TEXT";
  else if (type == T_UINT8)
    return "ARRAY OF UNSIGNED INT8";
  else if (type == T_UINT16)
    return "ARRAY OF UNSIGNED INT16";
  else if (type == T_UINT32)
    return "ARRAY OF UNSIGNED INT32";
  else if (type == T_UINT64)
    return "ARRAY OF UNSIGNED INT64";

  return "ARRAY";
}

static const D_CHAR*
field_to_text (D_UINT type)
{
  assert (IS_FIELD (type));

  type = GET_BASIC_TYPE (type);

  assert (type > T_UNKNOWN || type <= T_UNDETERMINED);

  if (type == T_BOOL)
    return "FIELD OF BOOL";
  else if (type == T_CHAR)
    return "FIELD OF CHARACTER";
  else if (type == T_DATE)
    return "FIELD OF DATE";
  else if (type == T_DATETIME)
    return "FIELD OF DATETIME";
  else if (type == T_HIRESTIME)
    return "FIELD OF HIRESTIME";
  else if (type == T_INT8)
    return "FIELD OF INT8";
  else if (type == T_INT16)
    return "FIELD OF INT16";
  else if (type == T_INT32)
    return "FIELD OF INT32";
  else if (type == T_INT64)
    return "FIELD OF INT64";
  else if (type == T_REAL)
    return "FIELD OF REAL";
  else if (type == T_RICHREAL)
    return "FIELD OF RICHREAL";
  else if (type == T_TEXT)
    return "FIELD OF TEXT";
  else if (type == T_UINT8)
    return "FIELD OF UNSIGNED INT8";
  else if (type == T_UINT16)
    return "FIELD OF UNSIGNED INT16";
  else if (type == T_UINT32)
    return "FIELD OF UNSIGNED INT32";
  else if (type == T_UINT64)
    return "FIELD OF UNSIGNED INT64";
  else if (IS_ARRAY (type))
    {
      type = GET_BASIC_TYPE (type);
      assert (type > T_UNKNOWN || type < T_UNDETERMINED);

      if (type == T_BOOL)
        return "FIELD OF ARRAY OF BOOL";
      else if (type == T_CHAR)
        return "FIELD OF ARRAY OF CHARACTER";
      else if (type == T_DATE)
        return "FIELD OF ARRAY OF DATE";
      else if (type == T_DATETIME)
        return "FIELD OF ARRAY OF DATETIME";
      else if (type == T_HIRESTIME)
        return "FIELD OF ARRAY OF HIRESTIME";
      else if (type == T_INT8)
        return "FIELD OF ARRAY OF INT8";
      else if (type == T_INT16)
        return "FIELD OF ARRAY OF INT16";
      else if (type == T_INT32)
        return "FIELD OF ARRAY OF INT32";
      else if (type == T_INT64)
        return "FIELD OF ARRAY OF INT64";
      else if (type == T_REAL)
        return "FIELD OF ARRAY OF REAL";
      else if (type == T_RICHREAL)
        return "FIELD OF ARRAY OF RICHREAL";
      else if (type == T_TEXT)
        return "FIELD OF ARRAY OF TEXT";
      else if (type == T_UINT8)
        return "FIELD OF ARRAY OF UNSIGNED INT8";
      else if (type == T_UINT16)
        return "FIELD OF ARRAY OF UNSIGNED INT16";
      else if (type == T_UINT32)
        return "FIELD OF ARRAY OF UNSIGNED INT32";
      else if (type == T_UINT64)
        return "FIELD OF ARRAY OF UNSIGNED INT64";
    }

  return "FIELD";
}

static const D_CHAR*
type_to_text (D_UINT type)
{
  type = GET_TYPE (type);

  if (type == T_BOOL)
    return "BOOL";
  else if (type == T_CHAR)
    return "CHARACTER";
  else if (type == T_DATE)
    return "DATE";
  else if (type == T_DATETIME)
    return "DATETIME";
  else if (type == T_HIRESTIME)
    return "HIRESTIME";
  else if (type == T_INT8)
    return "INT8";
  else if (type == T_INT16)
    return "INT16";
  else if (type == T_INT32)
    return "INT32";
  else if (type == T_INT64)
    return "INT64";
  else if (type == T_REAL)
    return "REAL";
  else if (type == T_RICHREAL)
    return "RICHREAL";
  else if (type == T_TEXT)
    return "TEXT";
  else if (type == T_UINT8)
    return "UNSIGNED INT8";
  else if (type == T_UINT16)
    return "UNSIGNED INT16";
  else if (type == T_UINT32)
    return "UNSIGNED INT32";
  else if (type == T_UINT64)
    return "UNSIGNED INT64";
  else if (IS_FIELD (type))
    return field_to_text (type);
  else if (IS_ARRAY (type))
    return array_to_text (type);
  else if (IS_TABLE (type))
    return "TABLE";

  assert (0);
  return NULL;
}

static struct ExpResultType
translate_exp_inc (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType)
{

  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  enum W_OPCODE           opcode      = W_NA;

  assert (pStmt->type == STMT_PROC);

  if (IS_L_VALUE (firstType->type) == FALSE)
    {
      w_log_msg (pState, pState->bufferPos, MSG_INC_ELV);
      return gResultUnk;
    }

  if (GET_TYPE (firstType->type) < T_END_OF_TYPES)
    opcode = inc_op [GET_TYPE (firstType->type)];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_INC_NA,
                 type_to_text (firstType->type));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  return *firstType;
}

static struct ExpResultType
translate_exp_dec (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  enum W_OPCODE           opcode      = W_NA;

  assert (pStmt->type == STMT_PROC);

  if (IS_L_VALUE (firstType->type) == FALSE)
    {
      w_log_msg (pState, pState->bufferPos, MSG_DEC_ELV);
      return gResultUnk;
    }

  if (GET_TYPE (firstType->type)  < T_END_OF_TYPES)
    opcode = dec_op[GET_TYPE (firstType->type)];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_DEC_NA,
                 type_to_text (firstType->type));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  return *firstType;
}

static struct ExpResultType
translate_exp_not (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType)
{
  struct Statement* const pStmt        = pState->pCurrentStmt;
  struct OutStream* const pCodeStream  = stmt_query_instrs (pStmt);
  enum W_OPCODE           opcode       = W_NA;

  assert (pStmt->type == STMT_PROC);

  if (GET_TYPE (firstType->type) < T_END_OF_TYPES)
    opcode = not_op[GET_TYPE (firstType->type)];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_NOT_NA,
                 type_to_text (firstType->type));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  return *firstType;
}

static struct ExpResultType
translate_exp_add (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType,
                   const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((GET_TYPE (firstType->type) < T_END_OF_TYPES) &&
      (GET_TYPE (secondType->type) < T_END_OF_TYPES))
    {
      opcode = add_op[GET_TYPE (firstType->type)][GET_TYPE (secondType->type)];
    }

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_ADD_NA,
                 type_to_text (firstType->type),
                 type_to_text (secondType->type));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  switch (opcode)
    {
    case W_ADD:
      if (is_unsigned (GET_TYPE (firstType->type)) !=
          is_unsigned (GET_TYPE (secondType->type)))
        {
          w_log_msg (pState, pState->bufferPos, MSG_ADD_SIGN);
          result.type = T_INT64;
        }
      else if (is_unsigned (GET_TYPE (firstType->type)))
        result.type = T_UINT64;
      else
        result.type = T_INT64;
      break;

    case W_ADDR:
      result.type = T_REAL;
      break;

    case W_ADDRR:
      result.type = T_RICHREAL;
      break;

    case W_ADDT:
      result.type = T_TEXT;
      break;

    default:
      assert (0);
    }

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_sub (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType,
                   const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;


  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = sub_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_SUB_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  switch (opcode)
    {
    case W_SUB:
      if (is_unsigned (ftype) != is_unsigned (stype))
        {
          w_log_msg (pState, pState->bufferPos, MSG_SUB_SIGN);
          result.type = T_INT64;
        }
      else if (is_unsigned (ftype))
        result.type = T_UINT64;
      else
        result.type = T_INT64;
      break;

    case W_SUBR:
      result.type = T_REAL;
      break;

    case W_SUBRR:
      result.type = T_RICHREAL;
      break;

    default:
      assert (0);
    }

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_mul (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType,
                   const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = mul_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_MUL_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  switch (opcode)
    {
    case W_MUL:
      if (is_unsigned (ftype) != is_unsigned (stype))
        {
          w_log_msg (pState, pState->bufferPos, MSG_MUL_SIGN);
          result.type = T_INT64;
        }
      else if (is_unsigned (ftype))
        result.type = T_UINT64;
      else
        result.type = T_INT64;
      break;

    case W_MULR:
      result.type = T_REAL;
      break;

    case W_MULRR:
      result.type = T_RICHREAL;
      break;

    default:
      assert (0);
    }

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_div (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType,
                   const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;


  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = div_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_DIV_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  switch (opcode)
    {
    case W_DIV:
      if (is_unsigned (ftype) != is_unsigned (stype))
        {
          w_log_msg (pState, pState->bufferPos, MSG_DIV_SIGN);
          result.type = T_INT64;
        }
      else if (is_unsigned (ftype))
        result.type = T_UINT64;
      else
        result.type = T_INT64;
      break;

    case W_DIVR:
      result.type = T_REAL;
      break;

    case W_DIVRR:
      result.type = T_RICHREAL;
      break;

    default:
      assert (0);
    }

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_mod (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType,
                   const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = mod_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_MOD_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert (opcode == W_MOD);

  result.type  = T_UINT64;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_less (struct ParserState* const         pState,
                    const struct ExpResultType* const firstType,
                    const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = less_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_LT_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert ((opcode == W_LT) || (opcode == W_LTR) || (opcode == W_LTRR) ||
          (opcode == W_LTC) || (opcode == W_LTD) || (opcode == W_LTDT) ||
          (opcode == W_LTHT) || (opcode == W_LTR) || (opcode == W_LTRR) ||
          (opcode == W_LTT));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_less_equal (struct ParserState* const         pState,
                          const struct ExpResultType* const firstType,
                          const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = less_eq_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_LE_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert ((opcode == W_LE) || (opcode == W_LER) || (opcode == W_LERR) ||
          (opcode == W_LEC) || (opcode == W_LED) || (opcode == W_LEDT) ||
          (opcode == W_LEHT) || (opcode == W_LER) || (opcode == W_LERR) ||
          (opcode == W_LET));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_grater (struct ParserState* const         pState,
                      const struct ExpResultType* const firstType,
                      const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = grater_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_GT_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert ((opcode == W_GT) || (opcode == W_GTR) || (opcode == W_GTRR) ||
          (opcode == W_GTC) || (opcode == W_GTD) || (opcode == W_GTDT) ||
          (opcode == W_GTHT) || (opcode == W_GTR) || (opcode == W_GTRR) ||
          (opcode == W_GTT));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_grater_equal (struct ParserState* const         pState,
                            const struct ExpResultType* const firstType,
                            const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = grater_eq_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_GE_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert ((opcode == W_GE) || (opcode == W_GER) || (opcode == W_GERR) ||
          (opcode == W_GEC) || (opcode == W_GED) || (opcode == W_GEDT) ||
          (opcode == W_GEHT) || (opcode == W_GER) || (opcode == W_GERR) ||
          (opcode == W_GET));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_equals (struct ParserState* const         pState,
                      const struct ExpResultType* const firstType,
                      const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = equals_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_EQ_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert ((opcode == W_EQ) || (opcode == W_EQR) || (opcode == W_EQRR) ||
          (opcode == W_EQC) || (opcode == W_EQD) || (opcode == W_EQDT) ||
          (opcode == W_EQHT) || (opcode == W_EQR) || (opcode == W_EQRR) ||
          (opcode == W_EQB) || (opcode == W_EQT));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_not_equals (struct ParserState* const         pState,
                          const struct ExpResultType* const firstType,
                          const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = not_equals_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_NE_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert ((opcode == W_NE) || (opcode == W_NER) || (opcode == W_NERR) ||
          (opcode == W_NEC) || (opcode == W_NED) || (opcode == W_NEDT) ||
          (opcode == W_NEHT) || (opcode == W_NER) || (opcode == W_NERR) ||
          (opcode == W_NEB) || (opcode == W_NET));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_or (struct ParserState* const         pState,
                  const struct ExpResultType* const firstType,
                  const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = or_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_OR_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  assert ((opcode == W_OR) || (opcode == W_ORB));

  if (opcode == W_OR)
    {
      assert (is_integer (stype));
      result.type = T_UINT64;
    }
  else
    {
      assert ((ftype == T_BOOL) && (stype == T_BOOL) && (opcode == W_ORB));
      result.type = T_BOOL;
    }

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_and (struct ParserState* const         pState,
                   const struct ExpResultType* const firstType,
                   const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = and_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_AND_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  if (opcode == W_AND)
    {
      assert (is_integer (stype));
      result.type = T_UINT64;
    }
  else
    {
      assert ((ftype == T_BOOL) && (stype == T_BOOL) && (opcode == W_ANDB));
      result.type = T_BOOL;
    }

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_xor (struct ParserState* const        pState,
                   const struct ExpResultType* const firstType,
                   const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = xor_op[ftype][stype];

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_XOR_NA,
                 type_to_text (ftype),
                 type_to_text (stype));
      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  if (opcode == W_XOR)
    {
      assert (is_integer (stype));
      result.type = T_UINT64;
    }
  else
    {
      assert ((ftype == T_BOOL) && (stype == T_BOOL) && (opcode == W_XORB));
      result.type = T_BOOL;
    }

  result.extra = NULL;

  return result;
}

static D_BOOL
are_compatible_tables (struct ParserState* const         pState,
                       const struct ExpResultType* const pFirstType,
                       const struct ExpResultType* const pSecondType,
                       D_BOOL                            ignoreUnfoundFields)
{
  const struct DeclaredVar* pFirstField  = NULL;
  const struct DeclaredVar* pSecondField = NULL;

  assert (IS_TABLE (pFirstType->type));

  if (IS_TABLE (pSecondType->type) == FALSE)
    {
      w_log_msg (pState, pState->bufferPos, MSG_CONTAINER_NA);
      return FALSE;
    }

  pFirstField = pFirstType->extra;

  if (pFirstField == NULL)
    return TRUE;

  pSecondField = pSecondType->extra;

  while (IS_TABLE_FIELD (pFirstField->type))
    {
      D_CHAR temp[128];

      const struct DeclaredVar* pFoundField = find_field (pFirstField->label,
                                                          pFirstField->labelLength,
                                                          pSecondField);
      assert ((pFoundField == NULL) || IS_TABLE_FIELD (pFoundField->type));

      if (pFoundField == NULL)
        {
          if ( ! ignoreUnfoundFields)
            {
              w_log_msg (pState,
                         pState->bufferPos,
                         MSG_NO_FIELD,
                         copy_text_truncate (temp, pFirstField->label,
                                             sizeof temp,
                                             pFirstField->labelLength));

              return FALSE;
            }
        }
      else if (are_compatible_fields (pFirstField, pFoundField) == FALSE)
        {
          w_log_msg (pState,
                     pState->bufferPos,
                     MSG_FIELD_NA,
                     copy_text_truncate (temp,
                                         pFirstField->label,
                                         sizeof temp,
                                         pFirstField->labelLength),
                     type_to_text (pFirstField->type),
                     type_to_text (pFoundField->type));

          pState->abortError = TRUE;
          return FALSE;
        }
      pFirstField = pFirstField->extra;
    }

  return TRUE;
}

static struct ExpResultType
translate_exp_store (struct ParserState* const         pState,
                     const struct ExpResultType* const firstType,
                     const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if (IS_L_VALUE (firstType->type) == FALSE)
    {
      w_log_msg (pState, pState->bufferPos, MSG_STORE_ELV);
      return gResultUnk;
    }

  if (stype != T_UNDETERMINED)
    {

      if ((ftype < T_UNDETERMINED) && (stype < T_UNDETERMINED))
        opcode = store_op[ftype][stype];
      else if (IS_TABLE (ftype) && IS_TABLE (stype))
        {
          if ( ! are_compatible_tables (pState, firstType, secondType, TRUE))
            return gResultUnk;

          opcode = W_STTA;
        }
      else if (IS_FIELD (ftype) && IS_FIELD (stype))
        {
          if ((GET_FIELD_TYPE (ftype) == T_UNDETERMINED) ||
              (GET_FIELD_TYPE (ftype) == GET_FIELD_TYPE (stype)))
            opcode = W_STF;
        }
      else if (IS_ARRAY (ftype) && IS_ARRAY (stype))
        {
          const D_UINT temp_ftype = GET_BASIC_TYPE (ftype);
          const D_UINT temp_stype = GET_BASIC_TYPE (stype);

          assert (temp_ftype <= T_UNDETERMINED);
          assert (temp_stype <= T_UNDETERMINED);

          if ((temp_ftype == T_UNDETERMINED) || (temp_ftype == temp_stype))
            opcode = W_STA;
        }
    }
  else
    {
      /* a NULL value is assigned */
      if (ftype < T_UNDETERMINED)
        opcode = store_op[ftype][ftype];
      else if (IS_TABLE (ftype))
        opcode = W_STTA;
      else if (IS_ARRAY (ftype))
        opcode = W_STA;
    }

  if (opcode == W_NA)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_STORE_NA,
                 type_to_text (ftype),
                 type_to_text (stype));

      return gResultUnk;
    }

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  result.type  = GET_TYPE (firstType->type);
  result.extra = firstType->extra;

  return result;
}

static struct ExpResultType
translate_exp_index (struct ParserState* const         pState,
                     const struct ExpResultType* const firstType,
                     const struct ExpResultType* const secondType)
{
  struct Statement* const pStmt       = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pStmt);
  const D_UINT            ftype       = GET_TYPE (firstType->type);
  const D_UINT            stype       = GET_TYPE (secondType->type);
  enum W_OPCODE           opcode      = W_NA;
  struct ExpResultType    result;

  if ((IS_FIELD (ftype) == FALSE) &&
      (IS_ARRAY (ftype) == FALSE) &&
      ((ftype != T_TEXT)))
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_INDEX_EAT,
                 type_to_text (ftype));
      pState->abortError = TRUE;
      return gResultUnk;
    }

  if (is_integer (stype) == FALSE)
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_INDEX_ENI,
                 type_to_text (stype));
      pState->abortError = TRUE;
      return gResultUnk;
    }
  else if (IS_FIELD (ftype))
    {
      assert (firstType->extra == NULL);

      opcode      = W_INDF;
      result.type = GET_FIELD_TYPE (ftype);

      assert (IS_ARRAY (result.type) ||
              ((result.type < T_UNDETERMINED) && (result.type > T_UNKNOWN)));
    }
  else if (IS_ARRAY (ftype))
    {
      assert (firstType->extra == NULL);

      opcode      = W_INDA;
      result.type = GET_BASIC_TYPE (ftype);

      if (result.type == T_UNDETERMINED)
        {
          w_log_msg (pState, pState->bufferPos, MSG_INDEX_UNA);
          pState->abortError = TRUE;
          return gResultUnk;
        }
    }
  else
    {
      assert (ftype == T_TEXT);

      opcode      = W_INDT;
      result.type = T_CHAR;
    }

  assert (opcode != W_NA);

  if (w_opcode_encode (pCodeStream, opcode) == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      pState->abortError = TRUE;
      return gResultUnk;
    }

  MARK_L_VALUE (result.type);

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_opcode (struct ParserState* const   pState,
                      struct Statement* const     pStmt,
                      const D_UINT16              opcode,
                      const struct ExpResultType* firstType,
                      const struct ExpResultType* secondType)
{
  struct ExpResultType result = { NULL, T_UNKNOWN };

  switch (opcode)
    {
    case OP_ADD:
      result = translate_exp_add (pState, firstType, secondType);
      break;

    case OP_SUB:
      result = translate_exp_sub (pState, firstType, secondType);
      break;

    case OP_MUL:
      result = translate_exp_mul (pState, firstType, secondType);
      break;

    case OP_DIV:
      result = translate_exp_div (pState, firstType, secondType);
      break;

    case OP_MOD:
      result = translate_exp_mod (pState, firstType, secondType);
      break;

    case OP_LT:
      result = translate_exp_less (pState, firstType, secondType);
      break;

    case OP_LE:
      result = translate_exp_less_equal (pState, firstType, secondType);
      break;

    case OP_GT:
      result = translate_exp_grater (pState, firstType, secondType);
      break;

    case OP_GE:
      result = translate_exp_grater_equal (pState, firstType, secondType);
      break;

    case OP_EQ:
      result = translate_exp_equals (pState, firstType, secondType);
      break;

    case OP_NE:
      result = translate_exp_not_equals (pState, firstType, secondType);
      break;

    case OP_INC:
      result = translate_exp_inc (pState, firstType);
      break;

    case OP_DEC:
      result = translate_exp_dec (pState, firstType);
      break;

    case OP_NOT:
      result = translate_exp_not (pState, firstType);
      break;

    case OP_OR:
      result = translate_exp_or (pState, firstType, secondType);
      break;

    case OP_AND:
      result = translate_exp_and (pState, firstType, secondType);
      break;

    case OP_XOR:
      result = translate_exp_xor (pState, firstType, secondType);
      break;

    case OP_GROUP:
      result = *firstType;
      break;

    case OP_INDEX:
      result = translate_exp_index (pState, firstType, secondType);
      break;

    case OP_ATTR:
      result = translate_exp_store (pState, firstType, secondType);
      break;

    default:
      assert (0);
    }

  return result;
}

static struct ExpResultType
translate_exp_leaf (struct ParserState* const pState,
                    struct Statement*         pStmt,
                    struct SemValue* const    expression)
{
  struct OutStream* const    pCodeStream = stmt_query_instrs (pStmt);
  const struct ExpResultType r_unk       = { NULL, T_UNKNOWN };
  const struct ExpResultType r_undet     = { NULL, T_UNDETERMINED };
  struct ExpResultType       result      = gResultUnk;

  if (expression == NULL)
    {
      if (w_opcode_encode (pCodeStream, W_LDNULL) == NULL)
        {
          w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
          return gResultUnk;
        }
      return r_undet;
    }

  if (expression->val_type == VAL_ID)
    {
      struct DeclaredVar* pVar = stmt_find_declaration (pStmt,
                                                       expression->val.u_id.text,
                                                       expression->val.u_id.length,
                                                       TRUE,
                                                       TRUE);
      D_UINT32 value;
      D_UINT32 value_32 = ~0;
      D_UINT16 value_16 = ~0;
      D_UINT8  value_8  = ~0;
      enum W_OPCODE op_code = W_NA;

      if (pVar == NULL)
        {
          D_CHAR temp[128];
          copy_text_truncate (temp,
                              expression->val.u_id.text,
                              sizeof temp,
                              expression->val.u_id.length);

          w_log_msg (pState, pState->bufferPos, MSG_VAR_NFOUND, temp);
          pState->abortError = TRUE;
          return r_unk;
        }

      value = RETRIVE_ID (pVar->varId);
      if (IS_GLOBAL (pVar->varId))
        {
          if (pStmt->pParentStmt != NULL)
            {
              assert (pStmt->type == STMT_PROC);
              pStmt = pStmt->pParentStmt;
            }
          assert (pStmt->type == STMT_GLOBAL);
        }

      result.extra = pVar->extra;
      result.type  = pVar->type;
      MARK_L_VALUE (result.type);

      if (value <= value_8)
        {
          value_8 = value;
          op_code = (pStmt->type == STMT_GLOBAL) ? W_LDGB8 : W_LDLO8;
          if ((w_opcode_encode (pCodeStream, op_code) == NULL) ||
              output_uint8 (pCodeStream, value_8) == NULL)
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
      else if (value <= value_16)
        {
          value_16 = value;
          op_code  = (pStmt->type == STMT_GLOBAL) ? W_LDGB16 : W_LDLO16;
          if ((w_opcode_encode (pCodeStream, op_code) == NULL) ||
              output_uint16 (pCodeStream, value_16) == NULL)
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
      else
        {
          value_32 = value;
          op_code  = (pStmt->type == STMT_GLOBAL) ? W_LDGB32 : W_LDLO32;
          if ((w_opcode_encode (pCodeStream, op_code) == NULL) ||
              output_uint32 (pCodeStream, value_32) == NULL)
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
    }
  else if (expression->val_type == VAL_C_INT)
    {
      const D_UINT32 value_32 = ~0;
      const D_UINT16 value_16 = ~0;
      const D_UINT8  value_8  = ~0;

      if (expression->val.u_int.value <= value_8)
        {
          result.type = expression->val.u_int.is_signed ? T_INT8 : T_UINT8;
          if ((w_opcode_encode (pCodeStream, W_LDI8) == NULL) ||
              (output_uint8 (pCodeStream,
                             (D_UINT8)expression->val.u_int.value & value_8) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
      else if (expression->val.u_int.value <= value_16)
        {
          result.type = expression->val.u_int.is_signed ? T_INT16 : T_UINT16;
          if ((w_opcode_encode (pCodeStream, W_LDI16) == NULL) ||
              (output_uint16 (pCodeStream,
                              (D_UINT16)expression->val.u_int.value & value_16) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
      else if (expression->val.u_int.value <= value_32)
        {
          result.type = expression->val.u_int.is_signed ? T_INT32 : T_UINT32;
          if ((w_opcode_encode (pCodeStream, W_LDI32) == NULL) ||
              (output_uint32 (pCodeStream,
                              (D_UINT32)expression->val.u_int.value & value_32) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
      else
        {
          result.type = expression->val.u_int.is_signed ? T_INT64 : T_UINT64;
          if ((w_opcode_encode (pCodeStream, W_LDI64) == NULL) ||
              (output_uint64 (pCodeStream, (D_UINT64) expression->val.u_int.value) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
    }
  else if (expression->val_type == VAL_C_CHAR)
    {
      result.type = T_CHAR;
      if ((w_opcode_encode (pCodeStream, W_LDC) == NULL) ||
          (output_data (pCodeStream,
                           (D_UINT8*)&expression->val.u_char.value,
                           sizeof (expression->val.u_char.value)) == NULL))
        {
          w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
          return gResultUnk;
        }
    }
  else if (expression->val_type == VAL_C_TIME)
    {
      struct SemCTime* const value = &expression->val.u_time;

      if (value->usec != 0)
        {
          result.type = T_HIRESTIME;
          if ((w_opcode_encode (pCodeStream, W_LDHT) == NULL) ||
              (output_uint32 (pCodeStream, value->usec) == NULL) ||
              (output_uint8 (pCodeStream, value->sec) == NULL) ||
              (output_uint8 (pCodeStream, value->min) == NULL) ||
              (output_uint8 (pCodeStream, value->hour) == NULL) ||
              (output_uint8 (pCodeStream, value->day) == NULL) ||
              (output_uint8 (pCodeStream, value->month) == NULL) ||
              (output_uint16 (pCodeStream, value->year) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }

        }
      else if ((value->sec != 0) || (value->min != 0) || (value->hour != 0))
        {
          result.type = T_DATETIME;
          if ((w_opcode_encode (pCodeStream, W_LDDT) == NULL) ||
              (output_uint8 (pCodeStream, value->sec) == NULL) ||
              (output_uint8 (pCodeStream, value->min) == NULL) ||
              (output_uint8 (pCodeStream, value->hour) == NULL) ||
              (output_uint8 (pCodeStream, value->day) == NULL) ||
              (output_uint8 (pCodeStream, value->month) == NULL) ||
              (output_uint16 (pCodeStream, value->year) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
      else
        {
          result.type = T_DATE;
          if ((w_opcode_encode (pCodeStream, W_LDD) == NULL) ||
              (output_uint8 (pCodeStream, value->day) == NULL) ||
              (output_uint8 (pCodeStream, value->month) == NULL) ||
              (output_uint16 (pCodeStream, value->year) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
        }
    }
  else if (expression->val_type == VAL_C_REAL)
    {
      result.type = T_REAL;
      if ((w_opcode_encode (pCodeStream, W_LDR) == NULL) ||
          (output_uint64 (pCodeStream, expression->val.u_real.integerPart) == NULL) ||
          (output_uint64 (pCodeStream, expression->val.u_real.fractionalPart) == NULL))

        {
          w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
          return gResultUnk;
        }
    }
  else if (expression->val_type == VAL_C_TEXT)
    {
      struct SemCText* const value    = &expression->val.u_text;
      D_INT32                constPos = add_text_const (pStmt,
                                                        (const D_UINT8*)value->text,
                                                        value->length);

      result.type = T_TEXT;
      if ((constPos < 0) ||
          (w_opcode_encode (pCodeStream, W_LDT) == NULL) ||
          (output_uint32 (pCodeStream, constPos) == NULL))
        {
          w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
          return gResultUnk;
        }
    }
  else if (expression->val_type == VAL_C_BOOL)
    {
      const enum W_OPCODE opcode = (expression->val.u_bool.value == FALSE) ? W_LDBF : W_LDBT;
      if (w_opcode_encode (pCodeStream, opcode) == NULL)
        {
          w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
          return gResultUnk;
        }
      result.type = T_BOOL;
    }
  else
    {
      assert (0);
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_INT_ERR);
      result.type = T_UNKNOWN;
    }

  free_sem_value (expression);
  return result;
}

static struct ExpResultType
translate_exp_call (struct ParserState* const   pState,
                    struct Statement* const     pStmt,
                    struct SemExpression* const callExp)
{
  struct OutStream* const   pCodeStream = stmt_query_instrs (pStmt);
  const struct Statement*   pProc       = NULL;
  const struct DeclaredVar* pProcVar    = NULL;
  struct SemValue*          pExpArg     = NULL;
  D_UINT                    argCount    = 0;
  struct ExpResultType      result;
  D_CHAR                    temp[128];

  assert (callExp->pFirstOp->val_type == VAL_ID);
  assert ((callExp->pSecondOp == NULL) ||
          (callExp->pSecondOp->val_type == VAL_PRC_ARG_LINK));

  pProc = find_proc_decl (pState,
                          callExp->pFirstOp->val.u_id.text,
                          callExp->pFirstOp->val.u_id.length,
                          TRUE);
  if (pProc == NULL)
    {
      w_log_msg (pState, pState->bufferPos,
                 MSG_NO_PROC,
                 copy_text_truncate (temp,
                                     callExp->pFirstOp->val.u_id.text,
                                     sizeof temp,
                                     callExp->pFirstOp->val.u_id.length));
      return gResultUnk;
    }

  pExpArg = callExp->pSecondOp;
  while (pExpArg != NULL)
    {
      struct SemValue* const pParam   = pExpArg->val.u_args.expr;
      struct SemValue*       pTempSem = NULL;
      struct ExpResultType   argType;

      assert (pExpArg->val_type == VAL_PRC_ARG_LINK);
      assert (pParam->val_type == VAL_EXP_LINK);

      ++argCount;
      pProcVar = stmt_get_param (pProc, argCount);

      if (pProcVar == NULL)
        {
          w_log_msg (pState,
                     pState->bufferPos,
                     MSG_PROC_MORE_ARGS,
                     copy_text_truncate (temp,
                                         callExp->pFirstOp->val.u_id.text,
                                         sizeof temp,
                                         callExp->pFirstOp->val.u_id.length),
                     stmt_get_param_count (pProc));
          pState->abortError = TRUE;
          return gResultUnk;
        }
      else
        {
          /* convert the declared variable to an expression result */
          argType.type  = GET_TYPE (pProcVar->type);
          argType.extra = pProcVar->extra;
        }

      result = translate_exp_tree (pState, pStmt, &pParam->val.u_exp);
      if (result.type == T_UNKNOWN)
        {
          /* An error that must be propagated upwards. The error message
           * was logged during expression's evaluation. */
          assert (pState->abortError == TRUE);
          w_log_msg (pState,
                     pState->bufferPos,
                     MSG_PROC_ARG_COUNT,
                     copy_text_truncate (temp, pProc->spec.proc.name,
                                         sizeof temp,
                                         pProc->spec.proc.nameLength),
                                         argCount);
          return gResultUnk;
        }
      else
        free_sem_value (pParam);

      if (result.type != T_UNDETERMINED)
        {
          if (IS_FIELD (result.type))
            {
              const D_BOOL isArgArray = IS_ARRAY (GET_FIELD_TYPE (argType.type));
              const D_BOOL isArgUndet = isArgArray ?
                                          FALSE :
                                          (GET_BASIC_TYPE (argType.type) == T_UNDETERMINED);

              assert ((isArgArray == FALSE) || (isArgUndet == FALSE));

              if ((IS_FIELD (argType.type) == FALSE) ||
                  ((GET_FIELD_TYPE (result.type) != GET_FIELD_TYPE (argType.type)) &&
                    (isArgUndet == FALSE)))
                {
                  w_log_msg (pState,
                             pState->bufferPos,
                             MSG_PROC_ARG_NA,
                             copy_text_truncate (temp,
                                                 pProc->spec.proc.name,
                                                 sizeof temp,
                                                 pProc->spec.proc.nameLength),
                             argCount,
                             type_to_text (argType.type),
                             type_to_text (result.type));

                  pState->abortError = TRUE;
                  return gResultUnk;
                }
            }
          else if (IS_TABLE (result.type) == FALSE)
            {
              if (IS_ARRAY (result.type) == IS_ARRAY (argType.type))
                {
                  const D_UINT        arg_t  = GET_BASIC_TYPE (argType.type);
                  const D_UINT        res_t  = GET_BASIC_TYPE (result.type);
                  const enum W_OPCODE tempOp = store_op[arg_t][res_t];

                  assert (arg_t <= T_UNDETERMINED);
                  assert (res_t <= T_UNDETERMINED);

                  if (((IS_ARRAY (result.type) == FALSE) && (W_NA == tempOp)) ||
                     (IS_ARRAY (result.type) && (arg_t != T_UNDETERMINED) && (arg_t != res_t)))
                    {
                      w_log_msg (pState,
                                 pState->bufferPos,
                                 MSG_PROC_ARG_NA,
                                 copy_text_truncate (temp,
                                                     pProc->spec.proc.name,
                                                     sizeof temp,
                                                     pProc->spec.proc.nameLength),
                                 argCount,
                                 type_to_text (argType.type),
                                 type_to_text (result.type));

                      return gResultUnk;
                    }
                }
              else
                {
                  w_log_msg (pState,
                             pState->bufferPos,
                             MSG_PROC_ARG_NA,
                             copy_text_truncate (temp,
                                                 pProc->spec.proc.name,
                                                 sizeof temp,
                                                 pProc->spec.proc.nameLength),
                             argCount,
                             type_to_text (argType.type),
                             type_to_text (result.type));

                  return gResultUnk;
                }
            }
          else if (are_compatible_tables (pState, &argType, &result, FALSE) == FALSE)
            {
              /* The two containers's types are not compatible.
               * The error was already logged. */
              w_log_msg (pState,
                         pState->bufferPos,
                         MSG_PROC_ARG_COUNT,
                         copy_text_truncate (temp, pProc->spec.proc.name,
                                             sizeof temp,
                                             pProc->spec.proc.nameLength),
                         argCount);

              return gResultUnk;
            }
        }

      pTempSem = pExpArg;
      pExpArg  = pExpArg->val.u_args.next;

      free_sem_value (pTempSem);
    }

  if (argCount < stmt_get_param_count (pProc))
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_PROC_LESS_ARGS,
                 copy_text_truncate (temp,
                                     callExp->pFirstOp->val.u_id.text,
                                     sizeof temp,
                                     callExp->pFirstOp->val.u_id.length),
                 stmt_get_param_count (pProc),
                 argCount);
      return gResultUnk;
    }
  else
    free_sem_value (callExp->pFirstOp);

  if ((w_opcode_encode (pCodeStream, W_CALL) == NULL) ||
      (output_uint32 (pCodeStream, stmt_get_import_id (pProc)) == NULL))
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return gResultUnk;
    }

  pProcVar = stmt_get_param (pProc, 0);
  if (pProcVar == 0)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_INTERNAL_ERROR);
      return gResultUnk;
    }

  result.type  = GET_TYPE (pProcVar->type);
  result.extra = pProcVar->extra;

  return result;
}

static struct ExpResultType
translate_exp_tabval (struct ParserState* const   pState,
                      struct Statement* const     pStmt,
                      struct SemExpression* const pCallExp)
{
  const struct DeclaredVar*   pVarField  = NULL;
  struct OutStream* const     pInstrs    = stmt_query_instrs (pState->pCurrentStmt);
  struct SemExpression* const pFirstExp  = &pCallExp->pFirstOp->val.u_exp;
  struct SemExpression* const pSecondExp = &pCallExp->pSecondOp->val.u_exp;
  struct SemId* const         pId        = &pCallExp->pThirdOp->val.u_id;
  struct ExpResultType        tableType;
  struct ExpResultType        expType;

  assert (pCallExp->opcode == OP_TABVAL);
  assert (pCallExp->pFirstOp->val_type == VAL_EXP_LINK);
  assert (pCallExp->pSecondOp->val_type == VAL_EXP_LINK);
  assert (pCallExp->pThirdOp->val_type == VAL_ID);

  tableType = translate_exp_tree (pState, pStmt, pFirstExp);
  if (tableType.type == T_UNKNOWN)
    {
      assert (pState->abortError);
      return gResultUnk;
    }
  if (IS_TABLE (tableType.type) == FALSE)
    {
      w_log_msg (pState,
                pState->bufferPos,
                MSG_MEMSEL_NA,
                type_to_text (tableType.type));

      pState->abortError = TRUE;
      return gResultUnk;
    }

  expType = translate_exp_tree (pState, pStmt, pSecondExp);
  if (expType.type == T_UNKNOWN)
    {
      assert (pState->abortError);
      return gResultUnk;
    }

  if ( ! is_integer (GET_TYPE (expType.type)))
    {
      w_log_msg (pState,
                 pState->bufferPos,
                 MSG_INDEX_ENI,
                 type_to_text (expType.type));

      pState->abortError = TRUE;
      return gResultUnk;
    }

  if (tableType.extra != NULL)
    pVarField = find_field (pId->text, pId->length, tableType.extra);

  if (pVarField == NULL)
    {
      D_CHAR temp[128];

      copy_text_truncate (temp, pId->text, sizeof temp, pId->length);
      w_log_msg (pState, pState->bufferPos, MSG_MEMSEL_ERD, temp);

      pState->abortError = TRUE;
      return gResultUnk;
    }

  {
    const D_INT32 constPos = add_text_const (pStmt,
                                             (const D_UINT8*)pId->text,
                                             pId->length);

    if ((constPos < 0) ||
        (w_opcode_encode (pInstrs, W_INDTA) == NULL) ||
        (output_uint32 (pInstrs, constPos) == NULL))
      {
        w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
        return gResultUnk;
      }
  }

  expType.extra = NULL;
  expType.type  = GET_TYPE (pVarField->type);

  MARK_L_VALUE (expType.type);

  free_sem_value (pCallExp->pFirstOp);
  free_sem_value (pCallExp->pSecondOp);
  free_sem_value (pCallExp->pThirdOp);

  return expType;
}

static struct ExpResultType
translate_exp_field (struct ParserState* const   pState,
                     struct Statement* const     pStmt,
                     struct SemExpression* const pCallExp)
{
  const struct DeclaredVar*   pVarField  = NULL;
  struct OutStream* const     pInstrs    = stmt_query_instrs (pState->pCurrentStmt);
  struct SemExpression* const pFirstExp  = &pCallExp->pFirstOp->val.u_exp;
  struct SemId* const         pId        = &pCallExp->pSecondOp->val.u_id;
  struct ExpResultType        tableType;
  struct ExpResultType        expType;

  assert (pCallExp->opcode == OP_FIELD);
  assert (pCallExp->pFirstOp->val_type == VAL_EXP_LINK);
  assert (pCallExp->pSecondOp->val_type == VAL_ID);

  tableType = translate_exp_tree (pState, pStmt, pFirstExp);
  if (tableType.type == T_UNKNOWN)
    {
      assert (pState->abortError);
      return gResultUnk;
    }

  if (IS_TABLE (tableType.type) == FALSE)
    {
      w_log_msg (pState,
                pState->bufferPos,
                MSG_MEMSEL_NA,
                type_to_text (tableType.type));

      pState->abortError = TRUE;
      return gResultUnk;
    }

  if (tableType.extra != NULL)
    pVarField = find_field (pId->text, pId->length, tableType.extra);

  if (pVarField == NULL)
    {
      D_CHAR temp[128];

      copy_text_truncate (temp, pId->text, sizeof temp, pId->length);
      w_log_msg (pState, pState->bufferPos, MSG_MEMSEL_ERD, temp);

      pState->abortError = TRUE;
      return gResultUnk;
    }

  {
    const D_INT32 constPos = add_text_const (pStmt,
                                             (const D_UINT8*)pId->text,
                                             pId->length);

    if ((constPos < 0) ||
        (w_opcode_encode (pInstrs, W_SELF) == NULL) ||
        (output_uint32 (pInstrs, constPos) == NULL))
      {
        w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
        return gResultUnk;
      }
  }

  expType.extra = NULL;
  expType.type  = GET_TYPE (pVarField->type);

  MARK_FIELD (expType.type);

  free_sem_value (pCallExp->pFirstOp);
  free_sem_value (pCallExp->pSecondOp);

  return expType;
}

static struct ExpResultType
translate_exp_tree (struct ParserState* const   pState,
                    struct Statement* const     pStmt,
                    struct SemExpression* const pTreeHead)
{
  struct OutStream* const pCodeStream        = stmt_query_instrs (pState->pCurrentStmt);
  D_BOOL                  needsJmpCorrection = FALSE;
  D_INT                   jmpPosition;
  D_INT                   jmpDataPos;
  struct ExpResultType    firstType;
  struct ExpResultType    secondType;

  assert (pState->pCurrentStmt->type == STMT_PROC);

  if (is_leaf_exp (pTreeHead))
    return translate_exp_leaf (pState, pStmt, pTreeHead->pFirstOp);
  else if (pTreeHead->opcode == OP_CALL)
    return translate_exp_call (pState, pStmt, pTreeHead);
  else if (pTreeHead->opcode == OP_TABVAL)
    return translate_exp_tabval (pState, pStmt, pTreeHead);
  else if (pTreeHead->opcode == OP_FIELD)
    return translate_exp_field (pState, pStmt, pTreeHead);

  assert (pTreeHead->pFirstOp->val_type == VAL_EXP_LINK);

  firstType = translate_exp_tree (pState,
                                  pStmt,
                                  &(pTreeHead->pFirstOp->val.u_exp));
  if (firstType.type == T_UNKNOWN)
    {
      assert (pState->abortError);
      /* something went wrong, and the error
       * should be already logged  */
      return gResultUnk;
    }

  free_sem_value (pTreeHead->pFirstOp);

  if (GET_TYPE (firstType.type) == T_BOOL)
    {
      /* Handle special case for OR or AND with boolean types not to evaluate
       * the second expression when is unnecessary!
       * Use 0 for jump offset just to reserve the space. It will be corrected
       * after we parse the second expression */
      jmpPosition = get_size_outstream (pCodeStream);
      if (pTreeHead->opcode == OP_OR)
        {
          if ((w_opcode_encode (pCodeStream, W_JT) == NULL) ||
              (output_uint32 (pCodeStream, 0) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
          needsJmpCorrection = TRUE;
        }
      else if (pTreeHead->opcode == OP_AND)
        {
          if ((w_opcode_encode (pCodeStream, W_JF) == NULL) ||
              (output_uint32 (pCodeStream, 0) == NULL))
            {
              w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return gResultUnk;
            }
          needsJmpCorrection = TRUE;
        }
      jmpDataPos = get_size_outstream (pCodeStream) - sizeof (D_UINT32);
    }

  if (pTreeHead->pSecondOp != NULL)
    {
      assert (pTreeHead->pSecondOp->val_type == VAL_EXP_LINK);
      secondType = translate_exp_tree (pState,
                                       pStmt,
                                       &(pTreeHead->pSecondOp->val.u_exp));
      if (secondType.type == T_UNKNOWN)
        {
          /* something went wrong, and the error
           * should be already signaled  */
          return gResultUnk;
        }
      free_sem_value (pTreeHead->pSecondOp);
    }

  /* use second_type to store result */
  secondType = translate_exp_opcode (pState,
                                     pStmt,
                                     pTreeHead->opcode,
                                     &firstType,
                                     &secondType);

  if (needsJmpCorrection && (GET_TYPE (secondType.type) == T_BOOL))
    {
      /* lets correct some jumps offsets */
      D_INT                currentPos = get_size_outstream (pCodeStream) - jmpPosition;
      const D_UINT8* const pCurrPos   = (D_UINT8*)&currentPos;
      D_UINT8* const       pCode      = get_buffer_outstream (pCodeStream);

      /* Fix the jump offset! */
      pCode[jmpDataPos + 0] = pCurrPos[0];
      pCode[jmpDataPos + 1] = pCurrPos[1];
      pCode[jmpDataPos + 2] = pCurrPos[2];
      pCode[jmpDataPos + 3] = pCurrPos[3];
    }

  return secondType;
}

YYSTYPE
translate_exp (struct ParserState* pState, YYSTYPE exp)
{
  struct Statement* const pCurrStmt   = pState->pCurrentStmt;
  struct OutStream* const pCodeStream = stmt_query_instrs (pCurrStmt);

  assert (exp->val_type = VAL_EXP_LINK);
  assert (pCurrStmt->type == STMT_PROC);

  translate_exp_tree (pState, pCurrStmt, &(exp->val.u_exp));

  free_sem_value (exp);

  if (w_opcode_encode (pCodeStream, W_CTS) == NULL)
    w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);

  return NULL;
}

YYSTYPE
translate_return_exp (struct ParserState* pState, YYSTYPE exp)
{
  struct Statement* const         pCurrStmt   = pState->pCurrentStmt;
  struct OutStream* const         pCodeStream = stmt_query_instrs (pCurrStmt);
  const struct DeclaredVar* const pRetVar     = stmt_get_param (pCurrStmt, 0);

  struct ExpResultType retType = gResultUnk;
  struct ExpResultType expType = gResultUnk;

  assert (exp->val_type = VAL_EXP_LINK);
  assert (pCurrStmt->type == STMT_PROC);

  expType = translate_exp_tree (pState, pCurrStmt, &(exp->val.u_exp));
  if (expType.type == T_UNKNOWN)
    {
      /* some error has been encountered evaluating of return expression */
      assert (pState->abortError != FALSE);
      return NULL;
    }

  free_sem_value (exp);

  /* convert the declared return type to an expression result type */
  retType.type  = GET_TYPE (pRetVar->type);
  retType.extra = pRetVar->extra;

  if (expType.type != T_UNDETERMINED)
    {
      /* The expression was not evaluated to NULL. */

      if ((IS_TABLE (retType.type) != IS_TABLE (expType.type)) ||
          (IS_FIELD (retType.type) != IS_FIELD (expType.type)) ||
          (IS_ARRAY (retType.type) != IS_ARRAY (expType.type)))
        {
          w_log_msg (pState,
                     pState->bufferPos,
                     MSG_PROC_RET_NA_EXT,
                     type_to_text (GET_TYPE (retType.type)),
                     type_to_text (GET_TYPE (expType.type)));
          pState->abortError = TRUE;
        }
      else if (IS_FIELD (retType.type))
        {
          if ((GET_FIELD_TYPE (retType.type) != T_UNDETERMINED) &&
              (GET_FIELD_TYPE (retType.type) != GET_FIELD_TYPE (expType.type)))
            {
              w_log_msg (pState,
                         pState->bufferPos,
                         MSG_PROC_RET_NA_EXT,
                         type_to_text (GET_TYPE (retType.type)),
                         type_to_text (GET_TYPE (expType.type)));
              pState->abortError = TRUE;
            }
        }
      else if ( IS_TABLE (retType.type) == FALSE)
        {
          if (IS_ARRAY (retType.type) == FALSE)
            {
              const D_UINT        baseExpType   = GET_BASIC_TYPE (expType.type);
              const D_UINT        baseRetType   = GET_BASIC_TYPE (retType.type);
              const enum W_OPCODE temp_op       = store_op[baseRetType][baseExpType];

              assert (IS_ARRAY (expType.type) == FALSE);
              assert (baseExpType <= T_UNDETERMINED);
              assert (baseRetType <= T_UNDETERMINED);


              if (temp_op == W_NA)
                {
                  w_log_msg (pState,
                             pState->bufferPos,
                             MSG_PROC_RET_NA_EXT,
                             type_to_text (retType.type),
                             type_to_text (expType.type));
                  pState->abortError = TRUE;
                }
            }
          else
            {
              assert (IS_ARRAY (expType.type));
              if ((GET_BASIC_TYPE (retType.type) != T_UNDETERMINED) &&
                  (GET_BASIC_TYPE (retType.type) != GET_BASIC_TYPE (expType.type)))
                {
                  w_log_msg (pState,
                             pState->bufferPos,
                             MSG_PROC_RET_NA_EXT,
                             type_to_text (retType.type),
                             type_to_text (expType.type));
                  pState->abortError = TRUE;
                }
            }
        }
      else if (are_compatible_tables (pState, &retType, &expType, FALSE) == FALSE)
        {
          /* The two containers types are not compatible.
           * The error was already logged. */
          w_log_msg (pState, pState->bufferPos, MSG_PROC_RET_NA);
          pState->abortError = TRUE;
        }
    }

  if (w_opcode_encode (pCodeStream, W_RET) == NULL)
    w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);

  return NULL;
}

D_BOOL
translate_bool_exp (struct ParserState* pState, YYSTYPE exp)
{
  struct ExpResultType expType;

  assert (exp->val_type == VAL_EXP_LINK);
  expType = translate_exp_tree (pState,
                                pState->pCurrentStmt,
                                 &(exp->val.u_exp));
  if (expType.type == T_UNKNOWN)
    {
      /* Some error was encounter evaluating expression.
       * The error should be already logged */
      assert (pState->abortError == TRUE);
      return FALSE;
    }
  else if (GET_TYPE (expType.type) != T_BOOL)
    {
      w_log_msg (pState, pState->bufferPos, MSG_EXP_NOT_BOOL);
      return FALSE;
    }

  free_sem_value (exp);

  return TRUE;
}

YYSTYPE
create_arg_link (struct ParserState* pState,
                 YYSTYPE             argument,
                 YYSTYPE             next)
{
  struct SemValue* const result = alloc_sem_value (pState);

  assert ((argument != NULL) && (argument->val_type == VAL_EXP_LINK));
  assert ((next == NULL) || (next->val_type = VAL_PRC_ARG_LINK));

  if (result == NULL)
    {
      w_log_msg (pState, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return NULL;
    }

  result->val_type        = VAL_PRC_ARG_LINK;
  result->val.u_args.expr = argument;
  result->val.u_args.next = next;

  return result;
}
