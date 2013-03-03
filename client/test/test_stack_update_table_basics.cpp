/*
 * test_stack_update_table_basics.cpp
 *
 *  Created on: Feb 27, 2013
 *      Author: ipopa
 */
#include <iostream>
#include <cstring>

#include "test_client_common.h"

using namespace std;

struct BasicValueEntry
{
  const D_UINT  type;
  const D_CHAR* value;
};

BasicValueEntry _values[] =
    {
        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

        /* Repeat */

        {WFT_BOOL, "1"},
        {WFT_CHAR, "\xC2\xA9"},
        {WFT_DATE, "2013/1/10"},
        {WFT_DATETIME, "21/11/11 23:2:3"},
        {WFT_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WFT_INT8, "-121"},
        {WFT_INT16, "-8546"},
        {WFT_INT32, "-742214963"},
        {WFT_INT64, "-1233411244563200"},
        {WFT_UINT8, "212"},
        {WFT_UINT16, "54328"},
        {WFT_UINT32, "3213965354"},
        {WFT_UINT64, "923341124456320012"},
        {WFT_REAL, "-1.12374"},
        {WFT_RICHREAL, "538.154454223"},

    };

static const W_FieldDescriptor _fields[] =
{
  {"WFT_BOOL", WFT_BOOL},
  {"WFT_CHAR", WFT_CHAR},
  {"WFT_DATE", WFT_DATE},
  {"WFT_DATETIME", WFT_DATETIME},
  {"WFT_HIRESTIME", WFT_HIRESTIME},
  {"WFT_INT8", WFT_INT8},
  {"WFT_INT16", WFT_INT16},
  {"WFT_INT32", WFT_INT32},
  {"WFT_INT64", WFT_INT64},
  {"WFT_UINT8", WFT_UINT8},
  {"WFT_UINT16", WFT_UINT16},
  {"WFT_UINT32", WFT_UINT32},
  {"WFT_UINT64", WFT_UINT64},
  {"WFT_REAL", WFT_REAL},
  {"WFT_RICHREAL", WFT_RICHREAL}
};

static const D_UINT _valuesCount = sizeof (_values) / sizeof (_values[0]);
static const D_UINT _fieldsCount = sizeof (_fields) / sizeof (_fields[0]);

static const D_CHAR*
get_table_field_name (const D_UINT fieldType)
{
  for (D_UINT i = 0; i < _fieldsCount; ++i)
    {
      if (fieldType == _fields[i].m_FieldType)
        return _fields[i].m_FieldName;
    }

  return NULL;
}


static bool
check_table_description (W_CONNECTOR_HND hnd)
{
  D_UINT type, fcount;

  const D_CHAR* fieldName = NULL;
  D_UINT        fieldType = 0;

  if ((WDescribeStackTop (hnd, &type) != WCS_OK)
      || (type != WFT_TABLE_MASK)
      || (WDescribeValueGetFieldsCount (hnd, &fcount) != WCS_OK)
      || (fcount != _fieldsCount))
    {
      return false;
    }

  for (D_UINT i = 0; i < _fieldsCount; ++i)
    {

      if (WDescribeValueFetchField (hnd, &fieldName, &fieldType) != WCS_OK)
        return false;

      if (strcmp (fieldName, get_table_field_name( fieldType)) != 0)
        return false;
    }

  if ((WDescribeValueFetchField (hnd, &fieldName, &fieldType) != WCS_OK)
      || (fieldName != NULL)
      || (fieldType != WFT_NOTSET))
    {
      return false;
    }

  return true;
}


static bool
check_table_value (W_CONNECTOR_HND      hnd,
                   const D_UINT         index,
                   const bool           extraCheck)
{
  const D_CHAR* const fieldName = get_table_field_name (_values[index].type);
  const D_CHAR*       tabVal    = NULL;
  const D_UINT64      row       = index / _fieldsCount;

  if (extraCheck)
    {
      if (WGetStackValueEntry (hnd,
                               "some_field_name",
                               row,
                               WIGNORE_OFF,
                               WIGNORE_OFF,
                               &tabVal) != WCS_INVALID_FIELD)
      {
        return false;
      }

      if (WGetStackValueEntry (hnd,
                               fieldName,
                               98012, //Some big row number
                               WIGNORE_OFF,
                               WIGNORE_OFF,
                               &tabVal) != WCS_INVALID_ROW)
      {
        return false;
      }

      if (WGetStackValueEntry (hnd,
                               fieldName,
                               row,
                               0,
                               WIGNORE_OFF,
                               &tabVal) != WCS_TYPE_MISMATCH)
        {
          return false;
        }

      if (WGetStackValueEntry (hnd,
                               fieldName,
                               row,
                               WIGNORE_OFF,
                               0,
                               &tabVal) != WCS_TYPE_MISMATCH)
        {
          return false;
        }
    }

  if (WGetStackValueEntry (hnd,
                           fieldName,
                           row,
                           WIGNORE_OFF,
                           WIGNORE_OFF,
                           &tabVal) != WCS_OK)
    {
      return false;
    }

  if (strcmp (tabVal, _values[index].value) != 0)
    return false;

  return true;
}


