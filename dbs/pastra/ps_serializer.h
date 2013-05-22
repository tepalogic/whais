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

#ifndef PS_SERIALIZER_H_
#define PS_SERIALIZER_H_

#include "dbs_values.h"

namespace whisper {
namespace pastra {

class Serializer
{
private:
  Serializer ();
  ~Serializer ();

public:
  static void Store (uint8_t* const dest, const DBool& value);
  static void Store (uint8_t* const dest, const DChar& value);
  static void Store (uint8_t* const dest, const DDate& value);
  static void Store (uint8_t* const dest, const DDateTime& value);
  static void Store (uint8_t* const dest, const DHiresTime& value);
  static void Store (uint8_t* const dest, const DInt8& value);
  static void Store (uint8_t* const dest, const DInt16& value);
  static void Store (uint8_t* const dest, const DInt32& value);
  static void Store (uint8_t* const dest, const DInt64& value);
  static void Store (uint8_t* const dest, const DReal& value);
  static void Store (uint8_t* const dest, const DRichReal& value);
  static void Store (uint8_t* const dest, const DUInt8& value);
  static void Store (uint8_t* const dest, const DUInt16& value);
  static void Store (uint8_t* const dest, const DUInt32& value);
  static void Store (uint8_t* const dest, const DUInt64& value);

  static void Load (const uint8_t* const src, DBool* const outValue);
  static void Load (const uint8_t* const src, DChar* const outValue);
  static void Load (const uint8_t* const src, DDate* const outValue);
  static void Load (const uint8_t* const src, DDateTime* const outValue);
  static void Load (const uint8_t* const src, DHiresTime* const outValue);
  static void Load (const uint8_t* const src, DInt8* const outValue);
  static void Load (const uint8_t* const src, DInt16* const outValue);
  static void Load (const uint8_t* const src, DInt32* const outValue);
  static void Load (const uint8_t* const src, DInt64* const outValue);
  static void Load (const uint8_t* const src, DReal* const outValue);
  static void Load (const uint8_t* const src, DRichReal* const outValue);
  static void Load (const uint8_t* const src, DUInt8* const outValue);
  static void Load (const uint8_t* const src, DUInt16* const outValue);
  static void Load (const uint8_t* const src, DUInt32* const outValue);
  static void Load (const uint8_t* const src, DUInt64* const outValue);

  static int Size (const DBS_FIELD_TYPE type, const bool isArray);

  static int Alignment (const DBS_FIELD_TYPE, const bool isArray);
};

} //namespace pastra
} //namespace whisper

#endif /* PS_SERIALIZER_H_ */

