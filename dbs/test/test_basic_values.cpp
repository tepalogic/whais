/*
 * test_basic_values.cpp
 *
 *  Created on: Jul 3, 2013
 *      Author: ipopa
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "dbs/dbs_values.h"

using namespace whisper;


template<class T> bool
test_bounds_values (const T min, const T minN, const T max, const T maxP)
{
  if ((min != T::Min ())
      || (max != T::Max ())
      || (min.Prev () != T ())
      || (max.Next () != T ())
      || (T ().Next () != T ())
      || (T ().Prev () != T ()))
  {
    return false;
  }

  if ((minN != T::Min ().Next ())
      || (minN.Prev () != T::Min ())
      || (minN <= min))
    {
      return false;
    }

  if ((maxP != T::Max ().Prev ())
      || (maxP.Next () != T::Max ())
      || (maxP >= max))
  {
    return false;
  }

  return true;
}

template <class T> bool
test_order_value (const T v1, const T v2, const T v3)
{
  if ((v1.Next () != v2)
      || (v2.Prev () != v1)
      || (v1.Next ().Next () != v3))
    {
      return false;
    }

  if ((v3.Prev () != v2)
      || (v2.Next () != v3)
      || (v3.Prev ().Prev () != v1))
    {
      return false;
    }

  return true;
}


int
main ()
{
  bool success = true;

  std::cout << "Testing boundaries conditions ... \n";
  success = success & test_bounds_values (DBool (false),
                                          DBool (true),
                                          DBool (true),
                                          DBool (false));
  success = success & test_bounds_values (DChar (1),
                                          DChar (2),
                                          DChar (0x10FFFF),
                                          DChar (0x10FFFE));
  success = success & test_bounds_values (DDate (-32768, 1, 1),
                                          DDate (-32768, 1, 2),
                                          DDate (32767, 12, 31),
                                          DDate (32767, 12, 30));
  success = success & test_bounds_values (
                                        DDateTime (-32768, 1, 1, 0, 0, 0),
                                        DDateTime (-32768, 1, 1, 0, 0, 1),
                                        DDateTime (32767, 12, 31, 23, 59, 59),
                                        DDateTime (32767, 12, 31, 23, 59, 58)
                                         );
  success = success & test_bounds_values (
                                DHiresTime (-32768, 1, 1, 0, 0, 0, 0),
                                DHiresTime (-32768, 1, 1, 0, 0, 0, 1),
                                DHiresTime (32767, 12, 31, 23, 59, 59, 999999),
                                DHiresTime (32767, 12, 31, 23, 59, 59, 999998)
                                         );
  success = success & test_bounds_values (DInt8 (-128),
                                          DInt8 (-127),
                                          DInt8 (127),
                                          DInt8 (126));
  success = success & test_bounds_values (DInt16 (-32768),
                                          DInt16 (-32767),
                                          DInt16 (32767),
                                          DInt16 (32766));
  success = success & test_bounds_values (DInt32 (-2147483648ll),
                                          DInt32 (-2147483647),
                                          DInt32 (2147483647),
                                          DInt32 (2147483646));
  success = success & test_bounds_values (DInt64 (-9223372036854775807ll - 1),
                                          DInt64 (-9223372036854775807ll),
                                          DInt64 (9223372036854775807ll),
                                          DInt64 (9223372036854775806ll));

  success = success & test_bounds_values (DUInt8 (0),
                                          DUInt8 (1),
                                          DUInt8 (0xFF),
                                          DUInt8 (0xFE));
  success = success & test_bounds_values (DUInt16 (0),
                                          DUInt16 (1),
                                          DUInt16 (0xFFFF),
                                          DUInt16 (0xFFFE));
  success = success & test_bounds_values (DUInt32 (0),
                                          DUInt32 (1),
                                          DUInt32 (0xFFFFFFFFu),
                                          DUInt32 (0xFFFFFFFEu));
  success = success & test_bounds_values (DUInt64 (0),
                                          DUInt64 (1),
                                          DUInt64 (0xFFFFFFFFFFFFFFFFull),
                                          DUInt64 (0xFFFFFFFFFFFFFFFEull));
  success = success & test_bounds_values (
                DReal (DBS_REAL_T (-549755813888ll, -999999, DBS_REAL_PREC)),
                DReal (DBS_REAL_T (-549755813888ll, -999998, DBS_REAL_PREC)),
                DReal (DBS_REAL_T (549755813887ll, 999999, DBS_REAL_PREC)),
                DReal (DBS_REAL_T (549755813887ll, 999998, DBS_REAL_PREC))
                                          );

  success = success & test_bounds_values (
                DRichReal (DBS_RICHREAL_T (-9223372036854775807ll - 1,
                                          -99999999999999ll,
                                          DBS_RICHREAL_PREC)),
                DRichReal (DBS_RICHREAL_T (-9223372036854775807ll - 1,
                                           -99999999999998ll,
                                           DBS_RICHREAL_PREC)),
                DRichReal (DBS_RICHREAL_T (9223372036854775807ll,
                                           99999999999999ll,
                                           DBS_RICHREAL_PREC)),
                DRichReal (DBS_RICHREAL_T (9223372036854775807ll,
                                           99999999999998ll,
                                           DBS_RICHREAL_PREC))
                                         );


  if (! success)
    goto test_fail;

  std::cout << "Testing char ordering ... \n";
  success = success & test_order_value (DChar ('a'), DChar ('b'), DChar ('c'));
  if (! success)
    goto test_fail;

  std::cout << "Testing date ordering ... \n";
  success = success & test_order_value ( DDate (2013, 7, 5),
                                         DDate (2013, 7, 6),
                                         DDate (2013, 7, 7));
  success = success & test_order_value ( DDate (2013, 2, 28),
                                         DDate (2013, 3, 1),
                                         DDate (2013, 3, 2));
  success = success & test_order_value ( DDate (2012, 2, 29),
                                         DDate (2012, 3, 1),
                                         DDate (2012, 3, 2));
  success = success & test_order_value ( DDate (2012, 1, 31),
                                         DDate (2012, 2, 1),
                                         DDate (2012, 2, 2));
  success = success & test_order_value ( DDate (-1, 12, 30),
                                         DDate (-1, 12, 31),
                                         DDate (0, 1, 1));
  if (! success)
    goto test_fail;

  std::cout << "Testing date time ordering ... \n";

  success = success & test_order_value (
                                      DDateTime (1878, 1, 6, 10, 45, 9),
                                      DDateTime (1878, 1, 6, 10, 45, 10),
                                      DDateTime (1878, 1, 6, 10, 45, 11)
                                       );
  success = success & test_order_value (
                                      DDateTime (1878, 1, 6, 10, 44, 59),
                                      DDateTime (1878, 1, 6, 10, 45, 0),
                                      DDateTime (1878, 1, 6, 10, 45, 1)
                                       );
  success = success & test_order_value (
                                      DDateTime (1878, 1, 4, 9, 59, 59),
                                      DDateTime (1878, 1, 4, 10, 0, 0),
                                      DDateTime (1878, 1, 4, 10, 0, 1)
                                       );
  success = success & test_order_value (
                                      DDateTime (1878, 1, 5, 23, 59, 59),
                                      DDateTime (1878, 1, 6, 0, 00, 0),
                                      DDateTime (1878, 1, 6, 0, 00, 1)
                                       );

  success = success & test_order_value (
                                      DDateTime (1878, 1, 31, 23, 59, 59),
                                      DDateTime (1878, 2, 1, 0, 0, 0),
                                      DDateTime (1878, 2, 1, 0, 0, 1)
                                       );
  success = success & test_order_value (
                                      DDateTime (1876, 2, 29, 23, 59, 59),
                                      DDateTime (1876, 3, 1, 0, 0, 0),
                                      DDateTime (1876, 3, 1, 0, 0, 1)
                                       );
  success = success & test_order_value (
                                      DDateTime (1877, 12, 31, 23, 59, 59),
                                      DDateTime (1878, 1, 1, 0, 0, 0),
                                      DDateTime (1878, 1, 1, 0, 0, 1)
                                       );
  success = success & test_order_value (
                                      DDateTime (-1, 12, 31, 23, 59, 59),
                                      DDateTime (0, 1, 1, 0, 0, 0),
                                      DDateTime (0, 1, 1, 0, 0, 1)
                                       );

  if (! success)
    goto test_fail;

  std::cout << "Testing hires time ordering ... \n";

  success = success & test_order_value (
                                DHiresTime (1231, 1, 31, 10, 45, 9, 998912),
                                DHiresTime (1231, 1, 31, 10, 45, 9, 998913),
                                DHiresTime (1231, 1, 31, 10, 45, 9, 998914)
                                       );
  success = success & test_order_value (
                                DHiresTime (1231, 2, 28, 10, 45, 8, 999999),
                                DHiresTime (1231, 2, 28, 10, 45, 9, 000000),
                                DHiresTime (1231, 2, 28, 10, 45, 9, 000001)
                                       );
  success = success & test_order_value (
                                DHiresTime (1231, 3, 31, 10, 25, 59, 999999),
                                DHiresTime (1231, 3, 31, 10, 26, 0, 000000),
                                DHiresTime (1231, 3, 31, 10, 26, 0, 000001)
                                       );
  success = success & test_order_value (
                                DHiresTime (1231, 3, 31, 9, 59, 59, 999999),
                                DHiresTime (1231, 3, 31, 10, 0, 0, 000000),
                                DHiresTime (1231, 3, 31, 10, 0, 0, 000001)
                                       );
  success = success & test_order_value (
                                DHiresTime (1231, 4, 29, 23, 59, 59, 999999),
                                DHiresTime (1231, 4, 30, 0, 0, 0, 000000),
                                DHiresTime (1231, 4, 30, 0, 0, 0, 000001)
                                       );
  success = success & test_order_value (
                                DHiresTime (1232, 2, 29, 23, 59, 59, 999999),
                                DHiresTime (1232, 3, 1, 0, 0, 0, 000000),
                                DHiresTime (1232, 3, 1, 0, 0, 0, 000001)
                                       );
  success = success & test_order_value (
                                DHiresTime (1230, 12, 31, 23, 59, 59, 999999),
                                DHiresTime (1231, 1, 1, 0, 0, 0, 000000),
                                DHiresTime (1231, 1, 1, 0, 0, 0, 000001)
                                       );
  success = success & test_order_value (
                                DHiresTime (-1, 12, 31, 23, 59, 59, 999999),
                                DHiresTime (0, 1, 1, 0, 0, 0, 000000),
                                DHiresTime (0, 1, 1, 0, 0, 0, 000001)
                                       );
  if (! success)
    goto test_fail;

  std::cout << "Testing signed integers ordering ... \n";

  success = success & test_order_value ( DInt8 (-1), DInt8 (0), DInt8 (1));
  success = success & test_order_value ( DInt8 (-5), DInt8 (-4), DInt8 (-3));
  success = success & test_order_value ( DInt8 (6), DInt8 (7), DInt8 (8));

  success = success & test_order_value (DInt16 (-1),
                                        DInt16 (0),
                                        DInt16 (1));
  success = success & test_order_value (DInt16 (-500),
                                        DInt16 (-499),
                                        DInt16 (-498));
  success = success & test_order_value (DInt16 (999),
                                        DInt16 (1000),
                                        DInt16 (1001));

  success = success & test_order_value (DInt32 (-1),
                                        DInt32 (0),
                                        DInt32 (1));
  success = success & test_order_value (DInt32 (-15000),
                                        DInt32 (-14999),
                                        DInt32 (-14998));
  success = success & test_order_value (DInt32 (100999),
                                        DInt32 (101000),
                                        DInt32 (101001));

  success = success & test_order_value (DInt64 (-1),
                                        DInt64 (0),
                                        DInt64 (1));
  success = success & test_order_value (DInt64 (-1500000000000ll),
                                        DInt64 (-1499999999999ll),
                                        DInt64 (-1499999999998ll));
  success = success & test_order_value (DInt64 (10005000000999ll),
                                        DInt64 (10005000001000ll),
                                        DInt64 (10005000001001ll));
  if (! success)
    goto test_fail;

  std::cout << "Testing unsigned integers ordering ... \n";

  success = success & test_order_value (DUInt8 (6), DUInt8 (7), DUInt8 (8));

  success = success & test_order_value (DUInt16 (999),
                                        DUInt16 (1000),
                                        DUInt16 (1001));

  success = success & test_order_value (DUInt32 (100999),
                                        DUInt32 (101000),
                                        DUInt32 (101001));

  success = success & test_order_value (DUInt64 (10005000000999ll),
                                        DUInt64 (10005000001000ll),
                                        DUInt64 (10005000001001ll));
  if (! success)
    goto test_fail;

  std::cout << "Testing real ordering ... \n";

  success = success & test_order_value (
                              DReal (DBS_REAL_T (0, -1, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (0, 0, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (0, 1, DBS_REAL_PREC))
                                       );
  success = success & test_order_value (
                              DReal (DBS_REAL_T (123, 889999, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (123, 890000, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (123, 890001, DBS_REAL_PREC))
                                       );
  success = success & test_order_value (
                              DReal (DBS_REAL_T (-123, -899991, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (-123, -899990, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (-123, -899989, DBS_REAL_PREC))
                                       );
  success = success & test_order_value (
                              DReal (DBS_REAL_T (300, 999999, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (301, 0, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (301, 1, DBS_REAL_PREC))
                                       );
  success = success & test_order_value (
                              DReal (DBS_REAL_T (-301, -1, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (-301, -0, DBS_REAL_PREC)),
                              DReal (DBS_REAL_T (-300, -999999, DBS_REAL_PREC))
                                       );
  if (! success)
    goto test_fail;

  std::cout << "Testing rich real ordering ... \n";

  success = success & test_order_value (
          DRichReal (DBS_RICHREAL_T (0, -1, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (0, 0, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (0, 1, DBS_RICHREAL_PREC))
                                       );
  success = success & test_order_value (
          DRichReal (DBS_RICHREAL_T (123, 12345678889999ll, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (123, 12345678890000ll, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (123, 12345678890001ll, DBS_RICHREAL_PREC))
                                       );
  success = success & test_order_value (
          DRichReal (DBS_RICHREAL_T (-123, -12345678999901ll, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (-123, -12345678999900ll, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (-123, -12345678999899ll, DBS_RICHREAL_PREC))
                                       );
  success = success & test_order_value (
          DRichReal (DBS_RICHREAL_T (300, 99999999999999ll, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (301, 0, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (301, 1, DBS_RICHREAL_PREC))
                                       );
  success = success & test_order_value (
          DRichReal (DBS_RICHREAL_T (-301, -1, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (-301, -0, DBS_RICHREAL_PREC)),
          DRichReal (DBS_RICHREAL_T (-300, -99999999999999ll, DBS_RICHREAL_PREC))
                                       );
  if (! success)
    goto test_fail;


test_fail:
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif

