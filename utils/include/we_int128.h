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
******************************************************************************/

#ifndef WE_INT128_H_
#define WE_INT128_H_

#include "whais.h"

#ifndef int128_t

class WE_I128
{
public:

  WE_I128 ()
  {
  }

  WE_I128 (const WE_I128& source)
  : mHi (source.mHi),
    mLo (source.mLo)
  {
  }

  template <typename T_INT>
  WE_I128 (const T_INT source)
    : mHi (0),
      mLo (source)
    {
      if (source < 0)
        {
          mLo = _SC (int64_t, source);
          mHi = ~(_SC (uint64_t, 0));
        }
    }

  WE_I128
  operator- () const
  {
    WE_I128 result (*this);

    result.mHi  = ~result.mHi;
    result.mLo  = ~result.mLo;
    result.mLo += 1;

    if (mLo == 0)
      result.mHi += 1;

    return result;
  }

  WE_I128
  operator+ (const WE_I128& op) const
    {
      WE_I128 result (*this);

      result.mHi += op.mHi;
      result.mLo += op.mLo;

      if (result.mLo < op.mLo)
        result.mHi++;

      return result;
    }

  WE_I128
  operator- (const WE_I128& op) const
    {
      WE_I128 result (*this);

      if (op.mLo > result.mLo)
        result.mHi--;

      result.mLo -= op.mLo;
      result.mHi -= op.mHi;

      return result;
    }

  WE_I128
  operator* (const WE_I128& op) const
    {
      WE_I128 tthis (*this);
      WE_I128 top (op);
      bool    tneg = false;
      bool    oneg = false;

      if (*this < 0)
        {
          tneg  = true;
          tthis = -(*this);
        }

      if (op < 0)
        {
          oneg = true;
          top  = -top;
        }

      if ((top.mHi == 0) && (top.mLo <= 0xFFFFFFFF))
        tthis = tthis.multiply32 (top.mLo);
      else
        tthis = tthis.multiply (top);

      if (tneg ^ oneg)
        return -tthis;

      return tthis;
    }

  WE_I128
  operator/ (const WE_I128& op) const
    {
      WE_I128 quotient, reminder;

      WE_I128 tthis (*this);
      WE_I128 top (op);
      bool    tneg = false;
      bool    oneg = false;

      if (*this < 0)
        {
          tneg  = true;
          tthis = - (*this);
        }

      if (op < 0)
        {
          oneg = true;
          top  = -top;
        }

      if ((top.mHi == 0)
          && ((tthis.mHi == 0)
              || (tthis.mHi >= top.mLo)
              || (top.mLo <= 0xFFFFFFFF)))
        {
          tthis.devide64 (top.mLo, quotient, reminder);
        }
      else
        tthis.devide (top, quotient, reminder);

      if (tneg ^ oneg)
        quotient = -quotient;

      return quotient;
    }

  WE_I128
  operator% (const WE_I128& op) const
    {
      WE_I128 quotient, reminder;

      WE_I128 tthis (*this);
      WE_I128 top (op);
      bool    tneg = false;

      if (*this < 0)
        {
          tneg  = true;
          tthis = - (*this);
        }

      if (op < 0)
        top  = -top;

      if ((top.mHi == 0)
          && ((tthis.mHi == 0)
              || (tthis.mHi >= top.mLo)
              || (top.mLo <= 0xFFFFFFFF)))
        {
          tthis.devide64 (top.mLo, quotient, reminder);
        }
      else
        tthis.devide (top, quotient, reminder);

      if (tneg)
        reminder = -reminder;

      return reminder;
    }

  WE_I128
  operator| (const WE_I128& op) const
    {
      WE_I128 result = *this;

      result.mHi |= op.mHi;
      result.mLo |= op.mLo;

      return result;
    }

  WE_I128
  operator& (const WE_I128& op) const
    {
      WE_I128 result = *this;

      result.mHi &= op.mHi;
      result.mLo &= op.mLo;

      return result;
    }

  bool
  operator== (const WE_I128& op) const
  {
    return (mHi == op.mHi) && (mLo == op.mLo);
  }

  bool
  operator!= (const WE_I128& op) const
  {
    return ! (*this == op);
  }

  bool
  operator<  (const WE_I128& op) const
  {
    if ((_SC (int64_t, mHi) < 0) && (_SC (int64_t, op.mHi) >= 0))
      return true;

    if ((_SC (int64_t, mHi) > 0) && (_SC (int64_t, op.mHi) <= 0))
      return false;

    return ((mHi < op.mHi)
            || ((mHi == op.mHi) && ((mLo < op.mLo))));
  }

  bool
  operator<=  (const WE_I128& op) const
  {
    return (*this == op) || (*this < op);
  }

  bool
  operator>  (const WE_I128& op) const
  {
    return ! (*this <= op);
  }

  bool
  operator>=  (const WE_I128& op) const
  {
    return ! (*this < op);
  }

  const WE_I128&
  operator+= (const WE_I128& op)
  {
    return *this = *this + op;
  }

  const WE_I128&
  operator-= (const WE_I128& op)
  {
    return *this = *this - op;
  }

  const WE_I128&
  operator*= (const WE_I128& op)
  {
    return *this = *this * op;
  }

  const WE_I128&
  operator/= (const WE_I128& op)
  {
    return *this = *this / op;
  }

  const WE_I128&
  operator%= (const WE_I128& op)
  {
    return *this = *this % op;
  }

  const WE_I128&
  operator|= (const WE_I128& op)
  {
    return *this = *this | op;
  }

  const WE_I128&
  operator&= (const WE_I128& op)
  {
    return *this = *this & op;
  }

