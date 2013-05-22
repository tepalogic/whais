
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "whisper.h"

#include "dbs/dbs_mgr.h"
#include "utils/logger.h"

#include "../common/configuration.h"
#include "../common/loader.h"
#include "../common/server.h"

using namespace std;
using namespace whisper;

static bool sDbsInited         = false;
static bool sInterpreterInited = false;

static vector<DBSDescriptors> databases;

static void
clean_frameworks (FileLogger& log)
{
  if (sInterpreterInited  && sDbsInited)
    {
      vector<DBSDescriptors>::reverse_iterator dbsIterator;

      for (dbsIterator = databases.rbegin ();
           dbsIterator != databases.rend ();
           ++dbsIterator)
        {
          if (dbsIterator->mSession != NULL)
              ReleaseInstance (*(dbsIterator->mSession));

          if (dbsIterator->mDbs != NULL)
            DBSReleaseDatabase (*(dbsIterator->mDbs));

          if (dbsIterator->mpLogger != NULL)
            {
              dbsIterator->mpLogger->Log (LOG_INFO,
                                           "Database context ended!");
              delete dbsIterator->mpLogger;
            }

          ostringstream logEntry;
          logEntry << "Cleaned resourses for database '";
          logEntry << dbsIterator->mDbsName << "'.\n";
          log.Log (LOG_INFO, logEntry.str ());
        }
    }

  if (sInterpreterInited)
    CleanInterpreter ();

  if (sDbsInited)
    DBSShoutdown ();
}

#ifndef ARCH_WINDOWS_VC

static void
sigterm_hdl (int sig, siginfo_t *siginfo, void *context)
{
  if (sig != SIGINT)
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

 return true;
}
#else

static BOOL WINAPI
ServerStopHandler (DWORD)
{
  StopServer ();

  return TRUE;
}

static BOOL
set_signals ()
{
 return SetConsoleCtrlHandler (ServerStopHandler, TRUE);
}
#endif


int
main (int argc, char** argv)
{
  auto_ptr<ifstream> config (NULL);
  auto_ptr<FileLogger>   glbLog (NULL);

  if (argc < 2)
    {
      cerr << "Main configuration file was not passed as argument!\n";
      return EINVAL;
    }
  else
    {
      config.reset (new ifstream (argv[1]));
      if (! config->good ())
        {
          cerr << "Could not open the configuration file ";
          cerr << '\'' << argv[1] << "'.\n";
          return EINVAL;
        }
    }

  if (! whs_init ())
    {
      cerr << "Couldn't not init the network socket framework\n";
      return ENOTSOCK;
    }

  try
  {
      uint_t sectionLine = 0;

      if (SeekAtConfigurationSection (*config, sectionLine) == false)
        {
          cerr << "Cannot find the CONFIG section in configuration file!\n";
          return -1;
        }
      assert (sectionLine > 0);

      if (ParseConfigurationSection (*config, sectionLine) == false)
        return -1;

      glbLog.reset (new FileLogger (GetAdminSettings ().mLogFile.c_str ()));

  }
  catch (...)
  {
    cerr << "Unknown error encountered during main configuration reading!\n";
    return -1;
  }

  try
  {
    if (! set_signals())
      throw std::runtime_error ("Signals handlers could not be overwritten.");

    vector<DBSDescriptors>::iterator dbsIterator;

    if ( ! PrepareConfigurationSection (*glbLog))
      return -1;

    uint_t configLine = 0;
    config->clear ();
    config->seekg (0);
    while (FindNextContextSection (*config, configLine))
      {
        DBSDescriptors  dbs (configLine);

        if ( ! ParseContextSection (*glbLog, *config, configLine, dbs))
          return -1;

        if ( ! PrepareContextSection (*glbLog, dbs))
          return -1;

        ostringstream   logEntry;

        for (dbsIterator = databases.begin ();
             dbsIterator != databases.end ();
             ++dbsIterator)
          {
            if (dbsIterator->mDbsName == dbs.mDbsName)
              {
                logEntry << "Duplicate entry '" << dbs.mDbsName << "'. ";
                logEntry << "Ignoring the last configuration entry.\n";
                glbLog->Log (LOG_ERROR, logEntry.str ());
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
        glbLog->Log (LOG_CRITICAL, "No session were configured.");
        return -1;
      }
    else if (databases[0].mDbsName != GlobalContextDatabase())
      {
        ostringstream noGlbMsg;
        noGlbMsg << "No entry for global section '";
        noGlbMsg << GlobalContextDatabase ();
        noGlbMsg << "' was found.";

        glbLog->Log (LOG_CRITICAL, noGlbMsg.str ());
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
        LoadDatabase (*glbLog, *dbsIterator);
      }

    StartServer (*glbLog, databases);
  }
  catch (Exception& e)
  {
    ostringstream logEntry;
    //TODO: Handle this exception with more specific err messages

    logEntry << "Unable to deal with error condition.\n";
    if (e.Description ())
      logEntry << "Description:\n\t" << e.Description () << endl;

    if (e.Message ())
      logEntry << "Message:\n\t" << e.Message () << endl;

    logEntry <<"Extra: " << e.Extra () << " (";
    logEntry << e.File () << ':' << e.Line() << ").\n";

    glbLog->Log (LOG_CRITICAL, logEntry.str ());

    clean_frameworks (*glbLog);
    return -1;

  }
  catch (std::bad_alloc&)
  {
    glbLog->Log (LOG_CRITICAL, "OUT OF MEMORY!!!");
    clean_frameworks (*glbLog);

    return -1;
  }
  catch (std::exception& e)
  {
    ostringstream logEntry;

    logEntry << "General system failure: " << e.what() << endl;

    glbLog->Log (LOG_CRITICAL, logEntry.str ());
    clean_frameworks (*glbLog);

    return -1;
  }
  catch (...)
  {
    assert (false);

    glbLog->Log (LOG_CRITICAL, "Unknown exception!");
    clean_frameworks (*glbLog);

    return -1;
  }

  clean_frameworks (*glbLog);
  whs_clean ();

  return 0;
}
