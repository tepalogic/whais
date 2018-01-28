/*
 * test_dbs_reals.cpp
 *
 *  Created on: Feb 7, 2013
 *      Author: ipopa
 */


#include <assert.h>
#include <iostream>
#include <cstring>
#include <cstdlib>


//#include <inttypes.h>
//#define int128 int128_t
#undef uint_t128

#include "dbs_real.h"
#include "dbs_values.h"
#include "utils/we_int128.h"
#include "utils/wrandom.h"
#include "custom/include/test/test_fmw.h"

using namespace std;
using namespace whais;

uint64_t _iterationsCount = 5000000;

template <typename TR, const int64_t precision>
static WE_I128
from_real(TR value)
{
  WE_I128 result = toInt64(value);

  value  -= toInt64(value);
  result *= precision;
  result += toInt64(value * precision);

  return result;
}

template <typename TR, const int64_t precision>
static TR
to_real(WE_I128 value)
{
  bool resneg = false;

  if (value < 0)
    resneg = true, value = -value;

  TR result(toInt64(value / precision),
             toInt64(value % precision),
             precision);

  if (resneg)
    return -result;

  return result;
}

template <typename TR, const int64_t precision>
bool test_addition_real(const char* type)
{
  int64_t i, j;

  cout << "Testing addition of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (uint64_t it = 0; it <= _iterationsCount; ++it)
    {
      i = wh_rnd();
      j = wh_rnd();

      const WE_I128 op1(i);
      const WE_I128 op2(j);

      const TR first  = to_real<TR, precision> (op1);
      const TR second = to_real<TR, precision> (op2);

      if ((first + second != second + first)
          || (first + second != to_real<TR, precision> (op2 + op1))
          || (from_real<TR, precision> (second + first) != (op2 + op1)))
        {
          goto test_addition_real_fail;
        }

      if (((first + 0) != (0 + first))
          || ((first + 0) != first)
          || ((second + 0) != (0 + second))
          || ((second + 0) != second))
        {
          goto test_addition_real_fail;
        }

      if (((first + 1) != (1 + first))
          || ((first + 1) != to_real<TR, precision> ((op1 + precision)))
          || ((second + 1) != (1 + second))
          || ((second + 1) != to_real<TR, precision> ((op2 + precision))))
        {
          goto test_addition_real_fail;
        }
    }


  cout << "OK\n";
  return true;

test_addition_real_fail:

  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";
  return false;
}

template <typename TR, const int64_t precision>
bool test_subtraction_real(const char* type)
{
  int64_t i, j;

  cout << "Testing subtraction of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (uint64_t it = 0; it <= _iterationsCount; ++it)
    {
      i = wh_rnd();
      j = wh_rnd();

      const WE_I128 op1(i);
      const WE_I128 op2(j);

      const TR first  = to_real<TR, precision> (op1);
      const TR second = to_real<TR, precision> (op2);

      if ((first - second != - (second - first))
          || (first - second != to_real<TR, precision> (op1 - op2)))
        {
          goto test_subtraction_real_fail;
        }

      if (((first - 0) != - (0 - first))
          || ((first - 0) != first)
          || ((first - first) != 0)
          || ((second - 0) != - (0 - second))
          || ((second - 0) != second)
          || ((second - second) != 0))
        {
          goto test_subtraction_real_fail;
        }

      if (((first - 1) != -(1 - first))
          || ((first - 1) != to_real<TR, precision> ((op1 - precision)))
          || ((first - -first) != to_real<TR, precision> ((2ull * op1)))
          || ((second - 1) != -(1 - second))
          || ((second - 1) != to_real<TR, precision> ((op2 - precision)))
          || ((second - -second) != to_real<TR, precision> ((2ull * op2))))
        {
          goto test_subtraction_real_fail;
        }
    }


  cout << "OK\n";
  return true;

test_subtraction_real_fail:

  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";
  return false;
}

template <typename TR, const int64_t precision, const int scale>
bool test_multiplication_real(const char* type)
{
  int64_t i, j;

  cout << "Testing multiplication of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (uint64_t it = 0; it <= _iterationsCount; ++it)
    {
      i = wh_rnd() / scale;
      j = wh_rnd() / scale;

      const WE_I128 op1(i);
      const WE_I128 op2(j);

      const TR first  = to_real<TR, precision> (op1);
      const TR second = to_real<TR, precision> (op2);

      if ((first * second != second * first)
          || (first * second != to_real<TR, precision> ((op2 * op1)/precision)))
        {
          goto test_multiplication_real_fail;
        }

      if (((first * 0) != (0 * first))
          || ((first * 0) != TR(0))
          || ((second * 0) != (0 * second))
          || ((second * 0) != TR(0)))
        {
          goto test_multiplication_real_fail;
        }

      if (((first * 1) != (1 * first))
          || ((first * 1) != first)
          || ((first * -1) != -first)
          || ((second * 1) != (1 * second))
          || ((second * 1) != second)
          || ((second * -1) != -second))
        {
          goto test_multiplication_real_fail;
        }
    }


  cout << "OK\n";
  return true;

test_multiplication_real_fail:

  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";
  return false;
}


