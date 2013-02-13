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

#include "whisper.h"

#include "utils/include/we_int128.h"

static const D_INT64 DBS_REAL_PREC      = 1000000ull;
static const D_INT64 DBS_RICHREAL_PREC  = 100000000000000ull;

template <typename TI, typename TF, const D_INT64 PRECISION>
class DBSDecimalReal
{
public:
  DBSDecimalReal ()
  {
  }

  DBSDecimalReal (const D_INT64 integer,
                  const D_INT64 fractional,
                  const D_INT64 precision)
  {
    assert ((integer >= 0) || (fractional <= 0));
    assert ((integer <= 0) || (fractional >= 0));

    build (integer, fractional, precision);
  }

  template <typename TI_S, typename TF_S, const D_INT64 PREC_S>
  DBSDecimalReal (const DBSDecimalReal<TI_S, TF_S, PREC_S>& source)
  {
    build (source.Integer (), source.Fractional (), PREC_S);
  }

  template <typename T_INT>
  DBSDecimalReal (const T_INT value)
    : m_IntPart (value),
      m_FracPart (0)
  {
  }

  DBSDecimalReal (const long double value)
  {
    build (value);
  }

  DBSDecimalReal (const double value)
  {
    build (value);
  }

  DBSDecimalReal (const float value)
  {
    build (value);
  }

  DBSDecimalReal (const WE_I128& value)
    : m_IntPart (toInt64 (value)),
      m_FracPart (0)
  {
  }

  DBSDecimalReal
  operator+ (const DBSDecimalReal& op) const
  {
    DBSDecimalReal result;

    result.m_IntPart  = this->m_IntPart + op.m_IntPart;
    result.m_FracPart = this->m_FracPart + op.m_FracPart;

    if (result.m_FracPart < 0)
      {
        result.m_FracPart  = -result.m_FracPart;

        result.m_IntPart  -= result.m_FracPart / PRECISION;
        result.m_FracPart %= PRECISION;

        result.m_FracPart  = -result.m_FracPart;
      }
    else
      {
        result.m_IntPart  += result.m_FracPart / PRECISION;
        result.m_FracPart %= PRECISION;
      }

    if ((result.m_IntPart < 0) && (result.m_FracPart > 0))
      {
        result.m_IntPart++;
        result.m_FracPart = -PRECISION + result.m_FracPart;
      }
    else if ((result.m_IntPart > 0) && (result.m_FracPart < 0))
      {
        result.m_IntPart--;
        result.m_FracPart = PRECISION + result.m_FracPart;
      }

    assert ((result.m_IntPart >= 0) || (result.m_FracPart <= 0));
    assert ((result.m_IntPart <= 0) || (result.m_FracPart >= 0));

    return result;
  }

  DBSDecimalReal
  operator- () const
  {
    DBSDecimalReal result (*this);

    result.m_IntPart  = -result.m_IntPart;
    result.m_FracPart = -result.m_FracPart;

    assert ((result.m_IntPart >= 0) || (result.m_FracPart <= 0));
    assert ((result.m_IntPart <= 0) || (result.m_FracPart >= 0));

    return result;
  }

  DBSDecimalReal
  operator- (const DBSDecimalReal& op) const
  {
    return *this + (-op);
  }

  DBSDecimalReal
  operator* (const DBSDecimalReal& op) const
  {
    DBSDecimalReal op1 = *this;
    DBSDecimalReal op2 = op;

    bool op1neg = false;
    bool op2neg = false;

    if (op1.m_IntPart < 0)
      op1neg = true, op1 = -op1;

    if (op2.m_IntPart < 0)
      op2neg = true, op2 = -op2;

    WE_I128        temp;
    DBSDecimalReal result;

    temp             = WE_I128 (op1.m_IntPart) * op2.m_IntPart;
    result.m_IntPart = toInt64 (temp);

    temp = WE_I128 (op1.m_IntPart) * op2.m_FracPart +
           WE_I128 (op2.m_IntPart) * op1.m_FracPart;

    result.m_IntPart  += toInt64 (temp / PRECISION);
    result.m_FracPart  = toInt64 (temp % PRECISION);

    temp  = WE_I128 (op1.m_FracPart) * op2.m_FracPart;
    temp /= PRECISION;

    result.m_FracPart += toInt64 (temp);
    result.m_IntPart  += result.m_FracPart / PRECISION;
    result.m_FracPart %= PRECISION;

    if (op1neg ^ op2neg)
      return -result;

    return result;
  }

