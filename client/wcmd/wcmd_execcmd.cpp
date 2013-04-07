/******************************************************************************
  WCMD - An utility to manage whisper database files.
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

#include <assert.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstring>

#include "utils/include/utf8.h"
#include "client/include/whisper_connector.h"

#include "wcmd_execcmd.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_onlinecmds.h"
#include "wcmd_optglbs.h"

using namespace std;


static const D_CHAR     NULL_VALUE[]   = "";
static const D_CHAR     NULL_LABEL[]  = "(NULL)";

typedef map<D_UINT, vector<string> > FIELD_ENTRY;
typedef FIELD_ENTRY::iterator        FIELD_ENTRY_IT;

struct FieldEntry
{
  FieldEntry (const D_UINT fieldType = WFT_NOTSET)
    : m_Values (),
      m_FieldType (fieldType)
  {
  }

  bool
  SetValue (const D_UINT           row,
            const D_UINT16         type,
            const string&          value)
  {
    if ((m_FieldType != WFT_NOTSET) && (type != m_FieldType))
      return false;

    m_FieldType = type;
    m_Values[row].push_back (value);

    return true;
  }

  FIELD_ENTRY m_Values;
  D_UINT16    m_FieldType;
};

typedef map<string, FieldEntry> FIELD_VALUE;
typedef FIELD_VALUE::iterator   FIELD_VALUE_IT;

struct TableParameter
{
  TableParameter ()
    : m_Fields (),
      m_RowsCount (0)
  {
  }

  bool
  SetValue (const string&          field,
            const D_UINT           row,
            const D_UINT           type,
            const string&          value)
  {
    if (m_RowsCount < (row + 1))
      m_RowsCount = row + 1;

    if (! m_Fields[field].SetValue (row, type, value))
      {
        cout << "Invalid command format.";
        cout << "Field '" << field << "' has different types values.\n";

        return false;
      }

    return true;
  }

  FIELD_VALUE  m_Fields;
  D_UINT       m_RowsCount;
};

D_UINT
update_stack_value (const W_CONNECTOR_HND         hnd,
                    const unsigned int            type,
                    const D_CHAR* const           fieldName,
                    const W_TABLE_ROW_INDEX       row,
                    const W_ELEMENT_OFFSET        arrayOff,
                    const D_CHAR* const           value)
{
  /* When the values was created on the stack,
   * it already has value set to NULL. We need a was to specify NULL values
   * so don't pass this connector because it will treated as an error. */
  if (strcmp (value, NULL_VALUE) == 0)
    return WCS_OK;

  D_UINT wcs = WCS_OK;

  if (type != WFT_TEXT)
    {
      return WUpdateStackValue (hnd,
                                type,
                                fieldName,
                                row,
                                arrayOff,
                                WIGNORE_OFF,
                                value);
    }

  assert (arrayOff == WIGNORE_OFF);

  //Send one character at a time to avoid large parameter errors.
  D_UINT       utf8TextOff  = 0;
  const D_UINT utf8ValueLen = utf8_strlen (_RC (const D_UINT8*, value));
  D_UINT       valueOff     = 0;
  while ((utf8TextOff < utf8ValueLen)
         && (wcs == WCS_OK))
    {
      assert (valueOff < strlen (value));

      const D_UINT charSize = get_utf8_char_size (value[valueOff]);

      D_CHAR tempBuffer[UTF8_MAX_BYTES_COUNT];
      memcpy (tempBuffer, value + valueOff, charSize);

      tempBuffer[charSize]  = 0;
      valueOff             += charSize;

      assert (valueOff <= strlen (value));

      wcs = WUpdateStackValue (hnd,
                               WFT_TEXT,
                               fieldName,
                               row,
                               WIGNORE_OFF,
                               utf8TextOff++,
                               tempBuffer);
    }

  return wcs;
}

