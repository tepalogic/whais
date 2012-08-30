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

#include "dbs/include/dbs_mgr.h"
#include "interpreter/include/interpreter.h"
#include "compiler/include/whisperc/compiledunit.h"

using namespace std;

bool
LoadDatabase (Logger& log, DBSDescriptors& ioDbsDesc)
{
  ostringstream logEntry;

  logEntry << "Loading database: " << ioDbsDesc.m_DatabaseName;
  log.Log (LOG_INFO, logEntry.str ());
  logEntry.str ("");



  ioDbsDesc.m_Dbs = &DBSRetrieveDatabase (ioDbsDesc.m_DatabaseName.c_str ());

  if (ioDbsDesc.m_DatabaseName != GlobalContextDatabase ())
    {
      ioDbsDesc.m_Session = &GetInstance (ioDbsDesc.m_DatabaseName.c_str (),
                                          &log);
    }
  else
    ioDbsDesc.m_Session = &GetInstance (NULL, &log);

  //TODO: Add support here for native libraries.

  for (vector<string>::iterator it = ioDbsDesc.m_ObjectLibs.begin ();
       it != ioDbsDesc.m_ObjectLibs.end ();
       ++it)
    {
      logEntry << "... Loading compiled object unit '" << *it << "'.";
      log.Log (LOG_INFO, logEntry.str ());
      logEntry.str ("");

      WFileCompiledUnit unit (it->c_str ());
      ioDbsDesc.m_Session->LoadCompiledUnit (unit);
    }

  return true;
}
