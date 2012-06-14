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

#include "wcmd_cmdsmgr.h"

using namespace std;

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
          cout << it->first << "\t\t" << it->second.m_pCmdDesc << endl;
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
  static string spaces = " \t";

  ioPosition = cmdLine.find_first_not_of (spaces, ioPosition);

  assert (ioPosition != string::npos);
  assert (ioPosition < cmdLine.length ());

  size_t lastPos = cmdLine.find_first_of (spaces, ioPosition);

  if (lastPos == string::npos)
    lastPos = cmdLine.length () - 1;
  else
    --lastPos;

  assert (ioPosition <= lastPos);

  string result = cmdLine.substr (ioPosition, lastPos - ioPosition + 1);

  ioPosition = lastPos + 1;

  return result;
}


