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

#define IGNORE_BUFFER_POS      (uint_t)(~0)

typedef void (*POSTMAN) (POSTMAN_BAG bag,
                         uint_t      bufferPosition,
                         uint_t      msgCode,
                         uint_t      msgType,
                         char*     msgFormat,
                         va_list     args);

void
register_postman (POSTMAN man, POSTMAN_BAG handle);

POSTMAN
get_postman (void);

POSTMAN_BAG
get_postman_bag (void);

void
LOGMSG (uint_t  buff_pos,
        uint_t  msgCode,
        uint_t  msgType,
        char* msgFormat,
        va_list args);

/* utils for LOGMSG arguments */
char*
copy_text_truncate (char*       dest,
                   const char*  src,
                   uint_t         destMax,
                   uint_t         srcLength);

#endif /*MSGLOG_H_ */
