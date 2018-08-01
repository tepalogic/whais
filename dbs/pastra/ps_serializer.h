/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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

#ifndef PS_SERIALIZER_H_
#define PS_SERIALIZER_H_

#include <cstring>

#include "dbs/dbs_values.h"
#include "utils/endianness.h"

namespace whais {
namespace pastra {

typedef uint32_t NODE_INDEX;

class Serializer
{
  Serializer() = delete;
  ~Serializer() = delete;

public:
  using VALUE_VALIDATOR = bool (*)(const uint8_t* const);

  static void Store(uint8_t* const dest, const DBool& value);
  static void Store(uint8_t* const dest, const DChar& value);
  static void Store(uint8_t* const dest, const DDate& value);
  static void Store(uint8_t* const dest, const DDateTime& value);
  static void Store(uint8_t* const dest, const DHiresTime& value);
  static void Store(uint8_t* const dest, const DInt8& value);
  static void Store(uint8_t* const dest, const DInt16& value);
  static void Store(uint8_t* const dest, const DInt32& value);
  static void Store(uint8_t* const dest, const DInt64& value);
  static void Store(uint8_t* const dest, const DReal& value);
  static void Store(uint8_t* const dest, const DRichReal& value);
  static void Store(uint8_t* const dest, const DUInt8& value);
  static void Store(uint8_t* const dest, const DUInt16& value);
  static void Store(uint8_t* const dest, const DUInt32& value);
  static void Store(uint8_t* const dest, const DUInt64& value);

  static void Load(const uint8_t* const src, DBool* const outValue);
  static void Load(const uint8_t* const src, DChar* const outValue);
  static void Load(const uint8_t* const src, DDate* const outValue);
  static void Load(const uint8_t* const src, DDateTime* const outValue);
  static void Load(const uint8_t* const src, DHiresTime* const outValue);
  static void Load(const uint8_t* const src, DInt8* const outValue);
  static void Load(const uint8_t* const src, DInt16* const outValue);
  static void Load(const uint8_t* const src, DInt32* const outValue);
  static void Load(const uint8_t* const src, DInt64* const outValue);
  static void Load(const uint8_t* const src, DReal* const outValue);
  static void Load(const uint8_t* const src, DRichReal* const outValue);
  static void Load(const uint8_t* const src, DUInt8* const outValue);
  static void Load(const uint8_t* const src, DUInt16* const outValue);
  static void Load(const uint8_t* const src, DUInt32* const outValue);
  static void Load(const uint8_t* const src, DUInt64* const outValue);

  static uint_t Size(const DBS_FIELD_TYPE type, const bool isArray);

  static ROW_INDEX LoadRow(const ROW_INDEX* const from)
  {
    if (sizeof(ROW_INDEX) == sizeof(uint64_t))
      return load_le_int64(_RC(const uint8_t*, from));

    else if (sizeof(ROW_INDEX) == sizeof(uint32_t))
      return load_le_int32(_RC(const uint8_t*, from));

    assert(false);
    return 0;
  }

  static void StoreRow(const ROW_INDEX row, ROW_INDEX* const to)
  {
    if (sizeof(ROW_INDEX) == sizeof(uint64_t))
      store_le_int64(row, _RC(uint8_t*, to));

    else if (sizeof(ROW_INDEX) == sizeof(uint32_t))
      store_le_int32(row, _RC(uint8_t*, to));

    else
    {
      assert(false);
    }
  }

  static NODE_INDEX LoadNode(const NODE_INDEX* const from)
  {
    assert(sizeof(NODE_INDEX) == sizeof(uint32_t));

    return load_le_int32(_RC(const uint8_t*, from));
  }

  static void StoreNode(const NODE_INDEX node, NODE_INDEX* const to)
  {
    assert(sizeof(NODE_INDEX) == sizeof(uint32_t));

    store_le_int32(node, _RC(uint8_t*, to));
  }