template <typename TR, const int64_t precision, const int scale, const int64_t MASK>
bool test_division_real(const char* type)
{
  int64_t i, j;

  cout << "Testing division of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (uint64_t it = 0; it <= _iterationsCount; ++it)
    {
      i = wh_rnd() & MASK;
      j = wh_rnd() & MASK;

      const WE_I128 op1(i);
      const WE_I128 op2(j);

      const TR first  = to_real<TR, precision> (op1) * scale;
      const TR second = to_real<TR, precision> (op2) * scale;

      const TR mult = first * second;

      if ((first != 0)
          && ((mult / first != second)
              || ((mult / -first ) != -second)
              || ((-mult / first ) != -second)))
        {
          goto test_division_real_fail;
        }

      if ((second != 0)
          && ((mult / second != first)
              || ((mult / -second ) != -first)
              || ((-mult / second ) != -first)))
        {
          goto test_division_real_fail;
        }


      if (((first / 1) != - (first / -1))
          || ((first / 1) != first)
          || ((first != 0 ) && ((first / first) != 1))
          || ((first != 0 ) && ((first / -first) != -1))
          || ((second / 1) != -(second / -1))
          || ((second / 1) != second)
          || ((second != 0 ) && ((second / second) != 1))
          || ((second != 0 ) && ((second / -second) != -1)))
        {
          goto test_division_real_fail;
        }
    }


  cout << "OK\n";
  return true;

test_division_real_fail:

  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";
  return false;
}


bool
test_number_literal()
{
  cout << "Testing literal values for real and richreals ... ";

  DReal r1 = 1.0_wr;
  DReal r2 = -1.0_wr;
  DReal r3 = 1_wr;
  DReal r4 = -1._wr;
  DReal r5 = .1_wr;
  DReal r6 = 20.1234567_wr;
  DReal r7 = -2.012_wr;


  DRichReal rr1 = -1.0_wrr;
  DRichReal rr2 = 1.0_wrr;
  DRichReal rr3 = -1_wrr;
  DRichReal rr4 = 1._wrr;
  DRichReal rr5 = .1_wrr;
  DRichReal rr6 = -20.01_wrr;
  DRichReal rr7 = 2.012_wrr;

  if ((r1 != DBS_REAL_T(1, 0, 1))
      || (r2 != DBS_REAL_T(-1, 0, 1))
      || (r3 != DBS_REAL_T(1, 0, 1))
      || (r4 != DBS_REAL_T(-1, 0, 1))
      || (r5 != DBS_REAL_T(0, 1, 10))
      || (r6 != DBS_REAL_T(20, 12345670, 100000000))
      || (r7 != DBS_REAL_T(-2, -12, 1000)))
  {
    cout << "FAIL\n";
    return false;
  }


  if ((rr1 != DBS_RICHREAL_T(-1, 0, 1))
      || (rr2 != DBS_RICHREAL_T(1, 0, 1))
      || (rr3 != DBS_RICHREAL_T(-1, 0, 1))
      || (rr4 != DBS_RICHREAL_T(1, 0, 1))
      || (rr5 != DBS_RICHREAL_T(0, 1, 10))
      || (rr6 != DBS_RICHREAL_T(-20, -1, 100))
      || (rr7 != DBS_RICHREAL_T(2, 12, 1000)))
  {
    cout << "FAIL\n";
    return false;
  }

  cout << "OK\n";
  return true;
}

int
main(int argc, char** argv)
{
  if (argc > 1)
    _iterationsCount = atol(argv[1]);

  bool success = true;

  success = success && test_number_literal();

  success = test_addition_real <DBS_REAL_T, DBS_REAL_PREC> ("reals");
  success = test_addition_real <DBS_RICHREAL_T, DBS_RICHREAL_PREC> ("richreals");

  success = test_subtraction_real <DBS_REAL_T, DBS_REAL_PREC> ("reals");
  success = test_subtraction_real <DBS_RICHREAL_T, DBS_RICHREAL_PREC> ("richreals");

  success = test_multiplication_real <DBS_REAL_T, DBS_REAL_PREC, 10000> ("reals");
  success = test_multiplication_real <DBS_RICHREAL_T, DBS_RICHREAL_PREC, 1> ("richreals");

  success = test_division_real <DBS_REAL_T, DBS_REAL_PREC, 10000, 0xFFFF> ("reals");
  success = test_division_real <DBS_RICHREAL_T, DBS_RICHREAL_PREC, 100000000, 0xFFFFFFFF> ("richreals");

  if (!success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif




