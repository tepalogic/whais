/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whais.
 Copyright(C) 2008  Iulian Popa

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
#include <sstream>

#include "whais.h"

#include "stdlib/interface.h"
#include "utils/wtypes.h"
#include "utils/wutf.h"
#include "utils/wrandom.h"
#include "interpreter/interpreter.h"

#include "base_types.h"


using namespace whais;


WLIB_PROC_DESCRIPTION     gProcPrevB;
WLIB_PROC_DESCRIPTION     gProcPrevC;

WLIB_PROC_DESCRIPTION     gProcPrevD;
WLIB_PROC_DESCRIPTION     gProcPrevDT;
WLIB_PROC_DESCRIPTION     gProcPrevHT;

WLIB_PROC_DESCRIPTION     gProcPrevI8;
WLIB_PROC_DESCRIPTION     gProcPrevI16;
WLIB_PROC_DESCRIPTION     gProcPrevI32;
WLIB_PROC_DESCRIPTION     gProcPrevI64;

WLIB_PROC_DESCRIPTION     gProcPrevU8;
WLIB_PROC_DESCRIPTION     gProcPrevU16;
WLIB_PROC_DESCRIPTION     gProcPrevU32;
WLIB_PROC_DESCRIPTION     gProcPrevU64;

WLIB_PROC_DESCRIPTION     gProcPrevR;
WLIB_PROC_DESCRIPTION     gProcPrevRR;


WLIB_PROC_DESCRIPTION     gProcNextB;
WLIB_PROC_DESCRIPTION     gProcNextC;

WLIB_PROC_DESCRIPTION     gProcNextD;
WLIB_PROC_DESCRIPTION     gProcNextDT;
WLIB_PROC_DESCRIPTION     gProcNextHT;

WLIB_PROC_DESCRIPTION     gProcNextI8;
WLIB_PROC_DESCRIPTION     gProcNextI16;
WLIB_PROC_DESCRIPTION     gProcNextI32;
WLIB_PROC_DESCRIPTION     gProcNextI64;

WLIB_PROC_DESCRIPTION     gProcNextU8;
WLIB_PROC_DESCRIPTION     gProcNextU16;
WLIB_PROC_DESCRIPTION     gProcNextU32;
WLIB_PROC_DESCRIPTION     gProcNextU64;

WLIB_PROC_DESCRIPTION     gProcNextR;
WLIB_PROC_DESCRIPTION     gProcNextRR;

WLIB_PROC_DESCRIPTION     gProcSleep;
WLIB_PROC_DESCRIPTION     gProcLog;

WLIB_PROC_DESCRIPTION     gProcCeil;
WLIB_PROC_DESCRIPTION     gProcRound;
WLIB_PROC_DESCRIPTION     gProcFloor;
WLIB_PROC_DESCRIPTION     gProcAbs;

WLIB_PROC_DESCRIPTION     gProcRnd;



template<class T>
WLIB_STATUS
get_prev_value( SessionStack& stack, ISession& )
{
  T value;

  stack[stack.Size() - 1].Operand().GetValue( value);

  value = value.Prev();

  stack.Pop(1);
  stack.Push( value);

  return WOP_OK;
}


template<class T>
WLIB_STATUS
get_next_value( SessionStack& stack, ISession& )
{
  T value;

  stack[stack.Size() - 1].Operand().GetValue( value);

  value = value.Next();

  stack.Pop(1);
  stack.Push( value);

  return WOP_OK;
}


static WLIB_STATUS
sleep( SessionStack& stack, ISession&)
{
  DUInt64 ms;

  stack[stack.Size() - 1].Operand().GetValue( ms);

  if (ms.IsNull() || (ms.mValue == 0))
    wh_yield();

  else
    wh_sleep( ms.mValue);

  stack.Pop(1);
  stack.Push( DBool( true));


  return WOP_OK;
}

static WLIB_STATUS
write_log( SessionStack& stack, ISession& session)
{
  DText   text;
  bool    result = false;
  uint8_t utf8Reserved[512];

  utf8Reserved[0] = 0;

  stack[stack.Size() - 1].Operand().GetValue( text);

  const uint64_t textRawSize = text.RawSize();
  if (textRawSize < sizeof( utf8Reserved) - 1)
    {
      text.RawRead( 0, textRawSize, utf8Reserved);
      utf8Reserved[textRawSize] = 0;

      session.GetLogger().Log(LT_INFO, _RC(const char*, utf8Reserved));
      result = true;
    }
  else
    {
      std::stringstream logEntry;

      uint64_t offset = 0;
      while ( offset < textRawSize)
        {
          const uint64_t chunkSize = MIN(textRawSize - offset,
                                          sizeof( utf8Reserved) -  1);

          text.RawRead( offset, chunkSize, utf8Reserved);
          utf8Reserved[chunkSize] = 0;

          logEntry << _RC(const char*, utf8Reserved);

          offset += chunkSize;
        }
      session.GetLogger().Log(LT_INFO, logEntry.str());
      result = true;
    }

  stack.Pop(1);
  stack.Push( DBool( result));

  return WOP_OK;
}


