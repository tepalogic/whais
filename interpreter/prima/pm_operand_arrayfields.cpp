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

#include "pm_operand.h"

#include "pm_typemanager.h"

using namespace std;
using namespace prima;


//////////////////////////CharTextFieldElOperand//////////////////////////////

bool
CharTextFieldElOperand::IsNull () const
{
  return false;
}

void
CharTextFieldElOperand::GetValue (DBSChar& outValue) const
{
  I_DBSTable& table = m_pRefTable->GetTable ();

  DBSText text;
  table.GetEntry (m_Row, m_Field, text);

  outValue = text.GetCharAtIndex (m_Index);

  assert (outValue.IsNull () == false);
}

void
CharTextFieldElOperand::GetValue (DBSText& outValue) const
{
  DBSChar ch;
  this->GetValue (ch);

  outValue = DBSText ();
  outValue.Append (ch);
}

void
CharTextFieldElOperand::SetValue (const DBSChar& value)
{
  I_DBSTable& table = m_pRefTable->GetTable ();

  DBSText text;
  table.GetEntry (m_Row, m_Field, text);

  text.SetCharAtIndex(value, m_Index);
  table.SetEntry (m_Row, m_Field, text);
}

uint_t
CharTextFieldElOperand::GetType ()
{
  return T_CHAR;
}

StackValue
CharTextFieldElOperand::Duplicate () const
{
  DBSChar ch;
  Get (ch);

  return StackValue (CharOperand (ch));
}

///////////////////////////BaseArrayFieldElOperand//////////////////////////////

BaseArrayFieldElOperand::~BaseArrayFieldElOperand ()
{
  m_pRefTable->DecrementRefCount ();
}

void
BaseArrayFieldElOperand::NotifyCopy ()
{
  m_pRefTable->IncrementRefCount ();
}

//////////////////////////BoolArrayFieldElOperand///////////////////////////////////

bool
BoolArrayFieldElOperand::IsNull () const
{
  return false;
}

void
BoolArrayFieldElOperand::GetValue (DBSBool& outValue) const
{
  Get (outValue);
}

void
BoolArrayFieldElOperand::SetValue (const DBSBool& value)
{
  Set (value);
}

void
BoolArrayFieldElOperand::SelfAnd (const DBSBool& value)
{
  DBSBool currValue;
  Get (currValue);

  currValue = internal_and (currValue, value);

  Set (currValue);
}

void
BoolArrayFieldElOperand::SelfXor (const DBSBool& value)
{
  DBSBool currValue;
  Get (currValue);

  currValue = internal_xor (currValue, value);

  Set (currValue);
}

void
BoolArrayFieldElOperand::SelfOr (const DBSBool& value)
{
  DBSBool currValue;
  Get (currValue);

  currValue = internal_or (currValue, value);

  Set (currValue);
}

uint_t
BoolArrayFieldElOperand::GetType ()
{
  return T_BOOL;
}


StackValue
BoolArrayFieldElOperand::Duplicate () const
{
  DBSBool value;
  Get (value);

  return StackValue (BoolOperand (value));
}

//////////////////////////CharArrayFieldElOperand///////////////////////////////////

bool
CharArrayFieldElOperand::IsNull () const
{
  return false;
}

void
CharArrayFieldElOperand::GetValue (DBSChar& outValue) const
{
  Get (outValue);
}

void
CharArrayFieldElOperand::GetValue (DBSText& outValue) const
{
  DBSChar ch;
  Get (ch);

  outValue = DBSText ();
  outValue.Append (ch);
}

void
CharArrayFieldElOperand::SetValue (const DBSChar& value)
{
  Set (value);
}

uint_t
CharArrayFieldElOperand::GetType ()
{
  return T_CHAR;
}

StackValue
CharArrayFieldElOperand::Duplicate () const
{
  DBSChar ch;
  Get (ch);

  return StackValue (CharOperand (ch));
}


//////////////////////////DateArrayFieldElOperand///////////////////////////////////

bool
DateArrayFieldElOperand::IsNull () const
{
  return false;
}

void
DateArrayFieldElOperand::GetValue (DBSDate& outValue) const
{
  Get (outValue);
}

void
DateArrayFieldElOperand::GetValue (DBSDateTime& outValue) const
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
DateArrayFieldElOperand::GetValue (DBSHiresTime& outValue) const
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
DateArrayFieldElOperand::SetValue (const DBSDate& value)
{
  Set (value);
}

uint_t
DateArrayFieldElOperand::GetType ()
{
  return T_DATE;
}

StackValue
DateArrayFieldElOperand::Duplicate () const
{
  DBSDate value;
  Get (value);

  return StackValue (DateOperand (value));
}


