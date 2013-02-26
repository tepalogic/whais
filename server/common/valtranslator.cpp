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
#include <limits>

#include "utils/include/utf8.h"

#include "valtranslator.h"

static const D_UINT   MAX_INTEGER_STR_SIZE  = 32;
static const D_UINT64 MAX_SIGNED_ABS        = 0x8000000000000000ull;
static const D_INT64  USECS_PRECISION       = 1000000;
static const D_UINT   MAX_UTF8_CHAR_SIZE    = 6;

static D_INT64
read_real (const D_UINT8* pLocation,
           D_INT64* const pIntPart,
           D_INT64* const pFraPart,
           D_INT64* const pPrecision )
{
  D_INT64 size     = 0;
  bool    negative = false;

  if (*pLocation == '-')
    {
      pLocation++;
      if ((*pLocation < '0') || (*pLocation > '9'))
        return 0;

      size++;
      negative = true;
    }

  D_UINT64 intPart = 0, fracPart = 0, precision = 1;
  while (('0' <= *pLocation) && (*pLocation <= '9' ))
    {
      if ((intPart * 10 + (*pLocation - '0')) > MAX_SIGNED_ABS)
        return 0; // Overflow

      intPart *= 10;
      intPart += *pLocation - '0';

      pLocation++, size++;
    }

  if (*pLocation == '.')
    {
      pLocation++, size++;
      if ((*pLocation < '0') || (*pLocation > '9'))
        return 0;
    }

  while (('0' <= *pLocation) && (*pLocation <= '9' ))
    {
      if ((fracPart > (fracPart * 10 + (*pLocation - '0')))
          || (precision > precision * 10))
        {
          return 0; // Overflow!
        }

      fracPart  *= 10;
      fracPart  += *pLocation - '0';
      precision *= 10;

      pLocation++, size++;
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

  *pIntPart   = intPart;
  *pFraPart   = fracPart;
  *pPrecision = precision;

  return size;
}

static D_UINT64
read_integer (const D_UINT8* pLocation,
              D_UINT* const   pCount,
              const bool      isSigned)
{
  D_UINT64 result     = 0;
  bool     isNegative = false;

  *pCount = 0;
  if (*pLocation == '-')
    {
      ++pLocation;
      if (! isSigned || (*pLocation < '0') || (*pLocation > '9'))
        return 0;

      isNegative  = true;
      *pCount    += 1;
    }

  while (*pLocation >= '0' && *pLocation <= '9')
    {
      if ((isSigned && ((result * 10 + (*pLocation - '0')) > MAX_SIGNED_ABS))
          || ( ! isSigned && (result > (result * 10 + (*pLocation - '0')))))
        {
          return 0; // Overflow condition
        }

      result *= 10;
      result += *pLocation - '0';

      ++pLocation;
      *pCount += 1;
    }

  if (isNegative && (result > 0))
    {
      result -= 1;
      result  = ~result;
    }

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSBool* const       pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSBool ();
      return 1;
    }
  else if ( 2 * sizeof (D_UINT8) > srcSize)
    return 0;
  else
    {
      *pValue = DBSBool (*utf8Src == '1');
      if ((utf8Src[1] != 0)
          || ((*utf8Src != '1') && (*utf8Src != '0')))
        {
          return 0;
        }

      return 2;
    }

  return 0;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSChar* const       pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSChar ();
      return 1;
    }

  const D_UINT result = get_utf8_char_size (*utf8Src);
  if ((result == 0) || (result + 1 > srcSize))
    return 0;
  else
    {
      D_UINT32 ch;
      if ((decode_utf8_char (utf8Src, &ch) != result)
          || (ch == 0)
          || (utf8Src[result] != 0))
        {
          return 0;
        }

      *pValue = DBSChar (ch);
    }

  return result + 1;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSDate* const       pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSDate ();
      return 1;
    }

  D_UINT temp   = 0;
  D_UINT result = 0;

  const D_INT64 year = read_integer (utf8Src, &temp, true);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const D_UINT64 month = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const D_UINT64 day = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != 0) || (result > srcSize))
    return 0;

  *pValue = DBSDate (year, month, day);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSDateTime* const   pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSDateTime ();
      return 1;
    }

  D_UINT temp   = 0;
  D_UINT result = 0;

  const D_INT64 year = read_integer (utf8Src, &temp, true);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const D_UINT64 month = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const D_UINT64 day = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ' ') || (result > srcSize))
    return 0;

  const D_UINT64 hour = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  const D_UINT64 min = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  const D_UINT64 secs = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != 0) || (result > srcSize))
    return 0;

  *pValue = DBSDateTime (year, month, day, hour, min, secs);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSHiresTime* const  pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSHiresTime ();
      return 1;
    }

  D_UINT temp   = 0;
  D_UINT result = 0;

  const D_INT64 year = read_integer (utf8Src, &temp, true);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const D_UINT64 month = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != '/') || (result > srcSize))
    return 0;

  const D_UINT64 day = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ' ') || (result > srcSize))
    return 0;

  const D_UINT64 hour = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  const D_UINT64 min = read_integer (utf8Src + result, &temp, false);
  result += temp;
  if ((temp == 0) || (utf8Src[result++] != ':') || (result > srcSize))
    return 0;

  D_INT64 secs, usecs, usecPrec;
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

  *pValue = DBSHiresTime (year, month, day, hour, min, secs, usecs);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSInt8* const       pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSInt8 ();
      return 1;
    }

  D_UINT        result = 0;
  const D_INT64 value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_INT8>::min ())
      || (std::numeric_limits<D_INT8>::max () < value))
    {
      return 0;
    }

  *pValue = DBSInt8 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSInt16* const      pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSInt16 ();
      return 1;
    }

  D_UINT        result = 0;
  const D_INT64 value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_INT16>::min ())
      || (std::numeric_limits<D_INT16>::max () < value))
    {
      return 0;
    }
  *pValue = DBSInt16 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSInt32* const      pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSInt32 ();
      return 1;
    }

  D_UINT        result = 0;
  const D_INT64 value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_INT32>::min ())
      || (std::numeric_limits<D_INT32>::max () < value))
    {
      return 0;
    }

  *pValue = DBSInt32 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSInt64* const      pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSInt64 ();
      return 1;
    }

  D_UINT        result = 0;
  const D_INT64 value  = read_integer (utf8Src, &result, true);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_INT64>::min ())
      || (std::numeric_limits<D_INT64>::max () < value))
    {
      return 0;
    }
  *pValue = DBSInt64 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSUInt8* const      pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSUInt8 ();
      return 1;
    }

  D_UINT         result = 0;
  const D_UINT64 value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_UINT8>::min ())
      || (std::numeric_limits<D_UINT8>::max () < value))
    {
      return 0;
    }
  *pValue = DBSUInt8 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSUInt16* const     pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSUInt16 ();
      return 1;
    }

  D_UINT         result = 0;
  const D_UINT64 value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_UINT16>::min ())
      || (std::numeric_limits<D_UINT16>::max () < value))
    {
      return 0;
    }
  *pValue = DBSUInt16 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSUInt32* const     pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSUInt32 ();
      return 1;
    }

  D_UINT         result = 0;
  const D_UINT64 value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_UINT32>::min ())
      || (std::numeric_limits<D_UINT32>::max () < value))
    {
      return 0;
    }

  *pValue = DBSUInt32 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*       utf8Src,
                      const D_UINT         srcSize,
                      DBSUInt64* const     pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSUInt64 ();
      return 1;
    }

  D_UINT         result = 0;
  const D_UINT64 value  = read_integer (utf8Src, &result, false);
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize)
      || (value < std::numeric_limits<D_UINT64>::min ())
      || (std::numeric_limits<D_UINT64>::max () < value))
    {
      return 0;
    }
  *pValue = DBSUInt64 (value);

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*      utf8Src,
                      const D_UINT        srcSize,
                      DBSReal* const      pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSReal ();
      return 1;
    }

  D_INT64 intPart, fracPart, precision;
  D_UINT  result  = read_real (utf8Src, &intPart, &fracPart, &precision);
  assert ((precision == 1) || (precision % 10 == 0));
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize))
    {
      return 0;
    }

  *pValue = DBSReal (DBS_REAL_T (intPart, fracPart, precision));

  return result;
}

