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

#ifndef WLOG_H_
#define WLOG_H_

#include "whais.h"
#include "utils/msglog.h"
#include "../parser/parser.h"


#define MSG_GENERAL_EVENT     0
#define MSG_ERROR_EVENT       1
#define MSG_WARNING_EVENT     2
#define MSG_INTERNAL_ERROR    3
#define MSG_EXTRA_EVENT       4


struct MsgCodeEntry
{
  uint_t   id;
  uint_t   type;
  char    *msg;
};


/* general eventes related errors, base 100 */
#define GEN_MSGS      100
#define MSG_NO_MEM    GEN_MSGS
#define MSG_INT_ERR   (MSG_NO_MEM + 1)

/* declaration related errors, base 250 */
#define DCLS_MSGS                250
#define MSG_VAR_DEFINED          DCLS_MSGS
#define MSG_VAR_DECL_NA          (MSG_VAR_DEFINED + 1)
#define MSG_VAR_EXT_LATE         (MSG_VAR_DECL_NA + 1)
#define MSG_VAR_NFOUND           (MSG_VAR_EXT_LATE + 1)
#define MSG_VAR_LATE             (MSG_VAR_NFOUND + 1)
#define MSG_VAR_GLB_RETURN       (MSG_VAR_LATE + 1)
#define MSG_NOT_TABLE            (MSG_VAR_GLB_RETURN + 1)
#define MSG_SAME_FIELD           (MSG_NOT_TABLE + 1)
#define MSG_FIELD_TYPE_INVALID   (MSG_SAME_FIELD + 1)
#define MSG_PROC_ADECL           (MSG_FIELD_TYPE_INVALID + 1)
#define MSG_PROC_EXT_LATE        (MSG_PROC_ADECL + 1)
#define MSG_PROC_DECL_RET_NA     (MSG_PROC_EXT_LATE + 1)
#define MSG_PROC_DECL_PARAM_NA   (MSG_PROC_DECL_RET_NA + 1)
#define MSG_PROC_DECL_LESS       (MSG_PROC_DECL_PARAM_NA + 1)
#define MSG_PROC_DECL_MORE       (MSG_PROC_DECL_LESS + 1)
#define MSG_DECL_PREV            (MSG_PROC_DECL_MORE + 1)
#define MSG_COMPILER_ERR         (MSG_DECL_PREV + 1)

