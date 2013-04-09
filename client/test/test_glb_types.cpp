/*
 * test_glb_types.cpp
 *
 *  Created on: Jan 15, 2013
 *      Author: ipopa
 */

#include <iostream>
#include <cstring>

#include "test_client_common.h"

using namespace std;

struct GlobalValueEntry
{
  const char* const name;
  const uint_t        type;
  const uint_t        fieldsCount;
};

struct FieldEntry
{
  const char* const name;
  uint16_t            type;
  bool                visited;
};

GlobalValueEntry no_fileds_types[] =
    {
        {"bool_", WFT_BOOL, 0},
        {"char_", WFT_CHAR, 0},
        {"date_", WFT_DATE, 0},
        {"datetime_", WFT_DATETIME, 0},
        {"hirestime_", WFT_HIRESTIME, 0},
        {"int8_", WFT_INT8, 0},
        {"int16_", WFT_INT16, 0},
        {"int32_", WFT_INT32, 0},
        {"int64_", WFT_INT64, 0},
        {"uint8_", WFT_UINT8, 0},
        {"uint16_", WFT_UINT16, 0},
        {"uint32_", WFT_UINT32, 0},
        {"uint64_", WFT_UINT64, 0},
        {"real_", WFT_REAL, 0},
        {"richreal_", WFT_RICHREAL, 0},
        {"text_", WFT_TEXT, 0},

        {"array_bool_", WFT_ARRAY_MASK | WFT_BOOL, 0},
        {"array_char_", WFT_ARRAY_MASK | WFT_CHAR, 0},
        {"array_date_", WFT_ARRAY_MASK | WFT_DATE, 0},
        {"array_datetime_", WFT_ARRAY_MASK | WFT_DATETIME, 0},
        {"array_hirestime_", WFT_ARRAY_MASK | WFT_HIRESTIME, 0},
        {"array_int8_", WFT_ARRAY_MASK | WFT_INT8, 0},
        {"array_int16_", WFT_ARRAY_MASK | WFT_INT16, 0},
        {"array_int32_", WFT_ARRAY_MASK | WFT_INT32, 0},
        {"array_int64_", WFT_ARRAY_MASK | WFT_INT64, 0},
        {"array_uint8_", WFT_ARRAY_MASK | WFT_UINT8, 0},
        {"array_uint16_", WFT_ARRAY_MASK | WFT_UINT16, 0},
        {"array_uint32_", WFT_ARRAY_MASK | WFT_UINT32, 0},
        {"array_uint64_", WFT_ARRAY_MASK | WFT_UINT64, 0},
        {"array_real_", WFT_ARRAY_MASK | WFT_REAL, 0},
        {"array_richreal_", WFT_ARRAY_MASK | WFT_RICHREAL, 0},
//        {"array_text_",  WFT_ARRAY_MASK | WFT_TEXT, 0},

        {"field_bool_", WFT_FIELD_MASK | WFT_BOOL, 0},
        {"field_char_", WFT_FIELD_MASK | WFT_CHAR, 0},
        {"field_date_", WFT_FIELD_MASK | WFT_DATE, 0},
        {"field_datetime_", WFT_FIELD_MASK | WFT_DATETIME, 0},
        {"field_hirestime_", WFT_FIELD_MASK | WFT_HIRESTIME, 0},
        {"field_int8_", WFT_FIELD_MASK | WFT_INT8, 0},
        {"field_int16_", WFT_FIELD_MASK | WFT_INT16, 0},
        {"field_int32_", WFT_FIELD_MASK | WFT_INT32, 0},
        {"field_int64_", WFT_FIELD_MASK | WFT_INT64, 0},
        {"field_uint8_", WFT_FIELD_MASK | WFT_UINT8, 0},
        {"field_uint16_", WFT_FIELD_MASK | WFT_UINT16, 0},
        {"field_uint32_", WFT_FIELD_MASK | WFT_UINT32, 0},
        {"field_uint64_", WFT_FIELD_MASK | WFT_UINT64, 0},
        {"field_real_", WFT_FIELD_MASK | WFT_REAL, 0},
        {"field_richreal_", WFT_FIELD_MASK | WFT_RICHREAL, 0},
        {"field_text_", WFT_FIELD_MASK | WFT_TEXT, 0},

        {"field_array_bool_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_BOOL, 0},
        {"field_array_char_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_CHAR, 0},
        {"field_array_date_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_DATE, 0},
        {"field_array_datetime_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_DATETIME, 0},
        {"field_array_hirestime_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_HIRESTIME, 0},
        {"field_array_int8_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT8, 0},
        {"field_array_int16_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT16, 0},
        {"field_array_int32_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT32, 0},
        {"field_array_int64_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_INT64, 0},
        {"field_array_uint8_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT8, 0},
        {"field_array_uint16_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT16, 0},
        {"field_array_uint32_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT32, 0},
        {"field_array_uint64_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_UINT64, 0},
        {"field_array_real_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_REAL, 0},
        {"field_array_richreal_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_RICHREAL, 0},
//        {"field_array_text_", WFT_FIELD_MASK | WFT_ARRAY_MASK | WFT_TEXT, 0},

        {"one_field_table_", WFT_TABLE_MASK, 1},
        {"complete_field_table_", WFT_TABLE_MASK, 31}
    };

FieldEntry tableFields[] =
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

static const char one_field_table[] = "one_field_table_global_var_this_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
static const char complete_field_table[] = "complete_field_table_global_var_this_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";

static bool
test_global_value_description (W_CONNECTOR_HND hnd)
{
  const char suffix[] = "global_var_this_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  char buffer[1024];
  const uint_t glbsCount = sizeof (no_fileds_types)/sizeof (no_fileds_types[0]);

  cout << "Testing global values types ... ";

  for (uint_t i = 0; i < glbsCount; ++i)
    {
      uint_t type;
      uint_t fieldsCount;

      strcpy (buffer, no_fileds_types[i].name);
      strcat (buffer, suffix);

      if ((WDescribeValue (hnd, buffer, &type) != WCS_OK)
          || (type != no_fileds_types[i].type))
        {
          goto test_global_value_description_err;
        }
      else if ((WDescribeValueGetFieldsCount (hnd, &fieldsCount) != WCS_OK)
          || (fieldsCount != no_fileds_types[i].fieldsCount))
        {
          goto test_global_value_description_err;
        }
    }

  cout << "OK\n";
  return true;

test_global_value_description_err:

  cout << "FAIL\n";
  return false;
}

static bool
test_field_match (const char*   field,
                  const uint_t    type)
{
  for (uint_t i = 0; i < sizeof (tableFields) / sizeof (tableFields[0]); ++i)
    {
      if (strcmp (tableFields[i].name, field) == 0)
        {
          if (tableFields[i].visited
              || (tableFields[i].type != type))
            {
              return false;
            }
          else
            {
              tableFields[i].visited = true;
              return true;
            }
        }
    }

  return false;
}

static bool
test_complete_field_global (W_CONNECTOR_HND hnd)
{
  uint_t        type;
  const char* fieldName;
  uint_t        fieldsCount;

  cout << "Testing the complete field ... ";

  if ((WDescribeValue (hnd, complete_field_table, &type) != WCS_OK)
      || (type != WFT_TABLE_MASK)
      || (WDescribeValueGetFieldsCount (hnd, &fieldsCount) != WCS_OK)
      || (fieldsCount != sizeof (tableFields) / sizeof (tableFields[0])))
    {
      goto test_complete_field_global_err;
    }

  while (fieldsCount-- > 0)
    {
      if ((WDescribeValueFetchField (hnd, &fieldName, &type) != WCS_OK)
          || ! test_field_match (fieldName, type))
        {
          goto test_complete_field_global_err;
        }
    }

  if ((WDescribeValueFetchField (hnd, &fieldName, &type) != WCS_OK)
      || (fieldName != NULL)
      || (type != WFT_NOTSET)
      || (WDescribeValueFetchField (hnd, &fieldName, &type) == WCS_OK))
    {
      goto test_complete_field_global_err; //Should not allow an extra fetch!
    }

  for (uint_t i = 0; i < sizeof (tableFields) / sizeof (tableFields[0]); ++i)
    {
      if (! tableFields[i].visited)
        goto test_complete_field_global_err;
    }

  cout << "OK\n";
  return true;

test_complete_field_global_err:
  cout << "FAIL\n";
  return false;
}

static bool
test_one_field_global (W_CONNECTOR_HND hnd)
{
  uint_t        type;
  const char* fieldName;

  cout << "Testing the one field ... ";

  if ((WDescribeValue (hnd, one_field_table, &type) != WCS_OK)
      || (type != WFT_TABLE_MASK))
    {
      goto test_one_field_global_err;
    }
  else if ((WDescribeValueFetchField (hnd, &fieldName, &type) != WCS_OK)
            || (type != WFT_BOOL)
            || (strcmp (fieldName, "field1") != 0))
    {
      goto test_one_field_global_err;
    }

  if ((WDescribeValueFetchField (hnd, &fieldName, &type) != WCS_OK)
      || (fieldName != NULL)
      || (type != WFT_NOTSET)
      || (WDescribeValueFetchField (hnd, &fieldName, &type) == WCS_OK))
    {
      goto test_one_field_global_err; //Should not allow an extra fetch!
    }

  cout << "OK\n";
  return true;

test_one_field_global_err:
  cout << "FAIL\n";
  return false;
}

static bool
test_for_errors (W_CONNECTOR_HND hnd)
{
  uint_t          fieldsCount;
  uint_t          type;
  const char*   nameFetched;

  cout << "Testing against error conditions ... ";

  if ((WDescribeValueFetchField (hnd, &nameFetched, &type) != WCS_INCOMPLETE_CMD)
      || (WDescribeValueGetFieldsCount (hnd, &fieldsCount) != WCS_INCOMPLETE_CMD))
    {
      goto test_for_errors_fail;
    }
  else if ((WDescribeValue (NULL, NULL, NULL) != WCS_INVALID_ARGS)
           || (WDescribeValue (hnd, one_field_table, NULL) != WCS_INVALID_ARGS)
           || (WDescribeValue (hnd, NULL, &type) != WCS_INVALID_ARGS))

    {
      goto test_for_errors_fail;
    }
  else if ((WDescribeValue (hnd, one_field_table, &type) != WCS_OK)

           || (WDescribeValueGetFieldsCount (NULL, NULL) != WCS_INVALID_ARGS)
           || (WDescribeValueGetFieldsCount (hnd, NULL) != WCS_INVALID_ARGS)
           || (WDescribeValueFetchField (NULL, NULL, NULL) != WCS_INVALID_ARGS)
           || (WDescribeValueFetchField (NULL, &nameFetched, &type) != WCS_INVALID_ARGS)
           || (WDescribeValueFetchField (hnd, NULL, &type) != WCS_INVALID_ARGS)
           || (WDescribeValueFetchField (hnd, &nameFetched, NULL) != WCS_INVALID_ARGS)

           || (WDescribeValueFetchField (hnd, &nameFetched, &type) != WCS_OK)
           || (WDescribeValueGetFieldsCount (hnd, &fieldsCount) != WCS_OK))
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
  return 0;
}

const char*
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
  success = success && test_global_value_description (hnd);
  success = success && test_complete_field_global (hnd);
  success = success && test_one_field_global (hnd);

  WClose (hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}




