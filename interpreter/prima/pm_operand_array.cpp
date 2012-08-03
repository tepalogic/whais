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
using namespace prima;

///////////////////////////ArrayOperand////////////////////////////////////////

ArrayOperand::~ArrayOperand ()
{
}

bool
ArrayOperand::IsNull () const
{
  return m_Value.IsNull ();
}

void
ArrayOperand::GetValue (DBSArray& outValue) const
{
  outValue = m_Value;
}

void
ArrayOperand::SetValue (const DBSArray& value)
{
  m_Value = value;
}

StackValue
ArrayOperand::GetValueAt (const D_UINT64 index)
{
  switch (m_Value.ElementsType ())
  {
  case T_BOOL:
    return StackValue (BoolArrayElOperand (m_Value, index));
  case T_CHAR:
    return StackValue (CharArrayElOperand (m_Value, index));
  case T_DATE:
    return StackValue (DateArrayElOperand (m_Value, index));
  case T_DATETIME:
    return StackValue (DateTimeArrayElOperand (m_Value, index));
  case T_HIRESTIME:
    return StackValue (HiresTimeArrayElOperand (m_Value, index));
  case T_UINT8:
    return StackValue (UInt8ArrayElOperand (m_Value, index));
  case T_UINT16:
    return StackValue (UInt16ArrayElOperand (m_Value, index));
  case T_UINT32:
    return StackValue (UInt32ArrayElOperand (m_Value, index));
  case T_UINT64:
    return StackValue (UInt64ArrayElOperand (m_Value, index));
  case T_INT8:
    return StackValue (Int8ArrayElOperand (m_Value, index));
  case T_INT16:
    return StackValue (Int16ArrayElOperand (m_Value, index));
  case T_INT32:
    return StackValue (Int32ArrayElOperand (m_Value, index));
  case T_INT64:
    return StackValue (Int64ArrayElOperand (m_Value, index));
  case T_REAL:
    return StackValue (RealArrayElOperand (m_Value, index));
  case T_RICHREAL:
    return StackValue (RichRealArrayElOperand (m_Value, index));
  default:
    assert (false);
  }

  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}

StackValue
ArrayOperand::CopyValue () const
{
  return StackValue (*this);
}

///////////////////////////BaseArrayElOperand//////////////////////////////////

BaseArrayElOperand::~BaseArrayElOperand ()
{
}

//////////////////////////BoolArrayElOperand///////////////////////////////////

bool
BoolArrayElOperand::IsNull () const
{
  return false;
}

void
BoolArrayElOperand::GetValue (DBSBool& outValue) const
{
  Get (outValue);
}

void
BoolArrayElOperand::SetValue (const DBSBool& value)
{
  Set (value);
}

void
BoolArrayElOperand::SelfAnd (const DBSBool& value)
{
  DBSBool currValue;
  Get (currValue);

  currValue = internal_and (currValue, value);

  Set (currValue);
}

void
BoolArrayElOperand::SelfXor (const DBSBool& value)
{
  DBSBool currValue;
  Get (currValue);

  currValue = internal_xor (currValue, value);

  Set (currValue);
}

void
BoolArrayElOperand::SelfOr (const DBSBool& value)
{
  DBSBool currValue;
  Get (currValue);

  currValue = internal_or (currValue, value);

  Set (currValue);
}

StackValue
BoolArrayElOperand::CopyValue () const
{
  DBSBool value;
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
CharArrayElOperand::GetValue (DBSChar& outValue) const
{
  Get (outValue);
}

void
CharArrayElOperand::GetValue (DBSText& outValue) const
{
  DBSChar ch;
  Get (ch);

  outValue = DBSText ();
  outValue.Append (ch);
}

void
CharArrayElOperand::SetValue (const DBSChar& value)
{
  Set (value);
}

StackValue
CharArrayElOperand::CopyValue () const
{
  DBSChar ch;
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
DateArrayElOperand::GetValue (DBSDate& outValue) const
{
  Get (outValue);
}

void
DateArrayElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSDate currValue;
  Get (currValue);

  outValue = DBSDateTime (currValue.m_Year,
                          currValue.m_Month,
                          currValue.m_Day,
                          0,
                          0,
                          0);
}

void
DateArrayElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDate currValue;
  Get (currValue);

  outValue = DBSHiresTime (currValue.m_Year,
                           currValue.m_Month,
                           currValue.m_Day,
                           0,
                           0,
                           0,
                           0);
}

void
DateArrayElOperand::SetValue (const DBSDate& value)
{
  Set (value);
}