  static bool ValidateDBoolBuffer(const uint8_t* const buffer)
  {
    return(buffer[0] == 0) || (buffer[0] == 1);
  }


  static bool ValidateDCharBuffer(const uint8_t* const buffer)
  {
    try
    {
        DChar(load_le_int32(buffer));
    }
    catch(...)
    {
        return false;
    }

    return true;
  }


  static bool ValidateDDateBuffer(const uint8_t* const buffer)
  {
    try
    {
      const int16_t year = load_le_int16(buffer);
      const uint8_t month = buffer[2];
      const uint8_t day = buffer[3];

      DDate(year, month, day);
    }
    catch(...)
    {
        return false;
    }
    return true;
  }


  static bool ValidateDDateTimeBuffer(const uint8_t* const buffer)
  {
    try
    {
      const int16_t year = load_le_int16(buffer);
      const uint8_t month = buffer[2];
      const uint8_t day = buffer[3];
      const uint8_t hours = buffer[4];
      const uint8_t mins = buffer[5];
      const uint8_t secs = buffer[6];

      DDateTime(year, month, day, hours, mins, secs);
    }
    catch(...)
    {
        return false;
    }
    return true;
  }


  static bool ValidateDHiresTimeBuffer(const uint8_t* const buffer)
  {
    try
    {
      const int32_t usecs = load_le_int32(buffer);
      const int16_t year = load_le_int16(buffer + sizeof(uint32_t));
      const uint8_t month = buffer[6];
      const uint8_t day = buffer[7];
      const uint8_t hours = buffer[8];
      const uint8_t mins = buffer[9];
      const uint8_t secs = buffer[10];

      DHiresTime(year, month, day, hours, mins, secs, usecs);
    }
    catch(...)
    {
        return false;
    }
    return true;
  }


  static bool ValidateDRealBuffer(const uint8_t* const buffer)
  {
    const uint_t integerSize = 5;
    const uint_t fractionalSize = 3;

    int64_t temp = 0;
    memcpy(&temp, buffer, integerSize);

    int64_t integer = load_le_int64(_RC(const uint8_t*, &temp));

    if (integer & 0x8000000000)
      integer |= ~_SC(int64_t, 0xFFFFFFFFFF);

    temp = 0;
    memcpy(&temp, buffer + integerSize, fractionalSize);

    int64_t fractional = load_le_int64(_RC(const uint8_t*, &temp));

    if (fractional & 0x800000)
      fractional |= ~_SC(int64_t, 0xFFFFFF);

    if (integer < 0
        && (fractional <= -DBS_REAL_PREC || 0 < fractional))
    {
      return false;
    }
    else if (integer > 0
             && (fractional < 0 || DBS_REAL_PREC <= fractional))
    {
      return false;
    }

    return true;
  }


  static bool ValidateDRichRealBuffer(const uint8_t* const buffer)
  {
    const uint_t integerSize = 8;
    const uint_t fractionalSize = 6;

    int64_t integer = load_le_int64(buffer);

    int64_t temp = 0;
    memcpy(&temp, buffer + integerSize, fractionalSize);

    int64_t fractional = load_le_int64(_RC(const uint8_t*, &temp));

    if (fractional & 0x800000000000)
      fractional |= ~_SC(int64_t, 0xFFFFFFFFFFFF);

    if (integer < 0
        && (fractional <= -DBS_RICHREAL_PREC || 0 < fractional))
    {
      return false;
    }
    else if (integer > 0
            && (fractional < 0 || DBS_RICHREAL_PREC <= fractional))
    {
      return false;
    }

    assert((fractional < 0) || (fractional < DBS_RICHREAL_PREC));
    assert((fractional > 0) || (fractional > -DBS_RICHREAL_PREC));

    return true;
  }


  static VALUE_VALIDATOR SelectValidator(const DBS_FIELD_TYPE type);

  static const int MAX_VALUE_RAW_SIZE = 0x20;
};






} //namespace pastra
} //namespace whais

#endif /* PS_SERIALIZER_H_ */