static bool
parse_value (const string& cmdLine,
             size_t&       ioLineOff,
             string&       oValue)
{
  const D_CHAR* const line = cmdLine.c_str ();

  assert (line[ioLineOff - 1] == '\'');

  oValue.clear ();
  while (ioLineOff < cmdLine.length ())
    {
      if (line[ioLineOff] == '\'')
        {
          ioLineOff++;
          return true;
        }

      if (line[ioLineOff] == '\\')
        {
          if (++ioLineOff >= cmdLine.length ())
            return false;

          switch (line[ioLineOff++])
          {
          case 'f':
            oValue.push_back ('\f');
            break;
          case 'n':
            oValue.push_back ('\n');
            break;
          case 'r':
            oValue.push_back ('\r');
            break;
          case 't':
            oValue.push_back ('\t');
            break;
          case 'v':
            oValue.push_back ('\v');
            break;
          case '\\':
            oValue.push_back ('\\');
            break;
          case '\'':
            oValue.push_back ('\'');
            break;
          case 'u':
            {
              D_UINT64 charCode = 0;
              while (line[ioLineOff] != '&')
                {
                  charCode *= 16;
                  if (('0' <= line[ioLineOff])
                      && (line[ioLineOff] <= '9'))
                    {
                      charCode += line[ioLineOff] - '0';
                    }
                  else if (('a' <= line[ioLineOff])
                           && (line[ioLineOff] <= 'f'))
                    {
                      charCode += line[ioLineOff] - 'a' + 10;
                    }
                  else if (('A' <= line[ioLineOff])
                           && (line[ioLineOff] <= 'F'))
                    {
                      charCode += line[ioLineOff] - 'A' + 10;
                    }
                  else
                    {
                      cout << "Invalid command format. ";
                      cout << "Missing '&' to specify the end of the '\\u' ";
                      cout << "character escape sequence.\n";

                      return false;
                    }

                  if (charCode > 0xFFFFFFFF)
                    {
                      cout << "Invalid command format.";
                      cout << "The value specified with the escape sequence ";
                      cout << "'\\u' is not a valid Unicode point.\n";

                      return false;
                    }
                  ++ioLineOff;
                }
              ++ioLineOff;

              if (charCode == 0)
                {
                  cout << "Invalid command format.";
                  cout << "The oValue specified with the escape sequence ";
                  cout << "'\\u' cannot be 0.\n";

                  return false;
                }

              D_UINT8 utf8CodeUnits[UTF8_MAX_BYTES_COUNT];
              D_UINT  utf8UnitsCount = encode_utf8_char (charCode,
                                                         utf8CodeUnits);
              assert (utf8UnitsCount > 0);
              oValue.append (_RC (const D_CHAR*, utf8CodeUnits),
                             utf8UnitsCount);
            }
            break;

          default:
            cout << "Invalid command format. Unknown escape sequence ";
            cout << "'\\" << line[ioLineOff - 1] << "'.\n";

            return false;
          }
        }
      else
        oValue.push_back (line[ioLineOff++]);
    }

  cout << "Invalid command format. Missing ' at the end of the oValue.\n";

  return false;
}

static bool
parse_type (const string&       cmdLine,
            size_t&             ioLineOff,
            D_UINT&             oType)
{
  const D_CHAR* const line = cmdLine.c_str ();

  switch (line[ioLineOff++])
  {
  case      'B':
  case      'b':
    oType = WFT_BOOL;
    break;

  case      'C':
  case      'c':
    oType = WFT_CHAR;
    break;

  case      'D':
  case      'd':
    oType = WFT_DATE;
    break;

  case      'H':
  case      'h':
    oType = WFT_DATETIME;
    break;

  case      'M':
  case      'm':
    oType = WFT_HIRESTIME;
    break;

  case      'I':
  case      'i':
    oType = WFT_INT64;
    break;

  case      'U':
  case      'u':
    oType = WFT_UINT64;
    break;

  case      'R':
  case      'r':
    oType = WFT_RICHREAL;
    break;

  case      't':
  case      'T':
    oType = WFT_TEXT;
    break;

  default:
    cout << "Invalid command format. Unexpected parameter specifier '"
         << line[ioLineOff - 1] <<"'.\n";

    return false;
  }

  return true;
}

