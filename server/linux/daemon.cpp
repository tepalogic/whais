#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <fcntl.h>
#include <stdexcept>
#include <sstream>
#include <iostream>


#include "whais.h"

#include "dbs/dbs_mgr.h"
#include "utils/logger.h"
#include "utils/license.h"

#include "../common/configuration.h"
#include "../common/loader.h"
#include "../common/server.h"

using namespace std;
using namespace whais;

static const char sProgramName[] = "Whais";
static const char sProgramDesc[] = "A database server daemon.";

static const char* sConfigFile = "/etc/whais.conf";
static const char* sPidFile    = NULL;

static bool sDbsInited         = false;
static bool sInterpreterInited = false;

static vector<DBSDescriptors> databases;

static void
clean_frameworks (FileLogger& log)
{
  if (sInterpreterInited)
    {
      assert (sDbsInited);

      vector<DBSDescriptors>::reverse_iterator dbsIterator;

      for (dbsIterator = databases.rbegin ();
           dbsIterator != databases.rend ();
           ++dbsIterator)
        {
          if (dbsIterator->mSession != NULL)
            {
              ReleaseInstance (*(dbsIterator->mSession));
              dbsIterator->mSession = NULL;
            }

          ostringstream logEntry;

          logEntry << "Closing session '";
          logEntry << dbsIterator->mDbsName << "'.\n";
          log.Log (LT_INFO, logEntry.str ());
        }
    }

  if (sInterpreterInited)
    CleanInterpreter ();

  if (sInterpreterInited)
    {
      assert (sDbsInited);

      vector<DBSDescriptors>::reverse_iterator dbsIterator;

      for (dbsIterator = databases.rbegin ();
           dbsIterator != databases.rend ();
           ++dbsIterator)
        {
          if (dbsIterator->mDbs != NULL)
            DBSReleaseDatabase (*(dbsIterator->mDbs));

          if (dbsIterator->mLogger != NULL)
            {
              dbsIterator->mLogger->Log (LT_INFO, "Database context ended!");
              delete dbsIterator->mLogger;
            }

          ostringstream logEntry;
          logEntry << "Cleaned resources of database '";
          logEntry << dbsIterator->mDbsName << "'.\n";
          log.Log (LT_INFO, logEntry.str ());
        }
    }

  if (sDbsInited)
    DBSShoutdown ();
}

static void
sigterm_hdl (int sig, siginfo_t *siginfo, void *context)
{
  if ((sig != SIGINT) && (sig != SIGTERM))
    return ; //Ignore this!

  StopServer ();
}


static bool
set_signals ()
{
  struct sigaction action;

  memset (&action, 0, sizeof action);
  action.sa_flags     = SA_SIGINFO;
  action.sa_sigaction = &sigterm_hdl;

 if (sigaction (SIGINT, &action, NULL) < 0)
   return false;

 if (sigaction (SIGTERM, &action, NULL) < 0)
   return false;

 return true;
}


