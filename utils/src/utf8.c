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

#include <assert.h>

#include "utf8.h"

uint_t
wh_utf8_cu_count (const uint8_t codeUnit)
{
  if (codeUnit < 0x80)
    return 1;

  else if (codeUnit < 0xC0)
    return 0;

  else if (codeUnit < 0xE0)
    return 2;

  else if (codeUnit < 0xF0)
    return 3;

  else if (codeUnit < 0xF8)
    return 4;

#if 0
  else if (codeUnit < 0xFC)
    return 5;

  return 6;
#endif

  return 0;
}


uint_t
wh_load_utf8_cp (const uint8_t* const utf8Str, uint32_t* const outCodePoint)
{
  const uint_t cpCount = wh_utf8_cu_count (utf8Str[0]);

  *outCodePoint = 0;

  switch (cpCount)
    {
    case 1:

      *outCodePoint |= utf8Str[0] & ~UTF8_7BIT_MASK;
      break;

    case 2:

      *outCodePoint |= utf8Str[0] & ~UTF8_11BIT_MASK;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[1] & ~UTF8_EXTRA_BYTE_SIG;
      break;

    case 3:

      *outCodePoint |= utf8Str[0] & ~UTF8_16BIT_MASK;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[1] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[2] & ~UTF8_EXTRA_BYTE_SIG;
      break;

    case 4:

      *outCodePoint |= utf8Str[0] & ~UTF8_21BIT_MASK;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[1] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[2] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[3] & ~UTF8_EXTRA_BYTE_SIG;
      break;

#if 0
    case 5:
      *outCodePoint |= utf8Str[0] & ~UTF8_26BIT_MASK;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[1] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[2] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[3] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[4] & ~UTF8_EXTRA_BYTE_SIG;
      break;

    case 6:
      *outCodePoint |= utf8Str[0] & ~UTF8_31BIT_MASK;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[1] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[2] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[3] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[4] & ~UTF8_EXTRA_BYTE_SIG;
      *outCodePoint <<= 6; *outCodePoint |= utf8Str[5] & ~UTF8_EXTRA_BYTE_SIG;
      break;
#endif

    default:
      assert (FALSE);
      return 0;
    }

  return cpCount;
}


uint_t
wh_store_utf8_cp (const uint32_t codePoint, uint8_t* const dest)
{
  if (codePoint < 0x80)
    {
      dest[0] = codePoint & 0xFF;

      return 1;
    }
  else if (codePoint < 0x800)
    {
      dest[0] = ((codePoint >> 6) & 0xFF) | UTF8_11BIT_MASK;
      dest[1] = (codePoint & 0x3F) | UTF8_EXTRA_BYTE_SIG;

      return 2;
    }
  else if (codePoint < 0x10000)
    {
      dest[0] = ((codePoint >> 12) & 0xFF) | UTF8_16BIT_MASK;
      dest[1] = ((codePoint >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[2] = (codePoint & 0x3F) | UTF8_EXTRA_BYTE_SIG;

      return 3;
    }
  else if (codePoint < 0x200000)
    {
      dest[0] = ((codePoint >> 18) & 0xFF) | UTF8_21BIT_MASK;
      dest[1] = ((codePoint >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[2] = ((codePoint >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[3] = (codePoint & 0x3F) | UTF8_EXTRA_BYTE_SIG;

      return 4;
    }

#if 0
  else if (codePoint < 0x4000000)
    {
      dest[0] = ((codePoint >> 24) & 0xFF) | UTF8_26BIT_MASK;
      dest[1] = ((codePoint >> 18) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[2] = ((codePoint >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[3] = ((codePoint >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[4] = (codePoint & 0x3F) | UTF8_EXTRA_BYTE_SIG;

      return 5;
    }
  else if (codePoint < 0x80000000)
    {
      dest[0] = ((codePoint >> 30) & 0xFF) | UTF8_31BIT_MASK;
      dest[1] = ((codePoint >> 24) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[2] = ((codePoint >> 18) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[3] = ((codePoint >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[4] = ((codePoint >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      dest[5] = (codePoint & 0x3F) | UTF8_EXTRA_BYTE_SIG;

      return 6;
    }
#endif

  assert (FALSE);

  return 0;
}


uint_t
wh_utf8_store_size (const uint32_t codePoint)
{
  if (codePoint < 0x80)
    return 1;

  else if (codePoint < 0x800)
    return 2;

  else if (codePoint < 0x10000)
    return 3;

  else if (codePoint < 0x200000)
    return 4;

#if 0
  else if (codePoint < 0x4000000)
    return 5;

  else if (codePoint < 0x80000000)
    return 6;
#endif

  assert (FALSE);

  return 0;
}


int
wh_utf8_strlen (const uint8_t* utf8Str)
{
  int result = 0;
  while (*utf8Str != 0)
    {
      uint_t chSize = wh_utf8_cu_count (utf8Str[0]);
      if (chSize == 0)
        return -1;

      ++result,  utf8Str += chSize;
    }

  return result;
}

