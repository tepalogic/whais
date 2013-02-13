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

#include "utf8.h"

D_UINT
get_utf8_char_size (D_UINT8 firstUtf8Byte)
{
  if ((firstUtf8Byte & UTF8_EXTRA_BYTE_SIG) == UTF8_7BIT_MASK)
    return 1;
  else if ((firstUtf8Byte & UTF8_16BIT_MASK) == UTF8_11BIT_MASK)
    return 2;
  else if ((firstUtf8Byte & UTF8_21BIT_MASK) == UTF8_16BIT_MASK)
    return 3;
  else if ((firstUtf8Byte & UTF8_26BIT_MASK) == UTF8_21BIT_MASK)
    return 4;
  else if ((firstUtf8Byte & UTF8_31BIT_MASK) == UTF8_26BIT_MASK)
    return 5;
  else if ((firstUtf8Byte & UTF8_37BIT_MASK) == UTF8_31BIT_MASK)
    return 6;

  return 0;
}

D_UINT
decode_utf8_char (const D_UINT8 *pSource, D_UINT32* pCh)
{
  *pCh = 0;
  if ((pSource[0] & UTF8_EXTRA_BYTE_SIG) == UTF8_7BIT_MASK)
    {
      *pCh |= pSource[0] & ~UTF8_7BIT_MASK;
      return 1;
    }
  else if ((pSource[0] & UTF8_16BIT_MASK) == UTF8_11BIT_MASK)
    {
      *pCh |= pSource[0] & ~UTF8_11BIT_MASK;
      *pCh <<= 6; *pCh |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      return 2;
    }
  else if ((pSource[0] & UTF8_21BIT_MASK) == UTF8_16BIT_MASK)
    {
      *pCh |= pSource[0] & ~UTF8_16BIT_MASK;
      *pCh <<= 6; *pCh |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      *pCh <<= 6; *pCh |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
      return 3;
    }
  else if ((pSource[0] & UTF8_26BIT_MASK) == UTF8_21BIT_MASK)
    {
      *pCh |= pSource[0] & ~UTF8_21BIT_MASK;
      *pCh <<= 6; *pCh |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      *pCh <<= 6; *pCh |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
      *pCh <<= 6; *pCh |= pSource[3] & ~UTF8_EXTRA_BYTE_SIG;
      return 4;
    }
  else if ((pSource[0] & UTF8_31BIT_MASK) == UTF8_26BIT_MASK)
    {
      *pCh |= pSource[0] & ~UTF8_26BIT_MASK;
      *pCh <<= 6; *pCh |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
      *pCh <<= 6; *pCh |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
      *pCh <<= 6; *pCh |= pSource[3] & ~UTF8_EXTRA_BYTE_SIG;
      *pCh <<= 6; *pCh |= pSource[4] & ~UTF8_EXTRA_BYTE_SIG;
      return 5;
    }
  else if ((pSource[0] & UTF8_37BIT_MASK) == UTF8_31BIT_MASK)
    {
        *pCh |= pSource[0] & ~UTF8_31BIT_MASK;
        *pCh <<= 6; *pCh |= pSource[1] & ~UTF8_EXTRA_BYTE_SIG;
        *pCh <<= 6; *pCh |= pSource[2] & ~UTF8_EXTRA_BYTE_SIG;
        *pCh <<= 6; *pCh |= pSource[3] & ~UTF8_EXTRA_BYTE_SIG;
        *pCh <<= 6; *pCh |= pSource[4] & ~UTF8_EXTRA_BYTE_SIG;
        *pCh <<= 6; *pCh |= pSource[5] & ~UTF8_EXTRA_BYTE_SIG;
        return 6;
    }

  return 0;
}

D_UINT
encode_utf8_char (D_UINT32 ch, D_UINT8 *pDest)
{
  if (ch < 0x80)
    {
      pDest[0] = ch & 0xFF;
      return 1;
    }
  else if (ch < 0x800)
    {
      pDest[0] = ((ch >> 6) & 0xFF) | UTF8_11BIT_MASK;
      pDest[1] = (ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 2;
    }
  else if (ch < 0x10000)
    {
      pDest[0] = ((ch >> 12) & 0xFF) | UTF8_16BIT_MASK;
      pDest[1] = ((ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = (ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 3;
    }
  else if (ch < 0x200000)
    {
      pDest[0] = ((ch >> 18) & 0xFF) | UTF8_21BIT_MASK;
      pDest[1] = ((ch >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = ((ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[3] = (ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 4;
    }
  else if (ch < 0x4000000)
    {
      pDest[0] = ((ch >> 24) & 0xFF) | UTF8_26BIT_MASK;
      pDest[1] = ((ch >> 18) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = ((ch >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[3] = ((ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[4] = (ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 5;
    }
  else if (ch < 0x80000000)
    {
      pDest[0] = ((ch >> 30) & 0xFF) | UTF8_31BIT_MASK;
      pDest[1] = ((ch >> 24) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[2] = ((ch >> 18) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[3] = ((ch >> 12) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[4] = ((ch >> 6) & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      pDest[5] = (ch & 0x3F) | UTF8_EXTRA_BYTE_SIG;
      return 6;
    }

  return 0;
}

D_INT
utf8_strlen (const D_UINT8* pSource)
{
  D_INT result = 0;
  while (*pSource != 0)
    {
      D_UINT chSize = get_utf8_char_size (pSource[0]);
      if (chSize == 0)
        return -1;

      result  += chSize;
      pSource += chSize;
    }

  return result;
}
