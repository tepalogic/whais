/******************************************************************************
  WCMD - An utility to manage whais database files.
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

#ifndef WCMD_OPTGLBS_H_
#define WCMD_OPTGLBS_H_

#include <string.h>

#include "whais.h"

#include "dbs/dbs_mgr.h"

typedef enum
{
  VL_NONE,
  VL_STATUS,
  VL_ERROR,
  VL_WARNING,
  VL_INFO,
  VL_DEBUG,

  VL_MAX = VL_DEBUG
} VERBOSE_LEVEL;


const std::string&
GetRemoteHostName ();


void
SetRemoteHostName (const char* const host);


const std::string&
GetConnectionPort ();


void
SetConnectionPort (const char* const port);


uint_t
GetUserId ();


void
SetUserId (const uint_t userId);

const std::string&
GetUserPassword ();


void
SetUserPassword (const char* const password);


const std::string&
GetWorkingDirectory ();


void
SetWorkingDirectory (const char* const directory);


const std::string&
GetWorkingDB ();

void
SetWorkingDB (const char* const dbName);


VERBOSE_LEVEL
GetVerbosityLevel ();


void
SetVerbosityLevel (const uint_t level);


bool
SetMaximumFileSize (std::string size);


uint64_t
GetMaximumFileSize ();


void
SetDbsHandler (whais::IDBSHandler& dbs);


whais::IDBSHandler&
GetDBSHandler ();


bool
IsOnlineDatabase ();


#endif //WCMD_OPTGLBS_H_

