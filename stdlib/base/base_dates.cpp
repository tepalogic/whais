/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whisper.
 Copyright (C) 2008  Iulian Popa

 Address: Str Olimp nr. 6
 Pantelimon Ilfov,
 Romania
 Phone:   +40721939650
 e-mail:  popaiulian@gmail.com

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include <assert.h>

#include "whisper.h"

#include "stdlib/interface.h"
#include "utils/wtypes.h"
#include "utils/date.h"
#include "interpreter/interpreter.h"

#include "base_types.h"


using namespace whisper;



struct YearsAttrs
{
  uint8_t       leaps           : 1;
  uint8_t       firstJanWeekDay : 3;
  uint8_t       reserved        : 4;
};



static const uint_t         REF_JAN_FIRST  = 5;
static const int            REF_YEAR       = 2000;

static const int64_t        SECS_PER_MIN   = 60;
static const int64_t        SECS_PER_HOUR  = 60 * SECS_PER_MIN;
static const int64_t        SECS_PER_DAY   = 24 * SECS_PER_HOUR;
static const int64_t        uSECS_PER_SEC  = 1000000;
static const int64_t        uSECS_PER_MIN  = SECS_PER_MIN  * uSECS_PER_SEC;
static const int64_t        uSECS_PER_HOUR = SECS_PER_HOUR * uSECS_PER_SEC;
static const int64_t        uSECS_PER_DAY  = SECS_PER_DAY  * uSECS_PER_SEC;


static YearsAttrs cachedYears[0xFFFF];


WLIB_PROC_DESCRIPTION       gProcTicks;
WLIB_PROC_DESCRIPTION       gProcNow;
WLIB_PROC_DESCRIPTION       gProcTimeYear;
WLIB_PROC_DESCRIPTION       gProcTimeMonth;
WLIB_PROC_DESCRIPTION       gProcTimeDay;
WLIB_PROC_DESCRIPTION       gProcTimeHours;
WLIB_PROC_DESCRIPTION       gProcTimeMinutess;
WLIB_PROC_DESCRIPTION       gProcTimeSeconds;
WLIB_PROC_DESCRIPTION       gProcTimeMicroseconds;
WLIB_PROC_DESCRIPTION       gProcDateDaysDiff;
WLIB_PROC_DESCRIPTION       gProcDateDaysDelta;
WLIB_PROC_DESCRIPTION       gProcDateSecondsDiff;
WLIB_PROC_DESCRIPTION       gProcDateSecondsDelta;
WLIB_PROC_DESCRIPTION       gProcDateMicrosecondsDiff;
WLIB_PROC_DESCRIPTION       gProcDateMicrosecondsDelta;
WLIB_PROC_DESCRIPTION       gProcNativeYear;
WLIB_PROC_DESCRIPTION       gProcDateWeek;
WLIB_PROC_DESCRIPTION       gProcLastDateOfWeek;
WLIB_PROC_DESCRIPTION       gProcDayOfWeek;



static void
compute_year_attrs (const int                refYear,
                    const uint_t             refFirstJan,
                    const int                year,
                    YearsAttrs* const        oAttrs)
{
  int  cYear            = refYear;
  int  firstJanWeekDay  = refFirstJan;

  while (cYear < year)
    firstJanWeekDay += is_leap_year (cYear++) ? 2 : 1;

  while (year < cYear)
    firstJanWeekDay -= is_leap_year (--cYear) ? 2 : 1;

  oAttrs->leaps  = is_leap_year (year);

  if (firstJanWeekDay < 0)
    oAttrs->firstJanWeekDay = 7 - (-firstJanWeekDay % 7);

  else
    oAttrs->firstJanWeekDay = firstJanWeekDay % 7;
}


static inline bool
is_leap (const int year)
{
  assert ((-32768 <= year) && (year <= 32767));

  return cachedYears[year + 32768].leaps != 0;
}


