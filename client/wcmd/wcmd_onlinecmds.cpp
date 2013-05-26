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

#include "utils/auto_array.h"
#include "utils/le_converter.h"
#include "client/whisper_connector.h"

#include "wcmd_onlinecmds.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"
#include "wcmd_execcmd.h"

using namespace std;


const char*
wcmd_translate_status (const uint32_t cs)
{
  if (cs > WCS_OS_ERR_BASE)
    {
      /* This is safe, coz this function is not supposed to be executed
       * in a multi thread environment. a*/
      static char statusStr [64];
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

  case WCS_PROTOCOL_NOTSUPP:
    return "No suitable protocol to communicate with the server.";

  case WCS_ENCTYPE_NOTSUPP:
    return "Could not agree on supported encryption type.";

  case WCS_UNEXPECTED_FRAME:
    return "Unexpected communication frame received.";

  case WCS_INVALID_FRAME:
    return "An frame with invalid content received.";

  case WCS_COMM_OUT_OF_SYNC:
    return "Communication is out of sync.";

  case WCS_LARGE_ARGS:
    return "Size of the request arguments is big.";

  case WCS_LARGE_RESPONSE:
    return "Size of the request response is too big.";

  case WCS_CONNECTION_TIMEOUT:
    return "Connection has timeout.";

  case WCS_SERVER_BUSY:
    return "Server is busy.";

  case WCS_INCOMPLETE_CMD:
    return "Previous command has not been completed.";

  case WCS_INVALID_ARRAY_OFF:
    return "An invalid array index was used.";

  case WCS_INVALID_TEXT_OFF:
    return "An invalid text index was used.";

  case WCS_INVALID_ROW:
    return "An invalid row index was used.";

  case WCS_INVALID_FIELD:
    return "An invalid table field was used.";

  case WCS_TYPE_MISMATCH:
    return "The request cannot be completed due to unexpected value types.";

  case WCS_GENERAL_ERR:
    return "Unexpected error condition.";
  }

  assert (false);

  return "Unknown error encountered!";
}



static const char globalShowDesc[]    = "List context database's "
                                          "global variables.";
static const char globalShowDescExt[] =
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
  WH_CONNECTION       conHdl    = NULL;
  unsigned int        glbsCount = 0;
  const VERBOSE_LEVEL level     = GetVerbosityLevel ();

  assert (token == "global");

  uint32_t cs = WConnect (GetRemoteHostName ().c_str (),
                          GetConnectionPort ().c_str (),
                          GetWorkingDB ().c_str (),
                          GetUserPassword ().c_str (),
                          GetUserId (),
                          &conHdl);
  if (cs != WCS_OK)
    {
      if (level >= VL_DEBUG)
        cout << "Failed to connect: " << wcmd_translate_status (cs) << endl;

      cout << wcmd_translate_status (cs) << endl;
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

      while ((cs == WCS_OK)
              && (glbsCount-- > 0))
        {
          const char* glbName = NULL;
          cs = WListGlobalsFetch (conHdl, &glbName);

          assert (glbName != NULL);

          globals += ' ';
          globals += glbName;

          if ((cs != WCS_OK) && (level < VL_DEBUG))
            cout << "Fetching global value name has failed.\n";
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
      cs    = WGlobalType (conHdl, token.c_str (), &rawType);

      if (cs != WCS_OK)
        {
          if (level <= VL_DEBUG)
            {
              cout << "Failed to fetch type information for '"
                   << token << "' global variable.\n";
            }
          break;
        }

      cout << token << ' ';
      if (rawType & WHC_TYPE_TABLE_MASK)
        {
          assert ((rawType & WHC_TYPE_FIELD_MASK) == 0);

          uint_t fieldsCount;

          cs = WFieldsCount (conHdl, &fieldsCount);
          if (cs != WCS_OK)
            break;

          if (fieldsCount == 0)
            cout << "TABLE";

          else
            cout << "TABLE OF ";

          for (uint_t field = 0; field < fieldsCount; field++)
            {
              const char* fieldName;

              cs = WFetchField (conHdl, &fieldName, &rawType);
              if (cs != WCS_OK)
                break;

              if (field > 0)
                cout << ", ";

              cout << wcmd_decode_typeinfo (rawType);
            }
          cout << endl;
        }
      else if (rawType & WHC_TYPE_FIELD_MASK)
        {
          rawType &= ~WHC_TYPE_FIELD_MASK;

          cout << "FIELD OF ";
          cout << wcmd_decode_typeinfo (rawType) << endl;
        }
      else
        cout << wcmd_decode_typeinfo (rawType) << endl;
    }
  while ((linePos < globals.length ())
         && (cs == WCS_OK));

cmdGlobalList_exit:
  WClose (conHdl);

  if (cs != WCS_OK)
    cout << wcmd_translate_status (cs) << endl;

  return (cs == WCS_OK) ? true : false;
}



