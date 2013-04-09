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

static INLINE void
to_le_int16 (uint8_t* pInOutValue)
{
  uint8_t        temp;
  const uint16_t le_machine = 0x1;

  if (*((uint8_t *) & le_machine))
    return;     /* no need for conversion */

  temp           = pInOutValue[0];
  pInOutValue[0] = pInOutValue[1];
  pInOutValue[1] = temp;
}

static INLINE void
to_le_int32 (uint8_t* pInOutValue)
{
  uint8_t        temp;
  const uint16_t le_machine = 0x1;

  if (*((uint8_t *) & le_machine))
    return;     /* no need for conversion */

  temp           = pInOutValue[0];
  pInOutValue[0] = pInOutValue[3];
  pInOutValue[3] = temp;
  temp           = pInOutValue[1];
  pInOutValue[1] = pInOutValue[2];
  pInOutValue[2] = temp;
}

static INLINE void
to_le_int64 (uint8_t* pInOutValue)
{
  uint8_t        temp;
  const uint16_t le_machine = 0x1;


  if (*((uint8_t *) & le_machine))
    return;     /* no need for conversion */

  temp           = pInOutValue[0];
  pInOutValue[0] = pInOutValue[7];
  pInOutValue[7] = temp;
  temp           = pInOutValue[1];
  pInOutValue[1] = pInOutValue[6];
  pInOutValue[6] = temp;
  temp           = pInOutValue[2];
  pInOutValue[2] = pInOutValue[5];
  pInOutValue[5] = temp;
  temp           = pInOutValue[3];
  pInOutValue[3] = pInOutValue[4];
  pInOutValue[4] = temp;
}

static INLINE uint16_t
from_le_int16 (const uint8_t* pValue)
{
  uint16_t result;

  result = pValue[1];
  result <<= 8;
  result += pValue[0];
  return result;
}

static INLINE uint32_t
from_le_int32 (const uint8_t* pValue)
{
  uint32_t result;

  result = pValue[3];
  result <<= 8;
  result += pValue[2];
  result <<= 8;
  result += pValue[1];
  result <<= 8;
  result += pValue[0];

  return result;
}

static INLINE uint64_t
from_le_int64 (const uint8_t* pValue)
{
  uint64_t result;

  result = pValue[7];
  result <<= 8;
  result += pValue[6];
  result <<= 8;
  result += pValue[5];
  result <<= 8;
  result += pValue[4];
  result <<= 8;
  result += pValue[3];
  result <<= 8;
  result += pValue[2];
  result <<= 8;
  result += pValue[1];
  result <<= 8;
  result += pValue[0];

  return result;
}

static INLINE void
store_le_int16 (const uint16_t value, uint8_t* const pDest)
{
  const uint8_t* pSrc = (const uint8_t*)&value;
  const uint16_t le_machine = 0x1;

    if (*((uint8_t*)&le_machine))
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          pDest[i] = pSrc[i];
      }
    else
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          pDest[i] = pSrc[sizeof (value) - (i + 1)];
      }
}

static INLINE void
store_le_int32 (const uint32_t value, uint8_t* const pDest)
{
  const uint8_t* pSrc = (const uint8_t*)&value;
  const uint16_t le_machine = 0x1;

    if (*((uint8_t*)&le_machine))
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          pDest[i] = pSrc[i];
      }
    else
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          pDest[i] = pSrc[sizeof (value) - (i + 1)];
      }
}

static INLINE void
store_le_int64 (const uint64_t value, uint8_t* const pDest)
{
  const uint8_t* pSrc = (const uint8_t*)&value;
  const uint16_t le_machine = 0x1;

    if (*((uint8_t*)&le_machine))
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          pDest[i] = pSrc[i];
      }
    else
      {
        uint_t i;
        for (i = 0; i < sizeof (value); ++i)
          pDest[i] = pSrc[sizeof (value) - (i + 1)];
      }
}



#endif /* LE_CONVERTER_H_ */