static WLIB_STATUS
ceil( SessionStack& stack, ISession& session)
{
  DRichReal value;

  stack[stack.Size() - 1].Operand().GetValue( value);

  if (value.IsNull())
    {
      stack.Pop(1);
      stack.Push( DInt64());

      return WOP_OK;
    }

  int64_t result;

  if (value < DRichReal( RICHREAL_T( 0, 0, DBS_RICHREAL_PREC)))
    result = value.mValue.Integer();

  else if (value.mValue.Fractional() != 0)
    result = value.mValue.Integer() + 1;

  else
    result =value.mValue.Integer();

  stack.Pop(1);
  stack.Push( DInt64(result));

  return WOP_OK;
}


static WLIB_STATUS
floor( SessionStack& stack, ISession& session)
{
  DRichReal value;

  stack[stack.Size() - 1].Operand().GetValue( value);

  if (value.IsNull())
    {
      stack.Pop(1);
      stack.Push( DInt64());

      return WOP_OK;
    }

  int64_t result;

  if (DRichReal( RICHREAL_T( 0, 0, DBS_RICHREAL_PREC)) <= value)
    result = value.mValue.Integer();

  else if (value.mValue.Fractional() != 0)
    result = value.mValue.Integer() - 1;

  else
    result =value.mValue.Integer();

  stack.Pop(1);
  stack.Push( DInt64(result));

  return WOP_OK;
}


static WLIB_STATUS
round( SessionStack& stack, ISession& session)
{
  DRichReal value;

  stack[stack.Size() - 1].Operand().GetValue( value);

  if (value.IsNull())
    {
      stack.Pop(1);
      stack.Push( DInt64());

      return WOP_OK;
    }

  int64_t result;

  if (DRichReal( RICHREAL_T( 0, 0, DBS_RICHREAL_PREC)) <= value)
    {
      result = value.mValue.Integer();

      if (value.mValue.Fractional() >= (5 * value.mValue.Precision() / 10))
        ++result;
    }

  else
    {
      result = value.mValue.Integer();

      if (value.mValue.Fractional() < (-5 * value.mValue.Precision() / 10))
        --result;
    }

  stack.Pop(1);
  stack.Push( DInt64(result));

  return WOP_OK;
}


static WLIB_STATUS
abs(SessionStack& stack, ISession& session)
{
  DRichReal value;

  stack[stack.Size() - 1].Operand().GetValue( value);

  if (! value.IsNull() &&
      ((value.mValue.Integer() <= 0) || (value.mValue.Fractional() < 0)))
    {
      const RICHREAL_T t( -value.mValue.Integer(),
                          -value.mValue.Fractional(),
                          DBS_RICHREAL_PREC);

      stack.Pop(1);
      stack.Push( DRichReal(t));
    }

  return WOP_OK;
}


static WLIB_STATUS
rnd(SessionStack& stack, ISession& session)
{
  DUInt64 maxValue;
  stack[stack.Size() - 1].Operand().GetValue( maxValue);

  uint64_t result = wh_rnd();
  if ( ! maxValue.IsNull() && (maxValue.mValue > 0))
    result %= maxValue.mValue;

  stack.Pop(1);
  stack.Push(DUInt64(result));

  return WOP_OK;
}



