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
#include "msglog.h"

#include <string.h>
#include <stdarg.h>
#include <assert.h>

static WLOG_FUNC_CONTEXT sgLoggerContext = NULL;
static WLOG_FUNC         sgLogger        = NULL;

void
wh_register_logger (WLOG_FUNC man, WLOG_FUNC_CONTEXT bag)
{
  sgLoggerContext = bag;
  sgLogger = man;
}


WLOG_FUNC
wh_logger (void)
{
  return sgLogger;
}

WLOG_FUNC_CONTEXT
wh_logger_context (void)
{
  return sgLoggerContext;
}


void
wh_log_msg (uint_t        position,
            uint_t        code,
            uint_t        type,
            char*         formatedMsg,
            va_list       args)
{
  sgLogger (sgLoggerContext, position, code, type, formatedMsg, args);
}


char*
wh_copy_first (char*         dest,
               const char*   src,
               uint_t        destMax,
               uint_t        srcLength)
{
  assert (destMax > 4);  /* Make sure we can hold the '...' string. */

  if (srcLength > destMax)
    {
      srcLength = destMax - 4;
      strncpy (dest, src, srcLength);
      dest[srcLength] = 0;
      strcat (dest, "...");
    }
  else
    {
      strncpy (dest, src, srcLength);
      dest[srcLength] = 0;
    }

  return dest;
}

