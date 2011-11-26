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

static POSTMAN_BAG __postman_bag = NULL;
static POSTMAN __postman = NULL;

void
register_postman (POSTMAN man, POSTMAN_BAG bag)
{
  __postman_bag = bag;
  __postman = man;
}

POSTMAN
get_postman (void)
{
  return __postman;
}

POSTMAN_BAG
get_postman_bag (void)
{
  return __postman_bag;
}

void
LOGMSG (D_UINT buff_pos, D_UINT msgCode, D_UINT msgType, D_CHAR * msgFormat,
	va_list args)
{
  __postman (__postman_bag, buff_pos, msgCode, msgType, msgFormat, args);
}

D_CHAR *
copy_text_truncate (D_CHAR * dest,
		    const D_CHAR * src, D_UINT dest_max, D_UINT src_len)
{

  assert (dest_max > 4);	/* make sure we can hold the '...' string */

  if (src_len > dest_max)
    {
      src_len = dest_max - 4;
      strncpy (dest, src, src_len);
      dest[src_len] = 0;
      strcat (dest, "...");
    }
  else
    {
      strncpy (dest, src, src_len);
      dest[src_len] = 0;
    }

  return dest;
}
