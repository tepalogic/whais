/******************************************************************************
WHAIS - An advanced database system
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
******************************************************************************/

#include <cassert>

#include "whais.h"

WTime
wh_get_currtime ()
{
  WTime      result;
  FILETIME   fileTime;
  SYSTEMTIME utcTime;
  uint64_t   usec;

  GetSystemTimeAsFileTime (&fileTime);
  FileTimeToSystemTime (&fileTime, &utcTime);

  /* Convert the FILETIME to a real 64 bit integer. */
  usec = fileTime.dwHighDateTime;
  usec <<= 32;
  usec += fileTime.dwLowDateTime;

  /* Get rid of the nanoseconds part. */
  usec /= 10;

  /* Keep only the microseconds. */
  usec %= 1000000;

  assert (usec / 1000 == utcTime.wMilliseconds);

  result.year  = utcTime.wYear;
  result.month = utcTime.wMonth;
  result.day   = utcTime.wDay;
  result.hour  = utcTime.wHour;
  result.min   = utcTime.wMinute;
  result.sec   = utcTime.wSecond;
  result.usec  = usec;

  return result;
}


WTICKS
wh_msec_ticks ()
{
  return GetTickCount64 ();
}




