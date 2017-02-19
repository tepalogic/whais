/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#ifndef WHAIS_H
#define WHAIS_H

#include <stddef.h>

#include "whais_types.h"
#include "general/whais_macros.h"
#include "whais_shl.h"

#ifdef __cplusplus
#define _DC(type, value)     (dynamic_cast<type> (value))
#define _CC(type, value)     (const_cast<type> (value))
#define _SC(type, value)     (static_cast<type> (value))
#define _RC(type, value)     (reinterpret_cast<type> (value))
#include "wexception.h"
#endif

#include "whais_memory.h"
#include "whais_time.h"
#if ! (defined(YYTOKENTYPE) || defined(YYBISON))
/* Avoid some type name redefinition(for Windows) */
#include "whais_fileio.h"
#include "whais_thread.h"
#include "whais_net.h"
#include "whais_console.h"
#endif

/* General operation status codes */
#define WOP_OK           0
#define WOP_UNKNOW       (~0)

#endif /* WHAIS_H */

