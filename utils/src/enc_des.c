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
#include <string.h>

#include "whais.h"

#include "enc_des.h"
#include "endianness.h"


static const uint8_t sgPC1[] =
    {
        57, 49, 41, 33, 25, 17, 9,
        1,  58, 50, 42, 34, 26, 18,
        10, 2,  59, 51, 43, 35, 27,
        19, 11, 3,  60, 52, 44, 36,
        63, 55, 47, 39, 31, 23, 15,
        7,  62, 54, 46, 38, 30, 22,
        14, 6,  61, 53, 45, 37, 29,
        21, 13, 5,  28, 20, 12, 4
    };

static const uint8_t sgLeftShifts[] =
    {
        1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
    };

static const uint8_t sgPC2[] =
    {
       14, 17, 11, 24, 1,  5,
       3,  28, 15, 6,  21, 10,
       23, 19, 12, 4,  26, 8,
       16, 7,  27, 20, 13, 2,
       41, 52, 31, 37, 47, 55,
       30, 40, 51, 45, 33, 48,
       44, 49, 39, 56, 34, 53,
       46, 42, 50, 36, 29, 32
    };

static const uint8_t sgIP[] =
    {
        58, 50, 42, 34, 26, 18, 10, 2,
        60, 52, 44, 36, 28, 20, 12, 4,
        62, 54, 46, 38, 30, 22, 14, 6,
        64, 56, 48, 40, 32, 24, 16, 8,
        57, 49, 41, 33, 25, 17, 9,  1,
        59, 51, 43, 35, 27, 19, 11, 3,
        61, 53, 45, 37, 29, 21, 13, 5,
        63, 55, 47, 39, 31, 23, 15, 7
    };


static const uint8_t sgExpand[] =
    {
        32, 1,  2,  3,  4,  5,
         4, 5,  6,  7,  8,  9,
         8, 9,  10, 11, 12, 13,
        12, 13, 14, 15, 16, 17,
        16, 17, 18, 19, 20, 21,
        20, 21, 22, 23, 24, 25,
        24, 25, 26, 27, 28, 29,
        28, 29, 30, 31, 32, 1
    };

