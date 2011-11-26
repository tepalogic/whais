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
#include <assert.h>

#include "../include/whisperc/compiledunit.h"

#include "../../utils/include/wfile.h"
#include "../../utils/include/le_converter.h"
#include "../../utils/include/outstream.h"

#include "msglog.h"
#include "whc_cmdline.h"
#include "wo_format.h"

using namespace std;

D_UINT8 whc_header[WHC_TABLE_SIZE] = { 0, };

static void
fill_globals_table (WICompiledUnit & unit,
		    struct OutStream *symbols, struct OutStream *glbs_table)
{
  const D_UINT globals_count = unit.GetGlobalsCount ();

  for (D_UINT glb_it = 0; glb_it < globals_count; ++glb_it)
    {
      D_UINT32 glb_type_index = unit.GetGlobalTypeIndex (glb_it);
      D_UINT32 glb_name_index = get_size_outstream (symbols);

      if (unit.IsGlobalExternal (glb_it))
	glb_type_index |= EXTERN_MASK;

      if ((uint32_outstream (glbs_table, glb_type_index) == NULL)
	  || (uint32_outstream (glbs_table, glb_name_index) == NULL)
	  || (data_outstream (symbols,
			      _RC (const D_UINT8 *,
				   unit.RetriveGlobalName (glb_it)),
			      unit.GetGlobalNameLenght (glb_it)) == NULL)
	  || (uint8_outstream (symbols, 0) == NULL))
	throw bad_alloc ();
    }
}

static void
process_procedures_table (WICompiledUnit & unit,
			  WFile & dest_file,
			  OutStream * symbols, OutStream * procs_table)
{
  const D_UINT proc_count = unit.GetProceduresCount ();

  for (D_UINT proc_it = 0; proc_it < proc_count; ++proc_it)
    {
      const D_UINT16 nlocals = unit.GetProcLocalsCount (proc_it);
      const D_UINT16 nparams = unit.GetProcParametersCount (proc_it);
      const D_UINT32 proc_off = dest_file.Tell ();
      const D_UINT32 proc_code_size = unit.GetProcCodeAreaSize (proc_it);
      D_UINT32 proc_retype = unit.GetProcReturnTypeIndex (proc_it);

      assert (nlocals >= nparams);

      for (D_UINT local_item = 0; local_item < nlocals; ++local_item)
	{
	  D_UINT32 local_index = unit.GetProcLocalTypeIndex (proc_it,
							     local_item);
	  to_le_int32 (_RC (D_UINT8 *, &local_index));
	  dest_file.Write (_RC (D_UINT8 *, &local_index), sizeof local_index);
	}

      if (unit.IsProcExternal (proc_it))
	proc_retype |= EXTERN_MASK;
      else
	{
	  D_UINT8 n_sync_stmts = unit.GetProcSyncStatementsCount (proc_it);
	  dest_file.Write (_RC (D_UINT8 *, &n_sync_stmts),
			   sizeof n_sync_stmts);
	  dest_file.Write (unit.RetriveProcCodeArea (proc_it),
			   unit.GetProcCodeAreaSize (proc_it));
	}

      if ((uint32_outstream (procs_table, get_size_outstream (symbols)) ==
	   NULL) || (uint32_outstream (procs_table, proc_off) == NULL)
	  || (uint32_outstream (procs_table, proc_retype) == NULL)
	  || (uint16_outstream (procs_table, nlocals) == NULL)
	  || (uint16_outstream (procs_table, nparams) == NULL)
	  || (uint32_outstream (procs_table, proc_code_size) == NULL))
	throw bad_alloc ();

      if ((data_outstream (symbols,
			   _RC (const D_UINT8 *,
				unit.RetriveProcName (proc_it)),
			   unit.GetProcNameSize (proc_it)) == NULL)
	  || (uint8_outstream (symbols, 0) == NULL))
	throw bad_alloc ();
    }
}

