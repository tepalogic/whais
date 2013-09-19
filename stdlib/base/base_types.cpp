/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whisper.
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

#include <assert.h>

#include "whisper.h"

#include "stdlib/interface.h"
#include "utils/wtypes.h"



using namespace whisper;


uint8_t gBoolType[sizeof (TypeSpec)];

uint8_t gCharType[sizeof (TypeSpec)];

uint8_t gDateType[sizeof (TypeSpec)];
uint8_t gDateTimeType[sizeof (TypeSpec)];
uint8_t gHiresTimeType[sizeof (TypeSpec)];

uint8_t gUInt8Type[sizeof (TypeSpec)];
uint8_t gUInt16Type[sizeof (TypeSpec)];
uint8_t gUInt32Type[sizeof (TypeSpec)];
uint8_t gUInt64Type[sizeof (TypeSpec)];

uint8_t gInt8Type[sizeof (TypeSpec)];
uint8_t gInt16Type[sizeof (TypeSpec)];
uint8_t gInt32Type[sizeof (TypeSpec)];
uint8_t gInt64Type[sizeof (TypeSpec)];

uint8_t gRealType[sizeof (TypeSpec)];
uint8_t gRichRealType[sizeof (TypeSpec)];

uint8_t gTextType[sizeof (TypeSpec)];

uint8_t gGenericArrayType[sizeof (TypeSpec)];
uint8_t gGenericFieldType[sizeof (TypeSpec)];
uint8_t gGenericTableType[sizeof (TypeSpec)];


uint8_t gABoolType[sizeof (TypeSpec)];

uint8_t gACharType[sizeof (TypeSpec)];

uint8_t gADateType[sizeof (TypeSpec)];
uint8_t gADateTimeType[sizeof (TypeSpec)];
uint8_t gAHiresTimeType[sizeof (TypeSpec)];

uint8_t gAUInt8Type[sizeof (TypeSpec)];
uint8_t gAUInt16Type[sizeof (TypeSpec)];
uint8_t gAUInt32Type[sizeof (TypeSpec)];
uint8_t gAUInt64Type[sizeof (TypeSpec)];

uint8_t gAInt8Type[sizeof (TypeSpec)];
uint8_t gAInt16Type[sizeof (TypeSpec)];
uint8_t gAInt32Type[sizeof (TypeSpec)];
uint8_t gAInt64Type[sizeof (TypeSpec)];

uint8_t gARealType[sizeof (TypeSpec)];
uint8_t gARichRealType[sizeof (TypeSpec)];



WLIB_STATUS
base_types_init ()
{
  if ((wh_define_basic_type (T_BOOL, _RC (TypeSpec*, gBoolType)) <= 0)
      || (wh_define_basic_type (T_CHAR, _RC (TypeSpec*, gCharType)) <= 0)
      || (wh_define_basic_type (T_DATE, _RC (TypeSpec*, gDateType)) <= 0)
      || (wh_define_basic_type (T_DATETIME,
                                _RC (TypeSpec*, gDateTimeType)) <= 0)
      || (wh_define_basic_type (T_HIRESTIME,
                                _RC (TypeSpec*, gHiresTimeType)) <= 0)
      || (wh_define_basic_type (T_INT8, _RC (TypeSpec*, gInt8Type)) <= 0)
      || (wh_define_basic_type (T_INT16, _RC (TypeSpec*, gInt16Type)) <= 0)
      || (wh_define_basic_type (T_INT32, _RC (TypeSpec*, gInt32Type)) <= 0)
      || (wh_define_basic_type (T_INT64, _RC (TypeSpec*, gInt64Type)) <= 0)

      || (wh_define_basic_type (T_UINT8, _RC (TypeSpec*, gUInt8Type)) <= 0)
      || (wh_define_basic_type (T_UINT16, _RC (TypeSpec*, gUInt16Type)) <= 0)
      || (wh_define_basic_type (T_UINT32, _RC (TypeSpec*, gUInt32Type)) <= 0)
      || (wh_define_basic_type (T_UINT64, _RC (TypeSpec*, gUInt64Type)) <= 0)

      || (wh_define_basic_type (T_REAL, _RC (TypeSpec*, gRealType)) <= 0)
      || (wh_define_basic_type (T_RICHREAL,
                                _RC (TypeSpec*, gRichRealType)) <= 0)
      || (wh_define_basic_type (T_TEXT, _RC (TypeSpec*, gTextType)) <= 0)

      || (wh_define_basic_type (T_UNDETERMINED,
                                _RC (TypeSpec*, gGenericArrayType)) <= 0)
      || (wh_define_basic_type (T_UNDETERMINED,
                                _RC (TypeSpec*, gGenericFieldType)) <= 0)
      || (wh_define_basic_type (T_UNDETERMINED,
                                _RC (TypeSpec*, gGenericTableType)) <= 0))
    {
      return WOP_UNKNOW;
    }

  memcpy (gABoolType, gBoolType, sizeof gBoolType);
  memcpy (gACharType, gCharType, sizeof gCharType);
  memcpy (gADateType, gDateType, sizeof gDateType);
  memcpy (gADateTimeType, gDateTimeType, sizeof gDateTimeType);
  memcpy (gAHiresTimeType, gHiresTimeType, sizeof gHiresTimeType);
  memcpy (gAInt8Type, gInt8Type, sizeof gInt8Type);
  memcpy (gAInt16Type, gInt16Type, sizeof gInt16Type);
  memcpy (gAInt32Type, gInt32Type, sizeof gInt32Type);
  memcpy (gAInt64Type, gInt64Type, sizeof gInt64Type);
  memcpy (gAUInt8Type, gUInt8Type, sizeof gUInt8Type);
  memcpy (gAUInt16Type, gUInt16Type, sizeof gUInt16Type);
  memcpy (gAUInt32Type, gUInt32Type, sizeof gUInt32Type);
  memcpy (gAUInt64Type, gUInt64Type, sizeof gUInt64Type);
  memcpy (gARealType, gRealType, sizeof gRealType);
  memcpy (gARichRealType, gRichRealType, sizeof gRichRealType);

  if ((wh_apply_array_modifier (_RC (TypeSpec*, gABoolType)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gACharType)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gADateType)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gADateTimeType)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAHiresTimeType)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAInt8Type)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAInt16Type)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAInt32Type)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAInt64Type)) <= 0)

      || (wh_apply_array_modifier (_RC (TypeSpec*, gAUInt8Type)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAUInt16Type)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAUInt32Type)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gAUInt64Type)) <= 0)

      || (wh_apply_array_modifier (_RC (TypeSpec*, gARealType)) <= 0)
      || (wh_apply_array_modifier (_RC (TypeSpec*, gARichRealType)) <= 0))
    {
      return WOP_UNKNOW;
    }
  return WOP_OK;
}


