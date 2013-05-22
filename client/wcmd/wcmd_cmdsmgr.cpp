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
#include <iostream>
#include <map>

#include "compiler//whisperc.h"
#include "client/whisper_connector.h"
#include "utils/tokenizer.h"

#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"

using namespace std;
using namespace whisper;

static const uint_t MAX_DECODED_STRING = 256;

static const char*
decode_basic_type (const uint16_t type)
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

string
wcmd_decode_typeinfo (unsigned int type)
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
      if (GET_BASIC_TYPE (type) == WHC_TYPE_NOTSET)
        return result;

      result += " OF ";
      result += decode_basic_type (GET_BASIC_TYPE (type));
      return result;
    }

  return decode_basic_type (GET_BASIC_TYPE (type));
}

map<string, CmdEntry> sCommands;
static const char descHelp[]    = "Display help on available commands.";
static const char descExtHelp[] = "Display the list of available commands "
                                     "or extended help.\n"
                                     "Usage:\n"
                                     "  help [command]\n"
                                     "Example:\n"
                                     "  help list";

static bool
cmdHelp (const string& cmdLine, ENTRY_CMD_CONTEXT)
{
  size_t currPosition = 0;
  string token        = CmdLineNextToken (cmdLine, currPosition);

  assert (token == "help");

  if (currPosition >= (cmdLine.length () - 1))
    {
      //This is the only token. List commands.
      map<string, CmdEntry>::iterator it = sCommands.begin ();

      while (it != sCommands.end ())
        {
          const streamsize prevWidth = cout.width (20);
          const char       prevFill  = cout.fill (' ');

          cout << left << it->first;
          cout.width (prevWidth);
          cout.fill (prevFill);
          cout << it->second.mpCmdDesc << endl;

          ++it;
        }
      return true;
    }

  //For every token in command line print the extended help.
  while (currPosition < cmdLine.length ())
    {
      token = CmdLineNextToken (cmdLine, currPosition);

      const CmdEntry *pEntry = FindCmdEntry (token.c_str ());
      if (pEntry == NULL)
        {
          cout << "Command not found: " << token << endl;
          return false;
        }
      cout << pEntry->mpExtHelpDesc << endl;
    }

  return true;
}

void
InitCmdManager ()
{
  CmdEntry entry;

  entry.mpCmdText     = "help";
  entry.mpCmdDesc     = descHelp;
  entry.mpExtHelpDesc = descExtHelp;
  entry.mcmd          = cmdHelp;
  entry.mcontext      = NULL;
  entry.mshowStatus   = false;

  RegisterCommand (entry);
}

void
RegisterCommand (const CmdEntry& entry)
{
  assert (entry.mpCmdText != NULL);
  assert (entry.mpCmdDesc != NULL);
  assert (entry.mpExtHelpDesc != NULL);

  pair<string, CmdEntry> cmdEntry (entry.mpCmdText, entry);

  sCommands.insert (cmdEntry);
}

const CmdEntry*
FindCmdEntry (const char* pCommand)
{
  const string command = pCommand;
  CmdEntry*    pEnt    = NULL;

  map<string, CmdEntry>::iterator it = sCommands.find (command);
  if (it != sCommands.end ())
    pEnt = &it->second;

  return pEnt;
}

const string
CmdLineNextToken (const string& cmdLine, size_t& ioPosition)
{
  static string delimiters = " \t";

  return NextToken (cmdLine, ioPosition, delimiters);
}

void
printException (ostream& outputStream, const Exception& e)
{
  const VERBOSE_LEVEL level = GetVerbosityLevel ();

  if (e.Type () == DBS_EXCEPTION)
    {
      if (level >= VL_DEBUG)
        outputStream << "Exception throwed from DBS framework.\n";

      if (level >= VL_ERROR)
        outputStream << e.Description () <<endl;
    }
  else if (e.Type () == FILE_EXCEPTION)
    {
      if (level >= VL_DEBUG)
        outputStream << "Filesystem IO exception throwed.\n";

      if (level >= VL_ERROR)
        {
          char errorDesc[MAX_DECODED_STRING];

          whf_err_to_str (e.Extra (), errorDesc, sizeof errorDesc);
          outputStream << errorDesc << endl;
        }
    }
  else
    {
      assert (false);
      outputStream << "Unknown exception throwed.\n";
    }

  if ((level >= VL_ERROR) && (e.Message() != NULL))
    outputStream << e.Message () << endl;

  if (level >= VL_DEBUG)
    {
      outputStream << "file: " << e.File () << endl;
      outputStream << "line: " << e.Line () << endl;
    }
}


