/*
 * test_dbstext.cpp
 *
 *  Created on: Oct 2, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"
#include "../include/dbs_values.h"

#include "../pastra/ps_varstorage.h"
#include "../pastra/ps_arraystrategy.h"

using namespace pastra;

bool test_bool_array ()
{
  std::cout << "Testing T_BOOL array... ";
  bool result = true;
  DBSArray anotherArray ((DBSBool *)NULL);

  if (result)
    {
      DBSArray nullArray ((DBSBool *)NULL);
      if (! nullArray.IsNull())
        result = false;

      if (! anotherArray.IsNull ())
        result = false;

      if ( anotherArray.AddElement(DBSBool (true)) != 0)
        result = false;
      else if  (anotherArray.AddElement(DBSBool (false)) != 1)
        result = false;

      if ( ! (anotherArray.IsNull() == false) && (anotherArray.ElementsCount() != 2) )
        result = false;

      DBSBool tv;
      anotherArray.GetElement( tv, 0);
      if ((tv.IsNull()) || (tv.m_Value == false))
        result = false;

      anotherArray.GetElement( tv, 1);
      if ((tv.IsNull()) || (tv.m_Value == true))
        result = false;
    }

  if (result)
    {


      anotherArray.SetElement (DBSBool(true), 1);
      DBSBool tv;
      anotherArray.GetElement( tv, 1);
      if ((tv.IsNull()) || (tv.m_Value == false))
        result = false;

      const DBSBool nullBull;
      anotherArray.SetElement (nullBull, 0);
      anotherArray.GetElement( tv, 0);
      if ((tv.IsNull()) || (tv.m_Value == false) || (anotherArray.ElementsCount() > 1))
        result = false;

      anotherArray.SetElement (nullBull, 0);
      if (anotherArray.IsNull() == false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

template <typename T, typename Tint> bool
test_integers_array (const char* type)
{
  std::cout << "Testing " << type << " array... ";
  bool result = true;

  uint64_t elementsCount = 0;
  T nullValue;
  DBSArray testArray((T *)NULL);

  for (uint64_t index = 1; index < 10000; index += 13)
    {
      if (testArray.ElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      //Limit to 0xFF because of VC++ run time check!
      //Don't worry C++0x will take care of this in future.
      testArray.AddElement (T(index & 0xFF));
      ++elementsCount;
    }

  if (result)
    {
      testArray.RemoveElement(0);
      testArray.SetElement (nullValue, elementsCount - 2);
      elementsCount -= 2;

      if (testArray.ElementsCount() != elementsCount)
        result = false;
    }

  if (result)
    {
      T testValue;
      int64_t content = 14;
      for (uint_t index = 0; index < elementsCount; index++)
        {
          testArray.GetElement(testValue, index);
          if (testValue.IsNull() || ((content & 0xFF) != testValue.m_Value))
            {
              result = false;
              break;
            }

          content += 13;

          static const Tint mask = ~0;
          content &= mask;
        }
    }


  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_dates_array ()
{
  std::cout << "Testing T_DATE array... ";
  bool result = true;

  uint_t elementsCount = 0;
  int year = 1;
  uint_t day = 1;
  uint_t month = 1;

  DBSArray testArray((DBSDate *)NULL);

  for (uint64_t elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.ElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      testArray.AddElement (DBSDate (year, month, day));

      year += 7;
      day += 3; day %= 25; day++;
      month += 7; month %= 12; month++;
    }


  //Read back;
  year = day = month = 1;
  for (uint64_t index = 0; index < elementsCount; ++index)
  {
    DBSDate testValue;
    testArray.GetElement (testValue, index);

    if ((testValue.m_IsNull != false) ||
        (testValue.m_Year != year) ||
        (testValue.m_Day != day) ||
        (testValue.m_Month != month))
      {
        result = false;
        break;
      }

    year += 7;
    day += 3; day %= 25; day++;
    month += 7; month %= 12; month++;
  }

  if (result)
    {
      for (uint_t index = 0; index < elementsCount; ++ index)
        testArray.RemoveElement (0);

      if (testArray.IsNull () != false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_datetimes_array ()
{
  std::cout << "Testing T_DATETIME array... ";
  bool result = true;

  uint_t elementsCount = 0;
  int year = 1;
  uint_t day = 1;
  uint_t month = 1;
  uint_t hour = 3;
  uint_t mins = 3;
  uint_t sec = 3;

  DBSArray testArray((DBSDateTime *)NULL);

  for (uint64_t elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.ElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      testArray.AddElement (DBSDateTime (year, month, day, hour, mins, sec));

      year += 7;
      day += 3; day %= 25; day++;
      month += 7; month %= 12; month++;
      hour += 3; hour %= 23;
      mins += 11; mins %= 60;
      sec += 23; sec %= 60;
    }


  //Read back;
  year = day = month = 1;
  hour = mins = sec = 3;
  for (uint64_t index = 0; index < elementsCount; ++index)
  {
    DBSDateTime testValue;
    testArray.GetElement (testValue, index);

    if ((testValue.m_IsNull != false) ||
        (testValue.m_Year != year) ||
        (testValue.m_Day != day) ||
        (testValue.m_Month != month) ||
        (testValue.m_Hour != hour) ||
        (testValue.m_Minutes != mins) ||
        (testValue.m_Seconds != sec))
      {
        result = false;
        break;
      }

    year += 7;
    day += 3; day %= 25; day++;
    month += 7; month %= 12; month++;
    hour += 3; hour %= 23;
    mins += 11; mins %= 60;
    sec += 23; sec %= 60;
  }

  if (result)
    {
      for (uint_t index = 0; index < elementsCount; ++ index)
        testArray.RemoveElement (0);

      if (testArray.IsNull () != false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_hiresdate_array ()
{
  std::cout << "Testing T_HIRESTIME array... ";
  bool result = true;

  uint_t elementsCount = 0;
  int year = 1;
  uint_t day = 1;
  uint_t month = 1;
  uint_t hour = 3;
  uint_t mins = 3;
  uint_t sec = 3;
  uint32_t msec = 4;

  DBSArray testArray((DBSHiresTime *)NULL);

  for (uint64_t elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.ElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      testArray.AddElement (DBSHiresTime (year, month, day, hour, mins, sec, msec));

      year += 7;
      day += 3; day %= 25; day++;
      month += 7; month %= 12; month++;
      hour += 3; hour %= 23;
      mins += 11; mins %= 60;
      sec += 23; sec %= 60;
      msec += 13; msec %= 1000000;
    }


  //Read back;
  year = day = month = 1;
  hour = mins = sec = 3;
  msec = 4;
  for (uint64_t index = 0; index < elementsCount; ++index)
  {
    DBSHiresTime testValue;
    testArray.GetElement (testValue, index);

    if ((testValue.m_IsNull != false) ||
        (testValue.m_Year != year) ||
        (testValue.m_Day != day) ||
        (testValue.m_Month != month) ||
        (testValue.m_Hour != hour) ||
        (testValue.m_Minutes != mins) ||
        (testValue.m_Seconds != sec) ||
        (testValue.m_Microsec != msec))
      {
        result = false;
        break;
      }

    year += 7;
    day += 3; day %= 25; day++;
    month += 7; month %= 12; month++;
    hour += 3; hour %= 23;
    mins += 11; mins %= 60;
    sec += 23; sec %= 60;
    msec += 13; msec %= 1000000;
  }

  if (result)
    {
      for (uint_t index = 0; index < elementsCount; ++ index)
        testArray.RemoveElement (0);

      if (testArray.IsNull () != false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_array_mirroring ()
{
  static uint32_t firstVals[] = {10, 1, 233, 0x4545};
  static uint32_t secondVals[] = {0, 12, 45};
  std::cout << "Testing array mirroring ...";
  bool result = true;

  DBSArray array1 ((DBSUInt32*)NULL);
  DBSArray array2 ((DBSUInt32*)NULL);

  array1.AddElement (DBSUInt32 (firstVals[0]));
  array1.AddElement (DBSUInt32 (firstVals[1]));
  array1.AddElement (DBSUInt32 (firstVals[2]));

  array2.AddElement (DBSUInt32 (secondVals[0]));
  array2.AddElement (DBSUInt32 (secondVals[1]));
  array2.AddElement (DBSUInt32 (secondVals[2]));

  if (array2.ElementsCount () != array1.ElementsCount ())
    result = false;

  for (uint_t index = 0; result && (index < array1.ElementsCount ()); ++index)
    {
      DBSUInt32 temp;
      array1.GetElement (temp, index);

      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.GetElement (temp, index);
      if (temp.m_Value != secondVals[index])
        {
          result = false;
          continue;
       }
    }

  array2 = array1;

  if (array2.ElementsCount () != array1.ElementsCount ())
    result = false;
  for (uint_t index = 0; result && (index < array1.ElementsCount ()); ++index)
    {
      DBSUInt32 temp;
      array1.GetElement (temp, index);

      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.GetElement (temp, index);
      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }
    }

  array1.SetElement(DBSUInt32 (0xFFFF), 0);
  if (array2.ElementsCount () != array1.ElementsCount ())
    result = false;
  for (uint_t index = 0; result && (index < array1.ElementsCount ()); ++index)
    {
      DBSUInt32 temp;
      array1.GetElement (temp, index);

      if (index == 0)
        {
          if (temp.m_Value != 0xFFFF)
            {
              result = false;
              continue;
            }
        }
      else if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.GetElement (temp, index);
      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }
    }

  array2.SetMirror (array1);
  if (array2.ElementsCount () != array1.ElementsCount ())
    result = false;
  for (uint_t index = 0; result && (index < array1.ElementsCount ()); ++index)
    {
      DBSUInt32 temp;
      array1.GetElement (temp, index);

      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.GetElement (temp, index);
      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }
    }

  array1.AddElement (DBSUInt32 (firstVals [3]));
  if (array2.ElementsCount () != array1.ElementsCount ())
    result = false;
  for (uint_t index = 0; result && (index < array1.ElementsCount ()); ++index)
    {
      DBSUInt32 temp;
      array1.GetElement (temp, index);

      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.GetElement (temp, index);
      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }
    }

  DBSArray array3((DBSUInt32*) NULL);
  array3 = array1;

  for (uint_t index = 0; result && (index < array1.ElementsCount ()); ++index)
    {
      DBSUInt32 temp;
      array1.GetElement (temp, index);

      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.GetElement (temp, index);
      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }

      array3.GetElement (temp, index);
      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }
    }

  array3.SetElement (DBSUInt32 (0), 0);
  for (uint_t index = 0; result && (index < array1.ElementsCount ()); ++index)
    {
      DBSUInt32 temp;
      array1.GetElement (temp, index);

      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.GetElement (temp, index);
      if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }

      array3.GetElement (temp, index);
      if ((index == 0))
        {
          if (temp.m_Value != 0)
            {
              result = false;
              continue;
            }
        }
      else if (temp.m_Value != firstVals[index])
        {
          result = false;
          continue;
       }

    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

int
main ()
{
  bool success = true;

  DBSInit (DBSSettings ());

  success = success && test_bool_array ();
  success = success && test_integers_array<DBSUInt8, uint8_t> ("T_UINT8");
  success = success && test_integers_array<DBSUInt16, uint16_t> ("T_UINT16");
  success = success && test_integers_array<DBSUInt32, uint32_t> ("T_UINT32");
  success = success && test_integers_array<DBSInt64, int64_t> ("T_INT64");
  success = success && test_dates_array ();
  success = success && test_datetimes_array ();
  success = success && test_hiresdate_array ();
  success = success && test_array_mirroring ();

  DBSShoutdown ();

  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
