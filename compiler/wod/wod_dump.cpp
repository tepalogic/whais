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



#include <assert.h>
#include <iomanip>
#include <string.h>


#include "../whc/wo_format.h"
#include "../include/whisperc/whisperc.h"
#include "../../utils/include/le_converter.h"

#include "wod_dump.h"

void
wod_dump_header (WFile & wobj, std::ostream & outs)
{
  D_UINT32 temp32;
  D_UINT8 woheader[WHC_TABLE_SIZE];

  wobj.Seek (0, WHC_SEEK_BEGIN);
  wobj.Read (woheader, sizeof woheader);

  if ((woheader[0] != 'W') || (woheader[1] != 'O'))
    throw WCompiledUnitException ("Not an whisper object file!", _EXTRA(0));

  outs << std::setbase (16);
  outs.flags (std::ios::hex | std::ios::uppercase);
  outs << "File Signature:\t\t\t\t0x" << woheader[0] << " 0x" << woheader[1] <<
      " ('" << woheader[0] << "', '" << woheader[1] << "')" << std::endl;
  outs.flags (std::ios::dec);
  outs << "Format major:\t\t\t\t" << _SC (D_UINT, woheader[WHC_FORMATMMAJ_OFF]) << std::endl;
  outs << "Format minor:\t\t\t\t" << _SC (D_UINT, woheader[WHC_FORMATMIN_OFF]) << std::endl;
  outs << "Language major:\t\t\t\t" << _SC (D_UINT, woheader[WHC_LANGVER_MAJ_OFF]) << std::endl;
  outs << "Language minor:\t\t\t\t" << _SC (D_UINT, woheader[WHC_LANGVER_MIN_OFF]) << std::endl;

  temp32 = from_le_int32 (woheader + WHC_GLOBS_COUNT_OFF);
  outs << "Globals count:\t\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_PROCS_COUNT_OFF);
  outs << "Procedures count:\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_TYPEINFO_START_OFF);
  outs << "Type info start position:\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_TYPEINFO_SIZE_OFF);
  outs << "Type info size:\t\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_SYMTABLE_START_OFF);
  outs << "Symbols start position:\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_SYMTABLE_SIZE_OFF);
  outs << "Symbols size:\t\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_CONSTAREA_START_OFF);
  outs << "Constant text area position:\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_CONSTAREA_SIZE_OFF);
  outs << "Constant text area size:\t\t\t" << temp32 << std::endl;
}

void
wod_dump_const_area (WICompiledUnit & unit, std::ostream & outs)
{
  outs << std::endl << std::endl <<
    "************************************************************************"
    << std::endl << "THE CONSTANT AREA DUMP" << std::endl <<
    "************************************************************************"
    << std::endl;

  D_UINT const_pos = 0;
  do
    {
      const D_UINT row_size = 16;

      outs.flags (std::ios::hex | std::ios::uppercase);
      outs << std::endl << std::setw (sizeof (D_UINT32) * 2) <<
           std::setfill ('0') << const_pos << ":\t";

      for (D_UINT row_pos = 0; row_pos < row_size; row_pos++)
	{
	  if (row_pos && (row_pos % sizeof (D_UINT32) == 0))
	    outs << " ";

	  if (row_pos + const_pos >= unit.GetConstAreaSize ())
	    outs << "  ";
	  else
	    {
	      outs << std::setw (sizeof (D_UINT8) * 2) << std::setfill ('0') <<
	           _SC(D_UINT, unit.RetrieveConstArea ()[const_pos + row_pos]);
	    }
	}
      outs << '\t';
      for (D_UINT row_pos = 0; row_pos < row_size; row_pos++)
	{
	  if (row_pos + const_pos >= unit.GetConstAreaSize ())
	    break;
	  else
	    {
	      if (row_pos && (row_pos % sizeof (D_UINT32) == 0))
		outs << " ";

	      D_UINT char_code = unit.RetrieveConstArea ()[const_pos + row_pos];
	      if (isalnum (char_code) || ispunct (char_code))
		outs << _SC (D_CHAR, char_code);
	      else
		outs << ".";
	    }
	}
      const_pos += row_size;
    }
  while (const_pos < unit.GetConstAreaSize ());

  outs << std::setw (1);
  outs.flags (std::ios::dec);
}