static bool
fill_table_with_values (W_CONNECTOR_HND hnd,
                        const bool      bulk)
{
  if (WPushStackValue (hnd, WFT_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
    return false;

  if ( ! bulk && (WUpdateStackFlush (hnd) != WCS_OK))
    return false;

  for (D_UINT i = 0; i < _valuesCount; ++i)
    {
      if (WUpdateStackValue (hnd,
                             _values[i].type,
                             get_table_field_name (_values[i].type),
                             i / _fieldsCount,
                             WIGNORE_OFF,
                             WIGNORE_OFF,
                             _values[i].value) != WCS_OK)
        {
          goto fill_table_fail;
        }

      if ( ! bulk && (WUpdateStackFlush (hnd) != WCS_OK))
        goto fill_table_fail;
    }

  if (WUpdateStackFlush (hnd) != WCS_OK)
    return false;

  return check_table_description (hnd);

fill_table_fail:
  return false;
}

static bool
test_step_table_fill (W_CONNECTOR_HND hnd)
{
  cout << "Testing filling a table with basic values (stepping mode)... ";

  if (! fill_table_with_values (hnd, false))
    goto test_step_table_fill_error;


  for (D_UINT i = 0; i < _valuesCount; ++i)
    {
      if (! check_table_value (hnd, i, true))
        goto test_step_table_fill_error;
    }

  if ((WPopStackValues (hnd, WPOP_ALL) != WCS_OK)
      || (WUpdateStackFlush (hnd) !=WCS_OK))
    {
      goto test_step_table_fill_error;
    }

  cout << "OK\n";
  return true;

test_step_table_fill_error:

  cout << "FAIL\n";
  return false;
}


static bool
test_bulk_table_fill (W_CONNECTOR_HND hnd)
{
  cout << "Testing filling a table with basic values (bulk mode)... ";

  if (! fill_table_with_values (hnd, true))
    goto test_bulk_table_fill_error;

  for (D_UINT i = 0; i < _valuesCount; ++i)
    {
      if (! check_table_value (hnd, i, false))
        goto test_bulk_table_fill_error;
    }

  if ((WPopStackValues (hnd, WPOP_ALL) != WCS_OK)
      || (WUpdateStackFlush (hnd) !=WCS_OK))
    {
      goto test_bulk_table_fill_error;
    }

  cout << "OK\n";
  return true;

test_bulk_table_fill_error:

  cout << "FAIL\n";
  return false;
}



static bool
test_for_errors (W_CONNECTOR_HND hnd)
{
  unsigned long long count;

  cout << "Testing against error conditions ... ";

  if ((WPushStackValue (hnd, WFT_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK) //Just for fun!
      || (WGetStackValueRowsCount (NULL, NULL) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (NULL, &count) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (hnd, NULL) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (hnd, &count) != WCS_OK)
      || (count != 0)
      || (WGetStackArrayElementsCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WGetStackArrayElementsCount (hnd, "some_f", WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WGetStackArrayElementsCount (hnd, "WFT_BOOL", 0, &count) != WCS_INVALID_ROW)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WGetStackTextLengthCount (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WGetStackTextLengthCount (hnd, "WFT_BOOL", 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW)
      || (WPopStackValues (hnd, WPOP_ALL) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
    {
      goto test_for_errors_fail;
    }

  cout << "OK\n";
  return true;

test_for_errors_fail :
  cout << "FAIL\n";
  return false;
}

const D_CHAR*
DefaultDatabaseName ()
{
  return "test_list_db";
}

const D_UINT
DefaultUserId ()
{
  return 1;
}

const D_CHAR*
DefaultUserPassword ()
{
  return "test_password";
}

int
main (int argc, const char** argv)
{
  W_CONNECTOR_HND hnd        = NULL;

  bool success = tc_settup_connection (argc, argv, &hnd);

  success = success && test_for_errors (hnd);
  success = success && test_step_table_fill (hnd);
  success = success && test_bulk_table_fill (hnd);

  WClose (hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}


