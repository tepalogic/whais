/******************************************************************************
WHISPER - An advanced database system
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

#include <string>
#include <sstream>
#include <vector>

#include "dbs/dbs_mgr.h"
#include "interpreter/interpreter.h"
#include "compiler//compiledunit.h"
#include "utils/logger.h"

#include "loader.h"


using namespace std;
using namespace whisper;



bool
LoadDatabase (FileLogger& log, DBSDescriptors& inoutDesc)
{
  ostringstream logEntry;


  logEntry << "Loading database: " << inoutDesc.mDbsName;
  log.Log (LOG_INFO, logEntry.str ());
  logEntry.str ("");

  inoutDesc.mDbs = &DBSRetrieveDatabase (inoutDesc.mDbsName.c_str (),
                                         inoutDesc.mDbsDirectory.c_str ());
  std::auto_ptr<Logger> dbsLogger (
                         new FileLogger (inoutDesc.mDbsLogFile.c_str (), true)
                                  );

  if (inoutDesc.mDbsName != GlobalContextDatabase ())
    {
      inoutDesc.mSession = &GetInstance (inoutDesc.mDbsName.c_str (),
                                         dbsLogger.get ());
    }
  else
    inoutDesc.mSession = &GetInstance (NULL, &log);

  for (vector<string>::iterator it = inoutDesc.mNativeLibs.begin ();
       it != inoutDesc.mNativeLibs.end ();
       ++it)
    {
      logEntry << "... Loading dynamic native library '" << *it << "'.";
      log.Log (LOG_INFO, logEntry.str ());
      logEntry.str ("");

      WH_SHLIB shl = wh_shl_load (it->c_str ());
      if ((shl == INVALID_SHL)
          || ! inoutDesc.mSession->LoadSharedLib (shl))
        {
          log.Log (LOG_ERROR, "Failed to load the dynamic library.");
        }
    }

  for (vector<string>::iterator it = inoutDesc.mObjectLibs.begin ();
       it != inoutDesc.mObjectLibs.end ();
       ++it)
    {
      logEntry << "... Loading compiled object unit '" << *it << "'.";
      log.Log (LOG_INFO, logEntry.str ());
      logEntry.str ("");

      CompiledFileUnit unit (it->c_str ());
      inoutDesc.mSession->LoadCompiledUnit (unit);
    }

  inoutDesc.mLogger = dbsLogger.release ();

  return true;
}

