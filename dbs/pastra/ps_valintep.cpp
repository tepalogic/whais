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

#include "dbs_exception.h"

#include "ps_valintep.h"

using namespace std;
using namespace pastra;

static const D_INT PS_BOOL_SIZE                = 1;
static const D_INT PS_CHAR_SIZE                = 4;
static const D_INT PS_DATE_SIZE                = 4;
static const D_INT PS_DATETIME_SIZE            = 7;
static const D_INT PS_HIRESDATE_SIZE           = 11;
static const D_INT PS_INT8_SIZE                = 1;
static const D_INT PS_INT16_SIZE               = 2;
static const D_INT PS_INT32_SIZE               = 4;
static const D_INT PS_INT64_SIZE               = 8;
static const D_INT PS_REAL_SIZE                = sizeof (float);
static const D_INT PS_RICHREAL_SIZE            = sizeof (long double);
static const D_INT PS_TEXT_SIZE                = 16;
static const D_INT PS_ARRAY_SIZE               = 16;

static const D_INT PS_BOOL_ALIGN               = 1;
static const D_INT PS_CHAR_ALIGN               = 4;
static const D_INT PS_DATE_ALIGN               = 2;
static const D_INT PS_DATETIME_ALIGN           = 2;
static const D_INT PS_HIRESDATE_ALIGN          = 4;
static const D_INT PS_INT8_ALIGN               = 1;
static const D_INT PS_INT16_ALIGN              = 2;
static const D_INT PS_INT32_ALIGN              = 4;
static const D_INT PS_INT64_ALIGN              = 8;
static const D_INT PS_REAL_ALIGN               = 0; //Based on system implementation
static const D_INT PS_RICHREAL_ALIGN           = 0; //Based on system implementation
static const D_INT PS_TEXT_ALIGN               = 8;
static const D_INT PS_ARRAY_ALIGN              = 8;

inline static void
new_bool (bool value, DBSBool* pOutBool)
{
  _placement_new (pOutBool, DBSBool (value));
}

inline static void
new_char (D_UINT32 value, DBSChar* pOutChar)
{
  _placement_new (pOutChar, DBSChar (value));
}

inline static void
new_date (D_INT32  year,
          D_UINT8  month,
          D_UINT8  day,
          DBSDate* pOutDate)
{
  _placement_new (pOutDate, DBSDate (year, month, day));
}

inline static void
new_datetime (D_INT32      year,
              D_UINT8      month,
              D_UINT8      day,
              D_UINT8      hours,
              D_UINT8      mins,
              D_UINT8      secs,
              DBSDateTime* pOutDateTime)
{
  _placement_new (pOutDateTime,
                  DBSDateTime (year, month, day, hours, mins, secs));
}

inline static void
new_hirestime (D_INT32       year,
               D_UINT8       month,
               D_UINT8       day,
               D_UINT8       hours,
               D_UINT8       mins,
               D_UINT8       secs,
               D_UINT32      usescs,
               DBSHiresTime* pOutHiresTime)
{
  _placement_new (pOutHiresTime,
                  DBSHiresTime (year, month, day, hours, mins, secs, usescs));
}

template <class T_OBJ, class T_VAL>
static void
new_integer (T_VAL value, T_OBJ* pOutValue)
{
  _placement_new (pOutValue, T_OBJ (value));
}

template <class T_OBJ, class T_REAL>
static void
new_real (T_REAL value, T_OBJ* pValue)
{
  _placement_new (pValue, T_OBJ (value));
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSBool& value)
{
  pLocation[0] = (value.m_Value == false) ? 0 : 1;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSChar& value)
{
  _RC(D_UINT32*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSDate& value)
{

  _RC(D_UINT16*, pLocation)[0] = value.m_Year;
  pLocation[2] = value.m_Month;
  pLocation[3] = value.m_Day;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSDateTime& value)
{
  _RC(D_UINT16*, pLocation)[0] = value.m_Year;
  pLocation[2] = value.m_Month;
  pLocation[3] = value.m_Day;
  pLocation[4] = value.m_Hour;
  pLocation[5] = value.m_Minutes;
  pLocation[6] = value.m_Seconds;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSHiresTime& value)
{
  _RC(D_UINT32 *, pLocation)[0] = value.m_Microsec;
  _RC(D_UINT16 *, pLocation)[2] = value.m_Year;
  pLocation[6]  = value.m_Month;
  pLocation[7]  = value.m_Day;
  pLocation[8]  = value.m_Hour;
  pLocation[9]  = value.m_Minutes;
  pLocation[10] = value.m_Seconds;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSInt8 &value)
{
  _RC(D_INT8*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSInt16 &value)
{
  _RC(D_INT16*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSInt32 &value)
{
  _RC(D_INT32*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSInt64 &value)
{
  _RC(D_INT64*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSReal& value)
{
  *_RC(float*, pLocation) = value.m_Value;
}
void
PSValInterp::Store (D_UINT8* pLocation, const DBSRichReal& value)
{
  _RC(long double*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSUInt8 &value)
{
  _RC(D_UINT8*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSUInt16 &value)
{
  _RC(D_UINT16*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSUInt32 &value)
{
  _RC(D_UINT32*, pLocation)[0] = value.m_Value;
}

void
PSValInterp::Store (D_UINT8* pLocation, const DBSUInt64 &value)
{
  _RC(D_UINT64*, pLocation)[0] = value.m_Value;
}


/////////////////////////////MARKER

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSBool* pOutValue)
{
  new_bool (*pLocation != 0, pOutValue);
  assert ((*pLocation == 0) || (*pLocation == 1));
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSChar* pOutValue)
{
  new_char (_RC(const D_UINT32*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSDate* pOutValue)
{

  D_INT16 year  = _RC (const D_UINT16*, pLocation)[0];
  D_UINT8 month = pLocation[2];
  D_UINT8 day   = pLocation[3];

  new_date (year, month, day, pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSDateTime* pOutValue)
{
  D_INT16 year    = _RC (const D_UINT16*, pLocation)[0];
  D_UINT8 month   = pLocation[2];
  D_UINT8 day     = pLocation[3];
  D_UINT8 hours   = pLocation[4];
  D_UINT8 mins    = pLocation[5];
  D_UINT8 secs    = pLocation[6];

  new_datetime (year, month, day, hours, mins, secs, pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSHiresTime* pOutValue)
{
  D_INT32 usecs    = _RC (const D_UINT32*, pLocation)[0];
  D_INT16 year     = _RC (const D_UINT16*, pLocation)[2];
  D_UINT8 month    = pLocation[6];
  D_UINT8 day      = pLocation[7];
  D_UINT8 hours    = pLocation[8];
  D_UINT8 mins     = pLocation[9];
  D_UINT8 secs     = pLocation[10];

  new_hirestime (year, month, day, hours, mins, secs, usecs, pOutValue);

}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSInt8* pOutValue)
{
  new_integer (_RC(const D_INT8*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSInt16* pOutValue)
{
  new_integer (_RC(const D_INT16*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSInt32* pOutValue)
{
  new_integer (_RC(const D_INT32*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSInt64* pOutValue)
{
  new_integer (_RC(const D_INT64*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSReal* pOutValue)
{
  new_real (_RC (const float*, pLocation)[0], pOutValue);
}
void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSRichReal* pOutValue)
{
  new_real (_RC (const long double*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSUInt8 *pOutValue)
{
  new_integer (pLocation[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSUInt16 *pOutValue)
{
  new_integer (_RC(const D_UINT16*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSUInt32 *pOutValue)
{
  new_integer (_RC(const D_UINT32*, pLocation)[0], pOutValue);
}

void
PSValInterp::Retrieve (const D_UINT8* pLocation, DBSUInt64 *pOutValue)
{
  new_integer (_RC(const D_UINT64*, pLocation)[0], pOutValue);
}

D_INT
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

D_INT
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
    {
      if (PS_REAL_SIZE >= 4 && PS_REAL_SIZE < 8)
        return 4;
      else if (PS_REAL_SIZE == 8)
        return 8;
      else
        {
          assert (false);
          throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));
        }
    }
  case T_RICHREAL:
    {
      if (PS_RICHREAL_SIZE >= 8 && PS_RICHREAL_SIZE < 16)
        return 8;
      else if (PS_RICHREAL_SIZE == 16)
        return 16;
      else
        {
          assert (false);
          throw DBSException (NULL, _EXTRA (DBSException::OPER_NOT_SUPPORTED));
        }
    }
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

