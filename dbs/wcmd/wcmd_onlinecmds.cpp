/******************************************************************************
  WCMD - An utility to manage whisper database files.
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

#include <assert.h>
#include <string>
#include <iostream>
#include <vector>

#include "whisper.h"

#include "client/include/whisper_connector.h"

#include "wcmd_onlinecmds.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"

using namespace std;

const D_CHAR*
translate_status (CONNECTOR_STATUS cs)
{
  switch (cs)
  {
  case CS_OK:
    return "No error returned.";
  case CS_INVALID_ARGS:
    return "Invalid arguments.";
  case CS_OP_NOTPERMITED:
    return "Operation not permitted.";
  case CS_DROPPED:
    return "Connection dropped by peer.";
  case CS_ENCTYPE_NOTSUPP:
    return "Could not agree on supported encryption type.";
  case CS_UNEXPECTED_FRAME:
    return "Unexpected communication frame received.";
  case CS_INVALID_FRAME:
    return "An frame with invalid content received.";
  case CS_COMM_OUT_OF_SYNC:
    return "Communication is out of sync.";
  case CS_LARGE_ARGS:
    return "Size of the operation arguments is not supported.";
  case CS_CONNECTION_TIMEOUT:
    return "Connection has timeout.";
  case CS_SERVER_BUSY:
    return "Server is too busy.";
  case CS_OS_INTERNAL:
    return "OS internal error encountered.";
  case CS_UNKNOWN_ERR:
    return "An unknown error was encountered.";
  }

  return "Unknown error.";
}

static const D_CHAR globalShowDesc[]    = "List context database's "
                                         "global variables.";
static const D_CHAR globalShowDescExt[] =
"Show the global variables installed in the database context.\n"
"If a name is provided it limits the listing to only those variables.\n"
"Usage:\n"
"  global [variable_name] ... ";

static bool
cmdGlobalList (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{

  CONNECTOR_HND       conHdl    = NULL;
  unsigned int        glbsCount = 0;
  const VERBOSE_LEVEL level     = GetVerbosityLevel ();

  CONNECTOR_STATUS cs  = Connect (GetRemoteHostName ().c_str (),
                                  GetConnectionPort ().c_str (),
                                  GetWorkingDB ().c_str (),
                                  GetUserPassword ().c_str (),
                                  GetUserId (),
                                  &conHdl);
  if (cs != CS_OK)
    {
      if (level >= VL_DEBUG)
        cout << "Failed to connect: " << translate_status (cs) << endl;

      cout << translate_status (cs) << endl;
      return false;
    }

  cs = ListGlobals (conHdl, &glbsCount);
  if (level >= VL_DEBUG)
    {
      if (cs == CS_OK)
        cout << "Got " << glbsCount << " globals.\n";
      else
        cout << "Listing globals variables has failed\n";
    }

  while ((cs == CS_OK) && (glbsCount > 0))
    {
      const unsigned char* pGlbName = NULL;
      cs = GlobalFetch (conHdl, &pGlbName);

      assert (pGlbName != NULL);
      cout << pGlbName << endl;

      if ((cs != CS_OK) && (level < VL_DEBUG))
        {
          cout << "Fetching global value name has failed.\n";
        }
      --glbsCount;
    }

  Close (conHdl);

  if (cs != CS_OK)
    cout << translate_status (cs) << endl;

  return (cs == CS_OK) ? true : false;
}

static const D_CHAR pingShowDesc[]    = "Ping the database sever. ";
static const D_CHAR pingShowDescExt[] =
"Ping the database server to check if it is up.\n"
"Usage:\n"
"  ping";

static bool
cmdPing (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  CONNECTOR_HND conHdl = NULL;
  WTICKS ticks  = wh_msec_ticks ();
  CONNECTOR_STATUS cs  = Connect (GetRemoteHostName ().c_str (),
                                  GetConnectionPort ().c_str (),
                                  GetWorkingDB ().c_str (),
                                  GetUserPassword ().c_str (),
                                  GetUserId (),
                                  &conHdl);

  if (cs != CS_OK)
    goto cmd_ping_exit;

  cs = PingServer (conHdl);

cmd_ping_exit:
  Close (conHdl);
  ticks = wh_msec_ticks () - ticks;
  if (cs != CS_OK)
    {
      cout << "Server ping failed : " << translate_status (cs) << endl;
    }

  cout << "Ping time: " << ticks / 1000 << '.';
  cout.width (3); cout.fill ('0');
  cout << ticks % 1000<< "s.\n";

  return true;
}



void
AddOnlineTableCommands ()
{

  CmdEntry entry;

  entry.m_showStatus   = true;
  entry.m_pCmdText     = "global";
  entry.m_pCmdDesc     = globalShowDesc;
  entry.m_pExtHelpDesc = globalShowDescExt;
  entry.m_cmd          = cmdGlobalList;

  RegisterCommand (entry);

  entry.m_showStatus   = false;
  entry.m_pCmdText     = "ping";
  entry.m_pCmdDesc     = pingShowDesc;
  entry.m_pExtHelpDesc = pingShowDescExt;
  entry.m_cmd          = cmdPing;

  RegisterCommand (entry);
}
