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
#include "compiler/whisperc.h"
#include "../../utils/include/le_converter.h"

#include "wod_dump.h"

void
wod_dump_header (WFile& rInObj, std::ostream& rOutputStream)
{
  D_UINT32 temp32;
  D_UINT8  woheader[WHC_TABLE_SIZE];

  rInObj.Seek (0, WHC_SEEK_BEGIN);
  rInObj.Read (woheader, sizeof woheader);

  if ((woheader[0] != 'W') || (woheader[1] != 'O'))
    throw WCompiledUnitException ("Not an whisper object file!", _EXTRA(0));

  rOutputStream << std::setbase (16);
  rOutputStream.flags (std::ios::hex | std::ios::uppercase);
  rOutputStream << "File Signature:\t\t\t\t0x" << woheader[0] << " 0x" << woheader[1] <<
       " ('" << woheader[0] << "', '" << woheader[1] << "')" << std::endl;
  rOutputStream.flags (std::ios::dec);
  rOutputStream << "Format major:\t\t\t\t" << _SC (D_UINT, woheader[WHC_FORMATMMAJ_OFF]) << std::endl;
  rOutputStream << "Format minor:\t\t\t\t" << _SC (D_UINT, woheader[WHC_FORMATMIN_OFF]) << std::endl;
  rOutputStream << "Language major:\t\t\t\t" << _SC (D_UINT, woheader[WHC_LANGVER_MAJ_OFF]) << std::endl;
  rOutputStream << "Language minor:\t\t\t\t" << _SC (D_UINT, woheader[WHC_LANGVER_MIN_OFF]) << std::endl;

  temp32 = from_le_int32 (woheader + WHC_GLOBS_COUNT_OFF);
  rOutputStream << "Globals count:\t\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_PROCS_COUNT_OFF);
  rOutputStream << "Procedures count:\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_TYPEINFO_START_OFF);
  rOutputStream << "Type info start position:\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_TYPEINFO_SIZE_OFF);
  rOutputStream << "Type info size:\t\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_SYMTABLE_START_OFF);
  rOutputStream << "Symbols start position:\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_SYMTABLE_SIZE_OFF);
  rOutputStream << "Symbols size:\t\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_CONSTAREA_START_OFF);
  rOutputStream << "Constant text area position:\t\t\t" << temp32 << std::endl;

  temp32 = from_le_int32 (woheader + WHC_CONSTAREA_SIZE_OFF);
  rOutputStream << "Constant text area size:\t\t\t" << temp32 << std::endl;
}

void
wod_dump_const_area (WICompiledUnit& rUnit, std::ostream& rOutputStream)
{
  rOutputStream << std::endl << std::endl <<
    "************************************************************************"
    << std::endl << "THE CONSTANT AREA DUMP" << std::endl <<
    "************************************************************************"
    << std::endl;

  D_UINT constantOff = 0;
  do
    {
      const D_UINT rowSize = 16;

      rOutputStream.flags (std::ios::hex | std::ios::uppercase);
      rOutputStream << std::endl << std::setw (sizeof (D_UINT32) * 2) <<
           std::setfill ('0') << constantOff << ":\t";

      for (D_UINT row_pos = 0; row_pos < rowSize; row_pos++)
        {
          if (row_pos && (row_pos % sizeof (D_UINT32) == 0))
            rOutputStream << " ";

          if (row_pos + constantOff >= rUnit.GetConstAreaSize ())
            rOutputStream << "  ";
          else
            {
              rOutputStream << std::setw (sizeof (D_UINT8) * 2) << std::setfill ('0') <<
                   _SC(D_UINT, rUnit.RetrieveConstArea ()[constantOff + row_pos]);
            }
        }

      rOutputStream << '\t';
      for (D_UINT row_pos = 0; row_pos < rowSize; row_pos++)
        {
          if (row_pos + constantOff >= rUnit.GetConstAreaSize ())
            break;
          else
            {
              if (row_pos && (row_pos % sizeof (D_UINT32) == 0))
                rOutputStream << " ";

              D_UINT char_code = rUnit.RetrieveConstArea ()[constantOff + row_pos];
              if (isalnum (char_code) || ispunct (char_code))
                rOutputStream << _SC (D_CHAR, char_code);
              else
                rOutputStream << ".";
            }
        }
      constantOff += rowSize;
    }
  while (constantOff < rUnit.GetConstAreaSize ());

  rOutputStream << std::setw (1);
  rOutputStream.flags (std::ios::dec);
}

