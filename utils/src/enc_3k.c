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

#include "whais.h"

#include "enc_3k.h"
#include "endianness.h"

static uint32_t
exchange_1bit_pair(uint32_t value, const uint_t p1, const uint_t p2)
{
  const uint32_t val1 = (value >> p1) & 1;
  const uint32_t val2 = (value >> p2) & 1;

  assert(p1 < 32);
  assert(p2 < 32);

  value &= ~((1 << p1) | (1 << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static uint32_t
exchange_2bit_pair(uint32_t value, const uint_t p1, const uint_t p2)
{
  const uint32_t val1 = (value >> p1) & 0x03;
  const uint32_t val2 = (value >> p2) & 0x03;

  assert((p1 < 32) && ((p1 % 2) == 0));
  assert((p2 < 32) && ((p2 % 2) == 0));

  value &= ~((0x03 << p1) | (0x03 << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static uint32_t
exchange_4bit_pair(uint32_t value, const uint_t p1, const uint_t p2)
{
  const uint32_t val1 = (value >> p1) & 0x0F;
  const uint32_t val2 = (value >> p2) & 0x0F;

  assert((p1 < 32) && ((p1 % 4) == 0));
  assert((p2 < 32) && ((p2 % 4) == 0));

  value &= ~((0x0F << p1) | (0x0F << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static uint32_t
exchange_8bit_pair(uint32_t value, const uint_t p1, const uint_t p2)
{
  const uint32_t val1 = (value >> p1) & 0xFF;
  const uint32_t val2 = (value >> p2) & 0xFF;

  assert((p1 < 32) && ((p1 % 8) == 0));
  assert((p2 < 32) && ((p2 % 8) == 0));

  value &= ~((0xFF << p1) | (0xFF << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

static uint32_t
exchange_16bit_pair(uint32_t value, const uint_t p1, const uint_t p2)
{
  const uint32_t val1 = (value >> p1) & 0xFFFF;
  const uint32_t val2 = (value >> p2) & 0xFFFF;

  assert((p1 < 32) && ((p1 % 16) == 0));
  assert((p2 < 32) && ((p2 % 16) == 0));

  value &= ~((0xFFFF << p1) | (0xFFFF << p2));
  value |= (val1 << p2) | (val2 << p1);

  return value;
}

void
wh_buff_3k_encode(const uint32_t           firstKing,
                  const uint32_t           secondKing,
                  const uint8_t* const     key,
                  const uint_t             keyLen,
                  uint8_t* const           buffer,
                  const uint_t             bufferSize)
{
  uint_t pos;
  uint_t keyIndex = firstKing % keyLen;
  int    b;

  assert(bufferSize % sizeof(uint32_t) == 0);


  for (pos = 0; pos < bufferSize; pos += sizeof(uint32_t))
  {
    uint64_t message = load_le_int32(buffer + pos);
    uint32_t thirdKing = 0;

    message -= firstKing;
    message ^= secondKing;

    thirdKing = key[keyIndex++];
    thirdKing <<= 8;
    keyIndex %= keyLen;

    thirdKing |= key[keyIndex++];
    thirdKing <<= 8;
    keyIndex %= keyLen;

    thirdKing |= key[keyIndex++];
    thirdKing <<= 8;
    keyIndex %= keyLen;

    thirdKing |= key[keyIndex++];
    keyIndex %= keyLen;

    for (b = 0; b < 16; ++b)
    {
      if (thirdKing & (1 << b))
        message = exchange_1bit_pair(message, 2 * b, 2 * b + 1);
    }

    for (b = 0; b < 8; ++b)
    {
      if (thirdKing & (1 << (16 + b)))
        message = exchange_2bit_pair(message, 4 * b, 4 * b + 2);
    }

    for (b = 0; b < 4; ++b)
    {
      if (thirdKing & (1 << (24 + b)))
        message = exchange_4bit_pair(message, 8 * b, 8 * b + 4);
    }

    for (b = 0; b < 2; ++b)
    {
      if (thirdKing & (1 << (28 + b)))
        message = exchange_8bit_pair(message, 16 * b, 16 * b + 8);
    }

    if (thirdKing & (1 << 30))
      message = exchange_16bit_pair(message, 0, 16);

    if (thirdKing & (1 << 31))
      message = exchange_8bit_pair(message, 8, 16);

    store_le_int32(message, buffer + pos);
  }
}


void
wh_buff_3k_decode(const uint32_t           firstKing,
                  const uint32_t           secondKing,
                  const uint8_t* const     key,
                  const uint_t             keyLen,
                  uint8_t* const           buffer,
                  const uint_t             bufferSize)
{
  uint_t pos, keyIndex = firstKing % keyLen;
  int    b;

  assert(bufferSize % sizeof(uint32_t) == 0);

  for (pos = 0; pos < bufferSize; pos += sizeof(uint32_t))
  {
    uint64_t message = load_le_int32(buffer + pos);
    uint32_t thirdKing = 0;

    thirdKing = key[keyIndex++];
    thirdKing <<= 8;
    keyIndex %= keyLen;

    thirdKing |= key[keyIndex++];
    thirdKing <<= 8;
    keyIndex %= keyLen;

    thirdKing |= key[keyIndex++];
    thirdKing <<= 8;
    keyIndex %= keyLen;

    thirdKing |= key[keyIndex++];
    keyIndex %= keyLen;

    if (thirdKing & (1 << 31))
      message = exchange_8bit_pair(message, 8, 16);

    if (thirdKing & (1 << 30))
      message = exchange_16bit_pair(message, 0, 16);

    for (b = 1; b >= 0; --b)
    {
      if (thirdKing & (1 << (28 + b)))
        message = exchange_8bit_pair(message, 16 * b, 16 * b + 8);
    }

    for (b = 3; b >= 0; --b)
    {
      if (thirdKing & (1 << (24 + b)))
        message = exchange_4bit_pair(message, 8 * b, 8 * b + 4);
    }

    for (b = 7; b >= 0; --b)
    {
      if (thirdKing & (1 << (16 + b)))
        message = exchange_2bit_pair(message, 4 * b, 4 * b + 2);
    }

    for (b = 15; b >= 0; --b)
    {
      if (thirdKing & (1 << b))
        message = exchange_1bit_pair(message, 2 * b, 2 * b + 1);
    }

    message ^= secondKing;
    message += firstKing;

    store_le_int32(message, buffer + pos);
  }
}
