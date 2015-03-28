/******************************************************************************
UTILS - Common routines used trough WHAIS project
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

#include "woutstream.h"

struct WOutputStream*
wh_ostream_init (const uint_t                increment,
                 struct WOutputStream* const outStream)
{
  assert (outStream != NULL);

  outStream->dataSize  = 0;
  outStream->increment = (increment != 0) ?
                            increment :
                            OUTSTREAM_INCREMENT_SIZE;
  outStream->data      = NULL;
  outStream->reserved  = 0;

  return outStream;
}


void
wh_ostream_clean (struct WOutputStream* const stream)
{
  if (stream->data != NULL)
    mem_free (stream->data);

  stream->data = NULL;
}


struct WOutputStream*
wh_ostream_write (struct WOutputStream* const stream,
                  const uint8_t*              data,
                  uint_t                      dataSize)
{
  assert (stream->reserved >= stream->dataSize);

  if (dataSize + stream->dataSize > stream->reserved)
    {
      /* output buffer needs to be enlarged */

      uint8_t* temp;
      uint_t   increment = dataSize + stream->dataSize - stream->reserved;
      if (increment > stream->increment)
        {
          /* The increment defined previously is not enough to hold the new
           * data. Avoid this problem in future */
          stream->increment = increment;
        }

      temp = mem_realloc (stream->data, stream->increment + stream->reserved);
      if (temp == NULL)
        return NULL;

      stream->data     = temp;
      stream->reserved += stream->increment;
    }

  /* add the data at the end */
  while (dataSize-- > 0)
    stream->data[stream->dataSize++] = *data++;

  return stream;
}

