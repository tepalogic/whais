/******************************************************************************
UTILS - Common routines used trough WHAIS project
Copyright(C) 2009  Iulian Popa

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


#include <stdarg.h>

#include "whais.h"


typedef const void* WLOG_FUNC_CONTEXT;
typedef void(*WLOG_FUNC) (WLOG_FUNC_CONTEXT      bag,
                           uint_t                 msgPosition,
                           uint_t                 msgCode,
                           uint_t                 msgType,
                           char*                  msgFormat,
                           va_list                args);


#define IGNORE_BUFFER_POS     (uint_t)(~0)


/* Allow one to personalize the message handling for messages generated
 * during compilation phase. */
void
wh_register_logger(WLOG_FUNC callback, WLOG_FUNC_CONTEXT context);

WLOG_FUNC
wh_logger(void);

WLOG_FUNC_CONTEXT
wh_logger_context(void);

/* Called whenever a message has to be generated. */
void
wh_log_msg(uint_t position, uint_t code, uint_t type, char* formatedMsg, va_list args);

/* Utility function to truncate and append '...' when the source
 * is too large. */
char*
wh_copy_first(char* dest, const char* src, uint_t destMax, uint_t srcLength);


#endif /*MSGLOG_H_ */
