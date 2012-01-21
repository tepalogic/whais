/******************************************************************************
WHISPERC - A compiler for whisper programs
Copyright (C) 2008  Iulian Popa

Address: Str Olimp nr. 6
         Pantelimon Ilfov,
         Rommania
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

#ifndef WHISPER_TYPES_H
#define WHISPER_TYPES_H

#ifndef ARCH_LINUX_GCC
#error "Do not compile this file with other compilator than linux's gcc/g++!"
#endif

#include "sys/types.h"
#include "pthread.h"

typedef int             WH_FILE_HND;
typedef pthread_mutex_t WH_SYNC;
typedef pthread_cond_t  WH_COND_VALUE;
typedef pthread_t       WH_THREAD;

typedef char          D_BOOL;

typedef char          D_CHAR;
typedef unsigned char D_UCHAR;

typedef int D_INT;
typedef unsigned int  D_UINT;

typedef int8_t        D_INT8;
typedef u_int8_t      D_UINT8;

typedef int16_t       D_INT16;
typedef u_int16_t     D_UINT16;

typedef int32_t       D_INT32;
typedef u_int32_t     D_UINT32;

typedef int64_t       D_INT64;
typedef u_int64_t     D_UINT64;

/* D_INTMAX is the type that could hold the biggest integer
 * for a specific architecture */
typedef D_INT64       D_INTMAX;
typedef D_UINT64      D_UINTMAX;

#endif /* WHISPER_TYPES_H */
