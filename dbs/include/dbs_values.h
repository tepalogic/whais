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

#include "dbs_shl.h"
#include "dbs_types.h"
#include "dbs_exception.h"

typedef float       REAL_T;
typedef long double RICHREAL_T;

struct DBSBool
{
  DBSBool ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSBool (const bool value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSBool (const DBSBool& source) :
    m_Value (source.m_Value), m_IsNull (source.m_IsNull)
  {}

  DBSBool& operator= (const DBSBool& source)
  {
    _CC (bool&, m_Value) = source.m_Value;
    _CC (bool&, m_IsNull) = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSBool& source) const
  {
    if (IsNull () && (source.IsNull () == false))
      return true;

    if (source.IsNull () == false)
      if ((m_Value == false) && (source.m_Value == true))
        return true;

    return false;
  }

  bool operator== (const DBSBool& source) const
  {
    if (m_IsNull != source.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == source.m_Value);
  }

  bool operator<= (const DBSBool& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSBool& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSBool& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSBool& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE GetDBSType() const { return T_BOOL; }
  bool IsNull () const { return m_IsNull; }

  const bool m_Value;
  const bool m_IsNull;
};

struct DBSChar
{
  DBSChar ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSChar (const D_UINT32 ch)
    : m_Value (ch),
      m_IsNull (false)
  {
  }

  DBSChar (const DBSChar& source) :
    m_Value (source.m_Value), m_IsNull (source.m_IsNull)
  {
  }

  DBSChar& operator= (const DBSChar& source)
  {
    _CC (D_UINT32&, m_Value) = source.m_Value;
    _CC (bool&, m_IsNull)    = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSChar& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSChar& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSChar& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSChar& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSChar& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSChar& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_CHAR; }
  bool IsNull () const { return m_IsNull; }

  const D_UINT32 m_Value;                //4 bytes to hold an UTF-8 character
  const bool     m_IsNull;
};

struct DBS_SHL DBSDate
{
  DBSDate ()
    : m_Year (),
      m_Month (),
      m_Day (),
      m_IsNull (true)
  {
  }

  DBSDate (const D_INT32 year,
           const D_UINT8 month,
           const D_UINT8 day);

  DBSDate (const DBSDate& source)
    : m_Year (source.m_Year),
      m_Month (source.m_Month),
      m_Day (source.m_Day),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSDate& operator= (const DBSDate& source)
  {
    _CC( D_INT16&, m_Year)   = source.m_Year;
    _CC( D_UINT8&, m_Month)  = source.m_Month;
    _CC( D_UINT8&, m_Day)    = source.m_Day;
    _CC( bool&,    m_IsNull) = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSDate& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      {
        if (m_Year < second.m_Year)
          return true;
        else if (m_Year == second.m_Year)
          {
            if (m_Month < second.m_Month)
              return true;
            else if (m_Month == second.m_Month)
              return m_Day < second.m_Day;
          }
      }

    return false;
  }

  bool operator== (const DBSDate& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Year == second.m_Year) &&
           (m_Month == second.m_Month) &&
           (m_Day  == second.m_Day);

  }

  bool operator<= (const DBSDate& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSDate& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSDate& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSDate& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_DATE; }
  bool IsNull () const { return m_IsNull; }

  const D_INT16 m_Year;
  const D_UINT8 m_Month;
  const D_UINT8 m_Day;
  const bool    m_IsNull;
};

struct DBS_SHL DBSDateTime
{
  DBSDateTime ()
    : m_Year (),
      m_Month (),
      m_Day (),
      m_Hour (),
      m_Minutes (),
      m_Seconds (),
      m_IsNull (true)
  {
  }

  explicit DBSDateTime (const D_INT32 year,
                        const D_UINT8 mounth,
                        const D_UINT8 day,
                        const D_UINT8 hour,
                        const D_UINT8 minutes,
                        const D_UINT8 seconds);

