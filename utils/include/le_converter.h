/******************************************************************************
WHISPERC - A compiler for whisper programs
Copyright (C) 2009  Iulian Popa

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

#ifndef LE_CONVERTER_H_
#define LE_CONVERTER_H_

#include "whisper.h"

#ifdef __cplusplus
#undef INLINE
#define INLINE inline
#endif

static INLINE uint16_t
load_le_int16 (const uint8_t* from)
{
  uint16_t result;

  result = from[1];
  result <<= 8; result |= from[0];

  return result;
}


static INLINE uint32_t
load_le_int32 (const uint8_t* from)
{
  uint32_t result;

  result = from[3];
  result <<= 8; result |= from[2];
  result <<= 8; result |= from[1];
  result <<= 8; result |= from[0];

  return result;
}


static INLINE uint64_t
load_le_int64 (const uint8_t* from)
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
store_le_int16 (const uint16_t value, uint8_t* const to)
{
  const uint8_t* from = (const uint8_t*)&value;
  const uint16_t le_machine = 0x1;

    if (*((uint8_t*)&le_machine))
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          to[i] = from[i];
      }
    else
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          to[i] = from[sizeof (value) - (i + 1)];
      }
}


static INLINE void
store_le_int32 (const uint32_t value, uint8_t* const to)
{
  const uint8_t* from = (const uint8_t*)&value;
  const uint16_t le_machine = 0x1;

    if (*((uint8_t*)&le_machine))
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          to[i] = from[i];
      }
    else
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          to[i] = from[sizeof (value) - (i + 1)];
      }
}


static INLINE void
store_le_int64 (const uint64_t value, uint8_t* const to)
{
  const uint8_t* from = (const uint8_t*)&value;
  const uint16_t le_machine = 0x1;

    if (*((uint8_t*)&le_machine))
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          to[i] = from[i];
      }
    else
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          to[i] = from[sizeof (value) - (i + 1)];
      }
}


#endif /* LE_CONVERTER_H_ */

