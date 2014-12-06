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

#include <assert.h>

#include "whais.h"

#include "utils/endianness.h"
#include "compiler/wopcodes.h"

#include "expression.h"
#include "wlog.h"
#include "statement.h"
#include "vardecl.h"
#include "opcodes.h"
#include "procdecl.h"


YYSTYPE
create_exp_link( struct ParserState* const parser,
                 YYSTYPE                   firstOp,
                 YYSTYPE                   secondOp,
                 YYSTYPE                   thirdOp,
                 const enum EXP_OPERATION  opcode)
{
  struct SemValue* const result = alloc_sem_value( parser);

  if (result == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return NULL;
    }

  /* Check if it's a special case of comparing against NULL values. */
  if ((opcode == OP_EQ) || (opcode == OP_NE))
    {
      const struct SemExpression* exp = &firstOp->val.u_exp;

      assert( firstOp->val_type == VAL_EXP_LINK);
      assert( secondOp->val_type == VAL_EXP_LINK);

      if ((exp->opcode == OP_NULL) && (exp->firstTree == NULL))
        {
          assert( exp->secondTree == NULL);
          assert( exp->thirdTree == NULL);
          assert( thirdOp == NULL);

          result->val_type             = VAL_EXP_LINK;
          result->val.u_exp.firstTree  = secondOp;
          result->val.u_exp.secondTree = NULL;
          result->val.u_exp.thirdTree  = NULL;
          result->val.u_exp.opcode     = (opcode == OP_EQ) ?
                                          OP_INULL :
                                          OP_NNULL;
          free_sem_value( firstOp);

          return result;
        }

      exp = &secondOp->val.u_exp;
      if ((exp->opcode == OP_NULL) && (exp->firstTree == NULL))
        {
          assert( exp->secondTree == NULL);
          assert( exp->thirdTree == NULL);
          assert( thirdOp == NULL);

          result->val_type             = VAL_EXP_LINK;
          result->val.u_exp.firstTree  = firstOp;
          result->val.u_exp.secondTree = NULL;
          result->val.u_exp.thirdTree  = NULL;
          result->val.u_exp.opcode     = (opcode == OP_EQ) ?
                                          OP_INULL :
                                          OP_NNULL;
          free_sem_value( secondOp);

          return result;
        }
    }

  result->val_type             = VAL_EXP_LINK;
  result->val.u_exp.firstTree  = firstOp;
  result->val.u_exp.secondTree = secondOp;
  result->val.u_exp.thirdTree  = thirdOp;
  result->val.u_exp.opcode     = opcode;

  return result;
}


static bool_t
is_unsigned( uint_t type)
{
  return( (type >= T_UINT8) && (type <= T_UINT64));
}

static bool_t
is_signed( uint_t type)
{
  return( (type >= T_INT8) && (type <= T_INT64));
}


static bool_t
is_integer( uint_t type)
{
  return is_unsigned( type) || is_signed( type);
}



struct ExpResultType
{
  const struct DeclaredVar* extra; /* only for table types */
  uint_t                    type;
};


static const struct ExpResultType sgResultUnk = { NULL, T_UNKNOWN };



static struct ExpResultType
translate_tree_exp( struct ParserState* const     parser,
                    struct Statement* const       stmt,
                    struct SemExpression* const   tree);



static bool_t
is_leaf_exp( struct SemExpression *exp)
{
  return( exp->opcode == OP_NULL) && (exp->secondTree == NULL);
}


static const struct DeclaredVar*
find_field( const char* const           label,
            const uint_t                labelLen,
            const struct DeclaredVar*   fieldsList)
{
  assert( fieldsList != NULL);

  while( IS_TABLE_FIELD( fieldsList->type))
    {
      if ((fieldsList->labelLength == labelLen)
          && (memcmp( fieldsList->label, label, labelLen) == 0))
        {
          break;
        }
      fieldsList = fieldsList->extra;
    }

  if (IS_TABLE_FIELD( fieldsList->type) == FALSE)
    {
      assert( IS_TABLE( fieldsList->type));

      /* The field was not found .*/
      return NULL;
    }

  return fieldsList;
}


static bool_t
are_fields_compatible( const struct DeclaredVar* const field1,
                       const struct DeclaredVar* const field2)
{
  const uint_t baseType1 = GET_BASIC_TYPE( field1->type);
  const uint_t baseType2 = GET_BASIC_TYPE( field2->type);

  assert( IS_TABLE_FIELD( field1->type));
  assert( IS_TABLE_FIELD( field2->type));

  assert( baseType1 < T_UNDETERMINED);
  assert( baseType2 < T_UNDETERMINED);

  if ( IS_ARRAY( GET_FIELD_TYPE( field1->type)) !=
       IS_ARRAY( GET_FIELD_TYPE( field2->type)) )
    {
      return FALSE;
    }
  else if (store_op[baseType1][baseType2] == W_NA)
    return FALSE;

  return TRUE;
}


static const char*
array_to_text( uint_t type)
{
  assert( IS_ARRAY( type));
  type = GET_BASIC_TYPE( type);

  assert( type > T_UNKNOWN || type <= T_UNDETERMINED);

  if (type == T_BOOL)
    return "ARRAY OF BOOL";

  else if (type == T_CHAR)
    return "ARRAY OF CHAR";

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
    return "ARRAY OF UINT8";

  else if (type == T_UINT16)
    return "ARRAY OF UINT16";

  else if (type == T_UINT32)
    return "ARRAY OF UINT32";

  else if (type == T_UINT64)
    return "ARRAY OF UINT64";

  return "ARRAY OF UNDEFINED";
}


static const char*
field_to_text( uint_t type)
{
  assert( IS_FIELD( type));

  type = GET_FIELD_TYPE( type);

  assert( (GET_BASIC_TYPE( type) > T_UNKNOWN)
          && (GET_BASIC_TYPE( type) <= T_UNDETERMINED));

  if (type == T_BOOL)
    return "FIELD OF BOOL";

  else if (type == T_CHAR)
    return "FIELD OF CHAR";

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
    return "FIELD OF UINT8";

  else if (type == T_UINT16)
    return "FIELD OF UINT16";

  else if (type == T_UINT32)
    return "FIELD OF UINT32";

  else if (type == T_UINT64)
    return "FIELD OF UINT64";

  else if (IS_ARRAY( type))
    {
      type = GET_BASIC_TYPE( type);
      assert( type > T_UNKNOWN || type <= T_UNDETERMINED);

      if (type == T_BOOL)
        return "FIELD OF ARRAY OF BOOL";

      else if (type == T_CHAR)
        return "FIELD OF ARRAY OF CHAR";

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
        return "FIELD OF ARRAY OF UINT8";

      else if (type == T_UINT16)
        return "FIELD OF ARRAY OF UINT16";

      else if (type == T_UINT32)
        return "FIELD OF ARRAY OF UINT32";

      else if (type == T_UINT64)
        return "FIELD OF ARRAY OF UINT64";

      else if (type == T_UNDETERMINED)
	return "FIELD OF ARRAY OF UNDEFINED";
    }

  return "FIELD OF UNDEFINED";
}


