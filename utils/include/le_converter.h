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
to_le_int16 (D_UINT8 * value)
{
  const D_UINT16 le_machine = 0x1;
  D_UINT8 temp;

  if (*((D_UINT8 *) & le_machine))
    return;			/* no need for conversion */

  temp = value[0];
  value[0] = value[1];
  value[1] = temp;
}

static INLINE void
to_le_int32 (D_UINT8 * value)
{
  const D_UINT16 le_machine = 0x1;
  D_UINT8 temp;

  if (*((D_UINT8 *) & le_machine))
    return;			/* no need for conversion */

  temp = value[0];
  value[0] = value[3];
  value[3] = temp;
  temp = value[1];
  value[1] = value[2];
  value[2] = temp;
}

static INLINE void
to_le_int64 (D_UINT8 * value)
{
  const D_UINT16 le_machine = 0x1;
  D_UINT8 temp;

  if (*((D_UINT8 *) & le_machine))
    return;			/* no need for conversion */

  temp = value[0];
  value[0] = value[7];
  value[7] = temp;
  temp = value[1];
  value[1] = value[6];
  value[6] = temp;
  temp = value[2];
  value[2] = value[5];
  value[5] = temp;
  temp = value[3];
  value[3] = value[4];
  value[4] = temp;
}

static INLINE D_UINT16
from_le_int16 (const D_UINT8 * value)
{

  D_UINT16 result;

  result = value[1];
  result <<= 8;
  result += value[0];
  return result;
}

static INLINE D_UINT32
from_le_int32 (const D_UINT8 * value)
{

  D_UINT32 result;

  result = value[3];
  result <<= 8;
  result += value[2];
  result <<= 8;
  result += value[1];
  result <<= 8;
  result += value[0];
  return result;
}

static INLINE D_UINT64
from_le_int64 (const D_UINT8 * value)
{
  D_UINT64 result;

  result = value[7];
  result <<= 8;
  result += value[6];
  result <<= 8;
  result += value[5];
  result <<= 8;
  result += value[4];
  result <<= 8;
  result += value[3];
  result <<= 8;
  result += value[2];
  result <<= 8;
  result += value[1];
  result <<= 8;
  result += value[0];
  return result;
}

#endif /* LE_CONVERTER_H_ */
