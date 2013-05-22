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
#ifndef DBS_VALUES_H_
#define DBS_VALUES_H_

#include "whisper.h"

#include "dbs_types.h"
#include "dbs_real.h"
#include "dbs_exception.h"

namespace whisper {

#ifndef REAL_T
typedef DBS_REAL_T REAL_T;
#endif

#ifndef RICHREAL_T
typedef DBS_RICHREAL_T RICHREAL_T;
#endif


struct DBool
{
  DBool ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DBool (const bool value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DBool (const DBool& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DBool& operator= (const DBool& source)
  {
    _CC (bool&, mValue)  = source.mValue;
    _CC (bool&, mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DBool& source) const
  {
    if (IsNull () && (source.IsNull () == false))
      return true;

    if (source.IsNull () == false)
      {
        if ((mValue == false) && (source.mValue == true))
          return true;
      }

    return false;
  }

  bool operator== (const DBool& source) const
  {
    if (mIsNull != source.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == source.mValue);
  }

  bool operator<= (const DBool& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBool& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBool& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBool& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_BOOL;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const bool mValue;
  const bool mIsNull;
};

struct DChar
{
  DChar ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DChar (const uint32_t ch)
    : mValue (ch),
      mIsNull (false)
  {
  }

  DChar (const DChar& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DChar& operator= (const DChar& source)
  {
    _CC (uint32_t&, mValue)  = source.mValue;
    _CC (bool&,     mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DChar& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DChar& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DChar& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DChar& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DChar& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DChar& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_CHAR;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const uint32_t mValue;
  const bool     mIsNull;
};

struct DBS_SHL DDate
{
  DDate ()
    : mYear (),
      mMonth (),
      mDay (),
      mIsNull (true)
  {
  }

  DDate (const int32_t year,
         const uint8_t month,
         const uint8_t day);

  DDate (const DDate& source)
    : mYear (source.mYear),
      mMonth (source.mMonth),
      mDay (source.mDay),
      mIsNull (source.mIsNull)
  {
  }

  DDate& operator= (const DDate& source)
  {
    _CC (int16_t&, mYear)   = source.mYear;
    _CC (uint8_t&, mMonth)  = source.mMonth;
    _CC (uint8_t&, mDay)    = source.mDay;
    _CC (bool&,    mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DDate& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      {
        if (mYear < second.mYear)
          return true;

        else if (mYear == second.mYear)
          {
            if (mMonth < second.mMonth)
              return true;

            else if (mMonth == second.mMonth)
              return mDay < second.mDay;
          }
      }

    return false;
  }

  bool operator== (const DDate& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mYear == second.mYear)
            && (mMonth == second.mMonth)
            && (mDay  == second.mDay);

  }

  bool operator<= (const DDate& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DDate& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DDate& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DDate& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  DBSType() const
  {
    return T_DATE;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const int16_t mYear;
  const uint8_t mMonth;
  const uint8_t mDay;
  const bool    mIsNull;
};

struct DBS_SHL DDateTime
{
  DDateTime ()
    : mYear (),
      mMonth (),
      mDay (),
      mHour (),
      mMinutes (),
      mSeconds (),
      mIsNull (true)
  {
  }

  explicit DDateTime (const int32_t year,
                      const uint8_t mounth,
                      const uint8_t day,
                      const uint8_t hour,
                      const uint8_t minutes,
                      const uint8_t seconds);

  DDateTime (const DDateTime& source)
    : mYear (source.mYear),
      mMonth (source.mMonth),
      mDay (source.mDay),
      mHour (source.mHour),
      mMinutes (source.mMinutes),
      mSeconds (source.mSeconds),
      mIsNull (source.mIsNull)
  {
  }

  DDateTime& operator= (const DDateTime& source)
  {
    _CC (int16_t&, mYear)    = source.mYear;
    _CC (uint8_t&, mMonth)   = source.mMonth;
    _CC (uint8_t&, mDay)     = source.mDay;
    _CC (uint8_t&, mHour)    = source.mHour;
    _CC (uint8_t&, mMinutes) = source.mMinutes;
    _CC (uint8_t&, mSeconds) = source.mSeconds;
    _CC (bool&,    mIsNull)  = source.mIsNull;

    return *this;
  }

  bool operator< (const DDateTime& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      {
        if (mYear < second.mYear)
          return true;

        else if (mYear == second.mYear)
          {
            if (mMonth < second.mMonth)
              return true;

            else if (mMonth == second.mMonth)
              {
                if (mDay < second.mDay)
                  return true;

                else if (mDay == second.mDay)
                  {
                    if (mHour < second.mHour)
                      return true;

                    else if (mHour == second.mHour)
                      {
                        if (mMinutes < second.mMinutes)
                          return true;

                        else if (mMinutes == second.mMinutes)
                          return mSeconds < second.mSeconds;
                      }
                  }
              }
          }
      }

    return false;
  }

  bool operator== (const DDateTime& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mYear == second.mYear)
            && (mMonth == second.mMonth)
            && (mDay  == second.mDay)
            && (mHour == second.mHour)
            && (mMinutes == second.mMinutes)
            && (mSeconds == second.mSeconds);
  }

  bool operator<= (const DDateTime& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DDateTime& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DDateTime& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DDateTime& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_DATETIME;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const int16_t mYear;
  const uint8_t mMonth;
  const uint8_t mDay;
  const uint8_t mHour;
  const uint8_t mMinutes;
  const uint8_t mSeconds;
  const bool    mIsNull;
};

struct DBS_SHL DHiresTime
{

  DHiresTime ()
    : mMicrosec (),
      mYear (),
      mMonth (),
      mDay (),
      mHour (),
      mMinutes (),
      mSeconds (),
      mIsNull (true)
  {
  }

  explicit DHiresTime (const int32_t year,
                       const uint8_t month,
                       const uint8_t day,
                       const uint8_t hour,
                       const uint8_t minutes,
                       const uint8_t seconds,
                       const uint32_t microsec);

  DHiresTime (const DHiresTime& source)
    : mMicrosec (source.mMicrosec),
      mYear (source.mYear),
      mMonth (source.mMonth),
      mDay (source.mDay),
      mHour (source.mHour),
      mMinutes (source.mMinutes),
      mSeconds (source.mSeconds),
      mIsNull (source.mIsNull)
  {
  }

  DHiresTime& operator= (const DHiresTime& source)
  {
    _CC (uint32_t&, mMicrosec) = source.mMicrosec;
    _CC (int16_t&,  mYear)     = source.mYear;
    _CC (uint8_t&,  mMonth)    = source.mMonth;
    _CC (uint8_t&,  mDay)      = source.mDay;
    _CC (uint8_t&,  mHour)     = source.mHour;
    _CC (uint8_t&,  mMinutes)  = source.mMinutes;
    _CC (uint8_t&,  mSeconds)  = source.mSeconds;
    _CC (bool&,     mIsNull)   = source.mIsNull;

    return *this;
  }

  bool operator< (const DHiresTime& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      {
        if (mYear < second.mYear)
          return true;

        else if (mYear == second.mYear)
          {
            if (mMonth < second.mMonth)
              return true;

            else if (mMonth == second.mMonth)
              {
                if (mDay < second.mDay)
                  return true;

                else if (mDay == second.mDay)
                  {
                    if (mHour < second.mHour)
                      return true;

                    else if (mHour == second.mHour)
                      {
                        if (mMinutes < second.mMinutes)
                          return true;

                        else if (mMinutes == second.mMinutes)
                          {
                            if (mSeconds < second.mSeconds)
                              return true;

                            else if (mSeconds == second.mSeconds)
                              return mMicrosec < second.mMicrosec;
                          }
                      }
                  }
              }
          }
      }

    return false;
  }

  bool operator== (const DHiresTime& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mYear == second.mYear)
            && (mMonth == second.mMonth)
            && (mDay  == second.mDay)
            && (mHour == second.mHour)
            && (mMinutes == second.mMinutes)
            && (mSeconds == second.mSeconds)
            && (mMicrosec == second.mMicrosec);
  }

  bool operator<= (const DHiresTime& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DHiresTime& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DHiresTime& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DHiresTime& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_HIRESTIME;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const uint32_t mMicrosec;
  const int16_t  mYear;
  const uint8_t  mMonth;
  const uint8_t  mDay;
  const uint8_t  mHour;
  const uint8_t  mMinutes;
  const uint8_t  mSeconds;
  const bool     mIsNull;

};

struct DUInt8
{
  DUInt8 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DUInt8 (const uint8_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DUInt8 (const DUInt8& source) :
    mValue (source.mValue),
    mIsNull (source.mIsNull)
  {
  }

  DUInt8& operator= (const DUInt8& source)
  {
    _CC (uint8_t&, mValue)   = source.mValue;
    _CC (bool&,    mIsNull)  = source.mIsNull;

    return *this;
  }

  bool operator< (const DUInt8& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DUInt8& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DUInt8& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DUInt8& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DUInt8& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DUInt8& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  DBSType() const
  {
    return T_UINT8;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const uint8_t mValue;
  const bool    mIsNull;
};

struct DUInt16
{
  DUInt16 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DUInt16 (const uint16_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DUInt16 (const DUInt16& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DUInt16& operator= (const DUInt16& source)
  {
    _CC (uint16_t&, mValue)  = source.mValue;
    _CC (bool&,     mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DUInt16& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DUInt16& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DUInt16& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DUInt16& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DUInt16& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DUInt16& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  DBSType() const
  {
    return T_UINT16;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const uint16_t mValue;
  const bool     mIsNull;
};

struct DUInt32
{
  DUInt32 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DUInt32 (const uint32_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DUInt32 (const DUInt32& source) :
    mValue (source.mValue),
    mIsNull (source.mIsNull)
  {}

  DUInt32& operator= (const DUInt32& source)
  {
    _CC (uint32_t&, mValue)  = source.mValue;
    _CC (bool&,     mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DUInt32& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DUInt32& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DUInt32& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DUInt32& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DUInt32& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DUInt32& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_UINT32;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const uint32_t mValue;
  const bool     mIsNull;
};

struct DUInt64
{
  DUInt64 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DUInt64 (const uint64_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DUInt64 (const DUInt64& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DUInt64& operator= (const DUInt64& source)
  {
    _CC (uint64_t&, mValue)  = source.mValue;
    _CC (bool&,     mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DUInt64& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DUInt64& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DUInt64& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DUInt64& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DUInt64& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DUInt64& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_UINT64;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const uint64_t mValue;
  const bool     mIsNull;
};

struct DInt8
{
  DInt8 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DInt8 (const int8_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DInt8 (const DInt8& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DInt8& operator= (const DInt8& source)
  {
    _CC (int8_t&, mValue)   = source.mValue;
    _CC (bool&,   mIsNull)  = source.mIsNull;

    return *this;
  }

  bool operator< (const DInt8& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DInt8& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DInt8& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DInt8& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DInt8& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DInt8& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_INT8;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const int8_t mValue;
  const bool   mIsNull;
};

struct DInt16
{
  DInt16 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DInt16 (const int16_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DInt16 (const DInt16& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DInt16& operator= (const DInt16& source)
  {
    _CC (int16_t&, mValue)  = source.mValue;
    _CC (bool&,    mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DInt16& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DInt16& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DInt16& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DInt16& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DInt16& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DInt16& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_INT16;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const int16_t mValue;
  bool          mIsNull;
};

struct DInt32
{
  DInt32 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DInt32 (const int32_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DInt32 (const DInt32& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DInt32& operator= (const DInt32& source)
  {
    _CC (int32_t&, mValue)   = source.mValue;
    _CC (bool&,    mIsNull)  = source.mIsNull;

    return *this;
  }

  bool operator< (const DInt32& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DInt32& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DInt32& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DInt32& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DInt32& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DInt32& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_INT32;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const int32_t mValue;
  const bool    mIsNull;
};

struct DInt64
{
  DInt64 ()
    : mValue (),
      mIsNull (true)
  {
  }

  explicit DInt64 (const int64_t value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DInt64 (const DInt64& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DInt64& operator= (const DInt64& source)
  {
    _CC (int64_t&, mValue)  = source.mValue;
    _CC (bool&,    mIsNull) = source.mIsNull;
    return *this;
  }

  bool operator< (const DInt64& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return mValue < second.mValue;

    return false;
  }

  bool operator== (const DInt64& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DInt64& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DInt64& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DInt64& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DInt64& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_INT64;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const int64_t mValue;
  const bool    mIsNull;
};

struct DReal
{
  DReal () :
    mValue (),
    mIsNull (true)
  {
  }

  explicit DReal (const REAL_T value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DReal (const DReal& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DReal& operator= (const DReal& source)
  {
    _CC (REAL_T&, mValue)  = source.mValue;
    _CC (bool&,   mIsNull) = source.mIsNull;

    return *this;
  }

  bool operator< (const DReal& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return (mValue < second.mValue);
    return false;
  }

  bool operator== (const DReal& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DReal& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DReal& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DReal& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DReal& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_REAL;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const REAL_T mValue;
  const bool   mIsNull;
};

struct DRichReal
{
  DRichReal ()
    : mValue (),
      mIsNull (true)
  {
  }

  DRichReal (const RICHREAL_T value)
    : mValue (value),
      mIsNull (false)
  {
  }

  DRichReal (const DRichReal& source)
    : mValue (source.mValue),
      mIsNull (source.mIsNull)
  {
  }

  DRichReal& operator= (const DRichReal& source)
  {
    _CC (RICHREAL_T&, mValue)  = source.mValue;
    _CC (bool&,       mIsNull) = source.mIsNull;
    return *this;
  }

  bool operator< (const DRichReal& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return (mValue < second.mValue);
    return false;
  }

  bool operator== (const DRichReal& second) const
  {
    if (mIsNull != second.mIsNull)
      return false;

    if (mIsNull == true)
      return true;

    return (mValue == second.mValue);
  }

  bool operator<= (const DRichReal& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DRichReal& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DRichReal& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DRichReal& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE DBSType() const
  {
    return T_RICHREAL;
  }

  bool IsNull () const
  {
    return mIsNull;
  }

  const RICHREAL_T mValue;
  const bool       mIsNull;
};


class ITextStrategy;
class DBS_SHL DText
{
public:
  explicit DText (const char* text = NULL);
  explicit DText (const uint8_t* utf8Src);
  explicit DText (ITextStrategy& text);

  DText (const DText& source);
  DText& operator= (const DText& source);

  bool operator== (const DText& text) const;

  bool operator!= (const DText& text) const
  {
    return (*this == text) == false;
  }

  virtual ~DText ();

  bool IsNull () const;

  uint64_t Count() const;

  uint64_t RawSize () const;

  void RawRead (uint64_t offset, uint64_t count, uint8_t* dest) const;

  void Append (const DChar& ch);
  void Append (const DText& text);

  DChar CharAt (const uint64_t index) const;
  void  CharAt (const uint64_t index, const DChar& ch);

  void MakeMirror (DText& mirror) const;

  DBS_FIELD_TYPE DBSType() const
  {
    return T_TEXT;
  }

  operator ITextStrategy&() const
  {
    return *mText;
  }

private:
  ITextStrategy*      mText;
};

class IArrayStrategy;
class DBS_SHL DArray
{
public:

  DArray ();
  explicit DArray (const DBool* const array, const uint64_t count = 0);
  explicit DArray (const DChar* const array, const uint64_t count = 0);
  explicit DArray (const DDate* const array, const uint64_t count = 0);
  explicit DArray (const DDateTime* const array, const uint64_t count = 0);
  explicit DArray (const DHiresTime* const array, const uint64_t count = 0);
  explicit DArray (const DUInt8* const array, const uint64_t count = 0);
  explicit DArray (const DUInt16* const array, const uint64_t count = 0);
  explicit DArray (const DUInt32* const array, const uint64_t count = 0);
  explicit DArray (const DUInt64* const array, const uint64_t count = 0);
  explicit DArray (const DReal* const array, const uint64_t count = 0);
  explicit DArray (const DRichReal* const array, const uint64_t count = 0);
  explicit DArray (const DInt8* const array, const uint64_t count = 0);
  explicit DArray (const DInt16* const array, const uint64_t count = 0);
  explicit DArray (const DInt32* const array, const uint64_t count = 0);
  explicit DArray (const DInt64* const array, const uint64_t count = 0);

  explicit DArray (IArrayStrategy& array);

  virtual ~DArray ();

  DArray (const DArray& source);
  DArray& operator= (const DArray& source);

  bool IsNull () const
  {
    return Count() == 0;
  }

  uint64_t Count () const;

  DBS_FIELD_TYPE Type () const;

  uint64_t Add (const DBool& value);
  uint64_t Add (const DChar& value);
  uint64_t Add (const DDate& value);
  uint64_t Add (const DDateTime& value);
  uint64_t Add (const DHiresTime& value);
  uint64_t Add (const DUInt8& value);
  uint64_t Add (const DUInt16& value);
  uint64_t Add (const DUInt32& value);
  uint64_t Add (const DUInt64& value);
  uint64_t Add (const DReal& value);
  uint64_t Add (const DRichReal& value);
  uint64_t Add (const DInt8& value);
  uint64_t Add (const DInt16& value);
  uint64_t Add (const DInt32& value);
  uint64_t Add (const DInt64& value);

  void Get (const uint64_t index, DBool& outValue) const;
  void Get (const uint64_t index, DChar& outValue) const;
  void Get (const uint64_t index, DDate& outValue) const;
  void Get (const uint64_t index, DDateTime& outValue) const;
  void Get (const uint64_t index, DHiresTime& outValue) const;
  void Get (const uint64_t index, DUInt8& outValue) const;
  void Get (const uint64_t index, DUInt16& outValue) const;
  void Get (const uint64_t index, DUInt32& outValue) const;
  void Get (const uint64_t index, DUInt64& outValue) const;
  void Get (const uint64_t index, DReal& outValue) const;
  void Get (const uint64_t index, DRichReal& outValue) const;
  void Get (const uint64_t index, DInt8& outValue) const;
  void Get (const uint64_t index, DInt16& outValue) const;
  void Get (const uint64_t index, DInt32& outValue) const;
  void Get (const uint64_t index, DInt64& outValue) const;

  void Set (const uint64_t index, const DBool& value);
  void Set (const uint64_t index, const DChar& value);
  void Set (const uint64_t index, const DDate& value);
  void Set (const uint64_t index, const DDateTime& value);
  void Set (const uint64_t index, const DHiresTime& value);
  void Set (const uint64_t index, const DUInt8& value);
  void Set (const uint64_t index, const DUInt16& value);
  void Set (const uint64_t index, const DUInt32& value);
  void Set (const uint64_t index, const DUInt64& value);
  void Set (const uint64_t index, const DReal& value);
  void Set (const uint64_t index, const DRichReal& value);
  void Set (const uint64_t index, const DInt8& value);
  void Set (const uint64_t index, const DInt16& value);
  void Set (const uint64_t index, const DInt32& value);
  void Set (const uint64_t index, const DInt64& value);

  void Remove (const uint64_t index);

  void Sort (bool reverse = false);

  void MakeMirror (DArray& mirror) const;

  operator IArrayStrategy&() const
  {
    return *mArray;
  }

private:
  IArrayStrategy       *mArray;
};

} //namespace whisper

#endif /* DBS_VALUES_H_ */
