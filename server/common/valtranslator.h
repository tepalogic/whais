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
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSBool* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSChar* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSDate* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSDateTime* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSHiresTime* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSInt8* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSInt16* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSInt32* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSInt64* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSReal* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSRichReal* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSUInt8* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSUInt16* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSUInt32* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, DBSUInt64* pValue);

  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSBool& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSChar& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSDate& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSDateTime& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSHiresTime& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSInt8 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSInt16 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSInt32 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSInt64 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSReal& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSRichReal& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSUInt8 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSUInt16 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSUInt32 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const DBSUInt64 &value);
};

#endif /* VALTRANSLATOR_H_ */