  DBSDateTime (const DBSDateTime& source)
    : m_Year (source.m_Year),
      m_Month (source.m_Month),
      m_Day (source.m_Day),
      m_Hour (source.m_Hour),
      m_Minutes (source.m_Minutes),
      m_Seconds (source.m_Seconds),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSDateTime& operator= (const DBSDateTime& source)
  {
    _CC( D_INT16&, m_Year)    = source.m_Year;
    _CC( D_UINT8&, m_Month)   = source.m_Month;
    _CC( D_UINT8&, m_Day)     = source.m_Day;
    _CC( D_UINT8&, m_Hour)    = source.m_Hour;
    _CC( D_UINT8&, m_Minutes) = source.m_Minutes;
    _CC( D_UINT8&, m_Seconds) = source.m_Seconds;
    _CC( bool&,    m_IsNull)  = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSDateTime& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      {
        if (m_Year < second.m_Year)
          return true;
        else if (m_Year == second.m_Year)
          {
            if (m_Month < second.m_Month)
              return true;
            else if (m_Month == second.m_Month)
              {
                if (m_Day < second.m_Day)
                  return true;
                else if (m_Day == second.m_Day)
                  {
                    if (m_Hour < second.m_Hour)
                      return true;
                    else if (m_Hour == second.m_Hour)
                      {
                        if (m_Minutes < second.m_Minutes)
                          return true;
                        else if (m_Minutes == second.m_Minutes)
                          return m_Seconds < second.m_Seconds;
                      }
                  }
              }
          }
      }

    return false;
  }

  bool operator== (const DBSDateTime& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Year == second.m_Year) &&
           (m_Month == second.m_Month) &&
           (m_Day  == second.m_Day) &&
           (m_Hour == second.m_Hour) &&
           (m_Minutes == second.m_Minutes) &&
           (m_Seconds == second.m_Seconds);
  }

  bool operator<= (const DBSDateTime& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSDateTime& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSDateTime& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSDateTime& second) const
  {
    return (*this < second) == false;
  }


  DBS_FIELD_TYPE  GetDBSType() const { return T_DATETIME; }
  bool IsNull () const { return m_IsNull; }

  const D_INT16 m_Year;
  const D_UINT8 m_Month;
  const D_UINT8 m_Day;
  const D_UINT8 m_Hour;
  const D_UINT8 m_Minutes;
  const D_UINT8 m_Seconds;
  const bool    m_IsNull;
};

struct DBS_SHL DBSHiresTime
{

  DBSHiresTime ()
    : m_Microsec (),
      m_Year (),
      m_Month (),
      m_Day (),
      m_Hour (),
      m_Minutes (),
      m_Seconds (),
      m_IsNull (true)
  {
  }

  explicit DBSHiresTime (const D_INT32 year,
                         const D_UINT8 month,
                         const D_UINT8 day,
                         const D_UINT8 hour,
                         const D_UINT8 minutes,
                         const D_UINT8 seconds,
                         const D_UINT32 microsec);

  DBSHiresTime (const DBSHiresTime& source)
    : m_Microsec (source.m_Microsec),
      m_Year (source.m_Year),
      m_Month (source.m_Month),
      m_Day (source.m_Day),
      m_Hour (source.m_Hour),
      m_Minutes (source.m_Minutes),
      m_Seconds (source.m_Seconds),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSHiresTime& operator= (const DBSHiresTime& source)
  {
    _CC( D_UINT32&, m_Microsec) = source.m_Microsec;
    _CC( D_INT16&,  m_Year)     = source.m_Year;
    _CC( D_UINT8&,  m_Month)    = source.m_Month;
    _CC( D_UINT8&,  m_Day)      = source.m_Day;
    _CC( D_UINT8&,  m_Hour)     = source.m_Hour;
    _CC( D_UINT8&,  m_Minutes)  = source.m_Minutes;
    _CC( D_UINT8&,  m_Seconds)  = source.m_Seconds;
    _CC( bool&,     m_IsNull)   = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSHiresTime& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      {
        if (m_Year < second.m_Year)
          return true;
        else if (m_Year == second.m_Year)
          {
            if (m_Month < second.m_Month)
              return true;
            else if (m_Month == second.m_Month)
              {
                if (m_Day < second.m_Day)
                  return true;
                else if (m_Day == second.m_Day)
                  {
                    if (m_Hour < second.m_Hour)
                      return true;
                    else if (m_Hour == second.m_Hour)
                      {
                        if (m_Minutes < second.m_Minutes)
                          return true;
                        else if (m_Minutes == second.m_Minutes)
                          {
                            if (m_Seconds < second.m_Seconds)
                              return true;
                            else if (m_Seconds == second.m_Seconds)
                              return m_Microsec < second.m_Microsec;
                          }
                      }
                  }
              }
          }
      }

    return false;
  }

