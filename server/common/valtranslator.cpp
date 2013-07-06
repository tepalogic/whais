/******************************************************************************
 PASTRA - A light database one file system and more.
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
#include <cstdio>
#include <cstring>
#include <limits.h>

#include "utils/utf8.h"
#include "valtranslator.h"

using namespace whisper;

static const uint_t   MAX_INTEGER_STR_SIZE  = 32;
static const uint64_t MAX_SIGNED_ABS        = 0x8000000000000000ull;
static const int64_t  USECS_PRECISION       = 1000000;
static const uint_t   MAX_UTF8_CHAR_SIZE    = 6;

static int64_t
read_real (const uint8_t*   from,
           int64_t* const   outIntPart,
           int64_t* const   outFracPart,
           int64_t* const   outPrecision )
{
  int64_t size     = 0;
  bool    negative = false;

  if (*from == '-')
    {
      from++;
      if ((*from < '0') || (*from > '9'))
        return 0;

      size++;
      negative = true;
    }

  uint64_t intPart = 0, fracPart = 0, precision = 1;
  while (('0' <= *from) && (*from <= '9' ))
    {
      if ((intPart * 10 + (*from - '0')) > MAX_SIGNED_ABS)
        return 0; // Overflow

      intPart *= 10;
      intPart += *from - '0';

      from++, size++;
    }

  if (*from == '.')
    {
      from++, size++;

      if ((*from < '0') || (*from > '9'))
        return 0;
    }

  while (('0' <= *from) && (*from <= '9' ))
    {
      if ((fracPart > (fracPart * 10 + (*from - '0')))
          || (precision > precision * 10))
        {
          return 0; // Overflow!
        }

      fracPart  *= 10;
      fracPart  += *from - '0';
      precision *= 10;

      from++, size++;
    }

  if (negative)
    {
      if (intPart > 0)
        {
          intPart -= 1;
          intPart  = ~intPart;
        }

      if (fracPart > 0)
        {
          fracPart -= 1;
          fracPart  = ~fracPart;
        }
    }

  *outIntPart    = intPart;
  *outFracPart   = fracPart;
  *outPrecision  = precision;

  return size;
}


static uint64_t
read_integer (const uint8_t*    from,
              uint_t* const     outCount,
              const bool        isSigned)
{
  uint64_t result     = 0;
  bool     isNegative = false;

  *outCount = 0;
  if (*from == '-')
    {
      ++from;
      if (! isSigned || (*from < '0') || (*from > '9'))
        return 0;

      isNegative  = true;
      *outCount  += 1;
    }

  while (*from >= '0' && *from <= '9')
    {
      if ((isSigned && ((result * 10 + (*from - '0')) > MAX_SIGNED_ABS))
          || ( ! isSigned && (result > (result * 10 + (*from - '0')))))
        {
          return 0; // Overflow condition
        }

      result *= 10;
      result += *from - '0';

      ++from, *outCount += 1;
    }

  if (isNegative && (result > 0))
    {
      result -= 1;
      result  = ~result;
    }

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const  utf8Src,
                      const uint_t          srcSize,
                      DBool* const          outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DBool ();
      return 1;
    }
  else if ( 2 * sizeof (uint8_t) > srcSize)
    return 0;

  else
    {
      *outValue = DBool (*utf8Src == '1');

      if ((utf8Src[1] != 0)
          || ((*utf8Src != '1') && (*utf8Src != '0')))
        {
          return 0;
        }

      return 2;
    }

  return 0;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DChar* const         outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DChar ();
      return 1;
    }

  const uint_t result = wh_utf8_cu_count (*utf8Src);
  if ((result == 0) || (result + 1 > srcSize))
    return 0;

  else
    {
      uint32_t ch;

      if ((wh_load_utf8_cp (utf8Src, &ch) != result)
          || (ch == 0)
          || (utf8Src[result] != 0))
        {
          return 0;
        }

      *outValue = DChar (ch);
    }

  return result + 1;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DDate* const         outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DDate ();
      return 1;
    }

  uint_t temp   = 0;
  uint_t result = 0;

  const int64_t year = read_integer (utf8Src, &temp, true);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const uint64_t month = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const uint64_t day = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != 0) || (result > srcSize))
    return 0;

  *outValue = DDate (year, month, day);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DDateTime* const     outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DDateTime ();
      return 1;
    }

  uint_t temp   = 0;
  uint_t result = 0;

  const int64_t year = read_integer (utf8Src, &temp, true);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const uint64_t month = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const uint64_t day = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ' ') || (result > srcSize))
    return 0;

  const uint64_t hour = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  const uint64_t min = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  const uint64_t secs = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != 0) || (result > srcSize))
    return 0;

  *outValue = DDateTime (year, month, day, hour, min, secs);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DHiresTime* const    outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DHiresTime ();
      return 1;
    }

  uint_t temp   = 0;
  uint_t result = 0;

  const int64_t year = read_integer (utf8Src, &temp, true);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const uint64_t month = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const uint64_t day = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ' ') || (result > srcSize))
    return 0;

  const uint64_t hour = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  const uint64_t min = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  int64_t secs, usecs, usecPrec;
  temp = read_real (utf8Src + result, &secs, &usecs, &usecPrec);
  result += temp;
  if ((temp == 0)
      || (secs < 0)
      || (usecs < 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize))
    {
      return 0;
    }

  if (usecPrec > USECS_PRECISION)
    usecs /= (usecPrec / USECS_PRECISION);

  else
    usecs *= USECS_PRECISION / usecPrec;

  *outValue = DHiresTime (year, month, day, hour, min, secs, usecs);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DInt8* const         outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DInt8 ();
      return 1;
    }

  uint_t        result = 0;
  const int64_t value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<int8_t>::min ())
      || (std::numeric_limits<int8_t>::max () < value))
    {
      return 0;
    }

  *outValue = DInt8 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DInt16* const        outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DInt16 ();
      return 1;
    }

  uint_t        result = 0;
  const int64_t value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<int16_t>::min ())
      || (std::numeric_limits<int16_t>::max () < value))
    {
      return 0;
    }
  *outValue = DInt16 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DInt32* const        outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DInt32 ();
      return 1;
    }

  uint_t        result = 0;
  const int64_t value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<int32_t>::min ())
      || (std::numeric_limits<int32_t>::max () < value))
    {
      return 0;
    }

  *outValue = DInt32 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DInt64* const        outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DInt64 ();
      return 1;
    }

  uint_t        result = 0;
  const int64_t value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize))

    {
      return 0;
    }

  *outValue = DInt64 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DUInt8* const        outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DUInt8 ();
      return 1;
    }

  uint_t         result = 0;
  const uint64_t value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<uint8_t>::min ())
      || (std::numeric_limits<uint8_t>::max () < value))
    {
      return 0;
    }
  *outValue = DUInt8 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DUInt16* const       outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DUInt16 ();
      return 1;
    }

  uint_t         result = 0;
  const uint64_t value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<uint16_t>::min ())
      || (std::numeric_limits<uint16_t>::max () < value))
    {
      return 0;
    }
  *outValue = DUInt16 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DUInt32* const       outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DUInt32 ();
      return 1;
    }

  uint_t         result = 0;
  const uint64_t value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<uint32_t>::min ())
      || (std::numeric_limits<uint32_t>::max () < value))
    {
      return 0;
    }

  *outValue = DUInt32 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DUInt64* const       outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DUInt64 ();
      return 1;
    }

  uint_t         result = 0;
  const uint64_t value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize))
    {
      return 0;
    }
  *outValue = DUInt64 (value);

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DReal* const         outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DReal ();
      return 1;
    }

  int64_t intPart, fracPart, precision;
  uint_t  result  = read_real (utf8Src, &intPart, &fracPart, &precision);
  assert ((precision == 1) || (precision % 10 == 0));
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize))
    {
      return 0;
    }

  *outValue = DReal (DBS_REAL_T (intPart, fracPart, precision));

  return result;
}


uint_t
Utf8Translator::Read (const uint8_t* const utf8Src,
                      const uint_t         srcSize,
                      DRichReal* const     outValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *outValue = DRichReal ();
      return 1;
    }

  int64_t intPart, fracPart, precision;
  uint_t  result  = read_real (utf8Src, &intPart, &fracPart, &precision);

  assert ((precision == 1) || (precision % 10 == 0));
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize))
    {
      return 0;
    }

  *outValue = DRichReal (DBS_RICHREAL_T (intPart, fracPart, precision));

  return result;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DBool&        value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }
  else if (maxSize < 2)
    return 0;

  else if (value.mValue)
    utf8Dest[0] = '1';

  else
    utf8Dest[0] = '0';

  utf8Dest[1] = 0;

  return 2;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DChar&        value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }
  else if (maxSize <= MAX_UTF8_CHAR_SIZE)
    return 0;

  uint_t result = wh_store_utf8_cp (value.mValue, utf8Dest);
  utf8Dest[result++] = 0;

  return result;
}


uint_t
Utf8Translator::Write (uint8_t* const    utf8Dest,
                       const uint_t      maxSize,
                       const DDate&      value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%d/%u/%u",
                         value.mYear,
                         value.mMonth,
                         value.mDay);

  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;

}

uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DDateTime&    value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%d/%u/%u %u:%u:%u",
                         value.mYear,
                         value.mMonth,
                         value.mDay,
                         value.mHour,
                         value.mMinutes,
                         value.mSeconds);

  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DHiresTime&   value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%d/%u/%u %u:%u:%u.",
                         value.mYear,
                         value.mMonth,
                         value.mDay,
                         value.mHour,
                         value.mMinutes,
                         value.mSeconds);

  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  uint64_t usecPrec = USECS_PRECISION / 10;
  uint32_t usec_t   = value.mMicrosec;

  while ((usec_t > 0 ) && (usecPrec > usec_t))
    {
      utf8Dest[result++] = '0';
      usecPrec /= 10;

      if (_SC (uint_t, result) >= maxSize)
        return 0;
    }

  while ((usec_t > 0) && ((usec_t % 10) == 0))
    usec_t /= 10;

  const int64_t res_t = result;

  result = snprintf (_RC (char*, utf8Dest + result),
                     maxSize - result,
                     "%u",
                     usec_t);

  if ((result < 0) || (result + res_t >= maxSize))
    return 0;

  return result + res_t + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DInt8&        value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%lld",
                         _SC (long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DInt16&       value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%lld",
                         _SC (long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DInt32&       value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%lld",
                         _SC (long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const    utf8Dest,
                       const uint_t      maxSize,
                       const DInt64&     value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%lld",
                         _SC (long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DUInt8&       value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                        "%llu",
                        _SC (unsigned long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DUInt16&      value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%llu",
                         _SC (unsigned long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DUInt32&      value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%llu",
                         _SC (unsigned long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DUInt64&      value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  int result = snprintf (_RC (char*, utf8Dest),
                         maxSize,
                         "%llu",
                         _SC (unsigned long long, value.mValue));
  if ((result < 0) || (_SC (uint_t, result) >= maxSize))
    return 0;

  return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DReal&        value)
{
  char tempBuffer[MAX_INTEGER_STR_SIZE];
  uint_t result = 0;

   if (maxSize <= 1)
     return 0;

   *utf8Dest = 0;
   if (value.IsNull ())
     return 1;

   uint64_t intPart  = _SC (uint64_t, value.mValue.Integer ());
   uint64_t fracPart = _SC (uint64_t, value.mValue.Fractional ());
   if ((MAX_SIGNED_ABS <= intPart) || (MAX_SIGNED_ABS <= fracPart))
     {
       strcat (_RC (char*, utf8Dest),  "-");

       intPart  -= 1; intPart  = ~intPart;
       fracPart -= 1; fracPart = ~fracPart;

       result++;
     }

   if (snprintf (tempBuffer,
                 sizeof (tempBuffer),
                 "%llu",
                 _SC (unsigned long long, intPart)) < 0)
     {
       return 0;
     }

   if (result + strlen (tempBuffer) >= maxSize)
     return 0;

   strcpy (_RC (char*, utf8Dest + result), tempBuffer);
   result += strlen (tempBuffer);
   if (fracPart > 0)
     {
       if (result + 1 >= maxSize)
         return 0;
       else
         {
           utf8Dest[result++] = '.';
           utf8Dest[result]   = 0;
         }

       uint64_t precision = value.mValue.Precision () / 10;
       while (precision > fracPart)
         {
           if (result + 1 >= maxSize)
             return 0;

           else
             {
               utf8Dest[result++] = '0';
               utf8Dest[result]   = 0;
             }
           precision /= 10;
         }

       while ((fracPart % 10) == 0)
         fracPart /= 10;

       if (snprintf (tempBuffer,
                     sizeof (tempBuffer),
                     "%llu",
                     _SC (unsigned long long, fracPart)) < 0)
         {
           return 0;
         }

       if (result + strlen (tempBuffer) >= maxSize)
         return 0;

       strcpy (_RC (char*, utf8Dest + result), tempBuffer);
       result += strlen (tempBuffer);
     }

   assert (result < maxSize);
   assert ((strlen (_RC (char*, utf8Dest))) == result);

   return result + 1;
}


uint_t
Utf8Translator::Write (uint8_t* const      utf8Dest,
                       const uint_t        maxSize,
                       const DRichReal&    value)
{
  char tempBuffer[MAX_INTEGER_STR_SIZE];
  uint_t result = 0;

   if (maxSize <= 1)
     return 0;

   *utf8Dest = 0;
   if (value.IsNull ())
     return 1;

   uint64_t intPart  = _SC (uint64_t, value.mValue.Integer ());
   uint64_t fracPart = _SC (uint64_t, value.mValue.Fractional ());
   if ((MAX_SIGNED_ABS <= intPart) || (MAX_SIGNED_ABS <= fracPart))
     {
       strcat (_RC (char*, utf8Dest),  "-");

       intPart  -= 1; intPart  = ~intPart;
       fracPart -= 1; fracPart = ~fracPart;

       result++;
     }

   if (snprintf (tempBuffer,
                 sizeof (tempBuffer),
                 "%llu",
                 _SC (unsigned long long, intPart)) < 0)
     {
       return 0;
     }

   if (result + strlen (tempBuffer) >= maxSize)
     return 0;

   strcpy (_RC (char*, utf8Dest + result), tempBuffer);
   result += strlen (tempBuffer);
   if (fracPart > 0)
     {
       if (result + 1 >= maxSize)
         return 0;

       else
         {
           utf8Dest[result++] = '.';
           utf8Dest[result]   = 0;
         }

       uint64_t precision = value.mValue.Precision () / 10;
       while (precision > fracPart)
         {
           if (result + 1 >= maxSize)
             return 0;

           else
             {
               utf8Dest[result++] = '0';
               utf8Dest[result]   = 0;
             }
           precision /= 10;
         }

       while ((fracPart % 10) == 0)
         fracPart /= 10;

       if (snprintf (tempBuffer,
                     sizeof (tempBuffer),
                     "%llu",
                     _SC (unsigned long long, fracPart)) < 0)
         {
           return 0;
         }

       if (result + strlen (tempBuffer) >= maxSize)
         return 0;

       strcpy (_RC (char*, utf8Dest + result), tempBuffer);
       result += strlen (tempBuffer);
     }

   assert (result < maxSize);
   assert ((strlen (_RC (char*, utf8Dest))) == result);

   return result + 1;
}

