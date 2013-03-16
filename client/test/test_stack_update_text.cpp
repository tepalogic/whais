/*
 * test_stack_update_text.cpp
 *
 *  Created on: Mar 6, 2013
 *      Author: ipopa
 */

#include <iostream>
#include <cstring>

#include "utils/include/random.h"
#include "utils/include/utf8.h"
#include "test_client_common.h"

using namespace std;

const D_UINT64 MAX_STRING_SIZE  = 32768;
const D_UINT64 MAX_REFS_STRINGS = 5;

const D_CHAR* _refStrings[MAX_REFS_STRINGS] =
    {
        "This is the first string!",
        "This is a second string!",
        "I'm the weird one.",
        "Supported currency: \xC2\xA2, \xC2\xA5, $, \xC2\xA4",
        "This is the end."
    };

const W_FieldDescriptor  _fields[] =
    {
        {"WFT_TEXT", WFT_TEXT},
        {"WFT_TEXT_2", WFT_TEXT},
    };

static const D_UINT _fieldsCount = sizeof _fields / sizeof (_fields[0]);

const D_CHAR*
insert_a_string (W_CONNECTOR_HND        hnd,
                 const D_CHAR*          fieldName,
                 const D_UINT64         row,
                 const bool             bulk)
{
  const D_UINT thisStringSize = w_rnd () % MAX_STRING_SIZE;

  D_CHAR* result = new D_CHAR[MAX_STRING_SIZE];

  memset (result, 0, MAX_STRING_SIZE);

  while (true)
    {
      const D_CHAR* temp = _refStrings[w_rnd () % MAX_REFS_STRINGS];

      if (strlen (result) + strlen (temp) >= thisStringSize)
        break;

      if ((WUpdateStackValue (hnd,
                              WFT_TEXT,
                              fieldName,
                              row,
                              WIGNORE_OFF,
                              utf8_strlen (_RC (D_UINT8*, result)),
                              temp) != WCS_OK)
          || (bulk && (WUpdateStackFlush (hnd) != WCS_OK)))
        {
          return NULL;
        }

      strcat (result, temp);
    }

  if (WUpdateStackFlush (hnd) != WCS_OK)
    return NULL;

  return result;
}


