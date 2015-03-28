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

#ifndef WCMD_CMDSMGR_H_
#define WCMD_CMDSMGR_H_

#include <string>
#include <iostream>

#include "whais.h"
#include "wexception.h"



typedef void* ENTRY_CMD_CONTEXT;
typedef bool (*ENTRY_CMD) (const std::string& cmdLine, ENTRY_CMD_CONTEXT);



struct CmdEntry
{
  const char*       mName;
  const char*       mDesc;
  const char*       mExtendedDesc;

  ENTRY_CMD         mCmd;
  ENTRY_CMD_CONTEXT mContext;

  bool              mShowStatus;
};


void
InitCmdManager ();


void
RegisterCommand (const CmdEntry& entry);


const CmdEntry*
FindCmdEntry (const char* const pCommand);


const std::string
CmdLineNextToken (const std::string& cmdLine, size_t& inoutPosition);


void
printException (std::ostream& outputStream, const whais::Exception& e);


std::string
wcmd_decode_typeinfo (unsigned int type);

#endif // WCMD_CMDSMGR_H_