static const char*
type_to_text( uint_t type)
{
  type = GET_TYPE( type);

  if (type == T_BOOL)
    return "BOOL";

  else if (type == T_CHAR)
    return "CHAR";

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
    return "UINT8";

  else if (type == T_UINT16)
    return "UINT16";

  else if (type == T_UINT32)
    return "UINT32";

  else if (type == T_UINT64)
    return "UINT64";

  else if (type == T_UNDETERMINED)
    return "UNDEFINED";

  else if (IS_FIELD( type))
    return field_to_text( type);

  else if (IS_ARRAY( type))
    return array_to_text( type);

  else if (IS_TABLE( type))
    return "TABLE";

  assert( FALSE);

  return NULL;
}


static struct ExpResultType
translate_not_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  enum W_OPCODE               opcode  = W_NA;
  const uint_t                ftype   = GET_TYPE( opType->type);

  assert( stmt->type == STMT_PROC);

  if (ftype == T_BOOL)
    opcode = W_NOTB;

  else if (is_integer( ftype))
    opcode = W_NOT;

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_NOT_NA,
                   type_to_text( opType->type));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  return *opType;
}


static struct ExpResultType
translate_chknull_exp( struct ParserState* const parser,
                       const bool_t              positive)
{
  struct Statement* const     stmt   = parser->pCurrentStmt;
  struct WOutputStream* const instrs = stmt_query_instrs( stmt);
  const struct ExpResultType  result = {NULL, T_BOOL};

  assert( stmt->type == STMT_PROC);

  if (encode_opcode( instrs, positive ? W_INULL : W_NNULL) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  return result;
}


static struct ExpResultType
translate_add_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  enum W_OPCODE               opcode  = W_NA;
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = add_op[GET_TYPE( opType1->type)][GET_TYPE( opType2->type)];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = add_op[ftype][ftype];

  if ((opcode == W_NA) && (ftype == T_UNDETERMINED))
    opcode = add_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_ADD_NA,
                   type_to_text( opType1->type),
                   type_to_text( opType2->type));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  switch( opcode)
    {
    case W_ADD:
      if (is_unsigned( GET_TYPE( opType1->type)) !=
          is_unsigned( GET_TYPE( opType2->type)))
        {
          log_message( parser, parser->bufferPos, MSG_ADD_SIGN);
          result.type = T_UINT64;
        }
      else if (is_unsigned( GET_TYPE( opType1->type)))
        result.type = T_UINT64;

      else
        result.type = T_INT64;

      break;

    case W_ADDRR:
      result.type = T_RICHREAL;
      break;

    case W_ADDT:
      result.type = T_TEXT;
      break;

    default:
      assert( 0);
    }

  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_sub_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt     = parser->pCurrentStmt;
  struct WOutputStream* const instrs   = stmt_query_instrs( stmt);
  const uint_t                ftype    = GET_TYPE( opType1->type);
  const uint_t                stype    = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode   = W_NA;
  struct ExpResultType        result;


  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = sub_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = sub_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = sub_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SUB_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  switch( opcode)
    {
    case W_SUB:
      if (is_unsigned( ftype) != is_unsigned( stype))
        {
          log_message( parser, parser->bufferPos, MSG_SUB_SIGN);
          result.type = T_UINT64;
        }
      else if (is_unsigned( ftype))
        result.type = T_UINT64;

      else
        result.type = T_INT64;

      break;

    case W_SUBRR:
      result.type = T_RICHREAL;
      break;

    default:
      assert( 0);
    }

  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_mul_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = mul_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = mul_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = mul_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_MUL_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  switch( opcode)
    {
    case W_MUL:
      if (is_unsigned( ftype) != is_unsigned( stype))
        {
          log_message( parser, parser->bufferPos, MSG_MUL_SIGN);
          result.type = T_UINT64;
        }
      else if (is_unsigned( ftype))
        result.type = T_UINT64;

      else
        result.type = T_INT64;

      break;

    case W_MULRR:
      result.type = T_RICHREAL;
      break;

    default:
      assert( 0);
    }

  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_div_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;


  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = div_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = div_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = div_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_DIV_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  switch( opcode)
    {
    case W_DIV:
      if (is_unsigned( ftype) != is_unsigned( stype))
        {
          log_message( parser, parser->bufferPos, MSG_DIV_SIGN);
          result.type = T_UINT64;
        }
      else if (is_unsigned( ftype))
        result.type = T_UINT64;
      else
        result.type = T_INT64;
      break;

    case W_DIVRR:
      result.type = T_RICHREAL;
      break;

    default:
      assert( 0);
    }

  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_mod_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt     = parser->pCurrentStmt;
  struct WOutputStream* const instrs   = stmt_query_instrs( stmt);
  const uint_t                ftype    = GET_TYPE( opType1->type);
  const uint_t                stype    = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode   = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = mod_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = mod_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = mod_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_MOD_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  assert( opcode == W_MOD);

  result.type  = T_UINT64;
  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_less_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = less_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = less_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = less_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_LT_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  assert( (opcode == W_LT) || (opcode == W_LTRR) || (opcode == W_LTC)
          || (opcode == W_LTD) || (opcode == W_LTDT) || (opcode == W_LTHT));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_exp_less_equal( struct ParserState* const         parser,
                          const struct ExpResultType* const opType1,
                          const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt   = parser->pCurrentStmt;
  struct WOutputStream* const instrs = stmt_query_instrs( stmt);
  const uint_t                ftype  = GET_TYPE( opType1->type);
  const uint_t                stype  = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = less_eq_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = less_eq_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = less_eq_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_LE_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  assert( (opcode == W_LE) || (opcode == W_LEC) || (opcode == W_LED)
          || (opcode == W_LEDT) || (opcode == W_LEHT) || (opcode == W_LERR));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_greater_exp( struct ParserState* const         parser,
                      const struct ExpResultType* const opType1,
                      const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = greater_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = greater_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = greater_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_GT_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  assert( (opcode == W_GT) || (opcode == W_GTC) || (opcode == W_GTD)
          || (opcode == W_GTDT) || (opcode == W_GTHT) || (opcode == W_GTRR));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_exp_greater_equal( struct ParserState* const         parser,
                             const struct ExpResultType* const opType1,
                             const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = greater_eq_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = greater_eq_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = greater_eq_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_GE_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  assert( (opcode == W_GE) || (opcode == W_GEC) || (opcode == W_GED)
          || (opcode == W_GEDT) || (opcode == W_GEHT) || (opcode == W_GERR));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_equals_exp( struct ParserState* const         parser,
                      const struct ExpResultType* const opType1,
                      const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt     = parser->pCurrentStmt;
  struct WOutputStream* const instrs   = stmt_query_instrs( stmt);
  const uint_t                ftype    = GET_TYPE( opType1->type);
  const uint_t                stype    = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode   = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = equals_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = equals_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = equals_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_EQ_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  assert( (opcode == W_EQ) || (opcode == W_EQC) || (opcode == W_EQD)
          || (opcode == W_EQDT) || (opcode == W_EQHT) || (opcode == W_EQRR)
          || (opcode == W_EQB) || (opcode == W_EQT));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_exp_not_equals( struct ParserState* const         parser,
                          const struct ExpResultType* const opType1,
                          const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = not_equals_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = not_equals_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = not_equals_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_NE_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  assert( (opcode == W_NE) || (opcode == W_NERR) || (opcode == W_NEC)
          || (opcode == W_NED) || (opcode == W_NEDT) ||(opcode == W_NEHT)
          || (opcode == W_NEB) || (opcode == W_NET));

  result.type  = T_BOOL;
  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_or_exp( struct ParserState* const         parser,
                  const struct ExpResultType* const opType1,
                  const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = or_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = or_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = or_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_OR_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  assert( (opcode == W_OR) || (opcode == W_ORB));

  if (opcode == W_OR)
    {
      assert( is_integer( stype));

      result.type = T_UINT64;
    }
  else
    {
      assert( (ftype == T_BOOL) && (stype == T_BOOL) && (opcode == W_ORB));

      result.type = T_BOOL;
    }

  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_and_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = and_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = and_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = and_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_AND_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  if (opcode == W_AND)
    {
      assert( is_integer( stype));
      result.type = T_UINT64;
    }
  else
    {
      assert( (ftype == T_BOOL) && (stype == T_BOOL) && (opcode == W_ANDB));
      result.type = T_BOOL;
    }

  result.extra = NULL;

  return result;
}


static struct ExpResultType
translate_xor_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((ftype < T_END_OF_TYPES) && (stype < T_END_OF_TYPES))
    opcode = xor_op[ftype][stype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = xor_op[ftype][ftype];

  if ((opcode == W_NA) && (stype == T_UNDETERMINED))
    opcode = xor_op[stype][stype];

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_XOR_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  if (opcode == W_XOR)
    {
      assert( is_integer( stype));
      result.type = T_UINT64;
    }
  else
    {
      assert( (ftype == T_BOOL) && (stype == T_BOOL) && (opcode == W_XORB));
      result.type = T_BOOL;
    }

  result.extra = NULL;

  return result;
}


static bool_t
are_compatible_tables( struct ParserState* const         parser,
                       const struct ExpResultType* const table1,
                       const struct ExpResultType* const table2)
{
  const struct DeclaredVar* field1  = NULL;
  const struct DeclaredVar* field2 = NULL;

  assert( IS_TABLE( table1->type));

  if (IS_TABLE( table2->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_CONTAINER_NA);
      return FALSE;
    }

  field1 = table1->extra;

  if (field1 == NULL)
    return TRUE;

  field2 = table2->extra;

  while( IS_TABLE_FIELD( field1->type))
    {
      char temp[128];

      const struct DeclaredVar* found = find_field( field1->label,
                                                    field1->labelLength,
                                                    field2);

      assert( (found == NULL) || IS_TABLE_FIELD( found->type));

      if (found == NULL)
        {
          log_message( parser,
                       parser->bufferPos,
                       MSG_NO_FIELD,
                       wh_copy_first( temp,
                                      field1->label,
                                      sizeof temp,
                                      field1->labelLength));
          return FALSE;
        }
      else if (are_fields_compatible( field1, found) == FALSE)
        {
          log_message( parser,
                       parser->bufferPos,
                       MSG_FIELD_NA,
                       wh_copy_first( temp,
                                      field1->label,
                                      sizeof temp,
                                      field1->labelLength),
                       type_to_text( field1->type),
                       type_to_text( found->type));
          parser->abortError = TRUE;

          return FALSE;
        }
      field1 = field1->extra;
    }

  return TRUE;
}


static struct ExpResultType
translate_store_exp( struct ParserState* const         parser,
                     const struct ExpResultType* const opType1,
                     const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_STORE_ELV);
      return sgResultUnk;
    }

  if (stype != T_UNDETERMINED)
    {

      if ((ftype < T_UNDETERMINED) && (stype < T_UNDETERMINED))
        opcode = store_op[ftype][stype];

      else if (IS_TABLE( ftype) && IS_TABLE( stype))
        {
          if ( ! are_compatible_tables( parser, opType1, opType2))
            return sgResultUnk;

          opcode = W_STTA;
        }
      else if (IS_FIELD( ftype) && IS_FIELD( stype))
        {
          if ((GET_FIELD_TYPE( ftype) == T_UNDETERMINED) ||
              (GET_FIELD_TYPE( ftype) == GET_FIELD_TYPE( stype)))
            {
              opcode = W_STF;
            }
        }
      else if (IS_ARRAY( ftype) && IS_ARRAY( stype))
        {
          const uint_t temp_ftype = GET_BASIC_TYPE( ftype);
          const uint_t temp_stype = GET_BASIC_TYPE( stype);

          assert( temp_ftype <= T_UNDETERMINED);
          assert( temp_stype <= T_UNDETERMINED);

          if ((temp_ftype == T_UNDETERMINED) || (temp_ftype == temp_stype))
            opcode = W_STA;
        }
      else if (ftype == T_UNDETERMINED)
        opcode = W_STUD; /* store an alias of the second object */
    }
  else
    {
      /* a NULL value is assigned */
      if (ftype < T_UNDETERMINED)
        opcode = store_op[ftype][ftype];

      else if (IS_TABLE( ftype))
        opcode = W_STTA;

      else if (IS_ARRAY( ftype))
        opcode = W_STA;

      else if (IS_FIELD( ftype))
        opcode = W_STF;

      else
        {
          /* Store an alias of a undefined object object! */
          assert( ftype == T_UNDETERMINED);

          opcode = W_STUD;
        }
    }

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_STORE_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  return result;
}


static struct ExpResultType
translate_sadd_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SADD_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype))
    {
      if (is_integer( stype))
        opcode = W_SADD;
    }
  else if ((ftype == T_REAL) || (ftype == T_RICHREAL))
    {
      if (is_integer( stype))
        opcode = W_SADD;

      else if ((stype == T_REAL) || (stype == T_RICHREAL))
        opcode = W_SADDRR;
    }
  else if (ftype == T_TEXT)
    {
      if (stype == T_CHAR)
        opcode = W_SADDC;

      else if (stype == T_TEXT)
        opcode = W_SADDT;
    }

  if (opcode == W_NA)
    {
      log_message( parser,
                 parser->bufferPos,
                 MSG_SADD_NA,
                 type_to_text( ftype),
                 type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}


static struct ExpResultType
translate_ssub_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SSUB_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype))
    {
      if (is_integer( stype))
        opcode = W_SSUB;
    }
  else if ((ftype == T_REAL) || (ftype == T_RICHREAL))
    {
      if (is_integer( stype))
        opcode = W_SSUB;

      else if ((stype == T_REAL) || (stype == T_RICHREAL))
        opcode = W_SSUBRR;
    }

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SSUB_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}

