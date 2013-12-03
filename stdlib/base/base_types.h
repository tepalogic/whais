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

#ifndef BASE_TYPES_H_
#define BASE_TYPES_H_

#include "whisper.h"

#include "stdlib/interface.h"

extern const uint8_t gBoolType[sizeof (TypeSpec)];

extern const uint8_t gCharType[sizeof (TypeSpec)];

extern const uint8_t gDateType[sizeof (TypeSpec)];
extern const uint8_t gDateTimeType[sizeof (TypeSpec)];
extern const uint8_t gHiresTimeType[sizeof (TypeSpec)];

extern const uint8_t gUInt8Type[sizeof (TypeSpec)];
extern const uint8_t gUInt16Type[sizeof (TypeSpec)];
extern const uint8_t gUInt32Type[sizeof (TypeSpec)];
extern const uint8_t gUInt64Type[sizeof (TypeSpec)];

extern const uint8_t gInt8Type[sizeof (TypeSpec)];
extern const uint8_t gInt16Type[sizeof (TypeSpec)];
extern const uint8_t gInt32Type[sizeof (TypeSpec)];
extern const uint8_t gInt64Type[sizeof (TypeSpec)];

extern const uint8_t gRealType[sizeof (TypeSpec)];
extern const uint8_t gRichRealType[sizeof (TypeSpec)];

extern const uint8_t gTextType[sizeof (TypeSpec)];

extern const uint8_t gUndefinedType[sizeof (TypeSpec)];

extern const uint8_t gGenericArrayType[sizeof (TypeSpec)];
extern const uint8_t gGenericFieldType[sizeof (TypeSpec)];
extern const uint8_t gGenericTableType[sizeof (TypeSpec)];

extern const uint8_t gABoolType[sizeof (TypeSpec)];

extern const uint8_t gACharType[sizeof (TypeSpec)];

extern const uint8_t gADateType[sizeof (TypeSpec)];
extern const uint8_t gADateTimeType[sizeof (TypeSpec)];
extern const uint8_t gAHiresTimeType[sizeof (TypeSpec)];

extern const uint8_t gAUInt8Type[sizeof (TypeSpec)];
extern const uint8_t gAUInt16Type[sizeof (TypeSpec)];
extern const uint8_t gAUInt32Type[sizeof (TypeSpec)];
extern const uint8_t gAUInt64Type[sizeof (TypeSpec)];

extern const uint8_t gAInt8Type[sizeof (TypeSpec)];
extern const uint8_t gAInt16Type[sizeof (TypeSpec)];
extern const uint8_t gAInt32Type[sizeof (TypeSpec)];
extern const uint8_t gAInt64Type[sizeof (TypeSpec)];

extern const uint8_t gARealType[sizeof (TypeSpec)];
extern const uint8_t gARichRealType[sizeof (TypeSpec)];



whisper::WLIB_STATUS
base_types_init ();

#endif //BASE_TYPES_H_
