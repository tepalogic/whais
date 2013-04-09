/******************************************************************************
 WHISPERC - A compiler for whisper programs
 Copyright (C) 2008  Iulian Popa

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
 * yy.c - Implements a lexical analyzer for our bison generated parser.
 *        (e.g. provides the required yylex() and yyerror() functions).
 */

/* careful whit this to be the same as in
 * whisper.y */
#ifndef YYSTYPE
#define YYSTYPE struct SemValue*
#endif

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "whisper.h"
#include "whisper.tab.h"
#include "compiler/wopcodes.h"

#include "parser.h"
#include "../semantics/wlog.h"


INLINE static bool_t
is_space (char c)
{
  return (c == ' ' || c == '\t' || c == 0x0A || c == 0x0D);
}

INLINE static bool_t
is_numeric (char c, bool_t is_hexa)
{
  return (c >= '0' && c <= '9') ||
         (is_hexa && ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')));
}

INLINE static bool_t
is_alpha (char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

INLINE static bool_t
is_idlegal (char c)
{
  return is_numeric (c, FALSE) || is_alpha (c) || (c == '_');
}

INLINE static bool_t
is_eol (char c)
{
  return (c == 0x0A || c == 0x0D);
}

typedef enum
{
  TK_CHAR,             /* represents a value of type char (e.g 'a') */
  TK_STRING,           /* represents a string */
  TK_DATETIME,         /* represents a date and time */
  TK_IDENTIFIER,       /* could be a keyword or an identifier */
  TK_NUMERIC,          /* contains only digits */
  TK_OPERATOR,         /* could be an operator */
  TK_REAL,             /* a real number */
  TK_KEYWORD,          /* the token is a key word */
  TK_UNDETERMINED,     /* token type is not yet determined */
  TK_ERROR             /* an obvious syntax error was encountered */
} TOKEN_TYPE;

/*
 * get_next_token()
 *
 * Find the next word in buffer, ignoring any white spaces
 * Warning!!! This function modifies the input buffer
 */
static TOKEN_TYPE
get_next_token (const char*  pBuffer,
                char const** pOutPToken,
                uint_t*      pOutTokenLen)
{
  TOKEN_TYPE result = TK_UNDETERMINED;

  assert (pOutPToken != NULL);
  assert (pOutTokenLen != NULL);

  while (is_space (*pBuffer))
    ++pBuffer;

  if (*pBuffer == '#')
    {
      /* skip line with commentaries and try again */
      while (!is_eol (*pBuffer))
        ++pBuffer;
      return get_next_token (pBuffer, pOutPToken, pOutTokenLen);
    }

  *pOutPToken   = pBuffer;
  *pOutTokenLen = 0;

  if ((*pBuffer == '-') && is_numeric (pBuffer[1], FALSE))
    pBuffer++;

  if (is_numeric (*pBuffer, FALSE))
    {
      bool_t isHexa = FALSE;

      if ((pBuffer[0] == '0') && (pBuffer[1] == 'x' || pBuffer[1] == 'X'))
        {
          isHexa  = TRUE;
          pBuffer += 2;
        }

      while (is_numeric (*pBuffer, isHexa))
        pBuffer++;

      result = TK_NUMERIC;
      if ((*pBuffer == '.') && (isHexa == FALSE))
        {
          ++pBuffer;

          while (is_numeric (*pBuffer, FALSE))
            ++pBuffer;

          result = TK_REAL;
        }
    }
  else if (is_idlegal (*pBuffer))
    {
      while (is_idlegal (*pBuffer))
        pBuffer++;

      result = TK_IDENTIFIER;
    }
  else if (*pBuffer == '<' || *pBuffer == '>' || *pBuffer == '!' ||

           *pBuffer == '+' || *pBuffer == '-' ||
           *pBuffer == '*' || *pBuffer == '/' || *pBuffer == '%' ||
           *pBuffer == '&' || *pBuffer == '^' || *pBuffer == '|' ||

           *pBuffer == '=')
    {
      pBuffer++;

      if (*pBuffer == '=')
        pBuffer++;

      result = TK_OPERATOR;
    }
  else if (*pBuffer == '\"')
    {
      do
        {
          pBuffer++;

          if (is_eol (*pBuffer) || (*pBuffer == 0))
            {
              /* an new line or end of buffer
               * encountered before ending \" */
              return TK_ERROR;
            }
        }
      while ((*pBuffer != '\"') ||
             ((*(pBuffer - 1) == '\\') && (*(pBuffer - 2) != '\\')));

      pBuffer++;

      result = TK_STRING;
    }
  else if (*pBuffer == '\'')
    {
      do
        {
          pBuffer++;

          if (is_eol (*pBuffer) || (*pBuffer == 0))
            {
              /* an new line or end of buffer
               * encountered before ending \' */
              return TK_ERROR;
            }
        }
      while ((*pBuffer != '\'') ||
             ((*(pBuffer - 1) == '\\') && (*(pBuffer - 2) != '\\')));

      pBuffer++;

      if ((*pOutPToken)[1] == '\\' || (*pOutPToken)[2] == '\'')
        {
          /* this clearly is not a date time entry */
          result = TK_CHAR;
        }
      else
        result = TK_DATETIME;
    }
  else
    pBuffer++;

  *pOutTokenLen = (uint_t) (pBuffer - *pOutPToken);

  return result;
}

typedef struct
{
  const char* text;
  uint_t      token;
} TOKEN_SEMANTIC;

/*
 * This is the longest keyword string length including the
 * ending NULL. Currently this has the value for "UNRESTRICTED".
 * Please update this if you add a new keyword longer than it.
 */
#define MAX_KEYWORD_LEN 13

static TOKEN_SEMANTIC keywords[] = {
                                      {"AND", AND},
                                      {"ARRAY", ARRAY},
                                      {"AS", AS},
                                      {"BOOL", BOOL},
                                      {"BREAK", BREAK},
                                      {"CHARACTER", CHARACTER},
                                      {"CONTINUE", CONTINUE},
                                      {"DATE", DATE},
                                      {"DATETIME", DATETIME},
                                      {"DO", DO},
                                      {"ELSE", ELSE},
                                      {"ELSEIF", ELSEIF},
                                      {"END", END},
                                      {"ENDPROC", ENDPROC},
                                      {"ENDSYNC", ENDSYNC},
                                      {"EXTERN", EXTERN},
                                      {"FALSE", W_FALSE},
                                      {"FIELD", FIELD},
                                      {"FOREACH", FOREACH},
                                      {"HIRESTIME", HIRESTIME},
                                      {"IF", IF},
                                      {"IN", IN},
                                      {"INT8", INT8},
                                      {"INT16", INT16},
                                      {"INT32", INT32},
                                      {"INT64", INT64},
                                      {"LET", LET},
                                      {"OF", OF},
                                      {"OR", OR},
                                      {"NOT", NOT},
                                      {"NULL", WHISPER_NULL},
                                      {"REAL", REAL},
                                      {"RETURN", RETURN},
                                      {"RICHREAL", RICHREAL},
                                      {"PROCEDURE", PROCEDURE},
                                      {"SYNC", SYNC},
                                      {"TABLE", TABLE},
                                      {"TEXT", TEXT},
                                      {"THEN", THEN},
                                      {"TRUE", W_TRUE},
                                      {"UNTIL", UNTIL},
                                      {"UNSIGNED", UNSIGNED},
                                      {"WHILE", WHILE},
                                      {"XOR", XOR},

                                      /* some aliases of our own */
                                      {"NUMBER", INT32},
                                      {NULL, 0}
                                    };

static int
parse_keyword (const char* keyWord, uint_t keyLen)
{
  uint_t count;
  char   key_upcase[MAX_KEYWORD_LEN];

  if (keyLen >= MAX_KEYWORD_LEN)
    return 0;

  for (count = 0; count < keyLen; count++)
    {
      key_upcase[count] = keyWord[count];

      if (keyWord[count] >= 'a' && keyWord[count] <= 'z')
        key_upcase[count] += ('A' - 'a');
    }
  key_upcase[count] = 0; /* add NULL at the end */

  count = 0;
  while (keywords[count].text != NULL)
    {
      if (strcmp (keywords[count].text, key_upcase) == 0)
        break;

      ++count;
    }

  return keywords[count].token;
}

#define COMPOSED_OPERATOR_LEN    2    /* A compose operator has exactly 2 chars */

static TOKEN_SEMANTIC composed_operators[] = {
                                               {"==", EQ},
                                               {"!=", NE},
                                               {"<=", LE},
                                               {">=", GE},
                                               {"+=", SADD},
                                               {"-=", SSUB},
                                               {"*=", SMUL},
                                               {"/=", SDIV},
                                               {"%=", SMOD},
                                               {"&=", SAND},
                                               {"^=", SXOR},
                                               {"|=", SOR},
                                               {NULL, 0}
                                             };

static int
parse_composed_operator (const char* pOpText)
{
  uint_t count = 0;

  while (composed_operators[count].text != NULL)
    {
      if (strncmp (composed_operators[count].text, pOpText, COMPOSED_OPERATOR_LEN) == 0)
        break;

      ++count;
    }

  return composed_operators[count].token;
}

static uint_t
parse_integer (const char* pBuffer,
               uint_t      bufferLen,
               uint64_t*   pOutVal,
               bool_t*     pOutSigned)
{
  const uint_t oldLen   = bufferLen;
  uint_t       base     = 10;
  bool_t       negative = FALSE;

  assert (pBuffer != NULL);
  assert (bufferLen != 0);
  assert (pOutVal != NULL);

  *pOutVal    = 0;
  *pOutSigned = FALSE;
  if (pBuffer[0] == '-' && bufferLen > 1 && is_numeric (pBuffer[1], FALSE))
    {
      negative = TRUE;
      pBuffer++;
      bufferLen--;
    }

  if (pBuffer[0] == '0' && bufferLen > 1 && (pBuffer[1] == 'x' || pBuffer[1] == 'X'))
    {
      bufferLen -= 2;
      pBuffer   += 2;
      base      = 16;        /* hexa decimal notation */
    }
  else if (pBuffer[0] == '0')
    {
      bufferLen -= 1;
      pBuffer   += 1;
    }

  if (negative)
    *pOutSigned = TRUE;

  while (bufferLen > 0)
    {
      uint8_t digit;

      if (*pBuffer >= '0' && *pBuffer <= '9')
        digit = (*pBuffer - '0');
      else if (base == 16)
        {
          if (*pBuffer >= 'a' && *pBuffer <= 'f')
            digit = ((*pBuffer - 'a') + 10);
          else if (*pBuffer >= 'A' && *pBuffer <= 'F')
            digit = ((*pBuffer - 'A') + 10);
          else
            break;                /* no more hexa digits for you */
        }
      else
        break;                /* no more digits for you */

      *pOutVal *= base;
      *pOutVal += digit;

      pBuffer++;
      bufferLen--;
    }

  if (negative)
    *pOutVal *= -1;

  return (oldLen - bufferLen);
}

static uint_t
parse_real_value (const char*      pBuffer,
                  uint_t           bufferLen,
                  struct SemCReal* pOutReal)
{
  const uint_t oldLen            = bufferLen;
  bool_t       foundDecimalPoint = FALSE;
  bool_t       negative          = FALSE;
  uint64_t     precision         = 1;

  assert (pBuffer != NULL);
  assert (bufferLen != 0);
  assert (pOutReal != NULL);

  pOutReal->integerPart    = 0;
  pOutReal->fractionalPart = 0;

  if ((pBuffer[0] == '-') &&
      (bufferLen > 1) &&
      is_numeric (pBuffer[1], FALSE))
    {
      negative = TRUE;
      pBuffer++;
      bufferLen--;
    }

  while (bufferLen > 0)
    {
      int digit = 0;

      if (*pBuffer >= '0' && *pBuffer <= '9')
        {
          digit += (*pBuffer - '0');
          if (! foundDecimalPoint)
            {
              pOutReal->integerPart *= 10;
              pOutReal->integerPart += digit;
            }
          else
            {
              precision               *= 10;
              pOutReal->fractionalPart *= 10;
              pOutReal->fractionalPart += digit;
            }
        }
      else if (*pBuffer == '.')
        {
          if (foundDecimalPoint == TRUE)
            {
              /* we already found a decimal point */
              return 0;
            }
          foundDecimalPoint = TRUE;
        }
      else
        break;  /* nothing for us here */

      pBuffer++;
      bufferLen--;
    }

  for (; precision < W_LDRR_PRECISSION; precision *= 10)
    pOutReal->fractionalPart *= 10;

  if (negative)
    {
      pOutReal->integerPart    = -pOutReal->integerPart;
      pOutReal->fractionalPart = -pOutReal->fractionalPart;
    }

  return (oldLen - bufferLen);
}

static uint_t
parse_character (const char* pBuffer,
                uint_t       bufferLen,
                char*      pOutChar)
{
  uint_t result = 0;

  assert (pBuffer != NULL);
  assert (bufferLen != 0);
  assert (pOutChar != NULL);

  *pOutChar = 0;

  if (*pBuffer == '\\')
    {
      if (bufferLen > 0)
        {
          ++pBuffer;
          ++result;
          --bufferLen;
        }
      else
        return 0;       /* error */

      if (*pBuffer == '\\')
        {
          *pOutChar = '\\';
          ++result;
        }
      else if (*pBuffer == 'n')
        {
          *pOutChar = '\n';
          ++result;
        }
      else if (*pBuffer == 'r')
        {
          *pOutChar = '\r';
          ++result;
        }
      else if (*pBuffer == 'f')
        {
          *pOutChar = '\f';
          ++result;
        }
      else if (*pBuffer == 't')
        {
          *pOutChar = '\t';
          ++result;
        }
      else if (*pBuffer == 'v')
        {
          *pOutChar = '\v';
          ++result;
        }
      else if (*pBuffer == 'b')
        {
          *pOutChar = '\b';
          ++result;
        }
      else if (*pBuffer == 'a')
        {
          *pOutChar = '\a';
          ++result;
        }
      else if (*pBuffer == '\'')
        {
          *pOutChar = '\'';
          ++result;
        }
      else if (*pBuffer == '\"')
        {
          *pOutChar = '\"';
          ++result;
        }
      else if (is_numeric (*pBuffer, FALSE))
        {
          uint64_t  intValue = 0;
          bool_t    dummy;

          result    += parse_integer (pBuffer, bufferLen, &intValue, &dummy);
          *pOutChar  = (char) intValue;
        }
    }
  else
    {
      *pOutChar = *pBuffer;
      result    = 1;
    }

  return result;
}

static uint_t
parse_string (const char* pBuffer,
              uint_t      bufferLen,
              char*     pOutString,
              uint_t*     oOutStringLen)
{
  const uint_t oldLen = bufferLen;

  assert (pBuffer != NULL);
  assert (bufferLen != 0);
  assert (pOutString != NULL);

  *oOutStringLen = 0;

  while ((bufferLen > 0) && (*pBuffer != '\"'))
    {
      uint_t result = parse_character (pBuffer, bufferLen, pOutString);

      if (result != 0)
        {
          pBuffer   += result;
          bufferLen -= result;

          pOutString++;
          (*oOutStringLen)++;
        }
      else
        return 0;
    }

  *pOutString = 0;      /* add the null character */
  (*oOutStringLen)++;

  return (oldLen - bufferLen);
}

static uint_t
parse_time_value (const char*      pBuffer,
                  uint_t           bufferLen,
                  struct SemCTime* pOutTime)
{
  int64_t  intVal;
  uint_t   intValLen = 0;
  uint_t   result    = 0;
  bool_t   dummy;

  /* initialise the structure with default valid values */
  memset (pOutTime, 0, sizeof (pOutTime[0]));
  pOutTime->month = pOutTime->day = 1;

  /* found the year part */
  intValLen = parse_integer (pBuffer, bufferLen, (uint64_t*)&intVal, &dummy);
  if (intValLen > 0)
    {
      result    += intValLen;
      pBuffer   += intValLen;
      bufferLen -= intValLen;

      pOutTime->year = (int16_t) intVal;
    }
  else
    return 0;   /* parsing error */

  if (pBuffer[0] == '\'')
    return result;      /* end of date/time entry */
  else if (pBuffer[0] != '/')
    {
      /* no date delimiter */
      return 0; /* parsing error */
    }
  else
    {
      ++result;
      ++pBuffer;
      --bufferLen;
    }

  /* found the month part */
  intValLen = parse_integer (pBuffer, bufferLen, (uint64_t*)&intVal, &dummy);
  if (intValLen > 0)
    {
      result    += intValLen;
      pBuffer   += intValLen;
      bufferLen -= intValLen;

      pOutTime->month = (uint8_t) intVal;
    }
  else
    return 0;   /* parsing error */

  if (pBuffer[0] == '\'')
    return result;      /* end of date/time entry */
  else if (pBuffer[0] != '/')
    {
      /* no date delimiter */
      return 0; /* parsing error */
    }
  else
    {
      ++result,
      ++pBuffer,
      --bufferLen;
    }

  /* found the day part */
  intValLen = parse_integer (pBuffer, bufferLen, (uint64_t *) & intVal, &dummy);
  if (intValLen > 0)
    {
      result    += intValLen;
      pBuffer   += intValLen;
      bufferLen -= intValLen;

      pOutTime->day = (uint8_t) intVal;
    }
  else
    return 0;   /* parsing error */

  if (pBuffer[0] == '\'')
    return result;      /* end of date/time entry */
  else if (pBuffer[0] != ' ')
    {
      /* no date delimiter */
      return 0; /* parsing error */
    }
  else
    {
      ++result;
      ++pBuffer;
      --bufferLen;
    }

  /* found the hour part */
  intValLen = parse_integer (pBuffer, bufferLen, (uint64_t *) & intVal, &dummy);
  if (intValLen > 0)
    {
      result    += intValLen;
      pBuffer   += intValLen;
      bufferLen -= intValLen;

      pOutTime->hour = (uint8_t) intVal;
    }
  else
    return 0;   /* parsing error */

  if (pBuffer[0] != ':')
    {
      /* no time delimiter */
      return 0; /* parsing error */
    }
  else
    {
      ++result;
      ++pBuffer;
      --bufferLen;
    }

  /* found the minute part */
  intValLen = parse_integer (pBuffer, bufferLen, (uint64_t *) & intVal, &dummy);
  if (intValLen > 0)
    {
      result    += intValLen;
      pBuffer   += intValLen;
      bufferLen -= intValLen;

      pOutTime->min = (uint8_t) intVal;
    }
  else
    return 0;   /* parsing error */

  if (pBuffer[0] == '\'')
    return result;
  else if (pBuffer[0] != ':')
    {
      /* no time delimiter */
      return 0; /* parsing error */
    }
  else
    {
      ++result;
      ++pBuffer;
      --bufferLen;
    }

  /* found the second part */
  intValLen = parse_integer (pBuffer, bufferLen, (uint64_t *) & intVal, &dummy);
  if (intValLen > 0)
    {
      result    += intValLen;
      pBuffer   += intValLen;
      bufferLen -= intValLen;

      pOutTime->sec = (uint8_t) intVal;
    }
  else
    return 0;   /* parsing error */

  if (pBuffer[0] == '\'')
    return result;
  else if (pBuffer[0] != '.')
    {
      /* no time delimiter */
      return 0; /* parsing error */
    }
  else
    {
      ++result;
      ++pBuffer;
      --bufferLen;
    }

  /* found the microsecond part */
  intValLen = parse_integer (pBuffer, bufferLen, (uint64_t *) & intVal, &dummy);
  if (intValLen > 0)
    {
      result    += intValLen;
      pBuffer   += intValLen;
      bufferLen -= intValLen;

      pOutTime->usec = (uint32_t) intVal;
    }
  else
    return 0;   /* parsing error */

  return result;
}

int
yylex (YYSTYPE * lvalp, struct ParserState* pState)
{
  int result = 0;
  const char* pBuffer   = pState->buffer;
  const char* pToken    = NULL;
  uint_t        tokenLen  = 0;
  TOKEN_TYPE    tokenType = TK_ERROR;
  uint_t        bufferOff = pState->bufferPos;

  if (pState->bufferPos > pState->bufferSize)
    return 0;

  /* recall where to start from */
  pBuffer   += bufferOff;
  tokenType  = get_next_token (pBuffer, &pToken, &tokenLen);

  /* remember to start from here  next time */
  pState->bufferPos += (uint_t) ((pToken + tokenLen) - pBuffer);

  /* allocate storage for value */
  *lvalp = NULL;                /* if we are to crash... make it loud */
  if ((tokenType != TK_UNDETERMINED) && (tokenType != TK_OPERATOR) &&
      ((tokenType != TK_IDENTIFIER) || ((result = parse_keyword (pToken, tokenLen)) == 0)))
    {
      /* if the token is not a key word */
      *lvalp = alloc_sem_value (pState);
      if (*lvalp == NULL)
        {
          w_log_msg (pState, IGNORE_BUFFER_POS, pState->bufferPos, MSG_NO_MEM);
          return 0;
        }
    }
  else if (tokenType == TK_IDENTIFIER)
    {
      /* after we parsed the token we found is a keyword */
      tokenType = TK_KEYWORD;
      return result;
    }
  else if (tokenLen == 1)
    return *pToken;

  switch (tokenType)
    {
    case TK_IDENTIFIER:
      /* parsing was successful */
      (*lvalp)->val_type        = VAL_ID;
      (*lvalp)->val.u_id.text   = pToken;
      (*lvalp)->val.u_id.length = tokenLen;

      result = IDENTIFIER;
      break;

    case TK_OPERATOR:
      return parse_composed_operator (pToken);

    case TK_NUMERIC:
      result = parse_integer (pToken,
                              tokenLen,
                              &((*lvalp)->val.u_int.value),
                              &((*lvalp)->val.u_int.is_signed));
      if (result != 0)
        {                        /* parsing was successful */
          (*lvalp)->val_type = VAL_C_INT;
          result             = WHISPER_INTEGER;
        }
      break;

    case TK_CHAR:
      if ((tokenLen > 2) || (pToken[0] != '\''))
        {
          pToken++;
          result = parse_character (pToken,
                                    tokenLen - 1,
                                    &((*lvalp)->val.u_char.value));
          if (pToken[result] != '\'')
            result = 0;
        }
      else
        result = 0;                /* error */

      if (result != 0)
        {                        /* parsing was successful */
          (*lvalp)->val_type = VAL_C_CHAR;
          result             = WHISPER_CHARACTER;
        }
      break;

    case TK_STRING:
      if ((tokenLen > 1) || (pToken[0] != '\"'))
        {
          pToken++;
          (*lvalp)->val.u_text.text = alloc_str (pState->strings, tokenLen - 1);
          result = parse_string (pToken,
                                 tokenLen,
                                 (*lvalp)->val.u_text.text,
                                 &(*lvalp)->val.u_text.length);

          if (pToken[result] != '\"')
            result = 0;
          else
            {
              /* parsing was successful, result contains the
               * length of the text */
              (*lvalp)->val_type = VAL_C_TEXT;
              result             = WHISPER_TEXT;
            }
        }
      else
        result = 0;     /* error */

      break;

    case TK_DATETIME:
      if ((tokenLen > 2) || (pToken[0] != '\''))
        {
          pToken++;
          result = parse_time_value (pToken,
                                     tokenLen - 1,
                                     &((*lvalp)->val.u_time));
          if (pToken[result] != '\'')
            result = 0;
        }
      else
        result = 0;     /* error */

      if (result != 0)
        {
          (*lvalp)->val_type = VAL_C_TIME;
          result             = WHISPER_TIME;
        }
      break;

    case TK_REAL:
      result = parse_real_value (pToken, tokenLen, &((*lvalp)->val.u_real));
      if (result != 0)
        {
          /* parsing was successful */
          (*lvalp)->val_type = VAL_C_REAL;
          result             = WHISPER_REAL;
        }
      break;

    default:
      /* What I'm doing here? Return an error!*/
      result = 0;
    }

  return result;
}

/* this is internally used by yyparse()
 * it's declaration is found on wisper.y */
int
yyerror (struct ParserState* pState, const char* msg)
{
  w_log_msg (pState, pState->bufferPos, MSG_COMPILER_ERR);

  return 0;
}
