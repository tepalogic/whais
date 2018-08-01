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

#include <assert.h>

#include "whais.h"

#include "stdlib/interface.h"
#include "utils/wtypes.h"
#include "interpreter/interpreter.h"

#include "base_types.h"


using namespace whais;

WLIB_PROC_DESCRIPTION     gProcPI;
WLIB_PROC_DESCRIPTION     gProcPHI;
WLIB_PROC_DESCRIPTION     gProcSQRT2;
WLIB_PROC_DESCRIPTION     gProcE;

WLIB_PROC_DESCRIPTION     gProcMinB;
WLIB_PROC_DESCRIPTION     gProcMinC;

WLIB_PROC_DESCRIPTION     gProcMinD;
WLIB_PROC_DESCRIPTION     gProcMinDT;
WLIB_PROC_DESCRIPTION     gProcMinHT;

WLIB_PROC_DESCRIPTION     gProcMinS8;
WLIB_PROC_DESCRIPTION     gProcMinS16;
WLIB_PROC_DESCRIPTION     gProcMinS32;
WLIB_PROC_DESCRIPTION     gProcMinS64;

WLIB_PROC_DESCRIPTION     gProcMinR;
WLIB_PROC_DESCRIPTION     gProcMinRR;

WLIB_PROC_DESCRIPTION     gProcMaxB;
WLIB_PROC_DESCRIPTION     gProcMaxC;

WLIB_PROC_DESCRIPTION     gProcMaxD;
WLIB_PROC_DESCRIPTION     gProcMaxDT;
WLIB_PROC_DESCRIPTION     gProcMaxHT;

WLIB_PROC_DESCRIPTION     gProcMaxS8;
WLIB_PROC_DESCRIPTION     gProcMaxS16;
WLIB_PROC_DESCRIPTION     gProcMaxS32;
WLIB_PROC_DESCRIPTION     gProcMaxS64;

WLIB_PROC_DESCRIPTION     gProcMaxU8;
WLIB_PROC_DESCRIPTION     gProcMaxU16;
WLIB_PROC_DESCRIPTION     gProcMaxU32;
WLIB_PROC_DESCRIPTION     gProcMaxU64;

WLIB_PROC_DESCRIPTION     gProcMaxR;
WLIB_PROC_DESCRIPTION     gProcMaxRR;


static WLIB_STATUS
get_PHI( SessionStack& stack, ISession&)
{
  stack.Push(DRichReal(RICHREAL_T(1, 61803398874989ull, DBS_RICHREAL_PREC)));
  return WOP_OK;
}


static WLIB_STATUS
get_PI( SessionStack& stack, ISession&)
{
  stack.Push(DRichReal(RICHREAL_T(3, 14159265358979ull, DBS_RICHREAL_PREC)));
  return WOP_OK;
}


static WLIB_STATUS
get_SQRT2(SessionStack& stack, ISession&)
{
  stack.Push(DRichReal(RICHREAL_T(1, 41421356237309ull, DBS_RICHREAL_PREC)));
  return WOP_OK;
}


static WLIB_STATUS
get_E( SessionStack& stack, ISession&)
{
  stack.Push(DRichReal(RICHREAL_T(2, 71828182845904ull, DBS_RICHREAL_PREC)));
  return WOP_OK;
}


template<class T>
WLIB_STATUS
get_min_value( SessionStack& stack, ISession& )
{
  stack.Push(T::Min());
  return WOP_OK;
}


template<class T>
WLIB_STATUS
get_max_value( SessionStack& stack, ISession& )
{
  stack.Push( T::Max());

  return WOP_OK;
}


