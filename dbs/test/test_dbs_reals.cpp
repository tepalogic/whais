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
//#define D_INT128 int128_t
#undef D_UINT128

#include "dbs_real.h"
#include "utils/include/we_int128.h"
#include "utils/include/random.h"
#include "custom/include/test/test_fmw.h"

using namespace std;

D_UINT64 _iterationsCount = 5000000;

template <typename TR, const D_INT64 precision>
static WE_I128
from_real (TR value)
{
  WE_I128 result = toInt64 (value);

  value  -= toInt64 (value);
  result *= precision;
  result += toInt64 (value * precision);

  return result;
}

template <typename TR, const D_INT64 precision>
static TR
to_real (WE_I128 value)
{
  bool resneg = false;

  if (value < 0)
    resneg = true, value = -value;

  TR result (toInt64 (value / precision),
             toInt64 (value % precision),
             precision);

  if (resneg)
    return -result;

  return result;
}

template <typename TR, const D_INT64 precision>
bool test_addition_real (const D_CHAR* type)
{
  D_INT64 i, j;

  cout << "Testing addition of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd ();
      j = w_rnd ();

      const WE_I128 op1 (i);
      const WE_I128 op2 (j);

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

template <typename TR, const D_INT64 precision>
bool test_subtraction_real (const D_CHAR* type)
{
  D_INT64 i, j;

  cout << "Testing subtraction of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd ();
      j = w_rnd ();

      const WE_I128 op1 (i);
      const WE_I128 op2 (j);

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

template <typename TR, const D_INT64 precision, const D_INT scale>
bool test_multiplication_real (const D_CHAR* type)
{
  D_INT64 i, j;

  cout << "Testing multiplication of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd () / scale;
      j = w_rnd () / scale;

      const WE_I128 op1 (i);
      const WE_I128 op2 (j);

      const TR first  = to_real<TR, precision> (op1);
      const TR second = to_real<TR, precision> (op2);

      if ((first * second != second * first)
          || (first * second != to_real<TR, precision> ((op2 * op1)/precision)))
        {
          goto test_multiplication_real_fail;
        }

      if (((first * 0) != (0 * first))
          || ((first * 0) != TR (0))
          || ((second * 0) != (0 * second))
          || ((second * 0) != TR (0)))
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


template <typename TR, const D_INT64 precision, const D_INT scale, const D_INT64 MASK>
bool test_division_real (const D_CHAR* type)
{
  D_INT64 i, j;

  cout << "Testing division of " << type <<" values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd () & MASK;
      j = w_rnd () & MASK;

      const WE_I128 op1 (i);
      const WE_I128 op2 (j);

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


int
main (int argc, char** argv)
{
  if (argc > 1)
    _iterationsCount = atol (argv[1]);

  bool success = true;

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
D_UINT32 WMemoryTracker::sm_InitCount = 0;
const D_CHAR* WMemoryTracker::sm_Module = "T";
#endif




