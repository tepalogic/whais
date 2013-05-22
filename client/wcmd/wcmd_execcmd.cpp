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

#include "utils/utf8.h"
#include "client/whisper_connector.h"

#include "wcmd_execcmd.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_onlinecmds.h"
#include "wcmd_optglbs.h"

using namespace std;


static const char     NULL_VALUE[]   = "";
static const char     NULL_LABEL[]  = "(NULL)";

typedef map<uint_t, vector<string> > FIELD_ENTRY;
typedef FIELD_ENTRY::iterator        FIELD_ENTRY_IT;

struct FieldEntry
{
  FieldEntry (const uint_t fieldType = WHC_TYPE_NOTSET)
    : mValues (),
      mType (fieldType)
  {
  }

  bool
  SetValue (const uint_t           row,
            const uint16_t         type,
            const string&          value)
  {
    if ((mType != WHC_TYPE_NOTSET) && (type != mType))
      return false;

    mType = type;
    mValues[row].push_back (value);

    return true;
  }

  FIELD_ENTRY mValues;
  uint16_t    mType;
};

typedef map<string, FieldEntry> FIELD_VALUE;
typedef FIELD_VALUE::iterator   FIELD_VALUE_IT;

struct TableParameter
{
  TableParameter ()
    : mFields (),
      mRowsCount (0)
  {
  }

  bool
  SetValue (const string&          field,
            const uint_t           row,
            const uint_t           type,
            const string&          value)
  {
    if (mRowsCount < (row + 1))
      mRowsCount = row + 1;

    if (! mFields[field].SetValue (row, type, value))
      {
        cout << "Invalid command format.";
        cout << "Field '" << field << "' has different types values.\n";

        return false;
      }

    return true;
  }

  FIELD_VALUE  mFields;
  uint_t       mRowsCount;
};