static void
wod_dump_basic_type_info (std::ostream & outs, D_UINT16 type)
{
  assert ((type & (T_CONTAINER_MASK | T_FIELD_MASK)) == 0);

  if (type & T_ARRAY_MASK)
    {
      outs << "ARRAY";
      type &= ~T_ARRAY_MASK;

      if (type != T_UNDETERMINED)
	outs << " OF ";
      else
	return;
    }

  assert (type > T_UNKNOWN);
  assert (type < T_END_OF_TYPES);

  switch (type)
    {
    case T_BOOL:
      outs << "BOOL";
      break;
    case T_CHAR:
      outs << "CHARACTER";
      break;
    case T_DATE:
      outs << "DATE";
      break;
    case T_DATETIME:
      outs << "DATETIME";
      break;
    case T_HIRESTIME:
      outs << "HIRESTME";
      break;
    case T_INT8:
      outs << "INT8";
      break;
    case T_INT16:
      outs << "INT16";
      break;
    case T_INT32:
      outs << "INT32";
      break;
    case T_INT64:
      outs << "INT64";
      break;
    case T_REAL:
      outs << "REAL";
      break;
    case T_RICHREAL:
      outs << "RICHREAL";
      break;
    case T_TEXT:
      outs << "TEXT";
      break;
    case T_UINT8:
      outs << "UNSIGNED INT8";
      break;
    case T_UINT16:
      outs << "UNSIGNED INT16";
      break;
    case T_UINT32:
      outs << "UNSIGNED INT32";
      break;
    case T_UINT64:
      outs << "UNSIGNED INT64";
      break;
    default:
      assert (0);
    }
}

static void
wod_dump_rectable_type_inf (const D_UINT8 * buffer, std::ostream & outs)
{
  const D_UINT16 type = from_le_int16 (buffer);
  buffer += sizeof (D_UINT16);

  const D_UINT16 type_size = from_le_int16 (buffer);
  buffer += sizeof (D_UINT16);

  if (type == T_TABLE_MASK)
    outs << "TABLE";
  else
    {
      assert (type == T_RECORD_MASK);
      outs << "RECORD";
    }

  if (type_size > 2)
    {
      D_BOOL print_comma = FALSE;
      outs << " WITH ( ";
      while (buffer[0] != ';' && buffer[1] != 0)
	{
	  if (print_comma)
	    outs << ", ";
	  else
	    print_comma = TRUE;

	  outs << buffer << " AS ";
	  buffer += strlen (_RC (const char *, buffer)) + 1;

	  D_UINT16 type = from_le_int16 (buffer);
	  buffer += sizeof (D_UINT16);
	  wod_dump_basic_type_info (outs, type);
	}
      outs << " )";
    }

}

static void
wod_dump_row_type_info (const D_UINT8 * buffer, std::ostream & outs)
{
  const D_UINT16 type = from_le_int16 (buffer);
  buffer += sizeof (D_UINT16);

  const D_UINT16 type_size = from_le_int16 (buffer);
  buffer += sizeof (D_UINT16);

  if (type_size > 2)
    {
      const D_UINT32 GLOBAL_DECLARED = 0x80000000;
      D_UINT32       var_id          = from_le_int32 (buffer);

      if ((var_id & GLOBAL_DECLARED) != 0)
	outs << "ROW OF GLOBAL VAR ID. " << (var_id & ~GLOBAL_DECLARED);
      else
	outs << "ROW OF LOCAL VAR ID. " << (var_id & ~GLOBAL_DECLARED);
    }
  else
    outs << "ROW";

  assert ((type & T_ROW_MASK) != 0);
}