static inline uint_t
get_first_jan_weekday (const int year)
{
  assert ((-32768 <= year) && (year <= 32767));

  return cachedYears[year + 32768].firstJanWeekDay;
}


static inline int
days_of_year (const int year)
{
  return is_leap (year) ? DAYS_PER_YEAR + 1 : DAYS_PER_YEAR;
}


static inline int
days_to_month (const int year, const uint_t month)
{
  assert ((1 <= month) && (month <= 12));

  const int16_t MNTH_YDAYS[] = MNTH_YDAYS_A;

  if (is_leap (year) && (month > 2))
    return MNTH_YDAYS[month - 1] + 1;

  else
    return MNTH_YDAYS[month - 1];
}


static inline uint_t
day_of_date (const int year, const uint_t month, const uint_t day)
{
  assert (0 < day);

  return days_to_month (year, month ) + day;
}


static inline uint_t
week_of_first_jan (const int year)
{
  const uint_t janFirst = get_first_jan_weekday (year);

  if (janFirst < 4)
    return 1;

  if (is_leap (year - 1))
    return (janFirst == 5) ? 53 : 52;

  return (janFirst == 4) ? 53 : 52;
}


static inline int
years_days_diff (const int year1, const int year2)
{
  int result = 0;

  if (year1 < year2)
    {
      for (int y = year1; y < year2; ++y)
        result += days_of_year (y);
    }
  else
    {
      for (int y = year2; y < year1; ++y)
        result -= days_of_year (y);
    }

  return result;
}


static inline uint_t
get_date_week (const int year, const uint_t month, const uint_t day)
{
  const uint_t janFirst = get_first_jan_weekday (year);

  uint_t dateDays   = day_of_date (year, month, day);
  uint_t weeksCount = week_of_first_jan (year);

  assert (janFirst < 7);
  assert (dateDays > 0);
  assert ((weeksCount == 1) || (weeksCount == 52) || (weeksCount == 53));

  if (dateDays <= (6 - janFirst + 1))
    return weeksCount;

  dateDays   -= 6 - janFirst + 1;
  weeksCount  = (weeksCount == 1) ? 1 : 0;
  weeksCount += (dateDays / 7) + ((dateDays % 7 != 0) ? 1 : 0);

  if ((weeksCount == 53)
      && (week_of_first_jan (year + 1) == 1))
    {
      //It looks like this date is actually part of the next year week count.
      weeksCount = 1;
    }

  return weeksCount;
}


static inline uint_t
get_date_weekday (const int year, const uint_t month, const uint_t day)
{
  const uint_t janFirst = get_first_jan_weekday (year);
  const uint_t dateDays = day_of_date (year, month, day);

  assert (janFirst < 7);
  assert (dateDays > 0);

  return (janFirst + dateDays - 1) % 7;
}


static bool
add_date_days (int32_t          days,
               int16_t* const   ioYear,
               int8_t* const    ioMonth,
               int8_t* const    ioDay)
{
  days += day_of_date (*ioYear, *ioMonth, *ioDay);

  int year = *ioYear;
  if (days > 0)
    {
      while (days > days_of_year (year))
        {
          if (year > 32767)
            return false;

          else
            days -= days_of_year (year++);
        }
    }
  else
    {
      while (days <= 0)
        {
          if (year <= -32768)
            return false;

          else
            days += days_of_year (--year);
        }
    }

  assert ((0 < days) && (days <= days_of_year (year)));
  assert ((-32768 <= year) && (year <= 32767));

  uint8_t month = 2;
  while ((month <= 12) && (days > days_to_month (year, month)))
    ++month;

  days -= days_to_month (year, --month);

  assert (month <= 12);
  assert (0 < days);

  *ioYear  = year;
  *ioMonth = month;
  *ioDay   = days;

  return true;
}


static WLIB_STATUS
get_ticks (SessionStack& stack, ISession&)
{
  stack.Push (DUInt64 (wh_msec_ticks ()));

  return WOP_OK;
}


static WLIB_STATUS
get_curr_time (SessionStack& stack, ISession&)
{
  const WTime ctime = wh_get_currtime ();

  stack.Push (DHiresTime (ctime.year,
                          ctime.month,
                          ctime.day,
                          ctime.hour,
                          ctime.min,
                          ctime.sec,
                          ctime.usec));
  return WOP_OK;
}


static WLIB_STATUS
get_date_year (SessionStack& stack, ISession&)
{
  DDate date;

  stack[stack.Size () - 1].Operand ().GetValue (date);
  stack.Pop (1);

  if (date.IsNull ())
    stack.Push (DInt16 ());

  else
    stack.Push (DInt16 (date.mYear));

  return WOP_OK;
}


static WLIB_STATUS
get_date_month (SessionStack& stack, ISession&)
{
  DDate date;

  stack[stack.Size () - 1].Operand ().GetValue (date);
  stack.Pop (1);

  if (date.IsNull ())
    stack.Push (DUInt8 ());

  else
    stack.Push (DUInt8 (date.mMonth));

  return WOP_OK;
}


static WLIB_STATUS
get_date_day (SessionStack& stack, ISession&)
{
  DDate date;

  stack[stack.Size () - 1].Operand ().GetValue (date);
  stack.Pop (1);

  if (date.IsNull ())
    stack.Push (DUInt8 ());

  else
    stack.Push (DUInt8 (date.mDay));

  return WOP_OK;
}


static WLIB_STATUS
get_date_hours (SessionStack& stack, ISession&)
{
  DDateTime time;

  stack[stack.Size () - 1].Operand ().GetValue (time);
  stack.Pop (1);

  if (time.IsNull ())
    stack.Push (DUInt8 ());

  else
    stack.Push (DUInt8 (time.mHour));

  return WOP_OK;
}


static WLIB_STATUS
get_date_mins (SessionStack& stack, ISession&)
{
  DDateTime time;

  stack[stack.Size () - 1].Operand ().GetValue (time);
  stack.Pop (1);

  if (time.IsNull ())
    stack.Push (DUInt8 ());

  else
    stack.Push (DUInt8 (time.mMinutes));

  return WOP_OK;
}


static WLIB_STATUS
get_date_secs (SessionStack& stack, ISession&)
{
  DDateTime time;

  stack[stack.Size () - 1].Operand ().GetValue (time);
  stack.Pop (1);

  if (time.IsNull ())
    stack.Push (DUInt8 ());

  else
    stack.Push (DUInt8 (time.mSeconds));

  return WOP_OK;
}


static WLIB_STATUS
get_date_microsecs (SessionStack& stack, ISession&)
{
  DHiresTime time;

  stack[stack.Size () - 1].Operand ().GetValue (time);
  stack.Pop (1);

  if (time.IsNull ())
    stack.Push (DUInt32 ());

  else
    stack.Push (DUInt32 (time.mMicrosec));

  return WOP_OK;
}


static WLIB_STATUS
diff_date_days (SessionStack& stack, ISession&)
{
  DDate firstDate, secondDate;

  stack[stack.Size () - 2].Operand ().GetValue (firstDate);
  stack[stack.Size () - 1].Operand ().GetValue (secondDate);
  stack.Pop (2);

  if (firstDate.IsNull () || secondDate.IsNull ())
    stack.Push (DInt32 ());

  else
    {
      int32_t result = years_days_diff (firstDate.mYear, secondDate.mYear);

      result += day_of_date (secondDate.mYear,
                             secondDate.mMonth,
                             secondDate.mDay);

      result -= day_of_date (firstDate.mYear,
                             firstDate.mMonth,
                             firstDate.mDay);

      stack.Push (DInt32 (result));
    }

  return WOP_OK;
}


