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

#include <stdarg.h>

#include "whisper.h"
#include "../../common/whisper.h"
#include "../../compiler/include/whisperc/whisperc.h"

#ifndef MSGLOG_H_
#define MSGLOG_H_

void
my_postman (WHC_MESSENGER_ARG data,
            D_UINT            buffOff,
            D_UINT            msgId,
            D_UINT            msgType,
            const D_CHAR*     msgFormat,
            va_list           args);

#endif /* MSGLOG_H_ */