static bool
boot_server ()
{
  unique_ptr<ifstream>   config;
  unique_ptr<FileLogger> glbLog;

  config.reset (new ifstream (sConfigFile,
                              ios_base::in | ios_base::binary));
  if (! config->good ())
    {
      syslog (LOG_CRIT,
              "Could not open configuration file '%s'.",
              sConfigFile);

      return false;
    }

  if (! whs_init ())
    {
      syslog (LOG_CRIT, "Could not initialize the network socket framework.");
      return false;
    }

  try
  {
      uint_t sectionLine = 0;
      if (SeekAtConfigurationSection (*config, sectionLine) == false)
        {
          syslog (LOG_CRIT,
                  "Cannot find the CONFIG section in configuration file!");
          return false;
        }

      assert (sectionLine > 0);

      ostringstream log;
      if ( ! ParseConfigurationSection (*config, sectionLine, log))
        {
          syslog (LOG_CRIT, log.str ().c_str ());

          return false;
        }

      glbLog.reset (new FileLogger (GetAdminSettings ().mLogFile.c_str ()));
  }
  catch (ios_base::failure& e)
  {
      syslog (LOG_CRIT, "Unexpected error: %s.", e.what ());

      return false;
  }
  catch (...)
  {
      syslog (LOG_CRIT,
              "Unexpected error encoutrered during main "
                "configuration file read.");
      return false;
  }

  try
  {

    if (! set_signals ())
      {
        syslog (LOG_CRIT, "Signals handlers could not be overwritten.");
        return false;
      }

    vector<DBSDescriptors>::iterator dbsIterator;

    syslog (LOG_INFO,
            "Parsing the main configuration section. "
               "Please follow the configured log files for more information.");

    if ( ! PrepareConfigurationSection (*glbLog))
      return false;

    uint_t configLine = 0;
    config->clear ();
    config->seekg (0);
    while (FindNextContextSection (*config, configLine))
      {
        DBSDescriptors dbs (configLine);

        //Inherit some global settings from the server configuration area in
        //case are not set in the context configuration section.
        dbs.mWaitReqTmo   = GetAdminSettings ().mWaitReqTmo;
        dbs.mSyncInterval = GetAdminSettings ().mSyncInterval;

        if ( ! ParseContextSection (*glbLog, *config, configLine, dbs))
          return false;

        if ( ! PrepareContextSection (*glbLog, dbs))
          return false;

        ostringstream   logEntry;

        for (dbsIterator = databases.begin ();
             dbsIterator != databases.end ();
             ++dbsIterator)
          {
            if (dbsIterator->mDbsName == dbs.mDbsName)
              {
                logEntry << "Duplicate entry '" << dbs.mDbsName << "'. ";
                logEntry << "Ignoring the last configuration entry.\n";
                glbLog->Log (LT_ERROR, logEntry.str ());
                continue;
              }
          }

        if (dbs.mDbsName == GlobalContextDatabase ())
          databases.insert (databases.begin (), dbs);

        else
          databases.push_back (dbs);
      }

    if (databases.size () == 0)
      {
        glbLog->Log (LT_CRITICAL, "No session were configured.");
        return false;
      }
    else if (databases[0].mDbsName != GlobalContextDatabase ())
      {
        ostringstream noGlbMsg;

        noGlbMsg << "No entry for global section '";
        noGlbMsg << GlobalContextDatabase ();
        noGlbMsg << "' was found.";

        glbLog->Log (LT_CRITICAL, noGlbMsg.str ());
      }

    const ServerSettings& confSettings = GetAdminSettings ();

    DBSSettings dbsSettings;
    dbsSettings.mTableCacheBlkCount   = confSettings.mTableCacheBlockCount;
    dbsSettings.mTableCacheBlkSize    = confSettings.mTableCacheBlockSize;
    dbsSettings.mWorkDir              = confSettings.mWorkDirectory;
    dbsSettings.mTempDir              = confSettings.mTempDirectory;
    dbsSettings.mVLStoreCacheBlkCount = confSettings.mVLBlockCount;
    dbsSettings.mVLStoreCacheBlkSize  = confSettings.mVLBlockSize;
    dbsSettings.mVLValueCacheSize     = confSettings.mTempValuesCache;

    DBSInit (dbsSettings);
    sDbsInited = true;

    InitInterpreter (databases[0].mDbsDirectory.c_str ());
    sInterpreterInited = true;

    for (dbsIterator = databases.begin ();
         dbsIterator != databases.end ();
         ++dbsIterator)
      {
        syslog (LOG_INFO,
                "Loading database '%s' ... ",
                dbsIterator->mDbsName.c_str ());
        LoadDatabase (*glbLog, *dbsIterator);
      }

    syslog (LOG_INFO, "All databases loaded!");
    if (sPidFile != NULL)
      {
        ofstream pidFile (sPidFile, ios_base::out | ios_base::trunc);
        pidFile << _SC (uint_t, getpid ()) << endl;
      }

    StartServer (*glbLog, databases);
  }
  catch (Exception& e)
  {
    ostringstream logEntry;

    logEntry << "Unable to deal with error condition.\n";
    if (e.Description ())
      logEntry << "Description:\n\t" << e.Description () << endl;

    if ( ! e.Message ().empty ())
      logEntry << "Message:\n\t" << e.Message () << endl;

    logEntry <<"Extra: " << e.Code () << " (";
    logEntry << e.File () << ':' << e.Line () << ").\n";

    glbLog->Log (LT_CRITICAL, logEntry.str ());

    clean_frameworks (*glbLog);
    whs_clean ();

    return false;
  }
  catch (std::bad_alloc&)
  {
    glbLog->Log (LT_CRITICAL, "OUT OF MEMORY!!!");

    clean_frameworks (*glbLog);
    whs_clean ();

    return false;
  }
  catch (std::exception& e)
  {
    ostringstream logEntry;

    logEntry << "General system failure: " << e.what () << endl;

    glbLog->Log (LT_CRITICAL, logEntry.str ());

    clean_frameworks (*glbLog);
    whs_clean ();

    return false;
  }
  catch (...)
  {
    assert (false);

    glbLog->Log (LT_CRITICAL, "Unknown exception!");

    return false;
  }

  clean_frameworks (*glbLog);
  whs_clean ();

  return true;
}


