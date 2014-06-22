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
#include "utils/endianness.h"

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


template<class T_OBJ, class T_VAL> static void
new_integer (T_VAL value, T_OBJ* const outValue)
{
  _placement_new (outValue, T_OBJ (value));
}


void
Serializer::Store (uint8_t* const dst, const DBool& value)
{
  assert (! value.IsNull ());

  dst[0] = value.mValue ? 1 : 0;
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

  uint8_t temp[sizeof (uint64_t)];

  store_le_int64 (value.mValue.Integer (), temp);

  const uint64_t i = value.mValue.Integer () & 0xFFFFFF8000000000;
  if ((i != 0) && (i != 0xFFFFFF8000000000))
    throw DBSException (_EXTRA (DBSException::NUMERIC_FAULT));

  memcpy (dst, &temp, integerSize);

  store_le_int64 (value.mValue.Fractional (), temp);

  const uint64_t f = value.mValue.Fractional () & 0xFFFFFFFFFF800000;
  if ((f != 0) && (f != 0xFFFFFFFFFF800000))
    throw DBSException (_EXTRA (DBSException::NUMERIC_FAULT));

  memcpy (dst + integerSize, &temp, fractionalSize);
}


void
Serializer::Store (uint8_t* const dst, const DRichReal& value)
{
  assert (! value.IsNull ());

  const uint_t integerSize    = 8;
  const uint_t fractionalSize = 6;

  store_le_int64 (value.mValue.Integer (), dst);

  uint8_t temp[sizeof (uint64_t)];

  store_le_int64 (value.mValue.Fractional (), temp);

  const uint64_t f = value.mValue.Fractional () & 0xFFFF800000000000;
  if ((f != 0) && (f != 0xFFFF800000000000))
    throw DBSException (_EXTRA (DBSException::NUMERIC_FAULT));

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

  const int16_t year  = load_le_int16 (src);
  const uint8_t month = src[2];
  const uint8_t day   = src[3];

  new_date (year, month, day, outValue);
}


void
Serializer::Load (const uint8_t* const src, DDateTime* const outValue)
{
  const int16_t year    = load_le_int16 (src);
  const uint8_t month   = src[2];
  const uint8_t day     = src[3];
  const uint8_t hours   = src[4];
  const uint8_t mins    = src[5];
  const uint8_t secs    = src[6];

  new_datetime (year, month, day, hours, mins, secs, outValue);
}


