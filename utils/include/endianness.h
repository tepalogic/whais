/******************************************************************************
WHAISC - A compiler for whais programs
Copyright(C) 2009  Iulian Popa

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

#ifndef ENDIANNESS_H_
#define ENDIANNESS_H_


#include "whais.h"


#ifdef __cplusplus
#undef INLINE
#define INLINE inline
#endif

static INLINE uint16_t
load_le_int16(const uint8_t* from)
{
  uint16_t result;

  result = from[1];
  result <<= 8; result |= from[0];

  return result;
}


static INLINE uint32_t
load_le_int32(const uint8_t* from)
{
  uint32_t result;

  result = from[3];
  result <<= 8; result |= from[2];
  result <<= 8; result |= from[1];
  result <<= 8; result |= from[0];

  return result;
}


static INLINE uint64_t
load_le_int64(const uint8_t* from)
{
  uint64_t result;

  result = from[7];
  result <<= 8; result |= from[6];
  result <<= 8; result |= from[5];
  result <<= 8; result |= from[4];
  result <<= 8; result |= from[3];
  result <<= 8; result |= from[2];
  result <<= 8; result |= from[1];
  result <<= 8; result |= from[0];

  return result;
}

static INLINE void
store_le_int16(const uint16_t value, uint8_t* const to)
{
  to[0] = value        & 0xFF;
  to[1] = (value >> 8) & 0xFF;
}

static INLINE void
store_le_int32(const uint32_t value, uint8_t* const to)
{
  to[0] = value         & 0xFF;
  to[1] = (value >>  8) & 0xFF;
  to[2] = (value >> 16) & 0xFF;
  to[3] = (value >> 24) & 0xFF;
}

static INLINE void
store_le_int64(const uint64_t value, uint8_t* const to)
{
  to[0] = value         & 0xFF;
  to[1] = (value >>  8) & 0xFF;
  to[2] = (value >> 16) & 0xFF;
  to[3] = (value >> 24) & 0xFF;
  to[4] = (value >> 32) & 0xFF;
  to[5] = (value >> 40) & 0xFF;
  to[6] = (value >> 48) & 0xFF;
  to[7] = (value >> 56) & 0xFF;
}

static INLINE uint16_t
load_ge_int16(const uint8_t* from)
{
  uint16_t result;

  result = from[0];
  result <<= 8; result |= from[1];

  return result;
}

static INLINE uint32_t
load_ge_int32(const uint8_t* from)
{
  uint32_t result;

  result = from[0];
  result <<= 8; result |= from[1];
  result <<= 8; result |= from[2];
  result <<= 8; result |= from[3];

  return result;
}

static INLINE uint64_t
load_ge_int64(const uint8_t* from)
{
  uint64_t result;

  result = from[0];
  result <<= 8; result |= from[1];
  result <<= 8; result |= from[2];
  result <<= 8; result |= from[3];
  result <<= 8; result |= from[4];
  result <<= 8; result |= from[5];
  result <<= 8; result |= from[6];
  result <<= 8; result |= from[7];

  return result;
}

static INLINE void
store_ge_int16(const uint16_t value, uint8_t* const to)
{
  to[1] = value        & 0xFF;
  to[0] = (value >> 8) & 0xFF;
}

static INLINE void
store_ge_int32(const uint32_t value, uint8_t* const to)
{
  to[3] = value         & 0xFF;
  to[2] = (value >>  8) & 0xFF;
  to[1] = (value >> 16) & 0xFF;
  to[0] = (value >> 24) & 0xFF;
}

static INLINE void
store_ge_int64(const uint64_t value, uint8_t* const to)
{
  to[7] = value         & 0xFF;
  to[6] = (value >>  8) & 0xFF;
  to[5] = (value >> 16) & 0xFF;
  to[4] = (value >> 24) & 0xFF;
  to[3] = (value >> 32) & 0xFF;
  to[2] = (value >> 40) & 0xFF;
  to[1] = (value >> 48) & 0xFF;
  to[0] = (value >> 56) & 0xFF;
}

#endif /* ENDIANNESS_H_ */
