/*
 * test_int128.cpp
 *
 *  Created on: Jan 28, 2013
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


static bool
test_for_addition_64bit_values ()
{

  WE_I128 addRes, mulRes;
  D_INT64 i,j;

  cout << "Test for adds and muls of 64 bit values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd ();
      j = w_rnd ();

      addRes = WE_I128(i) + j;
      if (addRes != WE_I128(j) + i)
        goto test_64_bit_ops_fail;

      if ((addRes - i != j) || (addRes - j != i))
        goto test_64_bit_ops_fail;

      mulRes = WE_I128(i) * j;
      if (mulRes != WE_I128(j) * i)
        goto test_64_bit_ops_fail;
      if (((i != 0 ) && (mulRes / i != j))
          || ((j != 0) && (mulRes / j != i))
          || ((i != 0 ) && (mulRes % i != 0))
          || ((j != 0 ) && (mulRes % j != 0)))
        {
          goto test_64_bit_ops_fail;
        }
    }

  cout << "OK\n";

  return true;

test_64_bit_ops_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";

  return false;
}

static bool
test_for_addition_32bit_values ()
{
  WE_I128 addRes, mulRes;
  D_INT64 i,j;

  cout << "Test for adds and muls of 32 bit values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = _SC (D_INT32, w_rnd () & 0xFFFFFFFF);
      j = _SC (D_INT32, w_rnd () & 0xFFFFFFFF);

      addRes = WE_I128(i) + j;
      if (addRes != WE_I128(j) + i)
        goto test_32_bit_ops_fail;

      if ((addRes - i != j) || (addRes - j != i))
        goto test_32_bit_ops_fail;

      mulRes = WE_I128(i) * j;
      if (mulRes != WE_I128(j) * i)
        goto test_32_bit_ops_fail;

      if (((i != 0 ) && (mulRes / i != j))
          || ((j != 0) && (mulRes / j != i))
          || ((i != 0 ) && (mulRes % i != 0))
          || ((j != 0 ) && (mulRes % j != 0)))
       {
          goto test_32_bit_ops_fail;
       }
    }

  cout << "OK\n";

  return true;

test_32_bit_ops_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";

  return false;
}


static bool
test_for_addition_mix_values ()
{

  WE_I128 addRes, mulRes;
  D_INT64 i,j;

  cout << "Test for adds and muls of mix 32 and 64 bit values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd ();
      j = _SC (D_INT32, w_rnd () & 0xFFFFFFFF);

      addRes = WE_I128(i) + j;
      if (addRes != WE_I128(j) + i)
        goto test_mix_bit_ops_fail;

      if ((addRes - i != j) || (addRes - j != i))
        goto test_mix_bit_ops_fail;

      mulRes = WE_I128(i) * j;
      if (mulRes != WE_I128(j) * i)
        goto test_mix_bit_ops_fail;

      if (((i != 0 ) && (mulRes / i != j))
          || ((j != 0) && (mulRes / j != i))
          || ((i != 0 ) && (mulRes % i != 0))
          || ((j != 0 ) && (mulRes % j != 0)))
        {
          goto test_mix_bit_ops_fail;
        }
    }

  cout << "OK\n";

  return true;

test_mix_bit_ops_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";

  return false;
}

static bool
test_for_reminder_64bit_values ()
{
  WE_I128 addRes, mulRes;
  D_INT64 i,j, k;

  cout << "Test for reminders of 64 bit values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd () & 0x7FFFFFFFFFFFFFFF;
      j = w_rnd () & 0x7FFFFFFFFFFFFFFF;
      k = w_rnd () & 0x7FFFFFFFFFFFFFFF;

      k %= i;
      k %= j;

      mulRes = WE_I128(i) * j + k;
      if (mulRes != WE_I128 (k) + WE_I128(j) * i)
        goto test_64_bit_reminder_fail;

      if (((i != 0 ) && (mulRes / i != j))
          || ((j != 0) && (mulRes / j != i))
          || ((i != 0 ) && (mulRes % i != k))
          || ((j != 0 ) && (mulRes % j != k))
          || ((i != 0 ) && (mulRes / i * i + mulRes % i != mulRes))
          || ((j != 0 ) && (mulRes / j * j + mulRes % j != mulRes)))
        {
          goto test_64_bit_reminder_fail;
        }

      mulRes = WE_I128(i) * -j + k;
      if (mulRes != k + -i * WE_I128(j))
        goto test_64_bit_reminder_fail;

      if (((i != 0 ) && (mulRes / i * i + mulRes % i != mulRes))
          || ((i != 0 ) && (mulRes / -i * -i + mulRes % -i != mulRes))
          || ((j != 0 ) && (mulRes / j * j + mulRes % j != mulRes))
          || ((j != 0 ) && (mulRes / -j * -j + mulRes % -j != mulRes)))
        {
          goto test_64_bit_reminder_fail;
        }
    }

  cout << "OK\n";

  return true;

test_64_bit_reminder_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";
  cout << "k = " << k << ";\n";

  return false;
}

static bool
test_for_reminder_32bit_values ()
{
  WE_I128 addRes, mulRes;
  D_INT64 i,j, k;

  cout << "Test for reminders of 32 bit values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd () & 0x7FFFFFFF;
      j = w_rnd () & 0x7FFFFFFF;
      k = w_rnd () & 0x7FFFFFFF;

      k %= i;
      k %= j;

      i = 1756457261;
      j = 513283075;
      k = 126229318;


      mulRes = WE_I128(i) * j + k;
      if (mulRes != WE_I128 (k) + WE_I128(j) * i)
        goto test_32_bit_reminder_fail;

      if (((i != 0 ) && (mulRes / i != j))
          || ((j != 0) && (mulRes / j != i))
          || ((i != 0 ) && (mulRes % i != k))
          || ((j != 0 ) && (mulRes % j != k))
          || ((i != 0 ) && (mulRes / i * i + mulRes % i != mulRes))
          || ((j != 0 ) && (mulRes / j * j + mulRes % j != mulRes)))
        {
          goto test_32_bit_reminder_fail;
        }

      mulRes = WE_I128(i) * -j + k;
      if (mulRes != k + -i * WE_I128(j))
        goto test_32_bit_reminder_fail;

      if (((i != 0 ) && (mulRes / i * i + mulRes % i != mulRes))
          || ((i != 0 ) && (mulRes / -i * -i + mulRes % -i != mulRes))
          || ((j != 0 ) && (mulRes / j * j + mulRes % j != mulRes))
          || ((j != 0 ) && (mulRes / -j * -j + mulRes % -j != mulRes)))
        {
          goto test_32_bit_reminder_fail;
        }
    }

  cout << "OK\n";

  return true;

test_32_bit_reminder_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";
  cout << "k = " << k << ";\n";

  return false;
}

static bool
test_for_reminder_mix_bit_values ()
{
  WE_I128 addRes, mulRes;
  D_INT64 i,j, k;

  cout << "Test for reminders of mix 32 and 64 bit values for ";
  cout << _iterationsCount << " random values.... ";

  for (D_UINT64 it = 0; it <= _iterationsCount; ++it)
    {
      i = w_rnd () & 0x7FFFFFFFFFFFFFFF;
      j = w_rnd () & 0x7FFFFFFF;
      k = w_rnd () & 0x7FFFFFFFFFFFFFFF;

      k %= i;
      k %= j;

      mulRes = WE_I128(i) * j + k;
      if (mulRes != WE_I128 (k) + WE_I128(j) * i)
        goto test_mix_bit_reminder_fail;

      if (((i != 0 ) && (mulRes / i != j))
          || ((j != 0) && (mulRes / j != i))
          || ((i != 0 ) && (mulRes % i != k))
          || ((j != 0 ) && (mulRes % j != k))
          || ((i != 0 ) && (mulRes / i * i + mulRes % i != mulRes))
          || ((j != 0 ) && (mulRes / j * j + mulRes % j != mulRes)))
        {
          goto test_mix_bit_reminder_fail;
        }

      mulRes = WE_I128(i) * -j + k;
      if (mulRes != k + -i * WE_I128(j))
        goto test_mix_bit_reminder_fail;

      if (((i != 0 ) && (mulRes / i * i + mulRes % i != mulRes))
          || ((i != 0 ) && (mulRes / -i * -i + mulRes % -i != mulRes))
          || ((j != 0 ) && (mulRes / j * j + mulRes % j != mulRes))
          || ((j != 0 ) && (mulRes / -j * -j + mulRes % -j != mulRes)))
        {
          goto test_mix_bit_reminder_fail;
        }
    }

  cout << "OK\n";

  return true;

test_mix_bit_reminder_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";
  cout << "k = " << k << ";\n";

  return false;
}

static bool
test_for_special_add_cases ()
{
  WE_I128 sum;
  D_INT64 i,j;

  cout << "Testing addition of special cases... ";

  for (i = 0; i < 64; ++i)
    {
      for (j = 0; j < 64; ++j)
        {
          WE_I128 val1 = 1 << i;
          WE_I128 val2 = 1 << j;

          sum = val1 + val2;
          if (sum != val2 + val1)
            goto test_special_add_fail;

          if ((sum - val1 != val2) || (sum - val2 != val1))
            goto test_special_add_fail;

          if ((val1 - val1 != 0) || (val2 - val2 != 0))
            goto test_special_add_fail;

          if ((val1 + -val1 != 0) || (val2 + -val2 != 0))
            goto test_special_add_fail;

          val1 = val1 * 0xFFFFFFFFFFFFFFFF + val1;
          val2 = val2 * 0xFFFFFFFFFFFFFFFF + val2;

          sum = val1 + val2;
          if (sum != val2 + val1)
            goto test_special_add_fail;

          if ((sum - val1 != val2) || (sum - val2 != val1))
            goto test_special_add_fail;

          if ((0ull - val1 != -val1) || (0ull - val2 != -val2))
            goto test_special_add_fail;

          if ((1ll - val1 != -val1 + 1) || (1ll - val2 != -val2 + 1))
            goto test_special_add_fail;

          D_INT64 temp = w_rnd();

          if ((temp - val1 != -val1 + temp) || (temp - val2 != -val2 + temp))
            goto test_special_add_fail;

          if ((val1 - val1 != 0) || (val2 - val2 != 0))
            goto test_special_add_fail;

          if ((val1 + -val1 != 0) || (val2 + -val2 != 0))
            goto test_special_add_fail;
        }
    }

  cout << "OK\n";

  return true;

test_special_add_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";
  cout << "j = " << j << ";\n";

  return false;
}


static bool
test_for_special_mul_cases ()
{
  D_INT64 i;

  cout << "Testing multiplication of special cases... ";

  for (i = 0; i < 64; ++i)
    {
      WE_I128 val = 1 << i;

      if ((val * 1ll != 1ll * val)
          || (val * 1ll != val))
        {
          goto test_special_mul_fail;
        }

      if ((val * -1ll != 1ll * -val)
          || (val * -1ll != -val))
        {
          goto test_special_mul_fail;
        }

      if ((val / 1ll != val / 1ll)
          || (val / 1ll != val))
        {
          goto test_special_mul_fail;
        }

      if ((val / -1ll != -val / 1ll)
          || (val / -1ll != -val))
        {
          goto test_special_mul_fail;
        }

      if (val / val != 1ll)
        {
          goto test_special_mul_fail;
        }

      if ((val / -val != -val / val)
          || (val / -val != -1ll))
        {
          goto test_special_mul_fail;
        }
    }

  cout << "OK\n";

  return true;

test_special_mul_fail:
  cout << "FAIL\n";
  cout << "i = " << i << ";\n";

  return false;
}

int
main (int argc, char** argv)
{
  if (argc > 1)
    _iterationsCount = atol (argv[1]);

  bool success = true;

  success = test_for_addition_64bit_values ();
  success = test_for_addition_32bit_values ();
  success = test_for_addition_mix_values ();
  success = test_for_reminder_64bit_values ();
  success = test_for_reminder_32bit_values ();
  success = test_for_reminder_mix_bit_values ();
  success = test_for_special_add_cases ();
  success = test_for_special_mul_cases ();

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