  int64_t
  Int64 () const
  {
    return mLo;
  }

private:

  const WE_I128&
  lshift96 ()
  {
    mHi = mLo;
    mHi <<= 32;
    mLo = 0;

    return *this;
  }

  const WE_I128&
  lshift64 ()
  {
    mHi = mLo;
    mLo = 0;

    return *this;
  }

  const WE_I128&
  lshift32 ()
  {
    mHi <<= 32;
    mHi  |= (mLo >> 32) & 0xFFFFFFFF;
    mLo <<= 32;

    return *this;
  }

  const WE_I128&
  lshift ()
  {
    mHi <<= 1;
    mHi  |= (mLo >> 63) & 1;
    mLo <<= 1;

    return *this;
  }

  const WE_I128&
  rshift ()
  {
    const uint64_t one = 1;

    mLo  = (mLo >> 1) & 0x7FFFFFFFFFFFFFFFull;
    mLo |= ((mHi & 1) << 63);

    const bool negative = _SC(int64_t, mHi) < 0;
    mHi = (mHi >> 1) & 0x7FFFFFFFFFFFFFFFull;
    if (negative)
      mHi |= (one << 63);

    return *this;
  }

  WE_I128
  multiply32 (const uint32_t op) const
  {
    WE_I128 temp   = op * (mLo & 0xFFFFFFFF);
    WE_I128 result = temp;

    temp    = op * ((mLo >> 32) & 0xFFFFFFFF);
    result += temp.lshift32 ();

    temp    = op * (mHi & 0xFFFFFFFF);
    result += temp.lshift64 ();

    temp    = op * ((mHi >> 32) & 0xFFFFFFFF);
    result += temp.lshift96 ();

    return result;
  }

  WE_I128
  multiply (const WE_I128& op) const
  {
    const uint32_t tw0 = mLo & 0xFFFFFFFF;
    const uint32_t tw1 = (mLo >> 32) & 0xFFFFFFFF;
    const uint32_t tw2 = (mHi) & 0xFFFFFFFF;
    const uint32_t tw3 = (mHi >> 32) & 0xFFFFFFFF;

    const uint64_t opw0 = op.mLo & 0xFFFFFFFF;
    const uint64_t opw1 = (op.mLo >> 32) & 0xFFFFFFFF;
    const uint64_t opw2 = (op.mHi) & 0xFFFFFFFF;
    const uint64_t opw3 = (op.mHi >> 32) & 0xFFFFFFFF;


    WE_I128 temp   = opw0 * tw0;
    WE_I128 result = temp;

    temp    = opw0 * tw1;
    result += temp.lshift32 ();

    temp    = opw0 * tw2;
    result += temp.lshift64 ();

    temp   = opw0 * tw3;
    result += temp.lshift96 ();



    temp    = opw1 * tw0;
    result += temp.lshift32 ();

    temp    = opw1 * tw1;
    result += temp.lshift64 ();

    temp    = opw1 * tw2;
    result += temp.lshift96 ();



    temp    = opw2 * tw0;
    result += temp.lshift64 ();

    temp    = opw2 * tw1;
    result += temp.lshift96 ();



    temp   = opw3 * tw0;
    result += temp.lshift96 ();

    return result;
  }

  void
  devide64 (const uint64_t op, WE_I128& quotient, WE_I128& reminder) const
  {
    quotient.mHi = mHi / op;
    quotient.mLo = mLo / op;

    reminder.mHi = mHi % op;
    reminder.mLo = mLo % op;

    uint64_t sq = 0xFFFFFFFFFFFFFFFFul / op;
    uint64_t sr = (0xFFFFFFFFFFFFFFFFul % op) + 1;

    if (sr == op)
      ++sq, sr = 0;

    while (reminder.mHi > 0)
      {
        WE_I128 temp;

        temp      = reminder.mHi;
        temp     *= sq;
        quotient += temp;

        temp  = reminder.mHi;
        temp *= sr;

        temp += reminder.mLo;

        quotient.mHi += temp.mHi / op;
        quotient      += temp.mLo / op;

        reminder.mHi = temp.mHi % op;
        reminder.mLo = temp.mLo % op;
      }
  }

  void
  devide (const WE_I128& op, WE_I128& quotient, WE_I128& reminder) const
  {
    quotient = *this;
    reminder = 0;

    for (uint_t i = 0; i < 128; ++i)
      {
        reminder.lshift ();
        reminder.mLo |= (quotient.mHi >> 63) & 1;
        quotient.lshift ();

        if (reminder >= op)
          {
            quotient.mLo |= 1;
            reminder -= op;
          }
      }
  }

  uint64_t  mHi;
  uint64_t  mLo;
};


template <typename T>
WE_I128
operator+ (const T op1, const WE_I128& op2)
{
  return op2 + op1;
}


template <typename T>
WE_I128
operator- (const T op1, const WE_I128& op2)
{
  return WE_I128 (op1) - op2;
}


template <typename T>
WE_I128
operator* (const T op1, const WE_I128& op2)
{
  return op2 * op1;
}


template <typename T>
WE_I128
operator/ (const T op1, const WE_I128& op2)
{
  return WE_I128 (op1) / op2;
}


template <typename T>
WE_I128
operator% (const T op1, const WE_I128& op2)
{
  return WE_I128 (op1) % op2;
}


static inline int64_t
toInt64 (const WE_I128& value)
{
  return value.Int64 ();
}


#else

typedef int128_t WE_I128;


static inline int64_t
toInt64 (const WE_I128& value)
{
  return value;
}


#endif /* int128_t */

#endif /* WE_INT128_H_ */

