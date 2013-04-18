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

#include "dbs/dbs_values.h"

class Utf8Translator
{
public:
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSBool* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSChar* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSDate* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSDateTime* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSHiresTime* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSInt8* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSInt16* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSInt32* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSInt64* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSReal* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSRichReal* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSUInt8* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSUInt16* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSUInt32* pValue);
  static uint_t Read (const uint8_t* utf8Src, const uint_t srcSize, whisper::DBSUInt64* pValue);

  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSBool& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSChar& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSDate& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSDateTime& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSHiresTime& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSInt8 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSInt16 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSInt32 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSInt64 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSReal& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSRichReal& value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSUInt8 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSUInt16 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSUInt32 &value);
  static uint_t Write (uint8_t* const utf8Dest, const uint_t maxSize, const whisper::DBSUInt64 &value);
};

#endif /* VALTRANSLATOR_H_ */