WLIB_STATUS
base_generics_init()
{
  static const uint8_t* procLocalBType[]     = { gBoolType };
  static const uint8_t* procLocalCType[]     = { gCharType };

  static const uint8_t* procLocalDType[]     = { gDateType };
  static const uint8_t* procLocalDTType[]    = { gDateTimeType };
  static const uint8_t* procLocalHTType[]    = { gHiresTimeType };

  static const uint8_t* procLocalI8Type[]    = { gInt8Type };
  static const uint8_t* procLocalI16Type[]   = { gInt16Type };
  static const uint8_t* procLocalI32Type[]   = { gInt32Type };
  static const uint8_t* procLocalI64Type[]   = { gInt64Type };

  static const uint8_t* procLocalUI8Type[]   = { gUInt8Type };
  static const uint8_t* procLocalUI16Type[]  = { gUInt16Type };
  static const uint8_t* procLocalUI32Type[]  = { gUInt32Type };
  static const uint8_t* procLocalUI64Type[]  = { gUInt64Type };

  static const uint8_t* procLocalRType[]     = { gRealType };
  static const uint8_t* procLocalRRType[]    = { gRichRealType };

  gProcPHI.name             = "pi";
  gProcPHI.localsCount      = 1;
  gProcPHI.localsTypes      = procLocalRRType;
  gProcPHI.code             = get_PI;

  gProcPI.name             = "phi";
  gProcPI.localsCount      = 1;
  gProcPI.localsTypes      = procLocalRRType;
  gProcPI.code             = get_PHI;

  gProcSQRT2.name          = "sqrt2";
  gProcSQRT2.localsCount   = 1;
  gProcSQRT2.localsTypes   = procLocalRRType;
  gProcSQRT2.code          = get_SQRT2;

  gProcE.name             = "e";
  gProcE.localsCount      = 1;
  gProcE.localsTypes      = procLocalRRType;
  gProcE.code             = get_E;


  gProcMinB.name          = "min_bool";
  gProcMinB.localsCount   = 1;
  gProcMinB.localsTypes   = procLocalBType;
  gProcMinB.code          = get_min_value<DBool>;

  gProcMaxB.name          = "max_bool";
  gProcMaxB.localsCount   = 1;
  gProcMaxB.localsTypes   = procLocalBType;
  gProcMaxB.code          = get_max_value<DBool>;

  gProcMinC.name          = "min_char";
  gProcMinC.localsCount   = 1;
  gProcMinC.localsTypes   = procLocalCType;
  gProcMinC.code          = get_min_value<DChar>;

  gProcMaxC.name          = "max_char";
  gProcMaxC.localsCount   = 1;
  gProcMaxC.localsTypes   = procLocalCType;
  gProcMaxC.code          = get_max_value<DChar>;

  gProcMinD.name          = "min_date";
  gProcMinD.localsCount   = 1;
  gProcMinD.localsTypes   = procLocalDType;
  gProcMinD.code          = get_min_value<DDate>;

  gProcMaxD.name          = "max_date";
  gProcMaxD.localsCount   = 1;
  gProcMaxD.localsTypes   = procLocalDType;
  gProcMaxD.code          = get_max_value<DDate>;

  gProcMinDT.name         = "min_dtime";
  gProcMinDT.localsCount  = 1;
  gProcMinDT.localsTypes  = procLocalDTType;
  gProcMinDT.code         = get_min_value<DDateTime>;

  gProcMaxDT.name         = "max_dtime";
  gProcMaxDT.localsCount  = 1;
  gProcMaxDT.localsTypes  = procLocalDTType;
  gProcMaxDT.code         = get_max_value<DDateTime>;

  gProcMinHT.name         = "min_htime";
  gProcMinHT.localsCount  = 1;
  gProcMinHT.localsTypes  = procLocalHTType;
  gProcMinHT.code         = get_min_value<DHiresTime>;

  gProcMaxHT.name         = "max_htime";
  gProcMaxHT.localsCount  = 1;
  gProcMaxHT.localsTypes  = procLocalHTType;
  gProcMaxHT.code         = get_max_value<DHiresTime>;

  gProcMinS8.name         = "min_s8";
  gProcMinS8.localsCount  = 1;
  gProcMinS8.localsTypes  = procLocalI8Type;
  gProcMinS8.code         = get_min_value<DInt8>;

  gProcMaxS8.name         = "max_s8";
  gProcMaxS8.localsCount  = 1;
  gProcMaxS8.localsTypes  = procLocalI8Type;
  gProcMaxS8.code         = get_max_value<DInt8>;

  gProcMinS16.name         = "min_s16";
  gProcMinS16.localsCount  = 1;
  gProcMinS16.localsTypes  = procLocalI16Type;
  gProcMinS16.code         = get_min_value<DInt16>;

  gProcMaxS16.name         = "max_s16";
  gProcMaxS16.localsCount  = 1;
  gProcMaxS16.localsTypes  = procLocalI16Type;
  gProcMaxS16.code         = get_max_value<DInt16>;

  gProcMinS32.name         = "min_s32";
  gProcMinS32.localsCount  = 1;
  gProcMinS32.localsTypes  = procLocalI32Type;
  gProcMinS32.code         = get_min_value<DInt32>;

  gProcMaxS32.name         = "max_s32";
  gProcMaxS32.localsCount  = 1;
  gProcMaxS32.localsTypes  = procLocalI32Type;
  gProcMaxS32.code         = get_max_value<DInt32>;

  gProcMinS64.name         = "min_s64";
  gProcMinS64.localsCount  = 1;
  gProcMinS64.localsTypes  = procLocalI64Type;
  gProcMinS64.code         = get_min_value<DInt64>;

  gProcMaxS64.name         = "max_s64";
  gProcMaxS64.localsCount  = 1;
  gProcMaxS64.localsTypes  = procLocalI64Type;
  gProcMaxS64.code         = get_max_value<DInt64>;

  gProcMaxU8.name          = "max_u8";
  gProcMaxU8.localsCount   = 1;
  gProcMaxU8.localsTypes   = procLocalUI8Type;
  gProcMaxU8.code          = get_max_value<DUInt8>;

  gProcMaxU16.name         = "max_u16";
  gProcMaxU16.localsCount  = 1;
  gProcMaxU16.localsTypes  = procLocalUI16Type;
  gProcMaxU16.code         = get_max_value<DUInt16>;

  gProcMaxU32.name         = "max_u32";
  gProcMaxU32.localsCount  = 1;
  gProcMaxU32.localsTypes  = procLocalUI32Type;
  gProcMaxU32.code         = get_max_value<DUInt32>;

  gProcMaxU64.name         = "max_u64";
  gProcMaxU64.localsCount  = 1;
  gProcMaxU64.localsTypes  = procLocalUI64Type;
  gProcMaxU64.code         = get_max_value<DUInt64>;


  gProcMinR.name           = "min_real";
  gProcMinR.localsCount    = 1;
  gProcMinR.localsTypes    = procLocalRType;
  gProcMinR.code           = get_min_value<DReal>;

  gProcMaxR.name           = "max_real";
  gProcMaxR.localsCount    = 1;
  gProcMaxR.localsTypes    = procLocalRType;
  gProcMaxR.code           = get_max_value<DReal>;

  gProcMinRR.name          = "min_rreal";
  gProcMinRR.localsCount   = 1;
  gProcMinRR.localsTypes   = procLocalRRType;
  gProcMinRR.code          = get_min_value<DRichReal>;

  gProcMaxRR.name          = "max_rreal";
  gProcMaxRR.localsCount   = 1;
  gProcMaxRR.localsTypes   = procLocalRRType;
  gProcMaxRR.code          = get_max_value<DRichReal>;

  return WOP_OK;
}