int
main (int argc, char **argv)
{
  D_UINT buff_size = 0;
  int ret_code = 0;
  D_UINT lang_ver_maj;
  D_UINT lang_ver_min;
  auto_ptr < D_UINT8 > buffer (NULL);
  OutStream symbols;
  OutStream glbs_table;
  OutStream procs_table;

  whc_get_lang_ver (&lang_ver_maj, &lang_ver_min);
  init_outstream (&symbols, OUTSTREAM_INCREMENT_SIZE);
  init_outstream (&glbs_table, OUTSTREAM_INCREMENT_SIZE);
  init_outstream (&procs_table, OUTSTREAM_INCREMENT_SIZE);
  try
  {

    WhcCmdLineParser args (argc, argv);

    WFile file_in (args.GetSourceFile (), WHC_FILEREAD);
    buff_size = file_in.GetSize ();

    if (buff_size >= 0xFFFFFFFE)
      throw - 1;		// we can not handle files these size anyway

    buffer.reset (new D_UINT8[_SC (unsigned int, buff_size + 1)]);
    buffer.get ()[buff_size + 1] = 0;	//ensures the null terminator
    file_in.Seek (0, WHC_SEEK_CURR);
    file_in.Read (buffer.get (), _SC (unsigned int, buff_size));
    assert (file_in.Tell () == buff_size);
    file_in.Close ();

    WBufferCompiledUnit unit (buffer.get (),
			      buff_size, my_postman, buffer.get ());

    WFile file_out (args.GetOutputFile (), WHC_FILEWRITE | WHC_FILECREATE);
    file_out.SetSize (0);
    file_out.Seek (0, WHC_SEEK_BEGIN);

    //reserve space for header file
    file_out.Write (whc_header, sizeof whc_header);

    process_procedures_table (unit, file_out, &symbols, &procs_table);
    fill_globals_table (unit, &symbols, &glbs_table);

    *_RC(D_UINT32*, whc_header + WHC_GLOBS_COUNT_OFF) = get_size_outstream (&glbs_table) / WHC_GLOBAL_ENTRY_SIZE;
    assert ((get_size_outstream (&glbs_table) % WHC_GLOBAL_ENTRY_SIZE) == 0);

    *_RC (D_UINT32*, whc_header + WHC_PROCS_COUNT_OFF) = get_size_outstream (&procs_table) / WHC_PROC_ENTRY_SIZE;

    assert ((get_size_outstream (&procs_table) % WHC_PROC_ENTRY_SIZE) == 0);

    *_RC (D_UINT32 *, whc_header + WHC_TYPEINFO_START_OFF) = file_out.Tell ();
    *_RC (D_UINT32 *, whc_header + WHC_TYPEINFO_SIZE_OFF) = unit.GetTypeInformationSize ();
    file_out.Write (unit.RetriveTypeInformation (), unit.GetTypeInformationSize ());

    *_RC (D_UINT32 *, whc_header + WHC_SYMTABLE_START_OFF) = file_out.Tell ();
    *_RC (D_UINT32 *, whc_header + WHC_SYMTABLE_SIZE_OFF) = get_size_outstream (&symbols);
    file_out.Write (get_buffer_outstream (&symbols), get_size_outstream (&symbols));

    *_RC (D_UINT32 *, whc_header + WHC_CONSTAREA_START_OFF) = file_out.Tell ();
    *_RC (D_UINT32 *, whc_header + WHC_CONSTAREA_SIZE_OFF) = unit.GetConsAreaSize ();
    file_out.Write (unit.RetrieveConstArea (), unit.GetConsAreaSize ());

    file_out.Write (get_buffer_outstream (&glbs_table),
		    get_size_outstream (&glbs_table));
    file_out.Write (get_buffer_outstream (&procs_table),
		    get_size_outstream (&procs_table));

    whc_header[WHC_SIGNATURE_OFF] = WH_SIGNATURE[0];
    whc_header[WHC_SIGNATURE_OFF + 1] = WH_SIGNATURE[1];
    whc_header[WHC_FORMATMMAJ_OFF] = WH_FFVER_MAJ;
    whc_header[WHC_FORMATMIN_OFF] = WH_FFVER_MIN;
    whc_header[WHC_LANGVER_MAJ_OFF] = lang_ver_maj;
    whc_header[WHC_LANGVER_MIN_OFF] = lang_ver_min;
    to_le_int32 (whc_header + WHC_GLOBS_COUNT_OFF);
    to_le_int32 (whc_header + WHC_PROCS_COUNT_OFF);
    to_le_int32 (whc_header + WHC_TYPEINFO_START_OFF);
    to_le_int32 (whc_header + WHC_TYPEINFO_SIZE_OFF);
    to_le_int32 (whc_header + WHC_SYMTABLE_START_OFF);
    to_le_int32 (whc_header + WHC_SYMTABLE_SIZE_OFF);

    file_out.Seek (0, WHC_SEEK_BEGIN);
    file_out.Write (whc_header, sizeof whc_header);
    file_out.Sync ();

  }
  catch (WFileException & e)
  {
    std::cerr << "File IO error: " << e.GetExtra ();
    if (e.GetDescription() != NULL)
      std::cerr << ": " << e.GetDescription () << std::endl;
    else
      std::cerr << '.' << std::endl;
    ret_code = -1;
  }
  catch (WCompiledUnitException &)
  {
    ret_code = -1;
  } catch (WhcCmdLineException & e)
  {
    std::cerr << e.GetDescription () << std::endl;
  } catch (WException & e)
  {
    std::cerr << "error : " << e.GetDescription () << std::endl;
    std::cerr << "file: " << e.GetFile() << " : " << e.GetLine() << std::endl;
    std::cerr << "Extra: " << e.GetExtra() << std::endl;
    ret_code = -1;
  }
  catch (std::bad_alloc &)
  {
    std::cerr << "Memory allocation failed!" << std::endl;
    ret_code = -1;
  }
  catch (...)
  {
    std::cerr << "Unknown exception thrown!" << std::endl;
    ret_code = -1;
  }

  destroy_outstream (&symbols);
  destroy_outstream (&glbs_table);
  destroy_outstream (&procs_table);

  return ret_code;
}
