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

#include "wod_dump.h"
#include "wod_cmdline.h"


#include "../include/whisperc/compiledunit.h"

#include "../../utils/include/wfile.h"
#include "../whc/wo_format.h"
#include "../../utils/include/le_converter.h"

int
main (int argc, char **argv)
{
  D_INT ret_code = 0;

  try
  {
    WodCmdLineParser CmdLine (argc, argv);

    {
      WFile FileObj (CmdLine.GetSourceFile (), WHC_FILEREAD);
      wod_dump_header (FileObj, CmdLine.GetOutStream ());
    }

    WFileCompiledUnit object (CmdLine.GetSourceFile ());

    wod_dump_const_area (object, CmdLine.GetOutStream ());
    wod_dump_globals_tables (object, CmdLine.GetOutStream ());
    wod_dump_procs (object, CmdLine.GetOutStream (), false);

  } catch (WCompiledUnitException & e)
  {
    std::cerr << e.GetDescription () << std::endl;
    ret_code = -1;
  } catch (WFileException & e)
  {
    std::cerr << "File IO error " << e.GetExtra ();
    if (e.GetDescription() != NULL)
      std::cerr << ": " << e.GetDescription () << std::endl;
    else
      std::cerr << '.' << std::endl;
    ret_code = -1;
  } catch (WodCmdLineException & e)
  {
    std::cerr << e.GetDescription () << std::endl;
  } catch (WException & e)
  {
    std::cerr << "error : " << e.GetDescription () << std::endl;
    std::cerr << "file: " << e.GetFile() << " : " << e.GetLine() << std::endl;
    std::cerr << "Extra: " << e.GetExtra() << std::endl;
    ret_code = -1;
  } catch (std::bad_alloc &)
  {
    std::cerr << "Memory allocation failed!" << std::endl;
    ret_code = -1;
  } catch (...)
  {
    std::cerr << "Unknown exception thrown!" << std::endl;
    ret_code = -1;
  }

  return ret_code;
}