void
Serializer::Load (const uint8_t* const src, DHiresTime* const outValue)
{
  const int32_t usecs    = load_le_int32 (src);
  const int16_t year     = load_le_int16 (src + sizeof (uint32_t));
  const uint8_t month    = src[6];
  const uint8_t day      = src[7];
  const uint8_t hours    = src[8];
  const uint8_t mins     = src[9];
  const uint8_t secs     = src[10];

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


template<> bool
Serializer::ValidateBuffer<DBool> (const uint8_t* const buffer)
{
  return (buffer[0] == 0) || (buffer[0] == 1);
}


template<> bool
Serializer::ValidateBuffer<DChar> (const uint8_t* const buffer)
{
  try
  {
      DChar (load_le_int32 (buffer));
  }
  catch (...)
  {
      return false;
  }

  return true;
}


template<> bool
Serializer::ValidateBuffer<DDate> (const uint8_t* const buffer)
{
  try
  {
      const int16_t year  = load_le_int16 (buffer);
      const uint8_t month = buffer[2];
      const uint8_t day   = buffer[3];

      DDate (year, month, day);
  }
  catch (...)
  {
      return false;
  }
  return true;
}


template<> bool
Serializer::ValidateBuffer<DDateTime> (const uint8_t* const buffer)
{
  try
  {
      const int16_t year    = load_le_int16 (buffer);
      const uint8_t month   = buffer[2];
      const uint8_t day     = buffer[3];
      const uint8_t hours   = buffer[4];
      const uint8_t mins    = buffer[5];
      const uint8_t secs    = buffer[6];

      DDateTime (year, month, day, hours, mins, secs);
  }
  catch (...)
  {
      return false;
  }
  return true;
}


template<> bool
Serializer::ValidateBuffer<DHiresTime> (const uint8_t* const buffer)
{
  try
  {
      const int32_t usecs    = load_le_int32 (buffer);
      const int16_t year     = load_le_int16 (buffer + sizeof (uint32_t));
      const uint8_t month    = buffer[6];
      const uint8_t day      = buffer[7];
      const uint8_t hours    = buffer[8];
      const uint8_t mins     = buffer[9];
      const uint8_t secs     = buffer[10];

      DHiresTime (year, month, day, hours, mins, secs, usecs);
  }
  catch (...)
  {
      return false;
  }
  return true;
}


template<> bool
Serializer::ValidateBuffer<DReal> (const uint8_t* const buffer)
{
  const uint_t integerSize    = 5;
  const uint_t fractionalSize = 3;

  int64_t temp = 0;
  memcpy (&temp, buffer, integerSize);

  int64_t integer = load_le_int64 (_RC (const uint8_t*, &temp));

  if (integer & 0x8000000000)
    integer |= ~_SC (int64_t, 0xFFFFFFFFFF);

  temp = 0;
  memcpy (&temp, buffer + integerSize, fractionalSize);

  int64_t fractional = load_le_int64 (_RC (const uint8_t*, &temp));

  if (fractional & 0x800000)
    fractional |= ~_SC (int64_t, 0xFFFFFF);

  if ((integer < 0)
      && ((fractional <= -DBS_REAL_PREC) || (0 < fractional)))
    {
      return false;
    }
  else if ((integer > 0)
           && ((fractional < 0) || (DBS_REAL_PREC <= fractional)))
    {
      return false;
    }

  return true;
}


template<> bool
Serializer::ValidateBuffer<DRichReal> (const uint8_t* const buffer)
{
  const uint_t integerSize    = 8;
  const uint_t fractionalSize = 6;

  int64_t integer = load_le_int64 (buffer);

  int64_t temp = 0;
  memcpy (&temp, buffer + integerSize, fractionalSize);

  int64_t fractional = load_le_int64 (_RC (const uint8_t*, &temp));

  if (fractional & 0x800000000000)
    fractional |= ~_SC (int64_t, 0xFFFFFFFFFFFF);

  if ((integer < 0)
      && ((fractional <= -DBS_RICHREAL_PREC) || (0 < fractional)))
    {
      return false;
    }
  else if ((integer > 0)
           && ((fractional < 0) || (DBS_RICHREAL_PREC <= fractional)))
    {
      return false;
    }

  assert ((fractional < 0) || (fractional < DBS_RICHREAL_PREC));
  assert ((fractional > 0) || (fractional > -DBS_RICHREAL_PREC));

  return true;
}



Serializer::VALUE_VALIDATOR
Serializer::SelectValidator (const DBS_FIELD_TYPE itemType)
{
  switch (itemType)
  {
  case T_BOOL:
    return Serializer::ValidateBuffer<DBool>;

  case T_CHAR:
    return Serializer::ValidateBuffer<DChar>;

  case T_DATE:
    return Serializer::ValidateBuffer<DDate>;

  case T_DATETIME:
    return Serializer::ValidateBuffer<DDateTime>;

  case T_HIRESTIME:
    return Serializer::ValidateBuffer<DHiresTime>;

  case T_INT8:
    return Serializer::ValidateBuffer<DInt8>;

  case T_INT16:
    return Serializer::ValidateBuffer<DInt16>;

  case T_INT32:
    return Serializer::ValidateBuffer<DInt32>;

  case T_INT64:
    return Serializer::ValidateBuffer<DInt64>;

 case T_UINT8:
    return Serializer::ValidateBuffer<DUInt8>;

  case T_UINT16:
    return Serializer::ValidateBuffer<DUInt16>;

  case T_UINT32:
    return Serializer::ValidateBuffer<DUInt32>;

  case T_UINT64:
    return Serializer::ValidateBuffer<DUInt64>;

  case T_REAL:
    return Serializer::ValidateBuffer<DReal>;

  case T_RICHREAL:
    return Serializer::ValidateBuffer<DRichReal>;

  default:
    assert (false);
  }

  return NULL;
}




} //namespace pastra
} //namespace whisper