/* expression semantic related errors, base 500 */
#define EXPS_MSGS              500
#define MSG_NOT_NA             (EXPS_MSGS + 1)
#define MSG_ADD_NA             (MSG_NOT_NA + 1)
#define MSG_COMPARE_SIGN       (MSG_ADD_NA + 1)
#define MSG_SUB_NA             (MSG_COMPARE_SIGN + 1)
#define MSG_MUL_NA             (MSG_SUB_NA + 1)
#define MSG_DIV_NA             (MSG_MUL_NA + 1)
#define MSG_MOD_NA             (MSG_DIV_NA + 1)
#define MSG_LT_NA              (MSG_MOD_NA + 1)
#define MSG_LE_NA              (MSG_LT_NA + 1)
#define MSG_GT_NA              (MSG_LE_NA + 1)
#define MSG_GE_NA              (MSG_GT_NA + 1)
#define MSG_EQ_NA              (MSG_GE_NA + 1)
#define MSG_NE_NA              (MSG_EQ_NA + 1)
#define MSG_OR_NA              (MSG_NE_NA + 1)
#define MSG_AND_NA             (MSG_OR_NA + 1)
#define MSG_XOR_NA             (MSG_AND_NA + 1)
#define MSG_INDEX_EAT          (MSG_XOR_NA + 1)
#define MSG_INDEX_ENI          (MSG_INDEX_EAT + 1)
#define MSG_INDEX_UNA          (MSG_INDEX_ENI + 1)
#define MSG_INDEX_UNF          (MSG_INDEX_UNA + 1)
#define MSG_MEMSEL_ERD         (MSG_INDEX_UNF + 1)
#define MSG_MEMSEL_NA          (MSG_MEMSEL_ERD + 1)
#define MSG_STORE_NA           (MSG_MEMSEL_NA + 1)
#define MSG_STORE_ELV          (MSG_STORE_NA + 1)
#define MSG_SADD_NA            (MSG_STORE_ELV + 1)
#define MSG_SADD_ELV           (MSG_SADD_NA + 1)
#define MSG_SSUB_NA            (MSG_SADD_ELV + 1)
#define MSG_SSUB_ELV           (MSG_SSUB_NA + 1)
#define MSG_SMUL_NA            (MSG_SSUB_ELV + 1)
#define MSG_SMUL_ELV           (MSG_SMUL_NA + 1)
#define MSG_SDIV_NA            (MSG_SMUL_ELV + 1)
#define MSG_SDIV_ELV           (MSG_SDIV_NA + 1)
#define MSG_SMOD_NA            (MSG_SDIV_ELV + 1)
#define MSG_SMOD_ELV           (MSG_SMOD_NA + 1)
#define MSG_SAND_NA            (MSG_SMOD_ELV + 1)
#define MSG_SAND_ELV           (MSG_SAND_NA + 1)
#define MSG_SXOR_NA            (MSG_SAND_ELV + 1)
#define MSG_SXOR_ELV           (MSG_SXOR_NA + 1)
#define MSG_SOR_NA             (MSG_SXOR_ELV + 1)
#define MSG_SOR_ELV            (MSG_SOR_NA + 1)
#define MSG_NO_FIELD           (MSG_SOR_ELV + 1)
#define MSG_FIELD_NA           (MSG_NO_FIELD + 1)
#define MSG_NO_PROC            (MSG_FIELD_NA + 1)
#define MSG_PROC_MORE_ARGS     (MSG_NO_PROC + 1)
#define MSG_PROC_LESS_ARGS     (MSG_PROC_MORE_ARGS + 1)
#define MSG_CONTAINER_NA       (MSG_PROC_LESS_ARGS + 1)
#define MSG_PROC_ARG_COUNT     (MSG_CONTAINER_NA + 1)
#define MSG_PROC_ARG_NA        (MSG_PROC_ARG_COUNT + 1)
#define MSG_PROC_RET_NA_EXT    (MSG_PROC_ARG_NA + 1)
#define MSG_PROC_RET_NA        (MSG_PROC_RET_NA_EXT + 1)
#define MSG_EXP_NOT_BOOL       (MSG_PROC_RET_NA + 1)
#define MSG_EXP_NOT_ITERABLE   (MSG_EXP_NOT_BOOL + 1)
#define MSG_BREAK_NOLOOP       (MSG_EXP_NOT_ITERABLE + 1)
#define MSG_CONTINUE_NOLOOP    (MSG_BREAK_NOLOOP + 1)
#define MSG_SYNC_NA            (MSG_CONTINUE_NOLOOP + 1)
#define MSG_SYNC_MANY          (MSG_SYNC_NA + 1)
#define MSG_PROC_NO_RET        (MSG_SYNC_MANY + 1)
#define MSG_DEAD_STMT          (MSG_PROC_NO_RET + 1)
#define MSG_IT_VARIABLE        (MSG_DEAD_STMT + 1)
#define MSG_IT_ALREADY         (MSG_IT_VARIABLE + 1)
#define MSG_IT_ID_TYPE_NA      (MSG_IT_ALREADY + 1)
#define MSG_IT_EXP_TYPE_NA     (MSG_IT_ID_TYPE_NA + 1)
#define MSG_SEL_NO_BOOL        (MSG_IT_EXP_TYPE_NA + 1)
#define MSG_SEL_EXP_NOT_EQ     (MSG_SEL_NO_BOOL + 1)
#define MSG_ROW_COPY_NOTABLE_L (MSG_SEL_EXP_NOT_EQ + 1)
#define MSG_ROW_COPY_NOTABLE_R (MSG_ROW_COPY_NOTABLE_L + 1)
#define MSG_ROW_COPY_NOINDEX_L (MSG_ROW_COPY_NOTABLE_R + 1)
#define MSG_ROW_COPY_NOINDEX_R (MSG_ROW_COPY_NOINDEX_L + 1)
#define MSG_ROW_COPY_NOFIELD_L (MSG_ROW_COPY_NOINDEX_R + 1)
#define MSG_ROW_COPY_NOFIELD_R (MSG_ROW_COPY_NOFIELD_L + 1)
#define MSG_ROW_COPY_TYPEFIELD_NA (MSG_ROW_COPY_NOFIELD_R + 1)
#define MSG_ROW_COPY_NOFIELD_SEL (MSG_ROW_COPY_TYPEFIELD_NA + 1)
#define MSG_ROW_COPY_LTABLE_TYPE (MSG_ROW_COPY_NOFIELD_SEL + 1)
#define MSG_ROW_COPY_RTABLE_TYPE (MSG_ROW_COPY_LTABLE_TYPE + 1)
#define MSG_ROW_COPY_TABLES_TYPE (MSG_ROW_COPY_RTABLE_TYPE + 1)
#define MSG_ROW_COPY_NOFIELDS_CNT (MSG_ROW_COPY_TABLES_TYPE + 1)
#define MSG_ARR_CONSTRUCT_EXP_TYPE_NA (MSG_ROW_COPY_NOFIELDS_CNT + 1)
#define MSG_ARR_CONSTRUCT_DEF_TEXT (MSG_ARR_CONSTRUCT_EXP_TYPE_NA + 1)
#define MSG_ARR_CONSTRUCT_EXP_TEXT (MSG_ARR_CONSTRUCT_DEF_TEXT + 1)
#define MSG_ARR_CONSTRUCT_EXP_FAIL (MSG_ARR_CONSTRUCT_EXP_TEXT + 1)
#define MSG_ARR_CONSTRUCT_EXP_SHOW (MSG_ARR_CONSTRUCT_EXP_FAIL + 1)
#define MSG_ARR_CONSTRUCT_TYPE_SHOW (MSG_ARR_CONSTRUCT_EXP_SHOW + 1)


void
log_message (struct ParserState* const parser,
             const uint_t buffPos,
             const uint_t msgCode, ...);

#endif /*WLOG_H_ */

