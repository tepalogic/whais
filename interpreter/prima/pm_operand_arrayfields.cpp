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
#include "pm_typemanager.h"

using namespace std;



namespace whais {
namespace prima {



bool
CharTextFieldElOperand::IsNull() const
{
  ITable& table = mTableRef->GetTable();

  DText text;
  table.Get(mRow, mField, text);

  return(text.Count() <= mIndex) ? true : false;
}


void
CharTextFieldElOperand::GetValue(DChar& outValue) const
{
  ITable& table = mTableRef->GetTable();

  if (table.AllocatedRows() <= mRow)
    {
      outValue = DChar();
      return;
    }

  DText text;
  table.Get(mRow, mField, text);

  if (text.Count() <= mIndex)
    {
      outValue = DChar();
      return;
    }

  outValue = text.CharAt(mIndex);
}


void
CharTextFieldElOperand::GetValue(DText& outValue) const
{
  DChar ch;

  this->GetValue(ch);

  outValue = DText();
  outValue.Append(ch);
}


void
CharTextFieldElOperand::SetValue(const DChar& value)
{
  ITable& table = mTableRef->GetTable();

  DText text;

  if (mRow < table.AllocatedRows())
    table.Get(mRow, mField, text);

  text.CharAt(mIndex, value);
  table.Set(mRow, mField, text);
}


uint_t
CharTextFieldElOperand::GetType()
{
  return T_CHAR;
}


StackValue
CharTextFieldElOperand::Duplicate() const
{
  DChar ch;

  Get(ch);

  return StackValue(CharOperand(ch));
}


bool
CharTextFieldElOperand::Iterate(const bool reverse)
{
  ITable& table = mTableRef->GetTable();

  assert(mRow < table.AllocatedRows());

  DText temp;
  table.Get(mRow, mField, temp);

  const uint64_t maxCount = temp.Count() - 1;
  if (maxCount == 0)
    return false;

  if (reverse)
    {
      if (mIndex == 0)
        return false;

      _CC(uint64_t&, mIndex) = min(mIndex - 1, maxCount);
    }

  if (mIndex >= maxCount)
    return false;

  _CC(uint64_t&, mIndex)++;
  return true;
}


uint64_t
CharTextFieldElOperand::IteratorOffset()
{
  return mIndex;
}


BaseArrayFieldElOperand::~BaseArrayFieldElOperand()
{
  mTableRef->DecrementRefCount();
}


bool
BaseArrayFieldElOperand::IsNull() const
{
  ITable& table = mTableRef->GetTable();

  if (table.AllocatedRows() <= mRow)
    return true;

  DArray array;
  table.Get(mRow, mField, array);

  return(array.Count() <= mIndex) ? true : false;
}


bool
BaseArrayFieldElOperand::Iterate(const bool reverse)
{
  ITable& table = mTableRef->GetTable();

  assert(mRow < table.AllocatedRows());

  DArray array;
  table.Get(mRow, mField, array);

  const uint64_t maxCount = array.Count() - 1;
  if (maxCount == 0)
    return false;

  if (reverse)
    {
      if (mIndex == 0)
        return false;

      _CC(uint64_t&, mIndex) = min(mIndex - 1, maxCount);
      return true;
    }

  if (mIndex >= maxCount)
    return false;

  _CC(uint64_t&, mIndex)++;
  return true;
}


uint64_t
BaseArrayFieldElOperand::IteratorOffset()
{
  return mIndex;
}


bool
BaseArrayFieldElOperand::PrepareToCopy(void* const)
{
  mTableRef->IncrementRefCount();

  return true;
}



void
BoolArrayFieldElOperand::GetValue(DBool& outValue) const
{
  Get(outValue);
}

void
BoolArrayFieldElOperand::GetValue(DText& outValue) const
{
  DBool currValue;
  Get(currValue);

  if (currValue.IsNull())
    outValue = DText();

  else if (currValue.mValue)
    outValue = DText("TRUE");

  else
    outValue = DText("FALSE");
}

void
BoolArrayFieldElOperand::SetValue(const DBool& value)
{
  Set(value);
}


void
BoolArrayFieldElOperand::SelfAnd(const DBool& value)
{
  DBool currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
BoolArrayFieldElOperand::SelfXor(const DBool& value)
{
  DBool currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
BoolArrayFieldElOperand::SelfOr(const DBool& value)
{
  DBool currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
BoolArrayFieldElOperand::GetType()
{
  return T_BOOL;
}


StackValue
BoolArrayFieldElOperand::Duplicate() const
{
  DBool value;
  Get(value);

  return StackValue(BoolOperand(value));
}



void
CharArrayFieldElOperand::GetValue(DChar& outValue) const
{
  Get(outValue);
}


void
CharArrayFieldElOperand::GetValue(DText& outValue) const
{
  DChar ch;
  Get(ch);

  outValue = DText();
  outValue.Append(ch);
}


void
CharArrayFieldElOperand::SetValue(const DChar& value)
{
  Set(value);
}


uint_t
CharArrayFieldElOperand::GetType()
{
  return T_CHAR;
}


StackValue
CharArrayFieldElOperand::Duplicate() const
{
  DChar ch;
  Get(ch);

  return StackValue(CharOperand(ch));
}




void
DateArrayFieldElOperand::GetValue(DDate& outValue) const
{
  Get(outValue);
}


void
DateArrayFieldElOperand::GetValue(DDateTime& outValue) const
{
  DDate currValue;
  Get(currValue);

  outValue = currValue;
}


void
DateArrayFieldElOperand::GetValue(DHiresTime& outValue) const
{
  DDate currValue;
  Get(currValue);

  outValue = currValue;
}


void
DateArrayFieldElOperand::GetValue(DText& outValue) const
{
  DDate   currValue;
  uint8_t text[32];
  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);

  outValue = DText(_RC(char*, text));
}


void
DateArrayFieldElOperand::SetValue(const DHiresTime& value)
{
  DDate v;

  _CC(bool&,    v.mIsNull) = value.mIsNull;
  _CC(int16_t&, v.mYear)   = value.mYear;
  _CC(uint8_t&, v.mMonth)  = value.mMonth;
  _CC(uint8_t&, v.mDay)    = value.mDay;

  Set(v);
}


uint_t
DateArrayFieldElOperand::GetType()
{
  return T_DATE;
}


StackValue
DateArrayFieldElOperand::Duplicate() const
{
  DDate value;

  Get(value);

  return StackValue(DateOperand(value));
}



void
DateTimeArrayFieldElOperand::GetValue(DDate& outValue) const
{
  DDateTime currValue;
  Get(currValue);

  _CC(bool&  ,   outValue.mIsNull) = currValue.mIsNull;
  _CC(int16_t&,  outValue.mYear)   = currValue.mYear;
  _CC(uint8_t&,  outValue.mMonth)  = currValue.mMonth;
  _CC(uint8_t&,  outValue.mDay)    = currValue.mDay;
}


void
DateTimeArrayFieldElOperand::GetValue(DDateTime& outValue) const
{
  Get(outValue);
}


void
DateTimeArrayFieldElOperand::GetValue(DHiresTime& outValue) const
{
  DDateTime currValue;
  Get(currValue);

  outValue = currValue;
}


void
DateTimeArrayFieldElOperand::GetValue(DText& outValue) const
{
  DDateTime currValue;
  uint8_t   text[32];

  Get(currValue);
  Utf8Translator::Write(text, sizeof text, currValue);

  outValue = DText(_RC(char*, text));
}


void
DateTimeArrayFieldElOperand::SetValue(const DHiresTime& value)
{
  DDateTime v;

  _CC(bool&,    v.mIsNull)   = value.mIsNull;
  _CC(int16_t&, v.mYear)     = value.mYear;
  _CC(uint8_t&, v.mMonth)    = value.mMonth;
  _CC(uint8_t&, v.mDay)      = value.mDay;
  _CC(uint8_t&, v.mHour)     = value.mHour;
  _CC(uint8_t&, v.mMinutes)  = value.mMinutes;
  _CC(uint8_t&, v.mSeconds)  = value.mSeconds;

  Set(v);
}


uint_t
DateTimeArrayFieldElOperand::GetType()
{
  return T_DATETIME;
}


StackValue
DateTimeArrayFieldElOperand::Duplicate() const
{
  DDateTime value;

  Get(value);

  return StackValue(DateTimeOperand(value));
}



void
HiresTimeArrayFieldElOperand::GetValue(DDate& outValue) const
{
  DHiresTime currValue;
  Get(currValue);

  _CC(bool&,    outValue.mIsNull) = currValue.mIsNull;
  _CC(int16_t&, outValue.mYear)   = currValue.mYear;
  _CC(uint8_t&, outValue.mMonth)  = currValue.mMonth;
  _CC(uint8_t&, outValue.mDay)    = currValue.mDay;;
}


void
HiresTimeArrayFieldElOperand::GetValue(DDateTime& outValue) const
{
  DHiresTime currValue;
  Get(currValue);

  _CC(bool&,    outValue.mIsNull)   = currValue.mIsNull;
  _CC(int16_t&, outValue.mYear)     = currValue.mYear;
  _CC(uint8_t&, outValue.mMonth)    = currValue.mMonth;
  _CC(uint8_t&, outValue.mDay)      = currValue.mDay;
  _CC(uint8_t&, outValue.mHour)     = currValue.mHour;
  _CC(uint8_t&, outValue.mMinutes)  = currValue.mMinutes;
  _CC(uint8_t&, outValue.mSeconds)  = currValue.mSeconds;
}


void
HiresTimeArrayFieldElOperand::GetValue(DHiresTime& outValue) const
{
  Get(outValue);
}


void
HiresTimeArrayFieldElOperand::GetValue(DText& outValue) const
{
  DHiresTime currValue;
  uint8_t    text[32];

  Get(currValue);
  Utf8Translator::Write(text, sizeof text, currValue);

  outValue = DText(_RC(char*, text));
}


void
HiresTimeArrayFieldElOperand::SetValue(const DHiresTime& value)
{
  Set(value);
}


uint_t
HiresTimeArrayFieldElOperand::GetType()
{
  return T_HIRESTIME;
}


StackValue
HiresTimeArrayFieldElOperand::Duplicate() const
{
  DHiresTime value;
  Get(value);

  return StackValue(HiresTimeOperand(value));
}



void
UInt8ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  Get(outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DUInt8  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt8ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  DUInt8  v;

  number_convert(value, v);
  Set(v);
}

void
UInt8ArrayFieldElOperand::SetValue(const DInt64& value)
{
  DUInt8  v;

  number_convert(value, v);
  Set(v);
}


void
UInt8ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt8ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt8ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt8ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt8ArrayFieldElOperand::GetType()
{
  return T_UINT8;
}


StackValue
UInt8ArrayFieldElOperand::Duplicate() const
{
  DUInt8 value;
  Get(value);

  return StackValue(UInt8Operand(value));
}



void
UInt16ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  Get(outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DUInt16  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt16ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  DUInt16  v;

  number_convert(value, v);
  Set(v);
}

void
UInt16ArrayFieldElOperand::SetValue(const DInt64& value)
{
  DUInt16  v;

  number_convert(value, v);
  Set(v);
}


void
UInt16ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt16ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
UInt16ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt16ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt16ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt16ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt16ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt16ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt16ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt16ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt16ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt16ArrayFieldElOperand::GetType()
{
  return T_UINT16;
}


StackValue
UInt16ArrayFieldElOperand::Duplicate() const
{
  DUInt16 value;
  Get(value);

  return StackValue(UInt16Operand(value));
}



void
UInt32ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  Get(outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DUInt32  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt32ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  DUInt32  v;

  number_convert(value, v);
  Set(v);
}

void
UInt32ArrayFieldElOperand::SetValue(const DInt64& value)
{
  DUInt32  v;

  number_convert(value, v);
  Set(v);
}


void
UInt32ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt32ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt32ArrayFieldElOperand::GetType()
{
  return T_UINT32;
}


StackValue
UInt32ArrayFieldElOperand::Duplicate() const
{
  DUInt32 value;
  Get(value);

  return StackValue(UInt32Operand(value));
}



void
UInt64ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  Get(outValue);
}


void
UInt64ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DUInt64  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt64ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  Set(value);
}

void
UInt64ArrayFieldElOperand::SetValue(const DInt64& value)
{
  DUInt64  v;

  number_convert(value, v);
  Set(v);
}


void
UInt64ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt64ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}

void
UInt64ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt64ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt64ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt64ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt64ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt64ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt64ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt64ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt64ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt64ArrayFieldElOperand::GetType()
{
  return T_UINT64;
}


StackValue
UInt64ArrayFieldElOperand::Duplicate() const
{
  DUInt64 value;
  Get(value);

  return StackValue(UInt64Operand(value));
}



void
Int8ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  Get(outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DInt8   currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int8ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  DInt8  v;

  number_convert(value, v);
  Set(v);
}

void
Int8ArrayFieldElOperand::SetValue(const DInt64& value)
{
  DInt8  v;

  number_convert(value, v);
  Set(v);
}


void
Int8ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int8ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int8ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
Int8ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
Int8ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int8ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int8ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int8ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int8ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int8ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int8ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int8ArrayFieldElOperand::GetType()
{
  return T_INT8;
}


StackValue
Int8ArrayFieldElOperand::Duplicate() const
{
  DInt8 value;
  Get(value);

  return StackValue(Int8Operand(value));
}



void
Int16ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  Get(outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DInt16  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int16ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  DInt16  v;

  number_convert(value, v);
  Set(v);
}


void
Int16ArrayFieldElOperand::SetValue(const DInt64& value)
{
  DInt16  v;

  number_convert(value, v);
  Set(v);
}


void
Int16ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int16ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int16ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
Int16ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
Int16ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int16ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int16ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int16ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
Int16ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int16ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int16ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int16ArrayFieldElOperand::GetType()
{
  return T_INT16;
}


StackValue
Int16ArrayFieldElOperand::Duplicate() const
{
  DInt16 value;

  Get(value);

  return StackValue(Int16Operand(value));
}



void
Int32ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  Get(outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DInt32  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int32ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  DInt32  v;

  number_convert(value, v);
  Set(v);
}


void
Int32ArrayFieldElOperand::SetValue(const DInt64& value)
{
  DInt32  v;

  number_convert(value, v);
  Set(v);
}


void
Int32ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int32ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int32ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int32ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int32ArrayFieldElOperand::GetType()
{
  return T_INT32;
}


StackValue
Int32ArrayFieldElOperand::Duplicate() const
{
  DInt32 value;
  Get(value);

  return StackValue(Int32Operand(value));
}



void
Int64ArrayFieldElOperand::GetValue(DInt8& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DInt16& outValue) const
{
  DInt64 currValue;

  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DInt32& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DInt64& outValue) const
{
  Get(outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue(DText& outValue) const
{
  DInt64  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int64ArrayFieldElOperand::SetValue(const DUInt64& value)
{
  DInt32  v;

  number_convert(value, v);
  Set(v);
}


void
Int64ArrayFieldElOperand::SetValue(const DInt64& value)
{
  Set(value);
}


void
Int64ArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int64ArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int64ArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
Int64ArrayFieldElOperand::SelfMul(const DUInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
Int64ArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int64ArrayFieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int64ArrayFieldElOperand::SelfMod(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int64ArrayFieldElOperand::SelfMod(const DUInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int64ArrayFieldElOperand::SelfAnd(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int64ArrayFieldElOperand::SelfXor(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int64ArrayFieldElOperand::SelfOr(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int64ArrayFieldElOperand::GetType()
{
  return T_INT64;
}


StackValue
Int64ArrayFieldElOperand::Duplicate() const
{
  DInt64 value;
  Get(value);

  return StackValue(Int64Operand(value));
}



void
RealArrayFieldElOperand::GetValue(DReal& outValue) const
{
  Get(outValue);
}


void
RealArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  DReal currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
RealArrayFieldElOperand::GetValue(DText& outValue) const
{
  DReal   currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
RealArrayFieldElOperand::SetValue(const DRichReal& value)
{
  Set(value.IsNull() ? DReal() : DReal(value.mValue));
}


void
RealArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}



void
RealArrayFieldElOperand::SelfAdd(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
RealArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RealArrayFieldElOperand::SelfSub(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RealArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RealArrayFieldElOperand::SelfMul(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RealArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
RealArrayFieldElOperand::SelfDiv(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


uint_t
RealArrayFieldElOperand::GetType()
{
  return T_REAL;
}


StackValue
RealArrayFieldElOperand::Duplicate() const
{
  DReal value;

  Get(value);

  return StackValue(RealOperand(value));
}


void
RichRealArrayFieldElOperand::GetValue(DReal& outValue) const
{
  DRichReal currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
RichRealArrayFieldElOperand::GetValue(DRichReal& outValue) const
{
  Get(outValue);
}


void
RichRealArrayFieldElOperand::GetValue(DText& outValue) const
{
  DRichReal currValue;
  uint8_t   text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
RichRealArrayFieldElOperand::SetValue(const DRichReal& value)
{
  Set(value);
}


void
RichRealArrayFieldElOperand::SelfAdd(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
RichRealArrayFieldElOperand::SelfAdd(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
RichRealArrayFieldElOperand::SelfSub(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RichRealArrayFieldElOperand::SelfSub(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RichRealArrayFieldElOperand::SelfMul(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RichRealArrayFieldElOperand::SelfMul(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RichRealArrayFieldElOperand::SelfDiv(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
RichRealArrayFieldElOperand::SelfDiv(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


uint_t
RichRealArrayFieldElOperand::GetType()
{
  return T_RICHREAL;
}


StackValue
RichRealArrayFieldElOperand::Duplicate() const
{
  DRichReal value;
  Get(value);

  return StackValue(RichRealOperand(value));
}

} //namespace prima
} //namespace whais

