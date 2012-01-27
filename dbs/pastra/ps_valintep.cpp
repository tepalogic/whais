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

static void new_bool (DBSBool *object, bool value);
static void new_char (DBSChar *object, D_UINT32 value);
static void new_date (DBSDate *object, D_INT32 year, D_UINT8 month, D_UINT8 day);
static void new_datetime (DBSDateTime *object, D_INT32 year, D_UINT8 month, D_UINT8 day, D_UINT8 hours, D_UINT8 minutes, D_UINT8 seconds);
static void new_hirestime (DBSHiresTime *object, D_INT32 year, D_UINT8 month, D_UINT8 day, D_UINT8 hours, D_UINT8 minutes, D_UINT8 seconds, D_UINT32 microsec);
template <class T_OBJ, class T_VAL>
static void new_integer (T_OBJ *object, T_VAL value);
template <class T_OBJ, class T_REAL>
static void new_real (T_OBJ *object, T_REAL value);

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
static const D_INT PS_REAL_SIZE                = sizeof (DBSReal::m_Value);
static const D_INT PS_RICHREAL_SIZE            = sizeof (DBSRichReal::m_Value);
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
static const D_INT PS_REAL_ALIGN               = sizeof (DBSReal::m_Value);
static const D_INT PS_RICHREAL_ALIGN           = sizeof (DBSRichReal::m_Value);
static const D_INT PS_TEXT_ALIGN               = 8;
static const D_INT PS_ARRAY_ALIGN              = 8;


void
PSValInterp::Store (const DBSBool &rSource, D_UINT8 *pDestination)
{
  if (rSource.m_Value == false)
    pDestination[0] = 0;
  else
    pDestination[0] = 1;
}