/////////////////////////DateTimeArrayFieldElOperand////////////////////////////////

bool
DateTimeArrayFieldElOperand::IsNull () const
{
  return false;
}

void
DateTimeArrayFieldElOperand::GetValue (DBSDate& outValue) const
{
  DBSDateTime currValue;
  Get (currValue);

  outValue = DBSDate (currValue.m_Year, currValue.m_Month, currValue.m_Day);
}

void
DateTimeArrayFieldElOperand::GetValue (DBSDateTime& outValue) const
{
  Get (outValue);
}

void
DateTimeArrayFieldElOperand::GetValue (DBSHiresTime& outValue) const
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
DateTimeArrayFieldElOperand::SetValue (const DBSDateTime& value)
{
  Set (value);
}

uint_t
DateTimeArrayFieldElOperand::GetType ()
{
  return T_DATETIME;
}

StackValue
DateTimeArrayFieldElOperand::Duplicate () const
{
  DBSDateTime value;
  Get (value);

  return StackValue (DateTimeOperand (value));
}

///////////////////////HiresTimeArrayFieldElOperand/////////////////////////////////

bool
HiresTimeArrayFieldElOperand::IsNull () const
{
  return false;
}

void
HiresTimeArrayFieldElOperand::GetValue (DBSDate& outValue) const
{
  DBSHiresTime currValue;
  Get (currValue);

  outValue = DBSDate (currValue.m_Year, currValue.m_Month, currValue.m_Day);
}

void
HiresTimeArrayFieldElOperand::GetValue (DBSDateTime& outValue) const
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
HiresTimeArrayFieldElOperand::GetValue (DBSHiresTime& outValue) const
{
  Get (outValue);
}

void
HiresTimeArrayFieldElOperand::SetValue (const DBSHiresTime& value)
{
  Set (value);
}

uint_t
HiresTimeArrayFieldElOperand::GetType ()
{
  return T_HIRESTIME;
}

StackValue
HiresTimeArrayFieldElOperand::Duplicate () const
{
  DBSHiresTime value;
  Get (value);

  return StackValue (HiresTimeOperand (value));
}

//////////////////////////UInt8ArrayFieldElOperand//////////////////////////////////

bool
UInt8ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  Get (outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt8ArrayFieldElOperand::SetValue (const DBSUInt8& value)
{
  Set (value);
}

void
UInt8ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt8ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt8ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt8ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt8ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt8ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_and (currValue, value);

  Set (currValue);
}

void
UInt8ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_xor (currValue, value);

  Set (currValue);
}

void
UInt8ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt8 currValue;
  Get (currValue);

  currValue = internal_or (currValue, value);

  Set (currValue);
}

uint_t
UInt8ArrayFieldElOperand::GetType ()
{
  return T_UINT8;
}

StackValue
UInt8ArrayFieldElOperand::Duplicate () const
{
  DBSUInt8 value;
  Get (value);

  return StackValue (UInt8Operand (value));
}


/////////////////////////UInt16ArrayFieldElOperand/////////////////////////////

bool
UInt16ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  Get (outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt16ArrayFieldElOperand::SetValue (const DBSUInt16& value)
{
  Set (value);
}

void
UInt16ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt16 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
UInt16ArrayFieldElOperand::GetType ()
{
  return T_UINT16;
}

StackValue
UInt16ArrayFieldElOperand::Duplicate () const
{
  DBSUInt16 value;
  Get (value);

  return StackValue (UInt16Operand (value));
}


/////////////////////////UInt32ArrayFieldElOperand//////////////////////////////////

bool
UInt32ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  Get (outValue);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt32ArrayFieldElOperand::SetValue (const DBSUInt32& value)
{
  Set (value);
}

void
UInt32ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt32ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt32ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt32ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt32ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt32ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt32ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt32ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt32 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
UInt32ArrayFieldElOperand::GetType ()
{
  return T_UINT32;
}

StackValue
UInt32ArrayFieldElOperand::Duplicate () const
{
  DBSUInt32 value;
  Get (value);

  return StackValue (UInt32Operand (value));
}

/////////////////////////UInt64ArrayFieldElOperand//////////////////////////////////

bool
UInt64ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  Get (outValue);
}

void
UInt64ArrayFieldElOperand::SetValue (const DBSUInt64& value)
{
  Set (value);
}

void
UInt64ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
UInt64ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
UInt64ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
UInt64ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
UInt64ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
UInt64ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
UInt64ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
UInt64ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt64 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
UInt64ArrayFieldElOperand::GetType ()
{
  return T_UINT64;
}

