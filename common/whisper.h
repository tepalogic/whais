/******************************************************************************
WHISPER - An advanced database system
Copyright (C) 2008  Iulian Popa

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

#ifndef WHISPER_H
#define WHISPER_H

#include <stddef.h>

/* This section includes compiler specific features */
#ifdef ARCH_LINUX_GCC
#include "linux_gcc/types.h"
#endif

#ifdef ARCH_WINDOWS_VC
#include "windows_vc/types.h"
#endif

/* This section contains whisper specific data */


/* This section will contain some general data */
#include "general/whisper_macros.h"
#include "whisper_shl.h"

#ifdef __cplusplus
#define _DC(type, value)     (dynamic_cast<type> (value))
#define _CC(type, value)     (const_cast<type> (value))
#define _SC(type, value)     (static_cast<type> (value))
#define _RC(type, value)     (reinterpret_cast<type> (value))
#include "wexception.h"
#endif

#include "whisper_memory.h"
#if ! (defined(YYTOKENTYPE) || defined (YYBISON))
/* Avoid some type name redefinition (for Windows) */
#include "whisper_fileio.h"
#include "whisper_thread.h"
#endif

/* General operation status codes */
#define WOP_OK           0
#define WOP_UNKNOW      -1

#endif /* WHISPER_H */
