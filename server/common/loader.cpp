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

#include "loader.h"

#include "dbs/dbs_mgr.h"
#include "interpreter/interpreter.h"
#include "compiler//compiledunit.h"
#include "utils/logger.h"

using namespace std;
using namespace whisper;

bool
LoadDatabase (FileLogger& log, DBSDescriptors& ioDbsDesc)
{
  ostringstream logEntry;


  logEntry << "Loading database: " << ioDbsDesc.m_DbsName;
  log.Log (LOG_INFO, logEntry.str ());
  logEntry.str ("");

  ioDbsDesc.m_Dbs = &DBSRetrieveDatabase (ioDbsDesc.m_DbsName.c_str (),
                                          ioDbsDesc.m_DbsDirectory.c_str ());

  std::auto_ptr<Logger> apLogger (
                         new FileLogger (ioDbsDesc.m_DbsLogFile.c_str (), true)
                                   );

  if (ioDbsDesc.m_DbsName != GlobalContextDatabase ())
    {
      ioDbsDesc.m_Session = &GetInstance (ioDbsDesc.m_DbsName.c_str (),
                                          apLogger.get ());
    }
  else
    ioDbsDesc.m_Session = &GetInstance (NULL, &log);

  for (vector<string>::iterator it = ioDbsDesc.m_ObjectLibs.begin ();
       it != ioDbsDesc.m_ObjectLibs.end ();
       ++it)
    {
      logEntry << "... Loading compiled object unit '" << *it << "'.";
      log.Log (LOG_INFO, logEntry.str ());
      logEntry.str ("");

      CompiledFileUnit unit (it->c_str ());
      ioDbsDesc.m_Session->LoadCompiledUnit (unit);
    }

  ioDbsDesc.m_pLogger = apLogger.release ();

  return true;
}