StackValue
UInt64ArrayFieldElOperand::Duplicate () const
{
  DBSUInt64 value;
  Get (value);

  return StackValue (UInt64Operand (value));
}

//////////////////////////Int8ArrayFieldElOperand//////////////////////////////////

bool
Int8ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  Get (outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt8 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int8ArrayFieldElOperand::SetValue (const DBSInt8& value)
{
  Set (value);
}

void
Int8ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int8ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int8ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int8ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int8ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int8ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int8ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int8ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt8 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int8ArrayFieldElOperand::GetType ()
{
  return T_INT8;
}

StackValue
Int8ArrayFieldElOperand::Duplicate () const
{
  DBSInt8 value;
  Get (value);

  return StackValue (Int8Operand (value));
}


/////////////////////Int16ArrayFieldElOperand//////////////////////////////////

bool
Int16ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  Get (outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt16 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int16ArrayFieldElOperand::SetValue (const DBSInt16& value)
{
  Set (value);
}

void
Int16ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int16ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int16ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int16ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int16ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int16ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int16ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int16ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt16 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int16ArrayFieldElOperand::GetType ()
{
  return T_INT16;
}

StackValue
Int16ArrayFieldElOperand::Duplicate () const
{
  DBSInt16 value;
  Get (value);

  return StackValue (Int16Operand (value));
}

/////////////////////Int32ArrayFieldElOperand//////////////////////////////////

bool
Int32ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  Get (outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt32 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int32ArrayFieldElOperand::SetValue (const DBSInt32& value)
{
  Set (value);
}

void
Int32ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int32ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int32ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int32ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int32ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int32ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int32ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int32ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt32 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int32ArrayFieldElOperand::GetType ()
{
  return T_INT32;
}

StackValue
Int32ArrayFieldElOperand::Duplicate () const
{
  DBSInt32 value;
  Get (value);

  return StackValue (Int32Operand (value));
}

/////////////////////Int64ArrayFieldElOperand//////////////////////////////////

bool
Int64ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  Get (outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt64 currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
Int64ArrayFieldElOperand::SetValue (const DBSInt64& value)
{
  Set (value);
}

void
Int64ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
Int64ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
Int64ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
Int64ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
Int64ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_mod (currValue, value);

  Set (currValue);
}

void
Int64ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_and(currValue, value);

  Set (currValue);
}

void
Int64ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_xor(currValue, value);

  Set (currValue);
}

void
Int64ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt64 currValue;
  Get (currValue);

  currValue = internal_or(currValue, value);

  Set (currValue);
}

uint_t
Int64ArrayFieldElOperand::GetType ()
{
  return T_INT64;
}

StackValue
Int64ArrayFieldElOperand::Duplicate () const
{
  DBSInt64 value;
  Get (value);

  return StackValue (Int64Operand (value));
}

/////////////////////////RealArrayFieldElOperand///////////////////////////////

bool
RealArrayFieldElOperand::IsNull () const
{
  return false;
}

void
RealArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  Get (outValue);
}

void
RealArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSReal currValue;
  Get (currValue);

  number_convert (currValue, outValue);
}

void
RealArrayFieldElOperand::SetValue (const DBSReal& value)
{
  Set (value);
}

void
RealArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RealArrayFieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RealArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RealArrayFieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RealArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RealArrayFieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RealArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
RealArrayFieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

uint_t
RealArrayFieldElOperand::GetType ()
{
  return T_REAL;
}

StackValue
RealArrayFieldElOperand::Duplicate () const
{
  DBSReal value;
  Get (value);

  return StackValue (RealOperand (value));
}


////////////////////RichRealArrayFieldElOperand////////////////////////////////

bool
RichRealArrayFieldElOperand::IsNull () const
{
  return false;
}

void
RichRealArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSRichReal currValue;
  Get (currValue);
}

void
RichRealArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  Get (outValue);
}

void
RichRealArrayFieldElOperand::SetValue (const DBSRichReal& value)
{
  Set (value);
}

void
RichRealArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RichRealArrayFieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_add (currValue, value);

  Set (currValue);
}

void
RichRealArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RichRealArrayFieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_sub (currValue, value);

  Set (currValue);
}

void
RichRealArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RichRealArrayFieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_mul (currValue, value);

  Set (currValue);
}

void
RichRealArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

void
RichRealArrayFieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSRichReal currValue;
  Get (currValue);

  currValue = internal_div (currValue, value);

  Set (currValue);
}

uint_t
RichRealArrayFieldElOperand::GetType ()
{
  return T_RICHREAL;
}


StackValue
RichRealArrayFieldElOperand::Duplicate () const
{
  DBSRichReal value;
  Get (value);

  return StackValue (RichRealOperand (value));
}