  DBSDecimalReal
  operator/ (const DBSDecimalReal& op) const
  {
    WE_I128        op1;
    WE_I128        op2;
    bool           op1neg  = false;
    bool           op2neg  = false;

    op1 = (WE_I128 (this->m_IntPart) * PRECISION) + this->m_FracPart;
    op2 = (WE_I128 (op.m_IntPart) * PRECISION) + op.m_FracPart;

    if (op1 < 0)
      op1neg = true, op1 = -op1;

    if (op2 < 0)
      op2neg = true, op2 = -op2;

    DBSDecimalReal result (toInt64 (op1 / op2));

    op1 %= op2;
    for (D_INT64 i = 1; i < PRECISION; i *= 10)
      {
        result.m_FracPart *= 10;
        op1 *= 10;

        assert (toInt64 (op1 / op2) < 10);
        result.m_FracPart += toInt64 (op1 / op2);
        op1 %= op2;
      }

    if (op1neg ^ op2neg)
      return -result;

    return result;
  }


  bool
  operator== (const DBSDecimalReal& op) const
  {
    return (this->m_IntPart == op.m_IntPart)
            && (this->m_FracPart == op.m_FracPart);
  }

  bool
  operator!= (const DBSDecimalReal& op) const
  {
    return ! (*this == op);
  }

  bool
  operator< (const DBSDecimalReal& op) const
  {
    return (this->m_IntPart < op.m_IntPart)
            || ((this->m_IntPart == op.m_IntPart)
                && (this->m_FracPart < op.m_FracPart));
  }

  bool
  operator<= (const DBSDecimalReal& op) const
  {
    return (*this == op) || (*this < op);
  }

  bool
  operator> (const DBSDecimalReal* op) const
  {
    return ! (*this <= op);
  }

  bool
  operator>= (const DBSDecimalReal* op) const
  {
    return ! (*this < op);
  }

  const DBSDecimalReal&
  operator+= (const DBSDecimalReal& op)
  {
    return *this = *this + op;
  }

  const DBSDecimalReal&
  operator-= (const DBSDecimalReal& op)
  {
    return *this = *this - op;
  }

  const DBSDecimalReal&
  operator/= (const DBSDecimalReal& op)
  {
    return *this = *this / op;
  }

  const DBSDecimalReal&
  operator%= (const DBSDecimalReal& op)
  {
    return *this = *this % op;
  }

  D_INT64 Integer () const
  {
    return m_IntPart;
  }

  D_INT64
  Fractional () const
  {
    return m_FracPart;
  }

  D_INT64
  Precision () const
  {
    return PRECISION;
  }

private:
  void build (const D_INT64 interger,
              const D_INT64 fractional,
              const D_INT64 precision)
  {
    m_IntPart  = interger;
    m_FracPart = fractional;

    const bool fracNegative = fractional < 0;

    if (fracNegative)
      m_FracPart = -m_FracPart;

    if (PRECISION < precision)
      m_FracPart /= (precision / PRECISION);
    else
      m_FracPart *= (PRECISION / precision);

    if (fracNegative)
      m_FracPart = -m_FracPart;
  }

  void build (const long double value)
  {
    D_INT64 integer    = value;
    D_INT64 fractional = - ((integer - value) * PRECISION);

    build (integer, fractional, PRECISION);
  }

  TI    m_IntPart;
  TF    m_FracPart;
};

typedef DBSDecimalReal<D_INT64, D_INT32, DBS_REAL_PREC>      DBS_REAL_T;
typedef DBSDecimalReal<D_INT64, D_INT64, DBS_RICHREAL_PREC>  DBS_RICHREAL_T;

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

static inline D_INT64
toInt64 (const DBS_REAL_T& value)
{
  return value.Integer ();
}

static inline D_INT64
toInt64 (const DBS_RICHREAL_T& value)
{
  return value.Integer ();
}

#endif /* DBS_REAL_H_ */