static const uint8_t sgSG[8][4][16] =
    {

      {
          { 14, 4,  13, 1, 2,  15, 11, 8,  3,  10, 6,  12, 5,  9,  0, 7  },
          { 0,  15, 7,  4, 14, 2,  13, 1,  10, 6,  12, 11, 9,  5,  3, 8  },
          { 4,  1,  14, 8, 13, 6,  2,  11, 15, 12, 9,  7,  3,  10, 5, 0  },
          { 15, 12, 8,  2, 4,  9,  1,  7,  5,  11, 3,  14, 10, 0,  6, 13 }
      },
      {
          { 15, 1,  8, 14, 6,  11, 3,  4,  9,  7, 2,  13,  12, 0, 5,  10 },
          { 3,  13, 4, 7,  15, 2,  8,  14, 12, 0, 1,  10,  6,  9, 11, 5 },
          { 0,  14, 7, 11, 10, 4,  13, 1,  5,  8, 12, 6,   9,  3, 2,  15 },
          { 13, 8, 10, 1,  3,  15, 4,  2,  11, 6, 7,  12,  0,  5, 14, 9 }
      },
      {
          { 10, 0,  9,  14, 6,  3, 15, 5,  1,  13, 12, 7,  11, 4,  2,  8  },
          { 13, 7,  0,  9,  3,  4, 6,  10, 2,  8,  5,  14, 12, 11, 15, 1  },
          { 13, 6,  4,  9,  8, 15, 3,  0,  11, 1,  2,  12, 5,  10, 14, 7  },
          { 1,  10, 13, 0,  6,  9, 8,  7,  4,  15, 14, 3,  11, 5,  2,  12 }
      },
      {
         { 7,  13, 14, 3,  0,  6,  9,  10, 1,  2, 8, 5,  11, 12, 4,  15 },
         { 13, 8,  11, 5,  6,  15, 0,  3,  4,  7, 2, 12, 1,  10, 14, 9  },
         { 10, 6,  9,  0,  12, 11, 7,  13, 15, 1, 3, 14, 5,  2,  8,  4  },
         { 3,  15, 0,  6,  10, 1,  13, 8,  9,  4, 5, 11, 12, 7,  2,  14 }
      },
      {
          { 2,  12, 4,  1,  7,  10, 11, 6,  8,  5,  3,  15, 13, 0, 14, 9  },
          { 14, 11, 2,  12, 4,  7,  13, 1,  5,  0,  15, 10, 3,  9, 8,  6  },
          { 4,  2,  1,  11, 10, 13, 7,  8,  15, 9,  12, 5,  6,  3, 0,  14 },
          { 11, 8,  12, 7,  1,  14, 2,  13, 6,  15, 0,  9,  10, 4, 5,  3  }
      },
      {
          { 12, 1,  10, 15, 9,  2, 6,  8,  0,  13, 3,  4,  14, 7,  5,  11 },
          { 10, 15, 4,  2,  7, 12, 9,  5,  6,  1,  13, 14, 0,  11, 3,  8  },
          { 9,  14, 15, 5,  2,  8, 12, 3,  7,  0,  4,  10, 1,  13, 11, 6  },
          { 4,  3,  2,  12, 9,  5, 15, 10, 11, 14, 1,  7,  6,  0,  8,  13 }
      },
      {
          { 4,  11, 2,  14, 15, 0, 8,  13,  3, 12, 9,  7, 5,  10, 6,  1 },
          { 13, 0,  11, 7,  4,  9, 1,  10, 14,  3, 5, 12, 2,  15, 8,  6 },
          { 1,  4,  11, 13, 12, 3, 7,  14, 10, 15, 6,  8, 0,  5,  9,  2 },
          { 6,  11, 13, 8,  1,  4, 10, 7,   9,  5, 0, 15, 14, 2,  3, 12 }
      },
      {
          { 13, 2,  8,  4, 6,  15, 11, 1,  10, 9,  3,  14,  5,  0,  12, 7  },
          { 1,  15, 13, 8, 10, 3,  7,  4,  12, 5,  6,  11,  0,  14, 9,  2  },
          { 7,  11, 4,  1, 9,  12, 14, 2,  0,  6,  10, 13,  15, 3,  5,  8  },
          { 2,  1,  14, 7, 4,  10, 8,  13, 15, 12, 9,  0,   3,  5,  6,  11 }
      }
    };

static const uint8_t sgSP[] =
    {
        16,   7, 20,  21,
        29,  12, 28,  17,
         1,  15, 23,  26,
         5,  18, 31,  10,
         2,   8, 24,  14,
        32,  27, 3,   9,
        19,  13, 30,  6,
        22,  11, 4,  25
    };

static const uint8_t sgIPInv[] =
    {
        40, 8, 48, 16, 56, 24, 64, 32,
        39, 7, 47, 15, 55, 23, 63, 31,
        38, 6, 46, 14, 54, 22, 62, 30,
        37, 5, 45, 13, 53, 21, 61, 29,
        36, 4, 44, 12, 52, 20, 60, 28,
        35, 3, 43, 11, 51, 19, 59, 27,
        34, 2, 42, 10, 50, 18, 58, 26,
        33, 1, 41, 9,  49, 17, 57, 25
    };



static uint64_t
permute(const uint64_t val,
         const uint_t   valBits,
         const uint8_t* perm,
         const uint_t   permCount)
{
  uint64_t result = 0, b;

  for (b = 0;  b < permCount; ++b) {
    result <<= 1;
    result |= ((val & (1ull << (valBits - perm[b]))) != 0);
  }

  return result;
}


static uint32_t
rotate_left(uint32_t value, const uint_t cnt)
{
  assert((value & 0xF0000000) == 0);

  const uint32_t hiBits = value >> (28 - cnt);
  value <<= cnt;
  value |= hiBits;

  return value & 0x0FFFFFFF;
}


static uint32_t
mangle(const uint64_t k, const uint32_t r)
{
  uint32_t result = 0;
  uint64_t nr = k ^ permute(r, 32, sgExpand, sizeof sgExpand);

  int b;
  for (b = 7; b >= 0; --b)
    {
      const uint8_t row = (nr & 1) | ((nr >> 4) & 0x02);
      const uint8_t col = (nr >> 1) & 0x0F;

      result |= ((uint32_t)sgSG[b][row][col] << ( 4 * (7 - b)));
      nr >>= 6;
    }

  return permute(result, 32, sgSP, sizeof sgSP);
}


static uint64_t
encode_buffer(const uint64_t* const k, uint64_t m)
{
  uint32_t l, r, i;

  m = permute(m, 64, sgIP, sizeof sgIP);
  r = m & 0xFFFFFFFF;
  l = (m >> 32) & 0xFFFFFFFF;

  for (i = 0; i < 16; ++i)
    {
      for (i = 0; i < 16; ++i)
        {
          const uint32_t nl = r;
          const uint32_t nr = l ^ mangle(k[i], r);

          l = nl, r = nr;
        }
    }

  m = r;
  m <<= 32;
  m |= l;

  return permute(m, 64, sgIPInv, sizeof sgIPInv);
}


static uint64_t
decode_buffer(const uint64_t* const k, uint64_t m)
{
  uint32_t l, r;
  int   i;

  m = permute(m, 64, sgIP, sizeof sgIP);
  l = m & 0xFFFFFFFF;
  r = (m >> 32) & 0xFFFFFFFF;

  for (i = 15; i >= 0; --i)
    {
      const uint32_t nr = l;
      const uint32_t nl = r ^ mangle(k[i], l);

      l = nl, r = nr;
    }

  m = l;
  m <<= 32;
  m |= r;

  return permute(m, 64, sgIPInv, sizeof sgIPInv);
}


void
wh_buff_des_encode(const uint8_t* const     key,
                    uint8_t* const           buffer,
                    const uint_t             bufferSize)
{
  assert(bufferSize % sizeof(uint64_t) == 0);

  uint64_t k[16];

  wh_prepare_des_keys(key, strlen((char*)key), FALSE, k);
  wh_buff_des_encode_ex(k, buffer, bufferSize);
}


void
wh_buff_des_decode(const uint8_t* const     key,
                    uint8_t* const           buffer,
                    const uint_t             bufferSize)
{
  assert(bufferSize % sizeof(uint64_t) == 0);

  uint64_t k[16];

  wh_prepare_des_keys(key, strlen((char*)key), FALSE, k);
  wh_buff_des_decode_ex(k, buffer, bufferSize);
}


void
wh_buff_3des_encode(const uint8_t* const     key,
                     uint8_t* const           buffer,
                     const uint_t             bufferSize)
{
  wh_buff_des_encode(key, buffer, bufferSize);
  wh_buff_des_decode(key + sizeof(uint64_t), buffer, bufferSize);
  wh_buff_des_encode(key + 2 * sizeof(uint64_t), buffer, bufferSize);
}


void
wh_buff_3des_decode(const uint8_t* const     key,
                     uint8_t* const           buffer,
                     const uint_t             bufferSize)
{
  wh_buff_des_decode(key + 2 * sizeof(uint64_t), buffer, bufferSize);
  wh_buff_des_encode(key + sizeof(uint64_t), buffer, bufferSize);
  wh_buff_des_decode(key, buffer, bufferSize);
}


