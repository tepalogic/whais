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


static const char *MSG_PREFIX[] = {
  "", "error ", "warning ", "error "
};

static uint_t
get_line_from_buffer (const char* pBuffer, uint_t bufferOff)
{
  uint_t count = 0;
  int result = 1;

  if (bufferOff == WHC_IGNORE_BUFFER_POS)
    return -1;

  while (count < bufferOff)
    {
      if (pBuffer[count] == '\n')
        ++result;
      else if (pBuffer[count] == 0)
        {
          assert (count == bufferOff - 1);
          if (pBuffer[count - 1] == '\n')
            {
              assert (result > 1);
              return result - 1;
            }
          else
            return result;
        }
      ++count;
    }
  return result;
}

void
my_postman (WHC_MESSENGER_ARG data,
            uint_t            buffOff,
            uint_t            msgId,
            uint_t            msgType,
            const char*     msgFormat,
            va_list           args)
{
  const char* pBuffer  = (const char*)data;
  int         buffLine = get_line_from_buffer (pBuffer, buffOff);

  fprintf (stderr, MSG_PREFIX[msgType]);
  fprintf (stderr, "%d : line %d: ", msgId, buffLine);
  vfprintf (stderr, msgFormat, args);
  fprintf (stderr, "\n");
}
