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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>

#include "dbs/dbs_mgr.h"

#include "wcmd_optglbs.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_tabcomds.h"
#include "wcmd_onlinecmds.h"

using namespace std;
using namespace whisper;

static const char usageDescription[] =
"\n"
"Usage: wcmd --create db_name [OPTIONS]\n"
"   or: wcmd --remove db_name [OPTIONS]\n"
"   or: wcmd --use db_name [OPTIONS]\n"
"   or: wcmd --use db_name [OPTIONS] [-s 'script']\n"
"   or: wcmd --help\n"
"   or: wcmd --license\n"
"An whisper database directory consists of multiples files. This is used to\n"
"manage the content of a database, performing operations that ranges from\n"
"database creation to updating different fields of selected tables. If there\n"
"is no execution script provided at a command line, then it executes\n"
"interactively by reading the command lines from the standard input. For\n"
"more information about the script command lines type 'help' in interactive\n"
"mode.\n"
"\n"
"Options:\n"
"    -h, --help             Prints this text.\n"
"\n"
"    -c, --create db_name   Create a database named 'db_name'.\n"
"    -r, --remove db_name   Remove the database named 'db_name'.\n"
"    -u, --use db_name      Select 'db_name' as the target database.\n"
"\n"
"    -H,  --host hostname   The host name for remote database access.\n"
"    -P,  --port portnum    The port number where the to connect.\n"
"    -U,  --user userId     The user id for connection. 0 to connect as\n"
"                           admin. Anything else to connect as regular user.\n"
"    -p,  --pass password   The password used to authenticate.\n"
"\n"
"    -d, --dir directory    Sets the working directory.\n"
"    -o, --dbdir directory  Sets the directory where the DB content resides.\n"
"    -m, --file_size size   Specify the maximum file size. Not all file\n"
"                           support the same maximum file systems, and this\n"
"                           is used to specify the respective limit. It also\n"
"                           the following size modifiers:\n"
"                            k,K : for Kilobytes.\n"
"                            m,M : for Megabytes.\n"
"                            g,G : for Gigabytes.\n"
"                           Default value for this is 2G (e.g. 2 Gigabytes),\n"
"                           and the minimum is 1M (e.g. one Megabyte).\n"
"    -v, --verbose level    Set the verbosity level. Level values:\n"
"                            0: No out put.\n"
"                            1: Print the status of the executed command.\n"
"                            2: Print the error messages in addition.\n"
"                            3: Print the warning messages in addition.\n"
"                            4: Print the various information in addition.\n"
"                            5: Print the debug information in addition.\n"
"                           Default values is 2.\n"
"    -s, --command script   Execute the script supplied at the command line,\n"
"                           rather to execute interactively.\n"
"\n"
"    -l, --license          Prints license terms.\n"
"\n"
"Examples:\n"
"    wcmd --create some_database -m 4G\n"
" or:\n"
"    wcmd --use some_database -d /database/path -s 'list tables'\n"
"    wcmd -u remote_database -H localhost -P 1761 -U 0 -p 'Some password'\n"
"\n";


static const char descExit[]    = "Terminate execution.";
static const char descExtExit[] = "Terminate execution.\n"
                                    "Usage:\n"
                                    "  quit";

static bool sFinishInteraction = false;

static void
PrintHelpUsage ()
{
  cout << usageDescription;
}

static void
PrintWrongUsage ()
{
  std::cout << " wcmd: invalid arguments. Use --help!" << std::endl;
}

static bool
ExecuteCommandLine (const std::string& cmdLine)
{
  assert (cmdLine.length () > 0);

  static string spaces = " \t";
  size_t firstPos = cmdLine.find_first_not_of (spaces);
  size_t lastPos  = cmdLine.find_last_not_of (spaces);

  if (lastPos == 0)
    lastPos = cmdLine.length () - 1;

  if (firstPos == lastPos)
    return true;

  string normalizeCmd = cmdLine.substr (firstPos, lastPos - firstPos + 1);

  assert (normalizeCmd != "");

  size_t          pos     = 0;
  const string    command = CmdLineNextToken (normalizeCmd, pos);
  const CmdEntry* pEntry  = FindCmdEntry (command.c_str ());

  if (pEntry == NULL)
    {
      cout << "Invalid command '" << command << "'." <<  endl;
      return false;
    }

  bool cmdResult = pEntry->mcmd (normalizeCmd, pEntry->mcontext);

  if (pEntry->mshowStatus && (GetVerbosityLevel () > VL_STATUS))
    cout << command << " : " << (cmdResult ? "OK" : "FAIL") << endl;

  return cmdResult;
}

