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

#include "whisper.tab.h"
#include "whisper.h"

#include "string.h"
#include "stdio.h"
#include "assert.h"

#include "parser.h"
#include "../semantics/wlog.h"


INLINE static D_BOOL
is_space (char c)
{
  return (c == ' ' || c == '\t' || c == 0x0A || c == 0x0D);
}

INLINE static D_BOOL
is_numeric (char c, D_BOOL is_hexa)
{
  return (c >= '0' && c <= '9')
    || (is_hexa && ((c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')));
}

INLINE static D_BOOL
is_alpha (char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

INLINE static D_BOOL
is_idlegal (char c)
{
  return is_numeric (c, FALSE) || is_alpha (c) || (c == '_');
}

INLINE static D_BOOL
is_eol (char c)
{
  return (c == 0x0A || c == 0x0D);
}

typedef enum
{
  TK_CHAR,			/* represents a value of type char (e.g 'a') */
  TK_STRING,			/* represents a string */
  TK_DATETIME,			/* represents a date and time */
  TK_IDENTIFIER,		/* could be a keyword or an identifier */
  TK_NUMERIC,			/* contains only digits */
  TK_OPERATOR,			/* could be an operator */
  TK_REAL,			/* a real number */
  TK_KEYWORD,			/* the token is a key word */
  TK_UNDETERMINED,		/* token type is not yet determined */
  TK_ERROR
/* an obvious syntax error was encountered */
} TOKEN_TYPE;

/*
 * get_next_token()
 *
 * Find the next word in buffer, ignoring any white spaces
 * Warning!!! This function modifies the input buffer
 */
static TOKEN_TYPE
get_next_token (const char *buffer, char const **token, D_UINT * token_len)
{
  TOKEN_TYPE result = TK_UNDETERMINED;
  assert (token != NULL);
  assert (token_len != NULL);

  while (is_space (*buffer))
    ++buffer;

  if (*buffer == '#')
    {
      /* skip line with commentaries and try again */
      while (!is_eol (*buffer))
	++buffer;
      return get_next_token (buffer, token, token_len);
    }

  *token = buffer;
  *token_len = 0;
  if ((*buffer == '-') && is_numeric (buffer[1], FALSE))
    {
      buffer++;
    }
  if (is_numeric (*buffer, FALSE))
    {
      D_BOOL is_hexa = FALSE;
      if ((buffer[0] == '0') && (buffer[1] == 'x' || buffer[1] == 'X'))
	{
	  is_hexa = TRUE;
	  buffer += 2;
	}

      while (is_numeric (*buffer, is_hexa))
	{
	  buffer++;
	}
      result = TK_NUMERIC;
      if ((*buffer == '.') && (is_hexa == FALSE))
	{
	  ++buffer;
	  while (is_numeric (*buffer, FALSE))
	    {
	      ++buffer;
	    }
	  result = TK_REAL;
	}
    }
  else if (is_idlegal (*buffer))
    {
      while (is_idlegal (*buffer))
	{
	  buffer++;
	}
      result = TK_IDENTIFIER;
    }
  else if (*buffer == '+')
    {
      buffer++;
      if (*buffer == '+')
	{
	  buffer++;
	}
      result = TK_OPERATOR;
    }
  else if (*buffer == '-')
    {
      buffer++;
      if (*buffer == '-')
	{
	  buffer++;
	}
      result = TK_OPERATOR;
    }
  else if (*buffer == '<' || *buffer == '>' || *buffer == '!'
	   || *buffer == '=')
    {
      buffer++;
      if (*buffer == '=')
	{
	  buffer++;
	}
      result = TK_OPERATOR;
    }
  else if (*buffer == '\"')
    {
      do
	{
	  buffer++;
	  if (is_eol (*buffer) || (*buffer == 0))
	    {
	      /* an new line or end of buffer
	       * encountered before ending \" */
	      return TK_ERROR;
	    }
	}
      while ((*buffer != '\"') || ((*(buffer - 1) == '\\') && (*(buffer - 2)
							       != '\\')));
      buffer++;

      result = TK_STRING;
    }
  else if (*buffer == '\'')
    {
      do
	{
	  buffer++;
	  if (is_eol (*buffer) || (*buffer == 0))
	    {
	      /* an new line or end of buffer
	       * encountered before ending \' */
	      return TK_ERROR;
	    }
	}
      while ((*buffer != '\'') || ((*(buffer - 1) == '\\') && (*(buffer - 2)
							       != '\\')));
      buffer++;

      if ((*token)[1] == '\\' || (*token)[2] == '\'')
	{
	  /* this clearly is not a date time entry */
	  result = TK_CHAR;
	}
      else
	{
	  result = TK_DATETIME;
	}
    }
  else
    {
      buffer++;
    }

  *token_len = (D_UINT) (buffer - *token);

  return result;
}

typedef struct
{
  const char *string;
  D_UINT value;
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
  {"RECORD", RECORD},
  {"RETURN", RETURN},
  {"RICHREAL", RICHREAL},
  {"ROW", ROW},
  {"PROCEDURE", PROCEDURE},
  {"SYNC", SYNC},
  {"TABLE", TABLE},
  {"TEXT", TEXT},
  {"THEN", THEN},
  {"TRUE", W_TRUE},
  {"UNTIL", UNTIL},
  {"UNSIGNED", UNSIGNED},
  {"WHILE", WHILE},
  {"WITH", WITH},
  {"XOR", XOR},

  /* some aliases of our own */
  {"NUMBER", INT32},

  {NULL, 0}
};

static int
parse_keyword_value (const char *key, D_UINT key_len)
{
  char key_upcase[MAX_KEYWORD_LEN];
  D_UINT count;

  if (key_len >= MAX_KEYWORD_LEN)
    {
      return 0;
    }

  for (count = 0; count < key_len; count++)
    {
      key_upcase[count] = key[count];
      if (key[count] >= 'a' && key[count] <= 'z')
	{
	  key_upcase[count] += 'A' - 'a';
	}
    }
  key_upcase[count] = 0;	/* add the NULL end */

  count = 0;
  while (keywords[count].string != NULL)
    {
      if (strcmp (keywords[count].string, key_upcase) == 0)
	{
	  break;
	}
      ++count;
    }

  return keywords[count].value;
}

#define COMPOSED_OPERATOR_LEN    2	/* a compose operator has exactly 2 chars */
static TOKEN_SEMANTIC composed_operators[] = {
  {"==", EQ},
  {"!=", NE},
  {"<=", LE},
  {">=", GE},
  {"++", INC},
  {"--", DEC},
  {NULL, 0}
};

static int
parse_composed_operator_value (const char *op_str)
{
  D_UINT count = 0;
  while (composed_operators[count].string != NULL)
    {
      if (strncmp (composed_operators[count].string, op_str, COMPOSED_OPERATOR_LEN) == 0)
	{
	  break;
	}
      ++count;
    }

  return composed_operators[count].value;
}

static D_UINT
parse_integer_value (const char *buffer,
		     D_UINT buffer_len,
		     D_UINTMAX * output, D_BOOL * is_signed)
{
  const D_UINT old_len = buffer_len;
  D_UINT numeric_base = 10;
  D_BOOL negative = FALSE;

  assert (buffer != NULL);
  assert (buffer_len != 0);
  assert (output != NULL);

  *output = 0;
  *is_signed = FALSE;
  if (buffer[0] == '-' && buffer_len > 1 && is_numeric (buffer[1], FALSE))
    {
      negative = TRUE;
      buffer++;
      buffer_len--;
    }
  if (buffer[0] == '0' && buffer_len > 1
      && (buffer[1] == 'x' || buffer[1] == 'X'))
    {
      buffer_len -= 2;
      buffer += 2;
      numeric_base = 16;	/* hexa decimal notation */
    }
  else if (buffer[0] == '0')
    {
      buffer_len -= 1;
      buffer += 1;
      if (buffer_len > 0)
	{
	  /* leading 0 means the number is unsigned */
	}
    }

  if (negative)
    {
      *is_signed = TRUE;
    }

  while (buffer_len > 0)
    {
      D_UINT8 digit;

      if (*buffer >= '0' && *buffer <= '9')
	{
	  digit = (*buffer - '0');
	}
      else if (numeric_base == 16)
	{
	  if (*buffer >= 'a' && *buffer <= 'f')
	    {
	      digit = ((*buffer - 'a') + 10);
	    }
	  else if (*buffer >= 'A' && *buffer <= 'F')
	    {
	      digit = ((*buffer - 'A') + 10);
	    }
	  else
	    {
	      break;		/* no more hexa digits for you */
	    }
	}
      else
	{
	  break;		/* no more digits for you */
	}

      *output *= numeric_base;
      *output += digit;

      buffer++;
      buffer_len--;
    }

  if (negative)
    {
      *output *= -1;
    }

  return (old_len - buffer_len);
}

static D_UINT
parse_real_value (const char *buffer,
		  D_UINT buffer_len, struct SemCReal *output)
{
  const D_UINT old_len = buffer_len;
  D_BOOL dpif = FALSE;		/* Decimal Point Is Found */
  D_BOOL negative = FALSE;
  D_UINT frac_nible = 15;

  assert (buffer != NULL);
  assert (buffer_len != 0);
  assert (output != NULL);

  output->int_part = 0;
  output->frac_part = 0;
  if (buffer[0] == '-' && buffer_len > 1 && is_numeric (buffer[1], FALSE))
    {
      negative = TRUE;
      buffer++;
      buffer_len--;
    }

  while (buffer_len > 0)
    {
      int digit = 0;
      if (*buffer >= '0' && *buffer <= '9')
	{
	  digit += (*buffer - '0');
	  if (dpif == FALSE)
	    {
	      output->int_part *= 10;
	      output->int_part += digit;
	    }
	  else
	    output->frac_part |=
	      (((D_UINT64) digit & 0x0F) << (frac_nible-- * 4));
	}
      else if (*buffer == '.')
	{
	  if (dpif == TRUE)
	    {
	      /* we already found a decimal point */
	      return 0;
	    }
	  dpif = TRUE;
	}
      else
	{
	  break;		/* nothing for us here */
	}
      buffer++;
      buffer_len--;
    }

  if (negative)
    {
      output->int_part *= -1;
    }

  return (old_len - buffer_len);
}

static D_UINT
parse_char_value (const char *buffer, D_UINT buffer_len, D_CHAR * output)
{
  D_UINT result = 0;

  assert (buffer != NULL);
  assert (buffer_len != 0);
  assert (output != NULL);

  *output = 0;
  if (*buffer == '\\')
    {
      if (buffer_len > 0)
	{
	  ++buffer;
	  ++result;
	  --buffer_len;
	}
      else
	{
	  return 0;		/* error */
	}
      if (*buffer == '\\')
	{
	  *output = '\\';
	  ++result;
	}
      else if (*buffer == 'n')
	{
	  *output = '\n';
	  ++result;
	}
      else if (*buffer == 'r')
	{
	  *output = '\r';
	  ++result;
	}
      else if (*buffer == 'f')
	{
	  *output = '\f';
	  ++result;
	}
      else if (*buffer == 't')
	{
	  *output = '\t';
	  ++result;
	}
      else if (*buffer == 'v')
	{
	  *output = '\v';
	  ++result;
	}
      else if (*buffer == 'b')
	{
	  *output = '\b';
	  ++result;
	}
      else if (*buffer == 'a')
	{
	  *output = '\a';
	  ++result;
	}
      else if (*buffer == '\'')
	{
	  *output = '\'';
	  ++result;
	}
      else if (*buffer == '\"')
	{
	  *output = '\"';
	  ++result;
	}
      else if (is_numeric (*buffer, FALSE))
	{
	  D_UINTMAX int_value = 0;
	  D_BOOL dummy;
	  result +=
	    parse_integer_value (buffer, buffer_len, &int_value, &dummy);
	  *output = (D_CHAR) int_value;
	}
    }
  else
    {
      *output = *buffer;
      result = 1;
    }

  return result;
}

static D_UINT
parse_string_value (const char *buffer,
		    D_UINT buffer_len, D_CHAR * output, D_UINT * output_len)
{
  const D_UINT old_len = buffer_len;

  assert (buffer != NULL);
  assert (buffer_len != 0);
  assert (output != NULL);

  *output_len = 0;

  while ((buffer_len > 0) && (*buffer != '\"'))
    {
      D_UINT result = 0;
      result = parse_char_value (buffer, buffer_len, output);
      if (result != 0)
	{
	  buffer += result;
	  buffer_len -= result;
	  output++;
	  (*output_len)++;
	}
      else
	{			/* reserved for possible future error cases */
	  return 0;
	}
    }

  *output = 0;			/* add the null character */
  (*output_len)++;
  return (old_len - buffer_len);
}

static D_UINT
parse_time_value (const char *buffer,
		  D_UINT buffer_len, struct SemCTime *output)
{
  D_INTMAX int_v;
  D_UINT int_vlen;
  D_UINT result = 0;
  D_BOOL dummy;

  /* initialise the structure with default valid values */
  memset (output, 0, sizeof (output[0]));
  output->month = output->day = 1;

  /* found the year part */
  int_vlen = parse_integer_value (buffer, buffer_len, (D_UINTMAX *) & int_v,
				  &dummy);
  if (int_vlen > 0)
    {
      result += int_vlen;
      buffer += int_vlen;
      buffer_len -= int_vlen;
      output->year = (D_INT16) int_v;
    }
  else
    {
      return 0;			/* parsing error */
    }

  if (buffer[0] == '\'')
    {
      return result;		/* end of date/time entry */
    }
  else if (buffer[0] != '/')
    {
      /* no date delimiter */
      return 0;			/* parsing error */
    }
  else
    {
      ++result, ++buffer, --buffer_len;
    }

  /* found the month part */
  int_vlen = parse_integer_value (buffer, buffer_len, (D_UINTMAX *) & int_v,
				  &dummy);
  if (int_vlen > 0)
    {
      result += int_vlen;
      buffer += int_vlen;
      buffer_len -= int_vlen;
      output->month = (D_UINT8) int_v;
    }
  else
    {
      return 0;			/* parsing error */
    }

  if (buffer[0] == '\'')
    {
      return result;		/* end of date/time entry */
    }
  else if (buffer[0] != '/')
    {
      /* no date delimiter */
      return 0;			/* parsing error */
    }
  else
    {
      ++result, ++buffer, --buffer_len;
    }

  /* found the day part */
  int_vlen = parse_integer_value (buffer, buffer_len, (D_UINTMAX *) & int_v,
				  &dummy);
  if (int_vlen > 0)
    {
      result += int_vlen;
      buffer += int_vlen;
      buffer_len -= int_vlen;
      output->day = (D_UINT8) int_v;
    }
  else
    {
      return 0;			/* parsing error */
    }

  if (buffer[0] == '\'')
    {
      return result;		/* end of date/time entry */
    }
  else if (buffer[0] != ' ')
    {
      /* no date delimiter */
      return 0;			/* parsing error */
    }
  else
    {
      ++result, ++buffer, --buffer_len;
    }

  /* found the hour part */
  int_vlen = parse_integer_value (buffer, buffer_len, (D_UINTMAX *) & int_v,
				  &dummy);
  if (int_vlen > 0)
    {
      result += int_vlen;
      buffer += int_vlen;
      buffer_len -= int_vlen;
      output->hour = (D_UINT8) int_v;
    }
  else
    {
      return 0;			/* parsing error */
    }

  if (buffer[0] != ':')
    {
      /* no time delimiter */
      return 0;			/* parsing error */
    }
  else
    {
      ++result, ++buffer, --buffer_len;
    }

  /* found the minute part */
  int_vlen = parse_integer_value (buffer, buffer_len, (D_UINTMAX *) & int_v,
				  &dummy);
  if (int_vlen > 0)
    {
      result += int_vlen;
      buffer += int_vlen;
      buffer_len -= int_vlen;
      output->min = (D_UINT8) int_v;
    }
  else
    {
      return 0;			/* parsing error */
    }

  if (buffer[0] == '\'')
    {
      return result;
    }
  else if (buffer[0] != ':')
    {
      /* no time delimiter */
      return 0;			/* parsing error */
    }
  else
    {
      ++result, ++buffer, --buffer_len;
    }
  /* found the second part */
  int_vlen = parse_integer_value (buffer, buffer_len, (D_UINTMAX *) & int_v,
				  &dummy);
  if (int_vlen > 0)
    {
      result += int_vlen;
      buffer += int_vlen;
      buffer_len -= int_vlen;
      output->sec = (D_UINT8) int_v;
    }
  else
    {
      return 0;			/* parsing error */
    }

  if (buffer[0] == '\'')
    {
      return result;
    }
  else if (buffer[0] != '.')
    {
      /* no time delimiter */
      return 0;			/* parsing error */
    }
  else
    {
      ++result, ++buffer, --buffer_len;
    }
  /* found the microsecond part */
  int_vlen = parse_integer_value (buffer, buffer_len, (D_UINTMAX *) & int_v,
				  &dummy);
  if (int_vlen > 0)
    {
      result += int_vlen;
      buffer += int_vlen;
      buffer_len -= int_vlen;
      output->usec = (D_UINT32) int_v;
    }
  else
    {
      return 0;			/* parsing error */
    }

  return result;
}

int
yylex (YYSTYPE * lvalp, struct ParserState *state)
{
  int result = 0;
  const D_CHAR *buffer = state->buffer;
  const D_CHAR *token = NULL;
  D_UINT token_len = 0;
  TOKEN_TYPE token_type;
  D_UINT buffer_pos = state->buffer_pos;

  if (state->buffer_pos > state->buffer_len)
    return 0;

  /* recall where to start from */
  buffer += buffer_pos;
  token_type = get_next_token (buffer, &token, &token_len);

  /* remember to start from here  next time */
  state->buffer_pos += (D_UINT) ((token + token_len) - buffer);

  /* allocate storage for value */
  *lvalp = NULL;		/* if we are to crash... make it loud */
  if ((token_type != TK_UNDETERMINED) && (token_type != TK_OPERATOR)
      && ((token_type != TK_IDENTIFIER)
	  || ((result = parse_keyword_value (token, token_len)) == 0)))
    {
      /* if the token is not a key word */
      *lvalp = get_sem_value (state);
      if (*lvalp == NULL)
	{
	  w_log_msg (state, IGNORE_BUFFER_POS, state->buffer_pos, MSG_NO_MEM);
	  return 0;
	}
    }
  else if (token_type == TK_IDENTIFIER)
    {
      /* after we parsed the token we found is a keyword */
      token_type = TK_KEYWORD;
      return result;
    }
  else if (token_len == 1)
    {
      return *token;
    }

  switch (token_type)
    {
    case TK_IDENTIFIER:
      /* parsing was successful */
      (*lvalp)->val_type = VAL_ID;
      (*lvalp)->val.u_id.text = token;
      (*lvalp)->val.u_id.length = token_len;
      result = IDENTIFIER;
      break;
    case TK_OPERATOR:
      return parse_composed_operator_value (token);
    case TK_NUMERIC:
      result = parse_integer_value (token, token_len,
				    &((*lvalp)->val.u_int.value),
				    &((*lvalp)->val.u_int.is_signed));
      if (result != 0)
	{			/* parsing was successful */
	  (*lvalp)->val_type = VAL_C_INT;
	  result = WHISPER_INTEGER;
	}
      break;
    case TK_CHAR:
      if ((token_len > 2) || (token[0] != '\''))
	{
	  token++;
	  result = parse_char_value (token, token_len - 1,
				     &((*lvalp)->val.u_char.value));
	  if (token[result] != '\'')
	    {
	      result = 0;
	    }
	}
      else
	{
	  result = 0;		/* error */
	}
      if (result != 0)
	{			/* parsing was successful */
	  (*lvalp)->val_type = VAL_C_CHAR;
	  result = WHISPER_CHARACTER;
	}
      break;
    case TK_STRING:
      if ((token_len > 1) || (token[0] != '\"'))
	{
	  token++;
	  (*lvalp)->val.u_text.text = alloc_str (state->strs, token_len - 1);
	  result = parse_string_value (token, token_len,
				       (*lvalp)->val.u_text.text,
				       &(*lvalp)->val.u_text.length);
	  if (token[result] != '\"')
	    {
	      result = 0;
	    }
	  else
	    {
	      /* parsing was successful, result contains the
	       * length of the text */
	      (*lvalp)->val_type = VAL_C_TEXT;
	      result = WHISPER_TEXT;
	    }
	}
      else
	{
	  result = 0;		/* error */
	}
      break;
    case TK_DATETIME:
      if ((token_len > 2) || (token[0] != '\''))
	{
	  token++;
	  result = parse_time_value (token, token_len - 1,
				     &((*lvalp)->val.u_time));
	  if (token[result] != '\'')
	    {
	      result = 0;
	    }
	}
      else
	{
	  result = 0;		/* error */
	}
      if (result != 0)
	{
	  (*lvalp)->val_type = VAL_C_TIME;
	  /* parsing was successful */
	  result = WHISPER_TIME;
	}
      break;
    case TK_REAL:
      result = parse_real_value (token, token_len, &((*lvalp)->val.u_real));
      if (result != 0)
	{			/* parsing was successful */
	  (*lvalp)->val_type = VAL_C_REAL;
	  result = WHISPER_REAL;
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
yyerror (struct ParserState *state, const D_CHAR * msg)
{
  w_log_msg (state, state->buffer_pos, MSG_COMPILER_ERR);

  return 0;
}
