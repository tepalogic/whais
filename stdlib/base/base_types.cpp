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

  return WOP_OK;
}


