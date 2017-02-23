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
 * parser.h - Declares the types required to manage the semantics objects.
 */

#ifndef PARSER_H
#define PARSER_H

#include "whais.h"
#include "utils/warray.h"
#include "compiler/whaisc.h"
#include "strstore.h"
#include "../semantics/statement.h"

/* Specify the tokens' semantic. */
enum SEMVALUE_TYPE
{
  VAL_ERROR = 0,       /* reserved for error situations */
  VAL_ID,

  VAL_C_CHAR,
  VAL_C_INT,
  VAL_C_REAL,
  VAL_C_TEXT,
  VAL_C_TIME,
  VAL_C_BOOL,

  VAL_ID_LIST,
  VAL_TYPE_SPEC,
  VAL_PRCDCL_LIST,
  VAL_PRC_ARG_LINK,

  VAL_EXP_LINK,

  VAL_NULL,
  VAL_REUSE,
  VAL_UNKNOWN
};

/* Describes semantically an identifier. */
struct SemId
{
  const char  *name;   /* identifier name(not null terminated) */
  uint_t       length; /* name's length */
};

/* Describes semantically an integer constant */
struct SemCInt
{
  uint64_t   value;    /* bit value representation. */
  bool_t     isSigned; /* should we tread as signed value. */
};

/* Describes semantically an real constant. */
struct SemCReal
{
  int64_t   integerPart;
  int64_t   fractionalPart;
};

/* Describes semantically a string constant. */
struct SemCText
{
  const char  *text;   /* The entry(not null terminated). */
  uint_t       length; /* The length of the entry */
};

/* Describes semantically a character constant. */
struct SemCChar
{
  uint32_t value;       /* The Unicode code point value. */
};

/* Describes semantically a time/moment constant. */
struct SemCTime
{
  uint32_t   usec;  /* microseconds part */
  int16_t    year;
  uint8_t    month;
  uint8_t    day;
  uint8_t    hour;
  uint8_t    min;
  uint8_t    sec;
};

/* Describes semantically a boolean constant. */
struct SemCBool
{
  bool_t value;
};

/* Represents a list of ids used during variable declarations. */
struct SemIdList
{
  struct SemValue  *next;
  struct SemId      id;
};

/* Holds the semantics for type specifier. */
struct SemTypeSpec
{
  void      *extra; /* Extra info for container types */
  uint16_t   type;  /* Contains the type specification */
};

/* Semantics for representing the declaration list of a procedure
   parameters */
struct SemProcParamList
{
  struct SemValue     *next; /* next in list */
  struct SemId         id;   /* the id of this parameter */
  struct SemTypeSpec   type; /* type of this parameter */
};

/* Semantic used to evaluate an expression(node).
   Depending on the type of the operator, it holds a pointer operands
   expression tree for evaluation. */
struct SemExpression
{
  struct SemValue  *firstTree;  /* First operand. */
  struct SemValue  *secondTree; /* Second operand, for binary operators. */
  struct SemValue  *thirdTree;  /* Third op, for ternary operators. */
  uint16_t          opcode;     /* Operator code. */
};

/* Holds the list of the parameters t be evaluated, before a procedure call. */
struct SemProcArgumentsList
{
  struct SemValue  *next; /* Next argument in list */
  struct SemValue  *expr; /* Expression tree of this parameter. */
};

/* Wrapper to allow uniform access for different semantic entries. */
struct SemValue
{
  enum SEMVALUE_TYPE val_type;   /* Type of the value */

  union
  {
    struct SemId                  u_id;
    struct SemCInt                u_int;
    struct SemCReal               u_real;
    struct SemCText               u_text;
    struct SemCChar               u_char;
    struct SemCTime               u_time;
    struct SemCBool               u_bool;
    struct SemIdList              u_idlist;
    struct SemTypeSpec            u_tspec;
    struct SemProcParamList       u_prdcl;
    struct SemProcArgumentsList   u_args;
    struct SemExpression          u_exp;
  } val;
};

/* Context type used during source code parsing. */
struct ParserState
{
  WH_MESSENGER        messenger;     /* Register messenger. */
  WH_MESSENGER_CTXT   messengerCtxt; /* Register messenger context. */

  const char*         buffer;        /* Program source code. */
  uint_t              bufferSize;    /* The length of the code. */
  uint_t              bufferPos;     /* Current parser position. */
  StringStoreHnd      strings;       /* Container for constant strings. */
  struct WArray       values;        /* Container to temporary hold semantic entries. */
  struct Statement    globalStmt;    /* The global statement */
  struct Statement   *pCurrentStmt;  /* The current statement. */
  bool_t              abortError;    /* Set if parsing was aborted. */
  bool_t              externDecl;    /* Indicates the parsing of an external declaration. */
};


/* Reuse or allocate space to hold a semantic description.
 * Limits the allocations of many small memory buffers. */
struct SemValue*
alloc_sem_value(struct ParserState* const parser);

/* Allocate and initialize a semantic description for a boolean value. */
struct SemValue*
alloc_bool_sem_value(struct ParserState* const   parser,
                     const bool_t                value);

/* Marks a previously allocated semantic description, as ready for reuse. */
INLINE static void
free_sem_value(struct SemValue* const value)
{
  value->val_type = VAL_REUSE;
}

#endif /* PARSER_H */
