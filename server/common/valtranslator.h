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
#ifndef VALTRANSLATOR_H_
#define VALTRANSLATOR_H_

#include "whisper.h"

#include "dbs/include/dbs_values.h"

class Utf8Translator
{
public:
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSBool* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSChar* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSDate* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSDateTime* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSHiresTime* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSInt8* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSInt16* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSInt32* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSInt64* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSReal* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSRichReal* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSUInt8* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSUInt16* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSUInt32* pValue);
  static D_UINT Read (const D_UINT8* utf8Src, const D_UINT srcSize, DBSUInt64* pValue);

  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSBool& value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSChar& value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSDate& value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSDateTime& value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSHiresTime& value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSInt8 &value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSInt16 &value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSInt32 &value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSInt64 &value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSReal& value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSRichReal& value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSUInt8 &value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSUInt16 &value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSUInt32 &value);
  static D_UINT Write (D_UINT8* const utf8Dest, const D_UINT maxSize, const DBSUInt64 &value);
};

#endif /* VALTRANSLATOR_H_ */