static bool
ExecuteCommandBatch (const std::string& line)
{
  size_t lastSemicolonPos = 0;
  while (lastSemicolonPos < line.length ())
    {
      size_t nextSemicolonPos = line.find_first_of (';', lastSemicolonPos);

      if (nextSemicolonPos == line.npos)
        nextSemicolonPos = line.length ();

      string command = line.substr (lastSemicolonPos,
                                    nextSemicolonPos - lastSemicolonPos);

      if ( (command != "" ) && (! ExecuteCommandLine (command)))
        break;

      lastSemicolonPos = nextSemicolonPos + 1;
    }

  return true;
}

static bool
cmdExit (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  bool* pFinishInteraction = _RC(bool*, context);

  *pFinishInteraction = true;
  return true;
}

void
ExecuteInteractively ()
{
  CmdEntry entry;

  entry.mpCmdText     = "quit";
  entry.mpCmdDesc     = descExit;
  entry.mpExtHelpDesc = descExtExit;
  entry.mcmd          = cmdExit;
  entry.mcontext      = &sFinishInteraction;
  entry.mshowStatus   = false;

  RegisterCommand (entry);

  string line;
  while (! sFinishInteraction)
    {
      cout << "> ";
      if (getline (cin, line))
        {
          if (!ExecuteCommandBatch (line))
            break;
        }
      else
        break;
    }
}

static void
InitDBS ()
{

  if (IsDatabaseRemote ())
    {
      if (GetVerbosityLevel () >= VL_DEBUG)
        {
          cout << "Connecting to remote database as  ";
          cout << ((GetUserId () == 0) ? "admin" : "regular user") << ":\n";
          cout << " remote host: " << GetRemoteHostName () << endl;
          cout << " port:" << GetConnectionPort () << endl;
          cout << " database: " << GetWorkingDB () << endl;
          cout << " user id: " << GetUserId () << endl;
          cout << " password: " << GetUserPassword () << endl;
        }
    }
  else
    {
      const string&  workDir     = GetWorkingDirectory ();
      const uint64_t maxFileSize = GetMaximumFileSize ();

      if (GetVerbosityLevel () >= VL_DEBUG)
        {
          cout << "Starting the DBS framework: " << endl;
          cout << " directory: " << workDir << endl;
          cout << " file_size: " << maxFileSize << endl;
        }

      DBSSettings settings;
      settings.mWorkDir = settings.mTempDir = workDir;
      DBSInit (settings);
    }

  if (GetVerbosityLevel () >= VL_DEBUG)
    cout << "done." << endl;
}

static void
StopDBS ()
{
  if (IsDatabaseRemote ())
    return;

  DBSShoutdown ();

  if (GetVerbosityLevel () >= VL_DEBUG)
    cout << "Stopping the DBS framework." << endl;
}



static void
CreateDB (const string& dbDirectory)
{
  const VERBOSE_LEVEL level  = GetVerbosityLevel ();
  const string&       workDB = GetWorkingDB ();

  if (level >= VL_INFO)
    cout << "Creating database: " << workDB << " ... ";

  DBSCreateDatabase (workDB.c_str ());

  if (level >= VL_INFO)
    cout << "done." << endl;
}

static void
OpenDB ()
{
  const VERBOSE_LEVEL level  = GetVerbosityLevel ();
  const string&       workDB = GetWorkingDB ();

  if (level >= VL_INFO)
    cout << "Opening database: " << workDB << " ... ";

  I_DBSHandler& dbsHnd = DBSRetrieveDatabase (workDB.c_str ());
  SetDbsHandler (dbsHnd);

  if (level >= VL_INFO)
    cout << "done." << endl;
}

static void
RemoveDB ()
{
  const VERBOSE_LEVEL level  = GetVerbosityLevel ();
  const string&       workDB = GetWorkingDB ();

  if (level >= VL_INFO)
    cout << "Removing database: " << workDB << " ... ";

  DBSRemoveDatabase (workDB.c_str ());

  if (level >= VL_INFO)
    cout << "done." << endl;
}

