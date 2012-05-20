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

#ifndef PS_VALINTEP_H_
#define PS_VALINTEP_H_

#include "dbs_values.h"

namespace pastra

{

class PSValInterp
{
private:
  PSValInterp ();
  ~PSValInterp ();

public:
  static void Store (D_UINT8* pLocation, const DBSBool& value);
  static void Store (D_UINT8* pLocation, const DBSChar& value);
  static void Store (D_UINT8* pLocation, const DBSDate& value);
  static void Store (D_UINT8* pLocation, const DBSDateTime& value);
  static void Store (D_UINT8* pLocation, const DBSHiresTime& value);
  static void Store (D_UINT8* pLocation, const DBSInt8 &value);
  static void Store (D_UINT8* pLocation, const DBSInt16 &value);
  static void Store (D_UINT8* pLocation, const DBSInt32 &value);
  static void Store (D_UINT8* pLocation, const DBSInt64 &value);
  static void Store (D_UINT8* pLocation, const DBSReal& value);
  static void Store (D_UINT8* pLocation, const DBSRichReal& value);
  static void Store (D_UINT8* pLocation, const DBSUInt8 &value);
  static void Store (D_UINT8* pLocation, const DBSUInt16 &value);
  static void Store (D_UINT8* pLocation, const DBSUInt32 &value);
  static void Store (D_UINT8* pLocation, const DBSUInt64 &value);

  static void Retrieve (const D_UINT8* pLocation, DBSBool* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSChar* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSDate* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSDateTime* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSHiresTime* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSInt8* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSInt16* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSInt32* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSInt64* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSReal* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSRichReal* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSUInt8* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSUInt16* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSUInt32* pOutValue);
  static void Retrieve (const D_UINT8* pLocation, DBSUInt64* pOutValue);

  static D_INT Size (DBS_FIELD_TYPE type, bool isArray);
  static D_INT Alignment (DBS_FIELD_TYPE, bool isArray);
};

};

#endif /* PS_VALINTEP_H_ */