static struct ExpResultType
translate_smul_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SMUL_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype))
    {
      if (is_integer( stype))
        opcode = W_SMUL;
    }
  else if ((ftype == T_REAL) || (ftype == T_RICHREAL))
    {
      if (is_integer( stype))
        opcode = W_SMUL;

      else if ((stype == T_REAL) || (stype == T_RICHREAL))
        opcode = W_SMULRR;
    }

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SMUL_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}


static struct ExpResultType
translate_sdiv_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const    stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs = stmt_query_instrs( stmt);
  const uint_t               ftype   = GET_TYPE( opType1->type);
  const uint_t               stype   = GET_TYPE( opType2->type);
  enum W_OPCODE              opcode  = W_NA;
  struct ExpResultType       result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SDIV_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype))
    {
      if (is_integer( stype))
        opcode = W_SDIV;
    }
  else if ((ftype == T_REAL) || (ftype == T_RICHREAL))
    {
      if (is_integer( stype))
        opcode = W_SDIV;

      else if ((stype == T_REAL) || (stype == T_RICHREAL))
        opcode = W_SDIVRR;
    }

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SDIV_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}


static struct ExpResultType
translate_smod_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SMOD_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype) && is_integer( stype))
    opcode = W_SMOD;

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SMOD_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}