int
main (const int argc, char *argv[])
{
  int         result     = 0;
  int         currentArg = 1;
  bool          createDB   = false;
  bool          removeDB   = false;
  string        script;
  string        dbDirectory;


  if (! whs_init ())
    {
      cerr << "Couldn't not init the network socket framework\n";
      return ENOTSOCK;
    }

  if (argc == currentArg)
    {
      PrintWrongUsage ();
      return EINVAL;
    }

  InitCmdManager ();

  while (currentArg < argc)
    {
      if ((strcmp (argv[currentArg], "--help") == 0) ||
          (strcmp (argv[currentArg], "-h") == 0))
        {
          PrintHelpUsage ();
          return 0;
        }
      else if ((strcmp (argv[currentArg], "-c") == 0) ||
               (strcmp (argv[currentArg], "--create" ) == 0))
        {
          ++currentArg;
          if ((currentArg == argc) || (GetWorkingDB ().length () != 0))
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetWorkingDB (argv[currentArg++]);
          createDB = true;
        }
      else if ((strcmp (argv[currentArg], "-r") == 0) ||
               (strcmp (argv[currentArg], "--remove" ) == 0))
        {
          ++currentArg;
          if ((currentArg == argc) || (GetWorkingDB ().length () != 0))
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetWorkingDB (argv[currentArg++]);
          removeDB = true;
        }
      else if ((strcmp (argv[currentArg], "-u") == 0) ||
               (strcmp (argv[currentArg], "--use" ) == 0))
        {
          ++currentArg;
          if ((currentArg == argc) || (GetWorkingDB ().length () != 0))
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetWorkingDB (argv[currentArg++]);
        }
      else if ((strcmp (argv[currentArg], "-H") == 0) ||
               (strcmp (argv[currentArg], "--host" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetRemoteHostName (argv[currentArg++]);
        }
      else if ((strcmp (argv[currentArg], "-P") == 0) ||
               (strcmp (argv[currentArg], "--port" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetConnectionPort (argv[currentArg++]);
        }
      else if ((strcmp (argv[currentArg], "-U") == 0) ||
               (strcmp (argv[currentArg], "--user" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetUserId (atoi (argv[currentArg++]));
        }
      else if ((strcmp (argv[currentArg], "-p") == 0) ||
               (strcmp (argv[currentArg], "--pass" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetUserPassword (argv[currentArg++]);
        }
      else if ((strcmp (argv[currentArg], "-d") == 0) ||
               (strcmp (argv[currentArg], "--dir" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetWorkingDirectory (argv[currentArg++]);
        }
      else if ((strcmp (argv[currentArg], "-o") == 0) ||
               (strcmp (argv[currentArg], "--dbdir" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }

          dbDirectory = argv[currentArg++];
        }
      else if ((strcmp (argv[currentArg], "-v") == 0) ||
               (strcmp (argv[currentArg], "--verbose" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          SetVerbosityLevel (atoi (argv[currentArg++]));
        }
      else if ((strcmp (argv[currentArg], "-s") == 0) ||
               (strcmp (argv[currentArg], "--command" ) == 0))
        {
          ++currentArg;
          if (currentArg == argc)
            {
              PrintWrongUsage ();
              return EINVAL;
            }
          script = argv [currentArg++];
        }
      else if ((strcmp (argv[currentArg], "-m") == 0) ||
               (strcmp (argv[currentArg], "--file_size" ) == 0))
        {
          ++currentArg;
          if ((currentArg == argc) ||
              ( ! SetMaximumFileSize (argv[currentArg++])))
            {
              PrintWrongUsage ();
              return EINVAL;
            }
        }
      else
        {
          PrintWrongUsage ();
          return EINVAL;
        }
    }

  try
  {
    InitDBS ();
  }
  catch (const Exception& e)
  {
    printException (cout, e);
    return e.Extra ();
  }
  catch (...)
  {
    cout << "Fatal error ... Unknown exception was thrown.\n";
    return 0xFF;
  }

  try
  {
    if (removeDB == false)
      {
        if (createDB)
          {
            CreateDB ((dbDirectory.length () != 0) ?
                      dbDirectory :
                      GetWorkingDirectory ());
          }

        if (IsDatabaseRemote ())
          {
            if (GetUserPassword ().size () == 0)
              {
                string password;
                cout << "Password [" << GetUserId () << "]: ";
                getline (cin, password);
                SetUserPassword (password.c_str ());
              }
            AddOnlineTableCommands ();
          }
        else
          {
            OpenDB ();
            AddOfflineTableCommands ();
          }

        if (script != "")
          ExecuteCommandBatch (script);
        else
          ExecuteInteractively ();
      }
    else
      RemoveDB ();
  }
  catch (const Exception& e)
  {
    printException (cout, e);

    result = e.Extra ();
  }
  catch (...)
  {
    cout << "Fatal error ... Unknown exception was thrown.\n";
    result = 0xFF;
  }

  try
  {
    StopDBS ();
  }
  catch (const Exception& e)
  {
    printException (cout, e);
    result = (result != 0) ? result : e.Extra ();
  }
  catch (...)
  {
    cout << "Fatal error ... Unknown exception was thrown.\n";
    result = (result != 0)  ? result : 0xFF;
  }

  whs_clean ();

  return result;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "WCMD";
#endif
