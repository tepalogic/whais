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

#ifndef WLOG_H_
#define WLOG_H_

#include "whisper.h"
#include "../../utils/include/msglog.h"
#include "../parser/parser.h"

#define MSG_GENERAL_EVENT  0
#define MSG_ERROR_EVENT    1
#define MSG_WARNING_EVENT  2
#define MSG_INTERNAL_ERROR 3
#define MSG_EXTRA_EVENT	   4

struct MsgCodeEntry
{
  D_UINT id;
  D_UINT type;
  D_CHAR *msg;
};

/* general eventes related errors, base 100 */
#define GEN_MSGS    100
#define MSG_NO_MEM  GEN_MSGS
#define MSG_INT_ERR (MSG_NO_MEM + 1)

/* declaration related errors, base 250 */
#define DCLS_MSGS    250
#define MSG_VAR_DEFINED  DCLS_MSGS
#define MSG_VAR_NFOUND   (MSG_VAR_DEFINED + 1)
#define MSG_NOT_TABLE    (MSG_VAR_NFOUND + 1)
#define MSG_SAME_FIELD   (MSG_NOT_TABLE + 1)
#define MSG_PROC_ADECL   (MSG_SAME_FIELD + 1)
#define MSG_COMPILER_ERR (MSG_PROC_ADECL + 1)

/* expression semantic related errors, base 500 */
#define EXPS_MSGS    500
#define MSG_INC_NA       EXPS_MSGS
#define MSG_INC_ELV      (MSG_INC_NA + 1)
#define MSG_DEC_NA       (MSG_INC_ELV + 1)
#define MSG_DEC_ELV      (MSG_DEC_NA + 1)
#define MSG_NOT_NA       (MSG_DEC_ELV + 1)
#define MSG_ADD_NA       (MSG_NOT_NA + 1)
#define MSG_ADD_SIGN     (MSG_ADD_NA + 1)
#define MSG_SUB_NA       (MSG_ADD_SIGN + 1)
#define MSG_SUB_SIGN     (MSG_SUB_NA + 1)
#define MSG_MUL_NA       (MSG_SUB_SIGN + 1)
#define MSG_MUL_SIGN     (MSG_MUL_NA + 1)
#define MSG_DIV_NA       (MSG_MUL_SIGN + 1)
#define MSG_DIV_SIGN     (MSG_DIV_NA + 1)
#define MSG_MOD_NA       (MSG_DIV_SIGN + 1)
#define MSG_LT_NA        (MSG_MOD_NA + 1)
#define MSG_LE_NA        (MSG_LT_NA + 1)
#define MSG_GT_NA        (MSG_LE_NA + 1)
#define MSG_GE_NA        (MSG_GT_NA + 1)
#define MSG_EQ_NA        (MSG_GE_NA + 1)
#define MSG_NE_NA        (MSG_EQ_NA + 1)
#define MSG_OR_NA        (MSG_NE_NA + 1)
#define MSG_AND_NA       (MSG_OR_NA + 1)
#define MSG_XOR_NA       (MSG_AND_NA + 1)
#define MSG_INDEX_EAT	 (MSG_XOR_NA + 1)
#define MSG_INDEX_ENI	 (MSG_INDEX_EAT + 1)
#define MSG_INDEX_UNA	 (MSG_INDEX_ENI + 1)
#define MSG_MEMSEL_ERD   (MSG_INDEX_UNA + 1)
#define MSG_MEMSEL_ERED  (MSG_MEMSEL_ERD + 1)
#define MSG_MEMSEL_NA	 (MSG_MEMSEL_ERED + 1)
#define MSG_STORE_NA	 (MSG_MEMSEL_NA + 1)
#define MSG_STORE_ELV	 (MSG_STORE_NA + 1)
#define MSG_NO_FIELD	 (MSG_STORE_ELV + 1)
#define MSG_FIELD_NA	 (MSG_NO_FIELD + 1)
#define MSG_NO_PROC		 (MSG_FIELD_NA + 1)
#define MSG_PROC_MORE_ARGS	 (MSG_NO_PROC + 1)
#define MSG_PROC_LESS_ARGS	 (MSG_PROC_MORE_ARGS + 1)
#define MSG_CONTAINER_NA	 (MSG_PROC_LESS_ARGS + 1)
#define MSG_PROC_ARG_COUNT	 (MSG_CONTAINER_NA + 1)
#define MSG_PROC_ARG_NA		 (MSG_PROC_ARG_COUNT + 1)
#define MSG_PROC_RET_NA_EXT	 (MSG_PROC_ARG_NA + 1)
#define MSG_PROC_RET_NA  	 (MSG_PROC_RET_NA_EXT + 1)
#define MSG_EXP_NOT_BOOL  	 (MSG_PROC_RET_NA + 1)
#define MSG_BREAK_NOLOOP  	 (MSG_EXP_NOT_BOOL + 1)
#define MSG_CONTINUE_NOLOOP	 (MSG_BREAK_NOLOOP + 1)
#define MSG_SYNC_NA     	 (MSG_CONTINUE_NOLOOP + 1)
#define MSG_SYNC_MANY     	 (MSG_SYNC_NA + 1)

void
w_log_msg (struct ParserState *state, D_UINT buff_pos, D_UINT msgCode, ...);

#endif /*WLOG_H_ */
