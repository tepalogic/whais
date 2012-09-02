
#include <errno.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "whisper.h"

#include "dbs/include/dbs_mgr.h"
#include "utils/include/logger.h"

#include "../common/configuration.h"
#include "../common/loader.h"


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

int
main (int argc, D_CHAR** argv)
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

  try
  {
      D_UINT   sectionLine = 0;

      if (SeekAtConfigurationSection (*config, sectionLine) == false)
        {
          cerr << "Cannot find the CONFIG section in configuration file!\n";
          return -1;
        }
      assert (sectionLine > 0);

      if (ParseConfigSection (*config, sectionLine) == false)
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
    vector<DBSDescriptors>::iterator dbsIterator;

    if ( ! PrepareConfigSection (*glbLog))
      return -1;

    D_UINT configLine = 0;
    config->seekg (0, ios::beg);
    while (FindNextDbsSection (*config, configLine))
      {
        DBSDescriptors  dbs (configLine);

        if ( ! ParseDbsSection (*glbLog, *config, configLine, dbs))
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
                logEntry << "Duplicate entry '" << dbs.m_DbsName;
                logEntry << "'. Ignoring the last configuration entry.\n";
                glbLog->Log (LOG_ERROR, logEntry.str ());
                continue;
              }
          }

        if (dbs.m_DbsName == GlobalContextDatabase ())
          databases.insert (databases.begin (), dbs);
        else
          databases.push_back (dbs);
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

    InitInterpreter ();
    sInterpreterInited = true;

    for (dbsIterator = databases.begin ();
         dbsIterator != databases.end ();
         ++dbsIterator)
      {
        LoadDatabase (*glbLog, *dbsIterator);
      }
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
  catch (std::bad_alloc& e)
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

    glbLog->Log (LOG_CRITICAL, "Unknow exception!");
    clean_frameworks (*glbLog);

    return -1;
  }

  clean_frameworks (*glbLog);

  return 0;
}
