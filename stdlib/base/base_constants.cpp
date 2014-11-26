/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whais.
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

#include "whais.h"

#include "stdlib/interface.h"
#include "utils/wtypes.h"
#include "interpreter/interpreter.h"

#include "base_types.h"


using namespace whais;


WLIB_PROC_DESCRIPTION     gProcPI;
WLIB_PROC_DESCRIPTION     gProcSQRT2;
WLIB_PROC_DESCRIPTION     gProcNE;

WLIB_PROC_DESCRIPTION     gProcMinB;
WLIB_PROC_DESCRIPTION     gProcMinC;

WLIB_PROC_DESCRIPTION     gProcMinD;
WLIB_PROC_DESCRIPTION     gProcMinDT;
WLIB_PROC_DESCRIPTION     gProcMinHT;

WLIB_PROC_DESCRIPTION     gProcMinI8;
WLIB_PROC_DESCRIPTION     gProcMinI16;
WLIB_PROC_DESCRIPTION     gProcMinI32;
WLIB_PROC_DESCRIPTION     gProcMinI64;

WLIB_PROC_DESCRIPTION     gProcMinU8;
WLIB_PROC_DESCRIPTION     gProcMinU16;
WLIB_PROC_DESCRIPTION     gProcMinU32;
WLIB_PROC_DESCRIPTION     gProcMinU64;

WLIB_PROC_DESCRIPTION     gProcMinR;
WLIB_PROC_DESCRIPTION     gProcMinRR;


WLIB_PROC_DESCRIPTION     gProcMaxB;
WLIB_PROC_DESCRIPTION     gProcMaxC;

WLIB_PROC_DESCRIPTION     gProcMaxD;
WLIB_PROC_DESCRIPTION     gProcMaxDT;
WLIB_PROC_DESCRIPTION     gProcMaxHT;

WLIB_PROC_DESCRIPTION     gProcMaxI8;
WLIB_PROC_DESCRIPTION     gProcMaxI16;
WLIB_PROC_DESCRIPTION     gProcMaxI32;
WLIB_PROC_DESCRIPTION     gProcMaxI64;

WLIB_PROC_DESCRIPTION     gProcMaxU8;
WLIB_PROC_DESCRIPTION     gProcMaxU16;
WLIB_PROC_DESCRIPTION     gProcMaxU32;
WLIB_PROC_DESCRIPTION     gProcMaxU64;

WLIB_PROC_DESCRIPTION     gProcMaxR;
WLIB_PROC_DESCRIPTION     gProcMaxRR;



static WLIB_STATUS
get_PI( SessionStack& stack, ISession&)
{
  stack.Push( DRichReal( RICHREAL_T( 3,
                                     14159265358979ull,
                                     DBS_RICHREAL_PREC)));

  return WOP_OK;
}


static WLIB_STATUS
get_SQRT2 (SessionStack& stack, ISession&)
{
  stack.Push( DRichReal( RICHREAL_T( 2,
                                     41421356237309ull,
                                     DBS_RICHREAL_PREC)));

  return WOP_OK;
}


static WLIB_STATUS
get_NE( SessionStack& stack, ISession&)
{
  stack.Push( DRichReal( RICHREAL_T( 2,
                                     71828182845904ull,
                                     DBS_RICHREAL_PREC)));

  return WOP_OK;
}


template<class T>
WLIB_STATUS
get_min_value( SessionStack& stack, ISession& )
{
  stack.Push( T::Min ());

  return WOP_OK;
}


template<class T>
WLIB_STATUS
get_max_value( SessionStack& stack, ISession& )
{
  stack.Push( T::Max ());

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

  gProcPI.name             = "pi";
  gProcPI.localsCount      = 1;
  gProcPI.localsTypes      = procLocalRRType;
  gProcPI.code             = get_PI;

  gProcSQRT2.name          = "sqrt2";
  gProcSQRT2.localsCount   = 1;
  gProcSQRT2.localsTypes   = procLocalRRType;
  gProcSQRT2.code          = get_SQRT2;

  gProcNE.name             = "e";
  gProcNE.localsCount      = 1;
  gProcNE.localsTypes      = procLocalRRType;
  gProcNE.code             = get_NE;


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

  gProcMinI8.name         = "min_i8";
  gProcMinI8.localsCount  = 1;
  gProcMinI8.localsTypes  = procLocalI8Type;
  gProcMinI8.code         = get_min_value<DInt8>;

  gProcMaxI8.name         = "max_i8";
  gProcMaxI8.localsCount  = 1;
  gProcMaxI8.localsTypes  = procLocalI8Type;
  gProcMaxI8.code         = get_max_value<DInt8>;

  gProcMinI16.name         = "min_i16";
  gProcMinI16.localsCount  = 1;
  gProcMinI16.localsTypes  = procLocalI16Type;
  gProcMinI16.code         = get_min_value<DInt16>;

  gProcMaxI16.name         = "max_i16";
  gProcMaxI16.localsCount  = 1;
  gProcMaxI16.localsTypes  = procLocalI16Type;
  gProcMaxI16.code         = get_max_value<DInt16>;

  gProcMinI32.name         = "min_i32";
  gProcMinI32.localsCount  = 1;
  gProcMinI32.localsTypes  = procLocalI32Type;
  gProcMinI32.code         = get_min_value<DInt32>;

  gProcMaxI32.name         = "max_i32";
  gProcMaxI32.localsCount  = 1;
  gProcMaxI32.localsTypes  = procLocalI32Type;
  gProcMaxI32.code         = get_max_value<DInt32>;

  gProcMinI64.name         = "min_i64";
  gProcMinI64.localsCount  = 1;
  gProcMinI64.localsTypes  = procLocalI64Type;
  gProcMinI64.code         = get_min_value<DInt64>;

  gProcMaxI64.name         = "max_i64";
  gProcMaxI64.localsCount  = 1;
  gProcMaxI64.localsTypes  = procLocalI64Type;
  gProcMaxI64.code         = get_max_value<DInt64>;


  gProcMinU8.name          = "min_u8";
  gProcMinU8.localsCount   = 1;
  gProcMinU8.localsTypes   = procLocalUI8Type;
  gProcMinU8.code          = get_min_value<DUInt8>;

  gProcMaxU8.name          = "max_u8";
  gProcMaxU8.localsCount   = 1;
  gProcMaxU8.localsTypes   = procLocalUI8Type;
  gProcMaxU8.code          = get_max_value<DUInt8>;

  gProcMinU16.name         = "min_u16";
  gProcMinU16.localsCount  = 1;
  gProcMinU16.localsTypes  = procLocalUI16Type;
  gProcMinU16.code         = get_min_value<DUInt16>;

  gProcMaxU16.name         = "max_u16";
  gProcMaxU16.localsCount  = 1;
  gProcMaxU16.localsTypes  = procLocalUI16Type;
  gProcMaxU16.code         = get_max_value<DUInt16>;

  gProcMinU32.name         = "min_u32";
  gProcMinU32.localsCount  = 1;
  gProcMinU32.localsTypes  = procLocalUI32Type;
  gProcMinU32.code         = get_min_value<DUInt32>;

  gProcMaxU32.name         = "max_u32";
  gProcMaxU32.localsCount  = 1;
  gProcMaxU32.localsTypes  = procLocalUI32Type;
  gProcMaxU32.code         = get_max_value<DUInt32>;

  gProcMinU64.name         = "min_u64";
  gProcMinU64.localsCount  = 1;
  gProcMinU64.localsTypes  = procLocalUI64Type;
  gProcMinU64.code         = get_min_value<DUInt64>;

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


