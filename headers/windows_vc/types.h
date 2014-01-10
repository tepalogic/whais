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
#error "Do not compile this file with other compilator than MS Visual C/C++"
#endif

#include "sys/types.h"

#if ! (defined(YYTOKENTYPE) || defined (YYBISON))
/* Avoid some type name redefinition */

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINSTYLES
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOSCROLL
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NODEFERWINDOWPOS
#define NOMCX


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef HANDLE              WH_FILE;
typedef CRITICAL_SECTION    WH_LOCK;
typedef HANDLE              WH_THREAD;
typedef SOCKET              WH_SOCKET;
typedef HMODULE             WH_SHLIB;
#endif

/* Simple type shortcuts */
typedef unsigned int       uint_t;
typedef unsigned long      ulong_t;
typedef long long          llong_t;
typedef unsigned long long ullong_t;

typedef __int8           int8_t;
typedef unsigned __int8  uint8_t;

typedef __int16          int16_t;
typedef unsigned __int16 uint16_t;

typedef __int32          int32_t;
typedef unsigned __int32 uint32_t;

typedef __int64          int64_t;
typedef unsigned __int64 uint64_t;

#ifndef bool_t
typedef uint8_t       bool_t;
#endif

#define INVALID_FILE        INVALID_HANDLE_VALUE
#define INVALID_SHL         NULL

#define SHL_EXPORT_SYMBOL __declspec(dllexport)
#define SHL_IMPORT_SYMBOL __declspec(dllimport)

#endif /* WHISPER_TYPES_H */
