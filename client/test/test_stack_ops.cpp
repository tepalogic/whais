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


D_UINT simpleTypes[] =
    {
        WFT_BOOL,
        WFT_CHAR,
        WFT_DATE,
        WFT_DATETIME,
        WFT_HIRESTIME,
        WFT_INT8,
        WFT_INT16,
        WFT_INT32,
        WFT_INT64,
        WFT_UINT8,
        WFT_UINT16,
        WFT_UINT32,
        WFT_UINT64,
        WFT_REAL,
        WFT_RICHREAL,
        WFT_TEXT,

        WFT_ARRAY_MASK | WFT_BOOL,
        WFT_ARRAY_MASK | WFT_CHAR,
        WFT_ARRAY_MASK | WFT_DATE,
        WFT_ARRAY_MASK | WFT_DATETIME,
        WFT_ARRAY_MASK | WFT_HIRESTIME,
        WFT_ARRAY_MASK | WFT_INT8,
        WFT_ARRAY_MASK | WFT_INT16,
        WFT_ARRAY_MASK | WFT_INT32,
        WFT_ARRAY_MASK | WFT_INT64,
        WFT_ARRAY_MASK | WFT_UINT8,
        WFT_ARRAY_MASK | WFT_UINT16,
        WFT_ARRAY_MASK | WFT_UINT32,
        WFT_ARRAY_MASK | WFT_UINT64,
        WFT_ARRAY_MASK | WFT_REAL,
        WFT_ARRAY_MASK | WFT_RICHREAL,
//         WFT_ARRAY_MASK | WFT_TEXT,
    };

const D_UINT simpleTypesSize = sizeof (simpleTypes) / sizeof (simpleTypes[0]);

W_FieldDescriptor tableFields[] =
    {
        {"bool_field", WFT_BOOL},
        {"char_field", WFT_CHAR},
        {"date_field", WFT_DATE},
        {"datetime_field", WFT_DATETIME},
        {"hirestime_field", WFT_HIRESTIME},
        {"int8_field", WFT_INT8},
        {"int16_field", WFT_INT16},
        {"int32_field", WFT_INT32},
        {"int64_field", WFT_INT64},
        {"uint8_field", WFT_UINT8},
        {"uint16_field", WFT_UINT16},
        {"uint32_field", WFT_UINT32},
        {"uint64_field", WFT_UINT64},
        {"real_field", WFT_REAL},
        {"richreal_field", WFT_RICHREAL},
        {"text_field", WFT_TEXT},

        {"array_bool_field", WFT_ARRAY_MASK | WFT_BOOL},
        {"array_char_field",  WFT_ARRAY_MASK | WFT_CHAR},
        {"array_date_field",  WFT_ARRAY_MASK | WFT_DATE},
        {"array_datetime_field", WFT_ARRAY_MASK | WFT_DATETIME},
        {"array_hirestime_field", WFT_ARRAY_MASK | WFT_HIRESTIME},
        {"array_int8_field", WFT_ARRAY_MASK | WFT_INT8},
        {"array_int16_field", WFT_ARRAY_MASK | WFT_INT16},
        {"array_int32_field", WFT_ARRAY_MASK | WFT_INT32},
        {"array_int64_field", WFT_ARRAY_MASK | WFT_INT64},
        {"array_uint8_field", WFT_ARRAY_MASK | WFT_UINT8},
        {"array_uint16_field", WFT_ARRAY_MASK | WFT_UINT16},
        {"array_uint32_field", WFT_ARRAY_MASK | WFT_UINT32},
        {"array_uint64_field", WFT_ARRAY_MASK | WFT_UINT64},
        {"array_real_field", WFT_ARRAY_MASK | WFT_REAL},
        {"array_richreal_field", WFT_ARRAY_MASK | WFT_RICHREAL}
    };

const D_UINT tableFieldsSize = sizeof (tableFields) / sizeof (tableFields[0]);

static D_UINT
get_field_entry_index (const D_CHAR*                  field,
                       const W_FieldDescriptor*       fields,
                       const D_UINT                   fieldsSize)
{
  for (D_UINT i = 0; i < fieldsSize; ++i)
    {
      if (strcmp (fields[i].m_FieldName, field) == 0)
        return i;
    }

  return fieldsSize + 1;
}

static bool
match_table_fields_match (W_CONNECTOR_HND               hnd,
                         const W_FieldDescriptor*       fields,
                         const D_UINT                   fieldsSize)
{
  D_UINT visitedFields = 0;
  D_UINT fieldsCount;
  D_UINT topType;

  if ((WDescribeStackTop (hnd, &topType) != WCS_OK)
      || (topType != WFT_TABLE_MASK)
      || (WDescribeValueGetFieldsCount (hnd, &fieldsCount) != WCS_OK)
      || (fieldsSize != fieldsCount))
    {
      return false;
    }

  for (D_UINT i = 0; i < fieldsSize; ++i)
    {
      const D_CHAR* fieldName;
      D_UINT        fieldType;

      if (WDescribeValueFetchField (hnd, &fieldName, &fieldType) != WCS_OK)
        return false;

      const D_UINT index = get_field_entry_index (fieldName,
                                                  fields,
                                                  fieldsSize);
      if (index > fieldsSize)
        return false;

      if (fieldType != fields[index].m_FieldType)
        return false;

      if (GET_BIT (visitedFields, index) != 0)
        return false;

      SET_BIT (visitedFields, index);
    }

  for (D_UINT i = 0; i < fieldsSize; ++i)
    {
      if (GET_BIT (visitedFields, i) == 0)
        return false;
    }

  return true;
}