D_UINT
Utf8Translator::Read (const D_UINT8*      utf8Src,
                      const D_UINT        srcSize,
                      DBSRichReal* const  pValue)
{
  if (sizeof (*utf8Src) > srcSize)
    return 0;

  if (*utf8Src == 0)
    {
      *pValue = DBSRichReal ();
      return 1;
    }

  D_INT64 intPart, fracPart, precision;
  D_UINT  result  = read_real (utf8Src, &intPart, &fracPart, &precision);
  assert ((precision == 1) || (precision % 10 == 0));
  if ((result == 0)
      || (utf8Src[result++] != 0)
      || (result > srcSize))
    {
      return 0;
    }

  *pValue = DBSRichReal (DBS_RICHREAL_T (intPart, fracPart, precision));

  return result;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSBool&      value)
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
  else if (value.m_Value)
    utf8Dest[0] = '1';
  else
    utf8Dest[0] = '0';

  utf8Dest[1] = 0;
  return 2;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSChar&      value)
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

  D_UINT result = encode_utf8_char (value.m_Value, utf8Dest);
  utf8Dest[result++] = 0;

  return result;
}

D_UINT
Utf8Translator::Write (D_UINT8* const    utf8Dest,
                       const D_UINT      maxSize,
                       const DBSDate&    value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%d/%u/%u",
                           value.m_Year,
                           value.m_Month,
                           value.m_Day);

  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;

}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSDateTime&  value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%d/%u/%u %u:%u:%u",
                           value.m_Year,
                           value.m_Month,
                           value.m_Day,
                           value.m_Hour,
                           value.m_Minutes,
                           value.m_Seconds);
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSHiresTime& value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }


  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%d/%u/%u %u:%u:%u.",
                           value.m_Year,
                           value.m_Month,
                           value.m_Day,
                           value.m_Hour,
                           value.m_Minutes,
                           value.m_Seconds);

  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  D_UINT64 usecPrec = USECS_PRECISION / 10;
  D_UINT32 msec_t   = value.m_Microsec;
  while (usecPrec > msec_t)
    {
      utf8Dest[result++] = '0';
      usecPrec /= 10;

      if (_SC (D_UINT, result) >= maxSize)
        return 0;
    }

  while ((msec_t % 10) == 0)
    msec_t /= 10;

  const D_INT64 res_t = result;

  result = snprintf (_RC (D_CHAR*, utf8Dest + result),
                     maxSize - result,
                     "%u",
                     msec_t);

  if ((result < 0) || (result + res_t >= maxSize))
    return 0;

  return result + res_t + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSInt8&      value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%lld",
                           _SC (long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSInt16&     value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%lld",
                           _SC (long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSInt32&     value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%lld",
                           _SC (long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSInt64&     value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%lld",
                           _SC (long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}


D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSUInt8&     value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%llu",
                           _SC (unsigned long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSUInt16&    value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%llu",
                           _SC (unsigned long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSUInt32&    value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%llu",
                           _SC (unsigned long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSUInt64&    value)
{
  if (maxSize == 0)
    return 0;

  if (value.IsNull ())
    {
      utf8Dest[0] = 0;
      return 1;
    }

  D_INT result = snprintf (_RC (D_CHAR*, utf8Dest),
                           maxSize,
                           "%llu",
                           _SC (unsigned long long, value.m_Value));
  if ((result < 0) || (_SC (D_UINT, result) >= maxSize))
    return 0;

  return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSReal&      value)
{
  D_CHAR tempBuffer[MAX_INTEGER_STR_SIZE];
  D_UINT result = 0;

   if (maxSize <= 1)
     return 0;

   *utf8Dest = 0;
   if (value.IsNull ())
     return 1;

   D_UINT64 intPart  = _SC (D_UINT64, value.m_Value.Integer ());
   D_UINT64 fracPart = _SC (D_UINT64, value.m_Value.Fractional ());
   if ((MAX_SIGNED_ABS <= intPart) || (MAX_SIGNED_ABS <= fracPart))
     {
       strcat (_RC (D_CHAR*, utf8Dest),  "-");

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

   strcpy (_RC (D_CHAR*, utf8Dest + result), tempBuffer);
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

       D_UINT64 precision = value.m_Value.Precision () / 10;
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

       strcpy (_RC (D_CHAR*, utf8Dest + result), tempBuffer);
       result += strlen (tempBuffer);
     }

   assert (result < maxSize);
   assert ((strlen (_RC (D_CHAR*, utf8Dest))) == result);

   return result + 1;
}

D_UINT
Utf8Translator::Write (D_UINT8* const      utf8Dest,
                       const D_UINT        maxSize,
                       const DBSRichReal&  value)
{
  D_CHAR tempBuffer[MAX_INTEGER_STR_SIZE];
  D_UINT result = 0;

   if (maxSize <= 1)
     return 0;

   *utf8Dest = 0;
   if (value.IsNull ())
     return 1;

   D_UINT64 intPart  = _SC (D_UINT64, value.m_Value.Integer ());
   D_UINT64 fracPart = _SC (D_UINT64, value.m_Value.Fractional ());
   if ((MAX_SIGNED_ABS <= intPart) || (MAX_SIGNED_ABS <= fracPart))
     {
       strcat (_RC (D_CHAR*, utf8Dest),  "-");

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

   strcpy (_RC (D_CHAR*, utf8Dest + result), tempBuffer);
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

       D_UINT64 precision = value.m_Value.Precision () / 10;
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

       strcpy (_RC (D_CHAR*, utf8Dest + result), tempBuffer);
       result += strlen (tempBuffer);
     }

   assert (result < maxSize);
   assert ((strlen (_RC (D_CHAR*, utf8Dest))) == result);

   return result + 1;
}
