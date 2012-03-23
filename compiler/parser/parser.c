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
 * parser.c - Provides the functionality required by the bison output source
 *            related to semantic value objects management.
 */

#include "parser.h"
#include "../semantics/wlog.h"

#include <assert.h>

struct SemValue *
get_sem_value (struct ParserState *state)
{
  struct SemValue *result = NULL;
  D_UINT iter = 0;
  D_UINT stored_vals = get_array_count (&state->parsedValues);

  while (iter < stored_vals)
    {
      result = get_item (&state->parsedValues, iter);
      assert (result != NULL);
      if (result->val_type == VAL_REUSE)
	break;			/* found something to be reused */
      iter++;
    }
  if (result == NULL || result->val_type != VAL_REUSE)
    {
      static struct SemValue dummy;
      result = (struct SemValue *) add_item (&state->parsedValues, &dummy);
    }
  return result;
}

struct SemValue *
get_bool_sem_value (struct ParserState *state, D_BOOL value)
{
  struct SemValue *val = get_sem_value (state);

  if (val == NULL)
    w_log_msg (state, IGNORE_BUFFER_POS, state->bufferPos, MSG_NO_MEM);
  else
    {
      val->val_type = VAL_C_BOOL;
      val->val.u_bool.value = value;
    }

  return val;
}
