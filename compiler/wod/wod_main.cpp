/******************************************************************************
WHISPERC - A compiler for whisper programs
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
#include "utils/le_converter.h"

#include "../whc/wo_format.h"
#include "wod_dump.h"
#include "wod_cmdline.h"

using namespace whisper;
using namespace whisper::wod;

int
main (int argc, char **argv)
{
  int retCode = 0;

  try
  {
    CmdLineParser cmdLine (argc, argv);

    {
      File inFileObj (cmdLine.SourceFile (), WHC_FILEREAD);
      wod_dump_header (inFileObj, cmdLine.OutStream ());
    }

    CompiledFileUnit inUnit (cmdLine.SourceFile ());

    wod_dump_const_area (inUnit, cmdLine.OutStream ());
    wod_dump_globals_tables (inUnit, cmdLine.OutStream ());
    wod_dump_procs (inUnit, cmdLine.OutStream (), false);

  }
  catch (FunctionalUnitException& e)
  {
    std::cerr << e.Message () << std::endl;
    retCode = -1;
  }
  catch (FileException& e)
  {
    std::cerr << "File IO error " << e.Extra ();
    if (e.Message () != NULL)
      std::cerr << ": " << e.Message () << std::endl;
    else
      std::cerr << '.' << std::endl;
    retCode = -1;
  }
  catch (CmdLineException& e)
  {
    std::cerr << e.Message () << std::endl;
  } catch (Exception & e)
  {
    std::cerr << "error : " << e.Message () << std::endl;
    std::cerr << "file: " << e.File() << " : " << e.Line() << std::endl;
    std::cerr << "Extra: " << e.Extra() << std::endl;

    retCode = -1;
  }
  catch (std::bad_alloc &)
  {
    std::cerr << "Memory allocation failed!" << std::endl;
    retCode = -1;
  }
  catch (...)
  {
    std::cerr << "Unknown exception thrown!" << std::endl;
    retCode = -1;
  }

  return retCode;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "WOD";
#endif

