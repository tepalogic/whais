/******************************************************************************
 PASTRA - A light database one file system and more.
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

#include "dbs/dbs_valtranslator.h"

#include "pm_operand.h"
#include "pm_operand_undefined.h"
#include "pm_typemanager.h"

using namespace std;


namespace whais {
namespace prima {


bool
BaseOperand::IsNull() const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DBool& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DChar& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DDate& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DDateTime& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DHiresTime& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DInt8& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DInt16& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DInt32& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DInt64& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DReal& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DRichReal& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DUInt8& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DUInt16& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DUInt32& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}

void
BaseOperand::GetValue(DText& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}

void
BaseOperand::GetValue(DArray& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::GetValue(DUInt64& outValue) const
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SetValue(const DBool& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SetValue(const DChar& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SetValue(const DHiresTime& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SetValue(const DUInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}

void
BaseOperand::SetValue(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SetValue(const DRichReal& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SetValue(const DText& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SetValue(const DArray& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfAdd(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfAdd(const DRichReal& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfAdd(const DChar& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfAdd(const DText& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfSub(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfSub(const DRichReal& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfMul(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfMul(const DUInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}

void
BaseOperand::SelfMul(const DRichReal& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfDiv(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfDiv(const DUInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfDiv(const DRichReal& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfMod(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}

void
BaseOperand::SelfMod(const DUInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}

void
BaseOperand::SelfAnd(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfAnd(const DBool& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfXor(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfXor(const DBool& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfOr(const DInt64& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::SelfOr(const DBool& value)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


FIELD_INDEX
BaseOperand::GetField()
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


ITable&
BaseOperand::GetTable()
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


StackValue
BaseOperand::GetTableValue()
{
  return StackValue(TableOperand(GetTableReference()));
}


StackValue
BaseOperand::GetFieldAt(const FIELD_INDEX field)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


StackValue
BaseOperand::GetValueAt(const uint64_t index)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


bool
BaseOperand::StartIterate(const bool reverse, StackValue& outStartItem)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


bool
BaseOperand::Iterate(const bool reverse)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


uint64_t
BaseOperand::IteratorOffset()
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


TableOperand
BaseOperand::GetTableOp()
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::CopyTableOp(const TableOperand& tableOp)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


FieldOperand
BaseOperand::GetFieldOp()
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::CopyFieldOp(const FieldOperand& fieldOp)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::CopyNativeObjectOperand(const NativeObjectOperand& source)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


void
BaseOperand::NativeObject(INativeObject* const)
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


INativeObject&
BaseOperand::NativeObject()
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


bool
BaseOperand::DoSimpleCopy(void* const)
{
  return true;
}


TableReference&
BaseOperand::GetTableReference()
{
  throw InterException(_EXTRA(InterException::INVALID_OP_REQ));
}


template <typename T>
static void assign_null(T& output)
{
  output = T();
}



bool
NullOperand::IsNull() const
{
  return true;
}


void
NullOperand::GetValue(DBool& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DChar& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DDate& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DDateTime& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DHiresTime& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DInt8& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DInt16& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DInt32& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DInt64& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DReal& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DRichReal& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DUInt8& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DUInt16& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DUInt32& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DUInt64& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DText& outValue) const
{
  assign_null(outValue);
}


void
NullOperand::GetValue(DArray& outValue) const
{
  assign_null(outValue);
}


uint_t
NullOperand::GetType()
{
  return T_UNKNOWN;
}


StackValue
NullOperand::Duplicate() const
{
  return StackValue(*this);
}




bool
BoolOperand::IsNull() const
{
  return mValue.IsNull();
}


void
BoolOperand::GetValue(DBool& outValue) const
{
  outValue = mValue;
}

void
BoolOperand::GetValue(DText& outValue) const
{
  if (mValue.IsNull())
    outValue = DText();

  else if (mValue.mValue)
    outValue = DText("TRUE");

  else
    outValue = DText("FALSE");
}



void
BoolOperand::SetValue(const DBool& value)
{
  mValue = value;
}


void
BoolOperand::SelfAnd(const DBool& value)
{
  mValue = internal_and(mValue, value);
}


void
BoolOperand::SelfXor(const DBool& value)
{
  mValue = internal_xor(mValue, value);
}


void
BoolOperand::SelfOr(const DBool& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
BoolOperand::GetType()
{
  return T_BOOL;
}


StackValue
BoolOperand::Duplicate() const
{
  return  StackValue(*this);
}



bool
CharOperand::IsNull() const
{
  return mValue.IsNull();
}


void
CharOperand::GetValue(DChar& outValue) const
{
  outValue = mValue;
}


void
CharOperand::GetValue(DText& outValue) const
{
  DText text;

  text.Append(mValue);

  outValue = text;
}


void
CharOperand::SetValue(const DChar& value)
{
  mValue = value;
}


uint_t
CharOperand::GetType()
{
  return T_CHAR;
}


StackValue
CharOperand::Duplicate() const
{
  return StackValue(*this);
}



bool
DateOperand::IsNull() const
{
  return mValue.IsNull();
}


void
DateOperand::GetValue(DDate& outValue) const
{
  outValue = mValue;
}

void
DateOperand::GetValue(DDateTime& outValue) const
{
  outValue = mValue;
}


void
DateOperand::GetValue(DHiresTime& outValue) const
{
  outValue = mValue;
}


void
DateOperand::GetValue(DText& outValue) const
{
  uint8_t text[32];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}



void
DateOperand::SetValue(const DHiresTime& value)
{
  _CC(bool&,    mValue.mIsNull) = value.mIsNull;
  _CC(int16_t&, mValue.mYear)   = value.mYear;
  _CC(uint8_t&, mValue.mMonth)  = value.mMonth;
  _CC(uint8_t&, mValue.mDay)    = value.mDay;
}


uint_t
DateOperand::GetType()
{
  return T_DATE;
}


StackValue
DateOperand::Duplicate() const
{
  return StackValue(*this);
}



bool
DateTimeOperand::IsNull() const
{
  return mValue.IsNull();
}


void
DateTimeOperand::GetValue(DDate& outValue) const
{
  _CC(bool&,    outValue.mIsNull) = mValue.mIsNull;
  _CC(int16_t&, outValue.mYear)   = mValue.mYear;
  _CC(uint8_t&, outValue.mMonth)  = mValue.mMonth;
  _CC(uint8_t&, outValue.mDay)    = mValue.mDay;
}


void
DateTimeOperand::GetValue(DDateTime& outValue) const
{
  outValue = mValue;
}


void
DateTimeOperand::GetValue(DHiresTime& outValue) const
{
  outValue = mValue;
}


void
DateTimeOperand::GetValue(DText& outValue) const
{
  uint8_t text[32];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}

void
DateTimeOperand::SetValue(const DHiresTime& value)
{
  _CC(bool&,    mValue.mIsNull)   = value.mIsNull;
  _CC(int16_t&, mValue.mYear)     = value.mYear;
  _CC(uint8_t&, mValue.mMonth)    = value.mMonth;
  _CC(uint8_t&, mValue.mDay)      = value.mDay;
  _CC(uint8_t&, mValue.mDay)      = value.mDay;
  _CC(uint8_t&, mValue.mHour)     = value.mHour;
  _CC(uint8_t&, mValue.mMinutes)  = value.mMinutes;
  _CC(uint8_t&, mValue.mSeconds)  = value.mSeconds;
}


uint_t
DateTimeOperand::GetType()
{
  return T_DATETIME;
}


StackValue
DateTimeOperand::Duplicate() const
{
  return StackValue(*this);
}



bool
HiresTimeOperand::IsNull() const
{
  return mValue.IsNull();
}


void
HiresTimeOperand::GetValue(DDate& outValue) const
{
  _CC(bool&,    outValue.mIsNull) = mValue.mIsNull;
  _CC(int16_t&, outValue.mYear)   = mValue.mYear;
  _CC(uint8_t&, outValue.mMonth)  = mValue.mMonth;
  _CC(uint8_t&, outValue.mDay)    = mValue.mDay;
}


void
HiresTimeOperand::GetValue(DDateTime& outValue) const
{
  _CC(bool&,    outValue.mIsNull)   = mValue.mIsNull;
  _CC(int16_t&, outValue.mYear)     = mValue.mYear;
  _CC(uint8_t&, outValue.mMonth)    = mValue.mMonth;
  _CC(uint8_t&, outValue.mDay)      = mValue.mDay;
  _CC(uint8_t&, outValue.mHour)     = mValue.mHour;
  _CC(uint8_t&, outValue.mMinutes)  = mValue.mMinutes;
  _CC(uint8_t&, outValue.mSeconds)  = mValue.mSeconds;
}


void
HiresTimeOperand::GetValue(DHiresTime& outValue) const
{
  outValue = mValue;
}


void
HiresTimeOperand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}

void
HiresTimeOperand::SetValue(const DHiresTime& value)
{
  mValue = value;
}


uint_t
HiresTimeOperand::GetType()
{
  return T_HIRESTIME;
}


StackValue
HiresTimeOperand::Duplicate() const
{
  return StackValue(*this);
}



bool
UInt8Operand::IsNull() const
{
  return mValue.IsNull();
}


void
UInt8Operand::GetValue(DInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt8Operand::GetValue(DInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt8Operand::GetValue(DInt32& outValue) const
{
  number_convert(mValue, outValue);
}

void
UInt8Operand::GetValue(DInt64& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt8Operand::GetValue(DRichReal& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt8Operand::GetValue(DReal& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt8Operand::GetValue(DUInt8& outValue) const
{
  outValue = mValue;
}


void
UInt8Operand::GetValue(DUInt16& outValue) const
{
  outValue = mValue;
}


void
UInt8Operand::GetValue(DUInt32& outValue) const
{
  outValue = mValue;
}


void
UInt8Operand::GetValue(DUInt64& outValue) const
{
  outValue = mValue;
}


void
UInt8Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
UInt8Operand::SetValue(const DUInt64& value)
{
  number_convert(value, mValue);
}

void
UInt8Operand::SetValue(const DInt64& value)
{
  number_convert(value, mValue);
}


void
UInt8Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
UInt8Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
UInt8Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt8Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt8Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt8Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt8Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
UInt8Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
UInt8Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}

void
UInt8Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


void
UInt8Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
UInt8Operand::GetType()
{
  return T_UINT8;
}


StackValue
UInt8Operand::Duplicate() const
{
  return StackValue(*this);
}




bool
UInt16Operand::IsNull() const
{
  return mValue.IsNull();
}


void
UInt16Operand::GetValue(DInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt16Operand::GetValue(DInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt16Operand::GetValue(DInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt16Operand::GetValue(DInt64& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt16Operand::GetValue(DRichReal& outValue) const
{
  outValue = mValue.IsNull() ? DRichReal() : DRichReal(mValue.mValue);
}


void
UInt16Operand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
UInt16Operand::GetValue(DUInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt16Operand::GetValue(DUInt16& outValue) const
{
  outValue = mValue;
}


void
UInt16Operand::GetValue(DUInt32& outValue) const
{
  outValue = mValue;
}


void
UInt16Operand::GetValue(DUInt64& outValue) const
{
  outValue = mValue;
}


void
UInt16Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
UInt16Operand::SetValue(const DUInt64& value)
{
  number_convert(value, mValue);
}

void
UInt16Operand::SetValue(const DInt64& value)
{
  number_convert(value, mValue);
}


void
UInt16Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
UInt16Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
UInt16Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt16Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt16Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt16Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt16Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
UInt16Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
UInt16Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}


void
UInt16Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


void
UInt16Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
UInt16Operand::GetType()
{
  return T_UINT16;
}


StackValue
UInt16Operand::Duplicate() const
{
  return StackValue(*this);
}



bool
UInt32Operand::IsNull() const
{
  return mValue.IsNull();
}


void
UInt32Operand::GetValue(DInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt32Operand::GetValue(DInt16& outValue) const
{
  number_convert(mValue, outValue);

}


void
UInt32Operand::GetValue(DInt32& outValue) const
{
  number_convert(mValue, outValue);

}


void
UInt32Operand::GetValue(DInt64& outValue) const
{
  number_convert(mValue, outValue);

}


void
UInt32Operand::GetValue(DRichReal& outValue) const
{
  outValue = mValue.IsNull() ? DRichReal() : DRichReal(mValue.mValue);
}


void
UInt32Operand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
UInt32Operand::GetValue(DUInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt32Operand::GetValue(DUInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt32Operand::GetValue(DUInt32& outValue) const
{
  outValue = mValue;
}


void
UInt32Operand::GetValue(DUInt64& outValue) const
{
  outValue = mValue;
}


void
UInt32Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
UInt32Operand::SetValue(const DUInt64& value)
{
  number_convert(value, mValue);
}

void
UInt32Operand::SetValue(const DInt64& value)
{
  number_convert(value, mValue);
}


void
UInt32Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
UInt32Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
UInt32Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt32Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt32Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt32Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt32Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
UInt32Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
UInt32Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}


void
UInt32Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


void
UInt32Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
UInt32Operand::GetType()
{
  return T_UINT32;
}


StackValue
UInt32Operand::Duplicate() const
{
  return StackValue(*this);
}



bool
UInt64Operand::IsNull() const
{
  return mValue.IsNull();
}

void
UInt64Operand::GetValue(DInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt64Operand::GetValue(DInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt64Operand::GetValue(DInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt64Operand::GetValue(DInt64& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt64Operand::GetValue(DRichReal& outValue) const
{
  outValue = mValue.IsNull() ? DRichReal() : DRichReal(mValue.mValue);
}


void
UInt64Operand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
UInt64Operand::GetValue(DUInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt64Operand::GetValue(DUInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt64Operand::GetValue(DUInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
UInt64Operand::GetValue(DUInt64& outValue) const
{
  outValue = mValue;
}


void
UInt64Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
UInt64Operand::SetValue(const DUInt64& value)
{
  mValue = value;
}

void
UInt64Operand::SetValue(const DInt64& value)
{
  number_convert(value, mValue);
}


void
UInt64Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
UInt64Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}

void
UInt64Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt64Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
UInt64Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt64Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
UInt64Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
UInt64Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}


void
UInt64Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}

void
UInt64Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


void
UInt64Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
UInt64Operand::GetType()
{
  return T_UINT64;
}


StackValue
UInt64Operand::Duplicate() const
{
  return StackValue(*this);
}



bool
Int8Operand::IsNull() const
{
  return mValue.IsNull();
}


void
Int8Operand::GetValue(DInt8& outValue) const
{
  outValue = mValue;
}


void
Int8Operand::GetValue(DInt16& outValue) const
{
  outValue = mValue;
}


void
Int8Operand::GetValue(DInt32& outValue) const
{
  outValue = mValue;
}


void
Int8Operand::GetValue(DInt64& outValue) const
{
  outValue = mValue;
}


void
Int8Operand::GetValue(DRichReal& outValue) const
{
  outValue = mValue.IsNull() ? DRichReal() : DRichReal(mValue.mValue);
}


void
Int8Operand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
Int8Operand::GetValue(DUInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int8Operand::GetValue(DUInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int8Operand::GetValue(DUInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int8Operand::GetValue(DUInt64& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int8Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
Int8Operand::SetValue(const DUInt64& value)
{
  number_convert(value, mValue);
}

void
Int8Operand::SetValue(const DInt64& value)
{
  number_convert(value, mValue);
}

void
Int8Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
Int8Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
Int8Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
Int8Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}


void
Int8Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
Int8Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
Int8Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}


void
Int8Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
Int8Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}


void
Int8Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}

void
Int8Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
Int8Operand::GetType()
{
  return T_INT8;
}


StackValue
Int8Operand::Duplicate() const
{
  return StackValue(*this);
}



bool
Int16Operand::IsNull() const
{
  return mValue.IsNull();
}


void
Int16Operand::GetValue(DInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int16Operand::GetValue(DInt16& outValue) const
{
  outValue = mValue;
}


void
Int16Operand::GetValue(DInt32& outValue) const
{
  outValue = mValue;
}


void
Int16Operand::GetValue(DInt64& outValue) const
{
  outValue = mValue;
}


void
Int16Operand::GetValue(DRichReal& outValue) const
{
  outValue = mValue.IsNull() ? DRichReal() : DRichReal(mValue.mValue);
}


void
Int16Operand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
Int16Operand::GetValue(DUInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int16Operand::GetValue(DUInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int16Operand::GetValue(DUInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int16Operand::GetValue(DUInt64& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int16Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
Int16Operand::SetValue(const DUInt64& value)
{
  number_convert(value, mValue);
}

void
Int16Operand::SetValue(const DInt64& value)
{
  number_convert(value, mValue);
}


void
Int16Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
Int16Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
Int16Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}


void
Int16Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}



void
Int16Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
Int16Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}


void
Int16Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}


void
Int16Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}


void
Int16Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}


void
Int16Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


void
Int16Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
Int16Operand::GetType()
{
  return T_INT16;
}


StackValue
Int16Operand::Duplicate() const
{
  return StackValue(*this);
}



bool
Int32Operand::IsNull() const
{
  return mValue.IsNull();
}


void
Int32Operand::GetValue(DInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int32Operand::GetValue(DInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int32Operand::GetValue(DInt32& outValue) const
{
  outValue = mValue;
}


void
Int32Operand::GetValue(DInt64& outValue) const
{
  outValue = mValue;
}


void
Int32Operand::GetValue(DRichReal& outValue) const
{
  outValue = mValue.IsNull() ? DRichReal() : DRichReal(mValue.mValue);
}


void
Int32Operand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
Int32Operand::GetValue(DUInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int32Operand::GetValue(DUInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int32Operand::GetValue(DUInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int32Operand::GetValue(DUInt64& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int32Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
Int32Operand::SetValue(const DUInt64& value)
{
  number_convert(value, mValue);
}

void
Int32Operand::SetValue(const DInt64& value)
{
  number_convert(value, mValue);
}


void
Int32Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
Int32Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
Int32Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}


void
Int32Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}


void
Int32Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}


void
Int32Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
Int32Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
Int32Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}


void
Int32Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}


void
Int32Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


void
Int32Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
Int32Operand::GetType()
{
  return T_INT32;
}


StackValue
Int32Operand::Duplicate() const
{
  return StackValue(*this);
}



bool
Int64Operand::IsNull() const
{
  return mValue.IsNull();
}


void
Int64Operand::GetValue(DInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int64Operand::GetValue(DInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int64Operand::GetValue(DInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int64Operand::GetValue(DInt64& outValue) const
{
  outValue = mValue;
}


void
Int64Operand::GetValue(DRichReal& outValue) const
{
  outValue = mValue.IsNull() ? DRichReal() : DRichReal(mValue.mValue);
}


void
Int64Operand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
Int64Operand::GetValue(DUInt8& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int64Operand::GetValue(DUInt16& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int64Operand::GetValue(DUInt32& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int64Operand::GetValue(DUInt64& outValue) const
{
  number_convert(mValue, outValue);
}


void
Int64Operand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
Int64Operand::SetValue(const DUInt64& value)
{
  number_convert(value, mValue);
}

void
Int64Operand::SetValue(const DInt64& value)
{
  mValue = value;
}


void
Int64Operand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
Int64Operand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
Int64Operand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
Int64Operand::SelfMul(const DUInt64& value)
{
  mValue = internal_mul(mValue, value);
}

void
Int64Operand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}

void
Int64Operand::SelfDiv(const DUInt64& value)
{
  mValue = internal_div(mValue, value);
}


void
Int64Operand::SelfMod(const DInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
Int64Operand::SelfMod(const DUInt64& value)
{
  mValue = internal_mod(mValue, value);
}

void
Int64Operand::SelfAnd(const DInt64& value)
{
  mValue = internal_and(mValue, value);
}


void
Int64Operand::SelfXor(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


void
Int64Operand::SelfOr(const DInt64& value)
{
  mValue = internal_xor(mValue, value);
}


uint_t
Int64Operand::GetType()
{
  return T_INT64;
}


StackValue
Int64Operand::Duplicate() const
{
  return StackValue(*this);
}



bool
RealOperand::IsNull() const
{
  return mValue.IsNull();
}


void
RealOperand::GetValue(DReal& outValue) const
{
  outValue = mValue;
}


void
RealOperand::GetValue(DRichReal& outValue) const
{
  outValue = mValue;
}

void
RealOperand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
RealOperand::SetValue(const DRichReal& value)
{
  mValue = value.IsNull() ? DReal() : DReal(value.mValue);
}


void
RealOperand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
RealOperand::SelfAdd(const DRichReal& value)
{
  mValue = internal_add(mValue, value);
}


void
RealOperand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
RealOperand::SelfSub(const DRichReal& value)
{
  mValue = internal_sub(mValue, value);
}


void
RealOperand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}


void
RealOperand::SelfMul(const DRichReal& value)
{
  mValue = internal_mul(mValue, value);
}


void
RealOperand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}


void
RealOperand::SelfDiv(const DRichReal& value)
{
  mValue = internal_div(mValue, value);
}


uint_t
RealOperand::GetType()
{
  return T_REAL;
}


StackValue
RealOperand::Duplicate() const
{
  return StackValue(*this);
}



bool
RichRealOperand::IsNull() const
{
  return mValue.IsNull();
}


void
RichRealOperand::GetValue(DReal& outValue) const
{
  outValue = mValue.IsNull() ? DReal() : DReal(mValue.mValue);
}


void
RichRealOperand::GetValue(DRichReal& outValue) const
{
  outValue = mValue;
}


void
RichRealOperand::GetValue(DText& outValue) const
{
  uint8_t text[64];

  Utf8Translator::Write(text, sizeof text, mValue);
  outValue = DText(_RC(char*, text));
}


void
RichRealOperand::SetValue(const DRichReal& value)
{
  mValue = value;
}


void
RichRealOperand::SelfAdd(const DInt64& value)
{
  mValue = internal_add(mValue, value);
}


void
RichRealOperand::SelfAdd(const DRichReal& value)
{
  mValue = internal_add(mValue, value);
}


void
RichRealOperand::SelfSub(const DInt64& value)
{
  mValue = internal_sub(mValue, value);
}


void
RichRealOperand::SelfSub(const DRichReal& value)
{
  mValue = internal_sub(mValue, value);
}


void
RichRealOperand::SelfMul(const DInt64& value)
{
  mValue = internal_mul(mValue, value);
}


void
RichRealOperand::SelfMul(const DRichReal& value)
{
  mValue = internal_mul(mValue, value);
}


void
RichRealOperand::SelfDiv(const DInt64& value)
{
  mValue = internal_div(mValue, value);
}


void
RichRealOperand::SelfDiv(const DRichReal& value)
{
  mValue = internal_div(mValue, value);
}


uint_t
RichRealOperand::GetType()
{
  return T_RICHREAL;
}


StackValue
RichRealOperand::Duplicate() const
{
  return StackValue(*this);
}




bool
TextOperand::IsNull() const
{
  return (*mValue)->IsNull();
}


void
TextOperand::GetValue(DText& outValue) const
{
  outValue = **mValue;
}


void
TextOperand::SetValue(const DText& value)
{
  **mValue = value;
}


void
TextOperand::SelfAdd(const DChar& value)
{
  (*mValue)->Append(value);
}


void
TextOperand::SelfAdd(const DText& value)
{
  (*mValue)->Append(value);
}


StackValue
TextOperand::GetValueAt(const uint64_t index)
{
  return StackValue(CharTextElOperand(*mValue, index));
}


uint_t
TextOperand::GetType()
{
  return T_TEXT;
}


StackValue
TextOperand::Duplicate() const
{
  return StackValue(*this);
}


bool
TextOperand::StartIterate(const bool reverse, StackValue& outStartItem)
{
  if (IsNull())
    return false;

  assert((*mValue)->Count() > 0);

  outStartItem = GetValueAt(reverse ? (*mValue)->Count() - 1 : 0);
  return true;
}


bool
TextOperand::DoSimpleCopy(void* const dest)
{
  _placement_new(dest, *this);
  return false;
}




bool
CharTextElOperand::IsNull() const
{
  return (*mText)->Count() <= mIndex ? true : false;
}


void
CharTextElOperand::GetValue(DChar& outValue) const
{
  outValue = ((*mText)->Count() <= mIndex) ? DChar() : (*mText)->CharAt(mIndex);
}


void
CharTextElOperand::GetValue(DText& outValue) const
{
  DChar ch = (*mText)->CharAt(mIndex);
  DText text;

  text.Append(ch);
  outValue = text;
}


void
CharTextElOperand::SetValue(const DChar& value)
{
  (*mText)->CharAt(mIndex, value);
}


uint_t
CharTextElOperand::GetType()
{
  return T_CHAR;
}


StackValue
CharTextElOperand::Duplicate() const
{
  DChar ch;
  GetValue(ch);

  return StackValue(CharOperand(ch));
}


bool
CharTextElOperand::Iterate(const bool reverse)
{
  if (reverse)
    {
      if (mIndex == 0)
        return false;

      _CC(uint64_t&, mIndex)--;
      return true;
    }

  if (mIndex >= (*mText)->Count() - 1)
    return false;

  _CC(uint64_t&, mIndex)++;
  return true;
}


uint64_t
CharTextElOperand::IteratorOffset()
{
  return mIndex;
}

bool
CharTextElOperand::DoSimpleCopy(void* const dest)
{
  _placement_new(dest, *this);

  return false;
}



GlobalOperand::GlobalOperand(GlobalValue& value)
  : BaseOperand(),
    mValue(value)
{
}

bool
GlobalOperand::IsNull() const
{
  return mValue.IsNull();
}


void
GlobalOperand::GetValue(DBool& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DChar& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DDate& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DDateTime& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DHiresTime& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DInt8& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DInt16& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DInt32& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DInt64& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DReal& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DRichReal& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DUInt8& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DUInt16& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DUInt32& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DUInt64& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DText& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::GetValue(DArray& outValue) const
{
  mValue.GetValue(outValue);
}


void
GlobalOperand::SetValue(const DBool& outValue)
{
  mValue.SetValue(outValue);
}


void
GlobalOperand::SetValue(const DChar& outValue)
{
  mValue.SetValue(outValue);
}

void
GlobalOperand::SetValue(const DHiresTime& outValue)
{
  mValue.SetValue(outValue);
}


void
GlobalOperand::SetValue(const DUInt64& outValue)
{
  mValue.SetValue(outValue);
}


void
GlobalOperand::SetValue(const DInt64& outValue)
{
  mValue.SetValue(outValue);
}


void
GlobalOperand::SetValue(const DRichReal& outValue)
{
  mValue.SetValue(outValue);
}


void
GlobalOperand::SetValue(const DText& outValue)
{
  mValue.SetValue(outValue);
}


void
GlobalOperand::SetValue(const DArray& outValue)
{
  mValue.SetValue(outValue);
}


void
GlobalOperand::SelfAdd(const DInt64& value)
{
  mValue.SelfAdd(value);
}


void
GlobalOperand::SelfAdd(const DRichReal& value)
{
  mValue.SelfAdd(value);
}


void
GlobalOperand::SelfAdd(const DChar& value)
{
  mValue.SelfAdd(value);
}


void
GlobalOperand::SelfAdd(const DText& value)
{
  mValue.SelfAdd(value);
}


void
GlobalOperand::SelfSub(const DInt64& value)
{
  mValue.SelfSub(value);
}


void
GlobalOperand::SelfSub(const DRichReal& value)
{
  mValue.SelfSub(value);
}


void
GlobalOperand::SelfMul(const DInt64& value)
{
  mValue.SelfMul(value);
}


void
GlobalOperand::SelfMul(const DUInt64& value)
{
  mValue.SelfMul(value);
}

void
GlobalOperand::SelfMul(const DRichReal& value)
{
  mValue.SelfMul(value);
}


void
GlobalOperand::SelfDiv(const DInt64& value)
{
  mValue.SelfDiv(value);
}


void
GlobalOperand::SelfDiv(const DUInt64& value)
{
  mValue.SelfDiv(value);
}


void
GlobalOperand::SelfDiv(const DRichReal& value)
{
  mValue.SelfDiv(value);
}


void
GlobalOperand::SelfMod(const DInt64& value)
{
  mValue.SelfMod(value);
}


void
GlobalOperand::SelfMod(const DUInt64& value)
{
  mValue.SelfMod(value);
}

void
GlobalOperand::SelfAnd(const DInt64& value)
{
  mValue.SelfAnd(value);
}


void
GlobalOperand::SelfAnd(const DBool& value)
{
  mValue.SelfAnd(value);
}


void
GlobalOperand::SelfXor(const DInt64& value)
{
  mValue.SelfXor(value);
}


void
GlobalOperand::SelfXor(const DBool& value)
{
  mValue.SelfXor(value);
}


void
GlobalOperand::SelfOr(const DInt64& value)
{
  mValue.SelfOr(value);
}


void
GlobalOperand::SelfOr(const DBool& value)
{
  mValue.SelfOr(value);
}


uint_t
GlobalOperand::GetType()
{
  return mValue.GetType();
}


FIELD_INDEX
GlobalOperand::GetField()
{
  return mValue.GetField();
}


ITable&
GlobalOperand::GetTable()
{
  return mValue.GetTable();
}


StackValue
GlobalOperand::GetFieldAt(const FIELD_INDEX field)
{
  return mValue.GetFieldAt(field);
}


StackValue
GlobalOperand::GetValueAt(const uint64_t index)
{
  return mValue.GetValueAt(index);
}


StackValue
GlobalOperand::Duplicate() const
{
  return mValue.Duplicate();
}


bool
GlobalOperand::StartIterate(const bool reverse, StackValue& outStartItem)
{
  return mValue.StartIterate(reverse, outStartItem);
}

void
GlobalOperand::NativeObject(INativeObject* const value)
{
  mValue.NativeObject(value);
}


INativeObject&
GlobalOperand::NativeObject()
{
  return mValue.NativeObject();
}


TableReference&
GlobalOperand::GetTableReference()
{
  return mValue.GetTableReference();
}


bool
GlobalOperand::DoSimpleCopy(void* const)
{
  return true;
}


TableOperand
GlobalOperand::GetTableOp()
{
  return mValue.GetTableOp();
}


void
GlobalOperand::CopyTableOp(const TableOperand& tableOp)
{
  mValue.CopyTableOp(tableOp);
}


FieldOperand
GlobalOperand::GetFieldOp()
{
  return mValue.GetFieldOp();
}


void
GlobalOperand::CopyFieldOp(const FieldOperand& fieldOp)
{
  mValue.CopyFieldOp(fieldOp);
}


void
GlobalOperand::CopyNativeObjectOperand(const NativeObjectOperand& source)
{
  mValue.CopyNativeObjectOperand(source);
}



LocalOperand::LocalOperand(SessionStack& stack, const uint64_t index)
  : BaseOperand(),
    mIndex(index),
    mStack(stack)
{
}


bool
LocalOperand::IsNull() const
{
  return mStack[mIndex].Operand().IsNull();
}


void
LocalOperand::GetValue(DBool& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DChar& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DDate& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DDateTime& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DHiresTime& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DInt8& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DInt16& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DInt32& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DInt64& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DReal& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DRichReal& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DUInt8& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DUInt16& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DUInt32& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DUInt64& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DText& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::GetValue(DArray& outValue) const
{
  mStack[mIndex].Operand().GetValue(outValue);
}


void
LocalOperand::SetValue(const DBool& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}


void
LocalOperand::SetValue(const DChar& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}


void
LocalOperand::SetValue(const DHiresTime& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}


void
LocalOperand::SetValue(const DUInt64& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}


void
LocalOperand::SetValue(const DInt64& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}

void
LocalOperand::SetValue(const DRichReal& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}


void
LocalOperand::SetValue(const DText& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}


void
LocalOperand::SetValue(const DArray& outValue)
{
  mStack[mIndex].Operand().SetValue(outValue);
}


void
LocalOperand::SelfAdd(const DInt64& value)
{
  mStack[mIndex].Operand().SelfAdd(value);
}


void
LocalOperand::SelfAdd(const DRichReal& value)
{
  mStack[mIndex].Operand().SelfAdd(value);
}


void
LocalOperand::SelfAdd(const DChar& value)
{
  mStack[mIndex].Operand().SelfAdd(value);
}


void
LocalOperand::SelfAdd(const DText& value)
{
  mStack[mIndex].Operand().SelfAdd(value);
}


void
LocalOperand::SelfSub(const DInt64& value)
{
  mStack[mIndex].Operand().SelfSub(value);
}


void
LocalOperand::SelfSub(const DRichReal& value)
{
  mStack[mIndex].Operand().SelfSub(value);
}


void
LocalOperand::SelfMul(const DInt64& value)
{
  mStack[mIndex].Operand().SelfMul(value);
}

void
LocalOperand::SelfMul(const DUInt64& value)
{
  mStack[mIndex].Operand().SelfMul(value);
}

void
LocalOperand::SelfMul(const DRichReal& value)
{
  mStack[mIndex].Operand().SelfMul(value);
}


void
LocalOperand::SelfDiv(const DInt64& value)
{
  mStack[mIndex].Operand().SelfDiv(value);
}

void
LocalOperand::SelfDiv(const DUInt64& value)
{
  mStack[mIndex].Operand().SelfDiv(value);
}

void
LocalOperand::SelfDiv(const DRichReal& value)
{
  mStack[mIndex].Operand().SelfDiv(value);
}

void
LocalOperand::SelfMod(const DInt64& value)
{
  mStack[mIndex].Operand().SelfMod(value);
}

void
LocalOperand::SelfMod(const DUInt64& value)
{
  mStack[mIndex].Operand().SelfMod(value);
}

void
LocalOperand::SelfAnd(const DInt64& value)
{
  mStack[mIndex].Operand().SelfAnd(value);
}


void
LocalOperand::SelfAnd(const DBool& value)
{
  mStack[mIndex].Operand().SelfAnd(value);
}


void
LocalOperand::SelfXor(const DInt64& value)
{
  mStack[mIndex].Operand().SelfXor(value);
}


void
LocalOperand::SelfXor(const DBool& value)
{
  mStack[mIndex].Operand().SelfXor(value);
}


void
LocalOperand::SelfOr(const DInt64& value)
{
  mStack[mIndex].Operand().SelfOr(value);
}


void
LocalOperand::SelfOr(const DBool& value)
{
  mStack[mIndex].Operand().SelfOr(value);
}


uint_t
LocalOperand::GetType()
{
  return mStack[mIndex].Operand().GetType();
}


FIELD_INDEX
LocalOperand::GetField()
{
  return mStack[mIndex].Operand().GetField();
}


ITable&
LocalOperand::GetTable()
{
  return mStack[mIndex].Operand().GetTable();
}


StackValue
LocalOperand::GetFieldAt(const FIELD_INDEX field)
{
  return mStack[mIndex].Operand().GetFieldAt(field);
}


StackValue
LocalOperand::GetValueAt(const uint64_t index)
{
  return mStack[mIndex].Operand().GetValueAt(index);
}


void
LocalOperand::NativeObject(INativeObject* const value)
{
  mStack[mIndex].Operand().NativeObject(value);
}


INativeObject&
LocalOperand::NativeObject()
{
  return mStack[mIndex].Operand().NativeObject();
}


TableReference&
LocalOperand::GetTableReference()
{
  return _SC(BaseOperand&, mStack[mIndex].Operand()).GetTableReference();
}


StackValue
LocalOperand::Duplicate() const
{
  return mStack[mIndex].Operand().Duplicate();
}


bool
LocalOperand::StartIterate(const bool reverse, StackValue& outStartItem)
{
  return mStack[mIndex].Operand().StartIterate(reverse, outStartItem);
}


bool
LocalOperand::Iterate(const bool reverse)
{
  return mStack[mIndex].Operand().Iterate(reverse);
}

uint64_t
LocalOperand::IteratorOffset()
{
  return mStack[mIndex].Operand().IteratorOffset();
}


TableOperand
LocalOperand::GetTableOp()
{
  BaseOperand& op = _SC(BaseOperand&, mStack[mIndex].Operand());
  return op.GetTableOp();
}


void
LocalOperand::CopyTableOp(const TableOperand& tableOp)
{
  BaseOperand& op = _SC(BaseOperand&, mStack[mIndex].Operand());
  op.CopyTableOp(tableOp);
}


FieldOperand
LocalOperand::GetFieldOp()
{
  BaseOperand& op = _SC(BaseOperand&, mStack[mIndex].Operand());
  return op.GetFieldOp();
}


void
LocalOperand::CopyFieldOp(const FieldOperand& fieldOp)
{
  BaseOperand& op = _SC(BaseOperand&, mStack[mIndex].Operand());
  op.CopyFieldOp(fieldOp);
}


void
LocalOperand::CopyNativeObjectOperand(const NativeObjectOperand& source)
{
  BaseOperand& op = _SC(BaseOperand&, mStack[mIndex].Operand());
  op.CopyNativeObjectOperand(source);
}



} //namespace prima



using namespace prima;

StackValue
StackValue::Create(const DBool& value)
{
  return StackValue(BoolOperand(value));
}


StackValue
StackValue::Create(const DChar& value)
{
  return StackValue(CharOperand(value));
}


StackValue
StackValue::Create(const DDate& value)
{
  return StackValue(DateOperand(value));
}


StackValue
StackValue::Create(const DDateTime& value)
{
  return StackValue(DateTimeOperand(value));
}


StackValue
StackValue::Create(const DHiresTime& value)
{
  return StackValue(HiresTimeOperand(value));
}


StackValue
StackValue::Create(const DInt8& value)
{
  return StackValue(Int8Operand(value));
}


StackValue
StackValue::Create(const DInt16& value)
{
  return StackValue(Int16Operand(value));
}

StackValue
StackValue::Create(const DInt32& value)
{
  return StackValue(Int32Operand(value));
}


StackValue
StackValue::Create(const DInt64& value)
{
  return StackValue(Int64Operand(value));
}


StackValue
StackValue::Create(const DUInt8& value)
{
  return StackValue(UInt8Operand(value));
}


StackValue
StackValue::Create(const DUInt16& value)
{
  return StackValue(UInt16Operand(value));
}


StackValue
StackValue::Create(const DUInt32& value)
{
  return StackValue(UInt32Operand(value));
}


StackValue
StackValue::Create(const DUInt64& value)
{
  return StackValue(UInt64Operand(value));
}


StackValue
StackValue::Create(const DReal& value)
{
  return StackValue(RealOperand(value));
}


StackValue
StackValue::Create(const DRichReal& value)
{
  return StackValue(RichRealOperand(value));
}


StackValue
StackValue::Create(const DText& value)
{
  return StackValue(TextOperand(value));
}


StackValue
StackValue::Create(const DArray& value)
{
  return StackValue(ArrayOperand(value));
}




SessionStack::SessionStack()
  : mStack()
{
}


SessionStack::~SessionStack()
{
  if (mStack.size() > 0)
    Pop(mStack.size());
}


void
SessionStack::Push()
{
  Push(StackValue(NullOperand()));
}

void
SessionStack::Push(const DBool& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DChar& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DDate& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DDateTime& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DHiresTime& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DInt8& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DInt16& value)
{
  Push(StackValue::Create(value));
}

void
SessionStack::Push(const DInt32& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DInt64& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DUInt8& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DUInt16& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DUInt32& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DUInt64& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DReal& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DRichReal& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DText& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(const DArray& value)
{
  Push(StackValue::Create(value));
}


void
SessionStack::Push(IDBSHandler& dbsHnd, ITable& table)
{
  Push(StackValue(TableOperand(dbsHnd, table, true)));
}


void
SessionStack::Push(INativeObject& object)
{
  NativeObjectOperand stackOp(object);

  StackValue stackValue(stackOp);

  Push(StackValue(NativeObjectOperand(object)));
}

void
SessionStack::Push(StackValue&& value)
{
  mStack.push_back(move(value));
}



void
SessionStack::Pop(const uint_t count)
{
  if (count > mStack.size())
    throw InterException(_EXTRA(InterException::STACK_CORRUPTED));

  uint_t topIndex = mStack.size() - 1;

  for (uint_t index = 0; index < count; ++index, --topIndex)
    mStack.pop_back();
}


size_t
SessionStack::Size() const
{
  return mStack.size();
}


StackValue&
SessionStack::operator[] (const uint_t index)
{
  if (index > mStack.size())
    throw InterException(_EXTRA(InterException::STACK_CORRUPTED));

  return mStack[index];
}


} //namespace whais