static void
wod_dump_nontable_type_info (std::ostream& rOutStream, D_UINT16 type)
{
  assert ((IS_TABLE (type) == FALSE) && (IS_TABLE_FIELD (type) == FALSE));

  if (IS_FIELD (type))
    {
      rOutStream << "FIELD";

      type = GET_FIELD_TYPE (type);
      if (type != T_UNDETERMINED)
        rOutStream << " OF ";
      else
        return;
    }

  if (IS_ARRAY (type))
    {
      rOutStream << "ARRAY";
      type = GET_BASIC_TYPE (type);

      if (type != T_UNDETERMINED)
        rOutStream << " OF ";
      else
        return;
    }

  assert (type > T_UNKNOWN);
  assert (type < T_END_OF_TYPES);

  switch (type)
    {
    case T_BOOL:
      rOutStream << "BOOL";
      break;
    case T_CHAR:
      rOutStream << "CHARACTER";
      break;
    case T_DATE:
      rOutStream << "DATE";
      break;
    case T_DATETIME:
      rOutStream << "DATETIME";
      break;
    case T_HIRESTIME:
      rOutStream << "HIRESTME";
      break;
    case T_INT8:
      rOutStream << "INT8";
      break;
    case T_INT16:
      rOutStream << "INT16";
      break;
    case T_INT32:
      rOutStream << "INT32";
      break;
    case T_INT64:
      rOutStream << "INT64";
      break;
    case T_REAL:
      rOutStream << "REAL";
      break;
    case T_RICHREAL:
      rOutStream << "RICHREAL";
      break;
    case T_TEXT:
      rOutStream << "TEXT";
      break;
    case T_UINT8:
      rOutStream << "UNSIGNED INT8";
      break;
    case T_UINT16:
      rOutStream << "UNSIGNED INT16";
      break;
    case T_UINT32:
      rOutStream << "UNSIGNED INT32";
      break;
    case T_UINT64:
      rOutStream << "UNSIGNED INT64";
      break;
    default:
      assert (0);
    }
}

static void
wod_dump_table_type_inf (const D_UINT8* pTypeDesc, std::ostream& rOutputStream)
{
  const D_UINT16 type = from_le_int16 (pTypeDesc);
  pTypeDesc += sizeof (D_UINT16);

  const D_UINT16 type_size = from_le_int16 (pTypeDesc);
  pTypeDesc += sizeof (D_UINT16);

  if (IS_TABLE (type))
    rOutputStream << "TABLE";

  if (type_size > 2)
    {
      D_BOOL printComma = FALSE;

      rOutputStream << " WITH ( ";
      while (pTypeDesc[0] != ';' && pTypeDesc[1] != 0)
        {
          if (printComma)
            rOutputStream << ", ";
          else
            printComma = TRUE;

          rOutputStream << pTypeDesc << " AS ";
          pTypeDesc += strlen (_RC (const char *, pTypeDesc)) + 1;

          D_UINT16 type = from_le_int16 (pTypeDesc);

          pTypeDesc += sizeof (D_UINT16);
          wod_dump_nontable_type_info (rOutputStream, type);
        }
      rOutputStream << " )";
    }

}

static void
wod_dump_type_info (const D_UINT8* pTypeDesc, std::ostream& rOutputStream)
{
  const D_UINT16 type     = from_le_int16 (pTypeDesc);
  const D_UINT16 typeSize = from_le_int16 (pTypeDesc + sizeof (D_UINT16));

  if ((typeSize < 2) ||
      (pTypeDesc[typeSize + sizeof (D_UINT16)] != ';') ||
      (pTypeDesc[typeSize + sizeof (D_UINT16) + 1] != 0x0))
    {
      throw WDumpException ("Object file is corrupt!", _EXTRA(0));
    }

  if (IS_TABLE (type) == FALSE)
    return wod_dump_nontable_type_info (rOutputStream, type);
  else
    return wod_dump_table_type_inf (pTypeDesc, rOutputStream);
}