static const char procShowDesc[]    = "List context database's procedures.";
static const char procShowDescExt[] =
  "Show the procedures installed in the database context.\n"
  "If a name is provided it limits the listing to only those procedures\n"
  "Usage:\n"
  "  procedure [procedure_name] ... ";


static bool
cmdProcList (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  size_t              linePos     = 0;
  string              token       = CmdLineNextToken (cmdLine, linePos);
  string              procedures;
  WH_CONNECTION       conHdl      = NULL;
  unsigned int        procsCount  = 0;
  const VERBOSE_LEVEL level       = GetVerbosityLevel ();

  uint32_t cs = WConnect (GetRemoteHostName ().c_str (),
                          GetConnectionPort ().c_str (),
                          GetWorkingDB ().c_str (),
                          GetUserPassword ().c_str (),
                          GetUserId (),
                          &conHdl);

  assert (token == "procedure");

  if (cs != WCS_OK)
    {
      if (level >= VL_DEBUG)
        cout << "Failed to connect: ";

      cout << wcmd_translate_status (cs) << endl;

      return false;
    }

  if (linePos >= cmdLine.length ())
    {
      cs = WListProcedures (conHdl, &procsCount);
      if (level >= VL_DEBUG)
        {
          if (cs == WCS_OK)
            cout << "Got " << procsCount << " globals.\n";

          else
            cout << "Listing procedures has failed\n";
        }

      while ((cs == WCS_OK)
              && (procsCount-- > 0))
        {
          const char* procName = NULL;
          cs = WListProceduresFetch (conHdl, &procName);

          assert (procName != NULL);

          procedures += ' ';
          procedures += procName;

          if ((cs != WCS_OK) && (level < VL_DEBUG))
            cout << "Fetching procedure name has failed.\n";
        }
      linePos = 0;
    }
  else
    procedures = cmdLine;

  if (cs != WCS_OK)
    goto cmdProcList_exit;

  do
    {
      uint_t procsParameter;

      token = CmdLineNextToken (procedures, linePos);
      cs    = WProcParamsCount (conHdl, token.c_str (), &procsParameter);

      if (cs != WCS_OK)
        {
          if (level <= VL_DEBUG)
            {
              cout << "Failed to get the number of arguments for procedure '"
                   << token << "'.\n";
            }
          break;
        }

      cout << token << " (";

      uint_t param = 1; //Start with the first procdure parameter
      do
        {
          param %= procsParameter;

          if (param == 0)
            cout << ") ";

          unsigned int paramType;

          cs = WProcParamType (conHdl, token.c_str (), param, &paramType);
          if (cs != WCS_OK)
            {
              if (level <= VL_DEBUG)
                {
                  cout << "Failed to fetch type information for '"
                       << token << "' procedure.\n";
                }
              break;
            }

          if (param > 1)
            cout << ", ";

          if (paramType & WHC_TYPE_TABLE_MASK)
            {
              assert ((paramType & WHC_TYPE_FIELD_MASK) == 0);

              uint_t fieldsCount;

              cs = WProcParamFieldCount (conHdl,
                                         token.c_str (),
                                         param,
                                         &fieldsCount);
              if (cs != WCS_OK)
                break;

              if (fieldsCount == 0)
                {
                  cout << "TABLE";
                  continue;
                }

              cout << "TABLE OF [";

              for (uint_t field = 0; field < fieldsCount; field++)
                {
                  const char* fieldName;

                  cs = WProcParamField (conHdl,
                                        token.c_str (),
                                        0,
                                        field,
                                        &fieldName,
                                        &paramType);
                  if (cs != WCS_OK)
                    break;

                  if (field > 0)
                    cout << ", ";

                  cout << fieldName << " AS ";
                  cout << wcmd_decode_typeinfo (paramType);
                }
              cout << ']';
            }
          else if (paramType & WHC_TYPE_FIELD_MASK)
            {
              paramType &= ~WHC_TYPE_FIELD_MASK;

              if (paramType == WHC_TYPE_NOTSET)
                cout << "FIELD";

              else
                {
                  cout << "FIELD OF ";
                  cout << wcmd_decode_typeinfo (paramType);
                }
            }
          else
            cout << wcmd_decode_typeinfo (paramType);
        }
      while (param++ > 0);

      cout << endl;
    }
  while ((linePos < procedures.length ())
         && (cs == WCS_OK));

cmdProcList_exit:
  WClose (conHdl);

  if (cs != WCS_OK)
    cout << wcmd_translate_status (cs) << endl;

  return (cs == WCS_OK) ? true : false;
}



