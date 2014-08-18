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
  const uint_t  type;
  const char* value;
};

BasicValueEntry _values[] =
    {
        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

        /* Repeat */

        {WHC_TYPE_BOOL, "1"},
        {WHC_TYPE_CHAR, "\xC2\xA9"},
        {WHC_TYPE_DATE, "2013/1/10"},
        {WHC_TYPE_DATETIME, "21/11/11 23:2:3"},
        {WHC_TYPE_HIRESTIME, "2013/2/28 19:1:27.4"},
        {WHC_TYPE_INT8, "-121"},
        {WHC_TYPE_INT16, "-8546"},
        {WHC_TYPE_INT32, "-742214963"},
        {WHC_TYPE_INT64, "-1233411244563200"},
        {WHC_TYPE_UINT8, "212"},
        {WHC_TYPE_UINT16, "54328"},
        {WHC_TYPE_UINT32, "3213965354"},
        {WHC_TYPE_UINT64, "923341124456320012"},
        {WHC_TYPE_REAL, "-1.12374"},
        {WHC_TYPE_RICHREAL, "538.154454223"},

    };

static const WField _fields[] =
{
  {"WHC_TYPE_BOOL", WHC_TYPE_BOOL},
  {"WHC_TYPE_CHAR", WHC_TYPE_CHAR},
  {"WHC_TYPE_DATE", WHC_TYPE_DATE},
  {"WHC_TYPE_DATETIME", WHC_TYPE_DATETIME},
  {"WHC_TYPE_HIRESTIME", WHC_TYPE_HIRESTIME},
  {"WHC_TYPE_INT8", WHC_TYPE_INT8},
  {"WHC_TYPE_INT16", WHC_TYPE_INT16},
  {"WHC_TYPE_INT32", WHC_TYPE_INT32},
  {"WHC_TYPE_INT64", WHC_TYPE_INT64},
  {"WHC_TYPE_UINT8", WHC_TYPE_UINT8},
  {"WHC_TYPE_UINT16", WHC_TYPE_UINT16},
  {"WHC_TYPE_UINT32", WHC_TYPE_UINT32},
  {"WHC_TYPE_UINT64", WHC_TYPE_UINT64},
  {"WHC_TYPE_REAL", WHC_TYPE_REAL},
  {"WHC_TYPE_RICHREAL", WHC_TYPE_RICHREAL}
};

static const uint_t _valuesCount = sizeof (_values) / sizeof (_values[0]);
static const uint_t _fieldsCount = sizeof (_fields) / sizeof (_fields[0]);

static const char*
get_table_field_name (const uint_t fieldType)
{
  for (uint_t i = 0; i < _fieldsCount; ++i)
    {
      if (fieldType == _fields[i].type)
        return _fields[i].name;
    }

  return NULL;
}


static bool
check_table_description (WH_CONNECTION hnd)
{
  uint_t type, fcount;

  const char* fieldName = NULL;
  uint_t        fieldType = 0;

  if ((WDescribeStackTop (hnd, &type) != WCS_OK)
      || (type != WHC_TYPE_TABLE_MASK)
      || (WValueFieldsCount (hnd, &fcount) != WCS_OK)
      || (fcount != _fieldsCount))
    {
      return false;
    }

  for (uint_t i = 0; i < _fieldsCount; ++i)
    {

      if (WValueFetchField (hnd, &fieldName, &fieldType) != WCS_OK)
        return false;

      if (strcmp (fieldName, get_table_field_name( fieldType)) != 0)
        return false;
    }

  if ((WValueFetchField (hnd, &fieldName, &fieldType) != WCS_OK)
      || (fieldName != NULL)
      || (fieldType != WHC_TYPE_NOTSET))
    {
      return false;
    }

  return true;
}