static WLIB_STATUS
add_time_delta_days (SessionStack& stack, ISession&)
{
  DHiresTime time;
  DInt64     days;

  stack[stack.Size () - 2].Operand ().GetValue (time);
  stack[stack.Size () - 1].Operand ().GetValue (days);
  stack.Pop (2);

  if (time.IsNull ())
    stack.Push (DHiresTime ());

  else if (days.IsNull () || (days.mValue == 0))
    stack.Push (time);

  else
    {
      int16_t year  = time.mYear;
      int8_t  month = time.mMonth;
      int8_t  day   = time.mDay;

      if (! add_date_days (days.mValue, &year, &month, &day))
        return WOP_UNKNOW;

      stack.Push (DHiresTime (year,
                              month,
                              day,
                              time.mHour,
                              time.mMinutes,
                              time.mSeconds,
                              time.mMicrosec));
    }

  return WOP_OK;
}


static WLIB_STATUS
diff_time_seconds (SessionStack& stack, ISession&)
{
  DDateTime firstTime, secondTime;

  stack[stack.Size () - 2].Operand ().GetValue (firstTime);
  stack[stack.Size () - 1].Operand ().GetValue (secondTime);
  stack.Pop (2);

  if (firstTime.IsNull () || secondTime.IsNull ())
    stack.Push (DInt64 ());

  else
    {
      int64_t result = years_days_diff (firstTime.mYear, secondTime.mYear);

      result += day_of_date (secondTime.mYear,
                             secondTime.mMonth,
                             secondTime.mDay);

      result -= day_of_date (firstTime.mYear,
                             firstTime.mMonth,
                             firstTime.mDay);

      result *= SECS_PER_DAY;

      result += secondTime.mHour    * SECS_PER_HOUR;
      result -= firstTime.mHour     * SECS_PER_HOUR;

      result += secondTime.mMinutes * SECS_PER_MIN;
      result -= firstTime.mMinutes  * SECS_PER_MIN;

      result += secondTime.mSeconds;
      result -= firstTime.mSeconds;

      stack.Push (DInt64 (result));
    }

  return WOP_OK;
}

static WLIB_STATUS
add_time_delta_seconds (SessionStack& stack, ISession&)
{
  DHiresTime time;
  DInt64     seconds;

  stack[stack.Size () - 2].Operand ().GetValue (time);
  stack[stack.Size () - 1].Operand ().GetValue (seconds);
  stack.Pop (2);

  if (time.IsNull ())
    stack.Push (DHiresTime ());

  else if (seconds.IsNull () || (seconds.mValue == 0))
    stack.Push (time);

  else
    {
      int days = 0;

      int64_t secs  = seconds.mValue                  +
                      time.mHour      * SECS_PER_HOUR +
                      time.mMinutes   * SECS_PER_MIN  +
                      time.mSeconds;

      if (secs < 0)
        {
          secs = -secs;
          days = secs / SECS_PER_DAY;

          if ((secs % SECS_PER_DAY) > 0)
            {
              days = -(days + 1);
              secs = SECS_PER_DAY - secs % SECS_PER_DAY;
            }
          else
            days = -days, secs = 0;
        }
      else
        {
          days  = secs / SECS_PER_DAY;
          secs %= SECS_PER_DAY;
        }

      int16_t year  = time.mYear;
      int8_t  month = time.mMonth;
      int8_t  day   = time.mDay;
      int8_t  hour  = secs / SECS_PER_HOUR;

      secs %= SECS_PER_HOUR;
      const int8_t min = secs / SECS_PER_MIN;

      secs %= SECS_PER_MIN;

      if (! add_date_days (days, &year, &month, &day))
        return WOP_UNKNOW;

      stack.Push (DHiresTime (year,
                              month,
                              day,
                              hour,
                              min,
                              secs,
                              time.mMicrosec));
    }

  return WOP_OK;
}


