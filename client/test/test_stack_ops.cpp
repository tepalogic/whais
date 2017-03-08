/*
 * test_stack_ops.cpp
 *
 *  Created on: Jan 18, 2013
 *      Author: ipopa
 */

#include <iostream>
#include <cstring>

#include "test_client_common.h"

using namespace std;


uint_t simpleTypes[] =
    {
        WHC_TYPE_BOOL,
        WHC_TYPE_CHAR,
        WHC_TYPE_DATE,
        WHC_TYPE_DATETIME,
        WHC_TYPE_HIRESTIME,
        WHC_TYPE_INT8,
        WHC_TYPE_INT16,
        WHC_TYPE_INT32,
        WHC_TYPE_INT64,
        WHC_TYPE_UINT8,
        WHC_TYPE_UINT16,
        WHC_TYPE_UINT32,
        WHC_TYPE_UINT64,
        WHC_TYPE_REAL,
        WHC_TYPE_RICHREAL,
        WHC_TYPE_TEXT,

        WHC_TYPE_ARRAY_MASK | WHC_TYPE_BOOL,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_CHAR,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATE,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATETIME,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_HIRESTIME,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT8,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT16,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT32,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT64,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT8,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT16,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT32,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT64,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_REAL,
        WHC_TYPE_ARRAY_MASK | WHC_TYPE_RICHREAL,
//         WHC_TYPE_ARRAY_MASK | WHC_TYPE_TEXT,
    };

const uint_t simpleTypesSize = sizeof(simpleTypes) / sizeof(simpleTypes[0]);