  bool operator== (const DBSHiresTime& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Year == second.m_Year) &&
           (m_Month == second.m_Month) &&
           (m_Day  == second.m_Day) &&
           (m_Hour == second.m_Hour) &&
           (m_Minutes == second.m_Minutes) &&
           (m_Seconds == second.m_Seconds) &&
           (m_Microsec == second.m_Microsec);
  }

  bool operator<= (const DBSHiresTime& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSHiresTime& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSHiresTime& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSHiresTime& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_HIRESTIME; }
  bool IsNull () const { return m_IsNull; }

  const D_UINT32 m_Microsec;
  const D_INT16  m_Year;
  const D_UINT8  m_Month;
  const D_UINT8  m_Day;
  const D_UINT8  m_Hour;
  const D_UINT8  m_Minutes;
  const D_UINT8  m_Seconds;
  const bool     m_IsNull;

};

struct DBSUInt8
{
  DBSUInt8 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSUInt8 (const D_UINT8 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSUInt8 (const DBSUInt8& source) :
    m_Value (source.m_Value),
    m_IsNull (source.m_IsNull)
  {
  }

  DBSUInt8& operator= (const DBSUInt8& source)
  {
    _CC( D_UINT8&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)   = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSUInt8& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSUInt8& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSUInt8& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSUInt8& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSUInt8& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSUInt8& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_UINT8; }
  bool IsNull () const { return m_IsNull; }

  const D_UINT8 m_Value;
  const bool    m_IsNull;
};

struct DBSUInt16
{
  DBSUInt16 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSUInt16 (const D_UINT16 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSUInt16 (const DBSUInt16& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSUInt16& operator= (const DBSUInt16& source)
  {
    _CC( D_UINT16&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)    = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSUInt16& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSUInt16& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSUInt16& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSUInt16& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSUInt16& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSUInt16& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_UINT16; }
  bool IsNull () const { return m_IsNull; }

  const D_UINT16 m_Value;
  const bool     m_IsNull;
};

struct DBSUInt32
{
  DBSUInt32 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSUInt32 (const D_UINT32 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSUInt32 (const DBSUInt32& source) :
    m_Value (source.m_Value),
    m_IsNull (source.m_IsNull)
  {}

  DBSUInt32& operator= (const DBSUInt32& source)
  {
    _CC( D_UINT32&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)    = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSUInt32& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSUInt32& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSUInt32& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSUInt32& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSUInt32& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSUInt32& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_UINT32; }
  bool IsNull () const { return m_IsNull; }

  const D_UINT32 m_Value;
  const bool     m_IsNull;
};

struct DBSUInt64
{
  DBSUInt64 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSUInt64 (const D_UINT64 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSUInt64 (const DBSUInt64& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSUInt64& operator= (const DBSUInt64& source)
  {
    _CC( D_UINT64&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)    = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSUInt64& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSUInt64& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSUInt64& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSUInt64& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSUInt64& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSUInt64& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_UINT64; }
  bool IsNull () const { return m_IsNull; }

  const D_UINT64 m_Value;
  const bool     m_IsNull;
};

struct DBSInt8
{
  DBSInt8 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSInt8 (const D_INT8 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSInt8 (const DBSInt8& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSInt8& operator= (const DBSInt8& source)
  {
    _CC( D_INT8&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)  = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSInt8& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSInt8& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSInt8& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSInt8& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSInt8& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSInt8& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_INT8; }
  bool IsNull () const { return m_IsNull; }

  const D_INT8 m_Value;
  const bool   m_IsNull;
};

struct DBSInt16
{
  DBSInt16 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSInt16 (const D_INT16 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSInt16 (const DBSInt16& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSInt16& operator= (const DBSInt16& source)
  {
    _CC( D_INT16&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)   = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSInt16& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSInt16& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSInt16& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSInt16& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSInt16& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSInt16& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_INT16; }
  bool IsNull () const { return m_IsNull; }

  const D_INT16 m_Value;
  bool          m_IsNull;
};

struct DBSInt32
{
  DBSInt32 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSInt32 (const D_INT32 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSInt32 (const DBSInt32& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSInt32& operator= (const DBSInt32& source)
  {
    _CC( D_INT32&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)   = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSInt32& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSInt32& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSInt32& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSInt32& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSInt32& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSInt32& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_INT32; }
  bool IsNull () const { return m_IsNull; }

  const D_INT32 m_Value;
  const bool    m_IsNull;
};

struct DBSInt64
{
  DBSInt64 ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSInt64 (const D_INT64 value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSInt64 (const DBSInt64& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSInt64& operator= (const DBSInt64& source)
  {
    _CC( D_INT64&, m_Value) = source.m_Value;
    _CC( bool&, m_IsNull)   = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSInt64& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return m_Value < second.m_Value;

    return false;
  }

  bool operator== (const DBSInt64& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSInt64& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSInt64& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSInt64& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSInt64& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_INT64; }
  bool IsNull () const { return m_IsNull; }

  const D_INT64 m_Value;
  const bool    m_IsNull;
};

struct DBSReal
{
  DBSReal () :
    m_Value (),
    m_IsNull (true)
  {
  }

  explicit DBSReal (const REAL_T value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSReal (const DBSReal& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSReal& operator= (const DBSReal& source)
  {
    _CC( REAL_T&, m_Value)  = source.m_Value;
    _CC( bool&,   m_IsNull) = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSReal& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return (m_Value < second.m_Value);
    return false;
  }

  bool operator== (const DBSReal& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSReal& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSReal& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSReal& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSReal& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_REAL; }
  bool IsNull () const { return m_IsNull; }

  const REAL_T m_Value;
  const bool   m_IsNull;
};

struct DBSRichReal
{
  DBSRichReal ()
    : m_Value (),
      m_IsNull (true)
  {
  }

  explicit DBSRichReal (const RICHREAL_T value)
    : m_Value (value),
      m_IsNull (false)
  {
  }

  DBSRichReal (const DBSRichReal& source)
    : m_Value (source.m_Value),
      m_IsNull (source.m_IsNull)
  {
  }

  DBSRichReal& operator= (const DBSRichReal& source)
  {
    _CC( RICHREAL_T&, m_Value)  = source.m_Value;
    _CC( bool&,       m_IsNull) = source.m_IsNull;
    return *this;
  }

  bool operator< (const DBSRichReal& second) const
  {
    if (IsNull () && (second.IsNull () == false))
      return true;

    if (second.IsNull () == false)
      return (m_Value < second.m_Value);
    return false;
  }

  bool operator== (const DBSRichReal& second) const
  {
    if (m_IsNull != second.m_IsNull)
      return false;

    if (m_IsNull == true)
      return true;

    return (m_Value == second.m_Value);
  }

  bool operator<= (const DBSRichReal& second) const
  {
    return (*this < second) || (*this == second);
  }

  bool operator!= (const DBSRichReal& second) const
  {
    return (*this == second) == false;
  }

  bool operator> (const DBSRichReal& second) const
  {
    return (*this <= second) == false;
  }

  bool operator>= (const DBSRichReal& second) const
  {
    return (*this < second) == false;
  }

  DBS_FIELD_TYPE  GetDBSType() const { return T_RICHREAL; }
  bool IsNull () const { return m_IsNull; }

  const RICHREAL_T m_Value;
  const bool       m_IsNull;
};


class I_TextStrategy;
class DBS_SHL DBSText
{
public:
  explicit DBSText (const D_CHAR* pText = NULL);
  explicit DBSText (const D_UINT8* pUtf8String);
  explicit DBSText (I_TextStrategy& text);

  DBSText (const DBSText& sourceText);
  DBSText& operator= (const DBSText& sourceText);
  bool operator== (const DBSText& text) const ;
  bool operator!= (const DBSText& text) const
  {
    return (*this == text) == false;
  }

  virtual ~DBSText ();

  bool IsNull () const;

  D_UINT64 GetCharactersCount() const;
  D_UINT64 GetRawUtf8Count() const;
  void     GetRawUtf8 (D_UINT64       offset,
                       D_UINT64       count,
                       D_UINT8* const pBuffer) const;

  void Append (const DBSChar& character);
  void Append (const DBSText& text);

  DBSChar GetCharAtIndex(const D_UINT64 index) const;
  void    SetCharAtIndex (const DBSChar& rCharacter, const D_UINT64 index);

  void SetMirror (DBSText& mirror) const;

  DBS_FIELD_TYPE  GetDBSType() const { return T_TEXT; }
  operator I_TextStrategy&() const { return *m_pText; }

private:
  I_TextStrategy* m_pText;
};

class I_ArrayStrategy;
class DBS_SHL DBSArray
{
public:

  DBSArray ();
  explicit DBSArray (const DBSBool* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSChar* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSDate* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSDateTime* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSHiresTime* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSUInt8* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSUInt16* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSUInt32* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSUInt64* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSReal* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSRichReal* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSInt8* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSInt16* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSInt32* array, D_UINT64 count = 0);
  explicit DBSArray (const DBSInt64* array, D_UINT64 count = 0);

  explicit DBSArray (I_ArrayStrategy& array);

  virtual ~DBSArray ();

  DBSArray (const DBSArray& rSource);
  DBSArray& operator= (const DBSArray& rSource);

  bool           IsNull () const { return ElementsCount() == 0; }
  D_UINT64       ElementsCount () const;
  DBS_FIELD_TYPE ElementsType () const;

  D_UINT64 AddElement (const DBSBool& value);
  D_UINT64 AddElement (const DBSChar& value);
  D_UINT64 AddElement (const DBSDate& value);
  D_UINT64 AddElement (const DBSDateTime& value);
  D_UINT64 AddElement (const DBSHiresTime& value);
  D_UINT64 AddElement (const DBSUInt8& value);
  D_UINT64 AddElement (const DBSUInt16& value);
  D_UINT64 AddElement (const DBSUInt32& value);
  D_UINT64 AddElement (const DBSUInt64& value);
  D_UINT64 AddElement (const DBSReal& value);
  D_UINT64 AddElement (const DBSRichReal& value);
  D_UINT64 AddElement (const DBSInt8& value);
  D_UINT64 AddElement (const DBSInt16& value);
  D_UINT64 AddElement (const DBSInt32& value);
  D_UINT64 AddElement (const DBSInt64& value);

  void GetElement (DBSBool& outValue, const D_UINT64 index) const;
  void GetElement (DBSChar& outValue, const D_UINT64 index) const;
  void GetElement (DBSDate& outValue, const D_UINT64 index) const;
  void GetElement (DBSDateTime& outValue, const D_UINT64 index) const;
  void GetElement (DBSHiresTime& outValue, const D_UINT64 index) const;
  void GetElement (DBSUInt8& outValue, const D_UINT64 index) const;
  void GetElement (DBSUInt16& outValue, const D_UINT64 index) const;
  void GetElement (DBSUInt32& outValue, const D_UINT64 index) const;
  void GetElement (DBSUInt64& outValue, const D_UINT64 index) const;
  void GetElement (DBSReal& outValue, const D_UINT64 index) const;
  void GetElement (DBSRichReal& outValue, const D_UINT64 index) const;
  void GetElement (DBSInt8& outValue, const D_UINT64 index) const;
  void GetElement (DBSInt16& outValue, const D_UINT64 index) const;
  void GetElement (DBSInt32& outValue, const D_UINT64 index) const;
  void GetElement (DBSInt64& outValue, const D_UINT64 index) const;

  void SetElement (const DBSBool& newValue, const D_UINT64 index);
  void SetElement (const DBSChar& newValue, const D_UINT64 index);
  void SetElement (const DBSDate& newValue, const D_UINT64 index);
  void SetElement (const DBSDateTime& newValue, const D_UINT64 index);
  void SetElement (const DBSHiresTime& newValue, const D_UINT64 index);
  void SetElement (const DBSUInt8& newValue, const D_UINT64 index);
  void SetElement (const DBSUInt16& newValue, const D_UINT64 index);
  void SetElement (const DBSUInt32& newValue, const D_UINT64 index);
  void SetElement (const DBSUInt64& newValue, const D_UINT64 index);
  void SetElement (const DBSReal& newValue, const D_UINT64 index);
  void SetElement (const DBSRichReal& newValue, const D_UINT64 index);
  void SetElement (const DBSInt8& newValue, const D_UINT64 index);
  void SetElement (const DBSInt16& newValue, const D_UINT64 index);
  void SetElement (const DBSInt32& newValue, const D_UINT64 index);
  void SetElement (const DBSInt64& newValue, const D_UINT64 index);

  void RemoveElement (const D_UINT64 index);
  void Sort (bool reverse = false);
  void SetMirror (DBSArray& mirror) const;

  operator I_ArrayStrategy&() const { return *m_pArray; }

private:
  I_ArrayStrategy       *m_pArray;
};

#endif /* DBS_VALUES_H_ */
