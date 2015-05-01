/******************************************************************************
  WCMD - An utility to manage whais database files.
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
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <cstring>

#include "utils/wutf.h"
#include "utils/valtranslator.h"
#include "client/whais_connector.h"

#include "wcmd_execcmd.h"
#include "wcmd_cmdsmgr.h"
#include "wcmd_onlinecmds.h"
#include "wcmd_optglbs.h"


using namespace std;


typedef map<uint_t, vector<string> > FIELD_ENTRY;
typedef FIELD_ENTRY::iterator        FIELD_ENTRY_IT;



static const char     NULL_VALUE[]   = "";
static const char     NULL_LABEL[]   = "(null)";



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

  FIELD_ENTRY   mValues;
  uint16_t      mType;
};



typedef map<string, FieldEntry>      FIELD_VALUE;
typedef FIELD_VALUE::iterator        FIELD_VALUE_IT;



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
        cerr << "Invalid command format. ";
        cerr << "Field '" << field << "' has different types of values.\n";

        return false;
      }

    return true;
  }

  FIELD_VALUE  mFields;
  uint_t       mRowsCount;
};



static uint_t
update_stack_value (const WH_CONNECTION         hnd,
                    const unsigned int          type,
                    const char* const           fieldName,
                    const WHT_ROW_INDEX         row,
                    const WHT_INDEX             arrayOff,
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
parse_value (const string&    cmdLine,
             size_t&          inoutLineOff,
             string&          outValue)
{
  const char* const line = cmdLine.c_str ();

  assert (line[inoutLineOff - 1] == '\'');

  outValue.clear ();
  while (inoutLineOff < cmdLine.length ())
    {
      if (line[inoutLineOff] == '\'')
        {
          inoutLineOff++;
          return true;
        }

      if (line[inoutLineOff] == '\\')
        {
          if (++inoutLineOff >= cmdLine.length ())
            return false;

          switch (line[inoutLineOff++])
          {
          case 'f':
            outValue.push_back ('\f');
            break;

          case 'n':
            outValue.push_back ('\n');
            break;

          case 'r':
            outValue.push_back ('\r');
            break;

          case 't':
            outValue.push_back ('\t');
            break;

          case 'v':
            outValue.push_back ('\v');
            break;

          case '\\':
            outValue.push_back ('\\');
            break;

          case '\'':
            outValue.push_back ('\'');
            break;

          case 'u':
            {
              uint64_t charCode = 0;
              while (line[inoutLineOff] != '&')
                {
                  charCode *= 16;
                  if (('0' <= line[inoutLineOff])
                      && (line[inoutLineOff] <= '9'))
                    {
                      charCode += line[inoutLineOff] - '0';
                    }
                  else if (('a' <= line[inoutLineOff])
                           && (line[inoutLineOff] <= 'f'))
                    {
                      charCode += line[inoutLineOff] - 'a' + 10;
                    }
                  else if (('A' <= line[inoutLineOff])
                           && (line[inoutLineOff] <= 'F'))
                    {
                      charCode += line[inoutLineOff] - 'A' + 10;
                    }
                  else
                    {
                      cerr << "Invalid command format. ";
                      cerr << "Missing '&' to specify the end of the '\\u' ";
                      cerr << "character escape sequence.\n";

                      return false;
                    }

                  if (charCode > 0xFFFFFFFF)
                    {
                      cerr << "Invalid command format.";
                      cerr << "The value specified with the escape sequence ";
                      cerr << "'\\u' is not a valid Unicode point.\n";

                      return false;
                    }
                  ++inoutLineOff;
                }
              ++inoutLineOff;

              if (charCode == 0)
                {
                  cerr << "Invalid command format.";
                  cerr << "The outValue specified with the escape sequence ";
                  cerr << "'\\u' cannot be 0.\n";

                  return false;
                }

              uint8_t utf8CodeUnits[UTF8_MAX_BYTES_COUNT];

              const uint_t utf8UnitsCount = wh_store_utf8_cp (charCode,
                                                              utf8CodeUnits);
              assert (utf8UnitsCount > 0);

              outValue.append (_RC (const char*, utf8CodeUnits),
                               utf8UnitsCount);
            }
            break;

          default:
            cerr << "Invalid command format. Unknown escape sequence ";
            cerr << "'\\" << line[inoutLineOff - 1] << "'.\n";

            return false;
          }
        }
      else
        outValue.push_back (line[inoutLineOff++]);
    }

  cerr << "Invalid command format. Missing ' at the end of the outValue.\n";

  return false;
}


static bool
parse_type (const string&       cmdLine,
            size_t&             inoutLineOff,
            uint_t&             outType)
{
  const char* const line = cmdLine.c_str ();

  switch (line[inoutLineOff++])
  {
  case      'B':
  case      'b':
    outType = WHC_TYPE_BOOL;
    break;

  case      'C':
  case      'c':
    outType = WHC_TYPE_CHAR;
    break;

  case      'D':
  case      'd':
    outType = WHC_TYPE_DATE;
    break;

  case      'H':
  case      'h':
    outType = WHC_TYPE_DATETIME;
    break;

  case      'M':
  case      'm':
    outType = WHC_TYPE_HIRESTIME;
    break;

  case      'I':
  case      'i':
    if (strncmp (line + inoutLineOff, "8", 1) == 0)
      {
        inoutLineOff += 1;
        outType = WHC_TYPE_INT8;
      }
    else if (strncmp (line + inoutLineOff, "16", 2) == 0)
      {
        inoutLineOff += 2;
        outType = WHC_TYPE_INT16;
      }
    else if (strncmp (line + inoutLineOff, "32", 2) == 0)
      {
        inoutLineOff += 2;
        outType = WHC_TYPE_INT32;
      }
    else if (strncmp (line + inoutLineOff, "64", 2) == 0)
      {
        inoutLineOff += 2;
        outType = WHC_TYPE_INT64;
      }
    else
      {
        cerr << "Invalid command format. Unexpected parameter specifier '"
             << line[inoutLineOff - 1] <<"'.\n";
        return false;
      }
    break;

  case      'U':
  case      'u':
    if (strncmp (line + inoutLineOff, "8", 1) == 0)
      {
        inoutLineOff += 1;
        outType = WHC_TYPE_UINT8;
      }
    else if (strncmp (line + inoutLineOff, "16", 2) == 0)
      {
        inoutLineOff += 2;
        outType = WHC_TYPE_UINT16;
      }
    else if (strncmp (line + inoutLineOff, "32", 2) == 0)
      {
        inoutLineOff += 2;
        outType = WHC_TYPE_UINT32;
      }
    else if (strncmp (line + inoutLineOff, "64", 2) == 0)
      {
        inoutLineOff += 2;
        outType = WHC_TYPE_UINT64;
      }
    else
      {
        cerr << "Invalid command format. Unexpected parameter specifier '"
             << line[inoutLineOff - 1] <<"'.\n";
        return false;
      }
    break;

  case      'R':
  case      'r':
    if ((line[inoutLineOff] == 'r') || (line[inoutLineOff] == 'R'))
      {
        inoutLineOff += 1;
        outType       = WHC_TYPE_RICHREAL;
      }
    else
      outType = WHC_TYPE_REAL;

    break;

  case      't':
  case      'T':
    outType = WHC_TYPE_TEXT;
    break;

  default:
    cerr << "Invalid command format. Unexpected parameter specifier '"
         << line[inoutLineOff - 1] <<"'.\n";

    return false;
  }

  return true;
}


static bool
handle_param_value (WH_CONNECTION           hnd,
                    const string&           cmdLine,
                    const uint_t            type,
                    const char* const       field,
                    const WHT_ROW_INDEX     row,
                    const WHT_INDEX         arrayOff,
                    size_t&                 inoutLineOff)
{
  uint_t wcs = WCS_OK;
  string value;

  if (! parse_value (cmdLine, inoutLineOff, value))
    {
      if (arrayOff != WIGNORE_OFF)
        {
          cerr << "Invalid command format. "
                  "An array values was not specified properly.\n";
        }
      else
        {
          cerr << "Invalid command format. "
                  "A value was not specified properly.\n";
        }
      return false;
    }
  else if ((value.length () == 0) && (arrayOff != WIGNORE_OFF))
    {
      cerr << "Invalid command format. "
              "An array value cannot hold a null value (e.g. '').\n";

      return false;
    }

  wcs = update_stack_value (hnd, type, field, row, arrayOff, value.c_str ());
  if (wcs != WCS_OK)
    goto proc_param_connector_error;

  return true;

proc_param_connector_error:
  assert (wcs != WCS_OK);

  cerr << "Failed to send procedure parameters:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;
}


static bool
handle_procedure_array_param (WH_CONNECTION           hnd,
                              const string&           cmdLine,
                              const uint_t            type,
                              const char* const       field,
                              const WHT_ROW_INDEX     row,
                              size_t&                 inoutLineOff)
{
  const char* const line = cmdLine.c_str ();

  uint64_t arrayIndex = 0;

  assert (type != WHC_TYPE_TEXT);
  assert (line[inoutLineOff - 1] == '{');

  while (inoutLineOff < cmdLine.length ())
    {
      if (line[inoutLineOff] == '}')
        {
          ++inoutLineOff;

          return true;
        }

      if ((line[inoutLineOff] == ' ') || (line[inoutLineOff] == '\t'))
        {
          ++inoutLineOff;

          continue;
        }
      else if (line[inoutLineOff++] != '\'')
        {
          cerr << "Invalid command format. "
                   "An array element should begin with a '.\n";

          return false;
        }

      if (! handle_param_value (hnd,
                                cmdLine,
                                type,
                                field,
                                row,
                                arrayIndex++,
                                inoutLineOff))
        {
          return false;
        }
    }

  cerr << "Invalid command format. Missing '}' to mark the end of array.\n";

  return false;
}


static bool
handle_procedure_table_param (WH_CONNECTION       hnd,
                              const string&       cmdLine,
                              size_t&             inoutLineOff)
{
  const char* const   line         = cmdLine.c_str ();
  uint_t              wcs          = WCS_OK;
  uint_t              row          = 0;
  uint_t              type         = 0;
  string              field;
  string              value;
  TableParameter      table;
  bool                rowStarted   = false;
  bool                fieldParse   = false;
  bool                typeParse    = false;
  bool                valueParse   = false;
  bool                arrayValue   = false;
  bool                endOfEntry   = false;
  bool                ignoreSpaces = true;

  assert (line[inoutLineOff - 1] == '(');

  while (inoutLineOff < cmdLine.length ())
    {
      if (ignoreSpaces
          && ((line[inoutLineOff] == ' ') || (line[inoutLineOff] == '\t')))
        {
          inoutLineOff++;

          continue;
        }

      if (! rowStarted)
        {
          if (line[inoutLineOff] == ')')
            {
              endOfEntry = true;

              ++inoutLineOff;

              break;
            }
          else if (line[inoutLineOff++] != '[')
            {
              cerr << "Invalid command format. Needs a '[' to begin a row.\n";

              return false;
            }

          rowStarted = true;
          fieldParse = true;

          field.clear ();

          continue;
        }

      if (fieldParse)
        {
          ignoreSpaces = false;

          if (line[inoutLineOff] == '.')
            {
              ++inoutLineOff;

              fieldParse = false;
              typeParse  = true;

              if (field.length () == 0)
                {
                  cerr << "Invalid command format. "
                          "A non empty field has to be provided.\n";

                  return false;
                }
            }
          else if (line[inoutLineOff] == ']')
            {
              ++inoutLineOff;
              ++row;

              fieldParse   = false;
              rowStarted   = false;
              ignoreSpaces = true;
            }
          else
            {
              field.append (line + inoutLineOff, 1);
              inoutLineOff++;
            }

          continue;
        }

      if (typeParse)
        {
          if ( ! parse_type (cmdLine, inoutLineOff, type))
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

          if ((line[inoutLineOff] == '{') || arrayValue)
            {
              if (line[inoutLineOff] == '{')
                {
                  ++inoutLineOff;

                  type       |= WHC_TYPE_ARRAY_MASK;
                  arrayValue  = true;

                  continue;
                }
              else
                {
                  assert (arrayValue);
                  assert (type & WHC_TYPE_ARRAY_MASK);
                }

              if (line[inoutLineOff] == '}')
                {
                  inoutLineOff++;

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
              else if (line[inoutLineOff++] != '\'')
                {
                  cerr << "Invalid table parameter format. "
                            "Unexpected character '"
                       << line[inoutLineOff] << "\'.\n";

                  return false;
                }

              if ( ! parse_value (cmdLine, inoutLineOff, value))
                return false;

              if ( ! table.SetValue (field, row, type, value))
                return false;
            }
          else if (line[inoutLineOff] == '\'')
            {
              ++inoutLineOff;

              if ( ! parse_value (cmdLine, inoutLineOff, value))
                return false;

              if ( ! table.SetValue (field, row, type, value))
                return false;

              valueParse   = false;
              fieldParse   = true;

              field.clear ();
            }
          else
            {
              cerr << "Invalid table parameter format. Unexpected character '"
                   << line[inoutLineOff] << "\'.\n";

              return false;
            }
        }
    }

  if (! endOfEntry)
    {
      cerr << "Invalid command format. "
              "Table parameter value is not complete.\n";

      return false;
    }
  else if (table.mRowsCount <= 0)
    {
      cerr << "Invalid table parameter. Table is not defined correctly.\n";
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

  wcs = WAddTableRows (hnd, table.mRowsCount);
  if (wcs != WCS_OK)
    goto proc_param_connector_error;

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
                  wcs = update_stack_value(
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

  cerr << "Failed to send procedure parameters:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;
}


static bool
handle_procedure_parameters (WH_CONNECTION   hnd,
                             const string&   cmdLine,
                             size_t&         inoutLineOff)
{
  const char* const   line       = cmdLine.c_str ();
  uint_t              type       = WHC_TYPE_NOTSET;
  uint_t              wcs        = WCS_OK;
  bool                needsFlush = false;

  while ((inoutLineOff < cmdLine.length ())
         && (wcs == WCS_OK))
    {
      if ((line[inoutLineOff] == ' ') || (line[inoutLineOff] == '\t'))
        {
          ++inoutLineOff;

          continue;
        }

      if (line[inoutLineOff] == '(')
        {
          ++inoutLineOff;

          if ( ! handle_procedure_table_param (hnd, cmdLine, inoutLineOff))
            return false;

          needsFlush = true;
        }
      else
        {
          if (! parse_type (cmdLine, inoutLineOff, type))
            return false;

          while ((inoutLineOff < cmdLine.length ())
                 && (line[inoutLineOff] == ' ')
                 && (line[inoutLineOff] == '\t'))
            {
              ++inoutLineOff;

              continue;
            }

          if (inoutLineOff >= cmdLine.length ())
            {
              cerr << "Invalid command format. No values was specified.\n";

              return false;
            }
          else if (line[inoutLineOff] == '{')
            {
              wcs = WPushValue (hnd, type | WHC_TYPE_ARRAY_MASK, 0, NULL);
              if (wcs != WCS_OK)
                goto proc_param_connector_error;

              ++inoutLineOff;
              if (! handle_procedure_array_param (hnd,
                                                  cmdLine,
                                                  type,
                                                  WIGNORE_FIELD,
                                                  WIGNORE_ROW,
                                                  inoutLineOff))
                {
                  return false;
                }
              needsFlush = true;
            }
          else if (line[inoutLineOff] == '\'')
            {
              wcs = WPushValue (hnd, type, 0, NULL);
              if (wcs != WCS_OK)
                goto proc_param_connector_error;

              ++inoutLineOff;
              if (! handle_param_value (hnd,
                                        cmdLine,
                                        type,
                                        WIGNORE_FIELD,
                                        WIGNORE_ROW,
                                        WIGNORE_OFF,
                                        inoutLineOff))
                {
                  return false;
                }
              needsFlush = true;
            }
          else
            {
              cerr << "Invalid command format. Unexpected character '";
              cerr << line[inoutLineOff] << "\'.\n";

              return false;
            }
        }
    }

  if (needsFlush)
    wcs = WFlush (hnd);

  if (wcs == WCS_OK)
    return true;

proc_param_connector_error:
  assert (wcs != WCS_OK);

  cerr << "Failed to send procedure parameters:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;
}


static bool
fetch_execution_simple_result (WH_CONNECTION         hnd,
                               const uint_t          type,
                               const char* const     field,
                               const uint64_t        row)

{
  uint_t       wcs = WCS_OK;
  const char*  retValue;

  if (type != WHC_TYPE_TEXT)
    {
      wcs = WValueEntry (hnd, field, row, WIGNORE_OFF, WIGNORE_OFF, &retValue);
      if (wcs != WCS_OK)
        goto fetch_result_fail;

      if (strcmp (retValue, NULL_VALUE) == 0)
        cout << NULL_LABEL;

      else if (type == WHC_TYPE_CHAR)
        {
          uint32_t cp, cpLen;
          char temp[8];

          cpLen = wh_load_utf8_cp (_RC (const uint8_t*, retValue), &cp);

          assert (cpLen > 0);
          assert (cpLen == strlen (retValue));

          Utf8Translator::Write (_RC (uint8_t*, temp),
                                 sizeof temp,
                                 true,
                                 whais::DChar (cp));
          cout << '\'' << temp << '\'';
        }
      else
        cout << '\'' << retValue << '\'';
    }
  else
    {
      unsigned long long length  = 0;
      uint64_t           offset  = 0;

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

              for (size_t i = 0; i < strlen (retValue);)
                {
                  uint32_t cp, cpLen;
                  char temp[8];

                  cpLen = wh_load_utf8_cp (_RC (const uint8_t*, retValue + i),
                                           &cp);
                  assert (cpLen > 0);
                  assert ((i + cpLen) <= strlen (retValue));

                  Utf8Translator::Write (_RC (uint8_t*, temp),
                                         sizeof temp,
                                         true,
                                         whais::DChar (cp));
                  cout << temp;
                  i += cpLen;
                }
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

  cerr << "Failed to fetch result:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;
}


static bool
fetch_execution_array_result (WH_CONNECTION        hnd,
                              const uint_t         type,
                              const char* const    field,
                              const uint64_t       row)

{
  assert (((type >= WHC_TYPE_BOOL) && (type < WHC_TYPE_TEXT))
          || (type == T_UNDETERMINED));

  const char*           retValue;
  unsigned long long    count;
  uint_t                wcs;

  if ((wcs = WValueArraySize (hnd, field, row, &count)) != WCS_OK)
    goto fetch_result_fail;

  if (count > 0)
    {
      assert (type != T_UNDETERMINED);

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

  cerr << "Failed to fetch array result:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;

}


static bool
fetch_execution_field_result (WH_CONNECTION hnd, const uint_t type)
{
  unsigned long long rowsCount = 0;

  uint_t wcs = WCS_OK;

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
    cout << endl;

  return true;

fetch_result_fail:

  assert (wcs != WCS_OK);

  cerr << "Failed to fetch field results:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;
}


static bool
fetch_execution_table_result (WH_CONNECTION       hnd,
                              vector<WField>&     fields)
{
  unsigned long long rowsCount = 0;

  uint_t wcs = WCS_OK;

  uint_t longestField = 8;
  uint_t rowDigits    = 1;

  if ((wcs = WValueRowsCount (hnd, &rowsCount)) != WCS_OK)
    goto fetch_result_fail;

  if (rowsCount > 0)
    {
      for (size_t i = 0; i < fields.size (); ++i)
        {
          if (longestField < strlen (fields[i].name))
            longestField = strlen (fields[i].name);
        }

      for (ROW_INDEX check = rowDigits; check < rowsCount; ++rowDigits)
        check *= 10;

      for (uint64_t row = 0; row < rowsCount; ++row)
        {
          if (row == 0)
            {
              cout << left << setfill ('-') << setw (rowDigits + 2) << '+'
                   << setw (longestField + 1) << '+'
                   << setw (longestField * 2 + 1) << '+' << '+'
                   << setw(0) << setfill (' ') << endl;
            }
          for (uint_t i = 0; i < fields.size (); ++i )
            {
              cout << right << setw (rowDigits + 1) << row
                   << left << setw (0) << " |"
                   << setw ( longestField) << fields[i].name
                   << setw (0) << "| ";

              if (fields[i].type & WHC_TYPE_ARRAY_MASK)
                {
                  if ( ! fetch_execution_array_result(
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
          cout << left << setfill ('-') << setw (rowDigits + 2) << '+'
               << setw (longestField + 1) << '+'
               << setw (longestField * 2+ 1) << '+' << '+'
               << setw(0) << setfill (' ') << endl;
        }
    }
  else
    cout << NULL_LABEL << endl;

  return true;

fetch_result_fail:

  assert (wcs != WCS_OK);

  cerr << "Failed to fetch table result:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;
}


static bool
fetch_execution_result (WH_CONNECTION hnd)
{
  uint_t stackResut = WHC_TYPE_NOTSET;
  uint_t wcs        = WCS_OK;

  if ((wcs = WDescribeStackTop (hnd, &stackResut)) != WCS_OK)
    goto fetch_result_fail;

  if (stackResut & WHC_TYPE_TABLE_MASK)
    {
      vector<string>   fields;
      vector<WField>   fieldsDescriptors;
      uint_t           fieldsCount;

      assert (stackResut == WHC_TYPE_TABLE_MASK);

      if ((wcs = WValueFieldsCount (hnd, &fieldsCount)) != WCS_OK)
        goto fetch_result_fail;

      if (fieldsCount == 0)
        {
          cout << "TABLE\n";
          return true;
        }

      cout << "TABLE OF (";
      for (uint_t i = 0; i < fieldsCount; ++i)
        {
          WField        fd;
          const char*   fieldName;
          uint_t        fieldType;

          wcs = WValueFetchField (hnd, &fieldName, &fieldType);
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

  cerr << "Failed to send procedure parameters:\n";
  cerr << wcmd_translate_status (wcs) << endl;

  return false;
}


bool
cmdExec (const string& cmdLine, ENTRY_CMD_CONTEXT context)
{
  const VERBOSE_LEVEL level    = GetVerbosityLevel ();
  size_t              linePos  = 0;
  string              token    = CmdLineNextToken (cmdLine, linePos);
  WH_CONNECTION       conHdl   = NULL;
  bool                result   = false;

  WTICKS paramTicks, execTicks, fetchTicks;

  assert (token == "exec");

  const string procName = CmdLineNextToken (cmdLine, linePos);
  if (procName.length () == 0)
    {
      cerr << "Invalid command format. The procedure name is missing.\n";
      return false;
    }

  uint32_t cs  = WConnect (GetRemoteHostName ().c_str (),
                           GetConnectionPort ().c_str (),
                           GetWorkingDB ().c_str (),
                           GetUserPassword ().c_str (),
                           GetUserId (),
                           DEFAULT_FRAME_SIZE,
                           &conHdl);
  if (cs != WCS_OK)
    {
      cerr << "Failed to connect: " << wcmd_translate_status (cs) << endl;
      return false;
    }

  paramTicks = wh_msec_ticks ();
  if (! handle_procedure_parameters (conHdl, cmdLine, linePos))
    {
      if (level >= VL_DEBUG)
        cerr << "Failed to parse procedure parameters.\n";

      goto cmd_exec_err;
    }
  paramTicks = wh_msec_ticks () - paramTicks;

  execTicks = wh_msec_ticks ();
  if ((cs = WExecuteProcedure (conHdl, procName.c_str ())) != WCS_OK)
    {
      if (level >= VL_DEBUG)
        cerr << "Failed to execute remote procedure '" << procName << "': ";

      cerr << wcmd_translate_status (cs) << endl;
      goto cmd_exec_err;
    }
  execTicks = wh_msec_ticks () - execTicks;

  fetchTicks = wh_msec_ticks ();
  if (! fetch_execution_result (conHdl))
    {
      if (level >= VL_DEBUG)
        {
          cerr << "\nFailed to fetch the execution result for '";
          cerr << procName << "'.\n";
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
      cout << "Total time           : " << totalTicks / 1000 << '.';
      cout.width (3); cout.fill ('0');
      cout << right << totalTicks % 1000<< "s.\n";
    }

  result = true;

cmd_exec_err:

  WClose (conHdl);

  return result;
}

