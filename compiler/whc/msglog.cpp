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
#include <cassert>
#include <vector>

#include "msglog.h"


using namespace std;



static const char *MSG_PREFIX[] = {
  "", "error ", "warning ", "error "
};



static uint_t
get_line_from_buffer (const vector<SourceCodeMark>& codeMark,
                      const char*                   buffer,
                      uint_t                        bufferOff,
                      int* const                    mark)
{
  *mark = -1;

  if (bufferOff == WHC_IGNORE_BUFFER_POS)
    return -1;

  for (uint_t i = 0;
      (i < codeMark.size ()) && (bufferOff <= codeMark[i].mBufferOffset);
      ++i)
    {
      *mark = i;
    }

  if (*mark < 0)
    return -1;

  uint_t count  = codeMark[*mark].mBufferOffset;
  int    result = codeMark[*mark].mBufferLine;

  while (count < bufferOff)
    {
      if (buffer[count] == '\n')
        ++result;

      else if (buffer[count] == 0)
        {
          assert (count == bufferOff - 1);

          if (buffer[count - 1] == '\n')
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
whc_messenger (WH_MESSENGER_CTXT    data,
               uint_t               buffOff,
               uint_t               msgId,
               uint_t               msgType,
               const char*          msgFormat,
               va_list              args)
{
  const WHC_MESSAGE_CTX&        ctx       = *_RC(const WHC_MESSAGE_CTX*, data);
  const char* const             buffer    = ctx.mCode;
  const vector<SourceCodeMark>& codeMarks = ctx.mCodeMarks;

  int mark     = 0;
  int buffLine = get_line_from_buffer (codeMarks, buffer, buffOff, &mark);

  fprintf (stderr, MSG_PREFIX[msgType]);
  if (buffLine > 0)
    {
      assert (codeMarks.size () > 0);

          assert (codeMarks[mark].mLevel == 0);

          fprintf (stderr,
                   "%s:line %d: %d: ",
                   codeMarks[mark].mBufferSource.c_str (),
                   buffLine,
                   msgId);
          vfprintf (stderr, msgFormat, args);

          while (codeMarks[mark].mLevel > 0)
            {
              const uint_t prevLevel = codeMarks[mark].mLevel - 1;

              while (codeMarks[mark].mLevel > prevLevel)
                {
                  assert (mark > 0);
                  --mark;
                }

              assert (codeMarks[mark].mLevel == prevLevel);

              fprintf (stderr,
                      "\n\tFile is included from '%s' at line %d%c",
                      codeMarks[mark].mBufferSource.c_str (),
                      codeMarks[mark].mBufferLine,
                      prevLevel == 0 ? '.' : ';');
            }

      fprintf (stderr, "%d : line %d: ", msgId, buffLine);
    }
  else
    {
      fprintf (stderr, "%d : ", msgId);
      vfprintf (stderr, msgFormat, args);
    }

  fprintf (stderr, "\n");
}


void
whc_messenger (WH_MESSENGER_CTXT data,
               uint_t            buffOff,
               uint_t            msgId,
               uint_t            msgType,
               const char*       msgFormat,
               ...)
{

}

