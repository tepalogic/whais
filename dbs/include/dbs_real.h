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

#ifndef DBS_REAL_H_
#define DBS_REAL_H_

#include <assert.h>
#include <limits>

#include "whais.h"

#include "utils/we_int128.h"


namespace whais {

static const int64_t DBS_REAL_PREC      = 1000000ull;         //6 decimals
static const int64_t DBS_RICHREAL_PREC  = 100000000000000ull; //14 decimals

template <typename TI, typename TF, const int64_t PRECISION>
class DBSReal
{
public:
  DBSReal ()
    : mIntPart (0),
      mFracPart (0)
  {
  }

  DBSReal (const int64_t integer,
           const int64_t fractional,
           const int64_t precision)
  {
    assert ((integer >= 0) || (fractional <= 0));
    assert ((integer <= 0) || (fractional >= 0));

    build (integer, fractional, precision);
  }

  template <typename TI_S, typename TF_S, const int64_t PREC_S>
  DBSReal (const DBSReal<TI_S, TF_S, PREC_S>& source)
  {
    build (source.Integer (), source.Fractional (), PREC_S);
  }

  template <typename T_INT>
  DBSReal (const T_INT value)
    : mIntPart (value),
      mFracPart (0)
  {
  }

  DBSReal (const long double value)
  {
    build (value);
  }

  DBSReal (const double value)
  {
    build (value);
  }

  DBSReal (const float value)
  {
    build (value);
  }

  DBSReal (const WE_I128& value)
    : mIntPart (toInt64 (value)),
      mFracPart (0)
  {
  }

  DBSReal
  operator+ (const DBSReal& op) const
  {
    DBSReal result;

    result.mIntPart  = this->mIntPart  + op.mIntPart;
    result.mFracPart = this->mFracPart + op.mFracPart;

    if (result.mFracPart < 0)
      {
        result.mFracPart  = -result.mFracPart;

        result.mIntPart  -= result.mFracPart / PRECISION;
        result.mFracPart %= PRECISION;

        result.mFracPart  = -result.mFracPart;
      }
    else
      {
        result.mIntPart  += result.mFracPart / PRECISION;
        result.mFracPart %= PRECISION;
      }

    if ((result.mIntPart < 0) && (result.mFracPart > 0))
      {
        result.mIntPart++;
        result.mFracPart = -PRECISION + result.mFracPart;
      }
    else if ((result.mIntPart > 0) && (result.mFracPart < 0))
      {
        result.mIntPart--;
        result.mFracPart = PRECISION + result.mFracPart;
      }

    assert ((result.mIntPart >= 0) || (result.mFracPart <= 0));
    assert ((result.mIntPart <= 0) || (result.mFracPart >= 0));

    return result;
  }

  DBSReal
  operator- () const
  {
    DBSReal result (*this);

    result.mIntPart  = -result.mIntPart;
    result.mFracPart = -result.mFracPart;

    assert ((result.mIntPart >= 0) || (result.mFracPart <= 0));
    assert ((result.mIntPart <= 0) || (result.mFracPart >= 0));

    return result;
  }

  DBSReal
  operator- (const DBSReal& op) const
  {
    return *this + (-op);
  }

  DBSReal
  operator* (const DBSReal& op) const
  {
    DBSReal op1 = *this;
    DBSReal op2 = op;

    bool op1neg = false;
    bool op2neg = false;

    if (op1.mIntPart < 0)
      op1neg = true, op1 = -op1;

    if (op2.mIntPart < 0)
      op2neg = true, op2 = -op2;

    WE_I128  temp;
    DBSReal  result;

    temp             = WE_I128 (op1.mIntPart) * op2.mIntPart;
    result.mIntPart  = toInt64 (temp);

    temp = WE_I128 (op1.mIntPart) * op2.mFracPart +
             WE_I128 (op2.mIntPart) * op1.mFracPart;

    result.mIntPart  += toInt64 (temp / PRECISION);
    result.mFracPart  = toInt64 (temp % PRECISION);

    temp  = WE_I128 (op1.mFracPart) * op2.mFracPart;
    temp /= PRECISION;

    result.mFracPart += toInt64 (temp);
    result.mIntPart  += result.mFracPart / PRECISION;
    result.mFracPart %= PRECISION;

    if (op1neg ^ op2neg)
      return -result;

    return result;
  }

  DBSReal
  operator/ (const DBSReal& op) const
  {
    WE_I128        op1;
    WE_I128        op2;
    bool           op1neg  = false;
    bool           op2neg  = false;

    op1 = (WE_I128 (this->mIntPart) * PRECISION) + this->mFracPart;
    op2 = (WE_I128 (op.mIntPart) * PRECISION) + op.mFracPart;

    if (op1 < 0)
      op1neg = true, op1 = -op1;

    if (op2 < 0)
      op2neg = true, op2 = -op2;

    DBSReal result (toInt64 (op1 / op2));

    op1 %= op2;
    for (int64_t i = 1; i < PRECISION; i *= 10)
      {
        result.mFracPart *= 10;
        op1 *= 10;

        assert (toInt64 (op1 / op2) < 10);

        result.mFracPart += toInt64 (op1 / op2);
        op1 %= op2;
      }

    if (op1neg ^ op2neg)
      return -result;

    return result;
  }


  bool
  operator== (const DBSReal& op) const
  {
    return (this->mIntPart == op.mIntPart)
            && (this->mFracPart == op.mFracPart);
  }

  bool
  operator!= (const DBSReal& op) const
  {
    return ! (*this == op);
  }

  bool
  operator< (const DBSReal& op) const
  {
    return (this->mIntPart < op.mIntPart)
            || ((this->mIntPart == op.mIntPart)
                && (this->mFracPart < op.mFracPart));
  }

  bool
  operator<= (const DBSReal& op) const
  {
    return (*this == op) || (*this < op);
  }

  bool
  operator> (const DBSReal* op) const
  {
    return ! (*this <= op);
  }

  bool
  operator>= (const DBSReal* op) const
  {
    return ! (*this < op);
  }

  const DBSReal&
  operator+= (const DBSReal& op)
  {
    return *this = *this + op;
  }

  const DBSReal&
  operator-= (const DBSReal& op)
  {
    return *this = *this - op;
  }

  const DBSReal&
  operator/= (const DBSReal& op)
  {
    return *this = *this / op;
  }

  const DBSReal&
  operator%= (const DBSReal& op)
  {
    return *this = *this % op;
  }

  int64_t Integer () const
  {
    return mIntPart;
  }

  int64_t
  Fractional () const
  {
    return mFracPart;
  }

  int64_t
  Precision () const
  {
    return PRECISION;
  }

private:
  void build (const int64_t interger,
              const int64_t fractional,
              const int64_t precision)
  {
    mIntPart  = interger;
    mFracPart = fractional;

    const bool fracNegative = fractional < 0;

    if (fracNegative)
      mFracPart = -mFracPart;

    if (PRECISION < precision)
      mFracPart /= (precision / PRECISION);

    else
      mFracPart *= (PRECISION / precision);

    if (fracNegative)
      mFracPart = -mFracPart;
  }

  void build (const long double value)
  {
    int64_t integer    = value;
    int64_t fractional = - ((integer - value) * PRECISION);

    build (integer, fractional, PRECISION);
  }

  TI    mIntPart;
  TF    mFracPart;
};

typedef DBSReal<int64_t, int64_t, DBS_REAL_PREC>      DBS_REAL_T;
typedef DBSReal<int64_t, int64_t, DBS_RICHREAL_PREC>  DBS_RICHREAL_T;

template<typename T>
DBS_REAL_T
operator+ (const T op1, const DBS_REAL_T& op2)
{
  return op2 + op1;
}

template<typename T>
DBS_REAL_T
operator- (const T op1, const DBS_REAL_T& op2)
{
  return DBS_REAL_T (op1) - op2;
}

template<typename T>
DBS_REAL_T
operator* (const T op1, const DBS_REAL_T& op2)
{
  return op2 * op1;
}

template<typename T>
DBS_RICHREAL_T
operator+ (const T op1, const DBS_RICHREAL_T& op2)
{
  return op2 + op1;
}

template<typename T>
DBS_RICHREAL_T
operator- (const T op1, const DBS_RICHREAL_T& op2)
{
  return DBS_RICHREAL_T (op1) - op2;
}

template<typename T>
DBS_RICHREAL_T
operator* (const T op1, const DBS_RICHREAL_T& op2)
{
  return op2 * op1;
}

static inline int64_t
toInt64 (const DBS_REAL_T& value)
{
  return value.Integer ();
}

static inline int64_t
toInt64 (const DBS_RICHREAL_T& value)
{
  return value.Integer ();
}

} //namespace whais

#endif /* DBS_REAL_H_ */
