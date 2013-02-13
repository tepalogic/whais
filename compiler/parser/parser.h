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
#include "whisperc/whisperc.h"
#include "utils/include/array.h"
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
  const D_CHAR* text;         /* points at the name of identifier */
  D_UINT        length;       /* the name's length of the identifier */
};

/* represents a number */
struct SemCInt
{
  D_UINTMAX value;
  D_BOOL    is_signed;
};

/*represents a real number */
struct SemCReal
{
  D_INT64 integerPart;
  D_INT64 fractionalPart;
};

/* represents a string, a text delimited by \" */
struct SemCText
{
  D_CHAR* text;
  D_UINT  length;  /* the length of the text including the null terminator */
};

/* represent a character, delimited by \' */
struct SemCChar
{
  D_CHAR value;
};

/* represents a date and time value */
struct SemCTime
{
  D_UINT32 usec;        /* microseconds */
  D_INT16  year;
  D_UINT8  month;
  D_UINT8  day;
  D_UINT8  hour;
  D_UINT8  min;
  D_UINT8  sec;
};

struct SemCBool
{
  D_BOOL value;
};

struct SemIdList
{
  struct SemValue* next;
  struct SemId     id;
};

struct SemTypeSpec
{
  void*    extra;       /* extra info for container types */
  D_UINT16 type;        /* contains the type specification */
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
  D_UINT16         opcode;
};

struct SemProcArgumentsList
{
  struct SemValue* expr;        /* holds the expression tree */
  struct SemValue* next;        /* next argument in list */
};

struct SemValue
{
  /*    D_UINT   buffer_pos; */
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

  const D_CHAR*     buffer;
  D_UINT            bufferSize;
  D_UINT            bufferPos;           /* Use this offset to get the next token */
  StringStoreHnd    strings;             /* String container to hold constant strings */
  struct UArray     parsedValues;        /* Array to store the semantics values parsed */
  struct Statement  globalStmt;          /* the global statement */
  struct Statement* pCurrentStmt;
  D_BOOL            abortError;          /* set to true to abort parsing. */
  D_BOOL            externDeclaration;   /* set to true if the declaration is external */

};

struct SemValue*
alloc_sem_value (struct ParserState* const pState);

struct SemValue*
alloc_boolean_sem_value (struct ParserState* const pState, const D_BOOL intialValue);

INLINE static void
free_sem_value (struct SemValue* const pValue)
{
  pValue->val_type = VAL_REUSE;
}

#endif /* PARSER_H */
