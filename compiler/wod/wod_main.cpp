/******************************************************************************
WHAISC - A compiler for whais programs
Copyright (C) 2009  Iulian Popa

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

#include <iostream>

#include "compiler/compiledunit.h"
#include "utils/wfile.h"
#include "utils/endianness.h"

#include "../whc/wo_format.h"
#include "wod_dump.h"
#include "wod_cmdline.h"

using namespace std;
using namespace whais;
using namespace whais::wod;

int
main (int argc, char **argv)
{
  int retCode = 0;

  try
  {
    CmdLineParser cmdLine (argc, argv);

    {
      File inFileObj (cmdLine.SourceFile (), WH_FILEREAD);
      wod_dump_header (inFileObj, cmdLine.OutStream ());
    }

    CompiledFileUnit inUnit (cmdLine.SourceFile ());

    wod_dump_const_area (inUnit, cmdLine.OutStream ());
    wod_dump_globals_tables (inUnit, cmdLine.OutStream ());
    wod_dump_procs (inUnit, cmdLine.OutStream (), false);

  }
  catch (FunctionalUnitException& e)
  {
    cerr << e.Message () << endl;
    retCode = -1;
  }
  catch (FileException& e)
  {
    cerr << "File IO error " << e.Code ();

    if ( ! e.Message ().empty ())
      cerr << ": " << e.Message () << endl;

    else
      cerr << '.' << endl;

    retCode = -1;
  }
  catch (CmdLineException& e)
  {
    cerr << e.Message () << endl;
  }
  catch (Exception & e)
  {
    cerr << "error : " << e.Message () << endl;
    cerr << "file: " << e.File () << " : " << e.Line () << endl;
    cerr << "Extra: " << e.Code () << endl;

    retCode = -1;
  }
  catch (bad_alloc &)
  {
    cerr << "Memory allocation failed!" << endl;

    retCode = -1;
  }
  catch (...)
  {
    cerr << "Unknown exception thrown!" << endl;
    retCode = -1;
  }

  return retCode;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "WOD";
#endif

