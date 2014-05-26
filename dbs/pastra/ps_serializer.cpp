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
#include <cstring>

#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"
#include "utils/le_converter.h"

#include "ps_serializer.h"

using namespace std;

namespace whisper {
namespace pastra {


static const int PS_BOOL_SIZE                = 1;
static const int PS_CHAR_SIZE                = 4;
static const int PS_DATE_SIZE                = 4;
static const int PS_DATETIME_SIZE            = 7;
static const int PS_HIRESDATE_SIZE           = 11;
static const int PS_INT8_SIZE                = 1;
static const int PS_INT16_SIZE               = 2;
static const int PS_INT32_SIZE               = 4;
static const int PS_INT64_SIZE               = 8;
static const int PS_TEXT_SIZE                = 16;
static const int PS_ARRAY_SIZE               = 16;

static const int PS_REAL_SIZE                = 8;
static const int PS_RICHREAL_SIZE            = 14;

static const int PS_BOOL_ALIGN               = 1;
static const int PS_CHAR_ALIGN               = 4;
static const int PS_DATE_ALIGN               = 2;
static const int PS_DATETIME_ALIGN           = 2;
static const int PS_HIRESDATE_ALIGN          = 4;
static const int PS_INT8_ALIGN               = 1;
static const int PS_INT16_ALIGN              = 2;
static const int PS_INT32_ALIGN              = 4;
static const int PS_INT64_ALIGN              = 8;
static const int PS_TEXT_ALIGN               = 8;
static const int PS_ARRAY_ALIGN              = 8;



inline static void
new_bool (bool value, DBool* outBool)
{
  _placement_new (outBool, DBool (value));
}


inline static void
new_char (uint32_t value, DChar* const outChar)
{
  _placement_new (outChar, DChar (value));
}


inline static void
new_date (int32_t       year,
          uint8_t       month,
          uint8_t       day,
          DDate* const  outDate)
{
  _CC (int16_t&, outDate->mYear)   = year;
  _CC (uint8_t&, outDate->mMonth)  = month;
  _CC (uint8_t&, outDate->mDay)    = day;
  _CC (bool&,    outDate->mIsNull) = false;
}


inline static void
new_datetime (int32_t      year,
              uint8_t      month,
              uint8_t      day,
              uint8_t      hours,
              uint8_t      mins,
              uint8_t      secs,
              DDateTime*   outDateTime)
{
  _CC (int16_t&, outDateTime->mYear)    = year;
  _CC (uint8_t&, outDateTime->mMonth)   = month;
  _CC (uint8_t&, outDateTime->mDay)     = day;
  _CC (uint8_t&, outDateTime->mHour)    = hours;
  _CC (uint8_t&, outDateTime->mMinutes) = mins;
  _CC (uint8_t&, outDateTime->mSeconds) = secs;
  _CC (bool&,    outDateTime->mIsNull)  = false;
}


inline static void
new_hirestime (int32_t           year,
               uint8_t           month,
               uint8_t           day,
               uint8_t           hours,
               uint8_t           mins,
               uint8_t           secs,
               uint32_t          usescs,
               DHiresTime* const outHiresTime)
{
  _CC (uint32_t&, outHiresTime->mMicrosec)  = usescs;
  _CC (int16_t&,  outHiresTime->mYear)      = year;
  _CC (uint8_t&,  outHiresTime->mMonth)     = month;
  _CC (uint8_t&,  outHiresTime->mDay)       = day;
  _CC (uint8_t&,  outHiresTime->mHour)      = hours;
  _CC (uint8_t&,  outHiresTime->mMinutes)   = mins;
  _CC (uint8_t&,  outHiresTime->mSeconds)   = secs;
  _CC (bool&,     outHiresTime->mIsNull)    = false;
}


template<class T_OBJ, class T_VAL>
static void
new_integer (T_VAL value, T_OBJ* const outValue)
{
  _placement_new (outValue, T_OBJ (value));
}


void
Serializer::Store (uint8_t* const dst, const DBool& value)
{
  assert (! value.IsNull ());
  dst[0] = (value.mValue == false) ? 0 : 1;
}


void
Serializer::Store (uint8_t* const dst, const DChar& value)
{
  assert (! value.IsNull ());
  store_le_int32 (value.mValue, dst);
}


void
Serializer::Store (uint8_t* const dst, const DDate& value)
{
  assert (! value.IsNull ());

  store_le_int16 (value.mYear, dst);
  dst[2] = value.mMonth;
  dst[3] = value.mDay;
}


void
Serializer::Store (uint8_t* const dst, const DDateTime& value)
{
  assert (! value.IsNull ());

  store_le_int16 (value.mYear, dst);
  dst[2] = value.mMonth;
  dst[3] = value.mDay;
  dst[4] = value.mHour;
  dst[5] = value.mMinutes;
  dst[6] = value.mSeconds;
}


void
Serializer::Store (uint8_t* const dst, const DHiresTime& value)
{
  assert (! value.IsNull ());

  store_le_int32 (value.mMicrosec, dst);
  store_le_int16 (value.mYear, dst + sizeof (uint32_t));
  dst[6]  = value.mMonth;
  dst[7]  = value.mDay;
  dst[8]  = value.mHour;
  dst[9]  = value.mMinutes;
  dst[10] = value.mSeconds;
}


void
Serializer::Store (uint8_t* const dst, const DInt8 &value)
{
  assert (! value.IsNull ());

  dst[0] = value.mValue;
}


void
Serializer::Store (uint8_t* const dst, const DInt16 &value)
{
  assert (! value.IsNull ());

  store_le_int16 (value.mValue, dst);
}


void
Serializer::Store (uint8_t* const dst, const DInt32 &value)
{
  assert (! value.IsNull ());

  store_le_int32 (value.mValue, dst);
}


void
Serializer::Store (uint8_t* const dst, const DInt64 &value)
{
  assert (! value.IsNull ());

  store_le_int64 (value.mValue, dst);
}


void
Serializer::Store (uint8_t* const dst, const DReal& value)
{
  assert (! value.IsNull ());

  const uint_t integerSize    = 5;
  const uint_t fractionalSize = 3;

  int64_t temp;

  store_le_int64 (value.mValue.Integer (), _RC (uint8_t*, &temp));
  memcpy (dst, &temp, integerSize);

  store_le_int64 (value.mValue.Fractional (), _RC (uint8_t*, &temp));
  memcpy (dst + integerSize, &temp, fractionalSize);
}


void
Serializer::Store (uint8_t* const dst, const DRichReal& value)
{
  assert (! value.IsNull ());

  const uint_t integerSize    = 8;
  const uint_t fractionalSize = 6;

  store_le_int64 (value.mValue.Integer (), dst);

  int64_t temp;

  store_le_int64 (value.mValue.Fractional (), _RC (uint8_t*, &temp));
  memcpy (dst + integerSize, &temp, fractionalSize);
}


void
Serializer::Store (uint8_t* const dst, const DUInt8 &value)
{
  dst[0] = value.mValue;
}


void
Serializer::Store (uint8_t* const dst, const DUInt16 &value)
{
  assert (! value.IsNull ());

  store_le_int16 (value.mValue, dst);
}


void
Serializer::Store (uint8_t* const dst, const DUInt32 &value)
{
  assert (! value.IsNull ());

  store_le_int32 (value.mValue, dst);
}

void
Serializer::Store (uint8_t* const dst, const DUInt64 &value)
{
  assert (! value.IsNull ());

  store_le_int64 (value.mValue, dst);
}


void
Serializer::Load (const uint8_t* const src, DBool* outValue)
{
  new_bool (*src != 0, outValue);

  assert ((*src == 0) || (*src == 1));
}


void
Serializer::Load (const uint8_t* const src, DChar* outValue)
{
  new_char (load_le_int32 (src), outValue);
}


void
Serializer::Load (const uint8_t* const src, DDate* const outValue)
{

  int16_t year  = load_le_int16 (src);
  uint8_t month = src[2];
  uint8_t day   = src[3];

  new_date (year, month, day, outValue);
}


void
Serializer::Load (const uint8_t* const src, DDateTime* const outValue)
{
  int16_t year    = load_le_int16 (src);
  uint8_t month   = src[2];
  uint8_t day     = src[3];
  uint8_t hours   = src[4];
  uint8_t mins    = src[5];
  uint8_t secs    = src[6];

  new_datetime (year, month, day, hours, mins, secs, outValue);
}


void
Serializer::Load (const uint8_t* const src, DHiresTime* const outValue)
{
  int32_t usecs    = load_le_int32 (src);
  int16_t year     = load_le_int16 (src + sizeof (uint32_t));
  uint8_t month    = src[6];
  uint8_t day      = src[7];
  uint8_t hours    = src[8];
  uint8_t mins     = src[9];
  uint8_t secs     = src[10];

  new_hirestime (year, month, day, hours, mins, secs, usecs, outValue);
}


void
Serializer::Load (const uint8_t* const src, DInt8* const outValue)
{
  new_integer (src[0], outValue);
}


void
Serializer::Load (const uint8_t* const src, DInt16* const outValue)
{
  new_integer (load_le_int16 (src), outValue);
}


void
Serializer::Load (const uint8_t* const src, DInt32* const outValue)
{
  new_integer (load_le_int32 (src), outValue);
}


void
Serializer::Load (const uint8_t* const src, DInt64* const outValue)
{
  new_integer (load_le_int64 (src), outValue);
}


void
Serializer::Load (const uint8_t* const src, DReal* const outValue)
{
  const uint_t integerSize    = 5;
  const uint_t fractionalSize = 3;

  int64_t temp = 0;
  memcpy (&temp, src, integerSize);

  int64_t integer = load_le_int64 (_RC (const uint8_t*, &temp));

  if (integer & 0x8000000000)
    integer |= ~_SC (int64_t, 0xFFFFFFFFFF);

  temp = 0;
  memcpy (&temp, src + integerSize, fractionalSize);

  int64_t fractional = load_le_int64 (_RC (const uint8_t*, &temp));

  if (fractional & 0x800000)
    fractional |= ~_SC (int64_t, 0xFFFFFF);

  *outValue = DReal (DBS_REAL_T (integer, fractional, DBS_REAL_PREC));
}


void
Serializer::Load (const uint8_t* const src, DRichReal* const outValue)
{
  const uint_t integerSize    = 8;
  const uint_t fractionalSize = 6;

  int64_t integer = load_le_int64 (src);

  int64_t temp = 0;
  memcpy (&temp, src + integerSize, fractionalSize);

  int64_t fractional = load_le_int64 (_RC (const uint8_t*, &temp));

  if (fractional & 0x800000000000)
    fractional |= ~_SC (int64_t, 0xFFFFFFFFFFFF);

  *outValue = DRichReal (DBS_RICHREAL_T (integer,
                                         fractional,
                                         DBS_RICHREAL_PREC));
}

void
Serializer::Load (const uint8_t* src, DUInt8* const outValue)
{
  new_integer (src[0], outValue);
}

void
Serializer::Load (const uint8_t* src, DUInt16* const outValue)
{
  new_integer (load_le_int16 (src), outValue);
}

void
Serializer::Load (const uint8_t* src, DUInt32* const outValue)
{
  new_integer (load_le_int32 (src), outValue);
}

void
Serializer::Load (const uint8_t* src, DUInt64* const outValue)
{
  new_integer (load_le_int64 (src), outValue);
}


int
Serializer::Size (const DBS_FIELD_TYPE type, const bool isArray)
{
  if (isArray)
    return PS_ARRAY_SIZE;

  switch (type)
  {
  case T_BOOL:
    return PS_BOOL_SIZE;

  case T_CHAR:
    return PS_CHAR_SIZE;

  case T_DATE:
    return PS_DATE_SIZE;

  case T_DATETIME:
    return PS_DATETIME_SIZE;

  case T_HIRESTIME:
    return PS_HIRESDATE_SIZE;

  case T_REAL:
    return PS_REAL_SIZE;

  case T_RICHREAL:
    return PS_RICHREAL_SIZE;

  case T_UINT8:
  case T_INT8:
    return PS_INT8_SIZE;

  case T_UINT16:
  case T_INT16:
      return PS_INT16_SIZE;

  case T_UINT32:
  case T_INT32:
      return PS_INT32_SIZE;

  case T_UINT64:
  case T_INT64:
      return PS_INT64_SIZE;

  case T_TEXT:
    return PS_TEXT_SIZE;

  default:
    assert (0);
    throw DBSException (_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
  }
}


int
Serializer::Alignment (const DBS_FIELD_TYPE type, bool isArray)
{
  if (isArray)
    return PS_ARRAY_ALIGN;

  switch (type)
  {
  case T_BOOL:
    return PS_BOOL_ALIGN;

  case T_CHAR:
    return PS_CHAR_ALIGN;

  case T_DATE:
    return PS_DATE_ALIGN;

  case T_DATETIME:
    return PS_DATETIME_ALIGN;

  case T_HIRESTIME:
    return PS_HIRESDATE_ALIGN;

  case T_REAL:
    return 1;

  case T_RICHREAL:
    return 1;

  case T_UINT8:
  case T_INT8:
    return PS_INT8_ALIGN;

  case T_UINT16:
  case T_INT16:
      return PS_INT16_ALIGN;

  case T_UINT32:
  case T_INT32:
      return PS_INT32_ALIGN;

  case T_UINT64:
  case T_INT64:
      return PS_INT64_ALIGN;

  case T_TEXT:
    return PS_TEXT_ALIGN;

  default:
    assert (false);

    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
  }
}


} //namespace pastra
} //namespace whisper