static void
wod_dump_type_info (const D_UINT8 * buffer, std::ostream & outs)
{
  const D_UINT16 type      = from_le_int16 (buffer);
  const D_UINT16 type_size = from_le_int16 (buffer + sizeof (D_UINT16));

  if ((type_size < 2)
      || (buffer[type_size + sizeof (D_UINT16)] != ';')
      || (buffer[type_size + sizeof (D_UINT16) + 1] != 0x0))
    throw WDumpException ("Object file is corrupt!", _EXTRA(0));

  if ((type & T_CONTAINER_MASK) == 0)
    return wod_dump_basic_type_info (outs, type);
  else if ((type == T_TABLE_MASK) || (type == T_RECORD_MASK))
    return wod_dump_rectable_type_inf (buffer, outs);
  else
    {
      assert (type == T_ROW_MASK);
      return wod_dump_row_type_info (buffer, outs);
    }
}

void
wod_dump_globals_tables (WICompiledUnit & unit, std::ostream & outs)
{
  outs << std::endl << std::endl <<
    "************************************************************************"
    << std::endl <<
    "THE TABLE WITH THE REFERENCES TO THE GLOBALS VALUES" <<
    std::endl <<
    "************************************************************************"
    << std::endl << std::endl;

  const D_INT nglobals = unit.GetGlobalsCount ();
  if (nglobals == 0)
    {
      outs << "No globals entries." << std::endl;
      return;
    }
  outs << "Id.\tName\t\t\tVisible\tType" << std::endl <<
    "************************************************************************" << std::endl;

  for (D_INT glb_it = 0; glb_it < nglobals; ++glb_it)
    {
      outs << glb_it << "\t";
      outs << unit.RetriveGlobalName (glb_it);
      outs << "\t\t\t";

      if (unit.IsGlobalExternal (glb_it))
	outs << "EXT" << "\t";
      else
	outs << "DEF" << "\t";

      wod_dump_type_info (unit.RetriveTypeInformation ()
			  + unit.GetGlobalTypeIndex (glb_it), outs);
      outs << std::endl;
    }
  outs << std::endl;
}

static void
wod_dump_code (const D_UINT8 * code,
	       const D_UINT code_size,
	       std::ostream & outs, const D_CHAR * prefix)
{
  D_UINT curr_pos = 0;

  while (curr_pos < code_size)
    {
      if (prefix != NULL)
	{
	  outs << prefix << "+";
	  outs << std::setw (4) << std::setfill ('0') << curr_pos;
	  outs << "\t";
	}

      D_CHAR operand1[24], operand2[24];
      enum W_OPCODE opcode = W_NA;
      D_UINT instr_size = whc_decode_opcode (code, &opcode);
      instr_size += wod_decode_table[opcode] (code + instr_size,
					      operand1, operand2);

      outs << wod_str_table[opcode] << "\t" << operand1;
      if (operand2[0] != 0)
	outs << ", " << operand2;
      outs << std::endl;

      curr_pos += instr_size;
      code += instr_size;
    }
  assert (curr_pos == code_size);
}

void
wod_dump_procs (WICompiledUnit & unit, std::ostream & outs, D_BOOL show_code)
{
  const D_UINT procs_count = unit.GetProceduresCount ();

  for (D_UINT proc_it = 0; proc_it < procs_count; ++proc_it)
    {
      outs << "PROCEDURE " << unit.RetriveProcName (proc_it) << std::endl
	<<
	"********************************************************************"
	<< std::endl;
      D_UINT locals_count = unit.GetProcLocalsCount (proc_it);
      for (D_UINT local_it = 0; local_it < locals_count; ++local_it)
	{
	  if (local_it == 0)
	    outs << "return (id. 0)\t\t";
	  else if (local_it <= unit.GetProcParametersCount (proc_it))
	    outs << "param (id. " << local_it << " )\t\t";
	  else
	    outs << "local (id. " << local_it << " )\t\t";

	  wod_dump_type_info (unit.RetriveTypeInformation () +
	                        unit.GetProcLocalTypeIndex (proc_it, local_it),
	                      outs);
	  outs << std::endl;
	}

      outs << std::endl << "Code:" << std::endl;
      wod_dump_code (unit.RetriveProcCodeArea (proc_it),
		     unit.GetProcCodeAreaSize (proc_it),
		     outs,
		     unit.RetriveProcName (proc_it));
      outs << std::endl << std::endl;
    }

}
