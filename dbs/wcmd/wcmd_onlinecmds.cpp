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
decode_glbvar_typeinfo (const D_UINT8* pTypeInfo)
{
  string result;
  bool fieldDesc = false;
  bool arrayDesc = false;

  D_UINT16       type = from_le_int16 (pTypeInfo);
  const D_UINT16 size = from_le_int16 (pTypeInfo + sizeof (D_UINT16));

  if (IS_ARRAY(type))
    {
      result += "ARRAY";
      arrayDesc = true;
    }
  else if (IS_FIELD (type))
    {
      result += "FIELD";
      fieldDesc = true;
    }
  else if (IS_TABLE (type))
    {
      result += "TABLE";
      fieldDesc = true;
    }

  if (arrayDesc)
    {
      if (GET_BASIC_TYPE (type) == T_UNDETERMINED)
        return result;

      result += " OF ";
      result += decode_basic_type (GET_BASIC_TYPE (type));
      return result;
    }

  if (fieldDesc)
    {
      assert (size >= 2);
      if (size <= 2)
        return result;

      result += " (";
      D_UINT16 progress = 0;

      pTypeInfo += 2 * sizeof (D_UINT16);
      while (progress < size -2 )
        {
          result += _RC (const D_CHAR*, pTypeInfo);

          const D_UINT fieldLen = strlen (_RC (const D_CHAR*, pTypeInfo)) + 1;
          pTypeInfo += fieldLen;
          type = from_le_int16 (pTypeInfo);

          result += " AS ";
          if (IS_ARRAY (type))
            result += " ARRAY OF ";
          result += decode_basic_type (GET_BASIC_TYPE (type));

          pTypeInfo += 2;

          progress += 2 + fieldLen;
          if (progress == (size - 2))
            result += ')';
          else
            result += ", ";
        }

      assert (pTypeInfo[0] == ';');
      assert (pTypeInfo[1] == 0);

      return result;
    }

  return decode_basic_type (GET_BASIC_TYPE (type));
}

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
  size_t              linePos   = 0;
  string              token     = CmdLineNextToken (cmdLine, linePos);
  string              globals;
  CONNECTOR_HND       conHdl    = NULL;
  unsigned int        glbsCount = 0;
  const VERBOSE_LEVEL level     = GetVerbosityLevel ();

  CONNECTOR_STATUS cs  = Connect (GetRemoteHostName ().c_str (),
                                  GetConnectionPort ().c_str (),
                                  GetWorkingDB ().c_str (),
                                  GetUserPassword ().c_str (),
                                  GetUserId (),
                                  &conHdl);

  assert (token == "global");

  if (cs != CS_OK)
    {
      if (level >= VL_DEBUG)
        cout << "Failed to connect: " << translate_status (cs) << endl;

      cout << translate_status (cs) << endl;
      return false;
    }

  if (linePos >= cmdLine.length ())
    {
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
          const char* pGlbName = NULL;
          cs = ListGlobalsFetch (conHdl, &pGlbName);

          assert (pGlbName != NULL);
          globals += ' ';
          globals += pGlbName;

          if ((cs != CS_OK) && (level < VL_DEBUG))
            {
              cout << "Fetching global value name has failed.\n";
            }
          --glbsCount;
        }
      linePos = 0;
    }
  else
    globals = cmdLine;

  do
    {
      unsigned int typeInfoSize = 0;
      token = CmdLineNextToken (globals, linePos);
      cs = DescribeGlobal (conHdl, token.c_str (), &typeInfoSize);

      if (cs != CS_OK)
        {
          if (level <= VL_DEBUG)
            {
              cout << "Failed to fetch type information for '"
                   << token << "' global variable.\n";
            }
          break;
        }
      assert (typeInfoSize >= 4);
      auto_array<D_UINT8> typeStore (typeInfoSize);
      D_UINT  progress = 0;

      while (progress < typeInfoSize)
        {
          const unsigned char *pChunk    = NULL;
          unsigned int         chunkSize = 0;

          cs = DescribeGlobalFetch (conHdl, &pChunk, &chunkSize);
          if (cs != CS_OK)
            {
              DescribeGlobalFetchCancel (conHdl);
              break;
            }

          memcpy (&typeStore[progress], pChunk, chunkSize);
          progress += chunkSize;

          assert (progress <= typeInfoSize);
        }
      if (cs != CS_OK)
        break;

      const streamsize prevWidth = cout.width (20);
      const char       prevFill  = cout.fill (' ');

      cout << left << token;
      cout.width (prevWidth);
      cout.fill (prevFill);
      cout << decode_glbvar_typeinfo (&typeStore[0]) << endl;
    }
  while (linePos < globals.length ());

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
