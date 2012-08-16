
#include <errno.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "whisper.h"

#include "utils/include/logger.h"

#include "../common/configuration.h"

using namespace std;

int
main (int argc, D_CHAR** argv)
{
  auto_ptr<ifstream> config (NULL);
  auto_ptr<Logger> glbLog (NULL);

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

      if (SeekAtGlobalSection (*config, sectionLine) == false)
        {
          cerr << "Cannot find the main section in the configuration file!\n";
          return -1;
        }
      assert (sectionLine > 0);

      if (ParseMainSection (*config, sectionLine) == false)
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
    if ( ! FixMainSection (*glbLog))
      return -1;

    D_UINT configLine = 0;
    config->seekg (0, ios::beg);
    while (FindNextSessionSection (*config, configLine))
      {
        ostringstream   logEntry;
        SessionSettings session;
        const D_UINT    sectionLine = configLine;

        if ( ! ParseSessionSection (*glbLog, *config, configLine, session))
          return -1;

        if (session.m_Name.length () == 0)
          {
            logEntry << "The session configuration started at line ";
            logEntry << sectionLine << " does not have a name!\n";
            logEntry << "Ignoring entire section!";
            glbLog->Log (LOG_ERROR, logEntry.str ());
            continue;
          }

        if (session.m_DBSDirectory.length () == 0)
          {
            logEntry << "The session configuration started at line ";
            logEntry << sectionLine << " does not have a home directory!\n";
            logEntry << "Ignoring entire section!";
            glbLog->Log (LOG_ERROR, logEntry.str ());
            continue;
          }

        FixSessionSection (*glbLog, session);
      }
  }
  catch (WException& e)
  {
    //TODO: Handle this exception with more specific err messages
    glbLog->Log (LOG_CRITICAL, "Some error from bellow was detect!");
    return -1;
  }
  catch (...)
  {
    //TODO: Handle this exception with more specific err messages
    glbLog->Log (LOG_CRITICAL, "Unknow exception");
    return -1;
  }

  return 0;
}
