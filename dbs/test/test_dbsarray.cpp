/*
 * test_dbstext.cpp
 *
 *  Created on: Oct 2, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "test/test_fmw.h"


#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"
#include "../include/dbs_values.h"

#include "../pastra/ps_varstorage.h"
#include "../pastra/ps_arraystrategy.h"

using namespace pastra;

bool test_bool_array ()
{
  std::cout << "Testing T_BOOL array... ";
  D_UINT result = true;
  DBSArray anotherArray ((DBSBool *)NULL);

  if (result)
    {
      DBSArray nullArray ((DBSBool *)NULL);
      if (! nullArray.IsNull())
        result = false;

      if (! anotherArray.IsNull ())
        result = false;

      if ( anotherArray.AddElement(DBSBool (false, true)) != 0)
        result = false;
      else if  (anotherArray.AddElement(DBSBool (false, false)) != 1)
        result = false;

      if ( ! (anotherArray.IsNull() == false) && (anotherArray.GetElementsCount() != 2) )
        result = false;

      DBSBool tv(true, false);
      anotherArray.GetElement( tv, 0);
      if ((tv.IsNull()) || (tv.m_Value == false))
        result = false;

      anotherArray.GetElement( tv, 1);
      if ((tv.IsNull()) || (tv.m_Value == true))
        result = false;
    }

  if (result)
    {


      anotherArray.SetElement (DBSBool(false, true), 1);
      DBSBool tv(true, false);
      anotherArray.GetElement( tv, 1);
      if ((tv.IsNull()) || (tv.m_Value == false))
        result = false;

      const DBSBool nullBull (true, true);
      anotherArray.SetElement (nullBull, 0);
      anotherArray.GetElement( tv, 0);
      if ((tv.IsNull()) || (tv.m_Value == false) || (anotherArray.GetElementsCount() > 1))
        result = false;

      anotherArray.SetElement (nullBull, 0);
      if (anotherArray.IsNull() == false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

template <typename T, typename Tint> bool
test_integers_array (const char *type)
{
  std::cout << "Testing " << type << " array... ";
  D_UINT result = true;

  D_UINT64 elementsCount = 0;
  T nullValue (true, 0);
  DBSArray testArray((T *)NULL);

  for (D_UINT64 index = 1; index < 10000; index += 13)
    {
      if (testArray.GetElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      //Limit to 0xFF because of VC++ run time check!
      //Don't worry C++0x will take care of this in future.
      testArray.AddElement (T(false, index & 0xFF));
      ++elementsCount;
    }

  if (result)
    {
      testArray.RemoveElement(0);
      testArray.SetElement (nullValue, elementsCount - 2);
      elementsCount -= 2;

      if (testArray.GetElementsCount() != elementsCount)
        result = false;
    }

  if (result)
    {
      T testValue (true, 0);
      D_INT64 content = 14;
      for (D_UINT index = 0; index < elementsCount; index++)
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
  D_UINT result = true;

  D_UINT elementsCount = 0;
  D_INT year = 1;
  D_UINT day = 1;
  D_UINT month = 1;

  DBSArray testArray((DBSDate *)NULL);

  for (D_UINT64 elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.GetElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      testArray.AddElement (DBSDate (false, year, month, day));

      year += 7;
      day += 3; day %= 25; day++;
      month += 7; month %= 12; month++;
    }


  //Read back;
  year = day = month = 1;
  for (D_UINT64 index = 0; index < elementsCount; ++index)
  {
    DBSDate testValue (true);
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
      for (D_UINT index = 0; index < elementsCount; ++ index)
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
  D_UINT result = true;

  D_UINT elementsCount = 0;
  D_INT year = 1;
  D_UINT day = 1;
  D_UINT month = 1;
  D_UINT hour = 3;
  D_UINT mins = 3;
  D_UINT sec = 3;

  DBSArray testArray((DBSDateTime *)NULL);

  for (D_UINT64 elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.GetElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      testArray.AddElement (DBSDateTime (false, year, month, day, hour, mins, sec));

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
  for (D_UINT64 index = 0; index < elementsCount; ++index)
  {
    DBSDateTime testValue (true);
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
      for (D_UINT index = 0; index < elementsCount; ++ index)
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
  D_UINT result = true;

  D_UINT elementsCount = 0;
  D_INT year = 1;
  D_UINT day = 1;
  D_UINT month = 1;
  D_UINT hour = 3;
  D_UINT mins = 3;
  D_UINT sec = 3;
  D_UINT32 msec = 4;

  DBSArray testArray((DBSHiresTime *)NULL);

  for (D_UINT64 elementsCount = 0; elementsCount < 500;  elementsCount++)
    {
      if (testArray.GetElementsCount () != elementsCount)
        {
          result = false;
          break;
        }
      testArray.AddElement (DBSHiresTime (false, year, month, day, hour, mins, sec, msec));

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
  for (D_UINT64 index = 0; index < elementsCount; ++index)
  {
    DBSHiresTime testValue (true);
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
      for (D_UINT index = 0; index < elementsCount; ++ index)
        testArray.RemoveElement (0);

      if (testArray.IsNull () != false)
        result = false;
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialized
  // We need not to test against it!
  D_UINT prealloc_mem = test_get_mem_used ();
  bool success = true;

  {
    std::string dir = ".";
    dir += whc_get_directory_delimiter ();

    DBSInit (dir.c_str (), dir.c_str ());
  }

  success = success && test_bool_array ();
  success = success && test_integers_array<DBSUInt8, D_UINT8> ("T_UINT8");
  success = success && test_integers_array<DBSUInt16, D_UINT16> ("T_UINT16");
  success = success && test_integers_array<DBSUInt32, D_UINT32> ("T_UINT32");
  success = success && test_integers_array<DBSInt64, D_INT64> ("T_INT64");
  success = success && test_dates_array ();
  success = success && test_datetimes_array ();
  success = success && test_hiresdate_array ();


  DBSShoutdown ();
  D_UINT mem_usage = test_get_mem_used () - prealloc_mem;

  if (mem_usage)
    {
      success = false;
      test_print_unfree_mem();
    }

  std::cout << "Memory peak (no prealloc): " <<
            test_get_mem_peak () - prealloc_mem << " bytes." << std::endl;
  std::cout << "Preallocated mem: " << prealloc_mem << " bytes." << std::endl;
  std::cout << "Current memory usage: " << mem_usage << " bytes." << std::endl;
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return -1;
    }
  else
    std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

