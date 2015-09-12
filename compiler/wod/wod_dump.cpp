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



#include <assert.h>
#include <iomanip>
#include <string.h>

#include "utils/endianness.h"
#include "compiler/whaisc.h"

#include "../whc/wo_format.h"

#include "wod_dump.h"

using namespace std;

namespace whais {
namespace wod {

static const uint_t HEADER_FIELD_LENGTH     = 32;
static const uint_t HEADER_SEPARATOR_LENGTH = 80;

void
wod_dump_header (File& obj, ostream& output)
{
  uint8_t woheader[WHC_TABLE_SIZE];

  obj.Seek (0, WH_SEEK_BEGIN);
  obj.Read (woheader, sizeof woheader);

  if ((woheader[0] != 'W') || (woheader[1] != 'O'))
    throw FunctionalUnitException (_EXTRA (0), "Not a whais object file!");

  output << setbase (16);
  output.flags (ios::hex | ios::uppercase);

  output.width (HEADER_FIELD_LENGTH);
  output << left << "File Signature";
  output.width (0);
  output << ": 0x" << woheader[0] << " 0x" << woheader[1]
         << " ('" << woheader[0] << "', '" << woheader[1] << "')\n";
  output.flags (ios::dec);


  output.width (HEADER_FIELD_LENGTH);
  output << left << "Header version major";
  output.width (0);
  output <<  ": " << _SC (uint_t, woheader[WHC_FORMATMMAJ_OFF]) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Header version minor";
  output.width (0);
  output << ": " << _SC (uint_t, woheader[WHC_FORMATMIN_OFF]) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Language major";
  output.width (0);
  output << ": " << _SC (uint_t, woheader[WHC_LANGVER_MAJ_OFF]) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Language minor";
  output << ": " << _SC (uint_t, woheader[WHC_LANGVER_MIN_OFF]) << endl;
  output.width (0);

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Globals count";
  output.width (0);
  output << ": " << load_le_int32 (woheader + WHC_GLOBS_COUNT_OFF) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Procedures count";
  output.width (0);
  output << ": " << load_le_int32 (woheader + WHC_PROCS_COUNT_OFF) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Type info start position";
  output.width (0);
  output << ": " << load_le_int32 (woheader + WHC_TYPEINFO_START_OFF) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Type info size";
  output.width (0);
  output << ": " <<  load_le_int32 (woheader + WHC_TYPEINFO_SIZE_OFF) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Symbols start position";
  output.width (0);
  output << ": " << load_le_int32 (woheader + WHC_SYMTABLE_START_OFF) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Symbols size";
  output.width (0);
  output << ": " << load_le_int32 (woheader + WHC_SYMTABLE_SIZE_OFF) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Constant text area position";
  output.width (0);
  output << ": " <<  load_le_int32 (woheader + WHC_CONSTAREA_START_OFF) << endl;

  output.width (HEADER_FIELD_LENGTH);
  output << left << "Constant text area size";
  output.width (0);
  output << ": " << load_le_int32 (woheader + WHC_CONSTAREA_SIZE_OFF) << endl;
}

void
wod_dump_const_area (WIFunctionalUnit& obj, ostream& output)
{
  output << endl << endl << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*')
    << '*' << setw (0) << "\nTHE CONSTANT AREA DUMP\n"
    << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*') << '*' << setw (0)
    << endl << endl;

  const uint8_t* const constArea = obj.RetrieveConstArea ();

  uint_t constantOff = 0;
  do
    {
      const uint_t rowSize = 16; //Chars per row to print.

      output.flags (ios::hex | ios::uppercase);
      output << endl << setw (sizeof (uint32_t) * 2)
             << setfill ('0') << constantOff << ":\t";

      /* Print first the binary representation. */
      for (uint_t rowPos = 0; rowPos < rowSize; rowPos++)
        {
          if (rowPos && (rowPos % sizeof (uint32_t) == 0))
            output << ' ';

          if (rowPos + constantOff >= obj.ConstsAreaSize ())
            output << "  ";

          else
            {
              output << setw (sizeof (uint8_t) * 2) << setfill ('0')
                     << _SC (uint_t, constArea[constantOff + rowPos]);
            }
        }

      /* Try to print the constant symbols. */
      output << '\t';
      for (uint_t rowPos = 0; rowPos < rowSize; rowPos++)
        {
          if (rowPos + constantOff >= obj.ConstsAreaSize ())
            break;

          else
            {
              if (rowPos && (rowPos % sizeof (uint32_t) == 0))
                output << ' ';

              const uint_t codeUnit = constArea[constantOff + rowPos];
              if (isalnum (codeUnit) || ispunct (codeUnit))
                output << _SC (char, codeUnit);

              else
                output << '.'; //Default representation for unprintable chars.
            }
        }
      constantOff += rowSize;
    }
  while (constantOff < obj.ConstsAreaSize ());

  output << setw (0);
  output.flags (ios::dec);
}

static void
wod_dump_nontable_type_info (ostream& output, uint16_t type)
{
  assert ((IS_TABLE (type) == FALSE) && (IS_TABLE_FIELD (type) == FALSE));

  if (IS_FIELD (type))
    {
      output << "FIELD";

      type = GET_FIELD_TYPE (type);
      if (type != T_UNDETERMINED)
        output << " ";

      else
        return;
    }

  if (IS_ARRAY (type))
    {
      output << "ARRAY";

      type = GET_BASIC_TYPE (type);
      if (type != T_UNDETERMINED)
        output << " ";

      else
        return;
    }

  assert (type > T_UNKNOWN);
  assert (type < T_END_OF_TYPES);

  switch (type)
    {
    case T_BOOL:
      output << "BOOL";
      break;

    case T_CHAR:
      output << "CHAR";
      break;

    case T_DATE:
      output << "DATE";
      break;

    case T_DATETIME:
      output << "DATETIME";
      break;

    case T_HIRESTIME:
      output << "HIRESTME";
      break;

    case T_INT8:
      output << "INT8";
      break;

    case T_INT16:
      output << "INT16";
      break;

    case T_INT32:
      output << "INT32";
      break;

    case T_INT64:
      output << "INT64";
      break;

    case T_REAL:
      output << "REAL";
      break;

    case T_RICHREAL:
      output << "RICHREAL";
      break;

    case T_TEXT:
      output << "TEXT";
      break;

    case T_UINT8:
      output << "UINT8";
      break;

    case T_UINT16:
      output << "UINT16";
      break;

    case T_UINT32:
      output << "UINT32";
      break;

    case T_UINT64:
      output << "UINT64";
      break;

    case T_UNDETERMINED:
      output << "UNDEFINED";
      break;

    default:
      assert (false);
    }
}


static void
wod_dump_table_type_inf (const uint8_t* typeDesc, ostream& output)
{
  const uint16_t type = load_le_int16 (typeDesc);
  typeDesc += sizeof (uint16_t);

  const uint16_t descSize = load_le_int16 (typeDesc);
  typeDesc += sizeof (uint16_t);

  if (IS_TABLE (type))
    output << "TABLE";

  if (descSize > 2)
    {
      bool_t printComma = FALSE;

      output << " (";
      while (typeDesc[0] != ';' && typeDesc[1] != 0)
        {
          if (printComma)
            output << ", ";

          else
            printComma = TRUE;

          output << typeDesc << " ";
          typeDesc += strlen (_RC (const char *, typeDesc)) + 1;

          uint16_t type = load_le_int16 (typeDesc);

          typeDesc += sizeof (uint16_t);
          wod_dump_nontable_type_info (output, type);
        }
      output << " )";
    }

}


static void
wod_dump_type_info (const uint8_t* typeDesc, ostream& output)
{
  const uint16_t type     = load_le_int16 (typeDesc);
  const uint16_t descSize = load_le_int16 (typeDesc + sizeof (uint16_t));

  if ((descSize < 2) ||
      (typeDesc[descSize + sizeof (uint16_t)] != ';') ||
      (typeDesc[descSize + sizeof (uint16_t) + 1] != 0x0))
    {
      throw DumpException (_EXTRA (0), "Object file is corrupt!");
    }

  if (IS_TABLE (type) == FALSE)
    return wod_dump_nontable_type_info (output, type);

  else
    return wod_dump_table_type_inf (typeDesc, output);
}


void
wod_dump_globals_tables (WIFunctionalUnit& obj, ostream& output)
{
  static const int ID_SIZE     = 4;
  static const int NAME_LEN    = 24;
  static const int VISIBLE_LEN = 3;

  output << endl << endl << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*')
    << '*' << setw (0) << "\nGLOBAL VALUES REFERENCES.\n"
    << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*') << '*' << setw (0)
    << endl << endl;

  const int globalsCount = obj.GlobalsCount ();
  if (globalsCount == 0)
    {
      output << "No globals entries.\n\n";
      return;
    }

  output.width (ID_SIZE);
  output << setfill (' ') << left << "Id. ";
  output.width (NAME_LEN);
  output << left << "Name ";
  output.width (VISIBLE_LEN);
  output << left << "Def ";
  output.width (0);
  output << "Type " << endl << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*')
         << '*' << setw (0) << endl;

  for (int i = 0; i < globalsCount; ++i)
    {
      output.width (ID_SIZE);
      output << setfill (' ') << i << setw (0) << ' ';

      output.width (NAME_LEN - 1);
      output << left << obj.RetriveGlobalName (i) << setw (0) << ' ';

      output.width (VISIBLE_LEN - 1);
      output << left << (obj.IsGlobalExternal (i) ? "EXT " : "DEF ");
      output.width (0);

      wod_dump_type_info (obj.RetriveTypeArea () + obj.GlobalTypeOff (i),
                          output);
      output << endl;
    }
  output << endl;
}


static void
wod_dump_code (const uint8_t*       code,
               const uint_t         codeSize,
               ostream&             output,
               const char*          prefix)
{
  uint_t currPos = 0;

  while (currPos < codeSize)
    {
      if (prefix != NULL)
        {
          output << prefix << "+";
          output << setw (4) << setfill ('0') << right << currPos;
          output << left << ' ';
        }

      char operand1[MAX_OP_STRING];
      char operand2[MAX_OP_STRING];

      enum W_OPCODE opcode    = W_NA;
      uint_t        instrSize = wh_compiler_decode_op (code, &opcode);

      instrSize += wod_decode_table[opcode] (code + instrSize,
                                             operand1,
                                             operand2);

      output << setw (10) << setfill (' ') << wod_str_table[opcode];
      output << setw (0) << operand1;

      if (operand2[0] != 0)
        output << ", " << operand2;

      output << endl;

      code += instrSize, currPos += instrSize;
    }

  assert (currPos == codeSize);
}


void
wod_dump_procs (WIFunctionalUnit& obj, ostream& output, bool_t showCode)
{

  output << endl << endl << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*')
    << '*' << setw (0) << "\nPROCEDURES DESCRIPTIONS\n"
    << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*') << '*' << setw (0)
    << endl << endl;

  const uint_t procsCount = obj.ProceduresCount ();
  if (procsCount == 0)
    {
      output << "No procedures entries.\n\n";
      return;
    }

  for (uint_t proc = 0; proc < procsCount; ++proc)
    {
      const bool externalProc = obj.ProcCodeAreaSize (proc) == 0;
      output << '[' << proc << "] ";
      if (externalProc)
          output << "EXTERN PROCEDURE ";

      else
          output << "PROCEDURE ";

      output << obj.RetriveProcName (proc) << endl
             << setw (HEADER_SEPARATOR_LENGTH) << setfill ('*') << '*'
             << setw (0) << endl;

      const uint_t localsCount = obj.ProcLocalsCount (proc);
      for (uint_t local = 0; local < localsCount; ++local)
        {
          if (local == 0)
            output << "return (id: -)\t\t";

          else if (local <= obj.ProcParametersCount (proc))
            output << "param (id: " << local - 1 << " )\t\t";

          else
            output << "local (id:  " << local - 1 << " )\t\t";

          wod_dump_type_info (obj.RetriveTypeArea () +
                                obj.GetProcLocalTypeOff (proc, local),
                              output);
          output << endl;
        }

      if (! externalProc)
        {
          output << endl << "Code:" << endl;

          wod_dump_code (obj.RetriveProcCodeArea (proc),
                         obj.ProcCodeAreaSize (proc),
                         output,
                         obj.RetriveProcName (proc));
        }

      output << endl << endl;
    }
}

} //namespace wod
} //namespace whais

