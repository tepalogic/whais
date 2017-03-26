/*
 * test_dbstext.cpp
 *
 *  Created on: Oct 2, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"

#include "../pastra/ps_varstorage.h"
#include "../pastra/ps_arraystrategy.h"

using namespace whais;
using namespace pastra;

bool test_bool_array()
{
  std::cout << "Testing T_BOOL array... ";
  bool result = true;
  DArray anotherArray((DBool *)nullptr);

  if (result)
  {
    DArray nullArray((DBool *)nullptr);
    if (! nullArray.IsNull())
      result = false;

    if (! anotherArray.IsNull())
      result = false;

    if ( anotherArray.Add(DBool(true)) != 0)
      result = false;
    else if  (anotherArray.Add(DBool(false)) != 1)
      result = false;

    if ( ! (anotherArray.IsNull() == false) && (anotherArray.Count() != 2) )
      result = false;

    DBool tv;
    anotherArray.Get(0,  tv);
    if ((tv.IsNull()) || (tv.mValue == false))
      result = false;

    anotherArray.Get(1,  tv);
    if ((tv.IsNull()) || (tv.mValue == true))
      result = false;
  }

  if (result)
  {
    anotherArray.Set(1, DBool(true));
    DBool tv;
    anotherArray.Get(1,  tv);
    if (tv.IsNull() || tv.mValue == false)
      result = false;

    const DBool nullBull;
    anotherArray.Set(0, nullBull);
    anotherArray.Get(0,  tv);
    if ((tv.IsNull()) || (tv.mValue == false) || (anotherArray.Count() > 1))
      result = false;

    anotherArray.Set(0, nullBull);
    if (anotherArray.IsNull() == false)
      result = false;
  }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

template <typename T, typename Tint> bool
test_integers_array(const char* type)
{
  std::cout << "Testing " << type << " array... ";
  bool result = true;

  uint64_t elementsCount = 0;
  T nullValue;
  DArray testArray((T *)nullptr);

  for (uint64_t index = 1; index < 10000; index += 13)
  {
    if (testArray.Count() != elementsCount)
    {
      result = false;
      break;
    }
    //Limit to 0xFF because of VC++ run time check!
    //Don't worry C++0x will take care of this in future.
    testArray.Add(T(index & 0xFF));
    ++elementsCount;
  }

  if (result)
  {
    testArray.Remove(0);
    testArray.Set(elementsCount - 2, nullValue);
    elementsCount -= 2;

    if (testArray.Count() != elementsCount)
      result = false;
  }

  if (result)
  {
    T testValue;
    int64_t content = 14;
    for (uint_t index = 0; index < elementsCount; index++)
    {
      testArray.Get(index, testValue);
      if (testValue.IsNull() || ((content & 0xFF) != testValue.mValue))
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
test_dates_array()
{
  std::cout << "Testing T_DATE array... ";
  bool result = true;

  uint_t elementsCount = 0;
  int year = 1;
  uint_t day = 1;
  uint_t month = 1;

  DArray testArray((DDate *)nullptr);

  for (uint64_t elementsCount = 0; elementsCount < 500;  elementsCount++)
  {
    if (testArray.Count() != elementsCount)
    {
      result = false;
      break;
    }
    testArray.Add(DDate(year, month, day));

    year += 7;
    day += 3; day %= 25; day++;
    month += 7; month %= 12; month++;
  }

  //Read back;
  year = day = month = 1;
  for (uint64_t index = 0; index < elementsCount; ++index)
  {
    DDate testValue;
    testArray.Get(index, testValue);

    if ((testValue.mIsNull != false) ||
        (testValue.mYear != year) ||
        (testValue.mDay != day) ||
        (testValue.mMonth != month))
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
      testArray.Remove(0);

    if (testArray.IsNull() != false)
      result = false;
  }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_datetimes_array()
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

  DArray testArray((DDateTime *)nullptr);

  for (uint64_t elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.Count() != elementsCount)
        {
          result = false;
          break;
        }
      testArray.Add(DDateTime(year, month, day, hour, mins, sec));

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
    DDateTime testValue;
    testArray.Get(index, testValue);

    if ((testValue.mIsNull != false) ||
        (testValue.mYear != year) ||
        (testValue.mDay != day) ||
        (testValue.mMonth != month) ||
        (testValue.mHour != hour) ||
        (testValue.mMinutes != mins) ||
        (testValue.mSeconds != sec))
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
        testArray.Remove(0);

      if (testArray.IsNull() != false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_hiresdate_array()
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

  DArray testArray((DHiresTime *)nullptr);

  for (uint64_t elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.Count() != elementsCount)
        {
          result = false;
          break;
        }
      testArray.Add(DHiresTime(year, month, day, hour, mins, sec, msec));

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
    DHiresTime testValue;
    testArray.Get(index, testValue);

    if ((testValue.mIsNull != false) ||
        (testValue.mYear != year) ||
        (testValue.mDay != day) ||
        (testValue.mMonth != month) ||
        (testValue.mHour != hour) ||
        (testValue.mMinutes != mins) ||
        (testValue.mSeconds != sec) ||
        (testValue.mMicrosec != msec))
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
        testArray.Remove(0);

      if (testArray.IsNull() != false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_array_copy_optimizations()
{
  static uint32_t firstVals[] = {10, 1, 233, 0x4545};
  static uint32_t secondVals[] = {0, 12, 45};
  std::cout << "Testing array mirroring ...";
  bool result = true;

  DArray array1((DUInt32*)nullptr);
  DArray array2((DUInt32*)nullptr);

  array1.Add(DUInt32(firstVals[0]));
  array1.Add(DUInt32(firstVals[1]));
  array1.Add(DUInt32(firstVals[2]));

  array2.Add(DUInt32(secondVals[0]));
  array2.Add(DUInt32(secondVals[1]));
  array2.Add(DUInt32(secondVals[2]));

  if (array2.Count() != array1.Count())
    result = false;

  for (uint_t index = 0; result && (index < array1.Count()); ++index)
    {
      DUInt32 temp;
      array1.Get(index, temp);

      if (temp.mValue != firstVals[index])
        {
          result = false;
          continue;
        }

      array2.Get(index, temp);
      if (temp.mValue != secondVals[index])
        {
          result = false;
          continue;
       }
    }

  array2 = array1;
  if (array2.Count() != array1.Count())
    result = false;
  for (uint_t index = 0; result && (index < array1.Count()); ++index)
  {
    DUInt32 temp;
    array1.Get(index, temp);

    if (temp.mValue != firstVals[index])
    {
      result = false;
      continue;
    }

    array2.Get(index, temp);
    if (temp.mValue != firstVals[index])
    {
      result = false;
      continue;
    }
  }

  array1.Set(0, DUInt32(0xFFFF));
  if (array2.Count() != array1.Count())
    result = false;
  for (uint_t index = 0; result && (index < array1.Count()); ++index)
  {
    DUInt32 temp;
    array1.Get(index, temp);

    if (index == 0)
    {
      if (temp.mValue != 0xFFFF)
      {
        result = false;
        continue;
      }
    }
    else if (temp.mValue != firstVals[index])
    {
      result = false;
      continue;
    }

    array2.Get(index, temp);
    if (temp.mValue != firstVals[index])
    {
      result = false;
      continue;
    }
  }

  array2 = array1;
  array2.Set(0, DUInt32(secondVals[0]));

  if (array2.Count() != array1.Count())
    result = false;
  for (uint_t index = 0; result && (index < array1.Count()); ++index)
  {
    DUInt32 temp;
    array1.Get(index, temp);

    if (index == 0)
    {
      if (temp.mValue != 0xFFFF)
      {
        result = false;
        continue;
      }

      array2.Get(index, temp);
      if (temp.mValue != secondVals[0])
      {
        result = false;
        continue;
      }
    }
    else if (temp.mValue != firstVals[index])
    {
      result = false;
      continue;
    }
    else
    {
      array2.Get(index, temp);
      if (temp.mValue != firstVals[index])
      {
        result = false;
        continue;
      }
    }
  }

  DArray array3((DUInt32*) nullptr);
  array1.Set(0, DUInt32(firstVals[0]));
  array3 = array2 = array1;
  if (array1.Count() != array2.Count() || array2.Count() != array3.Count())
    result = false;

  for (uint_t index = 0; result && (index < array1.Count()); ++index)
  {
    DUInt32 temp;
    array1.Get(index, temp);

    if (temp.mValue != firstVals[index])
      {
        result = false;
        continue;
      }

    array2.Get(index, temp);
    if (temp.mValue != firstVals[index])
      {
        result = false;
        continue;
     }

    array3.Get(index, temp);
    if (temp.mValue != firstVals[index])
      {
        result = false;
        continue;
     }
  }

  array2.Set(0, DUInt32(secondVals[0]));

  if (array1.Count() != array2.Count() || array2.Count() != array3.Count())
    result = false;
  for (uint_t index = 0; result && (index < array1.Count()); ++index)
  {
    DUInt32 temp, t2, t3;
    array1.Get(index, temp);

    if (index == 0)
    {
      if (temp.mValue != firstVals[0])
      {
        result = false;
        continue;
      }

      array2.Get(index, t2);
      if (t2.mValue != secondVals[0])
      {
        result = false;
        continue;
      }

      array3.Get(index, t3);
      if (t3 != temp)
      {
        result = false;
        continue;
      }

      continue;
    }
    else if (temp.mValue != firstVals[index])
    {
      result = false;
      continue;
    }

    array2.Get(index, t2);
    if (t2 != temp)
    {
      result = false;
      continue;
    }

    array3.Get(index, t3);
    if (t3 != temp)
    {
      result = false;
      continue;
    }
  }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

int
main()
{
  bool success = true;

  DBSInit(DBSSettings());

  success = success && test_bool_array();
  success = success && test_integers_array<DUInt8, uint8_t> ("T_UINT8");
  success = success && test_integers_array<DUInt16, uint16_t> ("T_UINT16");
  success = success && test_integers_array<DUInt32, uint32_t> ("T_UINT32");
  success = success && test_integers_array<DInt64, int64_t> ("T_INT64");
  success = success && test_dates_array();
  success = success && test_datetimes_array();
  success = success && test_hiresdate_array();
  success = success && test_array_copy_optimizations();

  DBSShoutdown();

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