void
wh_prepare_des_keys(const uint8_t*   userKey,
                     const uint_t     keyLenght,
                     const bool_t     _3des,
                     uint64_t* const  outPreparedKeys)
{
  uint64_t c, d, key;
  uint_t   i;

  uint8_t normalizedKey[3 * sizeof(uint64_t)] = {0, };
  memcpy(normalizedKey, userKey, MIN(sizeof normalizedKey, keyLenght));

  key = load_ge_int64(normalizedKey + 0 * sizeof(uint64_t));
  c = permute(key, 64, sgPC1, sizeof sgPC1);
  d = c & 0x0FFFFFFF;
  c >>= 28;

  for (i = 0; i < 16; ++i)
    {
      c = rotate_left(c, sgLeftShifts[i % 16]);
      d = rotate_left(d, sgLeftShifts[i % 16]);

      outPreparedKeys[i] = permute((c << 28) | d, 56, sgPC2, sizeof sgPC2);
    }

  if (! _3des)
    return ;

  key = load_ge_int64(normalizedKey + 1 * sizeof(uint64_t));
  c = permute(key, 64, sgPC1, sizeof sgPC1);
  d = c & 0x0FFFFFFF;
  c >>= 28;

  for (i = 16; i < 32; ++i)
    {
      c = rotate_left(c, sgLeftShifts[i % 16]);
      d = rotate_left(d, sgLeftShifts[i % 16]);

      outPreparedKeys[i] = permute((c << 28) | d, 56, sgPC2, sizeof sgPC2);
    }

  key = load_ge_int64(normalizedKey + 2 * sizeof(uint64_t));
  c = permute(key, 64, sgPC1, sizeof sgPC1);
  d = c & 0x0FFFFFFF;
  c >>= 28;

  for (i = 32; i < 48; ++i)
    {
      c = rotate_left(c, sgLeftShifts[i % 16]);
      d = rotate_left(d, sgLeftShifts[i % 16]);

      outPreparedKeys[i] = permute((c << 28) | d, 56, sgPC2, sizeof sgPC2);
    }
}


void
wh_buff_des_encode_ex(const uint64_t* const    preparedKeys,
                       uint8_t* const           buffer,
                       const uint_t             bufferSize)
{
  uint_t offset;

  assert(bufferSize % sizeof(uint64_t) == 0);

  for (offset = 0; offset < bufferSize; offset += sizeof(uint64_t))
    {
      uint64_t m = load_ge_int64(buffer + offset);
      m = encode_buffer(preparedKeys, m);
      store_ge_int64(m, buffer + offset);
    }
}


void
wh_buff_des_decode_ex(const uint64_t* const    preparedKeys,
                       uint8_t* const           buffer,
                       const uint_t             bufferSize)
{
  uint_t offset;

  assert(bufferSize % sizeof(uint64_t) == 0);

  for (offset = 0; offset < bufferSize; offset += sizeof(uint64_t))
    {
      uint64_t m = load_ge_int64(buffer + offset);
      m = decode_buffer(preparedKeys, m);
      store_ge_int64(m, buffer + offset);
    }
}


void
wh_buff_3des_encode_ex(const uint64_t* const    preparedKeys,
                        uint8_t* const           buffer,
                        const uint_t             bufferSize)
{
  wh_buff_des_encode_ex(preparedKeys, buffer, bufferSize);
  wh_buff_des_decode_ex(preparedKeys + 16, buffer, bufferSize);
  wh_buff_des_encode_ex(preparedKeys + 32, buffer, bufferSize);
}


void
wh_buff_3des_decode_ex(const uint64_t* const    preparedKeys,
                        uint8_t* const           buffer,
                        const uint_t             bufferSize)
{
  wh_buff_des_decode_ex(preparedKeys + 32, buffer, bufferSize);
  wh_buff_des_encode_ex(preparedKeys + 16, buffer, bufferSize);
  wh_buff_des_decode_ex(preparedKeys, buffer, bufferSize);
}