static bool
test_stack_bulk_update (W_CONNECTOR_HND hnd)
{
  cout << "Testing stack bulk update ... ";

  if (WPushStackValue (hnd, WFT_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      goto test_stack_bulk_update_err;

  for (D_UINT i = 0; i < simpleTypesSize; ++i)
    {
      if (WPushStackValue(hnd, simpleTypes[i], 0, NULL) != WCS_OK)
        goto test_stack_bulk_update_err;
    }

  if (WPushStackValue (hnd, WFT_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      goto test_stack_bulk_update_err;

  if (WUpdateStackFlush (hnd) != WCS_OK)
      goto test_stack_bulk_update_err;

  if ((match_table_fields_match (hnd, tableFields, tableFieldsSize) == false)
      || (WPopStackValues (hnd, 1) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
    {
      goto test_stack_bulk_update_err;
    }

  for (D_INT i = simpleTypesSize - 1; i >= 0; --i)
    {
      D_UINT    type;
      D_UINT    fieldsCount;

      if ((WDescribeStackTop (hnd, &type) != WCS_OK)
           || (type != simpleTypes[i])
           || (WDescribeValueGetFieldsCount (hnd, &fieldsCount) != WCS_OK)
           || (fieldsCount != 0)
           || (WPopStackValues (hnd, 1) != WCS_OK)
           || (WUpdateStackFlush (hnd) != WCS_OK))
        {
          goto test_stack_bulk_update_err;
        }
    }

  if ((match_table_fields_match (hnd, tableFields, tableFieldsSize) == false)
      || (WPopStackValues (hnd, 1) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
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
test_stack_step_update (W_CONNECTOR_HND hnd)
{
  cout << "Testing stack update step by step ... ";

  if ((WPushStackValue (hnd, WFT_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK)
      || (match_table_fields_match (hnd, tableFields, tableFieldsSize) == false))
    {
      goto test_stack_step_update_err;
    }

  for (D_UINT i = 0; i < simpleTypesSize; ++i)
    {
      D_UINT    type;
      D_UINT    fieldsCount;

      if ((WPushStackValue (hnd, simpleTypes[i], 0, NULL) != WCS_OK)
          || (WUpdateStackFlush (hnd) != WCS_OK))
        {
          goto test_stack_step_update_err;
        }
      else if ((WDescribeStackTop (hnd, &type) != WCS_OK)
               || (type != simpleTypes[i])
               || (WDescribeValueGetFieldsCount (hnd, &fieldsCount) != WCS_OK)
               || (fieldsCount != 0))
        {
          goto test_stack_step_update_err;
        }
    }

  if ((WPushStackValue (hnd, WFT_TABLE_MASK, tableFieldsSize, tableFields) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK)
      || (match_table_fields_match (hnd, tableFields, tableFieldsSize) == false))
    {
      goto test_stack_step_update_err;
    }

  if ((WPopStackValues (hnd, ~0) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
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
test_for_errors (W_CONNECTOR_HND hnd)
{
  D_UINT type;
  W_FieldDescriptor invalid1 = { "", WFT_ARRAY_MASK | WFT_BOOL };
  W_FieldDescriptor invalid2 = { "name", WFT_FIELD_MASK | WFT_INT8} ;

  cout << "Testing against error conditions ... ";

  if ((WPushStackValue (NULL, WFT_REAL, 0, NULL) != WCS_INVALID_ARGS)
      || (WPushStackValue (hnd, WFT_ARRAY_MASK | WFT_NOTSET, 0, NULL) != WCS_INVALID_ARGS)
      || (WPushStackValue (hnd, WFT_FIELD_MASK | WFT_NOTSET, 0, NULL) != WCS_INVALID_ARGS)
      || (WPushStackValue (hnd, WFT_TABLE_MASK , 0, tableFields) != WCS_INVALID_ARGS)
      || (WPushStackValue (hnd, WFT_TABLE_MASK, 1, &invalid1) != WCS_INVALID_ARGS)
      || (WPushStackValue (hnd, WFT_TABLE_MASK, 1, &invalid2) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WDescribeStackTop (NULL, NULL) != WCS_INVALID_ARGS)
            || (WDescribeStackTop (NULL, &type) != WCS_INVALID_ARGS)
            || (WDescribeStackTop (hnd, NULL) != WCS_INVALID_ARGS))
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
  success = success && test_stack_step_update (hnd);
  success = success && test_stack_bulk_update (hnd);

  WClose (hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