static void
print_usage ()
{
  displayBanner (cout, sProgramName, WVER_MAJ, WVER_MIN);
  cout << "Use:\n"
          "  --pidfile file   Write the pid of damon into the 'file'. \n"
          "  --conf    file   Use the 'file' rather than '/etc/whais.conf'\n"
          "                   as a configuration file.\n"
          "  --mask mask      Use the specified mask (octal 0zzz and hexa\n"
          "                   0xZZZ forms are also accepted) as the daemon\n"
          "                   process mask (default is 0).\n"
          "  --skip-fork      Skip the forking step.\n"
          "  --help, -h       Print this help.\n"
          "  -v, --version    Show the program versions.\n"
          "  -l, --license    Show information about the license.\n";
}

int
main (int argc, char** argv)
{
  uint16_t  mask        = 0;
  bool      skipFork    = false;

  for (int i = 1; i < argc; ++i)
    {
      if (strcmp (argv[i], "--pidfile") == 0)
        {
          if (++i >= argc)
            {
              cerr << "The parameter for '--pidfile' is missing.\n";
              return EINVAL;
            }
          sPidFile = argv[i];
        }
      else if (strcmp (argv[i], "--conf") == 0)
        {
          if (++i >= argc)
            {
              cerr << "The parameter for '--conf' is missing.\n";
              return EINVAL;
            }
          sConfigFile = argv[i];
        }
      else if (strcmp (argv[i], "--mask") == 0)
        {
          if (++i >= argc)
            {
              cerr << "The parameter for '--mask' is missing.\n";
              return EINVAL;
            }
          mask = atoi (argv[i]);
        }
      else if (strcmp (argv[i], "--skip-fork") == 0)
        skipFork = true;

      else if ((strcmp (argv[i], "--help") == 0)
               || (strcmp (argv[i], "-h") == 0))
        {
          print_usage ();
          return 0;
        }
      else if ((strcmp (argv[i], "--version") == 0)
               || (strcmp (argv[i], "-v") == 0))
        {
          displayBanner (cout, sProgramName, WVER_MAJ, WVER_MIN);
          return 0;
        }
      else if ((strcmp (argv[i], "--license") == 0)
               || (strcmp (argv[i], "-l") == 0))
        {
          displayLicenseInformation (cout, sProgramName, sProgramDesc);
          return 0;
        }
      else
        {
          cerr << "I don't know what to do with '" << argv[i] << "'.\n";
          return -1;
        }
    }

  if ( ! skipFork)
    {
      const pid_t forkPid = fork ();
      if (forkPid < 0)
        {
          openlog ("whaisd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
          syslog (LOG_CRIT, "Failed to fork (errno: %d).", errno);
          closelog ();

          return -1;
        }

      else if (forkPid > 0)
        return 0;

      if (setsid () < 0)
        {
          openlog ("whaisd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
          syslog (LOG_CRIT, "Failed to setsid (errno: %d).", errno);
          closelog ();

          return -1;
        }
    }

  openlog ("whaisd", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
  umask (mask);
  if ( ! skipFork)
    {
      close (STDIN_FILENO);
      close (STDOUT_FILENO);
      close (STDERR_FILENO);

      if ((open ("/dev/null", O_RDONLY) < 0)
          || (open ("/dev/null", O_RDONLY) < 0)
          || (open ("/dev/null", O_RDONLY) < 0))
        {
          syslog (LOG_CRIT,
                  "Failed to redirect the standard file descriptors to "
                    "to '/dev/null' (%d).", errno);
          closelog ();
          return -1;
        }

    }

  if ( !  boot_server ())
    {
      syslog (LOG_CRIT, "Server stopped due to error conditions.");
      closelog ();

      return -1;
    }

  syslog (LOG_INFO, "Server stopped on request.");
  closelog ();

  return 0;
}