WField tableFields[] =
    {
        {"bool_field", WHC_TYPE_BOOL},
        {"char_field", WHC_TYPE_CHAR},
        {"date_field", WHC_TYPE_DATE},
        {"datetime_field", WHC_TYPE_DATETIME},
        {"hirestime_field", WHC_TYPE_HIRESTIME},
        {"int8_field", WHC_TYPE_INT8},
        {"int16_field", WHC_TYPE_INT16},
        {"int32_field", WHC_TYPE_INT32},
        {"int64_field", WHC_TYPE_INT64},
        {"uint8_field", WHC_TYPE_UINT8},
        {"uint16_field", WHC_TYPE_UINT16},
        {"uint32_field", WHC_TYPE_UINT32},
        {"uint64_field", WHC_TYPE_UINT64},
        {"real_field", WHC_TYPE_REAL},
        {"richreal_field", WHC_TYPE_RICHREAL},
        {"text_field", WHC_TYPE_TEXT},

        {"array_bool_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_BOOL},
        {"array_char_field",  WHC_TYPE_ARRAY_MASK | WHC_TYPE_CHAR},
        {"array_date_field",  WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATE},
        {"array_datetime_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATETIME},
        {"array_hirestime_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_HIRESTIME},
        {"array_int8_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT8},
        {"array_int16_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT16},
        {"array_int32_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT32},
        {"array_int64_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT64},
        {"array_uint8_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT8},
        {"array_uint16_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT16},
        {"array_uint32_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT32},
        {"array_uint64_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT64},
        {"array_real_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_REAL},
        {"array_richreal_field", WHC_TYPE_ARRAY_MASK | WHC_TYPE_RICHREAL}
    };

const uint_t tableFieldsSize = sizeof(tableFields) / sizeof(tableFields[0]);

static uint_t
get_field_entry_index(const char*                  field,
                       const WField*       fields,
                       const uint_t                   fieldsSize)
{
  for (uint_t i = 0; i < fieldsSize; ++i)
    {
      if (strcmp(fields[i].name, field) == 0)
        return i;
    }

  return fieldsSize + 1;
}

static bool
match_table_fields_match(WH_CONNECTION               hnd,
                         const WField*       fields,
                         const uint_t                   fieldsSize)
{
  uint_t visitedFields = 0;
  uint_t fieldsCount;
  uint_t topType;

  if ((WDescribeStackTop(hnd, &topType) != WCS_OK)
      || (topType != WHC_TYPE_TABLE_MASK)
      || (WValueFieldsCount(hnd, &fieldsCount) != WCS_OK)
      || (fieldsSize != fieldsCount))
    {
      return false;
    }

  for (uint_t i = 0; i < fieldsSize; ++i)
    {
      const char* fieldName;
      uint_t        fieldType;

      if (WValueFetchField(hnd, &fieldName, &fieldType) != WCS_OK)
        return false;

      const uint_t index = get_field_entry_index(fieldName,
                                                  fields,
                                                  fieldsSize);
      if (index > fieldsSize)
        return false;

      if (fieldType != fields[index].type)
        return false;

      if (GET_BIT(visitedFields, index) != 0)
        return false;

      SET_BIT(visitedFields, index);
    }

  for (uint_t i = 0; i < fieldsSize; ++i)
    {
      if (GET_BIT(visitedFields, i) == 0)
        return false;
    }

  return true;
}


static bool
test_stack_bulk_update(WH_CONNECTION hnd)
{
  cout << "Testing stack bulk update ... ";

  if (WPushValue(hnd, WHC_TYPE_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      goto test_stack_bulk_update_err;

  for (uint_t i = 0; i < simpleTypesSize; ++i)
    {
      if (WPushValue(hnd, simpleTypes[i], 0, nullptr) != WCS_OK)
        goto test_stack_bulk_update_err;
    }

  if (WPushValue(hnd, WHC_TYPE_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      goto test_stack_bulk_update_err;

  if (WFlush(hnd) != WCS_OK)
      goto test_stack_bulk_update_err;

  if ((match_table_fields_match(hnd, tableFields, tableFieldsSize) == false)
      || (WPopValues(hnd, 1) != WCS_OK)
      || (WFlush(hnd) != WCS_OK))
    {
      goto test_stack_bulk_update_err;
    }

  for (int i = simpleTypesSize - 1; i >= 0; --i)
    {
      uint_t    type;
      uint_t    fieldsCount;

      if ((WDescribeStackTop(hnd, &type) != WCS_OK)
           || (type != simpleTypes[i])
           || (WValueFieldsCount(hnd, &fieldsCount) != WCS_OK)
           || (fieldsCount != 0)
           || (WPopValues(hnd, 1) != WCS_OK)
           || (WFlush(hnd) != WCS_OK))
        {
          goto test_stack_bulk_update_err;
        }
    }

  if ((match_table_fields_match(hnd, tableFields, tableFieldsSize) == false)
      || (WPopValues(hnd, 1) != WCS_OK)
      || (WFlush(hnd) != WCS_OK))
    {
      goto test_stack_bulk_update_err;
    }

  cout << "OK\n";
  return true;

test_stack_bulk_update_err:

  cout << "FAIL\n";
  return false;
}

static bool
test_stack_step_update(WH_CONNECTION hnd)
{
  cout << "Testing stack update step by step ... ";

  if ((WPushValue(hnd, WHC_TYPE_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      || (WFlush(hnd) != WCS_OK)
      || (match_table_fields_match(hnd, tableFields, tableFieldsSize) == false))
    {
      goto test_stack_step_update_err;
    }

  for (uint_t i = 0; i < simpleTypesSize; ++i)
    {
      uint_t    type;
      uint_t    fieldsCount;

      if ((WPushValue(hnd, simpleTypes[i], 0, nullptr) != WCS_OK)
          || (WFlush(hnd) != WCS_OK))
        {
          goto test_stack_step_update_err;
        }
      else if ((WDescribeStackTop(hnd, &type) != WCS_OK)
               || (type != simpleTypes[i])
               || (WValueFieldsCount(hnd, &fieldsCount) != WCS_OK)
               || (fieldsCount != 0))
        {
          goto test_stack_step_update_err;
        }
    }

  if ((WPushValue(hnd, WHC_TYPE_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      || (WFlush(hnd) != WCS_OK)
      || (match_table_fields_match(hnd, tableFields, tableFieldsSize) == false))
    {
      goto test_stack_step_update_err;
    }

  if ((WPopValues(hnd, ~0) != WCS_OK)
      || (WFlush(hnd) != WCS_OK))
    {
      goto test_stack_step_update_err;
    }

  cout << "OK\n";
  return true;

test_stack_step_update_err:
  cout << "FAIL\n";
  return false;
}

static bool
test_for_errors(WH_CONNECTION hnd)
{
  uint_t type;
  WField invalid1 = { "", WHC_TYPE_ARRAY_MASK | WHC_TYPE_BOOL };
  WField invalid2 = { "name", WHC_TYPE_FIELD_MASK | WHC_TYPE_INT8} ;

  cout << "Testing against error conditions ... ";

  if ((WPushValue(nullptr, WHC_TYPE_REAL, 0, nullptr) != WCS_INVALID_ARGS)
      || (WPushValue(hnd, WHC_TYPE_ARRAY_MASK | WHC_TYPE_NOTSET, 0, nullptr) != WCS_INVALID_ARGS)
      || (WPushValue(hnd, WHC_TYPE_FIELD_MASK | WHC_TYPE_NOTSET, 0, nullptr) != WCS_INVALID_ARGS)
      || (WPushValue(hnd, WHC_TYPE_TABLE_MASK , 0, tableFields) != WCS_INVALID_ARGS)
      || (WPushValue(hnd, WHC_TYPE_TABLE_MASK, 1, &invalid1) != WCS_INVALID_ARGS)
      || (WPushValue(hnd, WHC_TYPE_TABLE_MASK, 1, &invalid2) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WDescribeStackTop(nullptr, nullptr) != WCS_INVALID_ARGS)
            || (WDescribeStackTop(nullptr, &type) != WCS_INVALID_ARGS)
            || (WDescribeStackTop(hnd, nullptr) != WCS_INVALID_ARGS))
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
DefaultDatabaseName()
{
  return "test_list_db";
}

const uint_t
DefaultUserId()
{
  return 1;
}

const char*
DefaultUserPassword()
{
  return "test_password";
}

int
main(int argc, const char** argv)
{
  WH_CONNECTION hnd        = nullptr;


  bool success = tc_settup_connection(argc, argv, &hnd);

  success = success && test_for_errors(hnd);
  success = success && test_stack_step_update(hnd);
  success = success && test_stack_bulk_update(hnd);

  WClose(hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