StackValue
DateArrayElOperand::CopyValue () const
{
  DBSDate value;
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
DateTimeArrayElOperand::GetValue (DBSDate& outValue) const
{
  DBSDateTime currValue;
  Get (currValue);

  outValue = DBSDate (currValue.m_Year, currValue.m_Month, currValue.m_Day);
}

void
DateTimeArrayElOperand::GetValue (DBSDateTime& outValue) const
{
  Get (outValue);
}

void
DateTimeArrayElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDateTime currValue;
  Get (currValue);

  outValue = DBSHiresTime (currValue.m_Year,
                           currValue.m_Month,
                           currValue.m_Day,
                           currValue.m_Hour,
                           currValue.m_Minutes,
                           currValue.m_Seconds,
                           0);
}

void
DateTimeArrayElOperand::SetValue (const DBSDateTime& value)
{
  Set (value);
}

StackValue
DateTimeArrayElOperand::CopyValue () const
{
  DBSDateTime value;
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
HiresTimeArrayElOperand::GetValue (DBSDate& outValue) const
{
  DBSHiresTime currValue;
  Get (currValue);

  outValue = DBSDate (currValue.m_Year, currValue.m_Month, currValue.m_Day);
}

void
HiresTimeArrayElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSHiresTime currValue;
  Get (currValue);

  outValue = DBSDateTime (currValue.m_Year,
                          currValue.m_Month,
                          currValue.m_Day,
                          currValue.m_Hour,
                          currValue.m_Minutes,
                          currValue.m_Seconds);
}

void
HiresTimeArrayElOperand::GetValue (DBSHiresTime& outValue) const
{
  Get (outValue);
}

void
HiresTimeArrayElOperand::SetValue (const DBSHiresTime& value)
{
  Set (value);
}

StackValue
HiresTimeArrayElOperand::CopyValue () const
{
  DBSHiresTime value;
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
UInt8ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  Get (outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayElOperand::SetValue (const DBSUInt8& value)
{
  Set (value);
}

void
UInt8ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_and (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_xor (currValue, value);

  Set (currValue);
}

void
UInt8ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_or (currValue, value);

  Set (currValue);
}

StackValue
UInt8ArrayElOperand::CopyValue () const
{
  DBSUInt8 value;
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
UInt16ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  Get (outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayElOperand::SetValue (const DBSUInt16& value)
{
  Set (value);
}

void
UInt16ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt16ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

StackValue
UInt16ArrayElOperand::CopyValue () const
{
  DBSUInt16 value;
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
UInt32ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  Get (outValue);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayElOperand::SetValue (const DBSUInt32& value)
{
  Set (value);
}

void
UInt32ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt32ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

StackValue
UInt32ArrayElOperand::CopyValue () const
{
  DBSUInt32 value;
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
UInt64ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  Get (outValue);
}

void
UInt64ArrayElOperand::SetValue (const DBSUInt64& value)
{
  Set (value);
}

void
UInt64ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt64ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

StackValue
UInt64ArrayElOperand::CopyValue () const
{
  DBSUInt64 value;
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
Int8ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  Get (outValue);
}

void
Int8ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayElOperand::SetValue (const DBSInt8& value)
{
  Set (value);
}

void
Int8ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int8ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

StackValue
Int8ArrayElOperand::CopyValue () const
{
  DBSInt8 value;
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
Int16ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  Get (outValue);
}

void
Int16ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayElOperand::SetValue (const DBSInt16& value)
{
  Set (value);
}

void
Int16ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int16ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

StackValue
Int16ArrayElOperand::CopyValue () const
{
  DBSInt16 value;
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
Int32ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  Get (outValue);
}

void
Int32ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayElOperand::SetValue (const DBSInt32& value)
{
  Set (value);
}

void
Int32ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int32ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

StackValue
Int32ArrayElOperand::CopyValue () const
{
  DBSInt32 value;
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
Int64ArrayElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  Get (outValue);
}

void
Int64ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayElOperand::SetValue (const DBSInt64& value)
{
  Set (value);
}

void
Int64ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int64ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

StackValue
Int64ArrayElOperand::CopyValue () const
{
  DBSInt64 value;
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
RealArrayElOperand::GetValue (DBSReal& outValue) const
{
  Get (outValue);
}

void
RealArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSReal currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
RealArrayElOperand::SetValue (const DBSReal& value)
{
  Set (value);
}

void
RealArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfSub (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfMul (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
RealArrayElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

StackValue
RealArrayElOperand::CopyValue () const
{
  DBSReal value;
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
RichRealArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSRichReal currValue;
  Get (currValue);
}

void
RichRealArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  Get (outValue);
}

void
RichRealArrayElOperand::SetValue (const DBSRichReal& value)
{
  Set (value);
}

void
RichRealArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfSub (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfMul (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
RichRealArrayElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

StackValue
RichRealArrayElOperand::CopyValue () const
{
  DBSRichReal value;
  Get (value);

  return StackValue (RichRealOperand (value));
}
