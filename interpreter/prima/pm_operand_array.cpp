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
  if (index >= m_Value.ElementsCount ())
    throw InterException (NULL, _EXTRA (InterException::INVALID_ROW_INDEX));

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

//////////////////////////////ArrayElOperand///////////////////////////////////

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
  m_Array.GetElement (outValue, m_ElementIndex);
}

void
BoolArrayElOperand::SetValue (const DBSBool& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
BoolArrayElOperand::SelfAnd (const DBSBool& value)
{
  DBSBool currValue;

  m_Array.GetElement (currValue, m_ElementIndex);

  currValue = internal_and (currValue, value);
  assert (currValue.IsNull () == false);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
BoolArrayElOperand::SelfXor (const DBSBool& value)
{
  DBSBool currValue;

  m_Array.GetElement (currValue, m_ElementIndex);

  currValue = internal_xor (currValue, value);
  assert (currValue.IsNull() == false);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
BoolArrayElOperand::SelfOr (const DBSBool& value)
{
  DBSBool currValue;

  m_Array.GetElement (currValue, m_ElementIndex);

  currValue = internal_or (currValue, value);
  assert (currValue.IsNull() == false);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (outValue, m_ElementIndex);

  assert (outValue.IsNull () == false);
}

void
CharArrayElOperand::GetValue (DBSText& outValue) const
{
  DBSChar currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  const DBSText result;
  outValue = result;

  outValue.Append (currValue);
}

void
CharArrayElOperand::SetValue (const DBSChar& value)
{
  m_Array.SetElement (value, m_ElementIndex);
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
  m_Array.GetElement (outValue, m_ElementIndex);

  assert (outValue.IsNull () == false);
}

void
DateArrayElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSDate currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

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
DateArrayElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDate currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

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
DateArrayElOperand::SetValue (const DBSDate& value)
{
  m_Array.SetElement (value, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  const DBSDate temp (currValue.m_Year,
                      currValue.m_Month,
                      currValue.m_Day);
  outValue = temp;
}

void
DateTimeArrayElOperand::GetValue (DBSDateTime& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);

  assert (outValue.IsNull () == false);
}

void
DateTimeArrayElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDateTime currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

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
DateTimeArrayElOperand::SetValue (const DBSDateTime& value)
{
  m_Array.SetElement (value, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  const DBSDate temp (currValue.m_Year,
                      currValue.m_Month,
                      currValue.m_Day);
  outValue = temp;
}

void
HiresTimeArrayElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSHiresTime currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

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
HiresTimeArrayElOperand::GetValue (DBSHiresTime& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
HiresTimeArrayElOperand::SetValue (const DBSHiresTime& value)
{
  m_Array.SetElement (value, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt8ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt8ArrayElOperand::SetValue (const DBSUInt8& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt8ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt16ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt16ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt16ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt16ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt16ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt16ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);

}

void
UInt16ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
UInt16ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt16ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt16ArrayElOperand::SetValue (const DBSUInt16& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt16ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
UInt32ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt32ArrayElOperand::SetValue (const DBSUInt32& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt32ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt64ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
UInt64ArrayElOperand::SetValue (const DBSUInt64& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
UInt64ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull() == false);
}

void
Int8ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int8ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int8ArrayElOperand::SetValue (const DBSInt8& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int8ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
Int16ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
Int16ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
Int16ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
Int16ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int16ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int16ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);

}

void
Int16ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int16ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int16ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int16ArrayElOperand::SetValue (const DBSInt16& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int16ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
Int32ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt64 (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int32ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int32ArrayElOperand::SetValue (const DBSInt32& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int32ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt8 (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt16 (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSInt32 (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSInt64& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
Int64ArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int64ArrayElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int64ArrayElOperand::SetValue (const DBSInt64& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mod (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_and (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_xor (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
Int64ArrayElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_or (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
RealArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSRichReal (currValue.m_Value);
}

void
RealArrayElOperand::SetValue (const DBSReal& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
RealArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RealArrayElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RealArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RealArrayElOperand::SelfSub (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RealArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RealArrayElOperand::SelfMul (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RealArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RealArrayElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
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
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);
  outValue = DBSReal (currValue.m_Value);
}

void
RichRealArrayElOperand::GetValue (DBSRichReal& outValue) const
{
  m_Array.GetElement (outValue, m_ElementIndex);
  assert (outValue.IsNull () == false);
}

void
RichRealArrayElOperand::SetValue (const DBSRichReal& value)
{
  m_Array.SetElement (value, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfAdd (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_add (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfSub (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfSub (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_sub (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfMul (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfMul (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_mul (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfDiv (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}

void
RichRealArrayElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Array.GetElement (currValue, m_ElementIndex);

  assert (currValue.IsNull () == false);

  currValue = internal_div (currValue, value);

  m_Array.SetElement (currValue, m_ElementIndex);
}
