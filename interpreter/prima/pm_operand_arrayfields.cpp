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

CharTextFieldElOperand::~CharTextFieldElOperand ()
{
}

bool
CharTextFieldElOperand::IsNull () const
{
  return false;
}

void
CharTextFieldElOperand::GetValue (DBSChar& outValue) const
{
  DBSText text;

  m_Table.GetEntry (m_Row, m_Field, text);
  outValue = text.GetCharAtIndex (m_Index);

  assert (outValue.IsNull () == false);
}

void
CharTextFieldElOperand::GetValue (DBSText& outValue) const
{
  DBSChar currValue;
  DBSText text;

  m_Table.GetEntry (m_Row, m_Field, text);
  currValue = text.GetCharAtIndex (m_Index);

  assert (currValue.IsNull () == false);

  outValue = DBSText ();
  outValue.Append (currValue);
}

void
CharTextFieldElOperand::SetValue (const DBSChar& value)
{
  DBSText text;

  m_Table.GetEntry (m_Row, m_Field, text);
  text.SetCharAtIndex(value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, text);
}


//////////////////////////BoolArrayFieldElOperand///////////////////////////////

BoolArrayFieldElOperand::~BoolArrayFieldElOperand ()
{
}

bool
BoolArrayFieldElOperand::IsNull () const
{
  return false;
}

void
BoolArrayFieldElOperand::GetValue (DBSBool& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull() == false);
}

void
BoolArrayFieldElOperand::SetValue (const DBSBool& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
BoolArrayFieldElOperand::SelfAnd (const DBSBool& value)
{
  DBSBool  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
BoolArrayFieldElOperand::SelfXor (const DBSBool& value)
{
  DBSBool  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
BoolArrayFieldElOperand::SelfOr (const DBSBool& value)
{
  DBSBool  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

//////////////////////////CharArrayFieldElOperand//////////////////////////////

CharArrayFieldElOperand::~CharArrayFieldElOperand ()
{
}

bool
CharArrayFieldElOperand::IsNull () const
{
  return false;
}

void
CharArrayFieldElOperand::GetValue (DBSChar& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
CharArrayFieldElOperand::GetValue (DBSText& outValue) const
{
  DBSChar  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  outValue = DBSText ();
  outValue.Append (currValue);
}

void
CharArrayFieldElOperand::SetValue (const DBSChar& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

//////////////////////////DateArrayFieldElOperand//////////////////////////////

DateArrayFieldElOperand::~DateArrayFieldElOperand ()
{
}

bool
DateArrayFieldElOperand::IsNull () const
{
  return false;
}

void
DateArrayFieldElOperand::GetValue (DBSDate& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
DateArrayFieldElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSDate  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  const DBSDateTime temp (currValue.m_Year,
                          currValue.m_Month,
                          currValue.m_Day,
                          0,
                          0,
                          0);
  outValue = temp;
}

void
DateArrayFieldElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDate  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  const DBSHiresTime temp (currValue.m_Year,
                           currValue.m_Month,
                           currValue.m_Day,
                           0,
                           0,
                           0,
                           0);
  outValue = temp;
}

void
DateArrayFieldElOperand::SetValue (const DBSDate& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
}

/////////////////////////DateTimeArrayFieldElOperand////////////////////////////////

DateTimeArrayFieldElOperand::~DateTimeArrayFieldElOperand ()
{
}

bool
DateTimeArrayFieldElOperand::IsNull () const
{
  return false;
}

void
DateTimeArrayFieldElOperand::GetValue (DBSDate& outValue) const
{
  DBSDateTime currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  const DBSDate temp (currValue.m_Year,
                      currValue.m_Month,
                      currValue.m_Day);
  outValue = temp;
}

void
DateTimeArrayFieldElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
DateTimeArrayFieldElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDateTime currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  const DBSHiresTime temp (currValue.m_Year,
                           currValue.m_Month,
                           currValue.m_Day,
                           currValue.m_Hour,
                           currValue.m_Minutes,
                           currValue.m_Seconds,
                           0);
  outValue = temp;
}

void
DateTimeArrayFieldElOperand::SetValue (const DBSDateTime& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

///////////////////////HiresTimeArrayFieldElOperand/////////////////////////////

bool
HiresTimeArrayFieldElOperand::IsNull () const
{
  return false;
}

HiresTimeArrayFieldElOperand::~HiresTimeArrayFieldElOperand ()
{
}

void
HiresTimeArrayFieldElOperand::GetValue (DBSDate& outValue) const
{
  DBSHiresTime currValue;
  DBSArray     array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  const DBSDate temp (currValue.m_Year,
                      currValue.m_Month,
                      currValue.m_Day);
  outValue = temp;
}

void
HiresTimeArrayFieldElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSHiresTime currValue;
  DBSArray     array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  const DBSDateTime temp (currValue.m_Year,
                          currValue.m_Month,
                          currValue.m_Day,
                          currValue.m_Hour,
                          currValue.m_Minutes,
                          currValue.m_Seconds);
  outValue = temp;
}

void
HiresTimeArrayFieldElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);
  assert (outValue.IsNull () == false);
}

void
HiresTimeArrayFieldElOperand::SetValue (const DBSHiresTime& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

//////////////////////////UInt8ArrayFieldElOperand//////////////////////////////

UInt8ArrayFieldElOperand::~UInt8ArrayFieldElOperand ()
{
}

bool
UInt8ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);
  assert (outValue.IsNull () == false);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt8ArrayFieldElOperand::SetValue (const DBSUInt8& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt8ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt8 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

/////////////////////////UInt16ArrayFieldElOperand//////////////////////////////

UInt16ArrayFieldElOperand::~UInt16ArrayFieldElOperand ()
{
}

bool
UInt16ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);

}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);
  assert (outValue.IsNull () == false);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt16ArrayFieldElOperand::SetValue (const DBSUInt16& value)
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt16ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.GetEntry (m_Row, m_Field, array);
}