void
wod_dump_globals_tables (WICompiledUnit& rUnit, std::ostream& rOutputStream)
{
  rOutputStream << std::endl << std::endl <<
    "************************************************************************"
    << std::endl <<
    "THE TABLE WITH THE REFERENCES TO THE GLOBALS VALUES"
    << std::endl <<
    "************************************************************************"
    << std::endl << std::endl;

  const D_INT globalsCount = rUnit.GetGlobalsCount ();
  if (globalsCount == 0)
    {
      rOutputStream << "No globals entries." << std::endl;
      return;
    }
  rOutputStream << "Id.\tName\t\t\tVisible\tType" << std::endl <<
    "************************************************************************" << std::endl;

  for (D_INT globalIt = 0; globalIt < globalsCount; ++globalIt)
    {
      rOutputStream << globalIt << "\t";
      rOutputStream << rUnit.RetriveGlobalName (globalIt);
      rOutputStream << "\t\t\t";

      if (rUnit.IsGlobalExternal (globalIt))
        rOutputStream << "EXT" << "\t";
      else
        rOutputStream << "DEF" << "\t";

      wod_dump_type_info (rUnit.RetriveTypeInformation () + rUnit.GetGlobalTypeIndex (globalIt),
                          rOutputStream);
      rOutputStream << std::endl;
    }
  rOutputStream << std::endl;
}

static void
wod_dump_code (const D_UINT8* pCode,
               const D_UINT   codeSize,
               std::ostream&  rOutputStream,
               const D_CHAR*  pPrefix)
{
  D_UINT currPos = 0;

  while (currPos < codeSize)
    {
      if (pPrefix != NULL)
        {
          rOutputStream << pPrefix << "+";
          rOutputStream << std::setw (4) << std::setfill ('0') << currPos;
          rOutputStream << "\t";
        }

      D_CHAR        operand1[MAX_OP_STRING];
      D_CHAR        operand2[MAX_OP_STRING];
      enum W_OPCODE opcode     = W_NA;
      D_UINT        instr_size = whc_decode_opcode (pCode, &opcode);

      instr_size += wod_decode_table[opcode] (pCode + instr_size,
                                              operand1,
                                              operand2);

      rOutputStream << wod_str_table[opcode] << "\t" << operand1;
      if (operand2[0] != 0)
        rOutputStream << ", " << operand2;

      rOutputStream << std::endl;

      currPos += instr_size;
      pCode   += instr_size;
    }
  assert (currPos == codeSize);
}

void
wod_dump_procs (WICompiledUnit& rUnit, std::ostream& rOutputStream, D_BOOL showCode)
{
  const D_UINT procsCount = rUnit.GetProceduresCount ();

  for (D_UINT proc_it = 0; proc_it < procsCount; ++proc_it)
    {
      rOutputStream << "PROCEDURE " << rUnit.RetriveProcName (proc_it) << std::endl
        <<
        "********************************************************************"
        << std::endl;
      D_UINT localsCount = rUnit.GetProcLocalsCount (proc_it);
      for (D_UINT localIt = 0; localIt < localsCount; ++localIt)
        {
          if (localIt == 0)
            rOutputStream << "return (id. 0)\t\t";
          else if (localIt <= rUnit.GetProcParametersCount (proc_it))
            rOutputStream << "param (id. " << localIt << " )\t\t";
          else
            rOutputStream << "local (id. " << localIt << " )\t\t";

          wod_dump_type_info (rUnit.RetriveTypeInformation () +
                                rUnit.GetProcLocalTypeIndex (proc_it, localIt),
                              rOutputStream);
          rOutputStream << std::endl;
        }

      rOutputStream << std::endl << "Code:" << std::endl;
      wod_dump_code (rUnit.RetriveProcCodeArea (proc_it),
                     rUnit.GetProcCodeAreaSize (proc_it),
                     rOutputStream,
                     rUnit.RetriveProcName (proc_it));
      rOutputStream << std::endl << std::endl;
    }
}
