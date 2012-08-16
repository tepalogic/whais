
#include <errno.h>
#include <assert.h>
#include <iostream>
#include <fstream>

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

      glbLog->Log (LOG_ERROR, "Hello beauty! Waht's going on?");
      glbLog->Log (LOG_WARNING, "Nothing baby! Nothing!");
      glbLog->Log ((LOG_TYPE)12, "Long life and prosper!");
      glbLog->Log (LOG_INFO, "Who's this guy!\nAre you ok!\nWho are you!");
      glbLog->Log (LOG_DEBUG, "I'm just passing by! What!?");

  }
  catch (...)
  {
      cerr << "Unknown error encountered during main configuration reading!\n";

      return -1;
  }

  try
  {

  }
  catch (WException& e)
  {
  }
  return 0;
}

