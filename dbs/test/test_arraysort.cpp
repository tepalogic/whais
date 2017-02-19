/*
 * test_arraysort.cpp
 *
 *  Created on: Jan 21, 2012
 *      Author: ipopa
 */


#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "utils/wrandom.h"
#include "utils/wthread.h"
#include "custom/include/test/test_fmw.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"

using namespace whais;

uint_t _elemsCount = 1000000;

DDateTime
get_random_datetime()
{
  int16_t year  = wh_rnd() & 0xFFFF;
  uint8_t month = wh_rnd() % 12 + 1;
  uint8_t day   = wh_rnd() % 27 + 1;
  uint8_t hour  = wh_rnd() % 24;
  uint8_t mins  = wh_rnd() % 60;
  uint8_t secs  = wh_rnd() % 60;

  return DDateTime(year, month, day, hour, mins, secs);
}

DHiresTime
get_random_hirestime()
{
  int16_t year  = wh_rnd() & 0xFFFF;
  uint8_t month = wh_rnd() % 12 + 1;
  uint8_t day   = wh_rnd() % 27 + 1;
  uint8_t hour  = wh_rnd() % 24;
  uint8_t mins  = wh_rnd() % 60;
  uint8_t secs  = wh_rnd() % 60;
  uint32_t  mic = wh_rnd() % 1000000;

  return DHiresTime(year, month, day, hour, mins, secs, mic);
}



DDate
get_random_date()
{
  int16_t year  = wh_rnd() & 0xFFFF;
  uint8_t month = wh_rnd() % 12 + 1;
  uint8_t day   = wh_rnd() % 27 + 1;

  return DDate(year, month, day);
}

void
test_array_with_dates(void *)
{
  std::cout << "Testing array sort with dates ...\n";
  DArray array(_SC(DDate*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(get_random_date());

  array.Sort();

  if (array.Count() != _elemsCount)
    throw 0;

  DDate lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DDate currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 1;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with dates ended!\n";
}

void
test_array_with_datetimes(void *)
{
  std::cout << "Testing array sort with datetimes ...\n";
  DArray array(_SC(DDateTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(get_random_datetime());

  array.Sort();

  if (array.Count() != _elemsCount)
    throw 2;

  DDateTime lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DDateTime currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 3;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with datetimes ended!\n";
}

void
test_array_with_hirestimes(void *)
{
  std::cout << "Testing array sort with hirestimes ...\n";
  DArray array(_SC(DHiresTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(get_random_hirestime());

  array.Sort();

  if (array.Count() != _elemsCount)
    throw 4;

  DHiresTime lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DHiresTime currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 5;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with hirestimes ended!\n";
}

void
test_array_with_int8(void *)
{
  std::cout << "Testing array sort with int8 ...\n";
  DArray array(_SC(DUInt8*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(DUInt8( wh_rnd() & 0xFF));

  array.Sort();

  if (array.Count() != _elemsCount)
    throw 6;

  DUInt8 lastValue;
  for (uint_t index = 0; index < _elemsCount; ++index)
    {
      DUInt8 currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 7;

      lastValue = currValue;
    }

  std::cout << "Testing array sort with int8 ended!\n";
}


void
test_array_with_dates_r(void *)
{
  std::cout << "Testing array reverse sort with dates ...\n";
  DArray array(_SC(DDate*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(get_random_date());

  array.Sort(true);

  if (array.Count() != _elemsCount)
    throw 8;

  DDate lastValue;
  for (uint_t index = _elemsCount; index-- > 0;)
    {
      DDate currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 9;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with dates ended!\n";

}

void
test_array_with_datetimes_r(void *)
{
  std::cout << "Testing array reverse sort with datetimes ...\n";
  DArray array(_SC(DDateTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(get_random_datetime());

  array.Sort(true);

  if (array.Count() != _elemsCount)
    throw 10;

  DDateTime lastValue;
  for (uint_t index = _elemsCount; index-- > 0 ;)
    {
      DDateTime currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 11;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with datetimes ended!\n";
}

void
test_array_with_hirestimes_r(void *)
{
  std::cout << "Testing array reverse sort with hirestimes ...\n";
  DArray array(_SC(DHiresTime*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(get_random_hirestime());

  array.Sort(true);

  if (array.Count() != _elemsCount)
    throw 12;

  DHiresTime lastValue;
  for (uint_t index = _elemsCount; index-- > 0;)
    {
      DHiresTime currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 13;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with hirestimes ended!\n";
}


void
test_array_with_int8_r(void *)
{
  std::cout << "Testing array reverse sort with int8 ...\n";
  DArray array(_SC(DUInt8*, NULL));

  for (uint_t index = 0; index < _elemsCount; ++index)
    array.Add(DUInt8(wh_rnd() & 0xFF));

  array.Sort(true);

  if (array.Count() != _elemsCount)
    throw 14;

  DUInt8 lastValue;
  for (uint_t index = _elemsCount; index-- > 0;)
    {
    DUInt8 currValue;
      array.Get(index, currValue);
      if (currValue < lastValue)
        throw 15;

      lastValue = currValue;
    }

  std::cout << "Testing array reverse sort with int8 ended!\n";
}

int
main(int argc, char** argv)
{
  bool success = true;

  if (argc > 1)
    _elemsCount = atol(argv[1]);


  std::cout << "Sorting array with " << _elemsCount << " elements.\n";

  DBSInit(DBSSettings());

  {
    Thread th[8];

    th[0].Run(test_array_with_dates, NULL);
    th[1].Run(test_array_with_datetimes, NULL);
    th[2].Run(test_array_with_hirestimes, NULL);
    th[3].Run(test_array_with_dates_r, NULL);
    th[4].Run(test_array_with_datetimes_r, NULL);
    th[5].Run(test_array_with_hirestimes_r, NULL);
    th[6].Run(test_array_with_int8, NULL);
    th[7].Run(test_array_with_int8_r, NULL);

    th[0].WaitToEnd(true);
    th[1].WaitToEnd(true);
    th[2].WaitToEnd(true);
    th[3].WaitToEnd(true);
    th[4].WaitToEnd(true);
    th[5].WaitToEnd(true);
    th[6].WaitToEnd(true);
    th[7].WaitToEnd(true);
  }

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

