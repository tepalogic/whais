/*
 * test_range.cpp
 *
 *  Created on: Jul 15, 2013
 *      Author: ipopa
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "utils/range.h"

using namespace whais;

typedef Interval<uint8_t> INTERVAL;
typedef Range<uint8_t>    RANGE;

#define _I(x,y) INTERVAL(x, y)

INTERVAL Rv1_f[] = { _I(5, 30) };
INTERVAL Rv1_s[] = { _I(4, 10) };
INTERVAL Rv1_r[] = { _I(4, 30) };

INTERVAL Rv2_f[] = { _I(5, 30) };
INTERVAL Rv2_s[] = { _I(2, 10) };
INTERVAL Rv2_r[] = { _I(2, 30) };

INTERVAL Rv3_f[] = { _I(5, 30) };
INTERVAL Rv3_s[] = { _I(2, 4) };
INTERVAL Rv3_r[] = { _I(2, 30) };

INTERVAL Rv4_f[] = { _I(5, 30) };
INTERVAL Rv4_s[] = { _I(2, 3) };
INTERVAL Rv4_r[] = { _I(2, 3), _I(5, 30) };

INTERVAL Rv5_f[] = { _I(15, 48), _I(50, 61) };
INTERVAL Rv5_s[] = { _I(49, 49) };
INTERVAL Rv5_r[] = { _I(15, 61) };

INTERVAL Rv6_f[] = { _I(15, 40), _I(50, 61) };
INTERVAL Rv6_s[] = { _I(41, 49) };
INTERVAL Rv6_r[] = { _I(15, 61) };

INTERVAL Rv7_f[] = { _I(1, 13) };
INTERVAL Rv7_s[] = { _I(0, 7) };
INTERVAL Rv7_r[] = { _I(0, 13) };

INTERVAL Rv8_f[] = { _I(1, 254) };
INTERVAL Rv8_s[] = { _I(7, 255) };
INTERVAL Rv8_r[] = { _I(1, 255) };

INTERVAL Rv9_f[] = { _I(50, 60) };
INTERVAL Rv9_s[] = { _I(50, 55) };
INTERVAL Rv9_r[] = { _I(50, 60) };

INTERVAL Rv10_f[] = { _I(50, 60) };
INTERVAL Rv10_s[] = { _I(55, 60) };
INTERVAL Rv10_r[] = { _I(50, 60) };

INTERVAL Rv11_f[] = { _I(1, 13) };
INTERVAL Rv11_s[] = { _I(0, 0) };
INTERVAL Rv11_r[] = { _I(0, 13) };

INTERVAL Rv12_f[] = { _I(10, 13), _I(17,66), };
INTERVAL Rv12_s[] = { _I(4, 201) };
INTERVAL Rv12_r[] = { _I(4, 201) };

INTERVAL Rv13_f[] = { _I(10, 13), _I(17,66), _I(100, 110) };
INTERVAL Rv13_s[] = { _I(4, 201) };
INTERVAL Rv13_r[] = { _I(4, 201) };

INTERVAL Rv14_f[] = { _I(10, 13), _I(17,66), _I(100, 110), _I(220, 230) };
INTERVAL Rv14_s[] = { _I(4, 201) };
INTERVAL Rv14_r[] = { _I(4, 201), _I(220, 230) };

INTERVAL Rv15_f[] = { _I(3, 5), _I(10, 13), _I(17,66), _I(100, 110), _I(220, 230) };
INTERVAL Rv15_s[] = { _I(4, 201) };
INTERVAL Rv15_r[] = { _I(3, 201), _I(220, 230) };

INTERVAL Rv16_f[] = { _I(3, 5), _I(10, 13), _I(17, 66), _I(100, 110), _I(220, 230) };
INTERVAL Rv16_s[] = { _I(7, 201) };
INTERVAL Rv16_r[] = { _I(3, 5), _I(7, 201), _I(220, 230) };

INTERVAL Rv17_f[] = { _I(3, 5), _I(10, 13), _I(17,66), _I(100, 110), _I(220, 230) };
INTERVAL Rv17_s[] = { _I(0, 1), _I(5, 7), _I(14, 16), _I(77, 99), _I(112, 115), _I(255, 255)};
INTERVAL Rv17_r[] = { _I(0, 1), _I(3, 7), _I(10, 66), _I(77, 110),_I(112, 115), _I(220, 230), _I(255, 255) };


#if !defined(ARCH_WINDOWS_VC)
INTERVAL Iv1_f[] = { _I(10, 33) };
INTERVAL Iv1_s[] = { _I(2, 7) };
INTERVAL Iv1_r[0];

INTERVAL Iv2_f[] = { _I(10, 33) };
INTERVAL Iv2_s[] = { _I(2, 9) };
INTERVAL Iv2_r[0];
#endif


INTERVAL Iv3_f[] = { _I(10, 33) };
INTERVAL Iv3_s[] = { _I(2, 10) };
INTERVAL Iv3_r[] = { _I(10, 10) };

INTERVAL Iv4_f[] = { _I(10, 33) };
INTERVAL Iv4_s[] = { _I(15, 17) };
INTERVAL Iv4_r[] = { _I(15, 17) };

INTERVAL Iv5_f[] = { _I(10, 33) };
INTERVAL Iv5_s[] = { _I(15, 15) };
INTERVAL Iv5_r[] = { _I(15, 15) };

INTERVAL Iv6_f[] = { _I(10, 33), _I(50, 100) };
INTERVAL Iv6_s[] = { _I(2, 120) };
INTERVAL Iv6_r[] = { _I(10, 33), _I(50, 100) };

INTERVAL Iv7_f[] = { _I(10, 33), _I(50, 100) };
INTERVAL Iv7_s[] = { _I(7, 43) };
INTERVAL Iv7_r[] = { _I(10, 33) };

INTERVAL Iv8_f[] = { _I(10, 33), _I(50, 100) };
INTERVAL Iv8_s[] = { _I(27, 83) };
INTERVAL Iv8_r[] = { _I(27, 33), _I(50, 83) };

#if !defined(ARCH_WINDOWS_VC)
INTERVAL Iv9_f[] = { _I(10, 33), _I(50, 100) };
INTERVAL Iv9_s[] = { _I(34, 49) };
INTERVAL Iv9_r[0];
#endif

INTERVAL Iv10_f[] = { _I(10, 33), _I(50, 100), _I(110, 121) };
INTERVAL Iv10_s[] = { _I(9, 120) };
INTERVAL Iv10_r[] = { _I(10, 33), _I(50, 100), _I(110, 120) };

#if !defined(ARCH_WINDOWS_VC)
INTERVAL Iv11_f[] = { _I(10, 33), _I(50, 100), _I(110, 121) };
INTERVAL Iv11_s[] = { _I(1, 3), _I(101, 108), _I(234, 236), _I(255, 255) };
INTERVAL Iv11_r[0];
#endif

INTERVAL Iv12_f[] = { _I(10, 33), _I(50, 100), _I(110, 121), _I(135, 189) };
INTERVAL Iv12_s[] = { _I(1, 3), _I(108, 114), _I(200, 255) };
INTERVAL Iv12_r[] = { _I(110, 114) };

INTERVAL Iv13_f[] = { _I(10, 33) };
INTERVAL Iv13_s[] = { _I(9, 10), _I(33, 78)};
INTERVAL Iv13_r[] = { _I(10, 10), _I(33, 33) };

INTERVAL Iv14_f[] = { _I(10, 33), _I(67, 89), _I(110, 121), _I(135, 189), _I(190, 195), _I(200, 210), _I(251, 252), _I(254, 255)};
INTERVAL Iv14_s[] = { _I(1, 3), _I(68, 68), _I(71, 90), _I(92, 110), _I(127, 130), _I(220, 245) };
INTERVAL Iv14_r[] = { _I(68, 68), _I(71, 89), _I(110, 110) };

INTERVAL Iv15_f[] = { _I(10, 33), _I(67, 89), _I(110, 121), _I(135, 189), _I(190, 195), _I(200, 210), _I(251, 252), _I(254, 255)};
INTERVAL Iv15_s[] = { _I(1, 3), _I(68, 138), _I(220, 245), };
INTERVAL Iv15_r[] = { _I(68, 89), _I(110, 121), _I(135, 138) };

INTERVAL Iv16_f[] = { _I(10, 33), _I(67, 89), _I(110, 121), _I(135, 189), _I(190, 195), _I(200, 210), _I(251, 252), _I(254, 255)};
INTERVAL Iv16_s[] = { _I(1, 70), _I(115, 140), _I(220, 245), };
INTERVAL Iv16_r[] = { _I(10, 33), _I(67, 70), _I(115, 121), _I(135, 140) };

INTERVAL Iv17_f[] = { _I(10, 33), _I(67, 89), _I(110, 121), _I(135, 189), _I(190, 195), _I(200, 210), _I(251, 252), _I(254, 255)};
INTERVAL Iv17_s[] = { _I(1, 70), _I(115, 140)  };
INTERVAL Iv17_r[] = { _I(10, 33), _I(67, 70), _I(115, 121), _I(135, 140) };

INTERVAL Iv18_f[] = { _I(10, 33), _I(67, 89), _I(110, 121), _I(135, 189), _I(190, 195), _I(200, 210), _I(251, 252), _I(254, 255)};
INTERVAL Iv18_s[] = { _I(115, 140)  };
INTERVAL Iv18_r[] = { _I(115, 121), _I(135, 140) };


#if !defined(ARCH_WINDOWS_VC)
INTERVAL Cv1_f[0];
INTERVAL Cv1_r[] = { _I(0, 255) };
#endif

INTERVAL Cv2_f[] = { _I(1, 255) };
INTERVAL Cv2_r[] = { _I(0, 0) };

INTERVAL Cv3_f[] = { _I(0, 254) };
INTERVAL Cv3_r[] = { _I(255, 255) };

INTERVAL Cv4_f[] = { _I(1, 254) };
INTERVAL Cv4_r[] = { _I(0, 0), _I(255, 255) };

INTERVAL Cv5_f[] = { _I(10, 120) };
INTERVAL Cv5_r[] = { _I(0, 9), _I(121, 255) };

INTERVAL Cv6_f[] = { _I(0, 120) };
INTERVAL Cv6_r[] = { _I(121, 255) };

INTERVAL Cv7_f[] = { _I(10, 255) };
INTERVAL Cv7_r[] = { _I(0, 9) };

INTERVAL Cv8_f[] = { _I(8, 45), _I(78, 91), _I(110, 116) };
INTERVAL Cv8_r[] = { _I(0, 7), _I(46, 77), _I(92, 109), _I(117, 255) };

INTERVAL Cv9_f[] = { _I(5, 10), _I(21, 88), _I(109, 125), _I(200, 255) };
INTERVAL Cv9_r[] = { _I(0, 4), _I(11, 20), _I(89, 108), _I(126, 199) };

INTERVAL Cv10_f[] = { _I(0, 7), _I(11, 56), _I(201, 203), _I(217, 255) };
INTERVAL Cv10_r[] = { _I(8, 10), _I(57, 200), _I(204, 216) };

INTERVAL Cv11_f[] = { _I(0, 7), _I(11, 56), _I(201, 203)};
INTERVAL Cv11_r[] = { _I(8, 10), _I(57, 200), _I(204, 255) };


static RANGE
range_from_intervals(const INTERVAL intervals[], size_t count)
{
  RANGE result;

  for (size_t i = 0; i < count; ++i)
    result.Join(intervals[i]);

  return result;
}

#define TO_RANGE(x) range_from_intervals(x, sizeof(x) / sizeof(x[0]))


static bool
test_reunion(const RANGE& f, const RANGE& s, const RANGE& r)
{
  RANGE op1 = f, op2 = s;
  op1.Join(op2);

  if (op1 != r)
    return false;

  op1 = f, op2 = s;

  op2.Join(op1);
  if (op2 != r)
    return false;

  RANGE all, empty;

  all.Join(INTERVAL(0, 255));

  op1.Join(all);
  if (op1 != all)
    return false;

  op1.Join(op2);
  if (op1 != all)
    return false;

  op2 = f;
  op2.Join(empty);
  if (op2 != f)
    return false;

  op2 = empty;
  op2.Join(s);
  if (op2 != s)
    return false;

  return true;
}

static bool
test_match(const RANGE& f, const RANGE& s, const RANGE& r)
{
  RANGE op1 = f, op2 = s;

  op1.Match(op2);
  if (op1 != r)
    return false;

  op1 = f, op2 = s;

  op2.Match(op1);
  if (op2 != r)
    return false;

  RANGE all, empty;

  all.Join(INTERVAL(0, 255));

  op1 = f;
  op1.Match(all);
  if (op1 != f)
    return false;

  op1 = all;
  op1.Match(op2);
  if (op1 != op2)
    return false;

  op2 = f;
  op2.Match(empty);
  if (op2 != empty)
    return false;

  op2 = empty;
  op2.Match(s);
  if (op2 != empty)
    return false;

  return true;
}


static bool
test_complement(const RANGE& f, const RANGE& r)
{
  RANGE op1 = f, op2 = r;

  if (op1.Complement() != op2)
    return false;

  op1 = f; op2 = r;
  if (op2.Complement() != op1)
    return false;

  RANGE all, empty;

  all.Join(INTERVAL(0, 255));

  op1 = f; op2 = r;
  if (op1.Join(op2) != all)
    return false;

  op1 = f; op2 = r;
  if (op1.Match(op2) != empty)
    return false;

  return true;
}


static bool
test_complement_rel(const RANGE& r)
{
  RANGE op1 = r;

  RANGE all, empty;

  all.Join(INTERVAL(0, 255));

  if (op1.Complement().Complement() != r)
    return false;

  op1 = r;
  if (op1.Complement().Join(r) != all)
    return false;

  op1 = r;
  if (op1.Complement().Match(r) != empty)
    return false;

  return true;
}


int
main()
{
  bool success = true;

  std::cout << "Testing range union ... \n";

  success = success & test_reunion(TO_RANGE(Rv1_f), TO_RANGE(Rv1_s), TO_RANGE(Rv1_r));
  success = success & test_reunion(TO_RANGE(Rv2_f), TO_RANGE(Rv2_s), TO_RANGE(Rv2_r));
  success = success & test_reunion(TO_RANGE(Rv3_f), TO_RANGE(Rv3_s), TO_RANGE(Rv3_r));
  success = success & test_reunion(TO_RANGE(Rv4_f), TO_RANGE(Rv4_s), TO_RANGE(Rv4_r));
  success = success & test_reunion(TO_RANGE(Rv5_f), TO_RANGE(Rv5_s), TO_RANGE(Rv5_r));
  success = success & test_reunion(TO_RANGE(Rv6_f), TO_RANGE(Rv6_s), TO_RANGE(Rv6_r));
  success = success & test_reunion(TO_RANGE(Rv7_f), TO_RANGE(Rv7_s), TO_RANGE(Rv7_r));
  success = success & test_reunion(TO_RANGE(Rv8_f), TO_RANGE(Rv8_s), TO_RANGE(Rv8_r));
  success = success & test_reunion(TO_RANGE(Rv9_f), TO_RANGE(Rv9_s), TO_RANGE(Rv9_r));
  success = success & test_reunion(TO_RANGE(Rv10_f), TO_RANGE(Rv10_s), TO_RANGE(Rv10_r));
  success = success & test_reunion(TO_RANGE(Rv11_f), TO_RANGE(Rv11_s), TO_RANGE(Rv11_r));
  success = success & test_reunion(TO_RANGE(Rv12_f), TO_RANGE(Rv12_s), TO_RANGE(Rv12_r));
  success = success & test_reunion(TO_RANGE(Rv13_f), TO_RANGE(Rv13_s), TO_RANGE(Rv13_r));
  success = success & test_reunion(TO_RANGE(Rv14_f), TO_RANGE(Rv14_s), TO_RANGE(Rv14_r));
  success = success & test_reunion(TO_RANGE(Rv15_f), TO_RANGE(Rv15_s), TO_RANGE(Rv15_r));
  success = success & test_reunion(TO_RANGE(Rv16_f), TO_RANGE(Rv16_s), TO_RANGE(Rv16_r));
  success = success & test_reunion(TO_RANGE(Rv17_f), TO_RANGE(Rv17_s), TO_RANGE(Rv17_r));

  if (!success)
    goto test_fail;

  std::cout << "Testing range match ... \n";

#if !defined(ARCH_WINDOWS_VC)
  success = success & test_match(TO_RANGE(Iv1_f), TO_RANGE(Iv1_s), TO_RANGE(Iv1_r));
  success = success & test_match(TO_RANGE(Iv2_f), TO_RANGE(Iv2_s), TO_RANGE(Iv2_r));
#endif
  success = success & test_match(TO_RANGE(Iv3_f), TO_RANGE(Iv3_s), TO_RANGE(Iv3_r));
  success = success & test_match(TO_RANGE(Iv4_f), TO_RANGE(Iv4_s), TO_RANGE(Iv4_r));
  success = success & test_match(TO_RANGE(Iv5_f), TO_RANGE(Iv5_s), TO_RANGE(Iv5_r));
  success = success & test_match(TO_RANGE(Iv6_f), TO_RANGE(Iv6_s), TO_RANGE(Iv6_r));
  success = success & test_match(TO_RANGE(Iv7_f), TO_RANGE(Iv7_s), TO_RANGE(Iv7_r));
  success = success & test_match(TO_RANGE(Iv8_f), TO_RANGE(Iv8_s), TO_RANGE(Iv8_r));
#if !defined(ARCH_WINDOWS_VC)
  success = success & test_match(TO_RANGE(Iv9_f), TO_RANGE(Iv9_s), TO_RANGE(Iv9_r));
#endif
  success = success & test_match(TO_RANGE(Iv10_f), TO_RANGE(Iv10_s), TO_RANGE(Iv10_r));

#if !defined(ARCH_WINDOWS_VC)
  success = success & test_match(TO_RANGE(Iv11_f), TO_RANGE(Iv11_s), TO_RANGE(Iv11_r));
#endif
  success = success & test_match(TO_RANGE(Iv12_f), TO_RANGE(Iv12_s), TO_RANGE(Iv12_r));
  success = success & test_match(TO_RANGE(Iv13_f), TO_RANGE(Iv13_s), TO_RANGE(Iv13_r));
  success = success & test_match(TO_RANGE(Iv14_f), TO_RANGE(Iv14_s), TO_RANGE(Iv14_r));
  success = success & test_match(TO_RANGE(Iv15_f), TO_RANGE(Iv15_s), TO_RANGE(Iv15_r));
  success = success & test_match(TO_RANGE(Iv16_f), TO_RANGE(Iv16_s), TO_RANGE(Iv16_r));
  success = success & test_match(TO_RANGE(Iv17_f), TO_RANGE(Iv17_s), TO_RANGE(Iv17_r));
  success = success & test_match(TO_RANGE(Iv18_f), TO_RANGE(Iv18_s), TO_RANGE(Iv18_r));

  if (!success)
    goto test_fail;

  std::cout << "Testing range complement ... \n";

#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement(TO_RANGE(Cv1_f), TO_RANGE(Cv1_r));
#endif

  success = success & test_complement(TO_RANGE(Cv2_f), TO_RANGE(Cv2_r));
  success = success & test_complement(TO_RANGE(Cv3_f), TO_RANGE(Cv3_r));
  success = success & test_complement(TO_RANGE(Cv4_f), TO_RANGE(Cv4_r));
  success = success & test_complement(TO_RANGE(Cv5_f), TO_RANGE(Cv5_r));
  success = success & test_complement(TO_RANGE(Cv6_f), TO_RANGE(Cv6_r));
  success = success & test_complement(TO_RANGE(Cv7_f), TO_RANGE(Cv7_r));
  success = success & test_complement(TO_RANGE(Cv8_f), TO_RANGE(Cv8_r));
  success = success & test_complement(TO_RANGE(Cv9_f), TO_RANGE(Cv9_r));
  success = success & test_complement(TO_RANGE(Cv10_f), TO_RANGE(Cv10_r));
  success = success & test_complement(TO_RANGE(Cv11_f), TO_RANGE(Cv11_r));

  if (!success)
    goto test_fail;

  std::cout << "Testing range complement relations ... \n";

#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv1_f));
  success = success & test_complement_rel(TO_RANGE(Iv2_f));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv3_f));
  success = success & test_complement_rel(TO_RANGE(Iv4_f));
  success = success & test_complement_rel(TO_RANGE(Iv5_f));
  success = success & test_complement_rel(TO_RANGE(Iv6_f));
  success = success & test_complement_rel(TO_RANGE(Iv7_f));
  success = success & test_complement_rel(TO_RANGE(Iv8_f));
#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv9_f));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv10_f));
#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv11_f));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv12_f));
  success = success & test_complement_rel(TO_RANGE(Iv13_f));
  success = success & test_complement_rel(TO_RANGE(Iv14_f));
  success = success & test_complement_rel(TO_RANGE(Iv15_f));
  success = success & test_complement_rel(TO_RANGE(Iv16_f));
  success = success & test_complement_rel(TO_RANGE(Iv17_f));
  success = success & test_complement_rel(TO_RANGE(Iv18_f));

#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv1_s));
  success = success & test_complement_rel(TO_RANGE(Iv2_s));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv3_s));
  success = success & test_complement_rel(TO_RANGE(Iv4_s));
  success = success & test_complement_rel(TO_RANGE(Iv5_s));
  success = success & test_complement_rel(TO_RANGE(Iv6_s));
  success = success & test_complement_rel(TO_RANGE(Iv7_s));
  success = success & test_complement_rel(TO_RANGE(Iv8_s));
#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv9_s));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv10_s));
#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv11_s));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv12_s));
  success = success & test_complement_rel(TO_RANGE(Iv13_s));
  success = success & test_complement_rel(TO_RANGE(Iv14_s));
  success = success & test_complement_rel(TO_RANGE(Iv15_s));
  success = success & test_complement_rel(TO_RANGE(Iv16_s));
  success = success & test_complement_rel(TO_RANGE(Iv17_s));
  success = success & test_complement_rel(TO_RANGE(Iv18_s));

#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv1_r));
  success = success & test_complement_rel(TO_RANGE(Iv2_r));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv3_r));
  success = success & test_complement_rel(TO_RANGE(Iv4_r));
  success = success & test_complement_rel(TO_RANGE(Iv5_r));
  success = success & test_complement_rel(TO_RANGE(Iv6_r));
  success = success & test_complement_rel(TO_RANGE(Iv7_r));
  success = success & test_complement_rel(TO_RANGE(Iv8_r));
#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv9_r));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv10_r));
#if !defined(ARCH_WINDOWS_VC)
  success = success & test_complement_rel(TO_RANGE(Iv11_r));
#endif
  success = success & test_complement_rel(TO_RANGE(Iv12_r));
  success = success & test_complement_rel(TO_RANGE(Iv13_r));
  success = success & test_complement_rel(TO_RANGE(Iv14_r));
  success = success & test_complement_rel(TO_RANGE(Iv15_r));
  success = success & test_complement_rel(TO_RANGE(Iv16_r));
  success = success & test_complement_rel(TO_RANGE(Iv17_r));
  success = success & test_complement_rel(TO_RANGE(Iv18_r));



  success = success & test_complement_rel(TO_RANGE(Rv1_f));
  success = success & test_complement_rel(TO_RANGE(Rv2_f));
  success = success & test_complement_rel(TO_RANGE(Rv3_f));
  success = success & test_complement_rel(TO_RANGE(Rv4_f));
  success = success & test_complement_rel(TO_RANGE(Rv5_f));
  success = success & test_complement_rel(TO_RANGE(Rv6_f));
  success = success & test_complement_rel(TO_RANGE(Rv7_f));
  success = success & test_complement_rel(TO_RANGE(Rv8_f));
  success = success & test_complement_rel(TO_RANGE(Rv9_f));
  success = success & test_complement_rel(TO_RANGE(Rv10_f));
  success = success & test_complement_rel(TO_RANGE(Rv11_f));
  success = success & test_complement_rel(TO_RANGE(Rv12_f));
  success = success & test_complement_rel(TO_RANGE(Rv13_f));
  success = success & test_complement_rel(TO_RANGE(Rv14_f));
  success = success & test_complement_rel(TO_RANGE(Rv15_f));
  success = success & test_complement_rel(TO_RANGE(Rv16_f));
  success = success & test_complement_rel(TO_RANGE(Rv17_f));

  success = success & test_complement_rel(TO_RANGE(Rv1_s));
  success = success & test_complement_rel(TO_RANGE(Rv2_s));
  success = success & test_complement_rel(TO_RANGE(Rv3_s));
  success = success & test_complement_rel(TO_RANGE(Rv4_s));
  success = success & test_complement_rel(TO_RANGE(Rv5_s));
  success = success & test_complement_rel(TO_RANGE(Rv6_s));
  success = success & test_complement_rel(TO_RANGE(Rv7_s));
  success = success & test_complement_rel(TO_RANGE(Rv8_s));
  success = success & test_complement_rel(TO_RANGE(Rv9_s));
  success = success & test_complement_rel(TO_RANGE(Rv10_s));
  success = success & test_complement_rel(TO_RANGE(Rv11_s));
  success = success & test_complement_rel(TO_RANGE(Rv12_s));
  success = success & test_complement_rel(TO_RANGE(Rv13_s));
  success = success & test_complement_rel(TO_RANGE(Rv14_s));
  success = success & test_complement_rel(TO_RANGE(Rv15_s));
  success = success & test_complement_rel(TO_RANGE(Rv16_s));
  success = success & test_complement_rel(TO_RANGE(Rv17_s));

  success = success & test_complement_rel(TO_RANGE(Rv1_r));
  success = success & test_complement_rel(TO_RANGE(Rv2_r));
  success = success & test_complement_rel(TO_RANGE(Rv3_r));
  success = success & test_complement_rel(TO_RANGE(Rv4_r));
  success = success & test_complement_rel(TO_RANGE(Rv5_r));
  success = success & test_complement_rel(TO_RANGE(Rv6_r));
  success = success & test_complement_rel(TO_RANGE(Rv7_r));
  success = success & test_complement_rel(TO_RANGE(Rv8_r));
  success = success & test_complement_rel(TO_RANGE(Rv9_r));
  success = success & test_complement_rel(TO_RANGE(Rv10_r));
  success = success & test_complement_rel(TO_RANGE(Rv11_r));
  success = success & test_complement_rel(TO_RANGE(Rv12_r));
  success = success & test_complement_rel(TO_RANGE(Rv13_r));
  success = success & test_complement_rel(TO_RANGE(Rv14_r));
  success = success & test_complement_rel(TO_RANGE(Rv15_r));
  success = success & test_complement_rel(TO_RANGE(Rv16_r));
  success = success & test_complement_rel(TO_RANGE(Rv17_r));


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

