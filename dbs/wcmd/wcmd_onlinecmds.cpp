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
#include <cstdio>

#include "whisper.h"

#include "compiler/include/whisperc/whisperc.h"
#include "utils/include/auto_array.h"
#include "utils/include/le_converter.h"
#include "client/include/whisper_connector.h"

#include "wcmd_onlinecmds.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"

using namespace std;

const D_CHAR*
decode_basic_type (const D_UINT16 type)
{
  switch (GET_BASIC_TYPE (type))
  {
  case T_BOOL:
    return "BOOL";
  case T_CHAR:
    return "CHARACTER";
  case T_DATE:
    return "DATE";
  case T_DATETIME:
    return "DATETIME";
  case T_HIRESTIME:
    return "HIRESTIME";
  case T_UINT8:
    return "UNSIGNED INT8";
  case T_UINT16:
    return "UNSIGNED INT16";
  case T_UINT32:
    return "UNSIGNED INT32";
  case T_UINT64:
    return "UNSIGNED INT64";
  case T_INT8:
    return "INT8";
  case T_INT16:
    return "INT16";
  case T_INT32:
    return "INT32";
  case T_INT64:
    return "INT64";
  case T_REAL:
    return "REAL";
  case T_RICHREAL:
    return "RICHREAL";
  case T_TEXT:
    return "TEXT";
  default:
    assert (false);
  }

  return NULL;
}

static string
decode_glbvar_typeinfo (unsigned int type)
{
  string result;
  bool   arrayDesc = false;

  assert ((IS_FIELD (type) || IS_TABLE (type)) == false );

  if (IS_ARRAY(type))
    {
      result += "ARRAY";
      arrayDesc = true;
    }

  if (arrayDesc)
    {
      if (GET_BASIC_TYPE (type) == T_UNDETERMINED)
        return result;

      result += " OF ";
      result += decode_basic_type (GET_BASIC_TYPE (type));
      return result;
    }

  return decode_basic_type (GET_BASIC_TYPE (type));
}

const D_CHAR*
translate_status (const D_UINT32 cs)
{
  if (cs > WCS_OS_ERR_BASE)
    {
      /* This is safe, coz this function is not supposed to be executed
       * in a multi thread environment. a*/
      static D_CHAR statusStr [64];
      sprintf (statusStr, "OS internal error: %u.", cs - WCS_OS_ERR_BASE);

      return statusStr;
    }

  switch (cs)
  {
  case WCS_OK:
    return "No error returned.";
  case WCS_INVALID_ARGS:
    return "Invalid arguments.";
  case WCS_OP_NOTSUPP:
    return "Operation not supported";
  case WCS_OP_NOTPERMITED:
    return "Operation not permitted.";
  case WCS_DROPPED:
    return "Connection dropped by peer.";
  case WCS_ENCTYPE_NOTSUPP:
    return "Could not agree on supported encryption type.";
  case WCS_UNEXPECTED_FRAME:
    return "Unexpected communication frame received.";
  case WCS_INVALID_FRAME:
    return "An frame with invalid content received.";
  case WCS_COMM_OUT_OF_SYNC:
    return "Communication is out of sync.";
  case WCS_LARGE_ARGS:
    return "Size of the operation arguments is not supported.";
  case WCS_CONNECTION_TIMEOUT:
    return "Connection has timeout.";
  case WCS_SERVER_BUSY:
    return "Server is too busy.";
  case WCS_INCOMPLETE_CMD:
    return "Previous command has not been completed";
  case WCS_GENERAL_ERR:
    return "Unexpected error condition.";
  }

  assert (FALSE);
  return "Unknown error encountered!";
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
  size_t              linePos   = 0;
  string              token     = CmdLineNextToken (cmdLine, linePos);
  string              globals;
  W_CONNECTOR_HND       conHdl    = NULL;
  unsigned int        glbsCount = 0;
  const VERBOSE_LEVEL level     = GetVerbosityLevel ();

  D_UINT32 cs  = WConnect (GetRemoteHostName ().c_str (),
                           GetConnectionPort ().c_str (),
                           GetWorkingDB ().c_str (),
                           GetUserPassword ().c_str (),
                           GetUserId (),
                           &conHdl);

  assert (token == "global");

  if (cs != WCS_OK)
    {
      if (level >= VL_DEBUG)
        cout << "Failed to connect: " << translate_status (cs) << endl;

      cout << translate_status (cs) << endl;
      return false;
    }

  if (linePos >= cmdLine.length ())
    {
      cs = WListGlobals (conHdl, &glbsCount);
      if (level >= VL_DEBUG)
        {
          if (cs == WCS_OK)
            cout << "Got " << glbsCount << " globals.\n";
          else
            cout << "Listing globals variables has failed\n";
        }

      while ((cs == WCS_OK) && (glbsCount > 0))
        {
          const char* pGlbName = NULL;
          cs = WListGlobalsFetch (conHdl, &pGlbName);

          assert (pGlbName != NULL);
          globals += ' ';
          globals += pGlbName;

          if ((cs != WCS_OK) && (level < VL_DEBUG))
            {
              cout << "Fetching global value name has failed.\n";
            }
          --glbsCount;
        }
      linePos = 0;
    }
  else
    globals = cmdLine;

  if (cs != WCS_OK)
    goto cmdGlobalList_exit;

  do
    {
      unsigned int rawType = 0;
      token = CmdLineNextToken (globals, linePos);
      cs = WDescribeValue (conHdl, token.c_str (), &rawType);

      if (cs != WCS_OK)
        {
          if (level <= VL_DEBUG)
            {
              cout << "Failed to fetch type information for '"
                   << token << "' global variable.\n";
            }
          break;
        }

      const streamsize prevWidth = cout.width (20);
      const char       prevFill  = cout.fill (' ');

      cout << left << token;
      cout.width (prevWidth);
      cout.fill (prevFill);

      if (rawType & WFT_TABLE_MASK)
        {
          assert ((rawType & WFT_FIELD_MASK) == 0);

          D_UINT fieldsCount;

          cs = WDescribeValueGetFieldsCount (conHdl, &fieldsCount);
          if (cs != WCS_OK)
            break;

          if (fieldsCount == 0)
            cout << "TABLE";
          else
            cout << "TABLE OF ";

          for (D_UINT field = 0; field < fieldsCount; field++)
            {
              const D_CHAR* fieldName;

              cs = WDescribeValueFetchField (conHdl, &fieldName, &rawType);
              if (cs != WCS_OK)
                break;

              cout << decode_glbvar_typeinfo (rawType) << endl;
            }
        }
      else if (rawType & WFT_FIELD_MASK)
        {
          rawType &= ~WFT_FIELD_MASK;

          cout << "FIELD OF";
          cout << decode_glbvar_typeinfo (rawType) << endl;
        }
      else
        cout << decode_glbvar_typeinfo (rawType) << endl;
    }
  while ((linePos < globals.length ()) && (cs == WCS_OK));

cmdGlobalList_exit:
  WClose (conHdl);

  if (cs != WCS_OK)
    cout << translate_status (cs) << endl;

  return (cs == WCS_OK) ? true : false;
}

static const D_CHAR pingShowDesc[]    = "Ping the database sever. ";
static const D_CHAR pingShowDescExt[] =
"Ping the database server to check if it is up.\n"
"Usage:\n"
"  ping";

static bool
cmdPing (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  W_CONNECTOR_HND conHdl = NULL;
  WTICKS ticks  = wh_msec_ticks ();
  D_UINT32 cs  = WConnect (GetRemoteHostName ().c_str (),
                           GetConnectionPort ().c_str (),
                           GetWorkingDB ().c_str (),
                           GetUserPassword ().c_str (),
                           GetUserId (),
                           &conHdl);
  if (cs != WCS_OK)
    goto cmd_ping_exit;

  cs = WPingServer (conHdl);

cmd_ping_exit:
  WClose (conHdl);
  ticks = wh_msec_ticks () - ticks;
  if (cs != WCS_OK)
    {
      cout << translate_status (cs) << endl;
      return false;
    }
  else
    {
      cout << "Ping time: " << ticks / 1000 << '.';
      cout.width (3); cout.fill ('0');
      cout << right << ticks % 1000<< "s.\n";
    }

  return true;
}



void
AddOnlineTableCommands ()
{

  CmdEntry entry;

  entry.m_showStatus   = false;
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