WLIB_STATUS
base_constants_init()
{
  static const uint8_t* localsBType[]     = { gBoolType, gBoolType };
  static const uint8_t* localsCType[]     = { gCharType, gCharType };

  static const uint8_t* localsDType[]     = { gDateType, gDateType };
  static const uint8_t* localsDTType[]    = { gDateTimeType, gDateTimeType };
  static const uint8_t* localsHTType[]    = { gHiresTimeType, gHiresTimeType };

  static const uint8_t* localsI8Type[]    = { gInt8Type, gInt8Type };
  static const uint8_t* localsI16Type[]   = { gInt16Type, gInt16Type };
  static const uint8_t* localsI32Type[]   = { gInt32Type, gInt32Type };
  static const uint8_t* localsI64Type[]   = { gInt64Type, gInt64Type };

  static const uint8_t* localsUI8Type[]   = { gUInt8Type, gUInt8Type };
  static const uint8_t* localsUI16Type[]  = { gUInt16Type, gUInt16Type };
  static const uint8_t* localsUI32Type[]  = { gUInt32Type, gUInt32Type };
  static const uint8_t* localsUI64Type[]  = { gUInt64Type, gUInt64Type };

  static const uint8_t* localsRType[]     = { gRealType, gRealType };
  static const uint8_t* localsRRType[]    = { gRichRealType, gRichRealType };

  gProcPrevB.name          = "prev_bool";
  gProcPrevB.localsCount   = 2;
  gProcPrevB.localsTypes   = localsBType;
  gProcPrevB.code          = get_prev_value<DBool>;

  gProcNextB.name          = "next_bool";
  gProcNextB.localsCount   = 2;
  gProcNextB.localsTypes   = localsBType;
  gProcNextB.code          = get_next_value<DBool>;

  gProcPrevC.name          = "prev_char";
  gProcPrevC.localsCount   = 2;
  gProcPrevC.localsTypes   = localsCType;
  gProcPrevC.code          = get_prev_value<DChar>;

  gProcNextC.name          = "next_char";
  gProcNextC.localsCount   = 2;
  gProcNextC.localsTypes   = localsCType;
  gProcNextC.code          = get_next_value<DChar>;

  gProcPrevD.name          = "prev_date";
  gProcPrevD.localsCount   = 2;
  gProcPrevD.localsTypes   = localsDType;
  gProcPrevD.code          = get_prev_value<DDate>;

  gProcNextD.name          = "next_date";
  gProcNextD.localsCount   = 2;
  gProcNextD.localsTypes   = localsDType;
  gProcNextD.code          = get_next_value<DDate>;

  gProcPrevDT.name         = "prev_dtime";
  gProcPrevDT.localsCount  = 2;
  gProcPrevDT.localsTypes  = localsDTType;
  gProcPrevDT.code         = get_prev_value<DDateTime>;

  gProcNextDT.name         = "next_dtime";
  gProcNextDT.localsCount  = 2;
  gProcNextDT.localsTypes  = localsDTType;
  gProcNextDT.code         = get_next_value<DDateTime>;

  gProcPrevHT.name         = "prev_htime";
  gProcPrevHT.localsCount  = 2;
  gProcPrevHT.localsTypes  = localsHTType;
  gProcPrevHT.code         = get_prev_value<DHiresTime>;

  gProcNextHT.name         = "next_htime";
  gProcNextHT.localsCount  = 2;
  gProcNextHT.localsTypes  = localsHTType;
  gProcNextHT.code         = get_next_value<DHiresTime>;

  gProcPrevI8.name         = "prev_i8";
  gProcPrevI8.localsCount  = 2;
  gProcPrevI8.localsTypes  = localsI8Type;
  gProcPrevI8.code         = get_prev_value<DInt8>;

  gProcNextI8.name         = "next_i8";
  gProcNextI8.localsCount  = 2;
  gProcNextI8.localsTypes  = localsI8Type;
  gProcNextI8.code         = get_next_value<DInt8>;

  gProcPrevI16.name         = "prev_i16";
  gProcPrevI16.localsCount  = 2;
  gProcPrevI16.localsTypes  = localsI16Type;
  gProcPrevI16.code         = get_prev_value<DInt16>;

  gProcNextI16.name         = "next_i16";
  gProcNextI16.localsCount  = 2;
  gProcNextI16.localsTypes  = localsI16Type;
  gProcNextI16.code         = get_next_value<DInt16>;

  gProcPrevI32.name         = "prev_i32";
  gProcPrevI32.localsCount  = 2;
  gProcPrevI32.localsTypes  = localsI32Type;
  gProcPrevI32.code         = get_prev_value<DInt32>;

  gProcNextI32.name         = "next_i32";
  gProcNextI32.localsCount  = 2;
  gProcNextI32.localsTypes  = localsI32Type;
  gProcNextI32.code         = get_next_value<DInt32>;

  gProcPrevI64.name         = "prev_i64";
  gProcPrevI64.localsCount  = 2;
  gProcPrevI64.localsTypes  = localsI64Type;
  gProcPrevI64.code         = get_prev_value<DInt64>;

  gProcNextI64.name         = "next_i64";
  gProcNextI64.localsCount  = 2;
  gProcNextI64.localsTypes  = localsI64Type;
  gProcNextI64.code         = get_next_value<DInt64>;

  gProcPrevU8.name          = "prev_u8";
  gProcPrevU8.localsCount   = 2;
  gProcPrevU8.localsTypes   = localsUI8Type;
  gProcPrevU8.code          = get_prev_value<DUInt8>;

  gProcNextU8.name          = "next_u8";
  gProcNextU8.localsCount   = 2;
  gProcNextU8.localsTypes   = localsUI8Type;
  gProcNextU8.code          = get_next_value<DUInt8>;

  gProcPrevU16.name         = "prev_u16";
  gProcPrevU16.localsCount  = 2;
  gProcPrevU16.localsTypes  = localsUI16Type;
  gProcPrevU16.code         = get_prev_value<DUInt16>;

  gProcNextU16.name         = "next_u16";
  gProcNextU16.localsCount  = 2;
  gProcNextU16.localsTypes  = localsUI16Type;
  gProcNextU16.code         = get_next_value<DUInt16>;

  gProcPrevU32.name         = "prev_u32";
  gProcPrevU32.localsCount  = 2;
  gProcPrevU32.localsTypes  = localsUI32Type;
  gProcPrevU32.code         = get_prev_value<DUInt32>;

  gProcNextU32.name         = "next_u32";
  gProcNextU32.localsCount  = 2;
  gProcNextU32.localsTypes  = localsUI32Type;
  gProcNextU32.code         = get_next_value<DUInt32>;

  gProcPrevU64.name         = "prev_u64";
  gProcPrevU64.localsCount  = 2;
  gProcPrevU64.localsTypes  = localsUI64Type;
  gProcPrevU64.code         = get_prev_value<DUInt64>;

  gProcNextU64.name         = "next_u64";
  gProcNextU64.localsCount  = 2;
  gProcNextU64.localsTypes  = localsUI64Type;
  gProcNextU64.code         = get_next_value<DUInt64>;


  gProcPrevR.name           = "prev_real";
  gProcPrevR.localsCount    = 2;
  gProcPrevR.localsTypes    = localsRType;
  gProcPrevR.code           = get_prev_value<DReal>;

  gProcNextR.name           = "next_real";
  gProcNextR.localsCount    = 2;
  gProcNextR.localsTypes    = localsRType;
  gProcNextR.code           = get_next_value<DReal>;

  gProcPrevRR.name          = "prev_rreal";
  gProcPrevRR.localsCount   = 2;
  gProcPrevRR.localsTypes   = localsRRType;
  gProcPrevRR.code          = get_prev_value<DRichReal>;

  gProcNextRR.name          = "next_rreal";
  gProcNextRR.localsCount   = 2;
  gProcNextRR.localsTypes   = localsRRType;
  gProcNextRR.code          = get_next_value<DRichReal>;


  static const uint8_t* sleepLocasType[] = { gBoolType, gUInt64Type };

  gProcSleep.name           = "sleep";
  gProcSleep.localsCount    = 2;
  gProcSleep.localsTypes    = sleepLocasType;
  gProcSleep.code           = sleep;

  static const uint8_t* logLocalsType[] = { gBoolType, gTextType };

  gProcLog.name             = "write_log";
  gProcLog.localsCount      = 2;
  gProcLog.localsTypes      = logLocalsType;
  gProcLog.code             = write_log;

  static const uint8_t* roundLocalsType[] = { gInt64Type, gRichRealType };

  gProcCeil.name            = "ceil";
  gProcCeil.localsCount     = 2;
  gProcCeil.localsTypes     = roundLocalsType;
  gProcCeil.code            = ceil;

  gProcFloor.name           = "floor";
  gProcFloor.localsCount    = 2;
  gProcFloor.localsTypes    = roundLocalsType;
  gProcFloor.code           = floor;

  gProcRound.name           = "round";
  gProcRound.localsCount    = 2;
  gProcRound.localsTypes    = roundLocalsType;
  gProcRound.code           = round;

  static const uint8_t* absLocalsType[] = { gRichRealType, gRichRealType };

  gProcAbs.name             = "abs";
  gProcAbs.localsCount      = 2;
  gProcAbs.localsTypes      = absLocalsType;
  gProcAbs.code             = abs;

  static const uint8_t* rndLocalsType[] = { gUInt64Type, gUInt64Type };

  gProcRnd.name             = "random";
  gProcRnd.localsCount      = 2;
  gProcRnd.localsTypes      = rndLocalsType;
  gProcRnd.code             = rnd;


  return WOP_OK;
}


