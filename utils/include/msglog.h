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
#ifndef MSGLOG_H_
#define MSGLOG_H_

#include "whisper.h"
#include "stdarg.h"

typedef const void *POSTMAN_BAG;

#define IGNORE_BUFFER_POS      (D_UINT)(~0)

typedef void (*POSTMAN) (POSTMAN_BAG bag,
                         D_UINT      bufferPosition,
                         D_UINT      msgCode,
                         D_UINT      msgType,
                         D_CHAR*     msgFormat,
                         va_list     args);

void
register_postman (POSTMAN man, POSTMAN_BAG handle);

POSTMAN
get_postman (void);

POSTMAN_BAG
get_postman_bag (void);

void
LOGMSG (D_UINT  buff_pos,
        D_UINT  msgCode,
        D_UINT  msgType,
        D_CHAR* msgFormat,
        va_list args);

/* utils for LOGMSG arguments */
D_CHAR *copy_text_truncate (D_CHAR*        dest,
                            const D_CHAR*  src,
                            D_UINT         dest_max,
                            D_UINT         src_len);

#endif /*MSGLOG_H_ */
