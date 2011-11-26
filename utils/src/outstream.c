/******************************************************************************
UTILS - Common routines used trough WHISPER project
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

#include "outstream.h"

struct OutStream *
init_outstream (struct OutStream *os, D_UINT inc_size)
{
  os->size = 0;
  os->increment = (inc_size != 0) ? inc_size : OUTSTREAM_INCREMENT_SIZE;
  os->output = NULL;

  os->allocated = 0;

  return os;
}

struct OutStream *
data_outstream (struct OutStream *os, const D_UINT8 * data, D_UINT data_size)
{
  assert (os->allocated >= os->size);

  if (data_size + os->size > os->allocated)
    {
      /* output buffer needs to be enlarged */

      D_UINT8 *temp;
      D_UINT increment = data_size + os->size - os->allocated;
      if (increment > os->increment)
	{
	  /* The increment defined previously is not enough to hold the new
	   * data. Avoid this problem in future */
	  os->increment = increment;
	}
      temp = mem_realloc (os->output, os->increment + os->allocated);
      if (temp == NULL)
	{
	  return NULL;		/* not enough memory */
	}

      os->output = temp;
      os->allocated += os->increment;
    }

  /* add the data at the end */
  while (data_size-- > 0)
    {
      os->output[os->size++] = *data++;
    }

  return os;
}

void
destroy_outstream (struct OutStream *os)
{
  if (os->output != NULL)
    {
      mem_free (os->output);
    }
  os->output = NULL;
}