static struct ExpResultType
translate_sand_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SAND_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype) && is_integer( stype))
    opcode = W_SAND;

  else if ((ftype == T_BOOL) && (stype == T_BOOL))
    opcode = W_SANDB;

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SAND_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}


static struct ExpResultType
translate_sxor_exp( struct ParserState* const         parser,
                    const struct ExpResultType* const opType1,
                    const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt   = parser->pCurrentStmt;
  struct WOutputStream* const instrs = stmt_query_instrs( stmt);
  const uint_t                ftype  = GET_TYPE( opType1->type);
  const uint_t                stype  = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SXOR_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype) && is_integer( stype))
    opcode = W_SXOR;

  else if ((ftype == T_BOOL) && (stype == T_BOOL))
    opcode = W_SXORB;

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SXOR_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}


static struct ExpResultType
translate_sor_exp( struct ParserState* const         parser,
                   const struct ExpResultType* const opType1,
                   const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if (IS_L_VALUE( opType1->type) == FALSE)
    {
      log_message( parser, parser->bufferPos, MSG_SOR_ELV);
      return sgResultUnk;
    }

  if (is_integer( ftype) && is_integer( stype))
    opcode = W_SOR;

  else if ((ftype == T_BOOL) && (stype == T_BOOL))
    opcode = W_SORB;

  if (opcode == W_NA)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_SOR_NA,
                   type_to_text( ftype),
                   type_to_text( stype));

      return sgResultUnk;
    }

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return sgResultUnk;
    }

  result.type  = GET_TYPE( opType1->type);
  result.extra = opType1->extra;

  MARK_L_VALUE( result.type);

  return result;
}


static struct ExpResultType
translate_index_exp( struct ParserState* const         parser,
                     const struct ExpResultType* const opType1,
                     const struct ExpResultType* const opType2)
{
  struct Statement* const     stmt    = parser->pCurrentStmt;
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const uint_t                ftype   = GET_TYPE( opType1->type);
  const uint_t                stype   = GET_TYPE( opType2->type);
  enum W_OPCODE               opcode  = W_NA;
  struct ExpResultType        result;

  if ((IS_FIELD( ftype) == FALSE)
      && (IS_ARRAY( ftype) == FALSE)
      && ((ftype != T_TEXT)))
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_INDEX_EAT,
                   type_to_text( ftype));
      parser->abortError = TRUE;

      return sgResultUnk;
    }

  if ((is_integer( stype) == FALSE) && (stype != T_UNDETERMINED))
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_INDEX_ENI,
                   type_to_text( stype));
      parser->abortError = TRUE;

      return sgResultUnk;
    }
  else if (IS_FIELD( ftype))
    {
      assert( opType1->extra == NULL);

      opcode      = W_INDF;
      result.type = GET_FIELD_TYPE( ftype);

      assert( IS_ARRAY( result.type)
              || ((T_UNKNOWN < result.type)
                  && (result.type <= T_UNDETERMINED)));

      if (result.type == T_UNDETERMINED)
        {
          log_message( parser, parser->bufferPos, MSG_INDEX_UNF);
          parser->abortError = TRUE;

          return sgResultUnk;
        }
    }
  else if (IS_ARRAY( ftype))
    {
      assert( opType1->extra == NULL);

      opcode      = W_INDA;
      result.type = GET_BASIC_TYPE( ftype);

      if (result.type == T_UNDETERMINED)
        {
          log_message( parser, parser->bufferPos, MSG_INDEX_UNA);
          parser->abortError = TRUE;

          return sgResultUnk;
        }
    }
  else
    {
      assert( ftype == T_TEXT);

      opcode      = W_INDT;
      result.type = T_CHAR;
    }

  assert( opcode != W_NA);

  if (encode_opcode( instrs, opcode) == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      parser->abortError = TRUE;

      return sgResultUnk;
    }

  MARK_L_VALUE( result.type);

  result.extra = NULL;

  return result;
}

static struct ExpResultType
translate_opcode_exp( struct ParserState* const   parser,
                      struct Statement* const     stmt,
                      const uint16_t              opcode,
                      const struct ExpResultType* opType1,
                      const struct ExpResultType* opType2)
{
  struct ExpResultType result = { NULL, T_UNKNOWN };

  switch( opcode)
    {
    case OP_ADD:
      result = translate_add_exp( parser, opType1, opType2);
      break;

    case OP_SUB:
      result = translate_sub_exp( parser, opType1, opType2);
      break;

    case OP_MUL:
      result = translate_mul_exp( parser, opType1, opType2);
      break;

    case OP_DIV:
      result = translate_div_exp( parser, opType1, opType2);
      break;

    case OP_MOD:
      result = translate_mod_exp( parser, opType1, opType2);
      break;

    case OP_LT:
      result = translate_less_exp( parser, opType1, opType2);
      break;

    case OP_LE:
      result = translate_exp_less_equal( parser, opType1, opType2);
      break;

    case OP_GT:
      result = translate_greater_exp( parser, opType1, opType2);
      break;

    case OP_GE:
      result = translate_exp_greater_equal( parser, opType1, opType2);
      break;

    case OP_EQ:
      result = translate_equals_exp( parser, opType1, opType2);
      break;

    case OP_NE:
      result = translate_exp_not_equals( parser, opType1, opType2);
      break;

    case OP_INULL:
      result = translate_chknull_exp( parser, TRUE);
      break;

    case OP_NNULL:
      result = translate_chknull_exp( parser, FALSE);
      break;

    case OP_NOT:
      result = translate_not_exp( parser, opType1);
      break;

    case OP_OR:
      result = translate_or_exp( parser, opType1, opType2);
      break;

    case OP_AND:
      result = translate_and_exp( parser, opType1, opType2);
      break;

    case OP_XOR:
      result = translate_xor_exp( parser, opType1, opType2);
      break;

    case OP_GROUP:
      result = *opType1;
      break;

    case OP_INDEX:
      result = translate_index_exp( parser, opType1, opType2);
      break;

    case OP_ATTR:
      result = translate_store_exp( parser, opType1, opType2);
      break;

    case OP_SADD:
      result = translate_sadd_exp( parser, opType1, opType2);
      break;

    case OP_SSUB:
      result = translate_ssub_exp( parser, opType1, opType2);
      break;

    case OP_SMUL:
      result = translate_smul_exp( parser, opType1, opType2);
      break;

    case OP_SDIV:
      result = translate_sdiv_exp( parser, opType1, opType2);
      break;

    case OP_SMOD:
      result = translate_smod_exp( parser, opType1, opType2);
      break;

    case OP_SAND:
      result = translate_sand_exp( parser, opType1, opType2);
      break;

    case OP_SXOR:
      result = translate_sxor_exp( parser, opType1, opType2);
      break;

    case OP_SOR:
      result = translate_sor_exp( parser, opType1, opType2);
      break;

    default:
      assert( 0);
    }

  return result;
}


