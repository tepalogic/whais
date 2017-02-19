/*
 * test_proc_args.cpp
 *
 *  Created on: Jan 12, 2013
 *      Author: ipopa
 */

#include <iostream>
#include <cstring>

#include "test_client_common.h"

using namespace std;

struct ProcedureEntry
{
  const char* const name;
  uint16_t            retRawType;
  bool                visited;
};

struct FieldEntry
{
  const char* const name;
  uint16_t            type;
  bool                visited;
};

static const uint_t COMPLETE_PROC_ARGS  = 68;

ProcedureEntry _procedures[] =
    {
        {"bool_return_proc_no_args", WHC_TYPE_BOOL, false},
        {"char_return_proc_no_args", WHC_TYPE_CHAR, false},
        {"date_return_proc_no_args", WHC_TYPE_DATE, false},
        {"datetime_return_proc_no_args", WHC_TYPE_DATETIME, false},
        {"hirestime_return_proc_no_args", WHC_TYPE_HIRESTIME, false},
        {"int8_return_proc_no_args", WHC_TYPE_INT8, false},
        {"int16_return_proc_no_args", WHC_TYPE_INT16, false},
        {"int32_return_proc_no_args", WHC_TYPE_INT32, false},
        {"int64_return_proc_no_args", WHC_TYPE_INT64, false},
        {"uint8_return_proc_no_args", WHC_TYPE_UINT8, false},
        {"uint16_return_proc_no_args", WHC_TYPE_UINT16, false},
        {"uint32_return_proc_no_args", WHC_TYPE_UINT32,  false},
        {"uint64_return_proc_no_args", WHC_TYPE_UINT64,  false},
        {"real_return_proc_no_args", WHC_TYPE_REAL, false},
        {"richreal_return_proc_no_args", WHC_TYPE_RICHREAL, false},
        {"text_return_proc_no_args", WHC_TYPE_TEXT, false},

        {"array_bool_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_BOOL, false},
        {"array_char_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_CHAR, false},
        {"array_date_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATE, false},
        {"array_datetime_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATETIME, false},
        {"array_hirestime_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_HIRESTIME, false},
        {"array_int8_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT8, false},
        {"array_int16_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT16, false},
        {"array_int32_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT32, false},
        {"array_int64_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT64, false},
        {"array_uint8_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT8, false},
        {"array_uint16_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT16, false},
        {"array_uint32_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT32, false},
        {"array_uint64_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT64, false},
        {"array_real_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_REAL, false},
        {"array_richreal_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_RICHREAL, false},
//        {"array_text_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_TEXT, false},
        {"array_return_proc_no_args", WHC_TYPE_ARRAY_MASK | WHC_TYPE_NOTSET, false},

        {"field_bool_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_BOOL, false},
        {"field_char_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_CHAR, false},
        {"field_date_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_DATE, false},
        {"field_datetime_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_DATETIME, false},
        {"field_hirestime_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_HIRESTIME, false},
        {"field_int8_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_INT8, false},
        {"field_int16_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_INT16, false},
        {"field_int32_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_INT32, false},
        {"field_int64_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_INT64, false},
        {"field_uint8_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT8, false},
        {"field_uint16_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT16, false},
        {"field_uint32_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT32, false},
        {"field_uint64_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT64, false},
        {"field_real_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_REAL, false},
        {"field_richreal_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_RICHREAL, false},
        {"field_text_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_TEXT, false},

        {"field_array_bool_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_BOOL, false},
        {"field_array_char_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_CHAR, false},
        {"field_array_date_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATE, false},
        {"field_array_datetime_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATETIME, false},
        {"field_array_hirestime_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_HIRESTIME, false},
        {"field_array_int8_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT8, false},
        {"field_array_int16_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT16, false},
        {"field_array_int32_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT32, false},
        {"field_array_int64_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT64, false},
        {"field_array_uint8_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT8, false},
        {"field_array_uint16_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT16, false},
        {"field_array_uint32_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT32, false},
        {"field_array_uint64_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT64, false},
        {"field_array_real_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_REAL, false},
        {"field_array_richreal_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_RICHREAL, false},
//        {"field_array_text_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_TEXT, false},
        {"field_array_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_NOTSET, false},
        {"field_return_proc_no_args", WHC_TYPE_FIELD_MASK | WHC_TYPE_NOTSET, false},

        {"one_field_table_return_proc_no_args", WHC_TYPE_TABLE_MASK, false},
        {"two_field_table_return_proc_no_args", WHC_TYPE_TABLE_MASK, false},
        {"table_return_proc_no_args", WHC_TYPE_TABLE_MASK, false},
        {"table_return_proc_all_type_args", WHC_TYPE_TABLE_MASK, false}
    };

static bool
check_field_entry(FieldEntry*   fields,
                   const uint_t  fieldsCount,
                   const char* fieldName,
                   uint_t        fieldType)
{
  FieldEntry*  entry = NULL;

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      entry = fields + i;
      if (strcmp(entry->name, fieldName) == 0)
        break;
    }

  if ((entry == NULL)
      || (entry->type != fieldType))
    {
      return false;
    }

  entry->visited = true;
  return true;
}

static bool
test_proc_one_field_tab_ret(WH_CONNECTION hnd)
{
  const char  procName[]  = "one_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  const char* fieldName;

  uint_t fieldType;
  uint_t fieldsCount;

  cout << "Testing one field ret tab ... ";

  if ((WProcParamField(hnd, procName, 0, 0, &fieldName, &fieldType) != WCS_OK)
      || (strcmp(fieldName, "field1") != 0)
      || (fieldType != WHC_TYPE_TEXT))
    {
      goto test_proc_one_field_tab_ret_err;
    }
  else if ((WProcParamFieldCount(hnd, procName, 0, &fieldsCount) != WCS_OK)
           || (fieldsCount != 1))
    {
      goto test_proc_one_field_tab_ret_err;
    }

  cout << "OK\n";
  return true;

test_proc_one_field_tab_ret_err:

  cout << "FAIL\n";
  return false;
}

static bool
test_proc_two_field_tab_ret(WH_CONNECTION hnd)
{
  const char  procName[]  = "two_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";

  FieldEntry fields[] =
      {
          {"field1", WHC_TYPE_CHAR, false},
          {"field2", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT64, false}
      };

  uint_t fieldsCount = sizeof(fields) / sizeof(fields[0]);
  uint_t fieldType;

  cout << "Testing two field ret tab ... ";

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      const char* fieldName;
      if ((WProcParamField(hnd, procName, 0, i, &fieldName, &fieldType) != WCS_OK)
        || ! check_field_entry(fields, fieldsCount, fieldName, fieldType))
        {
          goto test_proc_two_field_tab_ret_err;
        }
    }

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      if (! fields[i].visited)
        return false;
    }

  fieldsCount = 0;
  if ((WProcParamFieldCount(hnd, procName, 0, &fieldsCount) != WCS_OK)
      || (fieldsCount != sizeof(fields) / sizeof(fields[0])))
    {
      goto test_proc_two_field_tab_ret_err;
    }

  cout << "OK\n";
  return true;

test_proc_two_field_tab_ret_err:

  cout << "FAIL\n";
  return false;
}

static bool
test_proc_complete_field_tab_ret(WH_CONNECTION hnd)
{
  const char  procName[]  = "table_return_proc_all_type_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  const uint_t  procParamas[] =
      {
          WHC_TYPE_TABLE_MASK,

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
          WHC_TYPE_ARRAY_MASK | WHC_TYPE_NOTSET,

          WHC_TYPE_FIELD_MASK | WHC_TYPE_BOOL,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_CHAR,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_DATE,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_DATETIME,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_HIRESTIME,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_INT8,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_INT16,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_INT32,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_INT64,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT8,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT16,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT32,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_UINT64,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_REAL,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_RICHREAL,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_TEXT,

          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_BOOL,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_CHAR,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATE,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATETIME,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_HIRESTIME,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT8,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT16,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT32,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT64,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT8,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT16,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT32,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT64,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_REAL,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_RICHREAL,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_NOTSET,
          WHC_TYPE_FIELD_MASK | WHC_TYPE_NOTSET,

          WHC_TYPE_TABLE_MASK,
          WHC_TYPE_TABLE_MASK
      };

  FieldEntry fields[] =
      {
          {"bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_BOOL, false},
          {"char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_CHAR, false},
          {"date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_DATE, false},
          {"datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_DATETIME, false},
          {"hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_HIRESTIME, false},
          {"int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_INT8, false},
          {"int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_INT16, false},
          {"int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_INT32, false},
          {"int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_INT64, false},
          {"uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_UINT8, false},
          {"uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_UINT16, false},
          {"uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_UINT32, false},
          {"uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_UINT64, false},
          {"real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_REAL, false},
          {"richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_RICHREAL, false},
          {"text_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_TEXT, false},

          {"array_bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_BOOL, false},
          {"array_char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_CHAR, false},
          {"array_date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATE, false},
          {"array_datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_DATETIME, false},
          {"array_hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_HIRESTIME, false},
          {"array_int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT8, false},
          {"array_int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT16, false},
          {"array_int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT32, false},
          {"array_int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_INT64, false},
          {"array_uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT8, false},
          {"array_uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT16, false},
          {"array_uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT32, false},
          {"array_uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_UINT64, false},
          {"array_real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_REAL, false},
          {"array_richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WHC_TYPE_ARRAY_MASK | WHC_TYPE_RICHREAL, false}
      };

  uint_t fieldsCount = sizeof(fields) / sizeof(fields[0]);
  uint_t fieldType;

  cout << "Testing complete proc   ... ";

  for (uint_t i = 0; i < COMPLETE_PROC_ARGS; ++i)
    {
      uint_t procType;
      if ((WProcParamType(hnd, procName, i, &procType) != WCS_OK)
          || (procType != procParamas[i]))
        {
          goto test_proc_complete_field_tab_ret_err;
        }
    }

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      const char* fieldName;
      if ((WProcParamField(hnd, procName, 0, i, &fieldName, &fieldType) != WCS_OK)
        || ! check_field_entry(fields, fieldsCount, fieldName, fieldType))
        {
          goto test_proc_complete_field_tab_ret_err;
        }
    }

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      if (! fields[i].visited)
        return false;

      fields[i].visited = false;
    }

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      const char* fieldName;
      if ((WProcParamField(hnd, procName, COMPLETE_PROC_ARGS - 1, i, &fieldName, &fieldType) != WCS_OK)
        || ! check_field_entry(fields, fieldsCount, fieldName, fieldType))
        {
          goto test_proc_complete_field_tab_ret_err;
        }
    }

  for (uint_t i = 0; i < fieldsCount; ++i)
    {
      if (! fields[i].visited)
        return false;

      fields[i].visited = false;
    }

  cout << "OK\n";
  return true;

test_proc_complete_field_tab_ret_err:

  cout << "FAIL\n";
  return false;
}

static bool
test_proc_entry_ret_match(WH_CONNECTION hnd, ProcedureEntry* entry)
{
  const char suffix[] = "_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  char buffer[1024];
  uint_t type;
  uint_t paramsCount;

  strcpy(buffer, entry->name);
  strcat(buffer, suffix);

  if ((WProcParamType(hnd, buffer, 0, &type) != WCS_OK)
      || (type != entry->retRawType))
    {
      goto test_proc_name_match_error;
    }
  else if (WProcParamsCount(hnd, buffer, &paramsCount) != WCS_OK)
    goto test_proc_name_match_error;

  if (strcmp(entry->name, "table_return_proc_all_type_args") == 0)
    {
      if (paramsCount != 68)
        goto test_proc_name_match_error;
    }
  else if (paramsCount != 1)
    goto test_proc_name_match_error;

  return true;

  entry->visited = true;

test_proc_name_match_error:
  return false;
}

static bool
test_procedures_list(WH_CONNECTION hnd)
{
  uint_t procsCount = sizeof(_procedures) / sizeof(_procedures[0]);
  cout << "Testing the procedures return values ... ";

  for (uint_t i = 0; i < procsCount; ++i)
    {
      if (! test_proc_entry_ret_match(hnd, &_procedures[i]))
        goto test_procedures_list_error;
    }

  cout << "OK\n";
  return true;

test_procedures_list_error:

  cout << "FAIL\n";
  return false;
}

static bool
test_for_errors(WH_CONNECTION hnd)
{
  uint_t        paramsCount;
  uint_t        type;
  uint_t        fieldCount;
  const char* fieldName;
  const char* procName  = "two_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";

  cout << "Testing against error conditions ... ";
  if ((WProcParamsCount(NULL, NULL, NULL) != WCS_INVALID_ARGS)
      || (WProcParamsCount(NULL, procName, &paramsCount) != WCS_INVALID_ARGS)
      || (WProcParamsCount(hnd, procName, NULL) != WCS_INVALID_ARGS)
      || (WProcParamsCount(hnd, NULL, &paramsCount) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WProcParamType(NULL, NULL, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcParamType(NULL, procName, 0, &type) != WCS_INVALID_ARGS)
      || (WProcParamType(hnd, procName, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcParamType(hnd, NULL, 0, &type) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WProcParamFieldCount(NULL, NULL, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcParamFieldCount(NULL, procName, 0, &fieldCount) != WCS_INVALID_ARGS)
      || (WProcParamFieldCount(hnd, procName, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcParamFieldCount(hnd, NULL, 0, &fieldCount) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WProcParamField(NULL, NULL, 0, 0, NULL, NULL) != WCS_INVALID_ARGS)
      || (WProcParamField(NULL, procName, 0, 0, &fieldName, &type) != WCS_INVALID_ARGS)
      || (WProcParamField(hnd, procName, 0, 0, NULL, &type) != WCS_INVALID_ARGS)
      || (WProcParamField(hnd, procName, 0, 0, &fieldName, NULL) != WCS_INVALID_ARGS)
      || (WProcParamField(hnd, NULL, 0, 0, &fieldName, &type) != WCS_INVALID_ARGS))
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
  return 0;
}

const char*
DefaultUserPassword()
{
  return "root_test_password";
}

int
main(int argc, const char** argv)
{
  WH_CONNECTION       hnd = NULL;

  bool success = tc_settup_connection(argc, argv, &hnd);

  success = success && test_for_errors(hnd);
  success = success && test_procedures_list(hnd);
  success = success && test_proc_one_field_tab_ret(hnd);
  success = success && test_proc_two_field_tab_ret(hnd);
  success = success && test_proc_complete_field_tab_ret(hnd);

  WClose(hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}


