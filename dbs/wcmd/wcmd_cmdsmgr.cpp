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

#include "utils/include/tokenizer.h"

#include "wcmd_cmdsmgr.h"
#include "wcmd_optglbs.h"

using namespace std;

static const D_UINT MAX_DECODED_STRING = 256;

map<string, CmdEntry> sCommands;
static const D_CHAR descHelp[]    = "Display help on available commands.";
static const D_CHAR descExtHelp[] = "Display the list of available commands "
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
          cout << it->second.m_pCmdDesc << endl;

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
      cout << pEntry->m_pExtHelpDesc << endl;
    }

  return true;
}

void
InitCmdManager ()
{
  CmdEntry entry;

  entry.m_pCmdText     = "help";
  entry.m_pCmdDesc     = descHelp;
  entry.m_pExtHelpDesc = descExtHelp;
  entry.m_cmd          = cmdHelp;
  entry.m_context      = NULL;
  entry.m_showStatus   = false;

  RegisterCommand (entry);
}

void
RegisterCommand (const CmdEntry& entry)
{
  assert (entry.m_pCmdText != NULL);
  assert (entry.m_pCmdDesc != NULL);
  assert (entry.m_pExtHelpDesc != NULL);

  pair<string, CmdEntry> cmdEntry (entry.m_pCmdText, entry);

  sCommands.insert (cmdEntry);
}

const CmdEntry*
FindCmdEntry (const D_CHAR* pCommand)
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
printException (ostream& outputStream, const WException& e)
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
          D_CHAR errorDesc[MAX_DECODED_STRING];

          whc_ferrtostrs (e.GetExtra (), errorDesc, sizeof errorDesc);
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
      outputStream << "file: " << e.GetFile () << endl;
      outputStream << "line: " << e.GetLine () << endl;
    }
}


