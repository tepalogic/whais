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

#include "dbs_exception.h"
#include "ps_valintep.h"
#include "dbs_values.h"

using namespace std;
using namespace pastra;
using namespace whisper;

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
new_bool (bool value, DBSBool* pOutBool)
{
  _placement_new (pOutBool, DBSBool (value));
}

inline static void
new_char (uint32_t value, DBSChar* pOutChar)
{
  _placement_new (pOutChar, DBSChar (value));
}

inline static void
new_date (int32_t  year,
          uint8_t  month,
          uint8_t  day,
          DBSDate* pOutDate)
{
  _placement_new (pOutDate, DBSDate (year, month, day));
}

inline static void
new_datetime (int32_t      year,
              uint8_t      month,
              uint8_t      day,
              uint8_t      hours,
              uint8_t      mins,
              uint8_t      secs,
              DBSDateTime* pOutDateTime)
{
  _placement_new (pOutDateTime,
                  DBSDateTime (year, month, day, hours, mins, secs));
}

inline static void
new_hirestime (int32_t       year,
               uint8_t       month,
               uint8_t       day,
               uint8_t       hours,
               uint8_t       mins,
               uint8_t       secs,
               uint32_t      usescs,
               DBSHiresTime* pOutHiresTime)
{
  _placement_new (pOutHiresTime,
                  DBSHiresTime (year, month, day, hours, mins, secs, usescs));
}

template <class T_OBJ, class T_VAL>
static void
new_integer (T_VAL value, T_OBJ* pValue)
{
  _placement_new (pValue, T_OBJ (value));
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSBool& value)
{
  pLocation[0] = (value.m_Value == false) ? 0 : 1;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSChar& value)
{
  _RC(uint32_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSDate& value)
{

  _RC(uint16_t*, pLocation)[0] = value.m_Year;
  pLocation[2] = value.m_Month;
  pLocation[3] = value.m_Day;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSDateTime& value)
{
  _RC(uint16_t*, pLocation)[0] = value.m_Year;
  pLocation[2] = value.m_Month;
  pLocation[3] = value.m_Day;
  pLocation[4] = value.m_Hour;
  pLocation[5] = value.m_Minutes;
  pLocation[6] = value.m_Seconds;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSHiresTime& value)
{
  _RC(uint32_t *, pLocation)[0] = value.m_Microsec;
  _RC(uint16_t *, pLocation)[2] = value.m_Year;
  pLocation[6]  = value.m_Month;
  pLocation[7]  = value.m_Day;
  pLocation[8]  = value.m_Hour;
  pLocation[9]  = value.m_Minutes;
  pLocation[10] = value.m_Seconds;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSInt8 &value)
{
  _RC(int8_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSInt16 &value)
{
  _RC(int16_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSInt32 &value)
{
  _RC(int32_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSInt64 &value)
{
  _RC(int64_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSReal& value)
{
  const uint_t integerSize    = 5;
  const uint_t fractionalSize = 3;

  int64_t temp;

  temp = value.m_Value.Integer ();
  memcpy (pLocation, &temp, integerSize);

  temp = value.m_Value.Fractional ();
  memcpy (pLocation + integerSize, &temp, fractionalSize);
}
void
PSValInterp::Store (uint8_t* pLocation, const DBSRichReal& value)
{
  const uint_t integerSize    = 8;
  const uint_t fractionalSize = 6;

  int64_t temp;

  temp = value.m_Value.Integer ();
  memcpy (pLocation, &temp, integerSize);

  temp = value.m_Value.Fractional ();
  memcpy (pLocation + integerSize, &temp, fractionalSize);
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSUInt8 &value)
{
  _RC(uint8_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSUInt16 &value)
{
  _RC(uint16_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSUInt32 &value)
{
  _RC(uint32_t*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (uint8_t* pLocation, const DBSUInt64 &value)
{
  _RC(uint64_t*, pLocation)[0] = value.m_Value;
}


void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSBool* pValue)
{
  new_bool (*pLocation != 0, pValue);
  assert ((*pLocation == 0) || (*pLocation == 1));
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSChar* pValue)
{
  new_char (_RC(const uint32_t*, pLocation)[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSDate* pValue)
{

  int16_t year  = _RC (const uint16_t*, pLocation)[0];
  uint8_t month = pLocation[2];
  uint8_t day   = pLocation[3];

  new_date (year, month, day, pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSDateTime* pValue)
{
  int16_t year    = _RC (const uint16_t*, pLocation)[0];
  uint8_t month   = pLocation[2];
  uint8_t day     = pLocation[3];
  uint8_t hours   = pLocation[4];
  uint8_t mins    = pLocation[5];
  uint8_t secs    = pLocation[6];

  new_datetime (year, month, day, hours, mins, secs, pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSHiresTime* pValue)
{
  int32_t usecs    = _RC (const uint32_t*, pLocation)[0];
  int16_t year     = _RC (const uint16_t*, pLocation)[2];
  uint8_t month    = pLocation[6];
  uint8_t day      = pLocation[7];
  uint8_t hours    = pLocation[8];
  uint8_t mins     = pLocation[9];
  uint8_t secs     = pLocation[10];

  new_hirestime (year, month, day, hours, mins, secs, usecs, pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSInt8* pValue)
{
  new_integer (_RC(const int8_t*, pLocation)[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSInt16* pValue)
{
  new_integer (_RC(const int16_t*, pLocation)[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSInt32* pValue)
{
  new_integer (_RC(const int32_t*, pLocation)[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSInt64* pValue)
{
  new_integer (_RC(const int64_t*, pLocation)[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSReal* pValue)
{
  const uint_t integerSize    = 5;
  const uint_t fractionalSize = 3;

  int64_t integer = 0;
  memcpy (&integer, pLocation, integerSize);

  if (integer & 0x8000000000)
    integer |= ~_SC (int64_t, 0xFFFFFFFFFF);

  int64_t fractional = 0;
  memcpy (&fractional, pLocation + integerSize, fractionalSize);

  if (fractional & 0x800000)
    fractional |= ~_SC (int64_t, 0xFFFFFF);

  *pValue = DBSReal (DBS_REAL_T (integer, fractional, DBS_REAL_PREC));
}
void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSRichReal* pValue)
{
  const uint_t integerSize    = 8;
  const uint_t fractionalSize = 6;

  int64_t integer = 0;
  memcpy (&integer, pLocation, integerSize);

  int64_t fractional = 0;
  memcpy (&fractional, pLocation + integerSize, fractionalSize);

  if (fractional & 0x800000000000)
    fractional |= ~_SC (int64_t, 0xFFFFFFFFFFFF);

  *pValue = DBSRichReal (DBS_RICHREAL_T (integer,
                                         fractional,
                                         DBS_RICHREAL_PREC));
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSUInt8 *pValue)
{
  new_integer (pLocation[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSUInt16 *pValue)
{
  new_integer (_RC(const uint16_t*, pLocation)[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSUInt32 *pValue)
{
  new_integer (_RC(const uint32_t*, pLocation)[0], pValue);
}

void
PSValInterp::Retrieve (const uint8_t* pLocation, DBSUInt64 *pValue)
{
  new_integer (_RC(const uint64_t*, pLocation)[0], pValue);
}

int
PSValInterp::Size (DBS_FIELD_TYPE type, bool isArray)
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
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
  }
}

int
PSValInterp::Alignment (DBS_FIELD_TYPE type, bool isArray)
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
    throw DBSException (NULL, _EXTRA (DBSException::FIELD_TYPE_INVALID));
  }
}
