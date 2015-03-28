/*
 * test_stack_update_text.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: ipopa
 */

#include <iostream>
#include <cstring>

#include "utils/wrandom.h"
#include "utils/wutf.h"
#include "test_client_common.h"

using namespace std;

const uint64_t MAX_STRING_SIZE  = 32768;
const uint64_t MAX_REFS_STRINGS = 5;

const char* _refStrings[MAX_REFS_STRINGS] =
    {
        "This is the first string!",
        "This is a second string!",
        "I'm the weird one.",
        "Supported currency: \xC2\xA2, \xC2\xA5, $, \xC2\xA4",
        "This is the end."
    };

const WField  _fields[] =
    {
        {"WHC_TYPE_TEXT", WHC_TYPE_TEXT},
        {"WHC_TYPE_TEXT_2", WHC_TYPE_TEXT},
    };

static const uint_t _fieldsCount = sizeof _fields / sizeof (_fields[0]);

const char*
insert_a_string (WH_CONNECTION        hnd,
                 const char*          fieldName,
                 const uint64_t         row,
                 const bool             bulk)
{
  const uint_t thisStringSize = wh_rnd () % MAX_STRING_SIZE;

  char* result = new char[MAX_STRING_SIZE];

  memset (result, 0, MAX_STRING_SIZE);

  while (true)
    {
      const char* temp = _refStrings[wh_rnd () % MAX_REFS_STRINGS];

      if (strlen (result) + strlen (temp) >= thisStringSize)
        break;

      if ((WUpdateValue (hnd,
                         WHC_TYPE_TEXT,
                         fieldName,
                         row,
                         WIGNORE_OFF,
                         wh_utf8_strlen (_RC (uint8_t*, result)),
                         temp) != WCS_OK)
          || (bulk && (WFlush (hnd) != WCS_OK)))
        {
          return NULL;
        }

      strcat (result, temp);
    }

  if (WFlush (hnd) != WCS_OK)
    return NULL;

  return result;
}


static bool
test_simple_text (WH_CONNECTION hnd)
{
  uint_t              aSimpleOffset = wh_rnd () % 7;

  const char*       ref;
  char              aValue[MAX_STRING_SIZE];
  const char*       value;
  unsigned long long  count;
  uint_t              rawType;

  cout << "Testing text simple updates ... ";

  if ((WPushValue (hnd, WHC_TYPE_TEXT, 0, NULL) != WCS_OK)
      || (WFlush (hnd) != WCS_OK))
    {
      goto test_simple_text_fail;
    }

  ref = insert_a_string (hnd, WIGNORE_FIELD, WIGNORE_OFF, false);
  if ((WValueTextLength (hnd,
                         WIGNORE_FIELD,
                         WIGNORE_ROW,
                         WIGNORE_OFF,
                         &count) != WCS_OK)
      || (_SC (int, count) != wh_utf8_strlen (_RC (const uint8_t*, ref)))
      || (WDescribeStackTop (hnd, &rawType) != WCS_OK)
      || (rawType != WHC_TYPE_TEXT))
    {
      goto test_simple_text_fail;
    }

  aValue[0] = 0;
  while (strlen (aValue) < strlen (ref + aSimpleOffset))
    {
      if (WValueEntry (hnd,
                       WIGNORE_FIELD,
                       WIGNORE_ROW,
                       WIGNORE_OFF,
                       wh_utf8_strlen (_RC (uint8_t*, aValue)) + aSimpleOffset,
                       &value) != WCS_OK)
        {
          goto test_simple_text_fail;
        }
      strcat (aValue, value);
    }

  if (strcmp (aValue, ref + aSimpleOffset) != 0)
    goto test_simple_text_fail;

  delete [] ref;

  if (WPushValue (hnd, WHC_TYPE_TEXT, 0, NULL) != WCS_OK)
    goto test_simple_text_fail;

  ref = insert_a_string (hnd, WIGNORE_FIELD, WIGNORE_OFF, true);
  if ((WValueTextLength (hnd,
                         WIGNORE_FIELD,
                         WIGNORE_ROW,
                         WIGNORE_OFF,
                         &count) != WCS_OK)
      || (_SC (int, count) != wh_utf8_strlen (_RC (const uint8_t*, ref)))
      || (WDescribeStackTop (hnd, &rawType) != WCS_OK)
      || (rawType != WHC_TYPE_TEXT))
    {
      goto test_simple_text_fail;
    }

  aSimpleOffset = wh_rnd () % 7;
  aValue[0]     = 0;
  while (strlen (aValue) < strlen (ref + aSimpleOffset))
    {
      if (WValueEntry (hnd,
                       WIGNORE_FIELD,
                       WIGNORE_ROW,
                       WIGNORE_OFF,
                       wh_utf8_strlen (_RC (uint8_t*, aValue)) + aSimpleOffset,
                       &value) != WCS_OK)
        {
          goto test_simple_text_fail;
        }
      strcat (aValue, value);
    }

  if (strcmp (aValue, ref + aSimpleOffset) != 0)
    goto test_simple_text_fail;

  delete [] ref;
  cout << "OK\n";
  return true;

test_simple_text_fail:

  delete [] ref;
  cout << "FAIL\n";
  return false;
}