static WLIB_STATUS
diff_time_microsecs (SessionStack& stack, ISession&)
{

  DHiresTime firstTime, secondTime;

  stack[stack.Size () - 2].Operand ().GetValue (firstTime);
  stack[stack.Size () - 1].Operand ().GetValue (secondTime);
  stack.Pop (2);

  if (firstTime.IsNull () || secondTime.IsNull ())
    stack.Push (DInt64 ());

  else
    {
      int64_t result = years_days_diff (firstTime.mYear, secondTime.mYear);

      result += day_of_date (secondTime.mYear,
                             secondTime.mMonth,
                             secondTime.mDay);

      result -= day_of_date (firstTime.mYear,
                              firstTime.mMonth,
                              firstTime.mDay);

      result *= uSECS_PER_DAY;

      result += secondTime.mHour    * uSECS_PER_HOUR;
      result -= firstTime.mHour     * uSECS_PER_HOUR;

      result += secondTime.mMinutes * uSECS_PER_MIN;
      result -= firstTime.mMinutes  * uSECS_PER_MIN;

      result += secondTime.mSeconds * uSECS_PER_SEC;
      result -= firstTime.mSeconds  * uSECS_PER_SEC;

      result += secondTime.mMicrosec;
      result -= firstTime.mMicrosec;

      stack.Push (DInt64 (result));
    }

  return WOP_OK;
}


static WLIB_STATUS
add_time_delta_microseconds (SessionStack& stack, ISession&)
{
  DHiresTime time;
  DInt64     useconds;

  stack[stack.Size () - 2].Operand ().GetValue (time);
  stack[stack.Size () - 1].Operand ().GetValue (useconds);
  stack.Pop (2);

  if (time.IsNull ())
    stack.Push (DHiresTime ());

  else if (useconds.IsNull () || (useconds.mValue == 0))
    stack.Push (time);

  else
    {
      int     days   = 0;
      int64_t usecs  = useconds.mValue                  +
                       time.mHour      * uSECS_PER_HOUR +
                       time.mMinutes   * uSECS_PER_MIN  +
                       time.mSeconds   * uSECS_PER_SEC  +
                       time.mMicrosec;

      if (usecs < 0)
        {
          usecs = -usecs;
          days  = usecs / uSECS_PER_DAY;

          if ((usecs % uSECS_PER_DAY) > 0)
            {
              days  = -(days + 1);
              usecs = uSECS_PER_DAY - usecs % uSECS_PER_DAY;
            }
          else
            days = -days, usecs = 0;
        }
      else
        {
          days   = usecs / uSECS_PER_DAY;
          usecs %= uSECS_PER_DAY;
        }

      int16_t year  = time.mYear;
      int8_t  month = time.mMonth;
      int8_t  day   = time.mDay;

      const int8_t hour = usecs / uSECS_PER_HOUR;
      usecs %= uSECS_PER_HOUR;

      const int8_t min = usecs / uSECS_PER_MIN;
      usecs %= uSECS_PER_MIN;

      const int8_t secs = usecs / uSECS_PER_SEC;
      usecs %= uSECS_PER_SEC;

      if (! add_date_days (days , &year, &month, &day))
        return WOP_UNKNOW;

      stack.Push (DHiresTime (year,
                              month,
                              day,
                              hour,
                              min,
                              secs,
                              usecs));
    }
  return WOP_OK;
}

static WLIB_STATUS
native_is_leap_year (SessionStack& stack, ISession& )
{
  DInt64 year;

  stack[stack.Size () - 1].Operand ().GetValue (year);
  stack.Pop (1);

  if ((year.IsNull () || (year.mValue < -32768) || (32768 <= year.mValue)))
    stack.Push (DBool ());

  else
    stack.Push (DBool (is_leap (year.mValue)));

  return WOP_OK;
}


static WLIB_STATUS
date_week (SessionStack& stack, ISession&)
{
  DDate date;

  stack[stack.Size () - 1].Operand ().GetValue (date);
  stack.Pop (1);

  if (date.IsNull ())
    stack.Push (DUInt8 ());

  else
    stack.Push (DUInt8 (get_date_week (date.mYear, date.mMonth, date.mDay)));

  return WOP_OK;
}


