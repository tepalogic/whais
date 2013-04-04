/******************************************************************************
UTILS - Common routines used trough WHISPER project
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

#include <assert.h>

#include "whisper.h"

#include "le_converter.h"

static D_UINT32
exchange_1bit_pair (D_UINT32 value, const D_UINT p1, const D_UINT p2)
{
  assert (p1 < 32);
  assert (p2 < 32);

  const D_UINT32 val1 = (value >> p1) & 1;
  const D_UINT32 val2 = (value >> p2) & 1;

  value &= ~((1 << p1) | (1 << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static D_UINT32
exchange_2bit_pair (D_UINT32 value, const D_UINT p1, const D_UINT p2)
{
  assert ((p1 < 32) && ((p1 % 2) == 0));
  assert ((p2 < 32) && ((p2 % 2) == 0));

  const D_UINT32 val1 = (value >> p1) & 0x03;
  const D_UINT32 val2 = (value >> p2) & 0x03;

  value &= ~((0x03 << p1) | (0x03 << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static D_UINT32
exchange_4bit_pair (D_UINT32 value, const D_UINT p1, const D_UINT p2)
{
  assert ((p1 < 32) && ((p1 % 4) == 0));
  assert ((p2 < 32) && ((p2 % 4) == 0));

  const D_UINT32 val1 = (value >> p1) & 0x0F;
  const D_UINT32 val2 = (value >> p2) & 0x0F;

  value &= ~((0x0F << p1) | (0x0F << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static D_UINT32
exchange_8bit_pair (D_UINT32 value, const D_UINT p1, const D_UINT p2)
{
  assert ((p1 < 32) && ((p1 % 8) == 0));
  assert ((p2 < 32) && ((p2 % 8) == 0));

  const D_UINT32 val1 = (value >> p1) & 0xFF;
  const D_UINT32 val2 = (value >> p2) & 0xFF;

  value &= ~((0xFF << p1) | (0xFF << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static D_UINT32
exchange_16bit_pair (D_UINT32 value, const D_UINT p1, const D_UINT p2)
{
  assert ((p1 < 32) && ((p1 % 16) == 0));
  assert ((p2 < 32) && ((p2 % 16) == 0));

  const D_UINT32 val1 = (value >> p1) & 0xFFFF;
  const D_UINT32 val2 = (value >> p2) & 0xFFFF;

  value &= ~((0xFFFF << p1) | (0xFFFF << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

void
encrypt_3k_buffer (const D_UINT32      firstKing,
                   const D_UINT32      secondKing,
                   const D_UINT* const key,
                   const D_UINT        keyLen,
                   D_UINT8*            buffer,
                   const D_UINT        bufferSize)
{
  D_UINT pos, b;

  assert (bufferSize % sizeof (D_UINT32) == 0);

  for (pos = 0; pos < bufferSize; pos += sizeof (D_UINT32))
    {
      D_UINT64 message   = from_le_int32 (buffer + pos);
      D_UINT32 thirdKing = 0;

      message -= firstKing;
      message ^= secondKing;

      thirdKing  = buffer[(pos + 0) % keyLen]; thirdKing <<= 8;
      thirdKing |= buffer[(pos + 1) % keyLen]; thirdKing <<= 8;
      thirdKing |= buffer[(pos + 2) % keyLen]; thirdKing <<= 8;
      thirdKing |= buffer[(pos + 3) % keyLen];

      for (b = 0; b < 16; ++b)
        {
          if (thirdKing & (1 << b))
            message = exchange_1bit_pair (message, 2 * b, 2 * b + 1);
        }

      for (b = 0; b < 8; ++b)
        {
          if (thirdKing & (1 << (16 + b)))
            message = exchange_2bit_pair (message, 4 * b, 4 * b + 2);
        }

      for (b = 0; b < 4; ++b)
        {
          if (thirdKing & (1 << (24 + b)))
            message = exchange_4bit_pair (message, 8 * b, 8 * b + 4);
        }

      for (b = 0; b < 2; ++b)
        {
          if (thirdKing & (1 << (28 + b)))
            message = exchange_8bit_pair (message, 16 * b, 16 * b + 8);
        }

      if (thirdKing & (1 << 30))
        message = exchange_16bit_pair (message, 0, 16);

      if (thirdKing & (1 << 30))
        message = exchange_8bit_pair (message, 8, 16);

      store_le_int32 (message, buffer + pos);
    }

}


void
decrypt_3k_buffer (const D_UINT32      firstKing,
                   const D_UINT32      secondKing,
                   const D_UINT* const key,
                   const D_UINT        keyLen,
                   D_UINT8*            buffer,
                   const D_UINT        bufferSize)
{
  D_UINT pos, b;
  assert (bufferSize % sizeof (D_UINT32) == 0);

  for (pos = 0; pos < bufferSize; pos += sizeof (D_UINT32))
    {
      D_UINT64 message   = from_le_int32 (buffer + pos);
      D_UINT32 thirdKing = 0;

      thirdKing  = buffer[(pos + 0) % keyLen]; thirdKing <<= 8;
      thirdKing |= buffer[(pos + 1) % keyLen]; thirdKing <<= 8;
      thirdKing |= buffer[(pos + 2) % keyLen]; thirdKing <<= 8;
      thirdKing |= buffer[(pos + 3) % keyLen];

      if (thirdKing & (1 << 30))
        message = exchange_8bit_pair (message, 8, 16);

      if (thirdKing & (1 << 30))
        message = exchange_16bit_pair (message, 0, 16);

      for (b = 0; b < 2; ++b)
        {
          if (thirdKing & (1 << (28 + b)))
            message = exchange_8bit_pair (message, 16 * b, 16 * b + 8);
        }

      for (b = 0; b < 4; ++b)
        {
          if (thirdKing & (1 << (24 + b)))
            message = exchange_4bit_pair (message, 8 * b, 8 * b + 4);
        }

      for (b = 0; b < 8; ++b)
        {
          if (thirdKing & (1 << (16 + b)))
            message = exchange_2bit_pair (message, 4 * b, 4 * b + 2);
        }

      for (b = 0; b < 16; ++b)
        {
          if (thirdKing & (1 << b))
            message = exchange_1bit_pair (message, 2 * b, 2 * b + 1);
        }

      message -= firstKing;
      message ^= secondKing;

      store_le_int32 (message, buffer + pos);
    }



}

