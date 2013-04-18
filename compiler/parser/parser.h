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
/*
 * parser.h - Declares the types required to manage the semantics objects.
 */

#ifndef PARSER_H
#define PARSER_H

#include "whisper.h"

#include "compiler/whisperc.h"
#include "utils/warray.h"

#include "strstore.h"
#include "../semantics/statement.h"

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

/* represents an identifier */
struct SemId
{
  const char* text;         /* points at the name of identifier */
  uint_t        length;       /* the name's length of the identifier */
};

/* represents a number */
struct SemCInt
{
  uint64_t  value;
  bool_t    is_signed;
};

/*represents a real number */
struct SemCReal
{
  int64_t integerPart;
  int64_t fractionalPart;
};

/* represents a string, a text delimited by \" */
struct SemCText
{
  char* text;
  uint_t  length;  /* the length of the text including the null terminator */
};

/* represent a character, delimited by \' */
struct SemCChar
{
  char value;
};

/* represents a date and time value */
struct SemCTime
{
  uint32_t usec;        /* microseconds */
  int16_t  year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  min;
  uint8_t  sec;
};

struct SemCBool
{
  bool_t value;
};

struct SemIdList
{
  struct SemValue* next;
  struct SemId     id;
};

struct SemTypeSpec
{
  void*    extra;       /* extra info for container types */
  uint16_t type;        /* contains the type specification */
};

struct SemProcParamList
{
  struct SemValue*   next;      /* next in list */
  struct SemId       id;        /* the id of this parameter */
  struct SemTypeSpec type;      /* type of this parameter */
};

struct SemExpression
{
  struct SemValue* pFirstOp;
  struct SemValue* pSecondOp;
  struct SemValue* pThirdOp;
  uint16_t         opcode;
};

struct SemProcArgumentsList
{
  struct SemValue* expr;        /* holds the expression tree */
  struct SemValue* next;        /* next argument in list */
};

struct SemValue
{
  /*    uint_t   buffer_pos; */
  enum SEMVALUE_TYPE val_type;        /* the type of the value */
  union
  {
    struct SemId                u_id;
    struct SemCInt              u_int;
    struct SemCReal             u_real;
    struct SemCText             u_text;
    struct SemCChar             u_char;
    struct SemCTime             u_time;
    struct SemCBool             u_bool;
    struct SemIdList            u_idlist;
    struct SemTypeSpec          u_tspec;
    struct SemProcParamList     u_prdcl;
    struct SemProcArgumentsList u_args;
    struct SemExpression        u_exp;
  } val;
};

struct ParserState
{
  WHC_MESSENGER_ARG messengerContext;
  WHC_MESSENGER     messenger;

  const char*     buffer;
  uint_t            bufferSize;
  uint_t            bufferPos;           /* Use this offset to get the next token */
  StringStoreHnd    strings;             /* String container to hold constant strings */
  struct WArray     parsedValues;        /* Array to store the semantics values parsed */
  struct Statement  globalStmt;          /* the global statement */
  struct Statement* pCurrentStmt;
  bool_t            abortError;          /* set to true to abort parsing. */
  bool_t            externDeclaration;   /* set to true if the declaration is external */

};

struct SemValue*
alloc_sem_value (struct ParserState* const pState);

struct SemValue*
alloc_boolean_sem_value (struct ParserState* const pState, const bool_t intialValue);

INLINE static void
free_sem_value (struct SemValue* const pValue)
{
  pValue->val_type = VAL_REUSE;
}

#endif /* PARSER_H */