static WLIB_STATUS
last_date_of_week (SessionStack& stack, ISession&)
{
  DInt16  year;
  DUInt8  week;

  stack[stack.Size () - 2].Operand ().GetValue (year);
  stack[stack.Size () - 1].Operand ().GetValue (week);
  stack.Pop (2);

  if (week.IsNull ()
      || year.IsNull ()
      || (week.mValue == 0)
      || (week.mValue > 53)
      || ((week.mValue == 53)
          && (week.mValue != get_date_week (year.mValue, 12, 31))))
    {
      stack.Push (DDate ());
      return WOP_OK;
    }

  int deltaDays = (6 - get_first_jan_weekday (year.mValue)) +
                  week.mValue * 7;

  deltaDays += (get_date_week (year.mValue, 1, 1) == 1) ? -7 : 0;

  int16_t year_ = year.mValue;
  int8_t  month = 1;
  int8_t  day   = 1;

  if (! add_date_days (deltaDays, &year_, &month, &day))
    {
      assert (false);

      return WOP_UNKNOW;
    }

  stack.Push (DDate (year_, month, day));

  return WOP_OK;
}

static WLIB_STATUS
get_weekday (SessionStack& stack, ISession&)
{
  DDate   date;

  stack[stack.Size () - 1].Operand ().GetValue (date);
  stack.Pop (1);

  if (date.IsNull ())
    {
      stack.Push (DUInt8 ());

      return WOP_OK;
    }

  stack.Push (DUInt8 (get_date_weekday (date.mYear,
                                        date.mMonth,
                                        date.mDay) + 1));
  return WOP_OK;
}