static bool
test_table_text (WH_CONNECTION hnd)
{

  const uint_t        rowsCount = 2;

  const char*       ref[rowsCount * _fieldsCount];
  const char*       value;
  uint_t              aSimpleOffset;
  char              aValue[MAX_STRING_SIZE];

  if ((WPushValue (hnd, WHC_TYPE_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
      || (WFlush (hnd) != WCS_OK))
    {
      goto test_table_text_fail;
    }


  for (uint_t row = 0; row < rowsCount; row++)
    {

      ref[row * _fieldsCount] = insert_a_string (hnd,
                                                 "WHC_TYPE_TEXT",
                                                 row,
                                                 true);

      ref[row * _fieldsCount + 1] = insert_a_string (hnd,
                                                     "WHC_TYPE_TEXT_2",
                                                     row,
                                                     false);
    }

  for (uint_t row = 0; row < rowsCount; row++)
    {
      aSimpleOffset = wh_rnd () % 7;
      aValue[0]     = 0;
      while (strlen (aValue) < strlen (ref[row * _fieldsCount] + aSimpleOffset))
        {
          if (WValueEntry (hnd,
                                   "WHC_TYPE_TEXT",
                                   row,
                                   WIGNORE_OFF,
                                   wh_utf8_strlen (_RC (uint8_t*, aValue)) + aSimpleOffset,
                                   &value) != WCS_OK)
            {
              goto test_table_text_fail;
            }
          strcat (aValue, value);
        }
      if (strcmp (aValue, ref[row * _fieldsCount] + aSimpleOffset) != 0)
        goto test_table_text_fail;

      aSimpleOffset = wh_rnd () % 7;
      aValue[0]     = 0;
      while (strlen (aValue) < strlen (ref[row * _fieldsCount + 1] + aSimpleOffset))
        {
          if (WValueEntry (hnd,
                           "WHC_TYPE_TEXT_2",
                           row,
                           WIGNORE_OFF,
                           wh_utf8_strlen (_RC (uint8_t*, aValue)) + aSimpleOffset,
                           &value) != WCS_OK)
            {
              goto test_table_text_fail;
            }
          strcat (aValue, value);
        }
      if (strcmp (aValue, ref[row * _fieldsCount + 1] + aSimpleOffset) != 0)
        goto test_table_text_fail;
    }

  for (uint_t i = 0; i < rowsCount; ++i)
    {
      delete [] ref[i * _fieldsCount];
      delete [] ref[i * _fieldsCount + 1];
    }

  cout << "OK\n";
  return true;

test_table_text_fail:

  for (uint_t i = 0; i < rowsCount; ++i)
  {
    delete [] ref[i * _fieldsCount];
    delete [] ref[i * _fieldsCount + 1];
  }

  cout << "FAIL\n";
  return false;
}



static bool
test_for_errors (WH_CONNECTION hnd)
{
  unsigned long long count;

  cout << "Testing against error conditions ... ";

  if ((WPushValue (hnd, WHC_TYPE_TEXT | WHC_TYPE_ARRAY_MASK, 0, NULL) != WCS_OK)
      || (WFlush (hnd) != WCS_OP_NOTSUPP)
      || (WPushValue (hnd, WHC_TYPE_TEXT, 0, NULL) != WCS_OK)
      || (WFlush (hnd) != WCS_OK)
      || (WValueRowsCount (hnd, &count) != WCS_TYPE_MISMATCH)
      || (WValueArraySize (hnd, WIGNORE_FIELD, WIGNORE_ROW, &count) != WCS_TYPE_MISMATCH)
      || (WValueArraySize (hnd, "some_f", WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WValueArraySize (hnd, WIGNORE_FIELD, 0, &count) != WCS_INVALID_ROW)
      || (WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_ROW, 0, &count) != WCS_TYPE_MISMATCH)
      || (WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_OK)
      || (count != 0)
      || (WValueTextLength (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WValueTextLength (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW))
    {
      goto test_for_errors_fail;
    }

  if ((WPushValue (hnd, WHC_TYPE_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
      || (WFlush (hnd) != WCS_OK)
      || (WFlush (hnd) != WCS_OK) //Just for fun!
      || (WValueRowsCount (NULL, NULL) != WCS_INVALID_ARGS)
      || (WValueRowsCount (NULL, &count) != WCS_INVALID_ARGS)
      || (WValueRowsCount (hnd, NULL) != WCS_INVALID_ARGS)
      || (WValueRowsCount (hnd, &count) != WCS_OK)
      || (count != 0)
      || (WValueTextLength (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WValueTextLength (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WValueTextLength (hnd, "WHC_TYPE_TEXT", 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW)
      || (WPopValues (hnd, WPOP_ALL) != WCS_OK)
      || (WFlush (hnd) != WCS_OK))
    {
      goto test_for_errors_fail;
    }

  cout << "OK\n";
  return true;

test_for_errors_fail:

  cout << "FAIL\n";
  return false;
}

const char*
DefaultDatabaseName()
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
  success = success && test_simple_text (hnd);
  success = success && test_table_text (hnd);

  WClose (hnd);

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}
