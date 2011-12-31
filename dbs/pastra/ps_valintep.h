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
  static void Store (const DBSBool &rSource, D_UINT8 *pDestination);
  static void Store (const DBSChar &rSource, D_UINT8 *pDestination);
  static void Store (const DBSDate &rSource, D_UINT8 *pDestination);
  static void Store (const DBSDateTime &rSource, D_UINT8 *pDestination);
  static void Store (const DBSHiresTime &rSource, D_UINT8 *pDestination);
  static void Store (const DBSInt8 &rSource, D_UINT8 *pDestination);
  static void Store (const DBSInt16 &rSource, D_UINT8 *pDestination);
  static void Store (const DBSInt32 &rSource, D_UINT8 *pDestination);
  static void Store (const DBSInt64 &rSource, D_UINT8 *pDestination);
  static void Store (const DBSReal &rSource, D_UINT8 *pDestination);
  static void Store (const DBSRichReal &rSource, D_UINT8 *pDestination);
  static void Store (const DBSUInt8 &rSource, D_UINT8 *pDestination);
  static void Store (const DBSUInt16 &rSource, D_UINT8 *pDestination);
  static void Store (const DBSUInt32 &rSource, D_UINT8 *pDestination);
  static void Store (const DBSUInt64 &rSource, D_UINT8 *pDestination);

  static void Retrieve (DBSBool *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSChar *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSDate *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSDateTime *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSHiresTime *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSInt8 *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSInt16 *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSInt32 *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSInt64 *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSReal *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSRichReal *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSUInt8 *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSUInt16 *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSUInt32 *pOutValue, const D_UINT8 *pSource);
  static void Retrieve (DBSUInt64 *pOutValue, const D_UINT8 *pSource);

  static D_INT GetSize (DBS_FIELD_TYPE type, bool isArray);
  static D_INT GetAlignment (DBS_FIELD_TYPE, bool isArray);

};

};

#endif /* PS_VALINTEP_H_ */