WLIB_STATUS
base_dates_init ()
{
  /* Precompute the date cache. */
  for (int year = 0; year <= 65535; ++year)
    {
      compute_year_attrs (REF_YEAR,
                          REF_JAN_FIRST,
                          year - 32768,
                          &cachedYears[year]);
    }

  static const uint8_t* ticksLocals[] = { gUInt64Type };

  gProcTicks.name        = "ticks";
  gProcTicks.localsCount = 1;
  gProcTicks.localsTypes = ticksLocals;
  gProcTicks.code        = get_ticks;

  static const uint8_t* nowLocals[] = { gHiresTimeType };

  gProcNow.name          = "now";
  gProcNow.localsCount   = 1;
  gProcNow.localsTypes   = nowLocals;
  gProcNow.code          = get_curr_time;


  static const uint8_t* yearLocal[] = { gInt16Type, gDateType };

  gProcTimeYear.name        = "year";
  gProcTimeYear.localsCount = 2;
  gProcTimeYear.localsTypes = yearLocal;
  gProcTimeYear.code        = get_date_year;

  static const uint8_t* dateLocal[] = { gUInt8Type, gDateType };

  gProcTimeMonth.name        = "month";
  gProcTimeMonth.localsCount = 2;
  gProcTimeMonth.localsTypes = dateLocal;
  gProcTimeMonth.code        = get_date_month;


  gProcTimeDay.name          = "day";
  gProcTimeDay.localsCount   = 2;
  gProcTimeDay.localsTypes   = dateLocal;
  gProcTimeDay.code          = get_date_day;

  static const uint8_t* timeLocal[] = { gUInt8Type, gDateTimeType };

  gProcTimeHours.name           = "hours";
  gProcTimeHours.localsCount    = 2;
  gProcTimeHours.localsTypes    = timeLocal;
  gProcTimeHours.code           = get_date_hours;

  gProcTimeMinutess.name        = "minutes";
  gProcTimeMinutess.localsCount = 2;
  gProcTimeMinutess.localsTypes = timeLocal;
  gProcTimeMinutess.code        = get_date_mins;

  gProcTimeSeconds.name         = "seconds";
  gProcTimeSeconds.localsCount  = 2;
  gProcTimeSeconds.localsTypes  = timeLocal;
  gProcTimeSeconds.code         = get_date_secs;

  static const uint8_t* htimeLocal[] = { gUInt32Type, gHiresTimeType };

  gProcTimeMicroseconds.name         = "microsecs";
  gProcTimeMicroseconds.localsCount  = 2;
  gProcTimeMicroseconds.localsTypes  = htimeLocal;
  gProcTimeMicroseconds.code         = get_date_microsecs;

  static const uint8_t* daysDiffLocals[] = { gInt32Type, gDateType, gDateType};

  gProcDateDaysDiff.name        = "days_diff";
  gProcDateDaysDiff.localsCount = 3;
  gProcDateDaysDiff.localsTypes = daysDiffLocals;
  gProcDateDaysDiff.code        = diff_date_days;


  static const uint8_t* secsDiffLocals[] = {
                                             gInt64Type,
                                             gDateTimeType,
                                             gDateTimeType
                                           };

  gProcDateSecondsDiff.name        = "seconds_diff";
  gProcDateSecondsDiff.localsCount = 3;
  gProcDateSecondsDiff.localsTypes = secsDiffLocals;
  gProcDateSecondsDiff.code        = diff_time_seconds;

  static const uint8_t* usecsDiffLocals[] = {
                                             gInt64Type,
                                             gHiresTimeType,
                                             gHiresTimeType
                                            };

  gProcDateMicrosecondsDiff.name        = "microsecs_diff";
  gProcDateMicrosecondsDiff.localsCount = 3;
  gProcDateMicrosecondsDiff.localsTypes = usecsDiffLocals;
  gProcDateMicrosecondsDiff.code        = diff_time_microsecs;


  static const uint8_t* deltaAddLocals[] = {
                                             gHiresTimeType,
                                             gHiresTimeType,
                                             gInt64Type
                                           };

  gProcDateDaysDelta.name                = "add_days";
  gProcDateDaysDelta.localsCount         = 3;
  gProcDateDaysDelta.localsTypes         = deltaAddLocals;
  gProcDateDaysDelta.code                = add_time_delta_days;

  gProcDateSecondsDelta.name             = "add_seconds";
  gProcDateSecondsDelta.localsCount      = 3;
  gProcDateSecondsDelta.localsTypes      = deltaAddLocals;
  gProcDateSecondsDelta.code             = add_time_delta_seconds;

  gProcDateMicrosecondsDelta.name         = "add_microsecs";
  gProcDateMicrosecondsDelta.localsCount  = 3;
  gProcDateMicrosecondsDelta.localsTypes  = deltaAddLocals;
  gProcDateMicrosecondsDelta.code         = add_time_delta_microseconds;

  static const uint8_t* leapLocals[]   = { gBoolType, gInt64Type };

  gProcNativeYear.name        = "year_leaps";
  gProcNativeYear.localsCount = 2;
  gProcNativeYear.localsTypes = leapLocals;
  gProcNativeYear.code        = native_is_leap_year;


  static const uint8_t* weekDateLocals[] = { gUInt8Type, gDateType };

  gProcDateWeek.name          = "week";
  gProcDateWeek.localsCount   = 2;
  gProcDateWeek.localsTypes   = weekDateLocals;
  gProcDateWeek.code          = date_week;


  static const uint8_t* lastWeekDateLocals[] = {
                                                  gDateType,
                                                  gInt16Type,
                                                  gUInt8Type,
                                                 };

  gProcLastDateOfWeek.name          = "last_week_date";
  gProcLastDateOfWeek.localsCount   = 3;
  gProcLastDateOfWeek.localsTypes   = lastWeekDateLocals;
  gProcLastDateOfWeek.code          = last_date_of_week;


  static const uint8_t* dayOfWeekLocals[] = { gUInt8Type, gDateType };

  gProcDayOfWeek.name        = "weekday";
  gProcDayOfWeek.localsCount = 2;
  gProcDayOfWeek.localsTypes = dayOfWeekLocals;
  gProcDayOfWeek.code        = get_weekday;

  return WOP_OK;
}


