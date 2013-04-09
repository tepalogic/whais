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
#error "Do not compile this file with a compilator other than Linux's gcc/g++!"
#endif

#include "sys/types.h"
#include "pthread.h"

#include <stdint.h>

typedef int             WH_FILE_HND;
typedef pthread_mutex_t WH_SYNC;
typedef pthread_t       WH_THREAD;
typedef int             WH_SOCKET;

/* Simple type shortcuts */
typedef unsigned int       uint_t;
typedef unsigned long      ulong_t;
typedef long long          llong_t;
typedef unsigned long long ullong_t;

#ifndef int8_t
typedef int8_t        int8_t;
#endif

#ifndef uint8_t
typedef u_int8_t      uint8_t;
#endif

#ifndef int16_t
typedef int16_t       int16_t;
#endif

#ifndef uint16_t
typedef u_int16_t     uint16_t;
#endif

#ifndef int32_t
typedef int32_t       int32_t;
#endif

#ifndef uint32_t
typedef u_int32_t     uint32_t;
#endif

#ifndef int64_t
typedef int64_t       int64_t;
#endif

#ifndef uint64_t
typedef u_int64_t     uint64_t;
#endif

#ifndef bool_t
typedef uint8_t       bool_t;
#endif

#define SHL_EXPORT_SYMBOL __attribute__((visibility("default")))
#define SHL_IMPORT_SYMBOL __attribute__((visibility("default")))

#define INVALID_SOCKET  ((int)-1)

#endif /* WHISPER_TYPES_H */
