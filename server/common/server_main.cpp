
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <memory.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "whisper.h"

#include "dbs/include/dbs_mgr.h"
#include "utils/include/logger.h"

#include "../common/configuration.h"
#include "../common/loader.h"
#include "../common/server.h"

using namespace std;

static bool sDbsInited         = false;
static bool sInterpreterInited = false;

static vector<DBSDescriptors> databases;

static void
clean_frameworks (Logger& log)
{
  if (sInterpreterInited  && sDbsInited)
    {
      vector<DBSDescriptors>::reverse_iterator dbsIterator;

      for (dbsIterator = databases.rbegin ();
           dbsIterator != databases.rend ();
           ++dbsIterator)
        {
          if (dbsIterator->m_Session != NULL)
              ReleaseInstance (*(dbsIterator->m_Session));

          if (dbsIterator->m_Dbs != NULL)
            DBSReleaseDatabase (*(dbsIterator->m_Dbs));

          if (dbsIterator->m_pLogger != NULL)
            {
              dbsIterator->m_pLogger->Log (LOG_INFO,
                                           "Database context ended!");
              delete dbsIterator->m_pLogger;
            }

          ostringstream logEntry;
          logEntry << "Cleaned resourses for database '";
          logEntry << dbsIterator->m_DbsName << "'.\n";
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
  auto_ptr<Logger>   glbLog (NULL);

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

  if (! wh_init_socks ())
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

      glbLog.reset (new Logger (GetAdminSettings ().m_LogFile.c_str ()));

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
            if (dbsIterator->m_DbsName == dbs.m_DbsName)
              {
                logEntry << "Duplicate entry '" << dbs.m_DbsName << "'. ";
                logEntry << "Ignoring the last configuration entry.\n";
                glbLog->Log (LOG_ERROR, logEntry.str ());
                continue;
              }
          }

        if (dbs.m_DbsName == GlobalContextDatabase ())
          databases.insert (databases.begin (), dbs);
        else
          databases.push_back (dbs);
      }

    if (databases.size () == 0)
      {
        glbLog->Log (LOG_CRITICAL, "No session were configured.");
        return -1;
      }
    else if (databases[0].m_DbsName != GlobalContextDatabase())
      {
        ostringstream noGlbMsg;
        noGlbMsg << "No entry for global section '";
        noGlbMsg << GlobalContextDatabase ();
        noGlbMsg << "' was found.";

        glbLog->Log (LOG_CRITICAL, noGlbMsg.str ());
      }

    const ServerSettings& confSettings = GetAdminSettings ();

    DBSSettings dbsSettings;
    dbsSettings.m_TableCacheBlkCount   = confSettings.m_TableCacheBlockCount;
    dbsSettings.m_TableCacheBlkSize    = confSettings.m_TableCacheBlockSize;
    dbsSettings.m_WorkDir              = confSettings.m_WorkDirectory;
    dbsSettings.m_TempDir              = confSettings.m_TempDirectory;
    dbsSettings.m_VLStoreCacheBlkCount = confSettings.m_VLBlockCount;
    dbsSettings.m_VLStoreCacheBlkSize  = confSettings.m_VLBlockSize;
    dbsSettings.m_VLValueCacheSize     = confSettings.m_TempValuesCache;

    DBSInit (dbsSettings);
    sDbsInited = true;

    InitInterpreter (databases[0].m_DbsDirectory.c_str ());
    sInterpreterInited = true;

    for (dbsIterator = databases.begin ();
         dbsIterator != databases.end ();
         ++dbsIterator)
      {
        LoadDatabase (*glbLog, *dbsIterator);
      }

    StartServer (*glbLog, databases);
  }
  catch (WException& e)
  {
    ostringstream logEntry;
    //TODO: Handle this exception with more specific err messages

    logEntry << "Unable to deal with error condition.\n";
    if (e.Description ())
      logEntry << "Description:\n\t" << e.Description () << endl;

    if (e.Message ())
      logEntry << "Message:\n\t" << e.Message () << endl;

    logEntry <<"Extra: " << e.GetExtra () << " (";
    logEntry << e.GetFile () << ':' << e.GetLine() << ").\n";

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
  wh_clean_socks ();

  return 0;
}