static bool
handle_param_value (W_CONNECTOR_HND           hnd,
                    const string&             cmdLine,
                    const D_UINT              type,
                    const D_CHAR* const       field,
                    const W_TABLE_ROW_INDEX   row,
                    const W_ELEMENT_OFFSET    arrayOff,
                    size_t&                   ioLineOff)
{
  D_UINT   wcs = WCS_OK;

  string value;
  if (! parse_value (cmdLine, ioLineOff, value))
    return false;
  else if ((value.length () == 0) && (arrayOff != WIGNORE_OFF))
    {
      cout << "Invalid command format. "
           << "Empty '' (null) is not a valid element for an array value.\n";

      return false;
    }

  wcs = update_stack_value (hnd,
                            type,
                            field,
                            row,
                            arrayOff,
                            value.c_str ());
  if (wcs != WCS_OK)
    goto proc_param_connector_error;

  return true;

proc_param_connector_error:
  assert (wcs != WCS_OK);

  cout << "Failed to send procedure parameters:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}

static bool
handle_procedure_array_param (W_CONNECTOR_HND           hnd,
                              const string&             cmdLine,
                              const D_UINT              type,
                              const D_CHAR* const       field,
                              const W_TABLE_ROW_INDEX   row,
                              size_t&                   ioLineOff)
{
  const D_CHAR* const line = cmdLine.c_str ();

  D_UINT64 arrayIndex = 0;
  D_UINT   wcs        = WCS_OK;

  assert (type != WFT_TEXT);
  assert (line[ioLineOff - 1] == '(');

  while (ioLineOff < cmdLine.length ())
    {
      if (line[ioLineOff] == ')')
        {
          ++ioLineOff;
          return true;
        }

      if ((line[ioLineOff] == ' ') || (line[ioLineOff] == '\t'))
        {
          ++ioLineOff;
          continue;
        }
      else if (line[ioLineOff++] != '\'')
        {
          cout << "Invalid command format. ";
          cout << "The value of the array's element should begin with a '.\n";

          return false;
        }

      if (! handle_param_value (hnd,
                                cmdLine,
                                type,
                                field,
                                row,
                                arrayIndex++,
                                ioLineOff))
        {
          return false;
        }
    }

  assert (wcs == WCS_OK);

  cout << "Invalid command format. Missing ')' to mark the end of array.\n";
  return false;
}

static bool
handle_procedure_table_param (W_CONNECTOR_HND           hnd,
                              const string&             cmdLine,
                              size_t&                   ioLineOff)
{
  const D_CHAR* const line       = cmdLine.c_str ();
  D_UINT              wcs        = WCS_OK;
  D_UINT              row        = 0;
  D_UINT              type       = 0;
  string              field;
  string              value;
  TableParameter      table;
  bool                rowStarted   = false;
  bool                fieldParse   = false;
  bool                typeParse    = false;
  bool                valueParse   = false;
  bool                arrayValue   = false;
  bool                emptyRow     = true;
  bool                endOfEntry   = false;
  bool                ignoreSpaces = true;

  assert (line[ioLineOff - 1] == '[');

  while (ioLineOff < cmdLine.length ())
    {
      if (ignoreSpaces
          && ((line[ioLineOff] == ' ') || (line[ioLineOff] == '\t')))
        {
          ioLineOff++;
          continue;
        }

      if (! rowStarted)
        {
          if (line[ioLineOff] == ']')
            {
              endOfEntry = true;
              ++ioLineOff;

              break;
            }
          else if (line[ioLineOff++] != '(')
            {
              cout << "Invalid command format. ";
              cout << "Missing '(' at the beginning of value.\n";

              return false;
            }

          rowStarted = true;
          fieldParse = true;
          emptyRow   = true;

          field.clear ();

          continue;
        }

      if (fieldParse)
        {
          ignoreSpaces = false;

          if (line[ioLineOff] == '.')
            {
              ++ioLineOff;
              emptyRow   = false;
              fieldParse = false;
              typeParse  = true;

              if (field.length () == 0)
                {
                  cout << "Invalid command format. ";
                  cout << "A non empty field has to be provided.\n";

                  return false;
                }
            }
          else if (line[ioLineOff] == ')')
            {
              ++ioLineOff;
              ++row;

              fieldParse   = false;
              rowStarted   = false;
              ignoreSpaces = true;

              if (emptyRow)
                {
                  cout << "Invalid command format. ";
                  cout << "A table value cannot have an empty row.\n";

                  return false;
                }
            }
          else
            {
              field.append (line + ioLineOff, 1);
              ioLineOff++;
            }

          continue;
        }

      if (typeParse)
        {
          assert (emptyRow == false);

          if ( ! parse_type (cmdLine, ioLineOff, type))
            return false;
          else
            {
              typeParse    = false;
              valueParse   = true;
              ignoreSpaces = true;
            }
          continue;
        }

      if (valueParse)
        {
          assert (ignoreSpaces);
          if ((line[ioLineOff] == '(') || arrayValue)
            {
              if (line[ioLineOff] == '(')
                {
                  ++ioLineOff;
                  type       |= WFT_ARRAY_MASK;
                  arrayValue  = true;

                  continue;
                }
              else
                {
                  assert (arrayValue);
                  assert (type & WFT_ARRAY_MASK);
                }

              if (line[ioLineOff] == ')')
                {
                  ioLineOff++;

                  //Put a simple place holder for this to notify
                  //his presence in case of an empty array
                  if ( ! table.SetValue (field, row, type, NULL_VALUE))
                    return false;

                  arrayValue   = false;
                  valueParse   = false;
                  fieldParse   = true;

                  field.clear ();

                  continue ;
                }
              else if (line[ioLineOff++] != '\'')
                {
                  cout << "Invalid command format. Unexpected character '";
                  cout << line[ioLineOff] << "\'.\n";

                  return false;
                }

              if ( ! parse_value (cmdLine, ioLineOff, value))
                return false;

              if ( ! table.SetValue (field, row, type, value))
                return false;
            }
          else if (line[ioLineOff] == '\'')
            {
              ++ioLineOff;
              if ( ! parse_value (cmdLine, ioLineOff, value))
                return false;

              if ( ! table.SetValue (field, row, type, value))
                return false;

              valueParse   = false;
              fieldParse   = true;

              field.clear ();
            }
          else
            {
              cout << "Invalid command format. Unexpected character '";
              cout << line[ioLineOff] << "\'.\n";

              return false;
            }
        }
    }

  if (! endOfEntry)
    {
      cout << "Invalid command format. Table value entry is not complete.\n";

      return false;
    }

  vector<W_FieldDescriptor> fields;
  for (FIELD_VALUE_IT it = table.m_Fields.begin ();
       it != table.m_Fields.end ();
       ++it)
    {
      W_FieldDescriptor fd;

      fd.m_FieldName = it->first.c_str ();
      fd.m_FieldType = it->second.m_FieldType;

      fields.push_back (fd);
    }

  assert (fields.size () == table.m_Fields.size ());
  assert (fields.size () > 0);

  wcs = WPushStackValue (hnd, WFT_TABLE_MASK, fields.size (), &fields[0]);
  if (wcs != WCS_OK)
    goto proc_param_connector_error;

  assert (table.m_RowsCount > 0);
  for (D_UINT row = 0; row < table.m_RowsCount; ++row)
    {
      for (FIELD_VALUE_IT it = table.m_Fields.begin ();
           it != table.m_Fields.end ();
           ++it)
        {
          FIELD_ENTRY_IT entry = it->second.m_Values.find (row);
          if (entry == it->second.m_Values.end ())
            continue;

          if (it->second.m_FieldType & WFT_ARRAY_MASK)
            {
              assert (entry->second.size () >= 1);

              for (D_UINT arrayId = 0;
                   arrayId < entry->second.size ();
                   ++arrayId)
                {
                  wcs = update_stack_value (
                              hnd,
                              it->second.m_FieldType & ~WFT_ARRAY_MASK,
                              it->first.c_str (),
                              row,
                              arrayId,
                              entry->second[arrayId].c_str ()
                                            );
                  if (wcs != WCS_OK)
                    goto proc_param_connector_error;
                }
            }
          else
            {
              assert (entry->second.size () == 1);
              wcs = update_stack_value (hnd,
                                        it->second.m_FieldType,
                                        it->first.c_str (),
                                        row,
                                        WIGNORE_OFF,
                                        entry->second[0].c_str ());
              if (wcs != WCS_OK)
                goto proc_param_connector_error;
            }
        }
    }

  assert (wcs == WCS_OK);
  return true;

proc_param_connector_error:
  assert (wcs != WCS_OK);

  cout << "Failed to send procedure parameters:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}

static bool
handle_procedure_parameters (W_CONNECTOR_HND hnd,
                             const string&   cmdLine,
                             size_t&         ioLineOff)
{
  const D_CHAR* const line = cmdLine.c_str ();
  D_UINT              type = WFT_NOTSET;
  D_UINT              wcs  = WCS_OK;

  while ((ioLineOff < cmdLine.length ())
         && (wcs == WCS_OK))
    {
      if ((line[ioLineOff] == ' ') || (line[ioLineOff] == '\t'))
        {
          ++ioLineOff;
          continue;
        }

      if (line[ioLineOff] == '[')
        {
          ++ioLineOff;

          if ( ! handle_procedure_table_param (hnd, cmdLine, ioLineOff))
            return false;
        }
      else
        {
          if (!parse_type (cmdLine, ioLineOff, type))
            return false;

          while ((ioLineOff < cmdLine.length ())
                 && (line[ioLineOff] == ' ')
                 && (line[ioLineOff] == '\t'))
            {
              ++ioLineOff;
              continue;
            }

          if (ioLineOff >= cmdLine.length ())
            {
              cout << "Invalid command format. No values was specified.\n";
              return false;

            }
          else if (line[ioLineOff] == '(')
            {
              wcs = WPushStackValue (hnd, type | WFT_ARRAY_MASK, 0, NULL);
              if (wcs != WCS_OK)
                goto proc_param_connector_error;

              ++ioLineOff;
              if (! handle_procedure_array_param (hnd,
                                                  cmdLine,
                                                  type,
                                                  WIGNORE_FIELD,
                                                  WIGNORE_ROW,
                                                  ioLineOff))
                {
                  return false;
                }
            }
          else if (line[ioLineOff] == '\'')
            {
              wcs = WPushStackValue (hnd, type, 0, NULL);
              if (wcs != WCS_OK)
                goto proc_param_connector_error;

              ++ioLineOff;
              if (! handle_param_value (hnd,
                                        cmdLine,
                                        type,
                                        WIGNORE_FIELD,
                                        WIGNORE_ROW,
                                        WIGNORE_OFF,
                                        ioLineOff))
                {
                  return false;
                }
            }
          else
            {
              cout << "Invalid command format. Unexpected character '";
              cout << line[ioLineOff] << "\'.\n";

              return false;
            }
        }
    }

  wcs = WUpdateStackFlush (hnd);
  if (wcs == WCS_OK)
    return true;

proc_param_connector_error:
  assert (wcs != WCS_OK);

  cout << "Failed to send procedure parameters:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}

static bool
fetch_execution_simple_result (W_CONNECTOR_HND         hnd,
                               const D_UINT            type,
                               const char* const       field,
                               const D_UINT64          row)

{
  D_UINT         wcs = WCS_OK;
  const D_CHAR*  retValue;

  if (type != WFT_TEXT)
    {
      wcs = WGetStackValueEntry (hnd,
                                 field,
                                 row,
                                 WIGNORE_OFF,
                                 WIGNORE_OFF,
                                 &retValue);
      if (wcs != WCS_OK)
        goto fetch_result_fail;

      if (strcmp (retValue, NULL_VALUE) == 0)
        cout << NULL_LABEL;
      else
        cout << '\'' << retValue << '\'';
    }
  else
    {
      unsigned long long length  = 0;
      D_UINT64 offset  = 0;
      wcs = WGetStackTextLengthCount (hnd, field, row, WIGNORE_OFF, &length);
      if (wcs != WCS_OK)
        goto fetch_result_fail;

      if (length > 0)
        {
          cout << '\'';
          while (offset < length)
            {
              wcs = WGetStackValueEntry (hnd,
                                        field,
                                        row,
                                        WIGNORE_OFF,
                                        offset,
                                        &retValue);
              if (wcs != WCS_OK)
                goto fetch_result_fail;

              offset += utf8_strlen (_RC (const D_UINT8*, retValue));

              cout << retValue;
            }
          cout << '\'';
        }
      else
        cout << NULL_LABEL;

      assert (offset == length);
    }

  assert (wcs == WCS_OK);
  return true;

fetch_result_fail:

  assert (wcs != WCS_OK);

  cout << "Failed to fetch result:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}


static bool
fetch_execution_array_result (W_CONNECTOR_HND        hnd,
                              const D_UINT           type,
                              const char* const      field,
                              const D_UINT64         row)

{
  assert ((type >= WFT_BOOL) && (type < WFT_TEXT));

  const D_CHAR*         retValue;
  unsigned long long              count;
  D_UINT                wcs;

  if ((wcs = WGetStackArrayElementsCount (hnd, field, row, &count)) != WCS_OK)
    goto fetch_result_fail;

  if (count > 0)
    {
      for (D_UINT64 i = 0; i < count; ++i)
        {
          wcs = WGetStackValueEntry (hnd, field, row, i, WIGNORE_OFF, &retValue);
          if (wcs != WCS_OK)
            goto fetch_result_fail;

          cout << ((i > 0) ? " '" : "'") << retValue << '\'';
        }
    }
  else
    cout << NULL_LABEL;

  return true;

fetch_result_fail:

  assert (wcs != WCS_OK);

  cout << "Failed to fetch array result:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;

}

static bool
fetch_execution_field_result (W_CONNECTOR_HND hnd, const D_UINT type)
{
  unsigned long long rowsCount = 0;
  D_UINT   wcs       = WCS_OK;

  if ((wcs = WGetStackValueRowsCount (hnd, &rowsCount)) != WCS_OK)
    goto fetch_result_fail;

  if (rowsCount > 0)
    {
      for (D_UINT64 row = 0; row < rowsCount; ++row)
        {
          cout << row << " | ";
          if (type & WFT_ARRAY_MASK)
            {
              if ( ! fetch_execution_array_result (hnd,
                                                   type & ~WFT_ARRAY_MASK,
                                                   WIGNORE_FIELD,
                                                   row))
                {
                  return false;
                }
            }
          else
            {
              if ( ! fetch_execution_simple_result (hnd,
                                                    type,
                                                    WIGNORE_FIELD,
                                                    row))
                {
                  return false;
                }
            }
          cout << endl;
        }
    }
  else
    cout << NULL_LABEL << endl;

  return true;

fetch_result_fail:

  assert (wcs != WCS_OK);

  cout << "Failed to fetch field results:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}

static bool
fetch_execution_table_result (W_CONNECTOR_HND hnd,
                              vector<W_FieldDescriptor>& fields)
{
  unsigned long long rowsCount = 0;
  D_UINT   wcs       = WCS_OK;

  if ((wcs = WGetStackValueRowsCount (hnd, &rowsCount)) != WCS_OK)
    goto fetch_result_fail;

  if (rowsCount > 0)
    {
      for (D_UINT64 row = 0; row < rowsCount; ++row)
        {
          for (D_UINT i = 0; i < fields.size (); ++i )
            {
              cout << row << " | " << fields[i].m_FieldName << " | ";
              if (fields[i].m_FieldType & WFT_ARRAY_MASK)
                {
                  if ( ! fetch_execution_array_result (
                                      hnd,
                                      fields[i].m_FieldType & ~WFT_ARRAY_MASK,
                                      fields[i].m_FieldName,
                                      row
                                                       ))
                    {
                      return false;
                    }
                }
              else
                {
                  if ( ! fetch_execution_simple_result (hnd,
                                                        fields[i].m_FieldType,
                                                        fields[i].m_FieldName,
                                                        row))
                    {
                      return false;
                    }
                }
              cout << endl;
            }
            cout << endl;
        }
    }
  else
    cout << NULL_LABEL << endl;

  return true;

fetch_result_fail:

  assert (wcs != WCS_OK);

  cout << "Failed to fetch table result:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}

static bool
fetch_execution_result (W_CONNECTOR_HND hnd)
{
  D_UINT stackResut = WFT_NOTSET;
  D_UINT wcs        = WCS_OK;

  if ((wcs = WDescribeStackTop (hnd, &stackResut)) != WCS_OK)
    goto fetch_result_fail;

  if (stackResut & WFT_TABLE_MASK)
    {
      vector<string>            fields;
      vector<W_FieldDescriptor> fieldsDescriptors;
      D_UINT                    fieldsCount;

      assert (stackResut == WFT_TABLE_MASK);

      if ((wcs = WDescribeValueGetFieldsCount (hnd, &fieldsCount)) != WCS_OK)
        goto fetch_result_fail;

      assert (fieldsCount > 0);

      cout << "TABLE OF (";
      for (D_UINT i = 0; i < fieldsCount; ++i)
        {
          W_FieldDescriptor     fd;
          const D_CHAR*         fieldName;
          D_UINT                fieldType;

          wcs = WDescribeValueFetchField (hnd, &fieldName, &fieldType);
          if (wcs != WCS_OK)
            goto fetch_result_fail;

          fields.push_back (fieldName);

          fd.m_FieldName = fields.back ().c_str ();
          fd.m_FieldType = fieldType;

          fieldsDescriptors.push_back (fd);

          if (i > 0)
            cout << ", ";
          cout << fd.m_FieldName << " AS ";
          cout << wcmd_decode_typeinfo (fd.m_FieldType);
        }
      cout << ")\n";

      assert (fieldsCount == fields.size ());
      assert (fieldsCount == fieldsDescriptors.size ());

      if ( !  fetch_execution_table_result (hnd, fieldsDescriptors))
        return false;

    }
  else if (stackResut & WFT_FIELD_MASK)
    {
      cout << "FIELD OF ";
      cout << wcmd_decode_typeinfo (stackResut & ~WFT_FIELD_MASK) << endl;

      if ( ! fetch_execution_field_result (hnd,
                                           stackResut & ~WFT_FIELD_MASK))
        {
          return false;
        }
    }
  else if (stackResut & WFT_ARRAY_MASK)
    {
      cout << wcmd_decode_typeinfo (stackResut) << endl;

      if (! fetch_execution_array_result (hnd,
                                          stackResut & ~WFT_ARRAY_MASK,
                                          WIGNORE_FIELD,
                                          WIGNORE_ROW))
        {
          return false;
        }
      cout << endl;
    }
  else
    {
      cout << wcmd_decode_typeinfo (stackResut & ~WFT_ARRAY_MASK) << endl;
      if ( ! fetch_execution_simple_result (hnd,
                                            stackResut,
                                            WIGNORE_FIELD,
                                            WIGNORE_ROW))
        {
          return false;
        }
      cout << endl;
    }

  assert (wcs == WCS_OK);

  return true;

fetch_result_fail:

  assert (wcs != WCS_OK);

  cout << "Failed to send procedure parameters:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}

bool
cmdExec (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  const VERBOSE_LEVEL level       = GetVerbosityLevel ();
  size_t              linePos     = 0;
  string              token       = CmdLineNextToken (cmdLine, linePos);
  W_CONNECTOR_HND     conHdl      = NULL;
  bool                result      = false;

  WTICKS paramTicks, execTicks, fetchTicks;

  assert (token == "exec");

  const string procName = CmdLineNextToken (cmdLine, linePos);
  if (procName.length () == 0)
    {
      cout << "Invalid command format. The procedure name is missing.\n";
      return false;
    }

  D_UINT32 cs  = WConnect (GetRemoteHostName ().c_str (),
                           GetConnectionPort ().c_str (),
                           GetWorkingDB ().c_str (),
                           GetUserPassword ().c_str (),
                           GetUserId (),
                           &conHdl);

  if (cs != WCS_OK)
    {
      cout << "Failed to connect: " << wcmd_translate_status (cs) << endl;
      return false;
    }

  paramTicks = wh_msec_ticks ();
  if (! handle_procedure_parameters (conHdl, cmdLine, linePos))
    {
      if (level >= VL_DEBUG)
        cout << "Failed to parse procedure parameters.\n";

      goto cmd_exec_err;
    }
  paramTicks = wh_msec_ticks () - paramTicks;

  execTicks = wh_msec_ticks ();
  if ((cs = WExecuteProcedure (conHdl, procName.c_str ())) != WCS_OK)
    {
      if (level >= VL_DEBUG)
        cout << "Failed to execute remote procedure '" << procName << "': ";

      cout << wcmd_translate_status (cs) << endl;
      goto cmd_exec_err;
    }
  execTicks = wh_msec_ticks () - execTicks;

  fetchTicks = wh_msec_ticks ();
  if (! fetch_execution_result (conHdl))
    {
      if (level >= VL_DEBUG)
        {
          cout << "\nFailed to fetch the execution result for '";
          cout << procName << "'.\n";
        }

        goto cmd_exec_err;
    }
  fetchTicks = wh_msec_ticks () - fetchTicks;

  if  (level >= VL_INFO)
    {
      cout << "Send parameters time : " << paramTicks / 1000 << '.';
      cout.width (3); cout.fill ('0');
      cout << right << paramTicks % 1000<< "s.\n";

      cout << "Procedure exec time  : " << execTicks / 1000 << '.';
      cout.width (3); cout.fill ('0');
      cout << right << execTicks % 1000<< "s.\n";


      cout << "Procedure fetch time : " << fetchTicks / 1000 << '.';
      cout.width (3); cout.fill ('0');
      cout << right << fetchTicks % 1000<< "s.\n";

      WTICKS totalTicks = paramTicks + fetchTicks + execTicks;

      cout << "---\n";
      cout << "Total time          : " << totalTicks / 1000 << '.';
      cout.width (3); cout.fill ('0');
      cout << right << totalTicks % 1000<< "s.\n";
    }

  result = true;

cmd_exec_err:

  WClose (conHdl);

  return result;
}
