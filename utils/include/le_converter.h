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

#ifdef __cplusplus
#undef INLINE
#define INLINE inline
#endif

static INLINE void
to_le_int16 (D_UINT8* pValue)
{
  D_UINT8        temp;
  const D_UINT16 le_machine = 0x1;

  if (*((D_UINT8 *) & le_machine))
    return;     /* no need for conversion */

  temp      = pValue[0];
  pValue[0] = pValue[1];
  pValue[1] = temp;
}

static INLINE void
to_le_int32 (D_UINT8* pValue)
{
  D_UINT8        temp;
  const D_UINT16 le_machine = 0x1;

  if (*((D_UINT8 *) & le_machine))
    return;     /* no need for conversion */

  temp      = pValue[0];
  pValue[0] = pValue[3];
  pValue[3] = temp;
  temp      = pValue[1];
  pValue[1] = pValue[2];
  pValue[2] = temp;
}

static INLINE void
to_le_int64 (D_UINT8* pValue)
{
  D_UINT8        temp;
  const D_UINT16 le_machine = 0x1;


  if (*((D_UINT8 *) & le_machine))
    return;     /* no need for conversion */

  temp      = pValue[0];
  pValue[0] = pValue[7];
  pValue[7] = temp;
  temp      = pValue[1];
  pValue[1] = pValue[6];
  pValue[6] = temp;
  temp      = pValue[2];
  pValue[2] = pValue[5];
  pValue[5] = temp;
  temp      = pValue[3];
  pValue[3] = pValue[4];
  pValue[4] = temp;
}

static INLINE D_UINT16
from_le_int16 (const D_UINT8* pValue)
{
  D_UINT16 result;

  result = pValue[1];
  result <<= 8;
  result += pValue[0];
  return result;
}

static INLINE D_UINT32
from_le_int32 (const D_UINT8* pValue)
{
  D_UINT32 result;

  result = pValue[3];
  result <<= 8;
  result += pValue[2];
  result <<= 8;
  result += pValue[1];
  result <<= 8;
  result += pValue[0];

  return result;
}

static INLINE D_UINT64
from_le_int64 (const D_UINT8* pValue)
{
  D_UINT64 result;

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

#endif /* LE_CONVERTER_H_ */