static bool
test_simple_text (W_CONNECTOR_HND hnd)
{
  D_UINT              aSimpleOffset = w_rnd () % 7;

  const D_CHAR*       ref;
  D_CHAR              aValue[MAX_STRING_SIZE];
  const D_CHAR*       value;
  unsigned long long  count;
  D_UINT              rawType;

  cout << "Testing text simple updates ... ";

  if ((WPushStackValue (hnd, WFT_TEXT, 0, NULL) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
    {
      goto test_simple_text_fail;
    }

  ref = insert_a_string (hnd, WIGNORE_FIELD, WIGNORE_OFF, false);
  if ((WGetStackTextLengthCount (hnd,
                                 WIGNORE_FIELD,
                                 WIGNORE_ROW,
                                 WIGNORE_OFF,
                                 &count) != WCS_OK)
      || (_SC (D_INT, count) != utf8_strlen (_RC (const D_UINT8*, ref)))
      || (WDescribeStackTop (hnd, &rawType) != WCS_OK)
      || (rawType != WFT_TEXT))
    {
      goto test_simple_text_fail;
    }

  aValue[0] = 0;
  while (strlen (aValue) < strlen (ref + aSimpleOffset))
    {
      if (WGetStackValueEntry (hnd,
                               WIGNORE_FIELD,
                               WIGNORE_ROW,
                               WIGNORE_OFF,
                               utf8_strlen (_RC (D_UINT8*, aValue)) + aSimpleOffset,
                               &value) != WCS_OK)
        {
          goto test_simple_text_fail;
        }
      strcat (aValue, value);
    }

  if (strcmp (aValue, ref + aSimpleOffset) != 0)
    goto test_simple_text_fail;

  delete [] ref;

  if (WPushStackValue (hnd, WFT_TEXT, 0, NULL) != WCS_OK)
    goto test_simple_text_fail;

  ref = insert_a_string (hnd, WIGNORE_FIELD, WIGNORE_OFF, true);
  if ((WGetStackTextLengthCount (hnd,
                                 WIGNORE_FIELD,
                                 WIGNORE_ROW,
                                 WIGNORE_OFF,
                                 &count) != WCS_OK)
      || (_SC (D_INT, count) != utf8_strlen (_RC (const D_UINT8*, ref)))
      || (WDescribeStackTop (hnd, &rawType) != WCS_OK)
      || (rawType != WFT_TEXT))
    {
      goto test_simple_text_fail;
    }

  aSimpleOffset = w_rnd () % 7;
  aValue[0]     = 0;
  while (strlen (aValue) < strlen (ref + aSimpleOffset))
    {
      if (WGetStackValueEntry (hnd,
                               WIGNORE_FIELD,
                               WIGNORE_ROW,
                               WIGNORE_OFF,
                               utf8_strlen (_RC (D_UINT8*, aValue)) + aSimpleOffset,
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
test_table_text (W_CONNECTOR_HND hnd)
{

  const D_UINT        rowsCount = 2;

  const D_CHAR*       ref[rowsCount * _fieldsCount];
  const D_CHAR*       value;
  D_UINT              aSimpleOffset;
  D_CHAR              aValue[MAX_STRING_SIZE];

  if ((WPushStackValue (hnd, WFT_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
    {
      goto test_table_text_fail;
    }


  for (D_UINT row = 0; row < rowsCount; row++)
    {

      ref[row * _fieldsCount] = insert_a_string (hnd,
                                                 "WFT_TEXT",
                                                 row,
                                                 true);

      ref[row * _fieldsCount + 1] = insert_a_string (hnd,
                                                     "WFT_TEXT_2",
                                                     row,
                                                     false);
    }

  for (D_UINT row = 0; row < rowsCount; row++)
    {
      aSimpleOffset = w_rnd () % 7;
      aValue[0]     = 0;
      while (strlen (aValue) < strlen (ref[row * _fieldsCount] + aSimpleOffset))
        {
          if (WGetStackValueEntry (hnd,
                                   "WFT_TEXT",
                                   row,
                                   WIGNORE_OFF,
                                   utf8_strlen (_RC (D_UINT8*, aValue)) + aSimpleOffset,
                                   &value) != WCS_OK)
            {
              goto test_table_text_fail;
            }
          strcat (aValue, value);
        }
      if (strcmp (aValue, ref[row * _fieldsCount] + aSimpleOffset) != 0)
        goto test_table_text_fail;

      aSimpleOffset = w_rnd () % 7;
      aValue[0]     = 0;
      while (strlen (aValue) < strlen (ref[row * _fieldsCount + 1] + aSimpleOffset))
        {
          if (WGetStackValueEntry (hnd,
                                   "WFT_TEXT_2",
                                   row,
                                   WIGNORE_OFF,
                                   utf8_strlen (_RC (D_UINT8*, aValue)) + aSimpleOffset,
                                   &value) != WCS_OK)
            {
              goto test_table_text_fail;
            }
          strcat (aValue, value);
        }
      if (strcmp (aValue, ref[row * _fieldsCount + 1] + aSimpleOffset) != 0)
        goto test_table_text_fail;
    }

  for (D_UINT i = 0; i < rowsCount; ++i)
    {
      delete [] ref[i * _fieldsCount];
      delete [] ref[i * _fieldsCount + 1];
    }

  cout << "OK\n";
  return true;

test_table_text_fail:

  for (D_UINT i = 0; i < rowsCount; ++i)
  {
    delete [] ref[i * _fieldsCount];
    delete [] ref[i * _fieldsCount + 1];
  }

  cout << "FAIL\n";
  return false;
}



static bool
test_for_errors (W_CONNECTOR_HND hnd)
{
  unsigned long long count;

  cout << "Testing against error conditions ... ";

  if ((WPushStackValue (hnd, WFT_TEXT | WFT_ARRAY_MASK, 0, NULL) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OP_NOTSUPP)
      || (WPushStackValue (hnd, WFT_TEXT, 0, NULL) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK)
      || (WGetStackValueRowsCount (hnd, &count) != WCS_TYPE_MISMATCH)
      || (WGetStackArrayElementsCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, &count) != WCS_TYPE_MISMATCH)
      || (WGetStackArrayElementsCount (hnd, "some_f", WIGNORE_ROW, &count) != WCS_INVALID_FIELD)
      || (WGetStackArrayElementsCount (hnd, WIGNORE_FIELD, 0, &count) != WCS_INVALID_ROW)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, 0, &count) != WCS_TYPE_MISMATCH)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_OK)
      || (count != 0)
      || (WGetStackTextLengthCount (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW))
    {
      goto test_for_errors_fail;
    }

  if ((WPushStackValue (hnd, WFT_TABLE_MASK, _fieldsCount, _fields) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK) //Just for fun!
      || (WGetStackValueRowsCount (NULL, NULL) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (NULL, &count) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (hnd, NULL) != WCS_INVALID_ARGS)
      || (WGetStackValueRowsCount (hnd, &count) != WCS_OK)
      || (count != 0)
      || (WGetStackTextLengthCount (hnd, WIGNORE_FIELD, WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WGetStackTextLengthCount (hnd, "some_f", WIGNORE_ROW, WIGNORE_OFF, &count) != WCS_INVALID_FIELD)
      || (WGetStackTextLengthCount (hnd, "WFT_TEXT", 0, WIGNORE_OFF, &count) != WCS_INVALID_ROW)
      || (WPopStackValues (hnd, WPOP_ALL) != WCS_OK)
      || (WUpdateStackFlush (hnd) != WCS_OK))
    {
      goto test_for_errors_fail;
    }

  cout << "OK\n";
  return true;

test_for_errors_fail:

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
