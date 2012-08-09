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

#ifndef ARCH_WINDOWS_VC
#error Do not compile this file with other compilator than MS Visual C
#endif

#include "sys/types.h"

#if ! (defined(YYTOKENTYPE) || defined (YYBISON))
/* Avoid some type name redefinition */
#include "windows.h"
typedef HANDLE              WH_FILE_HND;
typedef CRITICAL_SECTION    WH_SYNC;
typedef HANDLE              WH_THREAD;
#endif

typedef char             D_BOOL;

typedef char             D_CHAR;
typedef unsigned char    D_UCHAR;

typedef int              D_INT;
typedef unsigned int     D_UINT;

typedef __int8           D_INT8;
typedef unsigned __int8  D_UINT8;

typedef __int16          D_INT16;
typedef unsigned __int16 D_UINT16;

typedef __int32          D_INT32;
typedef unsigned __int32 D_UINT32;

typedef __int64          D_INT64;
typedef unsigned __int64 D_UINT64;

/* D_INTMAX is the type that could hold the biggest integer
 * for a specific architecture */
typedef D_INT64          D_INTMAX;
typedef D_UINT64         D_UINTMAX;

#define SHL_EXPORT_SYMBOL __declspec(dllexport)
#define SHL_IMPORT_SYMBOL __declspec(dllimport)


#endif /* WHISPER_TYPES_H */
