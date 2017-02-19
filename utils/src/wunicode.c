/******************************************************************************
UTILS - Common routines used trough WHAIS project
Copyright(C) 2008  Iulian Popa

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

#include <assert.h>

#include "wunicode.h"
#include "wutf.h"


static INLINE uint32_t
to_uppercase_basic_latin(const uint32_t codePoint)
{
  assert((0 <= codePoint) && (codePoint < 0x0080));

  if (('a' <= codePoint) && (codePoint <= 'z'))
    return codePoint - 'a' + 'A';

  return codePoint;
}


static INLINE uint32_t
to_lowercase_basic_latin(const uint32_t codePoint)
{
  assert((0 <= codePoint) && (codePoint < 0x0080));

  if (('A' <= codePoint) && (codePoint <= 'Z'))
    return codePoint - 'A' + 'a';

  return codePoint;
}


static INLINE uint32_t
to_uppercase_basic_latin1_supp(const uint32_t codePoint)
{
  assert((0x0080 <= codePoint) && (codePoint < 0x0100));

  if ((0x00E0 <= codePoint) && (codePoint <= 0x00F6))
    return codePoint - 0x00E0 + 0x00C0;

  if ((0x00F8 <= codePoint) && (codePoint <= 0x00FE))
    return codePoint - 0x00F8 + 0x00D8;

  if (codePoint == 0x00FF)
    return 0x178;

  return codePoint;
}


static INLINE uint32_t
to_lowercase_basic_latin1_supp(const uint32_t codePoint)
{
  assert((0x0080 <= codePoint) && (codePoint < 0x0100));

  if ((0x00C0 <= codePoint) && (codePoint <= 0x00D6))
    return codePoint - 0x00C0 + 0x00E0;

  if ((0x00D8 <= codePoint) && (codePoint <= 0x00DE))
    return codePoint - 0x00D8 + 0x00F8;

  return codePoint;
}


static INLINE uint32_t
to_uppercase_basic_latinA_ext(const uint32_t codePoint)
{
  assert((0x0100 <= codePoint) && (codePoint < 0x0180));

  if ((codePoint & 1) != 0)
    {
      if ((0x0100 <= codePoint) && (codePoint <= 0x012F))
        return codePoint - 1;

      if ((0x0132 <= codePoint) && (codePoint <= 0x0137))
        return codePoint - 1;

      if ((0x014A <= codePoint) && (codePoint <= 0x0177))
        return codePoint - 1;
    }
  else
    {
      if ((0x0139 <= codePoint) && (codePoint <= 0x0148))
        return codePoint - 1;

      if ((0x0179 <= codePoint) && (codePoint <= 0x017E))
        return codePoint - 1;
    }

  return codePoint;
}


static INLINE uint32_t
to_lowercase_basic_latinA_ext(const uint32_t codePoint)
{
  assert((0x0100 <= codePoint) && (codePoint < 0x0180));

  if ((codePoint & 1) == 0)
    {
      if ((0x0100 <= codePoint) && (codePoint <= 0x012F))
        return codePoint + 1;

      if ((0x0132 <= codePoint) && (codePoint <= 0x0137))
        return codePoint + 1;

      if ((0x014A <= codePoint) && (codePoint <= 0x0177))
        return codePoint + 1;
    }
  else
    {
      if ((0x0139 <= codePoint) && (codePoint <= 0x0148))
        return codePoint + 1;

      if ((0x0179 <= codePoint) && (codePoint <= 0x017E))
        return codePoint + 1;
    }

  if (codePoint == 0x0178)
    return 0x00FF;

  return codePoint;
}


static INLINE uint32_t
to_uppercase_basic_latinB_ext(const uint32_t codePoint)
{
  assert((0x0180 <= codePoint) && (codePoint < 0x0250));

  if ((codePoint & 1) == 0)
    {
      if ((0x01CD <= codePoint) && (codePoint <= 0x01DC))
        return codePoint - 1;
    }
  else
    {
      if ((0x01DE <= codePoint) && (codePoint <= 0x01EF))
        return codePoint - 1;

      if ((0x01F8 <= codePoint) && (codePoint <= 0x021F))
        return codePoint - 1;

      if ((0x0222 <= codePoint) && (codePoint <= 0x0233))
        return codePoint - 1;

      if ((0x0246 <= codePoint) && (codePoint <= 0x024F))
        return codePoint - 1;
    }

  switch(codePoint)
    {
    case 0x0180:
      return 0x0243;

    case 0x019A:
      return 0x023D;

    case 0x019E:
      return 0x0220;

    case 0x01BF:
      return 0x01F7;

    case 0x0183:
    case 0x0185:
    case 0x0188:
    case 0x018C:
    case 0x0192:
    case 0x0199:
    case 0x01A1:
    case 0x01A3:
    case 0x01A5:
    case 0x01A8:
    case 0x01AD:
    case 0x01B0:
    case 0x01B4:
    case 0x01B6:
    case 0x01B9:
    case 0x01BD:
    case 0x01C6:
    case 0x01C9:
    case 0x01CC:
    case 0x01F3:
    case 0x01F5:
    case 0x0239:
    case 0x023C:
    case 0x0242:
      return codePoint - 1;
    }

  return codePoint;
}


static INLINE uint32_t
to_lowercase_basic_latinB_ext(const uint32_t codePoint)
{
  assert((0x0180 <= codePoint) && (codePoint < 0x0250));

  if ((codePoint & 1) != 0)
    {
      if ((0x01CD <= codePoint) && (codePoint <= 0x01DC))
        return codePoint + 1;
    }
  else
    {
      if ((0x01DE <= codePoint) && (codePoint <= 0x01EF))
        return codePoint + 1;

      if ((0x01F8 <= codePoint) && (codePoint <= 0x021F))
        return codePoint + 1;

      if ((0x0222 <= codePoint) && (codePoint <= 0x0233))
        return codePoint + 1;

      if ((0x0246 <= codePoint) && (codePoint <= 0x024F))
        return codePoint + 1;
    }

  switch(codePoint)
    {
    case 0x0182:
    case 0x0184:
    case 0x0187:
    case 0x018B:
    case 0x0191:
    case 0x0198:
    case 0x01A0:
    case 0x01A2:
    case 0x01A4:
    case 0x01A7:
    case 0x01AC:
    case 0x01AF:
    case 0x01B3:
    case 0x01B5:
    case 0x01B8:
    case 0x01BC:
    case 0x01F4:
    case 0x0238:
    case 0x023B:
    case 0x0241:
      return codePoint + 1;

    case 0x01C4:
    case 0x01C5:
      return 0x01C6;

    case 0x01C7:
    case 0x01C8:
      return 0x01C9;

    case 0x01CA:
    case 0x01CB:
      return 0x01CC;

    case 0x01F1:
    case 0x01F2:
      return 0x01F3;

    case 0x01F7:
      return 0x01BF;

    case 0x0220:
      return 0X019E;

    case 0x023D:
      return 0x019A;

    case 0x0243:
      return 0x0180;
    }

  return codePoint;
}


static INLINE uint32_t
to_uppercase_greek_coptic(const uint32_t codePoint)
{
  assert((0x0374 <= codePoint) && (codePoint <= 0x03FF));

  if (codePoint == 0x03AC)
    return 0x0386;

  if ((0x03AD <= codePoint) && (codePoint <= 0x03AF))
    return codePoint - 0x03AD + 0x388;

  if ((0x03B1 <= codePoint) && (codePoint <= 0x3C1))
    return codePoint - 0x03B1 + 0x0391;

  if ((0x03C3 <= codePoint) && (codePoint <= 0x3CB))
    return codePoint - 0x03C3 + 0x03A3;

  if (codePoint == 0x3CC)
    return 0x38C;

  if (codePoint == 0x03CD)
    return 0x038E;

  if (codePoint == 0x03CE)
    return 0x038F;

  if ((0x03E2 <= codePoint) && (codePoint <= 0x03EF))
    return((codePoint & 1) != 0) ? codePoint - 1 : codePoint;

  return codePoint;
}


static INLINE uint32_t
to_lowercase_greek_coptic(const uint32_t codePoint)
{
  assert((0x0374 <= codePoint) && (codePoint <= 0x03FF));

  if (codePoint == 0x0386)
    return 0x03AC;

  if ((0x0388 <= codePoint) && (codePoint < 0x038B))
    return codePoint - 0x0388 + 0x03AD;

  if (codePoint == 0x38C)
    return 0x3CC;

  if (codePoint == 0x038E)
    return 0x03CD;

  if (codePoint == 0x038F)
    return 0x03CE;

  if ((0x0391 <= codePoint) && (codePoint <= 0x03A1))
    return codePoint - 0x0391 + 0x03B1;

  if ((0x03A3 <= codePoint) && (codePoint <= 0x3AB))
    return codePoint - 0x03A3 + 0x03C3;

  if ((0x03E2 <= codePoint) && (codePoint <= 0x03EF))
    return((codePoint & 1) == 0) ? codePoint + 1 : codePoint;

  return codePoint;
}


static INLINE uint32_t
to_uppercase_cyrillic(const uint32_t codePoint)
{
  assert((0x0400 <= codePoint) && (codePoint <= 0x0527));

  if ((0x0430 <= codePoint) && (codePoint <= 0x044F))
    return codePoint - 0x0430 + 0x0410;

  if ((0x0450 <= codePoint) && (codePoint <= 0x45F))
    return codePoint - 0x0450 + 0x0400;

  if ((0x0482 <= codePoint) && (codePoint <= 0x0489))
    return codePoint;

  if ((codePoint & 1) != 0)
    {
      if ((0x0460 <= codePoint) && (codePoint < 0x04C0))
        return codePoint - 1;

      if (codePoint == 0x04CF)
        return 0x04C0;

      if ((0x04D0 <= codePoint) && (codePoint <= 0x0527))
        return codePoint - 1;
    }
  else
    {
      if ((0x04C2 <= codePoint) && (codePoint < 0x04CF))
        return codePoint - 1;
    }

  return codePoint;
}


static INLINE uint32_t
to_lowercase_cyrillic(const uint32_t codePoint)
{
  assert((0x0400 <= codePoint) && (codePoint <= 0x0527));

  if ((0x0410 <= codePoint) && (codePoint <= 0x042F))
    return codePoint - 0x0410 + 0x0430;

  if ((0x0400 <= codePoint) && (codePoint <= 0x040F))
    return codePoint - 0x0400 + 0x0450;

  if ((0x0482 <= codePoint) && (codePoint <= 0x0489))
    return codePoint;

  if ((codePoint & 1) == 0)
    {
      if ((0x0460 <= codePoint) && (codePoint < 0x04C0))
        return codePoint + 1;

      if (codePoint == 0x04C0)
        return 0x04CF;

      if ((0x04D0 <= codePoint) && (codePoint <= 0x0527))
        return codePoint + 1;

    }
  else
    {
      if ((0x04C1 <= codePoint) && (codePoint < 0x04CF))
        return codePoint + 1;
    }

  return codePoint;
}


static INLINE uint32_t
to_uppercase_armenian(const uint32_t codePoint)
{
  assert((0x0561 <= codePoint) && (codePoint <= 0x0586));

  return codePoint - 0x0561 + 0x0531;
}


static INLINE uint32_t
to_lowercase_armenian(const uint32_t codePoint)
{
  assert((0x0531 <= codePoint) && (codePoint <= 0x0556));

  return codePoint - 0x0531 + 0x0561;
}


static INLINE uint32_t
to_canonical_latin1_supp(const uint32_t codePoint)
{
  assert((0x0080 <= codePoint) && (codePoint <= 0x00FF));

  if ((0x00E0 <= codePoint) && (codePoint <= 0x00E6))
    return 'a';

  if (codePoint == 0x00E7)
    return 'c';

  if ((0x00E8 <= codePoint) && (codePoint <= 0x00EB))
    return 'e';

  if ((0x00EC <= codePoint) && (codePoint <= 0x00EF))
    return 'i';

  if (codePoint == 0x00F1)
    return 'n';

  if ((0x00F2 <= codePoint) && (codePoint <= 0x00F8) && (codePoint != 0x00F7))
    return 'o';

  if ((0x00F9 <= codePoint) && (codePoint <= 0x00FC))
    return 'u';

  if ((codePoint == 0x00FD) || (codePoint == 0x00FF))
    return 'y';

  if ((0x00C0 <= codePoint) && (codePoint <= 0x00C6))
    return 'A';

  if (codePoint == 0x00C7)
    return 'C';

  if ((0x00C8 <= codePoint) && (codePoint <= 0x00CB))
    return 'E';

  if ((0x00CC <= codePoint) && (codePoint <= 0x00CF))
    return 'I';

  if (codePoint == 0x00D1)
    return 'N';

  if ((0x00D2 <= codePoint) && (codePoint <= 0x00D8) && (codePoint != 0x00D7))
    return 'O';

  if ((0x00D9 <= codePoint) && (codePoint <= 0x00DC))
    return 'U';

  if (codePoint == 0x00DD)
    return 'Y';

  return codePoint;
}


static INLINE uint32_t
to_canonical_latinA_ext(const uint32_t codePoint)
{
  assert((0x0100 <= codePoint) && (codePoint <= 0x17F));

  if ((codePoint & 1) == 0)
    {
      if ((0x0100 <= codePoint) && (codePoint <= 0x0105))
        return 'A';

      if ((0x0106 <= codePoint) && (codePoint <= 0x010D))
        return 'C';

      if ((0x010E <= codePoint) && (codePoint <= 0x0111))
        return 'D';

      if ((0x0112 <= codePoint) && (codePoint <= 0x011B))
        return 'E';

      if ((0x011C <= codePoint) && (codePoint <= 0x0123))
        return 'G';

      if ((0x0124 <= codePoint) && (codePoint <= 0x0127))
        return 'H';

      if ((0x0128 <= codePoint) && (codePoint <= 0x012F))
        return 'I';

      if ((0x0128 <= codePoint) && (codePoint <= 0x012F))
        return 'I';

      if ((codePoint == 0x132 ))
        return 'I';

      if ((0x134 <= codePoint) && (codePoint <= 0x0135))
        return 'J';

      if ((0x136 <= codePoint) && (codePoint <= 0x0137))
        return 'K';

      if ((0x0139 <= codePoint) && (codePoint <= 0x0142))
        return 'l';

      if ((0x0142 <= codePoint) && (codePoint <= 0x0148))
        return 'n';

      if ((0x014A <= codePoint) && (codePoint <= 0x014B))
        return 'N';

      if ((0x014C <= codePoint) && (codePoint <= 0x0153))
        return 'O';

      if ((0x0154 <= codePoint) && (codePoint <= 0x0159))
        return 'R';

      if ((0x015A <= codePoint) && (codePoint <= 0x0161))
        return 'S';

      if ((0x0162 <= codePoint) && (codePoint <= 0x0167))
        return 'T';

      if ((0x0168 <= codePoint) && (codePoint <= 0x0173))
        return 'U';

      if ((0x0174 <= codePoint) && (codePoint <= 0x0175))
        return 'W';

      if ((0x0176 <= codePoint) && (codePoint <= 0x0178))
        return 'Y';

      if ((0x0179 <= codePoint) && (codePoint <= 0x017E))
        return 'z';
    }
  else
    {
      if ((0x0100 <= codePoint) && (codePoint <= 0x0105))
        return 'a';

      if ((0x0106 <= codePoint) && (codePoint <= 0x010D))
        return 'c';

      if ((0x010E <= codePoint) && (codePoint <= 0x0111))
        return 'd';

      if ((0x0112 <= codePoint) && (codePoint <= 0x011B))
        return 'e';

      if ((0x011C <= codePoint) && (codePoint <= 0x0123))
        return 'g';

      if ((0x0124 <= codePoint) && (codePoint <= 0x0127))
        return 'h';

      if ((0x0128 <= codePoint) && (codePoint <= 0x012F))
        return 'i';

      if (codePoint == 0x0133)
        return 'i';

      if ((0x134 <= codePoint) && (codePoint <= 0x0135))
        return 'j';

      if ((0x136 <= codePoint) && (codePoint <= 0x0137))
        return 'k';

      if ((0x0139 <= codePoint) && (codePoint <= 0x0142))
        return 'L';

      if ((0x0142 <= codePoint) && (codePoint <= 0x0148))
        return 'N';

      if ((0x014A <= codePoint) && (codePoint <= 0x014B))
        return 'n';

      if ((0x014C <= codePoint) && (codePoint <= 0x0153))
        return 'o';

      if ((0x0154 <= codePoint) && (codePoint <= 0x0159))
        return 'r';

      if ((0x015A <= codePoint) && (codePoint <= 0x0161))
        return 's';

      if ((0x0162 <= codePoint) && (codePoint <= 0x0167))
        return 't';

      if ((0x0168 <= codePoint) && (codePoint <= 0x0173))
        return 'u';

      if ((0x0174 <= codePoint) && (codePoint <= 0x0175))
        return 'w';

      if ((0x0176 <= codePoint) && (codePoint <= 0x0177))
        return 'y';

      if ((0x0179 <= codePoint) && (codePoint <= 0x017E))
        return 'Z';
    }
  return codePoint;
}


static INLINE uint32_t
to_canonical_latinB_ext(const uint32_t codePoint)
{
  assert((0x0180 <= codePoint) && (codePoint <= 0x024F));

  switch(codePoint)
    {
    case 0x0187:
      return 'C';

    case 0x0188:
      return 'c';

    case 0x018B:
      return 'D';

    case 0x018C:
      return 'd';

    case 0x0191:
      return 'F';

    case 0x0192:
      return 'f';

    case 0x0198:
      return 'K';

    case 0x0199:
      return 'k';

    case 0x019A:
      return 'l';

    case 0x01A0:
      return 'O';

    case 0x01A1:
      return 'o';

    case 0x01AD:
      return 't';

    case 0x01AC:
      return 'T';

    case 0x01AF:
      return 'U';

    case 0x01B0:
      return 'u';

    case 0x01B3:
      return 'Y';

    case 0x01B4:
      return 'y';

    case 0x01B5:
      return 'Z';

    case 0x01B6:
      return 'z';

    case 0x01C4:
    case 0x01C5:
      return 'D';

    case 0x01C6:
      return 'd';

    case 0x01C7:
    case 0x01C8:
      return 'L';

    case 0x01C9:
      return 'l';

    case 0x01CA:
    case 0x01CB:
      return 'N';

    case 0x01CC:
      return 'n';

    case 0x01CD:
      return 'A';

    case 0x01CE:
      return 'a';

    case 0x01CF:
      return 'I';

    case 0x01D0:
      return 'i';

    case 0x01D1:
      return 'O';

    case 0x01D2:
      return 'o';

    case 0x01D3:
    case 0x01D5:
    case 0x01D7:
    case 0x01D9:
    case 0x01DB:
      return 'U';

    case 0x01D4:
    case 0x01D6:
    case 0x01D8:
    case 0x01DA:
    case 0x01DC:
      return 'u';

    case 0x01DE:
    case 0x01E0:
    case 0x01E2:
      return 'A';

    case 0x01DF:
    case 0x01E1:
    case 0x01E3:
      return 'a';

    case 0x01E4:
    case 0x01E6:
      return 'G';

    case 0x01E5:
    case 0x01E7:
      return 'g';

    case 0x01E8:
      return 'K';

    case 0x01E9:
      return 'k';

    case 0x01EA:
    case 0x01EC:
      return 'O';

    case 0x01EB:
    case 0x01ED:
      return 'o';

    case 0x01F1:
    case 0x01F2:
      return 'D';

    case 0x01F3:
      return 'd';

    case 0x01F4:
      return 'G';

    case 0x01F5:
      return 'g';

    case 0x01F8:
      return 'N';

    case 0x01F9:
      return 'n';

    case 0x01FA:
    case 0x01FC:
      return 'A';

    case 0x01FB:
    case 0x01FD:
      return 'a';

    case 0x01FE:
      return 'O';

    case 0x01FF:
      return 'o';

    case 0x0200:
    case 0x0202:
      return 'A';

    case 0x0201:
    case 0x0203:
      return 'a';

    case 0x0204:
    case 0x0206:
      return 'E';

    case 0x0205:
    case 0x0207:
      return 'e';

    case 0x0208:
    case 0x020A:
      return 'I';

    case 0x0209:
    case 0x020B:
      return 'i';

    case 0x020C:
    case 0x020E:
      return 'O';

    case 0x020D:
    case 0x020F:
      return 'o';

    case 0x0210:
    case 0x0212:
      return 'R';

    case 0x0211:
    case 0x0213:
      return 'r';

    case 0x0214:
    case 0x0216:
      return 'U';

    case 0x0215:
    case 0x0217:
      return 'u';

    case 0x0218:
      return 'S';

    case 0x0219:
      return 's';

    case 0x021A:
      return 'T';

    case 0x021B:
      return 't';

    case 0x021E:
      return 'H';

    case 0x021F:
      return 'h';

    case 0x0224:
      return 'Z';

    case 0x0225:
      return 'z';

    case 0x0226:
      return 'A';

    case 0x0227:
      return 'a';

    case 0x0228:
      return 'E';

    case 0x0229:
      return 'e';

    case 0x022A:
    case 0x022C:
    case 0x022E:
    case 0x0230:
      return 'O';

    case 0x022B:
    case 0x022D:
    case 0x022F:
    case 0x0231:
      return 'o';

    case 0x0232:
      return 'Y';

    case 0x0233:
      return 'y';

    case 0x023B:
      return 'C';

    case 0x023C:
      return 'c';

    case 0x023D:
      return 'L';

    case 0x0246:
      return 'E';

    case 0x0247:
      return 'e';

    case 0x0248:
      return 'J';

    case 0x0249:
      return 'j';

    case 0x024A:
      return 'Q';

    case 0x024B:
      return 'q';

    case 0x024C:
      return 'R';

    case 0x024D:
      return 'r';

    case 0x024E:
      return 'Y';

    case 0x024F:
      return 'y';
    }

  return codePoint;
}


static uint32_t
to_canonical_greek_coptic(const uint32_t codePoint)
{
  switch(codePoint)
    {
    case 0x0386:
      return 0x0391;

    case 0x0388:
      return 0x0395;

    case 0x0389:
      return 0x0397;

    case 0x038A:
      return 0x0399;

    case 0x038C:
      return 0x039F;

    case 0x038E:
      return 0x03A5;

    case 0x038F:
      return 0x03A9;

    case 0x0390:
      return 0x03B9;

    case 0x03AA:
      return 0x0399;

    case 0x03AB:
      return 0x03A5;

    case 0x03AC:
      return 0x03B1;

    case 0x03AD:
      return 0x03B5;

    case 0x03AE:
      return 0x03B7;

    case 0x03AF:
      return 0x03B9;

    case 0x03B0:
      return 0x03C5;

    case 0x03CA:
      return 0x03B9;

    case 0x03CB:
      return 0x03C5;

    case 0x03CC:
      return 0x03BF;

    case 0x03CD:
      return 0x03C5;

    case 0x03CE:
      return 0x03C9;

    case 0x03D2:
      return 0x03A5;

    case 0x03D3:
      return 0x03A5;

    case 0x03D4:
      return 0x03A5;
    }

  return codePoint;
}


static uint32_t
to_canonical_cyrillic(const uint32_t codePoint)
{
  switch(codePoint)
  {
  case 0x0400:
    return 0x0415;

  case 0x040D:
    return 0x0418;

  case 0x0419:
    return 0x0418;

  case 0x0439:
    return 0x0438;

  case 0x0450:
    return 0x0435;

  case 0x045D:
    return 0x0438;

  case 0x0476:
    return 0x0474;

  case 0x0477:
    return 0x0475;

  case 0x047C:
    return 0x0460;

  case 0x047D:
    return 0x0461;

  case 0x048A:
    return 0x0418;

  case 0x048B:
    return 0x0438;

  case 0x048E:
    return 0x0420;

  case 0x048F:
    return 0x0440;

  case 0x0490:
    return 0x0413;

  case 0x0491:
    return 0x0433;

  case 0x0492:
    return 0x0413;

  case 0x0493:
    return 0x0433;

  case 0x0494:
    return 0x0413;

  case 0x0495:
    return 0x0433;

  case 0x0496:
    return 0x0416;

  case 0x0497:
    return 0x0436;

  case 0x0498:
    return 0x0417;

  case 0x0499:
    return 0x0437;

  case 0x049A:
    return 0x041A;

  case 0x049B:
    return 0x043A;

  case 0x049C:
    return 0x041A;

  case 0x049D:
    return 0x043A;

  case 0x049E:
    return 0x041A;

  case 0x049F:
    return 0x043A;

  case 0x04A2:
    return 0x041D;

  case 0x04A3:
    return 0x043D;

  case 0x04A6:
    return 0x041F;

  case 0x04A7:
    return 0x043F;

  case 0x04AA:
    return 0x0421;

  case 0x04AB:
    return 0x0441;

  case 0x04AC:
    return 0x0422;

  case 0x04AD:
    return 0x0442;

  case 0x04B0:
    return 0x0423;

  case 0x04B1:
    return 0x0443;

  case 0x04B2:
    return 0x0425;

  case 0x04B3:
    return 0x0445;

  case 0x04B6:
    return 0x0427;

  case 0x04B7:
    return 0x0447;

  case 0x04B8:
    return 0x0427;

  case 0x04B9:
    return 0x0447;

  case 0x04BE:
    return 0x04BC;

  case 0x04BF:
    return 0x04BD;

  case 0x04C1:
    return 0x0416;

  case 0x04C2:
    return 0x0436;

  case 0x04C3:
    return 0x041A;

  case 0x04C4:
    return 0x043A;

  case 0x04C5:
    return 0x041B;

  case 0x04C6:
    return 0x043B;

  case 0x04C7:
    return 0x041D;

  case 0x04C8:
    return 0x043D;

  case 0x04C9:
    return 0x041D;

  case 0x04CA:
    return 0x043D;

  case 0x04CD:
    return 0x041C;

  case 0x04CE:
    return 0x043C;

  case 0x04D0:
    return 0x0410;

  case 0x04D1:
    return 0x0430;

  case 0x04D2:
    return 0x0410;

  case 0x04D3:
    return 0x0430;

  case 0x04D6:
    return 0x0415;

  case 0x04D7:
    return 0x0435;

  case 0x04DA:
    return 0x0429;

  case 0x04DB:
    return 0x0449;

  case 0x04DC:
    return 0x0416;

  case 0x04DD:
    return 0x0436;

  case 0x04DE:
    return 0x0417;

  case 0x04DF:
    return 0x0437;

  case 0x04E2:
    return 0x0418;

  case 0x04E3:
    return 0x0438;

  case 0x04E4:
    return 0x0418;

  case 0x04E5:
    return 0x0438;

  case 0x04E6:
    return 0x041E;

  case 0x04E7:
    return 0x043E;

  case 0x04EA:
    return 0x041E;

  case 0x04EB:
    return 0x043E;

  case 0x04EC:
    return 0x042D;

  case 0x04ED:
    return 0x044D;

  case 0x04EE:
    return 0x0423;

  case 0x04EF:
    return 0x0443;

  case 0x04F0:
    return 0x0423;

  case 0x04F1:
    return 0x0443;

  case 0x04F2:
    return 0x0423;

  case 0x04F3:
    return 0x0443;

  case 0x04F4:
    return 0x0427;

  case 0x04F5:
    return 0x0447;

  case 0x04F6:
    return 0x0413;

  case 0x04F7:
    return 0x0433;

  case 0x04F8:
    return 0x042B;

  case 0x04F9:
    return 0x044B;

  case 0x04FA:
    return 0x0413;

  case 0x04FB:
    return 0x0433;

  case 0x04FC:
    return 0x0425;

  case 0x04FD:
    return 0x0445;

  case 0x04FE:
    return 0x0425;

  case 0x04FF:
    return 0x0445;
  }
  return codePoint;
}


uint32_t
wh_to_canonical(const uint32_t codePoint)
{
  if (codePoint < 0x0374)
    {
      if (codePoint < 0x0080)
        return codePoint;

      else if (codePoint < 0x0100)
        return to_canonical_latin1_supp(codePoint);

      else if (codePoint < 0x0180)
        return to_canonical_latinA_ext(codePoint);

      else if (codePoint < 0x0250)
        return to_canonical_latinB_ext(codePoint);
    }
  else if ((0x0374 <= codePoint) && (codePoint < 0x0400))
    return to_canonical_greek_coptic(codePoint);

  else if (codePoint < 0x528)
    return to_canonical_cyrillic(codePoint);

  return codePoint;
}


bool_t
wh_is_lowercase(const uint32_t codePoint)
{
  if (wh_to_uppercase( codePoint) != codePoint)
    return TRUE;

  return FALSE;
}


bool_t
wh_is_uppercase(const uint32_t codePoint)
{
  if (wh_to_lowercase( codePoint) != codePoint)
    return TRUE;

  return FALSE;
}


uint32_t
wh_to_lowercase(const uint32_t codePoint)
{
  if (codePoint < 0x0374)
    {
      if (codePoint < 0x80)
        return to_lowercase_basic_latin(codePoint);

      else if (codePoint < 0x0100)
        return to_lowercase_basic_latin1_supp(codePoint);

      else if (codePoint < 0x0180)
        return to_lowercase_basic_latinA_ext(codePoint);

      else if (codePoint < 0x0250)
        return to_lowercase_basic_latinB_ext(codePoint);
    }
  else if ((0x0374 <= codePoint) && (codePoint < 0x0400))
    return to_lowercase_greek_coptic(codePoint);

  else if (codePoint < 0x528)
    return to_lowercase_cyrillic(codePoint);

  else if ((0x0531 <= codePoint) && (codePoint <= 0x0556))
    return to_lowercase_armenian(codePoint);

  return codePoint;
}


uint32_t
wh_to_uppercase(const uint32_t codePoint)
{
  if (codePoint < 0x0374)
    {
      if (codePoint < 0x80)
        return to_uppercase_basic_latin(codePoint);

      else if (codePoint < 0x0100)
        return to_uppercase_basic_latin1_supp(codePoint);

      else if (codePoint < 0x0180)
        return to_uppercase_basic_latinA_ext(codePoint);

      else if (codePoint < 0x0250)
        return to_uppercase_basic_latinB_ext(codePoint);
    }
  else if ((0x0374 <= codePoint) && (codePoint < 0x0400))
    return to_uppercase_greek_coptic(codePoint);

  else if (codePoint < 0x528)
    return to_uppercase_cyrillic(codePoint);

  else if ((0x0561 <= codePoint) && (codePoint <= 0x0586))
    return to_uppercase_armenian(codePoint);

  return codePoint;
}


int
wh_cmp_alphabetically(const uint32_t cp1, const uint32_t cp2)
{
  const int bcp1 = wh_to_canonical(cp1);
  const int bcp2 = wh_to_canonical(cp2);

  if (bcp1 == bcp2)
    return(int)cp1 - (int)cp2;

  const int lbcp1 = wh_to_uppercase(bcp1);
  const int lbcp2 = wh_to_uppercase(bcp2);

  return(lbcp1 == lbcp2) ? (bcp1 - bcp2 ) : (lbcp1 - lbcp2);
}


uint32_t
wh_prev_char(const uint32_t codePoint)
{
  assert(codePoint > 1);
  assert(codePoint <= UTF_LAST_CODEPOINT);

  if (codePoint == (UTF16_EXTRA_BYTE_MAX + 1))
    return UTF16_EXTRA_BYTE_MIN - 1;

  return codePoint - 1;
}


uint32_t
wh_next_char(const uint32_t codePoint)
{
  assert(codePoint >= 1);
  assert(codePoint < UTF_LAST_CODEPOINT);

  if (codePoint == (UTF16_EXTRA_BYTE_MIN - 1))
    return UTF16_EXTRA_BYTE_MAX + 1;

  return codePoint + 1;
}