void
PSValInterp::Store (const DBSChar &rSource, D_UINT8 *pDestination)
{
  _RC(D_UINT32 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSDate &rSource, D_UINT8 *pDestination)
{

  _RC(D_UINT16 *, pDestination)[0] = rSource.m_Year;
  pDestination[2] = rSource.m_Month;
  pDestination[3] = rSource.m_Day;
}

void
PSValInterp::Store (const DBSDateTime &rSource, D_UINT8 *pDestination)
{
  _RC(D_UINT16 *, pDestination)[0] = rSource.m_Year;
  pDestination[2] = rSource.m_Month;
  pDestination[3] = rSource.m_Day;
  pDestination[4] = rSource.m_Hour;
  pDestination[5] = rSource.m_Minutes;
  pDestination[6] = rSource.m_Seconds;
}

void
PSValInterp::Store (const DBSHiresTime &rSource, D_UINT8 *pDestination)
{
  _RC(D_UINT32 *, pDestination)[0] = rSource.m_Microsec;
  _RC(D_UINT16 *, pDestination)[2] = rSource.m_Year;
  pDestination[6] = rSource.m_Month;
  pDestination[7] = rSource.m_Day;
  pDestination[8] = rSource.m_Hour;
  pDestination[9] = rSource.m_Minutes;
  pDestination[10] = rSource.m_Seconds;
}

void
PSValInterp::Store (const DBSInt8 &rSource, D_UINT8 *pDestination)
{
  _RC(D_INT8 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSInt16 &rSource, D_UINT8 *pDestination)
{
  _RC(D_INT16 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSInt32 &rSource, D_UINT8 *pDestination)
{
  _RC(D_INT32 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSInt64 &rSource, D_UINT8 *pDestination)
{
  _RC(D_INT64 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSReal &rSource, D_UINT8 *pDestination)
{
  *_RC(float*, pDestination) = rSource.m_Value;
}
void
PSValInterp::Store (const DBSRichReal &rSource, D_UINT8 *pDestination)
{
  _RC(long double*, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSUInt8 &rSource, D_UINT8 *pDestination)
{
  _RC(D_UINT8 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSUInt16 &rSource, D_UINT8 *pDestination)
{
  _RC(D_UINT16 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSUInt32 &rSource, D_UINT8 *pDestination)
{
  _RC(D_UINT32 *, pDestination)[0] = rSource.m_Value;
}

void
PSValInterp::Store (const DBSUInt64 &rSource, D_UINT8 *pDestination)
{
  _RC(D_UINT64 *, pDestination)[0] = rSource.m_Value;
}


/////////////////////////////MARKER

void
PSValInterp::Retrieve (DBSBool *pOutValue, const D_UINT8 *pSource)
{
  new_bool (pOutValue, *pSource != 0);
  assert ((*pSource == 0) || (*pSource == 1));
}

void
PSValInterp::Retrieve (DBSChar *pOutValue, const D_UINT8 *pDestination)
{
  new_char (pOutValue, _RC(const D_UINT32 *, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSDate *pOutValue, const D_UINT8 *pDestination)
{

  D_INT16 year = _RC (const D_UINT16 *, pDestination)[0];
  D_UINT8 month = pDestination[2];
  D_UINT8 day = pDestination[3];

  new_date (pOutValue, year, month, day);
}

void
PSValInterp::Retrieve (DBSDateTime *pOutValue, const D_UINT8 *pDestination)
{
  D_INT16 year = _RC (const D_UINT16 *, pDestination)[0];
  D_UINT8 month = pDestination[2];
  D_UINT8 day = pDestination[3];
  D_UINT8 hours = pDestination[4];
  D_UINT8 minutes = pDestination[5];
  D_UINT8 seconds = pDestination[6];

  new_datetime (pOutValue, year, month, day, hours, minutes, seconds);
}

void
PSValInterp::Retrieve (DBSHiresTime *pOutValue, const D_UINT8 *pDestination)
{
  D_INT32 microsec = _RC (const D_UINT32 *, pDestination)[0];
  D_INT16 year = _RC (const D_UINT16 *, pDestination)[2];
  D_UINT8 month = pDestination[6];
  D_UINT8 day = pDestination[7];
  D_UINT8 hours = pDestination[8];
  D_UINT8 minutes = pDestination[9];
  D_UINT8 seconds = pDestination[10];

  new_hirestime (pOutValue, year, month, day, hours, minutes, seconds, microsec);

}

void
PSValInterp::Retrieve (DBSInt8 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, _RC(const D_INT8 *, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSInt16 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, _RC(const D_INT16 *, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSInt32 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, _RC(const D_INT32 *, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSInt64 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, _RC(const D_INT64 *, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSReal *pOutValue, const D_UINT8 *pDestination)
{
  new_real (pOutValue, _RC (const float*, pDestination)[0]);
}
void
PSValInterp::Retrieve (DBSRichReal *pOutValue, const D_UINT8 *pDestination)
{
  new_real (pOutValue, _RC (const long double*, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSUInt8 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, pDestination[0]);
}

void
PSValInterp::Retrieve (DBSUInt16 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, _RC(const D_UINT16 *, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSUInt32 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, _RC(const D_UINT32 *, pDestination)[0]);
}

void
PSValInterp::Retrieve (DBSUInt64 *pOutValue, const D_UINT8 *pDestination)
{
  new_integer (pOutValue, _RC(const D_UINT64 *, pDestination)[0]);
}

D_INT
PSValInterp::GetSize (DBS_FIELD_TYPE type, bool isArray)
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
PSValInterp::GetAlignment (DBS_FIELD_TYPE type, bool isArray)
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
    return PS_REAL_ALIGN;
  case T_RICHREAL:
    return PS_RICHREAL_ALIGN;
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
    assert (0);
    throw DBSException (NULL, _EXTRA(DBSException::FIELD_TYPE_INVALID));
  }
}

#ifdef new
#undef new
#endif

void
new_bool (DBSBool *object, bool value)
{
  object->~DBSBool ();
  new (object) DBSBool(false, value);
}

void
new_char (DBSChar *object, D_UINT32 value)
{
  object->~DBSChar ();
  new (object) DBSChar (false, value);
}

void
new_date (DBSDate *object, D_INT32 year, D_UINT8 month, D_UINT8 day)
{
  object->~DBSDate ();
  new (object) DBSDate (false, year, month, day);
}

void
new_datetime (DBSDateTime *object,
              D_INT32 year,
              D_UINT8 month,
              D_UINT8 day,
              D_UINT8 hours,
              D_UINT8 minutes,
              D_UINT8 seconds)
{
  object->~DBSDateTime ();
  new (object) DBSDateTime (false, year, month, day, hours, minutes, seconds);
}

void
new_hirestime (DBSHiresTime *object,
               D_INT32 year,
               D_UINT8 month,
               D_UINT8 day,
               D_UINT8 hours,
               D_UINT8 minutes,
               D_UINT8 seconds,
               D_UINT32 microsec)
{
  object->~DBSHiresTime ();
  new (object) DBSHiresTime (false, year, month, day,
                             hours, minutes, seconds, microsec);
}

template <class T_OBJ, class T_VAL>
static void
new_integer (T_OBJ *object, T_VAL value)
{
  object->~T_OBJ ();
  new (object) T_OBJ (false, value);
}

template <class T_OBJ, class T_REAL>
static void
new_real (T_OBJ *object, T_REAL value)
{
  object->~T_OBJ ();
  new (object) T_OBJ (false, value);
}
