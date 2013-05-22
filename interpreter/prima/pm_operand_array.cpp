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

#include <assert.h>

#include "pm_operand.h"

#include "pm_typemanager.h"

using namespace std;

namespace whisper {
namespace prima {

///////////////////////////ArrayOperand////////////////////////////////////////

ArrayOperand::~ArrayOperand ()
{
}

bool
ArrayOperand::IsNull () const
{
  return mValue.IsNull ();
}

void
ArrayOperand::GetValue (DArray& outValue) const
{
  outValue = mValue;
}

void
ArrayOperand::SetValue (const DArray& value)
{
  mValue = value;
}

uint_t
ArrayOperand::GetType ()
{
  uint_t type = mFirstArrayType;
  MARK_ARRAY (type);

  return type;
}

StackValue
ArrayOperand::GetValueAt (const uint64_t index)
{
  switch (mValue.Type ())
  {
  case T_BOOL:
    return StackValue (BoolArrayElOperand (mValue, index));
  case T_CHAR:
    return StackValue (CharArrayElOperand (mValue, index));
  case T_DATE:
    return StackValue (DateArrayElOperand (mValue, index));
  case T_DATETIME:
    return StackValue (DateTimeArrayElOperand (mValue, index));
  case T_HIRESTIME:
    return StackValue (HiresTimeArrayElOperand (mValue, index));
  case T_UINT8:
    return StackValue (UInt8ArrayElOperand (mValue, index));
  case T_UINT16:
    return StackValue (UInt16ArrayElOperand (mValue, index));
  case T_UINT32:
    return StackValue (UInt32ArrayElOperand (mValue, index));
  case T_UINT64:
    return StackValue (UInt64ArrayElOperand (mValue, index));
  case T_INT8:
    return StackValue (Int8ArrayElOperand (mValue, index));
  case T_INT16:
    return StackValue (Int16ArrayElOperand (mValue, index));
  case T_INT32:
    return StackValue (Int32ArrayElOperand (mValue, index));
  case T_INT64:
    return StackValue (Int64ArrayElOperand (mValue, index));
  case T_REAL:
    return StackValue (RealArrayElOperand (mValue, index));
  case T_RICHREAL:
    return StackValue (RichRealArrayElOperand (mValue, index));
  default:
    assert (false);
  }

  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

StackValue
ArrayOperand::Duplicate () const
{
  return StackValue (*this);
}

void
ArrayOperand::NotifyCopy ()
{
  mValue.MakeMirror (mValue);
}

///////////////////////////BaseArrayElOperand//////////////////////////////////

BaseArrayElOperand::~BaseArrayElOperand ()
{
}

void
BaseArrayElOperand::NotifyCopy ()
{
  mArray.MakeMirror (mArray);
}

//////////////////////////BoolArrayElOperand///////////////////////////////////

bool
BoolArrayElOperand::IsNull () const
{
  return false;
}

void
BoolArrayElOperand::GetValue (DBool& outValue) const
{
  Get (outValue);
}

void
BoolArrayElOperand::SetValue (const DBool& value)
{
  Set (value);
}

void
BoolArrayElOperand::SelfAnd (const DBool& value)
{
  DBool currValue;
  Get (currValue);

  currValue = internal_and (currValue, value);

  Set (currValue);
}

void
BoolArrayElOperand::SelfXor (const DBool& value)
{
  DBool currValue;
  Get (currValue);

  currValue = internal_xor (currValue, value);

  Set (currValue);
}

void
BoolArrayElOperand::SelfOr (const DBool& value)
{
  DBool currValue;
  Get (currValue);

  currValue = internal_or (currValue, value);

  Set (currValue);
}

uint_t
BoolArrayElOperand::GetType ()
{
  return T_BOOL;
}

StackValue
BoolArrayElOperand::Duplicate () const
{
  DBool value;
  Get (value);

  return StackValue (BoolOperand (value));
}


//////////////////////////CharArrayElOperand///////////////////////////////////

bool
CharArrayElOperand::IsNull () const
{
  return false;
}

void
CharArrayElOperand::GetValue (DChar& outValue) const
{
  Get (outValue);
}

void
CharArrayElOperand::GetValue (DText& outValue) const
{
  DChar ch;
  Get (ch);

  outValue = DText ();
  outValue.Append (ch);
}

void
CharArrayElOperand::SetValue (const DChar& value)
{
  Set (value);
}

uint_t
CharArrayElOperand::GetType ()
{
  return T_CHAR;
}

StackValue
CharArrayElOperand::Duplicate () const
{
  DChar ch;
  Get (ch);

  return StackValue (CharOperand (ch));
}

//////////////////////////DateArrayElOperand///////////////////////////////////

bool
DateArrayElOperand::IsNull () const
{
  return false;
}

void
DateArrayElOperand::GetValue (DDate& outValue) const
{
  Get (outValue);
}

void
DateArrayElOperand::GetValue (DDateTime& outValue) const
{
  DDate currValue;
  Get (currValue);

  outValue = DDateTime (currValue.mYear,
                          currValue.mMonth,
                          currValue.mDay,
                          0,
                          0,
                          0);
}

void
DateArrayElOperand::GetValue (DHiresTime& outValue) const
{
  DDate currValue;
  Get (currValue);

  outValue = DHiresTime (currValue.mYear,
                           currValue.mMonth,
                           currValue.mDay,
                           0,
                           0,
                           0,
                           0);
}

void
DateArrayElOperand::SetValue (const DDate& value)
{
  Set (value);
}

uint_t
DateArrayElOperand::GetType ()
{
  return T_DATE;
}

StackValue
DateArrayElOperand::Duplicate () const
{
  DDate value;
  Get (value);

  return StackValue (DateOperand (value));
}

/////////////////////////DateTimeArrayElOperand////////////////////////////////

bool
DateTimeArrayElOperand::IsNull () const
{
  return false;
}

void
DateTimeArrayElOperand::GetValue (DDate& outValue) const
{
  DDateTime currValue;
  Get (currValue);

  outValue = DDate (currValue.mYear, currValue.mMonth, currValue.mDay);
}

void
DateTimeArrayElOperand::GetValue (DDateTime& outValue) const
{
  Get (outValue);
}

void
DateTimeArrayElOperand::GetValue (DHiresTime& outValue) const
{
  DDateTime currValue;
  Get (currValue);

  outValue = DHiresTime (currValue.mYear,
                           currValue.mMonth,
                           currValue.mDay,
                           currValue.mHour,
                           currValue.mMinutes,
                           currValue.mSeconds,
                           0);
}

void
DateTimeArrayElOperand::SetValue (const DDateTime& value)
{
  Set (value);
}

uint_t
DateTimeArrayElOperand::GetType ()
{
  return T_DATETIME;
}

StackValue
DateTimeArrayElOperand::Duplicate () const
{
  DDateTime value;
  Get (value);

  return StackValue (DateTimeOperand (value));
}

///////////////////////HiresTimeArrayElOperand/////////////////////////////////

bool
HiresTimeArrayElOperand::IsNull () const
{
  return false;
}

void
HiresTimeArrayElOperand::GetValue (DDate& outValue) const
{
  DHiresTime currValue;
  Get (currValue);

  outValue = DDate (currValue.mYear, currValue.mMonth, currValue.mDay);
}

void
HiresTimeArrayElOperand::GetValue (DDateTime& outValue) const
{
  DHiresTime currValue;
  Get (currValue);

  outValue = DDateTime (currValue.mYear,
                          currValue.mMonth,
                          currValue.mDay,
                          currValue.mHour,
                          currValue.mMinutes,
                          currValue.mSeconds);
}

void
HiresTimeArrayElOperand::GetValue (DHiresTime& outValue) const
{
  Get (outValue);
}

void
HiresTimeArrayElOperand::SetValue (const DHiresTime& value)
{
  Set (value);
}

uint_t
HiresTimeArrayElOperand::GetType ()
{
  return T_HIRESTIME;
}

StackValue
HiresTimeArrayElOperand::Duplicate () const
{
  DHiresTime value;
  Get (value);

  return StackValue (HiresTimeOperand (value));
}

//////////////////////////UInt8ArrayElOperand//////////////////////////////////

bool
UInt8ArrayElOperand::IsNull () const
{
  return false;
}

void
UInt8ArrayElOperand::GetValue (DInt8& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DInt16& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DInt32& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DInt64& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DReal& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DUInt8& outValue) const
{
  Get (outValue);
}

void
UInt8ArrayElOperand::GetValue (DUInt16& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DUInt32& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DUInt64& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::SetValue (const DUInt8& value)
{
  Set (value);
}

void
UInt8ArrayElOperand::SelfAdd (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfSub (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfMul (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfDiv (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfMod (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfAnd (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_and (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfXor (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_xor (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfOr (const DInt64& value)
{
  DUInt8 currValue;
  Get (currValue);

  currValue = internal_or (currValue, value);

  Set (currValue);
}

uint_t
UInt8ArrayElOperand::GetType ()
{
  return T_UINT8;
}

StackValue
UInt8ArrayElOperand::Duplicate () const
{
  DUInt8 value;
  Get (value);

  return StackValue (UInt8Operand (value));
}

/////////////////////////UInt16ArrayElOperand//////////////////////////////////

bool
UInt16ArrayElOperand::IsNull () const
{
  return false;
}

void
UInt16ArrayElOperand::GetValue (DInt8& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DInt16& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DInt32& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DInt64& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DReal& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DUInt8& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DUInt16& outValue) const
{
  Get (outValue);
}

void
UInt16ArrayElOperand::GetValue (DUInt32& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DUInt64& outValue) const
{
  DUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::SetValue (const DUInt16& value)
{
  Set (value);
}

void
UInt16ArrayElOperand::SelfAdd (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfSub (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfMul (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfDiv (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfMod (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfAnd (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfXor (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfOr (const DInt64& value)
{
  DUInt16 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
UInt16ArrayElOperand::GetType ()
{
  return T_UINT16;
}

StackValue
UInt16ArrayElOperand::Duplicate () const
{
  DUInt16 value;
  Get (value);

  return StackValue (UInt16Operand (value));
}

/////////////////////////UInt32ArrayElOperand//////////////////////////////////

bool
UInt32ArrayElOperand::IsNull () const
{
  return false;
}

void
UInt32ArrayElOperand::GetValue (DInt8& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DInt16& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DInt32& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DInt64& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DReal& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DUInt8& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DUInt16& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DUInt32& outValue) const
{
  Get (outValue);
}

void
UInt32ArrayElOperand::GetValue (DUInt64& outValue) const
{
  DUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::SetValue (const DUInt32& value)
{
  Set (value);
}

void
UInt32ArrayElOperand::SelfAdd (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfSub (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfMul (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfDiv (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfMod (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfAnd (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfXor (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfOr (const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
UInt32ArrayElOperand::GetType ()
{
  return T_UINT32;
}

StackValue
UInt32ArrayElOperand::Duplicate () const
{
  DUInt32 value;
  Get (value);

  return StackValue (UInt32Operand (value));
}

/////////////////////////UInt64ArrayElOperand//////////////////////////////////

bool
UInt64ArrayElOperand::IsNull () const
{
  return false;
}

void
UInt64ArrayElOperand::GetValue (DInt8& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DInt16& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DInt32& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DInt64& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DReal& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DUInt8& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DUInt16& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DUInt32& outValue) const
{
  DUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DUInt64& outValue) const
{
  Get (outValue);
}

void
UInt64ArrayElOperand::SetValue (const DUInt64& value)
{
  Set (value);
}

void
UInt64ArrayElOperand::SelfAdd (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfSub (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfMul (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfDiv (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfMod (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfAnd (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfXor (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfOr (const DInt64& value)
{
  DUInt64 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
UInt64ArrayElOperand::GetType ()
{
  return T_UINT64;
}

StackValue
UInt64ArrayElOperand::Duplicate () const
{
  DUInt64 value;
  Get (value);

  return StackValue (UInt64Operand (value));
}

//////////////////////////Int8ArrayElOperand//////////////////////////////////

bool
Int8ArrayElOperand::IsNull () const
{
  return false;
}

void
Int8ArrayElOperand::GetValue (DInt8& outValue) const
{
  Get (outValue);
}

void
Int8ArrayElOperand::GetValue (DInt16& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DInt32& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DInt64& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DReal& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DUInt8& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DUInt16& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DUInt32& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DUInt64& outValue) const
{
  DInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::SetValue (const DInt8& value)
{
  Set (value);
}

void
Int8ArrayElOperand::SelfAdd (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfSub (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfMul (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfDiv (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfMod (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfAnd (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfXor (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfOr (const DInt64& value)
{
  DInt8 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int8ArrayElOperand::GetType ()
{
  return T_INT8;
}

StackValue
Int8ArrayElOperand::Duplicate () const
{
  DInt8 value;
  Get (value);

  return StackValue (Int8Operand (value));
}

/////////////////////////Int16ArrayElOperand//////////////////////////////////

bool
Int16ArrayElOperand::IsNull () const
{
  return false;
}

void
Int16ArrayElOperand::GetValue (DInt8& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DInt16& outValue) const
{
  Get (outValue);
}

void
Int16ArrayElOperand::GetValue (DInt32& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DInt64& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DReal& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DUInt8& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DUInt16& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DUInt32& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DUInt64& outValue) const
{
  DInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::SetValue (const DInt16& value)
{
  Set (value);
}

void
Int16ArrayElOperand::SelfAdd (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfSub (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfMul (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfDiv (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfMod (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfAnd (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfXor (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfOr (const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int16ArrayElOperand::GetType ()
{
  return T_INT16;
}

StackValue
Int16ArrayElOperand::Duplicate () const
{
  DInt16 value;
  Get (value);

  return StackValue (Int16Operand (value));
}

/////////////////////////Int32ArrayElOperand//////////////////////////////////

bool
Int32ArrayElOperand::IsNull () const
{
  return false;
}

void
Int32ArrayElOperand::GetValue (DInt8& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DInt16& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DInt32& outValue) const
{
  Get (outValue);
}

void
Int32ArrayElOperand::GetValue (DInt64& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DReal& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DUInt8& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DUInt16& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DUInt32& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DUInt64& outValue) const
{
  DInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::SetValue (const DInt32& value)
{
  Set (value);
}

void
Int32ArrayElOperand::SelfAdd (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfSub (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfMul (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfDiv (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfMod (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfAnd (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfXor (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfOr (const DInt64& value)
{
  DInt32 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int32ArrayElOperand::GetType ()
{
  return T_INT32;
}

StackValue
Int32ArrayElOperand::Duplicate () const
{
  DInt32 value;
  Get (value);

  return StackValue (Int32Operand (value));
}

/////////////////////////Int64ArrayElOperand//////////////////////////////////

bool
Int64ArrayElOperand::IsNull () const
{
  return false;
}

void
Int64ArrayElOperand::GetValue (DInt8& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DInt16& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DInt32& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DInt64& outValue) const
{
  Get (outValue);
}

void
Int64ArrayElOperand::GetValue (DRichReal& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DReal& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DUInt8& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DUInt16& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DUInt32& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DUInt64& outValue) const
{
  DInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::SetValue (const DInt64& value)
{
  Set (value);
}

void
Int64ArrayElOperand::SelfAdd (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfSub (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfMul (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfDiv (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfMod (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfAnd (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfXor (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfOr (const DInt64& value)
{
  DInt64 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int64ArrayElOperand::GetType ()
{
  return T_INT64;
}

StackValue
Int64ArrayElOperand::Duplicate () const
{
  DInt64 value;
  Get (value);

  return StackValue (Int64Operand (value));
}

/////////////////////////RealArrayElOperand//////////////////////////////////

bool
RealArrayElOperand::IsNull () const
{
  return false;
}

void
RealArrayElOperand::GetValue (DReal& outValue) const
{
  Get (outValue);
}

void
RealArrayElOperand::GetValue (DRichReal& outValue) const
{
  DReal currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
RealArrayElOperand::SetValue (const DReal& value)
{
  Set (value);
}

void
RealArrayElOperand::SelfAdd (const DInt64& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfAdd (const DRichReal& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfSub (const DInt64& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfSub (const DRichReal& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfMul (const DInt64& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfMul (const DRichReal& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfDiv (const DInt64& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfDiv (const DRichReal& value)
{
  DReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

uint_t
RealArrayElOperand::GetType ()
{
  return T_REAL;
}

StackValue
RealArrayElOperand::Duplicate () const
{
  DReal value;
  Get (value);

  return StackValue (RealOperand (value));
}

/////////////////////////RichRealArrayElOperand////////////////////////////////

bool
RichRealArrayElOperand::IsNull () const
{
  return false;
}

void
RichRealArrayElOperand::GetValue (DReal& outValue) const
{
  DRichReal currValue;
  Get (currValue);
}

void
RichRealArrayElOperand::GetValue (DRichReal& outValue) const
{
  Get (outValue);
}

void
RichRealArrayElOperand::SetValue (const DRichReal& value)
{
  Set (value);
}

void
RichRealArrayElOperand::SelfAdd (const DInt64& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfAdd (const DRichReal& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfSub (const DInt64& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfSub (const DRichReal& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfMul (const DInt64& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfMul (const DRichReal& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfDiv (const DInt64& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfDiv (const DRichReal& value)
{
  DRichReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

uint_t
RichRealArrayElOperand::GetType ()
{
  return T_RICHREAL;
}

StackValue
RichRealArrayElOperand::Duplicate () const
{
  DRichReal value;
  Get (value);

  return StackValue (RichRealOperand (value));
}

} //namespace prima
} //namespace whisper