static bool
check_table_value (WH_CONNECTION      hnd,
                   const uint_t         index,
                   const bool           extraCheck)
{
  const char* const fieldName = get_table_field_name (_values[index].type);
  const char*       tabVal    = NULL;
  const uint64_t      row       = index / _fieldsCount;

  if (extraCheck)
    {
      if (WValueEntry (hnd,
                               "some_field_name",
                               row,
                               WIGNORE_OFF,
                               WIGNORE_OFF,
                               &tabVal) != WCS_INVALID_FIELD)
      {
        return false;
      }

      if (WValueEntry (hnd,
                               fieldName,
                               98012, //Some big row number
                               WIGNORE_OFF,
                               WIGNORE_OFF,
                               &tabVal) != WCS_INVALID_ROW)
      {
        return false;
      }

      if (WValueEntry (hnd,
                               fieldName,
                               row,
                               0,
                               WIGNORE_OFF,
                               &tabVal) != WCS_TYPE_MISMATCH)
        {
          return false;
        }

      if (WValueEntry (hnd,
                               fieldName,
                               row,
                               WIGNORE_OFF,
                               0,
                               &tabVal) != WCS_TYPE_MISMATCH)
        {
          return false;
        }
    }

  if (WValueEntry (hnd,
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
fill_table_with_values (WH_CONNECTION hnd,
                        const bool      bulk)
{
  if (WPushValue (hnd, WHC_TYPE_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
    return false;

  if ( ! bulk && (WFlush (hnd) != WCS_OK))
    return false;

  for (uint_t i = 0; i < _valuesCount; ++i)
    {
      if (WUpdateValue (hnd,
                             _values[i].type,
                             get_table_field_name (_values[i].type),
                             i / _fieldsCount,
                             WIGNORE_OFF,
                             WIGNORE_OFF,
                             _values[i].value) != WCS_OK)
        {
          goto fill_table_fail;
        }

      if ( ! bulk && (WFlush (hnd) != WCS_OK))
        goto fill_table_fail;
    }

  if (WFlush (hnd) != WCS_OK)
    return false;

  return check_table_description (hnd);

fill_table_fail:
  return false;
}

static bool
test_step_table_fill (WH_CONNECTION hnd)
{
  cout << "Testing filling a table with basic values (stepping mode)... ";

  if (! fill_table_with_values (hnd, false))
    goto test_step_table_fill_error;


  for (uint_t i = 0; i < _valuesCount; ++i)
    {
      if (! check_table_value (hnd, i, true))
        goto test_step_table_fill_error;
    }

  if ((WPopValues (hnd, WPOP_ALL) != WCS_OK)
      || (WFlush (hnd) !=WCS_OK))
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
test_bulk_table_fill (WH_CONNECTION hnd)
{
  cout << "Testing filling a table with basic values (bulk mode)... ";

  if (! fill_table_with_values (hnd, true))
    goto test_bulk_table_fill_error;

  for (uint_t i = 0; i < _valuesCount; ++i)
    {
      if (! check_table_value (hnd, i, false))
        goto test_bulk_table_fill_error;
    }

  if ((WPopValues (hnd, WPOP_ALL) != WCS_OK)
      || (WFlush (hnd) !=WCS_OK))
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
test_for_errors (WH_CONNECTION hnd)
{
  unsigned long long count;

  cout << "Testing against error conditions ... ";

  if ((WPushValue (hnd, WHC_TYPE_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
      || (WFlush (hnd) != WCS_OK)
      || (WFlush (hnd) != WCS_OK) //Just for fun!
      || (WValueRowsCount (NULL, NULL) != WCS_INVALID_ARGS)
      || (WValueRowsCount (NULL, &count) != WCS_INVALID_ARGS)
      || (WValueRowsCount (hnd, NULL) != WCS_INVALID_ARGS)
      || (WValueRowsCount (hnd, &count) != WCS_OK)
      || (count != 0)
      || (WValueArraySize (hnd, WIGNORE_FIELD, WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WValueArraySize (hnd, "some_f", WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WValueArraySize (hnd, "WHC_TYPE_BOOL", 0, &count) != WCS_INVALID_ROW)
      || (WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WValueTextLength (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WValueTextLength (hnd, "WHC_TYPE_BOOL", 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW)
      || (WPopValues (hnd, WPOP_ALL) != WCS_OK)
      || (WFlush (hnd) != WCS_OK))
    {
      goto test_for_errors_fail;
    }

  cout << "OK\n";
  return true;

test_for_errors_fail :
  cout << "FAIL\n";
  return false;
}

const char*
DefaultDatabaseName ()
{
  return "test_list_db";
}

const uint_t
DefaultUserId ()
{
  return 1;
}

const char*
DefaultUserPassword ()
{
  return "test_password";
}

int
main (int argc, const char** argv)
{
  WH_CONNECTION hnd        = NULL;

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