/////////////////////////UInt32ArrayFieldElOperand//////////////////////////////

UInt32ArrayFieldElOperand::~UInt32ArrayFieldElOperand ()
{
}

bool
UInt32ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
UInt32ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt32ArrayFieldElOperand::SetValue (const DBSUInt32& value)
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt32ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt32 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

/////////////////////////UInt64ArrayFieldElOperand//////////////////////////////

UInt64ArrayFieldElOperand::~UInt64ArrayFieldElOperand ()
{
}

bool
UInt64ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt64ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);
  assert (outValue.IsNull () == false);
}

void
UInt64ArrayFieldElOperand::SetValue (const DBSUInt64& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
UInt64ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

//////////////////////////Int8ArrayFieldElOperand//////////////////////////////

Int8ArrayFieldElOperand::~Int8ArrayFieldElOperand ()
{
}

bool
Int8ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int8ArrayFieldElOperand::SetValue (const DBSInt8& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int8ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt8  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

/////////////////////////Int16ArrayFieldElOperand//////////////////////////////

Int16ArrayFieldElOperand::~Int16ArrayFieldElOperand ()
{
}

bool
Int16ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);

}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt16 currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int16ArrayFieldElOperand::SetValue (const DBSInt16& value)
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt16 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int16ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt16  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

/////////////////////////Int32ArrayFieldElOperand//////////////////////////////

Int32ArrayFieldElOperand::~Int32ArrayFieldElOperand ()
{
}

bool
Int32ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
Int32ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int32ArrayFieldElOperand::SetValue (const DBSInt32& value)
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int32ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt32  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

/////////////////////////Int64ArrayFieldElOperand//////////////////////////////

Int64ArrayFieldElOperand::~Int64ArrayFieldElOperand ()
{
}

bool
Int64ArrayFieldElOperand::IsNull () const
{
  return false;
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt64 currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);
}

void
Int64ArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int64ArrayFieldElOperand::SetValue (const DBSInt64& value)
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
Int64ArrayFieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt64  currValue;
  DBSArray  array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

//////////////////////RealArrayFieldElOperand//////////////////////////////////

RealArrayFieldElOperand::~RealArrayFieldElOperand ()
{
}

bool
RealArrayFieldElOperand::IsNull () const
{
  return false;
}

void
RealArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);

  assert (outValue.IsNull () == false);
}

void
RealArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);
  assert (currValue.IsNull () == false);

  outValue = DBSRichReal (currValue.m_Value);
}

void
RealArrayFieldElOperand::SetValue (const DBSReal& value)
{
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.GetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RealArrayFieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSReal  currValue;
  DBSArray array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

/////////////////////////RichRealArrayFieldElOperand////////////////////////////

RichRealArrayFieldElOperand::~RichRealArrayFieldElOperand ()
{
}

bool
RichRealArrayFieldElOperand::IsNull () const
{
  return false;
}

void
RichRealArrayFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);
  assert (currValue.IsNull () == false);

  outValue = DBSReal (currValue.m_Value);
}

void
RichRealArrayFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (outValue, m_Index);
  assert (outValue.IsNull () == false);
}

void
RichRealArrayFieldElOperand::SetValue (const DBSRichReal& value)
{
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.SetElement (value, m_Index);
  m_Table.GetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}

void
RichRealArrayFieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSRichReal currValue;
  DBSArray    array;

  m_Table.GetEntry (m_Row, m_Field, array);
  array.GetElement (currValue, m_Index);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  array.SetElement (currValue, m_Index);
  m_Table.SetEntry (m_Row, m_Field, array);
}
