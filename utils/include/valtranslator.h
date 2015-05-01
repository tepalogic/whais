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

#include "whais.h"

#include "dbs/dbs_values.h"

class Utf8Translator
{
public:
  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DBool* const        outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   const bool                 checkSpecial,
                   whais::DChar* const        outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DDate* const        outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DDateTime* const    outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DHiresTime* const  outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DInt8* const        outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DInt16* const       outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DInt32* const       outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DInt64* const       outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DReal* const        outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DRichReal* const    outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DUInt8* const       outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DUInt16* const      outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DUInt32* const      outValue);

  static int Read (const uint8_t* const       utf8Src,
                   const uint_t               srcSize,
                   whais::DUInt64* const      outValue);


  static uint_t Write (uint8_t* const           utf8Dest,
                       const uint_t             maxSize,
                       const whais::DBool&      value);

  static uint_t Write (uint8_t* const           utf8Dest,
                       const uint_t             maxSize,
                       const bool               checkSpecial,
                       const whais::DChar&      value);

  static uint_t Write (uint8_t* const           utf8Dest,
                       const uint_t             maxSize,
                       const whais::DDate&      value);

  static uint_t Write (uint8_t* const           utf8Dest,
                       const uint_t             maxSize,
                       const whais::DDateTime&  value);

  static uint_t Write (uint8_t* const           utf8Dest,
                       const uint_t             maxSize,
                       const whais::DHiresTime& value);

  static uint_t Write (uint8_t* const           utf8Dest,
                       const uint_t             maxSize,
                       const whais::DInt8&      value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DInt16&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DInt32&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DInt64&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DReal&      value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DRichReal&  value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DUInt8&     value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DUInt16&    value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DUInt32&    value);

  static uint_t Write (uint8_t* const             utf8Dest,
                       const uint_t               maxSize,
                       const whais::DUInt64&    value);
};

#endif /* VALTRANSLATOR_H_ */

