/*
 * test_arraysort.cpp
 *
 *  Created on: Jan 21, 2012
 *      Author: ipopa
 */


#include <assert.h>
#include <iostream>
#include <string.h>

#include "utils/random.h"
#include "utils/wthread.h"
#include "custom/include/test/test_fmw.h"

#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"
#include "../include/dbs_values.h"


const uint_t _elemsCount = 1000000;

DBSDateTime
get_random_datetime ()
{
  int16_t year  = w_rnd () & 0xFFFF;
  uint8_t month = w_rnd () % 12 + 1;
  uint8_t day   = w_rnd () % 27 + 1;
  uint8_t hour  = w_rnd () % 24;
  uint8_t mins  = w_rnd () % 60;
  uint8_t secs  = w_rnd () % 60;

  return DBSDateTime (year, month, day, hour, mins, secs);
}

DBSHiresTime
get_random_hirestime ()
{
  int16_t year  = w_rnd () & 0xFFFF;
  uint8_t month = w_rnd () % 12 + 1;
  uint8_t day   = w_rnd () % 27 + 1;
  uint8_t hour  = w_rnd () % 24;
  uint8_t mins  = w_rnd () % 60;
  uint8_t secs  = w_rnd () % 60;
  uint32_t  mic = w_rnd () % 1000000000;

  return DBSHiresTime (year, month, day, hour, mins, secs, mic);
}



DBSDate
get_random_date ()
{
  int16_t year  = w_rnd () & 0xFFFF;
  uint8_t month = w_rnd () % 12 + 1;
  uint8_t day   = w_rnd () % 27 + 1;

  return DBSDate (year, month, day);
}

void
test_array_with_dates (void *)
{
  std::cout << "Testing array sort with dates ...\n";
  DBSArray array (_SC (DBSDate*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (get_random_date ());

  array.Sort ();

  if (array.ElementsCount () != _elemsCount)
    throw 0;

  DBSDate lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DBSDate currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 1;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with dates ended!\n";
}

void
test_array_with_datetimes (void *)
{
  std::cout << "Testing array sort with datetimes ...\n";
  DBSArray array (_SC (DBSDateTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (get_random_datetime ());

  array.Sort ();

  if (array.ElementsCount () != _elemsCount)
    throw 2;

  DBSDateTime lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DBSDateTime currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 3;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with datetimes ended!\n";
}

void
test_array_with_hirestimes (void *)
{
  std::cout << "Testing array sort with hirestimes ...\n";
  DBSArray array (_SC (DBSHiresTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (get_random_hirestime ());

  array.Sort ();

  if (array.ElementsCount () != _elemsCount)
    throw 4;

  DBSHiresTime lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DBSHiresTime currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 5;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with hirestimes ended!\n";
}

void
test_array_with_int8 (void *)
{
  std::cout << "Testing array sort with int8 ...\n";
  DBSArray array (_SC (DBSUInt8*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (DBSUInt8 ( w_rnd () & 0xFF));

  array.Sort ();

  if (array.ElementsCount () != _elemsCount)
    throw 6;

  DBSUInt8 lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DBSUInt8 currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 7;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with int8 ended!\n";
}


void
test_array_with_dates_r (void *)
{
  std::cout << "Testing array reverse sort with dates ...\n";
  DBSArray array (_SC (DBSDate*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (get_random_date ());

  array.Sort (true);

  if (array.ElementsCount () != _elemsCount)
    throw 8;

  DBSDate lastValue;
  for (uint_t index = _elemsCount; index-- > 0;)
    {
      DBSDate currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 9;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with dates ended!\n";

}

void
test_array_with_datetimes_r (void *)
{
  std::cout << "Testing array reverse sort with datetimes ...\n";
  DBSArray array (_SC (DBSDateTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (get_random_datetime ());

  array.Sort (true);

  if (array.ElementsCount () != _elemsCount)
    throw 10;

  DBSDateTime lastValue;
  for (uint_t index = _elemsCount; index-- > 0 ;)
    {
      DBSDateTime currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 11;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with datetimes ended!\n";
}

void
test_array_with_hirestimes_r (void *)
{
  std::cout << "Testing array reverse sort with hirestimes ...\n";
  DBSArray array (_SC (DBSHiresTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (get_random_hirestime ());

  array.Sort (true);

  if (array.ElementsCount () != _elemsCount)
    throw 12;

  DBSHiresTime lastValue;
  for (uint_t index = _elemsCount; index-- > 0;)
    {
      DBSHiresTime currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 13;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with hirestimes ended!\n";
}


void
test_array_with_int8_r (void *)
{
  std::cout << "Testing array reverse sort with int8 ...\n";
  DBSArray array (_SC (DBSUInt8*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.AddElement (DBSUInt8 (w_rnd () & 0xFF));

  array.Sort (true);

  if (array.ElementsCount () != _elemsCount)
    throw 14;

  DBSUInt8 lastValue;
  for (uint_t index = _elemsCount; index-- > 0;)
    {
    DBSUInt8 currValue;
      array.GetElement (currValue, index);
      if (currValue < lastValue)
        throw 15;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with int8 ended!\n";
}

int
main ()
{
  bool success = true;

  DBSInit (DBSSettings ());

  {
    WThread th[8];

    th[0].Run (test_array_with_dates, NULL);
    th[1].Run (test_array_with_datetimes, NULL);
    th[2].Run (test_array_with_hirestimes, NULL);
    th[3].Run (test_array_with_dates_r, NULL);
    th[4].Run (test_array_with_datetimes_r, NULL);
    th[5].Run (test_array_with_hirestimes_r, NULL);
    th[6].Run (test_array_with_int8, NULL);
    th[7].Run (test_array_with_int8_r, NULL);
  }

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
