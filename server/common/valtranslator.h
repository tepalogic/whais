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
  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DBool* const      outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DChar* const      outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DDate* const      outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DDateTime* const  outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DHiresTime* const outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DInt8* const      outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DInt16* const     outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DInt32* const     outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DInt64* const     outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DReal* const      outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DRichReal* const  outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DUInt8* const     outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DUInt16* const    outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DUInt32* const    outValue);

  static uint_t Read (const uint8_t* const       utf8Src,
                      const uint_t               srcSize,
                      whisper::DUInt64* const    outValue);


  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DBool&      value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DChar&      value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DDate&      value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DDateTime&  value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DHiresTime& value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DInt8&      value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DInt16&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DInt32&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DInt64&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DReal&      value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DRichReal&  value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DUInt8&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DUInt16&    value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DUInt32&    value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whisper::DUInt64&    value);
};

#endif /* VALTRANSLATOR_H_ */