uint_t
update_stack_value (const WH_CONNECTION         hnd,
                    const unsigned int            type,
                    const char* const           fieldName,
                    const WHT_ROW_INDEX       row,
                    const WHT_INDEX        arrayOff,
                    const char* const           value)
{
  /* When the values was created on the stack,
   * it already has value set to NULL. We need a was to specify NULL values
   * so don't pass this connector because it will treated as an error. */
  if (strcmp (value, NULL_VALUE) == 0)
    return WCS_OK;

  uint_t wcs = WCS_OK;

  if (type != WHC_TYPE_TEXT)
    {
      return WUpdateValue (hnd,
                                type,
                                fieldName,
                                row,
                                arrayOff,
                                WIGNORE_OFF,
                                value);
    }

  assert (arrayOff == WIGNORE_OFF);

  //Send one character at a time to avoid large parameter errors.
  uint_t       utf8TextOff  = 0;
  const uint_t utf8ValueLen = wh_utf8_strlen (_RC (const uint8_t*, value));
  uint_t       valueOff     = 0;
  while ((utf8TextOff < utf8ValueLen)
         && (wcs == WCS_OK))
    {
      assert (valueOff < strlen (value));

      const uint_t charSize = wh_utf8_cu_count (value[valueOff]);

      char tempBuffer[UTF8_MAX_BYTES_COUNT];
      memcpy (tempBuffer, value + valueOff, charSize);

      tempBuffer[charSize]  = 0;
      valueOff             += charSize;

      assert (valueOff <= strlen (value));

      wcs = WUpdateValue (hnd,
                               WHC_TYPE_TEXT,
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
  const char* const line = cmdLine.c_str ();

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
              uint64_t charCode = 0;
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

              uint8_t utf8CodeUnits[UTF8_MAX_BYTES_COUNT];
              uint_t  utf8UnitsCount = wh_store_utf8_cp (charCode,
                                                         utf8CodeUnits);
              assert (utf8UnitsCount > 0);
              oValue.append (_RC (const char*, utf8CodeUnits),
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
            uint_t&             oType)
{
  const char* const line = cmdLine.c_str ();

  switch (line[ioLineOff++])
  {
  case      'B':
  case      'b':
    oType = WHC_TYPE_BOOL;
    break;

  case      'C':
  case      'c':
    oType = WHC_TYPE_CHAR;
    break;

  case      'D':
  case      'd':
    oType = WHC_TYPE_DATE;
    break;

  case      'H':
  case      'h':
    oType = WHC_TYPE_DATETIME;
    break;

  case      'M':
  case      'm':
    oType = WHC_TYPE_HIRESTIME;
    break;

  case      'I':
  case      'i':
    oType = WHC_TYPE_INT64;
    break;

  case      'U':
  case      'u':
    oType = WHC_TYPE_UINT64;
    break;

  case      'R':
  case      'r':
    oType = WHC_TYPE_RICHREAL;
    break;

  case      't':
  case      'T':
    oType = WHC_TYPE_TEXT;
    break;

  default:
    cout << "Invalid command format. Unexpected parameter specifier '"
         << line[ioLineOff - 1] <<"'.\n";

    return false;
  }

  return true;
}

static bool
handle_param_value (WH_CONNECTION           hnd,
                    const string&             cmdLine,
                    const uint_t              type,
                    const char* const       field,
                    const WHT_ROW_INDEX   row,
                    const WHT_INDEX    arrayOff,
                    size_t&                   ioLineOff)
{
  uint_t   wcs = WCS_OK;

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
handle_procedure_array_param (WH_CONNECTION           hnd,
                              const string&             cmdLine,
                              const uint_t              type,
                              const char* const       field,
                              const WHT_ROW_INDEX   row,
                              size_t&                   ioLineOff)
{
  const char* const line = cmdLine.c_str ();

  uint64_t arrayIndex = 0;
  uint_t   wcs        = WCS_OK;

  assert (type != WHC_TYPE_TEXT);
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
handle_procedure_table_param (WH_CONNECTION           hnd,
                              const string&             cmdLine,
                              size_t&                   ioLineOff)
{
  const char* const line       = cmdLine.c_str ();
  uint_t              wcs        = WCS_OK;
  uint_t              row        = 0;
  uint_t              type       = 0;
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
                  type       |= WHC_TYPE_ARRAY_MASK;
                  arrayValue  = true;

                  continue;
                }
              else
                {
                  assert (arrayValue);
                  assert (type & WHC_TYPE_ARRAY_MASK);
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

  vector<WField> fields;
  for (FIELD_VALUE_IT it = table.mFields.begin ();
       it != table.mFields.end ();
       ++it)
    {
      WField fd;

      fd.name = it->first.c_str ();
      fd.type = it->second.mType;

      fields.push_back (fd);
    }

  assert (fields.size () == table.mFields.size ());
  assert (fields.size () > 0);

  wcs = WPushValue (hnd, WHC_TYPE_TABLE_MASK, fields.size (), &fields[0]);
  if (wcs != WCS_OK)
    goto proc_param_connector_error;

  assert (table.mRowsCount > 0);
  for (uint_t row = 0; row < table.mRowsCount; ++row)
    {
      for (FIELD_VALUE_IT it = table.mFields.begin ();
           it != table.mFields.end ();
           ++it)
        {
          FIELD_ENTRY_IT entry = it->second.mValues.find (row);
          if (entry == it->second.mValues.end ())
            continue;

          if (it->second.mType & WHC_TYPE_ARRAY_MASK)
            {
              assert (entry->second.size () >= 1);

              for (uint_t arrayId = 0;
                   arrayId < entry->second.size ();
                   ++arrayId)
                {
                  wcs = update_stack_value (
                              hnd,
                              it->second.mType & ~WHC_TYPE_ARRAY_MASK,
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
                                        it->second.mType,
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
handle_procedure_parameters (WH_CONNECTION hnd,
                             const string&   cmdLine,
                             size_t&         ioLineOff)
{
  const char* const line = cmdLine.c_str ();
  uint_t              type = WHC_TYPE_NOTSET;
  uint_t              wcs  = WCS_OK;

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
              wcs = WPushValue (hnd, type | WHC_TYPE_ARRAY_MASK, 0, NULL);
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
              wcs = WPushValue (hnd, type, 0, NULL);
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

  wcs = WFlush (hnd);
  if (wcs == WCS_OK)
    return true;

proc_param_connector_error:
  assert (wcs != WCS_OK);

  cout << "Failed to send procedure parameters:\n";
  cout << wcmd_translate_status (wcs) << endl;

  return false;
}

static bool
fetch_execution_simple_result (WH_CONNECTION         hnd,
                               const uint_t            type,
                               const char* const       field,
                               const uint64_t          row)

{
  uint_t         wcs = WCS_OK;
  const char*  retValue;

  if (type != WHC_TYPE_TEXT)
    {
      wcs = WValueEntry (hnd,
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
      uint64_t offset  = 0;
      wcs = WValueTextLength (hnd, field, row, WIGNORE_OFF, &length);
      if (wcs != WCS_OK)
        goto fetch_result_fail;

      if (length > 0)
        {
          cout << '\'';
          while (offset < length)
            {
              wcs = WValueEntry (hnd,
                                        field,
                                        row,
                                        WIGNORE_OFF,
                                        offset,
                                        &retValue);
              if (wcs != WCS_OK)
                goto fetch_result_fail;

              offset += wh_utf8_strlen (_RC (const uint8_t*, retValue));

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
fetch_execution_array_result (WH_CONNECTION        hnd,
                              const uint_t           type,
                              const char* const      field,
                              const uint64_t         row)

{
  assert ((type >= WHC_TYPE_BOOL) && (type < WHC_TYPE_TEXT));

  const char*         retValue;
  unsigned long long              count;
  uint_t                wcs;

  if ((wcs = WValueArraySize (hnd, field, row, &count)) != WCS_OK)
    goto fetch_result_fail;

  if (count > 0)
    {
      for (uint64_t i = 0; i < count; ++i)
        {
          wcs = WValueEntry (hnd, field, row, i, WIGNORE_OFF, &retValue);
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
fetch_execution_field_result (WH_CONNECTION hnd, const uint_t type)
{
  unsigned long long rowsCount = 0;
  uint_t   wcs       = WCS_OK;

  if ((wcs = WValueRowsCount (hnd, &rowsCount)) != WCS_OK)
    goto fetch_result_fail;

  if (rowsCount > 0)
    {
      for (uint64_t row = 0; row < rowsCount; ++row)
        {
          cout << row << " | ";
          if (type & WHC_TYPE_ARRAY_MASK)
            {
              if ( ! fetch_execution_array_result (hnd,
                                                   type & ~WHC_TYPE_ARRAY_MASK,
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
fetch_execution_table_result (WH_CONNECTION hnd,
                              vector<WField>& fields)
{
  unsigned long long rowsCount = 0;
  uint_t   wcs       = WCS_OK;

  if ((wcs = WValueRowsCount (hnd, &rowsCount)) != WCS_OK)
    goto fetch_result_fail;

  if (rowsCount > 0)
    {
      for (uint64_t row = 0; row < rowsCount; ++row)
        {
          for (uint_t i = 0; i < fields.size (); ++i )
            {
              cout << row << " | " << fields[i].name << " | ";
              if (fields[i].type & WHC_TYPE_ARRAY_MASK)
                {
                  if ( ! fetch_execution_array_result (
                                      hnd,
                                      fields[i].type & ~WHC_TYPE_ARRAY_MASK,
                                      fields[i].name,
                                      row
                                                       ))
                    {
                      return false;
                    }
                }
              else
                {
                  if ( ! fetch_execution_simple_result (hnd,
                                                        fields[i].type,
                                                        fields[i].name,
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
fetch_execution_result (WH_CONNECTION hnd)
{
  uint_t stackResut = WHC_TYPE_NOTSET;
  uint_t wcs        = WCS_OK;

  if ((wcs = WStackValueType (hnd, &stackResut)) != WCS_OK)
    goto fetch_result_fail;

  if (stackResut & WHC_TYPE_TABLE_MASK)
    {
      vector<string>            fields;
      vector<WField> fieldsDescriptors;
      uint_t                    fieldsCount;

      assert (stackResut == WHC_TYPE_TABLE_MASK);

      if ((wcs = WFieldsCount (hnd, &fieldsCount)) != WCS_OK)
        goto fetch_result_fail;

      assert (fieldsCount > 0);

      cout << "TABLE OF (";
      for (uint_t i = 0; i < fieldsCount; ++i)
        {
          WField     fd;
          const char*         fieldName;
          uint_t                fieldType;

          wcs = WFetchField (hnd, &fieldName, &fieldType);
          if (wcs != WCS_OK)
            goto fetch_result_fail;

          fields.push_back (fieldName);

          fd.name = fields.back ().c_str ();
          fd.type = fieldType;

          fieldsDescriptors.push_back (fd);

          if (i > 0)
            cout << ", ";
          cout << fd.name << " AS ";
          cout << wcmd_decode_typeinfo (fd.type);
        }
      cout << ")\n";

      assert (fieldsCount == fields.size ());
      assert (fieldsCount == fieldsDescriptors.size ());

      if ( !  fetch_execution_table_result (hnd, fieldsDescriptors))
        return false;

    }
  else if (stackResut & WHC_TYPE_FIELD_MASK)
    {
      cout << "FIELD OF ";
      cout << wcmd_decode_typeinfo (stackResut & ~WHC_TYPE_FIELD_MASK) << endl;

      if ( ! fetch_execution_field_result (hnd,
                                           stackResut & ~WHC_TYPE_FIELD_MASK))
        {
          return false;
        }
    }
  else if (stackResut & WHC_TYPE_ARRAY_MASK)
    {
      cout << wcmd_decode_typeinfo (stackResut) << endl;

      if (! fetch_execution_array_result (hnd,
                                          stackResut & ~WHC_TYPE_ARRAY_MASK,
                                          WIGNORE_FIELD,
                                          WIGNORE_ROW))
        {
          return false;
        }
      cout << endl;
    }
  else
    {
      cout << wcmd_decode_typeinfo (stackResut & ~WHC_TYPE_ARRAY_MASK) << endl;
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
  WH_CONNECTION     conHdl      = NULL;
  bool                result      = false;

  WTICKS paramTicks, execTicks, fetchTicks;

  assert (token == "exec");

  const string procName = CmdLineNextToken (cmdLine, linePos);
  if (procName.length () == 0)
    {
      cout << "Invalid command format. The procedure name is missing.\n";
      return false;
    }

  uint32_t cs  = WConnect (GetRemoteHostName ().c_str (),
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
