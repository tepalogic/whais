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

struct OutStream*
init_outstream (struct OutStream* pOutStream, D_UINT increment)
{
  pOutStream->dataSize  = 0;
  pOutStream->increment = (increment != 0) ? increment : OUTSTREAM_INCREMENT_SIZE;
  pOutStream->data      = NULL;
  pOutStream->reserved = 0;

  return pOutStream;
}

void
destroy_outstream (struct OutStream* pStream)
{
  if (pStream->data != NULL)
    mem_free (pStream->data);

  pStream->data = NULL;
}

struct OutStream*
output_data (struct OutStream *pStream, const D_UINT8* pData, D_UINT dataSize)
{
  assert (pStream->reserved >= pStream->dataSize);

  if (dataSize + pStream->dataSize > pStream->reserved)
    {
      /* output buffer needs to be enlarged */

      D_UINT8* temp;
      D_UINT   increment = dataSize + pStream->dataSize - pStream->reserved;
      if (increment > pStream->increment)
        {
          /* The increment defined previously is not enough to hold the new
           * data. Avoid this problem in future */
          pStream->increment = increment;
        }

      temp = mem_realloc (pStream->data, pStream->increment + pStream->reserved);
      if (temp == NULL)
        return NULL;                /* not enough memory */

      pStream->data     = temp;
      pStream->reserved += pStream->increment;
    }

  /* add the data at the end */
  while (dataSize-- > 0)
    pStream->data[pStream->dataSize++] = *pData++;

  return pStream;
}
