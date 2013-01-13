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
  const D_CHAR* const name;
  D_UINT16            retRawType;
  bool                visited;
};

struct FieldEntry
{
  const D_CHAR* const name;
  D_UINT16            type;
  bool                visited;
};

static const D_UINT COMPLETE_PROC_ARGS  = 68;

ProcedureEntry _procedures[] =
    {
        {"bool_return_proc_no_args", WFT_BOOL, false},
        {"char_return_proc_no_args", WFT_CHAR, false},
        {"date_return_proc_no_args", WFT_DATE, false},
        {"datetime_return_proc_no_args", WFT_DATETIME, false},
        {"hirestime_return_proc_no_args", WFT_HIRESTIME, false},
        {"int8_return_proc_no_args", WFT_INT8, false},
        {"int16_return_proc_no_args", WFT_INT16, false},
        {"int32_return_proc_no_args", WFT_INT32, false},
        {"int64_return_proc_no_args", WFT_INT64, false},
        {"uint8_return_proc_no_args", WFT_UINT8, false},
        {"uint16_return_proc_no_args", WFT_UINT16, false},
        {"uint32_return_proc_no_args", WFT_UINT32,  false},
        {"uint64_return_proc_no_args", WFT_UINT64,  false},
        {"real_return_proc_no_args", WFT_REAL, false},
        {"richreal_return_proc_no_args", WFT_RICHREAL, false},
        {"text_return_proc_no_args", WFT_TEXT, false},

        {"array_bool_return_proc_no_args", WFT_ARRAY_MASK | WFT_BOOL, false},
        {"array_char_return_proc_no_args", WFT_ARRAY_MASK | WFT_CHAR, false},
        {"array_date_return_proc_no_args", WFT_ARRAY_MASK | WFT_DATE, false},
        {"array_datetime_return_proc_no_args", WFT_ARRAY_MASK | WFT_DATETIME, false},
        {"array_hirestime_return_proc_no_args", WFT_ARRAY_MASK | WFT_HIRESTIME, false},
        {"array_int8_return_proc_no_args", WFT_ARRAY_MASK | WFT_INT8, false},
        {"array_int16_return_proc_no_args", WFT_ARRAY_MASK | WFT_INT16, false},
        {"array_int32_return_proc_no_args", WFT_ARRAY_MASK | WFT_INT32, false},
        {"array_int64_return_proc_no_args", WFT_ARRAY_MASK | WFT_INT64, false},
        {"array_uint8_return_proc_no_args", WFT_ARRAY_MASK | WFT_UINT8, false},
        {"array_uint16_return_proc_no_args", WFT_ARRAY_MASK | WFT_UINT16, false},
        {"array_uint32_return_proc_no_args", WFT_ARRAY_MASK | WFT_UINT32, false},
        {"array_uint64_return_proc_no_args", WFT_ARRAY_MASK | WFT_UINT64, false},
        {"array_real_return_proc_no_args", WFT_ARRAY_MASK | WFT_REAL, false},
        {"array_richreal_return_proc_no_args", WFT_ARRAY_MASK | WFT_RICHREAL, false},
//        {"array_text_return_proc_no_args", WFT_ARRAY_MASK | WFT_TEXT, false},
        {"array_return_proc_no_args", WFT_ARRAY_MASK | WFT_NOTSET, false},

        {"field_bool_return_proc_no_args", WFT_FIELD_MASK | WFT_BOOL, false},
        {"field_char_return_proc_no_args", WFT_FIELD_MASK | WFT_CHAR, false},
        {"field_date_return_proc_no_args", WFT_FIELD_MASK | WFT_DATE, false},
        {"field_datetime_return_proc_no_args", WFT_FIELD_MASK | WFT_DATETIME, false},
        {"field_hirestime_return_proc_no_args", WFT_FIELD_MASK | WFT_HIRESTIME, false},
        {"field_int8_return_proc_no_args", WFT_FIELD_MASK | WFT_INT8, false},
        {"field_int16_return_proc_no_args", WFT_FIELD_MASK | WFT_INT16, false},
        {"field_int32_return_proc_no_args", WFT_FIELD_MASK | WFT_INT32, false},
        {"field_int64_return_proc_no_args", WFT_FIELD_MASK | WFT_INT64, false},
        {"field_uint8_return_proc_no_args", WFT_FIELD_MASK | WFT_UINT8, false},
        {"field_uint16_return_proc_no_args", WFT_FIELD_MASK | WFT_UINT16, false},
        {"field_uint32_return_proc_no_args", WFT_FIELD_MASK | WFT_UINT32, false},
        {"field_uint64_return_proc_no_args", WFT_FIELD_MASK | WFT_UINT64, false},
        {"field_real_return_proc_no_args", WFT_FIELD_MASK | WFT_REAL, false},
        {"field_richreal_return_proc_no_args", WFT_FIELD_MASK | WFT_RICHREAL, false},
        {"field_text_return_proc_no_args", WFT_FIELD_MASK | WFT_TEXT, false},

        {"field_array_bool_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_BOOL, false},
        {"field_array_char_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_CHAR, false},
        {"field_array_date_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_DATE, false},
        {"field_array_datetime_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_DATETIME, false},
        {"field_array_hirestime_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_HIRESTIME, false},
        {"field_array_int8_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT8, false},
        {"field_array_int16_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT16, false},
        {"field_array_int32_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT32, false},
        {"field_array_int64_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT64, false},
        {"field_array_uint8_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT8, false},
        {"field_array_uint16_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT16, false},
        {"field_array_uint32_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT32, false},
        {"field_array_uint64_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT64, false},
        {"field_array_real_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_REAL, false},
        {"field_array_richreal_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_RICHREAL, false},
//        {"field_array_text_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_TEXT, false},
        {"field_array_return_proc_no_args", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_NOTSET, false},
        {"field_return_proc_no_args", WFT_FIELD_MASK | WFT_NOTSET, false},

        {"one_field_table_return_proc_no_args", WFT_TABLE_MASK, false},
        {"two_field_table_return_proc_no_args", WFT_TABLE_MASK, false},
        {"table_return_proc_no_args", WFT_TABLE_MASK, false},
        {"table_return_proc_all_type_args", WFT_TABLE_MASK, false}
    };

static bool
check_field_entry (FieldEntry*   fields,
                   const D_UINT  fieldsCount,
                   const D_CHAR* fieldName,
                   D_UINT        fieldType)
{
  FieldEntry*  entry = NULL;

  for (D_UINT i = 0; i < fieldsCount; ++i)
    {
      entry = fields + i;
      if (strcmp (entry->name, fieldName) == 0)
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
test_proc_one_field_tab_ret (W_CONNECTOR_HND hnd)
{
  const D_CHAR  procName[]  = "one_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  const D_CHAR* fieldName;

  D_UINT fieldType;
  D_UINT fieldsCount;

  cout << "Testing one field ret tab ... ";

  if ((WProcedureParameterField (hnd, procName, 0, 0, &fieldName, &fieldType) != WCS_OK)
      || (strcmp (fieldName, "field1") != 0)
      || (fieldType != WFT_TEXT))
    {
      goto test_proc_one_field_tab_ret_err;
    }
  else if ((WProcedureParameterFieldCount (hnd, procName, 0, &fieldsCount) != WCS_OK)
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
test_proc_two_field_tab_ret (W_CONNECTOR_HND hnd)
{
  const D_CHAR  procName[]  = "two_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";

  FieldEntry fields[] =
      {
          {"field1", WFT_CHAR, false},
          {"field2", WFT_ARRAY_MASK | WFT_UINT64, false}
      };

  D_UINT fieldsCount = sizeof (fields) / sizeof (fields[0]);
  D_UINT fieldType;

  cout << "Testing two field ret tab ... ";

  for (D_UINT i = 0; i < fieldsCount; ++i)
    {
      const D_CHAR* fieldName;
      if ((WProcedureParameterField (hnd, procName, 0, i, &fieldName, &fieldType) != WCS_OK)
        || ! check_field_entry (fields, fieldsCount, fieldName, fieldType))
        {
          goto test_proc_two_field_tab_ret_err;
        }
    }

  for (D_UINT i = 0; i < fieldsCount; ++i)
    {
      if (! fields[i].visited)
        return false;
    }

  fieldsCount = 0;
  if ((WProcedureParameterFieldCount (hnd, procName, 0, &fieldsCount) != WCS_OK)
      || (fieldsCount != sizeof (fields) / sizeof (fields[0])))
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
test_proc_complete_field_tab_ret (W_CONNECTOR_HND hnd)
{
  const D_CHAR  procName[]  = "table_return_proc_all_type_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  const D_UINT  procParamas[] =
      {
          WFT_TABLE_MASK,

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
          WFT_ARRAY_MASK | WFT_NOTSET,

          WFT_FIELD_MASK | WFT_BOOL,
          WFT_FIELD_MASK | WFT_CHAR,
          WFT_FIELD_MASK | WFT_DATE,
          WFT_FIELD_MASK | WFT_DATETIME,
          WFT_FIELD_MASK | WFT_HIRESTIME,
          WFT_FIELD_MASK | WFT_INT8,
          WFT_FIELD_MASK | WFT_INT16,
          WFT_FIELD_MASK | WFT_INT32,
          WFT_FIELD_MASK | WFT_INT64,
          WFT_FIELD_MASK | WFT_UINT8,
          WFT_FIELD_MASK | WFT_UINT16,
          WFT_FIELD_MASK | WFT_UINT32,
          WFT_FIELD_MASK | WFT_UINT64,
          WFT_FIELD_MASK | WFT_REAL,
          WFT_FIELD_MASK | WFT_RICHREAL,
          WFT_FIELD_MASK | WFT_TEXT,

          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_BOOL,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_CHAR,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_DATE,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_DATETIME,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_HIRESTIME,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT8,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT16,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT32,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT64,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT8,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT16,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT32,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT64,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_REAL,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_RICHREAL,
          WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_NOTSET,
          WFT_FIELD_MASK | WFT_NOTSET,

          WFT_TABLE_MASK,
          WFT_TABLE_MASK
      };

  FieldEntry fields[] =
      {
          {"bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_BOOL, false},
          {"char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_CHAR, false},
          {"date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_DATE, false},
          {"datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_DATETIME, false},
          {"hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_HIRESTIME, false},
          {"int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_INT8, false},
          {"int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_INT16, false},
          {"int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_INT32, false},
          {"int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_INT64, false},
          {"uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_UINT8, false},
          {"uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_UINT16, false},
          {"uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_UINT32, false},
          {"uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_UINT64, false},
          {"real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_REAL, false},
          {"richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_RICHREAL, false},
          {"text_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_TEXT, false},

          {"array_bool_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_ARRAY_MASK | WFT_ARRAY_MASK | WFT_BOOL, false},
          {"array_char_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_ARRAY_MASK | WFT_ARRAY_MASK | WFT_CHAR, false},
          {"array_date_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_ARRAY_MASK | WFT_ARRAY_MASK | WFT_DATE, false},
          {"array_datetime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_DATETIME, false},
          {"array_hirestime_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_HIRESTIME, false},
          {"array_int8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_INT8, false},
          {"array_int16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_INT16, false},
          {"array_int32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_INT32, false},
          {"array_int64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_INT64, false},
          {"array_uint8_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_UINT8, false},
          {"array_uint16_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_UINT16, false},
          {"array_uint32_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_UINT32, false},
          {"array_uint64_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_UINT64, false},
          {"array_real_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_REAL, false},
          {"array_richreal_field_This_is_a_long_field_suffix_coz_I_need_to_trigger_an_odd_behavior_002_bad", WFT_ARRAY_MASK | WFT_RICHREAL, false}
      };

  D_UINT fieldsCount = sizeof (fields) / sizeof (fields[0]);
  D_UINT fieldType;

  cout << "Testing complete proc   ... ";

  for (D_UINT i = 0; i < COMPLETE_PROC_ARGS; ++i)
    {
      D_UINT procType;
      if ((WProcedureParameter (hnd, procName, i, &procType) != WCS_OK)
          || (procType != procParamas[i]))
        {
          goto test_proc_complete_field_tab_ret_err;
        }
    }

  for (D_UINT i = 0; i < fieldsCount; ++i)
    {
      const D_CHAR* fieldName;
      if ((WProcedureParameterField (hnd, procName, 0, i, &fieldName, &fieldType) != WCS_OK)
        || ! check_field_entry (fields, fieldsCount, fieldName, fieldType))
        {
          goto test_proc_complete_field_tab_ret_err;
        }
    }

  for (D_UINT i = 0; i < fieldsCount; ++i)
    {
      if (! fields[i].visited)
        return false;

      fields[i].visited = false;
    }

  for (D_UINT i = 0; i < fieldsCount; ++i)
    {
      const D_CHAR* fieldName;
      if ((WProcedureParameterField (hnd, procName, COMPLETE_PROC_ARGS - 1, i, &fieldName, &fieldType) != WCS_OK)
        || ! check_field_entry (fields, fieldsCount, fieldName, fieldType))
        {
          goto test_proc_complete_field_tab_ret_err;
        }
    }

  for (D_UINT i = 0; i < fieldsCount; ++i)
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
test_proc_entry_ret_match (W_CONNECTOR_HND hnd, ProcedureEntry* entry)
{
  const D_CHAR suffix[] = "_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  D_CHAR buffer[1024];
  D_UINT type;
  D_UINT paramsCount;

  strcpy (buffer, entry->name);
  strcat (buffer, suffix);

  if ((WProcedureParameter (hnd, buffer, 0, &type) != WCS_OK)
      || (type != entry->retRawType))
    {
      goto test_proc_name_match_error;
    }
  else if (WProcedureParametersCount (hnd, buffer, &paramsCount) != WCS_OK)
    goto test_proc_name_match_error;

  if (strcmp (entry->name, "table_return_proc_all_type_args") == 0)
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
test_procedures_list (W_CONNECTOR_HND hnd)
{
  D_UINT procsCount = sizeof (_procedures) / sizeof (_procedures[0]);
  cout << "Testing the procedures return values ... ";

  for (D_UINT i = 0; i < procsCount; ++i)
    {
      if (! test_proc_entry_ret_match (hnd, &_procedures[i]))
        goto test_procedures_list_error;
    }

  cout << "OK\n";
  return true;

test_procedures_list_error:

  cout << "FAIL\n";
  return false;
}

static bool
test_for_errors (W_CONNECTOR_HND hnd)
{
  D_UINT        paramsCount;
  D_UINT        type;
  D_UINT        fieldCount;
  const D_CHAR* fieldName;
  const D_CHAR* procName  = "two_field_table_return_proc_no_args_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";

  cout << "Testing against error conditions ... ";
  if ((WProcedureParametersCount (NULL, NULL, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParametersCount (NULL, procName, &paramsCount) != WCS_INVALID_ARGS)
      || (WProcedureParametersCount (hnd, procName, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParametersCount (hnd, NULL, &paramsCount) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WProcedureParameter (NULL, NULL, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParameter (NULL, procName, 0, &type) != WCS_INVALID_ARGS)
      || (WProcedureParameter (hnd, procName, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParameter (hnd, NULL, 0, &type) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WProcedureParameterFieldCount (NULL, NULL, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParameterFieldCount (NULL, procName, 0, &fieldCount) != WCS_INVALID_ARGS)
      || (WProcedureParameterFieldCount (hnd, procName, 0, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParameterFieldCount (hnd, NULL, 0, &fieldCount) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if ((WProcedureParameterField (NULL, NULL, 0, 0, NULL, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParameterField (NULL, procName, 0, 0, &fieldName, &type) != WCS_INVALID_ARGS)
      || (WProcedureParameterField (hnd, procName, 0, 0, NULL, &type) != WCS_INVALID_ARGS)
      || (WProcedureParameterField (hnd, procName, 0, 0, &fieldName, NULL) != WCS_INVALID_ARGS)
      || (WProcedureParameterField (hnd, NULL, 0, 0, &fieldName, &type) != WCS_INVALID_ARGS))
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
  return 0;
}

const D_CHAR*
DefaultUserPassword ()
{
  return "root_test_password";
}

int
main (int argc, const char** argv)
{
  W_CONNECTOR_HND       hnd = NULL;

  bool success = tc_settup_connection (argc, argv, &hnd);

  success = success && test_for_errors (hnd);
  success = success && test_procedures_list (hnd);
  success = success && test_proc_one_field_tab_ret (hnd);
  success = success && test_proc_two_field_tab_ret (hnd);
  success = success && test_proc_complete_field_tab_ret (hnd);

  WClose (hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}