static struct ExpResultType
translate_leaf_exp( struct ParserState* const parser,
                    struct Statement*         stmt,
                    struct SemValue* const    exp)
{
  struct WOutputStream* const instrs  = stmt_query_instrs( stmt);
  const struct ExpResultType  unk     = { NULL, T_UNKNOWN };
  const struct ExpResultType  undet   = { NULL, T_UNDETERMINED };
  struct ExpResultType        result  = sgResultUnk;

  if (exp == NULL)
    {
      if (encode_opcode( instrs, W_LDNULL) == NULL)
        {
          log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
          return sgResultUnk;
        }
      return undet;
    }

  if (exp->val_type == VAL_ID)
    {
      const uint16_t value_16 = ~0;
      const uint8_t  value_8  = ~0;

      enum W_OPCODE op_code = W_NA;
      uint32_t      value   = 0;

      struct DeclaredVar* var = stmt_find_declaration( stmt,
                                                       exp->val.u_id.name,
                                                       exp->val.u_id.length,
                                                       TRUE,
                                                       TRUE );
      if (var == NULL)
        {
          char temp[128];

          wh_copy_first( temp,
                         exp->val.u_id.name,
                         sizeof temp,
                         exp->val.u_id.length);

          log_message( parser, parser->bufferPos, MSG_VAR_NFOUND, temp);

          parser->abortError = TRUE;
          return unk;
        }

      value = RETRIVE_ID( var->varId);
      if (IS_GLOBAL( var->varId))
        {
          if (stmt->parent != NULL)
            {
              assert( stmt->type == STMT_PROC);
              stmt = stmt->parent;
            }
          assert( stmt->type == STMT_GLOBAL);
        }
      else
        value -= 1; /* Don't count the return value! */

      result.extra = var->extra;
      result.type  = var->type;
      MARK_L_VALUE( result.type);

      if (value <= value_8)
        {
          op_code = (stmt->type == STMT_GLOBAL) ? W_LDGB8 : W_LDLO8;

          if ((encode_opcode( instrs, op_code) == NULL)
              || (wh_ostream_wint8 (instrs, value & 0xFF) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
      else if (value <= value_16)
        {
          op_code  = (stmt->type == STMT_GLOBAL) ? W_LDGB16 : W_LDLO16;

          if ((encode_opcode( instrs, op_code) == NULL)
              || (wh_ostream_wint16 (instrs, value & 0xFFFF) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
      else
        {
          op_code  = (stmt->type == STMT_GLOBAL) ? W_LDGB32 : W_LDLO32;

          if ((encode_opcode( instrs, op_code) == NULL)
              || (wh_ostream_wint32 (instrs, value) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return sgResultUnk;
            }
        }
    }
  else if (exp->val_type == VAL_C_INT)
    {
      const uint32_t value_32 = ~0;
      const uint16_t value_16 = ~0;
      const uint8_t  value_8  = ~0;

      if (exp->val.u_int.value <= value_8)
        {
          result.type = exp->val.u_int.isSigned ? T_INT8 : T_UINT8;
          if ((encode_opcode( instrs, W_LDI8) == NULL)
              || (wh_ostream_wint8 (instrs,
                                    exp->val.u_int.value & 0xFF) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
      else if (exp->val.u_int.value <= value_16)
        {
          result.type = exp->val.u_int.isSigned ? T_INT16 : T_UINT16;
          if ((encode_opcode( instrs, W_LDI16) == NULL)
              || (wh_ostream_wint16 (instrs,
                                     exp->val.u_int.value & 0xFFFF) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
      else if (exp->val.u_int.value <= value_32)
        {
          result.type = exp->val.u_int.isSigned ? T_INT32 : T_UINT32;

          if ((encode_opcode( instrs, W_LDI32) == NULL)
              || (wh_ostream_wint32 (instrs,
                                    exp->val.u_int.value & 0xFFFFFFFF) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
      else
        {
          result.type = exp->val.u_int.isSigned ? T_INT64 : T_UINT64;
          if ((encode_opcode( instrs, W_LDI64) == NULL)
              || (wh_ostream_wint64 (instrs, exp->val.u_int.value) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
    }
  else if (exp->val_type == VAL_C_CHAR)
    {
      const uint32_t unicodeCh = exp->val.u_char.value;

      result.type = T_CHAR;
      if ((encode_opcode( instrs, W_LDC) == NULL)
          || (wh_ostream_wint32 (instrs, unicodeCh) == NULL))
        {
          log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

          return sgResultUnk;
        }
    }
  else if (exp->val_type == VAL_C_TIME)
    {
      struct SemCTime* const value = &exp->val.u_time;

      if (value->usec != 0)
        {
          result.type = T_HIRESTIME;
          if ((encode_opcode( instrs, W_LDHT) == NULL)
              || (wh_ostream_wint32 (instrs, value->usec) == NULL)
              || (wh_ostream_wint8 (instrs, value->sec) == NULL)
              || (wh_ostream_wint8 (instrs, value->min) == NULL)
              || (wh_ostream_wint8 (instrs, value->hour) == NULL)
              || (wh_ostream_wint8 (instrs, value->day) == NULL)
              || (wh_ostream_wint8 (instrs, value->month) == NULL)
              || (wh_ostream_wint16 (instrs, value->year) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
      else if ((value->sec != 0) || (value->min != 0) || (value->hour != 0))
        {
          result.type = T_DATETIME;
          if ((encode_opcode( instrs, W_LDDT) == NULL)
              || (wh_ostream_wint8 (instrs, value->sec) == NULL)
              || (wh_ostream_wint8 (instrs, value->min) == NULL)
              || (wh_ostream_wint8 (instrs, value->hour) == NULL)
              || (wh_ostream_wint8 (instrs, value->day) == NULL)
              || (wh_ostream_wint8 (instrs, value->month) == NULL)
              || (wh_ostream_wint16 (instrs, value->year) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

              return sgResultUnk;
            }
        }
      else
        {
          result.type = T_DATE;
          if ((encode_opcode( instrs, W_LDD) == NULL)
              || (wh_ostream_wint8 (instrs, value->day) == NULL)
              || (wh_ostream_wint8 (instrs, value->month) == NULL)
              || (wh_ostream_wint16 (instrs, value->year) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return sgResultUnk;
            }
        }
    }
  else if (exp->val_type == VAL_C_REAL)
    {
      result.type = T_RICHREAL;
      if ((encode_opcode( instrs, W_LDRR) == NULL)
          || (wh_ostream_wint64 (instrs, exp->val.u_real.integerPart) == NULL)
          || (wh_ostream_wint64 (instrs,
                                 exp->val.u_real.fractionalPart) == NULL))

        {
          log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

          return sgResultUnk;
        }
    }
  else if (exp->val_type == VAL_C_TEXT)
    {
      struct SemCText* const value    = &exp->val.u_text;
      int32_t                constPos = add_constant_text(
                                                  stmt,
                                                  (const uint8_t*)value->text,
                                                  value->length
                                                         );
      result.type = T_TEXT;
      if ((constPos < 0)
          || (encode_opcode( instrs, W_LDT) == NULL)
          || (wh_ostream_wint32 (instrs, constPos) == NULL))
        {
          log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

          return sgResultUnk;
        }
    }
  else if (exp->val_type == VAL_C_BOOL)
    {
      const enum W_OPCODE opcode = (exp->val.u_bool.value == FALSE) ?
                                    W_LDBF :
                                    W_LDBT;
      if (encode_opcode( instrs, opcode) == NULL)
        {
          log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

          return sgResultUnk;
        }
      result.type = T_BOOL;
    }
  else
    {
      assert( FALSE);

      log_message( parser, IGNORE_BUFFER_POS, MSG_INT_ERR);
      result.type = T_UNKNOWN;
    }

  free_sem_value( exp);
  return result;
}


static struct ExpResultType
translate_call_exp( struct ParserState* const   parser,
                    struct Statement* const     stmt,
                    struct SemExpression* const exp)
{
  struct WOutputStream* const instrs   = stmt_query_instrs( stmt);
  const struct Statement*     proc     = NULL;
  const struct DeclaredVar*   procVar  = NULL;
  struct SemValue*            expArg   = NULL;
  uint_t                      argCount = 0;
  struct ExpResultType        result;

  char temp[128];

  assert( exp->firstTree->val_type == VAL_ID);
  assert( (exp->secondTree == NULL) ||
          (exp->secondTree->val_type == VAL_PRC_ARG_LINK));

  proc = find_proc_decl( parser,
                         exp->firstTree->val.u_id.name,
                         exp->firstTree->val.u_id.length,
                         TRUE);
  if (proc == NULL)
    {
      log_message( parser, parser->bufferPos,
                   MSG_NO_PROC,
                   wh_copy_first( temp,
                                  exp->firstTree->val.u_id.name,
                                  sizeof temp,
                                  exp->firstTree->val.u_id.length));
      return sgResultUnk;
    }

  expArg = exp->secondTree;
  while( expArg != NULL)
    {
      struct SemValue* const param   = expArg->val.u_args.expr;
      struct SemValue*       tempVal = NULL;
      struct ExpResultType   argType;

      assert( expArg->val_type == VAL_PRC_ARG_LINK);
      assert( param->val_type == VAL_EXP_LINK);

      ++argCount;
      procVar = stmt_get_param( proc, argCount);

      if (procVar == NULL)
        {
          log_message( parser,
                     parser->bufferPos,
                     MSG_PROC_MORE_ARGS,
                     wh_copy_first( temp,
                                    exp->firstTree->val.u_id.name,
                                    sizeof temp,
                                    exp->firstTree->val.u_id.length),
                     stmt_get_param_count( proc));

          parser->abortError = TRUE;
          return sgResultUnk;
        }
      else
        {
          /* convert the declared variable to an expression result */
          argType.type  = GET_TYPE( procVar->type);
          argType.extra = procVar->extra;
        }

      result = translate_tree_exp( parser, stmt, &param->val.u_exp);
      if (result.type == T_UNKNOWN)
        {
          /* An error that must be propagated upwards. The error message
           * was logged during expression's evaluation. */
          assert( parser->abortError == TRUE);
          log_message( parser,
                       parser->bufferPos,
                       MSG_PROC_ARG_COUNT,
                       wh_copy_first( temp, proc->spec.proc.name,
                                      sizeof temp,
                                      proc->spec.proc.nameLength),
                                      argCount);

          return sgResultUnk;
        }
      else
        free_sem_value( param);

      if (GET_TYPE( result.type) != T_UNDETERMINED)
        {
          if (IS_FIELD( result.type))
            {
              const bool_t isArgArray = IS_ARRAY(
                                            GET_FIELD_TYPE( argType.type)
                                                 );

              const bool_t isArgUndet = isArgArray ?
                                          FALSE :
                                          (GET_BASIC_TYPE( argType.type) ==
                                              T_UNDETERMINED);

              assert( (isArgArray == FALSE) || (isArgUndet == FALSE));

              if ((IS_FIELD( argType.type) == FALSE)
                  || ((GET_FIELD_TYPE( result.type) !=
                        GET_FIELD_TYPE( argType.type))
                     && (isArgUndet == FALSE)))
                {
                  log_message( parser,
                               parser->bufferPos,
                               MSG_PROC_ARG_NA,
                               wh_copy_first( temp,
                                              proc->spec.proc.name,
                                              sizeof temp,
                                              proc->spec.proc.nameLength),
                               argCount,
                               type_to_text( result.type),
                               type_to_text( argType.type));

                  parser->abortError = TRUE;
                  return sgResultUnk;
                }
            }
          else if (IS_TABLE( result.type) == FALSE)
            {
              if (IS_ARRAY( result.type) == IS_ARRAY( argType.type))
                {
                  const uint_t        arg_t  = GET_BASIC_TYPE( argType.type);
                  const uint_t        res_t  = GET_BASIC_TYPE( result.type);
                  const enum W_OPCODE tempOp = store_op[arg_t][res_t];

                  assert( arg_t <= T_UNDETERMINED);
                  assert( res_t <= T_UNDETERMINED);

                  if (((IS_ARRAY( result.type) == FALSE) && (W_NA == tempOp))
                      || (IS_ARRAY( result.type)
                         && (arg_t != T_UNDETERMINED) && (arg_t != res_t)))
                    {
                      log_message( parser,
                                   parser->bufferPos,
                                   MSG_PROC_ARG_NA,
                                   wh_copy_first( temp,
                                                  proc->spec.proc.name,
                                                  sizeof temp,
                                                  proc->spec.proc.nameLength),
                                   argCount,
                                   type_to_text( result.type),
                                   type_to_text( argType.type));

                      return sgResultUnk;
                    }
                }
              else
                {
                  log_message( parser,
                               parser->bufferPos,
                               MSG_PROC_ARG_NA,
                               wh_copy_first( temp,
                                              proc->spec.proc.name,
                                              sizeof temp,
                                              proc->spec.proc.nameLength),
                               argCount,
                               type_to_text( result.type),
                               type_to_text( argType.type));

                  return sgResultUnk;
                }
            }
          else if ( ! are_compatible_tables( parser, &argType, &result))
            {
              /* The two containers's types are not compatible.
               * The error was already logged. */
              log_message( parser,
                           parser->bufferPos,
                           MSG_PROC_ARG_COUNT,
                           wh_copy_first( temp, proc->spec.proc.name,
                                          sizeof temp,
                                          proc->spec.proc.nameLength),
                           argCount);

              return sgResultUnk;
            }
        }

      tempVal = expArg;
      expArg  = expArg->val.u_args.next;

      free_sem_value( tempVal);
    }

  if (argCount < stmt_get_param_count( proc))
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_PROC_LESS_ARGS,
                   wh_copy_first( temp,
                                  exp->firstTree->val.u_id.name,
                                  sizeof temp,
                                  exp->firstTree->val.u_id.length),
                   stmt_get_param_count( proc),
                   argCount);

      return sgResultUnk;
    }
  else
    free_sem_value( exp->firstTree);

  if ((encode_opcode( instrs, W_CALL) == NULL)
      || (wh_ostream_wint32 (instrs, stmt_get_import_id( proc)) == NULL))
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
      return sgResultUnk;
    }

  procVar = stmt_get_param( proc, 0);
  if (procVar == 0)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_INTERNAL_ERROR);
      return sgResultUnk;
    }

  result.type  = GET_TYPE( procVar->type);
  result.extra = procVar->extra;

  return result;
}


static struct ExpResultType
translate_tabval_exp( struct ParserState* const   parser,
                      struct Statement* const     stmt,
                      struct SemExpression* const exp)
{
  const struct DeclaredVar*   fieldVar = NULL;
  struct WOutputStream* const instrs   = stmt_query_instrs(
                                                        parser->pCurrentStmt
                                                           );
  struct SemExpression* const expOp1 = &exp->firstTree->val.u_exp;
  struct SemExpression* const expOp2 = &exp->secondTree->val.u_exp;
  struct SemId* const         id     = &exp->thirdTree->val.u_id;

  struct ExpResultType  tableType;
  struct ExpResultType  expType;

  assert( exp->opcode == OP_TABVAL);
  assert( exp->firstTree->val_type == VAL_EXP_LINK);
  assert( exp->secondTree->val_type == VAL_EXP_LINK);
  assert( exp->thirdTree->val_type == VAL_ID);

  tableType = translate_tree_exp( parser, stmt, expOp1);
  if (tableType.type == T_UNKNOWN)
    {
      assert( parser->abortError);
      return sgResultUnk;
    }

  if (IS_TABLE( tableType.type) == FALSE)
    {
      log_message( parser,
                   parser->bufferPos,
                  MSG_MEMSEL_NA,
                  type_to_text( tableType.type));

      parser->abortError = TRUE;
      return sgResultUnk;
    }

  expType = translate_tree_exp( parser, stmt, expOp2);
  if (expType.type == T_UNKNOWN)
    {
      assert( parser->abortError);
      return sgResultUnk;
    }

  if ( ! is_integer( GET_TYPE( expType.type)))
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_INDEX_ENI,
                   type_to_text( expType.type));

      parser->abortError = TRUE;
      return sgResultUnk;
    }

  if (tableType.extra != NULL)
    fieldVar = find_field( id->name, id->length, tableType.extra);

  if (fieldVar == NULL)
    {
      char temp[128];

      wh_copy_first( temp, id->name, sizeof temp, id->length);
      log_message( parser, parser->bufferPos, MSG_MEMSEL_ERD, temp);

      parser->abortError = TRUE;
      return sgResultUnk;
    }

  {
    const int32_t constPos = add_constant_text( stmt,
                                                (const uint8_t*)id->name,
                                                id->length);

    if ((constPos < 0)
        || (encode_opcode( instrs, W_INDTA) == NULL)
        || (wh_ostream_wint32 (instrs, constPos) == NULL))
      {
        log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
        return sgResultUnk;
      }
  }

  expType.extra = NULL;
  expType.type  = GET_TYPE( fieldVar->type);

  MARK_L_VALUE( expType.type);

  free_sem_value( exp->firstTree);
  free_sem_value( exp->secondTree);
  free_sem_value( exp->thirdTree);

  return expType;
}


static struct ExpResultType
translate_field_exp( struct ParserState* const   parser,
                     struct Statement* const     stmt,
                     struct SemExpression* const exp)
{
  const struct DeclaredVar*    fieldVar = NULL;
  struct WOutputStream* const  instrs   = stmt_query_instrs(
                                                         parser->pCurrentStmt
                                                            );
  struct SemExpression* const  expOp1   = &exp->firstTree->val.u_exp;
  struct SemId* const          id       = &exp->secondTree->val.u_id;

  struct ExpResultType tableType;
  struct ExpResultType expType;

  assert( exp->opcode == OP_FIELD);
  assert( exp->firstTree->val_type == VAL_EXP_LINK);
  assert( exp->secondTree->val_type == VAL_ID);

  tableType = translate_tree_exp( parser, stmt, expOp1);
  if (tableType.type == T_UNKNOWN)
    {
      assert( parser->abortError);
      return sgResultUnk;
    }

  if (IS_TABLE( tableType.type) == FALSE)
    {
      log_message( parser,
                   parser->bufferPos,
                   MSG_MEMSEL_NA,
                   type_to_text( tableType.type));

      parser->abortError = TRUE;
      return sgResultUnk;
    }

  if (tableType.extra != NULL)
    fieldVar = find_field( id->name, id->length, tableType.extra);

  if (fieldVar == NULL)
    {
      char temp[128];

      wh_copy_first( temp, id->name, sizeof temp, id->length);
      log_message( parser, parser->bufferPos, MSG_MEMSEL_ERD, temp);

      parser->abortError = TRUE;
      return sgResultUnk;
    }

  {
    const int32_t constPos = add_constant_text( stmt,
                                                (const uint8_t*)id->name,
                                                id->length);

    if ((constPos < 0)
        || (encode_opcode( instrs, W_SELF) == NULL)
        || (wh_ostream_wint32 (instrs, constPos) == NULL))
      {
        log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
        return sgResultUnk;
      }
  }

  expType.extra = NULL;
  expType.type  = GET_TYPE( fieldVar->type);

  MARK_FIELD( expType.type);

  free_sem_value( exp->firstTree);
  free_sem_value( exp->secondTree);

  return expType;
}


static struct ExpResultType
translate_tree_exp( struct ParserState* const   parser,
                    struct Statement* const     stmt,
                    struct SemExpression* const tree)
{
  struct WOutputStream* const instrs = stmt_query_instrs(
                                                      parser->pCurrentStmt
                                                         );

  bool_t needsJmpAdjust = FALSE;
  int    jmpPosition;
  int    jmpDataPos;

  struct ExpResultType opType1;
  struct ExpResultType opType2;

  assert( parser->pCurrentStmt->type == STMT_PROC);

  if (is_leaf_exp( tree))
    return translate_leaf_exp( parser, stmt, tree->firstTree);

  else if (tree->opcode == OP_CALL)
    return translate_call_exp( parser, stmt, tree);

  else if (tree->opcode == OP_TABVAL)
    return translate_tabval_exp( parser, stmt, tree);

  else if (tree->opcode == OP_FIELD)
    return translate_field_exp( parser, stmt, tree);

  assert( tree->firstTree->val_type == VAL_EXP_LINK);

  opType1 = translate_tree_exp( parser,
                                stmt,
                                &(tree->firstTree->val.u_exp));
  if (opType1.type == T_UNKNOWN)
    {
      assert( parser->abortError);
      /* something went wrong, and the error
       * should be already logged  */
      return sgResultUnk;
    }

  free_sem_value( tree->firstTree);

  if (GET_TYPE( opType1.type) == T_BOOL)
    {
      /* Handle special case for OR or AND with boolean types not to evaluate
       * the second expression when is unnecessary!
       * Use 0 for jump offset just to reserve the space. It will be corrected
       * after we parse the second expression */
      jmpPosition = wh_ostream_size( instrs);
      if (tree->opcode == OP_OR)
        {
          if ((encode_opcode( instrs, W_JT) == NULL)
              || (wh_ostream_wint32 (instrs, 0) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return sgResultUnk;
            }
          needsJmpAdjust = TRUE;
        }
      else if (tree->opcode == OP_AND)
        {
          if ((encode_opcode( instrs, W_JF) == NULL)
              || (wh_ostream_wint32 (instrs, 0) == NULL))
            {
              log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);
              return sgResultUnk;
            }
          needsJmpAdjust = TRUE;
        }
      jmpDataPos = wh_ostream_size( instrs) - sizeof( uint32_t);
    }

  if (tree->secondTree != NULL)
    {
      assert( tree->secondTree->val_type == VAL_EXP_LINK);
      opType2 = translate_tree_exp( parser,
                                    stmt,
                                    &(tree->secondTree->val.u_exp));
      if (opType2.type == T_UNKNOWN)
        {
          /* something went wrong, and the error
           * should be already signaled  */
          return sgResultUnk;
        }
      free_sem_value( tree->secondTree);
    }

  /* use second_type to store result */
  opType2 = translate_opcode_exp( parser,
                                  stmt,
                                  tree->opcode,
                                  &opType1,
                                  &opType2);

  if (needsJmpAdjust && (GET_TYPE( opType2.type) == T_BOOL))
    {
      /* Lets correct some jumps offsets */
      int currentPos = wh_ostream_size( instrs) - jmpPosition;

      uint8_t* const code  = wh_ostream_data( instrs);
      store_le_int32 (currentPos, code + jmpDataPos);
    }

  return opType2;
}


YYSTYPE
translate_exp( struct ParserState* const parser, YYSTYPE exp)
{
  struct Statement* const     stmt   = parser->pCurrentStmt;
  struct WOutputStream* const instrs = stmt_query_instrs( stmt);

  assert( exp->val_type = VAL_EXP_LINK);
  assert( stmt->type == STMT_PROC);

  translate_tree_exp( parser, stmt, &(exp->val.u_exp));

  free_sem_value( exp);

  if (encode_opcode( instrs, W_CTS) == NULL)
    log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

  return NULL;
}


YYSTYPE
translate_return_exp( struct ParserState* const parser, YYSTYPE exp)
{
  struct Statement* const         stmt    = parser->pCurrentStmt;
  struct WOutputStream* const     instrs  = stmt_query_instrs( stmt);
  const struct DeclaredVar* const pRetVar = stmt_get_param( stmt, 0);

  struct ExpResultType retType = sgResultUnk;
  struct ExpResultType expType = sgResultUnk;

  assert( exp->val_type = VAL_EXP_LINK);
  assert( stmt->type == STMT_PROC);

  expType = translate_tree_exp( parser, stmt, &(exp->val.u_exp));
  if (expType.type == T_UNKNOWN)
    {
      /* Some error has been encountered evaluating of return expression. */
      assert( parser->abortError != FALSE);
      return NULL;
    }

  free_sem_value( exp);

  /* Convert the declared return type to an expression result type. */
  retType.type  = GET_TYPE( pRetVar->type);
  retType.extra = pRetVar->extra;

  /* Verify the type of the returned expression if the procedure's return type
     is defined and the returned type is not a NULL value. */
  if ((retType.type != T_UNDETERMINED)
      && (expType.type != T_UNDETERMINED))
    {
      if ((IS_TABLE( retType.type) != IS_TABLE( expType.type))
          || (IS_FIELD( retType.type) != IS_FIELD( expType.type))
          || (IS_ARRAY( retType.type) != IS_ARRAY( expType.type)))
        {
          log_message( parser,
                       parser->bufferPos,
                       MSG_PROC_RET_NA_EXT,
                       type_to_text( GET_TYPE( retType.type)),
                       type_to_text( GET_TYPE( expType.type)));
          parser->abortError = TRUE;
        }
      else if (IS_FIELD( retType.type))
        {
          if ((GET_FIELD_TYPE( retType.type) != T_UNDETERMINED)
              && (GET_FIELD_TYPE( retType.type) !=
                   GET_FIELD_TYPE( expType.type)))
            {
              if ( ! (IS_ARRAY(  GET_FIELD_TYPE( retType.type))
        	     && (GET_BASIC_TYPE( retType.type) == T_UNDETERMINED)
        	     && (IS_ARRAY( GET_FIELD_TYPE( expType.type)))))
        	{
                      log_message( parser,
                                   parser->bufferPos,
                                   MSG_PROC_RET_NA_EXT,
                                   type_to_text( GET_TYPE( retType.type)),
                                   type_to_text( GET_TYPE( expType.type)));
                      parser->abortError = TRUE;
        	}
            }
        }
      else if ( IS_TABLE( retType.type) == FALSE)
        {
          if (IS_ARRAY( retType.type) == FALSE)
            {
              const uint_t baseExpType = GET_BASIC_TYPE( expType.type);
              const uint_t baseRetType = GET_BASIC_TYPE( retType.type);

              const enum W_OPCODE temp_op = store_op[baseRetType][baseExpType];

              assert( IS_ARRAY( expType.type) == FALSE);
              assert( baseExpType <= T_UNDETERMINED);
              assert( baseRetType <= T_UNDETERMINED);


              if (temp_op == W_NA)
                {
                  log_message( parser,
                               parser->bufferPos,
                               MSG_PROC_RET_NA_EXT,
                               type_to_text( retType.type),
                               type_to_text( expType.type));
                  parser->abortError = TRUE;
                }
            }
          else
            {
              assert( IS_ARRAY( expType.type));
              if ((GET_BASIC_TYPE( retType.type) != T_UNDETERMINED)
                  && (GET_BASIC_TYPE( retType.type) !=
                        GET_BASIC_TYPE( expType.type)))
                {
                  log_message( parser,
                               parser->bufferPos,
                               MSG_PROC_RET_NA_EXT,
                               type_to_text( retType.type),
                               type_to_text( expType.type));
                  parser->abortError = TRUE;
                }
            }
        }
      else if ( ! are_compatible_tables( parser, &retType, &expType))
        {
          /* The two containers types are not compatible.
           * The error was already logged. */
          log_message( parser, parser->bufferPos, MSG_PROC_RET_NA);
          parser->abortError = TRUE;
        }
    }

  if (encode_opcode( instrs, W_RET) == NULL)
    log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

  return NULL;
}


bool_t
translate_bool_exp( struct ParserState* const parser, YYSTYPE exp)
{
  struct ExpResultType expType;

  assert( exp->val_type == VAL_EXP_LINK);

  expType = translate_tree_exp( parser,
                                parser->pCurrentStmt,
                                 &(exp->val.u_exp));
  if (expType.type == T_UNKNOWN)
    {
      /* Some error was encounter evaluating expression.
       * The error should be already logged */
      assert( parser->abortError == TRUE);
      return FALSE;
    }
  else if (GET_TYPE( expType.type) != T_BOOL)
    {
      log_message( parser, parser->bufferPos, MSG_EXP_NOT_BOOL);
      return FALSE;
    }

  free_sem_value( exp);

  return TRUE;
}


YYSTYPE
create_arg_link( struct ParserState* const parser,
                 YYSTYPE                   argument,
                 YYSTYPE                   next)
{
  struct SemValue* const result = alloc_sem_value( parser);

  assert( (argument != NULL) && (argument->val_type == VAL_EXP_LINK));
  assert( (next == NULL) || (next->val_type = VAL_PRC_ARG_LINK));

  if (result == NULL)
    {
      log_message( parser, IGNORE_BUFFER_POS, MSG_NO_MEM);

      return NULL;
    }

  result->val_type        = VAL_PRC_ARG_LINK;
  result->val.u_args.expr = argument;
  result->val.u_args.next = next;

  return result;
}