static const char pingShowDesc[]    = "Ping the database sever. ";
static const char pingShowDescExt[] =
  "Ping the database server to check if it is up.\n"
  "Usage:\n"
  "  ping";


static bool
cmdPing (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  WH_CONNECTION conHdl = NULL;
  WTICKS        ticks  = wh_msec_ticks ();
  uint32_t      cs     = WConnect (GetRemoteHostName ().c_str (),
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
      cout << wcmd_translate_status (cs) << endl;
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



static const char execShowDesc[]    = "Execute a procedure. ";
static const char execShowDescExt[] =
  "Execute a procedure on the remote server using the "
  "specified parameters.\n"
  "Parameters specifiers:\n"
  "  B -- denotes a value of boolean type.\n"
  "  C -- denotes a value of character type.\n"
  "  D -- denotes a value of date type.\n"
  "  H -- denotes a value of time type.\n"
  "  M -- denotes a value of high resolution time type.\n"
  "  I -- denotes a value of integer type.\n"
  "  U -- denotes a value of unsigned integer type.\n"
  "  R -- denotes a value of real type.\n"
  "  T -- denotes a value of text type.\n"
  "Usage:\n"
  "  exec proc_name\n"
  "  exec proc_name I('231') T('Text \\'example\\'')\n"
  "  exec proc_name [ { field1:C('A') field2:R('123.45') } "
                      "{ field1:C('A') field2:R('123.45') } ]\n"
  "  exec proc_name C{'A' 'B' 'C'} R{'123' '123.12' '-9999.99'}\n"
  "  exec proc_name [ { field1.C{'A' 'B' 'C'} "
                      "field2.R{'123' '123.12' '-9999.99'} } ]\n";


void
AddOnlineTableCommands ()
{

  CmdEntry entry;

  entry.mShowStatus   = false;
  entry.mName         = "global";
  entry.mDesc         = globalShowDesc;
  entry.mExtendedDesc = globalShowDescExt;
  entry.mCmd          = cmdGlobalList;

  RegisterCommand (entry);

  entry.mShowStatus   = false;
  entry.mName         = "procedure";
  entry.mDesc         = procShowDesc;
  entry.mExtendedDesc = procShowDescExt;
  entry.mCmd          = cmdProcList;

  RegisterCommand (entry);

  entry.mShowStatus   = false;
  entry.mName         = "ping";
  entry.mDesc         = pingShowDesc;
  entry.mExtendedDesc = pingShowDescExt;
  entry.mCmd          = cmdPing;

  RegisterCommand (entry);

  entry.mShowStatus   = false;
  entry.mName         = "exec";
  entry.mDesc         = execShowDesc;
  entry.mExtendedDesc = execShowDescExt;
  entry.mCmd          = cmdExec;

  RegisterCommand (entry);
}

