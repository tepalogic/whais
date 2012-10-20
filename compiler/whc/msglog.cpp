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

#include <cstdio>
#include <assert.h>

#include "msglog.h"


static const D_CHAR *MSG_PREFIX[] = {
  "", "error ", "warning ", "error "
};

static D_UINT
get_line_from_buffer (const D_CHAR* pBuffer, D_UINT bufferOff)
{
  D_UINT count = 0;
  D_INT result = 1;

  if (bufferOff == WHC_IGNORE_BUFFER_POS)
    return -1;

  while (count < bufferOff)
    {
      if (pBuffer[count] == '\n')
        ++result;
      else if (pBuffer[count] == 0)
        {
          assert (0);
        }
      ++count;
    }
  return result;
}

void
my_postman (WHC_MESSENGER_ARG data,
            D_UINT            buffOff,
            D_UINT            msgId,
            D_UINT            msgType,
            const D_CHAR*     msgFormat,
            va_list           args)
{
  const D_CHAR* pBuffer  = (const D_CHAR*)data;
  D_INT         buffLine = get_line_from_buffer (pBuffer, buffOff);

  fprintf (stderr, MSG_PREFIX[msgType]);
  fprintf (stderr, "%d : line %d: ", msgId, buffLine);
  vfprintf (stderr, msgFormat, args);
  fprintf (stderr, "\n");
}
