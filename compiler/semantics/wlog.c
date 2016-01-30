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

#include "wlog.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#include "../parser/parser.h"

#if 0
#define MAX_DECIMAL_REPRESENTING_LEN  41        /* 41 > (Log10(2^128) + 2) */
#endif

static const struct MsgCodeEntry messages[] = {
  {MSG_NO_MEM, MSG_GENERAL_EVENT, "OUT OF MEMORY!!!"},
  {MSG_INT_ERR, MSG_INTERNAL_ERROR, "Internal error!"},

  {MSG_VAR_DEFINED, MSG_ERROR_EVENT,
   "Variable \'%s\' is declared second time in this scope."},
  {MSG_VAR_DECL_NA, MSG_ERROR_EVENT,
   "Variable \'%s\' is declared again but with another type."},
  {MSG_VAR_EXT_LATE, MSG_WARNING_EVENT,
   "External variable \'%s\' is after its definition."},
  {MSG_VAR_NFOUND, MSG_ERROR_EVENT,
   "Variable \'%s\' is not declared in this scope nor globally."},
  {MSG_VAR_LATE, MSG_ERROR_EVENT,
   "Variable \'%s\' is not declared before all procedures code statements."},
  {MSG_VAR_GLB_RETURN, MSG_ERROR_EVENT,
    "Global variable \'%s\' could not be declared as RETURN."},
  {MSG_NOT_TABLE, MSG_ERROR_EVENT,
   "Variable \'%s\' is not declared as a table."},
  {MSG_SAME_FIELD, MSG_ERROR_EVENT,
   "Field identifier \'%s\' is already declared for this table."},
  {MSG_FIELD_TYPE_INVALID, MSG_ERROR_EVENT,
   "Field identifier \'%s\' must have an unambiguous type."},
  {MSG_PROC_ADECL, MSG_ERROR_EVENT, "Procedure \'%s\' is already declared."},
  {MSG_PROC_EXT_LATE, MSG_WARNING_EVENT,
   "Procedure \'%s\' external declaration is made after it definition."},
   {MSG_PROC_DECL_RET_NA, MSG_ERROR_EVENT,
    "Procedure '%s' declared again, but with a different return value type."},
  {MSG_PROC_DECL_PARAM_NA, MSG_ERROR_EVENT,
   "Procedure '%s' declared again, but parameter %d is different "
      "(parameters names and types should be identical)."},
  {MSG_PROC_DECL_LESS, MSG_ERROR_EVENT,
   "Procedure '%s' declared again, with fewer parameters."},
  {MSG_PROC_DECL_MORE, MSG_ERROR_EVENT,
   "Procedure '%s' declared again, but with more parameters."},
  {MSG_DECL_PREV, MSG_EXTRA_EVENT, "Here is a reference."},
  {MSG_COMPILER_ERR, MSG_ERROR_EVENT,
   "General syntax error! The content of the code line near error: '%s'."},
  {MSG_NOT_NA, MSG_ERROR_EVENT,
   "NOT operator requires a boolean or an integer expression but not a \'%s\'."},
  {MSG_ADD_NA, MSG_ERROR_EVENT,
   "\'+\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_ADD_SIGN, MSG_WARNING_EVENT,
   "\'+\' operator has one operand signed integer and another unsigned integer."},
  {MSG_SUB_NA, MSG_ERROR_EVENT,
   "\'-\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_SUB_SIGN, MSG_WARNING_EVENT,
   "\'-\' operator has one operand signed integer and another unsigned integer."},
  {MSG_MUL_NA, MSG_ERROR_EVENT,
   "\'*\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_MUL_SIGN, MSG_WARNING_EVENT,
   "\'*\' operator has one operand signed integer and another unsigned integer."},
  {MSG_DIV_NA, MSG_ERROR_EVENT,
   "\'/\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_DIV_SIGN, MSG_WARNING_EVENT,
   "\'/\' operator has one operand signed integer and another unsigned integer."},
  {MSG_MOD_NA, MSG_ERROR_EVENT,
   "\'%%\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_LT_NA, MSG_ERROR_EVENT,
   "\'<\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_LE_NA, MSG_ERROR_EVENT,
   "\'<=\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_GT_NA, MSG_ERROR_EVENT,
   "\'>\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_GE_NA, MSG_ERROR_EVENT,
   "\'>=\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_EQ_NA, MSG_ERROR_EVENT,
   "\'==\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_NE_NA, MSG_ERROR_EVENT,
   "\'!=\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_OR_NA, MSG_ERROR_EVENT,
   "\'OR\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_AND_NA, MSG_ERROR_EVENT,
   "\'AND\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_XOR_NA, MSG_ERROR_EVENT,
   "\'XOR\' operator can not be used with operands of type \'%s\' and \'%s\',"},
  {MSG_INDEX_EAT, MSG_ERROR_EVENT,
   "[] operator applied to a \'%s\'. But an array, a field or a text is required."},
  {MSG_INDEX_ENI, MSG_ERROR_EVENT,
   "[] operator has the indexer as a \'%s\' but an integer is required."},
  {MSG_INDEX_UNA, MSG_ERROR_EVENT,
   "[] operator applied to an array that is not completely defined."},
  {MSG_INDEX_UNF, MSG_ERROR_EVENT,
    "[] operator applied to a field that is not completely defined."},
  {MSG_MEMSEL_ERD, MSG_ERROR_EVENT,
   "Field \'%s\' is selected from a table that does not contains its declaration."},
  {MSG_MEMSEL_NA, MSG_ERROR_EVENT,
   "Could not select a member of a \'%s\'. A table is needed!"},
  {MSG_STORE_NA, MSG_ERROR_EVENT,
   "To a variable of type '%s' can not be assigned a value of type '%s'."},
  {MSG_STORE_ELV, MSG_ERROR_EVENT,
   "The left operand of the assignment operator must be a l-value."},
  {MSG_SADD_NA, MSG_ERROR_EVENT,
   "Cannot use operator '+=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SADD_ELV, MSG_ERROR_EVENT,
   "The left operand of the '+=' operator must be a l-value."},
  {MSG_SSUB_NA, MSG_ERROR_EVENT,
   "Cannot use operator '-=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SSUB_ELV, MSG_ERROR_EVENT,
   "The left operand of the '-=' operator must be a l-value."},
  {MSG_SMUL_NA, MSG_ERROR_EVENT,
   "Cannot use operator '*=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SMUL_ELV, MSG_ERROR_EVENT,
   "The left operand of the '*=' operator must be a l-value."},
  {MSG_SDIV_NA, MSG_ERROR_EVENT,
  "Cannot use operator '/=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SDIV_ELV, MSG_ERROR_EVENT,
  "The left operand of the '/=' operator must be a l-value."},
  {MSG_SMOD_NA, MSG_ERROR_EVENT,
  "Cannot use operator '%=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SMOD_ELV, MSG_ERROR_EVENT,
  "The left operand of the '%=' operator must be a l-value."},
  {MSG_SAND_NA, MSG_ERROR_EVENT,
  "Cannot use operator '&=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SAND_ELV, MSG_ERROR_EVENT,
  "The left operand of the '&=' operator must be a l-value."},
  {MSG_SXOR_NA, MSG_ERROR_EVENT,
  "Cannot use operator '^=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SXOR_ELV, MSG_ERROR_EVENT,
  "The left operand of the '^=' operator must be a l-value."},
  {MSG_SOR_NA, MSG_ERROR_EVENT,
  "Cannot use operator '|=' with a variable of type '%s' and an operand of type '%s'."},
  {MSG_SOR_ELV, MSG_ERROR_EVENT,
  "The left operand of the '|=' operator must be a l-value."},
  {MSG_NO_FIELD, MSG_ERROR_EVENT,
   "Initialization of composite type requires the filed '%s' to be defined."},
  {MSG_FIELD_NA, MSG_ERROR_EVENT,
   "Composite type initialization not allowed for field '%s'. The destination requires a field of type '%s' not a '%s'."},
  {MSG_NO_PROC, MSG_ERROR_EVENT, "The procedure '%s' is not declared."},
  {MSG_PROC_MORE_ARGS, MSG_ERROR_EVENT,
   "The procedure '%s' must not take more than %d parameters."},
  {MSG_PROC_LESS_ARGS, MSG_WARNING_EVENT,
   "The procedure '%s' requires %d parameters. Only %d have been provided."},
  {MSG_CONTAINER_NA, MSG_ERROR_EVENT,
   "A table could not be initialized from a row/record nor vice versa."},
  {MSG_PROC_ARG_COUNT, MSG_EXTRA_EVENT,
   "During evaluation of procedure '%s' argument number %d."},
  {MSG_PROC_ARG_NA, MSG_ERROR_EVENT,
   "During evaluation of '%s' procedure, argument %d, cannot use '%s' for '%s'. "},
  {MSG_PROC_RET_NA_EXT, MSG_ERROR_EVENT,
   "Invalid return expression. Cannot return '%s' from '%s'."},
  {MSG_PROC_RET_NA, MSG_ERROR_EVENT, "Invalid return expression."},
  {MSG_EXP_NOT_BOOL, MSG_ERROR_EVENT,
   "The result of a conditional expression should be of type BOOL."},
  {MSG_EXP_NOT_ITERABLE, MSG_ERROR_EVENT,
   "The expression needs to have an iterable type (TEXT, ARRAY or FILED) but not '%s'."},
  {MSG_BREAK_NOLOOP, MSG_ERROR_EVENT,
   "Break statement used outside of a looping statement."},
  {MSG_CONTINUE_NOLOOP, MSG_ERROR_EVENT,
   "Continue statement used outside of a looping statement."},
  {MSG_SYNC_NA, MSG_ERROR_EVENT,
   "Synchronized statement inside another synchronized statement."},
  {MSG_SYNC_MANY, MSG_ERROR_EVENT,
   "No more than 256 synchronized statements can be used inside a procedure."},
  {MSG_PROC_NO_RET, MSG_ERROR_EVENT,
    "Procedure '%s' must return a value."},
  {MSG_DEAD_STMT, MSG_WARNING_EVENT,
    "This statement will never be executed. Same for the rest within this block."},
  {MSG_IT_VARIABLE, MSG_WARNING_EVENT,
    "A variable with the iterator's name '%s' was previously declared."},
  {MSG_IT_ALREADY, MSG_ERROR_EVENT,
    "An outer loop use the same name '%s' for the iterator identifier."},
  {MSG_IT_NOTFOUND, MSG_ERROR_EVENT,
    "Cannot find an iterator named '%s'."},
  {0, 0, 0}
};

static const struct MsgCodeEntry*
find_string (uint_t msgCode)
{
  uint_t i = 0;

  while ((msgCode != messages[i].id) && (messages[i].id != 0))
      ++i;

  if (messages[i].msg == NULL)
    return NULL;

  return &messages[i];
}

void
log_message (struct ParserState* parser, uint_t buffPos, uint_t msgCode, ...)
{
  const struct MsgCodeEntry* entry = find_string (msgCode);

  va_list args;

  if (entry == NULL)
    {
      log_message (parser, IGNORE_BUFFER_POS, MSG_INT_ERR);
      return;
    }

  va_start (args, msgCode);
  if (parser->messenger != NULL)
    {
      parser->messenger( parser->messengerCtxt,
                        buffPos,
                        msgCode,
                        entry->type,
                        entry->msg,
                        args);
    }
  else
    {
      /* Just don't send the message. */
      assert (parser->messengerCtxt == NULL);
    }

  if ((entry->type == MSG_ERROR_EVENT)
      || (entry->type == MSG_INTERNAL_ERROR)
      || (entry->type == MSG_GENERAL_EVENT))
    {
      parser->abortError = TRUE;
    }

  va_end (args);
}

