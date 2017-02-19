/*
 * test_proc_list.cpp
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
  bool                visited;
};

ProcedureEntry _procedures[] =
    {
        {"bool_return_proc_no_args", false},
        {"char_return_proc_no_args", false},
        {"date_return_proc_no_args", false},
        {"datetime_return_proc_no_args", false},
        {"hirestime_return_proc_no_args", false},
        {"int8_return_proc_no_args", false},
        {"int16_return_proc_no_args", false},
        {"int32_return_proc_no_args", false},
        {"int64_return_proc_no_args", false},
        {"uint8_return_proc_no_args", false},
        {"uint16_return_proc_no_args", false},
        {"uint32_return_proc_no_args", false},
        {"uint64_return_proc_no_args", false},
        {"real_return_proc_no_args", false},
        {"richreal_return_proc_no_args", false},
        {"text_return_proc_no_args", false},

        {"array_bool_return_proc_no_args", false},
        {"array_char_return_proc_no_args", false},
        {"array_date_return_proc_no_args", false},
        {"array_datetime_return_proc_no_args", false},
        {"array_hirestime_return_proc_no_args", false},
        {"array_int8_return_proc_no_args", false},
        {"array_int16_return_proc_no_args", false},
        {"array_int32_return_proc_no_args", false},
        {"array_int64_return_proc_no_args", false},
        {"array_uint8_return_proc_no_args", false},
        {"array_uint16_return_proc_no_args", false},
        {"array_uint32_return_proc_no_args", false},
        {"array_uint64_return_proc_no_args", false},
        {"array_real_return_proc_no_args", false},
        {"array_richreal_return_proc_no_args", false},
//        {"array_text_return_proc_no_args", false},
        {"array_return_proc_no_args", false},

        {"field_bool_return_proc_no_args", false},
        {"field_char_return_proc_no_args", false},
        {"field_date_return_proc_no_args", false},
        {"field_datetime_return_proc_no_args", false},
        {"field_hirestime_return_proc_no_args", false},
        {"field_int8_return_proc_no_args", false},
        {"field_int16_return_proc_no_args", false},
        {"field_int32_return_proc_no_args", false},
        {"field_int64_return_proc_no_args", false},
        {"field_uint8_return_proc_no_args", false},
        {"field_uint16_return_proc_no_args", false},
        {"field_uint32_return_proc_no_args", false},
        {"field_uint64_return_proc_no_args", false},
        {"field_real_return_proc_no_args", false},
        {"field_richreal_return_proc_no_args", false},
        {"field_text_return_proc_no_args", false},

        {"field_array_bool_return_proc_no_args", false},
        {"field_array_char_return_proc_no_args", false},
        {"field_array_date_return_proc_no_args", false},
        {"field_array_datetime_return_proc_no_args", false},
        {"field_array_hirestime_return_proc_no_args", false},
        {"field_array_int8_return_proc_no_args", false},
        {"field_array_int16_return_proc_no_args", false},
        {"field_array_int32_return_proc_no_args", false},
        {"field_array_int64_return_proc_no_args", false},
        {"field_array_uint8_return_proc_no_args", false},
        {"field_array_uint16_return_proc_no_args", false},
        {"field_array_uint32_return_proc_no_args", false},
        {"field_array_uint64_return_proc_no_args", false},
        {"field_array_real_return_proc_no_args", false},
        {"field_array_richreal_return_proc_no_args", false},
//        {"field_array_text_return_proc_no_args", false},
        {"field_array_return_proc_no_args", false},
        {"field_return_proc_no_args", false},

        {"one_field_table_return_proc_no_args", false},
        {"two_field_table_return_proc_no_args", false},
        {"table_return_proc_no_args", false},
        {"table_return_proc_all_type_args", false}
    };

static bool
test_proc_name_match(const char* proc_name)
{
  const char suffix[] = "_This_is_a_long_variable_name_suffix_coz_I_need_to_trigger_an_odd_behavior_001_good";
  char buffer[1024];
  const uint_t glbsCount = sizeof(_procedures)/sizeof(_procedures[0]);

  for (uint_t i = 0; i < glbsCount; ++i)
    {
      strcpy(buffer, _procedures[i].name);
      strcat(buffer, suffix);
      if (strcmp(buffer, proc_name) == 0)
        {
          if (_procedures[i].visited)
            return false;
          else
            {
              _procedures[i].visited = true;
              return true;
            }
        }
    }

  return false;
}

static bool
test_procedures_list(WH_CONNECTION hnd)
{
  const char* recvGlbName = NULL;
  const uint_t  procsCount = sizeof(_procedures)/sizeof(_procedures[0]);

  uint_t globalsCount;
  uint_t index = 0;

  cout << "Testing the procedures listing ... ";

  if ((WStartProceduresList(hnd, &globalsCount) != WCS_OK)
      || (globalsCount != procsCount))
    {
      goto test_procedures_list_error;
    }

  do
    {
      if (WFetchProcedure(hnd, &recvGlbName) != WCS_OK)
        goto test_procedures_list_error;
      else if ((recvGlbName != NULL)
               && ! test_proc_name_match(recvGlbName))
        {
          goto test_procedures_list_error;
        }

      ++index; //Only good for conditional breakpoints.
    }
  while (recvGlbName != NULL);

  for (index = 0; index < procsCount; ++index)
    {
      if (! _procedures[index].visited)
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
  uint_t          procsCount;
  const char*   nameFetched;

  cout << "Testing against error conditions ... ";
  if ((WStartProceduresList(NULL, &procsCount) != WCS_INVALID_ARGS)
      || (WStartProceduresList(NULL, NULL) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if (WFetchProcedure(hnd, &nameFetched) != WCS_INVALID_ARGS)
    goto test_for_errors_fail;

  else if (WStartProceduresList(hnd, NULL) != WCS_OK)
    goto test_for_errors_fail;

  else if ((WFetchProcedure(NULL, NULL) != WCS_INVALID_ARGS)
           || (WFetchProcedure(NULL, &nameFetched) != WCS_INVALID_ARGS)
           || (WFetchProcedure(hnd, NULL) != WCS_INVALID_ARGS))
    {
      goto test_for_errors_fail;
    }
  else if (WFetchProcedure(hnd, &nameFetched) != WCS_OK)
    goto test_for_errors_fail;

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

  WClose(hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }


  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

