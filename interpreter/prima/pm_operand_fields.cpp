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



TableOperand::~TableOperand()
{
  mTableRef->DecrementRefCount();
}


bool
TableOperand::IsNull() const
{
  return(mTableRef->GetTable().AllocatedRows() == 0);
}


uint_t
TableOperand::GetType()
{
  uint_t type = 0;

  MARK_TABLE(type);

  return type;
}


StackValue
TableOperand::GetFieldAt(const FIELD_INDEX field)
{
  return StackValue(FieldOperand(GetTableReference(), field));
}


ITable&
TableOperand::GetTable()
{
  return TableOperand::GetTableReference().GetTable();
}


StackValue
TableOperand::Duplicate() const
{
  return StackValue(*this);
}


bool
TableOperand::PrepareToCopy(void* const)
{
  mTableRef->IncrementRefCount();
  return true;
}


TableOperand
TableOperand::GetTableOp()
{
  return *this;
}


void
TableOperand::CopyTableOp(const TableOperand& source)
{
  *this = source;
}


TableReference&
TableOperand::GetTableReference()
{
  assert(mTableRef != nullptr);

  if ( ! mChangeable)
    {
      TableReference* const temp = mTableRef->Spawn();

      mTableRef->DecrementRefCount();
      mTableRef = temp;
      mTableRef->IncrementRefCount();
      mChangeable = true;
    }

  return *mTableRef;
}


FieldOperand::FieldOperand(TableOperand& tableOp, const FIELD_INDEX field)
  : BaseOperand(),
    mTableRef(&tableOp.GetTableReference()),
    mField(field)
{
  mTableRef->IncrementRefCount();

  ITable&                  table     = mTableRef->GetTable();
  const DBSFieldDescriptor fieldDesc = table.DescribeField(field);

  mFieldType = fieldDesc.type;

  assert((mFieldType > T_UNKNOWN) && (mFieldType < T_UNDETERMINED));

  if (fieldDesc.isArray)
    {
      assert(mFieldType != T_TEXT);

      MARK_ARRAY(mFieldType);
    }
}


FieldOperand::FieldOperand(TableReference& tableRef, const FIELD_INDEX field)
  : BaseOperand(),
    mTableRef(&tableRef),
    mField(field)
{
  mTableRef->IncrementRefCount();

  ITable&                  table     = mTableRef->GetTable();
  const DBSFieldDescriptor fieldDesc = table.DescribeField(field);

  mFieldType = fieldDesc.type;

  assert((mFieldType > T_UNKNOWN) && (mFieldType < T_UNDETERMINED));

  if (fieldDesc.isArray)
    {
      assert(mFieldType != T_TEXT);

      MARK_ARRAY(mFieldType);
    }
}


FieldOperand::FieldOperand(const FieldOperand& source)
  : mTableRef(source.mTableRef),
    mField(source.mField),
    mFieldType(source.mFieldType)
{
  if (mTableRef)
    mTableRef->IncrementRefCount();
}


FieldOperand::~FieldOperand()
{
  if (mTableRef)
    mTableRef->DecrementRefCount();
}


const FieldOperand&
FieldOperand::operator= (const FieldOperand& source)
{
  if (this != &source)
    {
      if ((mFieldType != T_UNDETERMINED)
          && (mFieldType != source.mFieldType))
        {
          throw InterException(_EXTRA(InterException::FIELD_TYPE_MISMATCH));
        }

      if (mTableRef != nullptr)
        mTableRef->DecrementRefCount();

      mField     = source.mField;
      mTableRef  = source.mTableRef;
      mFieldType = source.mFieldType;

      if (mTableRef)
        mTableRef->IncrementRefCount();
    }
  return *this;
}


bool
FieldOperand::IsNull() const
{
  return mTableRef == nullptr;
}


uint_t
FieldOperand::GetType()
{
  uint_t type = mFieldType;

  MARK_FIELD(type);

  return type;
}


FIELD_INDEX
FieldOperand::GetField()
{
  return mField;
}


ITable&
FieldOperand::GetTable()
{
  return mTableRef->GetTable();
}


StackValue
FieldOperand::GetValueAt(const uint64_t index)
{

  if ((mFieldType == T_UNKNOWN) || (mTableRef == nullptr))
    throw InterException(_EXTRA(InterException::FIELD_TYPE_MISMATCH));

  if (IS_ARRAY(mFieldType))
    return StackValue(ArrayFieldElOperand(mTableRef, index, mField));

  switch(mFieldType)
  {
  case T_BOOL:
    return StackValue(BoolFieldElOperand(mTableRef, index, mField));

  case T_CHAR:
    return StackValue(CharFieldElOperand(mTableRef, index, mField));

  case T_DATE:
    return StackValue(DateFieldElOperand(mTableRef, index, mField));

  case T_DATETIME:
    return StackValue(DateTimeFieldElOperand(mTableRef, index, mField));

  case T_HIRESTIME:
    return StackValue(HiresTimeFieldElOperand(mTableRef, index, mField));

  case T_UINT8:
    return StackValue(UInt8FieldElOperand(mTableRef, index, mField));

  case T_UINT16:
    return StackValue(UInt16FieldElOperand(mTableRef, index, mField));

  case T_UINT32:
    return StackValue(UInt32FieldElOperand(mTableRef, index, mField));

  case T_UINT64:
    return StackValue(UInt64FieldElOperand(mTableRef, index, mField));

  case T_INT8:
    return StackValue(Int8FieldElOperand(mTableRef, index, mField));

  case T_INT16:
    return StackValue(Int16FieldElOperand(mTableRef, index, mField));

  case T_INT32:
    return StackValue(Int32FieldElOperand(mTableRef, index, mField));

  case T_INT64:
    return StackValue(Int64FieldElOperand(mTableRef, index, mField));

  case T_REAL:
    return StackValue(RealFieldElOperand(mTableRef, index, mField));

  case T_RICHREAL:
    return StackValue(RichRealFieldElOperand(mTableRef, index, mField));

  case T_TEXT:
    return StackValue(TextFieldElOperand(mTableRef, index, mField));
  }

  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


StackValue
FieldOperand::Duplicate() const
{
  return StackValue(*this);
}


bool
FieldOperand::StartIterate(const bool reverse, StackValue& outStartItem)
{
  if (IsNull())
    return false;

  if (mTableRef->GetTable().AllocatedRows() <= 0)
    return false;

  outStartItem = GetValueAt(reverse
                             ? mTableRef->GetTable().AllocatedRows() - 1
                             : 0);
  return true;
}


bool
FieldOperand::PrepareToCopy(void *const )
{
  if (mTableRef)
    mTableRef->IncrementRefCount();

  return true;
}


FieldOperand
FieldOperand::GetFieldOp()
{
  return *this;
}


void
FieldOperand::CopyFieldOp(const FieldOperand& source)
{
  *this = source;
}


TableReference&
FieldOperand::GetTableReference()
{
  assert(mTableRef != nullptr);
  assert(IsNull() == false);

  return *mTableRef;
}


BaseFieldElOperand::~BaseFieldElOperand()
{
  mTableRef->DecrementRefCount();
}


bool
BaseFieldElOperand::Iterate(const bool reverse)
{
  ITable& table = mTableRef->GetTable();

  if (reverse)
    {
      if (mRow == 0)
        return false;

      _CC(ROW_INDEX&, mRow)--;
      return true;
    }

  if (mRow >= table.AllocatedRows() - 1)
    return false;

  _CC(ROW_INDEX&, mRow)++;
  return true;
}


uint64_t
BaseFieldElOperand::IteratorOffset()
{
  return mRow;
}


bool
BaseFieldElOperand::PrepareToCopy(void* const)
{
  mTableRef->IncrementRefCount();
  return true;
}


bool
BoolFieldElOperand::IsNull() const
{
  DBool currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
BoolFieldElOperand::GetValue(DBool& outValue) const
{
  Get(outValue);
}

void
BoolFieldElOperand::GetValue(DText& outValue) const
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
BoolFieldElOperand::SetValue(const DBool& value)
{
  Set(value);
}


void
BoolFieldElOperand::SelfAnd(const DBool& value)
{
  DBool currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
BoolFieldElOperand::SelfXor(const DBool& value)
{
  DBool currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
BoolFieldElOperand::SelfOr(const DBool& value)
{
  DBool currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
BoolFieldElOperand::GetType()
{
  return T_BOOL;
}


StackValue
BoolFieldElOperand::Duplicate() const
{
  DBool value;
  Get(value);

  return StackValue(BoolOperand(value));
}



bool
CharFieldElOperand::IsNull() const
{
  DChar currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
CharFieldElOperand::GetValue(DChar& outValue) const
{
  Get(outValue);
}


void
CharFieldElOperand::GetValue(DText& outValue) const
{
  DChar ch;
  Get(ch);

  outValue = DText();
  outValue.Append(ch);
}


void
CharFieldElOperand::SetValue(const DChar& value)
{
  Set(value);
}


uint_t
CharFieldElOperand::GetType()
{
  return T_CHAR;
}


StackValue
CharFieldElOperand::Duplicate() const
{
  DChar ch;
  Get(ch);

  return StackValue(CharOperand(ch));
}



bool
DateFieldElOperand::IsNull() const
{
  DDate currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
DateFieldElOperand::GetValue(DDate& outValue) const
{
  Get(outValue);
}


void
DateFieldElOperand::GetValue(DDateTime& outValue) const
{
  DDate currValue;
  Get(currValue);

  outValue = currValue;
}


void
DateFieldElOperand::GetValue(DHiresTime& outValue) const
{
  DDate currValue;
  Get(currValue);

  outValue = currValue;
}


void
DateFieldElOperand::GetValue(DText& outValue) const
{
  DDate   currValue;
  uint8_t text[32];
  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);

  outValue = DText(_RC(char*, text));
}


void
DateFieldElOperand::SetValue(const DHiresTime& value)
{
  DDate v;

  _CC(bool&,    v.mIsNull) = value.mIsNull;
  _CC(int16_t&, v.mYear)   = value.mYear;
  _CC(uint8_t&, v.mMonth)  = value.mMonth;
  _CC(uint8_t&, v.mDay)    = value.mDay;

  Set(v);
}


uint_t
DateFieldElOperand::GetType()
{
  return T_DATE;
}


StackValue
DateFieldElOperand::Duplicate() const
{
  DDate value;

  Get(value);

  return StackValue(DateOperand(value));
}



bool
DateTimeFieldElOperand::IsNull() const
{
  DDateTime currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
DateTimeFieldElOperand::GetValue(DDate& outValue) const
{
  DDateTime currValue;
  Get(currValue);

  _CC(bool&  ,   outValue.mIsNull) = currValue.mIsNull;
  _CC(int16_t&,  outValue.mYear)   = currValue.mYear;
  _CC(uint8_t&,  outValue.mMonth)  = currValue.mMonth;
  _CC(uint8_t&,  outValue.mDay)    = currValue.mDay;
}


void
DateTimeFieldElOperand::GetValue(DDateTime& outValue) const
{
  Get(outValue);
}


void
DateTimeFieldElOperand::GetValue(DHiresTime& outValue) const
{
  DDateTime currValue;
  Get(currValue);

  outValue = currValue;
}


void
DateTimeFieldElOperand::GetValue(DText& outValue) const
{
  DDateTime currValue;
  uint8_t   text[32];

  Get(currValue);
  Utf8Translator::Write(text, sizeof text, currValue);

  outValue = DText(_RC(char*, text));
}


void
DateTimeFieldElOperand::SetValue(const DHiresTime& value)
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
DateTimeFieldElOperand::GetType()
{
  return T_DATETIME;
}


StackValue
DateTimeFieldElOperand::Duplicate() const
{
  DDateTime value;

  Get(value);

  return StackValue(DateTimeOperand(value));
}



bool
HiresTimeFieldElOperand::IsNull() const
{
  DHiresTime currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
HiresTimeFieldElOperand::GetValue(DDate& outValue) const
{
  DHiresTime currValue;
  Get(currValue);

  _CC(bool&,    outValue.mIsNull) = currValue.mIsNull;
  _CC(int16_t&, outValue.mYear)   = currValue.mYear;
  _CC(uint8_t&, outValue.mMonth)  = currValue.mMonth;
  _CC(uint8_t&, outValue.mDay)    = currValue.mDay;
}


void
HiresTimeFieldElOperand::GetValue(DDateTime& outValue) const
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
HiresTimeFieldElOperand::GetValue(DHiresTime& outValue) const
{
  Get(outValue);
}


void
HiresTimeFieldElOperand::GetValue(DText& outValue) const
{
  DHiresTime currValue;
  uint8_t    text[32];

  Get(currValue);
  Utf8Translator::Write(text, sizeof text, currValue);

  outValue = DText(_RC(char*, text));
}


void
HiresTimeFieldElOperand::SetValue(const DHiresTime& value)
{
  Set(value);
}


uint_t
HiresTimeFieldElOperand::GetType()
{
  return T_HIRESTIME;
}


StackValue
HiresTimeFieldElOperand::Duplicate() const
{
  DHiresTime value;
  Get(value);

  return StackValue(HiresTimeOperand(value));
}



bool
UInt8FieldElOperand::IsNull() const
{
  DUInt8 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
UInt8FieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DReal& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DUInt8& outValue) const
{
  Get(outValue);
}


void
UInt8FieldElOperand::GetValue(DUInt16& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DUInt32& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DUInt64& outValue) const
{
  DUInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt8FieldElOperand::GetValue(DText& outValue) const
{
  DUInt8  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt8FieldElOperand::SetValue(const DUInt64& value)
{
  DUInt8  v;

  number_convert(value, v);
  Set(v);
}

void
UInt8FieldElOperand::SetValue(const DInt64& value)
{
  DUInt8  v;

  number_convert(value, v);
  Set(v);
}


void
UInt8FieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt8FieldElOperand::SelfSub(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
UInt8FieldElOperand::SelfMul(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt8FieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt8FieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt8FieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt8FieldElOperand::SelfMod(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt8FieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt8FieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt8FieldElOperand::SelfXor(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt8FieldElOperand::SelfOr(const DInt64& value)
{
  DUInt8 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt8FieldElOperand::GetType()
{
  return T_UINT8;
}


StackValue
UInt8FieldElOperand::Duplicate() const
{
  DUInt8 value;
  Get(value);

  return StackValue(UInt8Operand(value));
}



bool
UInt16FieldElOperand::IsNull() const
{
  DUInt16 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
UInt16FieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DReal& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DUInt8& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DUInt16& outValue) const
{
  Get(outValue);
}


void
UInt16FieldElOperand::GetValue(DUInt32& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DUInt64& outValue) const
{
  DUInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt16FieldElOperand::GetValue(DText& outValue) const
{
  DUInt16  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt16FieldElOperand::SetValue(const DUInt64& value)
{
  DUInt16  v;

  number_convert(value, v);
  Set(v);
}

void
UInt16FieldElOperand::SetValue(const DInt64& value)
{
  DUInt16  v;

  number_convert(value, v);
  Set(v);
}


void
UInt16FieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt16FieldElOperand::SelfSub(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
UInt16FieldElOperand::SelfMul(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
UInt16FieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
UInt16FieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt16FieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt16FieldElOperand::SelfMod(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt16FieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt16FieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt16FieldElOperand::SelfXor(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt16FieldElOperand::SelfOr(const DInt64& value)
{
  DUInt16 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt16FieldElOperand::GetType()
{
  return T_UINT16;
}


StackValue
UInt16FieldElOperand::Duplicate() const
{
  DUInt16 value;
  Get(value);

  return StackValue(UInt16Operand(value));
}



bool
UInt32FieldElOperand::IsNull() const
{
  DUInt32 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
UInt32FieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DReal& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DUInt8& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DUInt16& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DUInt32& outValue) const
{
  Get(outValue);
}


void
UInt32FieldElOperand::GetValue(DUInt64& outValue) const
{
  DUInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt32FieldElOperand::GetValue(DText& outValue) const
{
  DUInt32  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt32FieldElOperand::SetValue(const DUInt64& value)
{
  DUInt32  v;

  number_convert(value, v);
  Set(v);
}

void
UInt32FieldElOperand::SetValue(const DInt64& value)
{
  DUInt32  v;

  number_convert(value, v);
  Set(v);
}


void
UInt32FieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt32FieldElOperand::SelfSub(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
UInt32FieldElOperand::SelfMul(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt32FieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt32FieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
UInt32FieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt32FieldElOperand::SelfMod(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt32FieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt32FieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt32FieldElOperand::SelfXor(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt32FieldElOperand::SelfOr(const DInt64& value)
{
  DUInt32 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt32FieldElOperand::GetType()
{
  return T_UINT32;
}


StackValue
UInt32FieldElOperand::Duplicate() const
{
  DUInt32 value;
  Get(value);

  return StackValue(UInt32Operand(value));
}



bool
UInt64FieldElOperand::IsNull() const
{
  DUInt64 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
UInt64FieldElOperand::GetValue(DInt8& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64FieldElOperand::GetValue(DInt16& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64FieldElOperand::GetValue(DInt32& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64FieldElOperand::GetValue(DInt64& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}

void
UInt64FieldElOperand::GetValue(DRichReal& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64FieldElOperand::GetValue(DReal& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64FieldElOperand::GetValue(DUInt8& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64FieldElOperand::GetValue(DUInt16& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64FieldElOperand::GetValue(DUInt32& outValue) const
{
  DUInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
UInt64FieldElOperand::GetValue(DUInt64& outValue) const
{
  Get(outValue);
}


void
UInt64FieldElOperand::GetValue(DText& outValue) const
{
  DUInt64  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
UInt64FieldElOperand::SetValue(const DUInt64& value)
{
  Set(value);
}

void
UInt64FieldElOperand::SetValue(const DInt64& value)
{
  DUInt64  v;

  number_convert(value, v);
  Set(v);
}


void
UInt64FieldElOperand::SelfAdd(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
UInt64FieldElOperand::SelfSub(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}

void
UInt64FieldElOperand::SelfMul(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt64FieldElOperand::SelfMul(const DUInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
UInt64FieldElOperand::SelfDiv(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt64FieldElOperand::SelfDiv(const DUInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
UInt64FieldElOperand::SelfMod(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
UInt64FieldElOperand::SelfMod(const DUInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
UInt64FieldElOperand::SelfAnd(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
UInt64FieldElOperand::SelfXor(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
UInt64FieldElOperand::SelfOr(const DInt64& value)
{
  DUInt64 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
UInt64FieldElOperand::GetType()
{
  return T_UINT64;
}


StackValue
UInt64FieldElOperand::Duplicate() const
{
  DUInt64 value;
  Get(value);

  return StackValue(UInt64Operand(value));
}



bool
Int8FieldElOperand::IsNull() const
{
  DInt8 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
Int8FieldElOperand::GetValue(DInt8& outValue) const
{
  Get(outValue);
}


void
Int8FieldElOperand::GetValue(DInt16& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DInt32& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DInt64& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DReal& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt8 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int8FieldElOperand::GetValue(DText& outValue) const
{
  DInt8   currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int8FieldElOperand::SetValue(const DUInt64& value)
{
  DInt8  v;

  number_convert(value, v);
  Set(v);
}

void
Int8FieldElOperand::SetValue(const DInt64& value)
{
  DInt8  v;

  number_convert(value, v);
  Set(v);
}


void
Int8FieldElOperand::SelfAdd(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfSub(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfMul(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfMul(const DUInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfDiv(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int8FieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfMod(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfMod(const DUInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfAnd(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfXor(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int8FieldElOperand::SelfOr(const DInt64& value)
{
  DInt8 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int8FieldElOperand::GetType()
{
  return T_INT8;
}


StackValue
Int8FieldElOperand::Duplicate() const
{
  DInt8 value;
  Get(value);

  return StackValue(Int8Operand(value));
}



bool
Int16FieldElOperand::IsNull() const
{
  DInt16 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
Int16FieldElOperand::GetValue(DInt8& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DInt16& outValue) const
{
  Get(outValue);
}


void
Int16FieldElOperand::GetValue(DInt32& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DInt64& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DReal& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt16 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int16FieldElOperand::GetValue(DText& outValue) const
{
  DInt16  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int16FieldElOperand::SetValue(const DUInt64& value)
{
  DInt16  v;

  number_convert(value, v);
  Set(v);
}


void
Int16FieldElOperand::SetValue(const DInt64& value)
{
  DInt16  v;

  number_convert(value, v);
  Set(v);
}


void
Int16FieldElOperand::SelfAdd(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int16FieldElOperand::SelfSub(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int16FieldElOperand::SelfMul(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int16FieldElOperand::SelfMul(const DUInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int16FieldElOperand::SelfDiv(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int16FieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}

void
Int16FieldElOperand::SelfMod(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int16FieldElOperand::SelfMod(const DUInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
Int16FieldElOperand::SelfAnd(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int16FieldElOperand::SelfXor(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int16FieldElOperand::SelfOr(const DInt64& value)
{
  DInt16 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int16FieldElOperand::GetType()
{
  return T_INT16;
}


StackValue
Int16FieldElOperand::Duplicate() const
{
  DInt16 value;

  Get(value);

  return StackValue(Int16Operand(value));
}



bool
Int32FieldElOperand::IsNull() const
{
  DInt32 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
Int32FieldElOperand::GetValue(DInt8& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DInt16& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DInt32& outValue) const
{
  Get(outValue);
}


void
Int32FieldElOperand::GetValue(DInt64& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DReal& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt32 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int32FieldElOperand::GetValue(DText& outValue) const
{
  DInt32  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int32FieldElOperand::SetValue(const DUInt64& value)
{
  DInt32  v;

  number_convert(value, v);
  Set(v);
}


void
Int32FieldElOperand::SetValue(const DInt64& value)
{
  DInt32  v;

  number_convert(value, v);
  Set(v);
}


void
Int32FieldElOperand::SelfAdd(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int32FieldElOperand::SelfSub(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int32FieldElOperand::SelfMul(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}

void
Int32FieldElOperand::SelfMul(const DUInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int32FieldElOperand::SelfDiv(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
Int32FieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
Int32FieldElOperand::SelfMod(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int32FieldElOperand::SelfMod(const DUInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}

void
Int32FieldElOperand::SelfAnd(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int32FieldElOperand::SelfXor(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int32FieldElOperand::SelfOr(const DInt64& value)
{
  DInt32 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int32FieldElOperand::GetType()
{
  return T_INT32;
}


StackValue
Int32FieldElOperand::Duplicate() const
{
  DInt32 value;
  Get(value);

  return StackValue(Int32Operand(value));
}



bool
Int64FieldElOperand::IsNull() const
{
  DInt64 currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
Int64FieldElOperand::GetValue(DInt8& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DInt16& outValue) const
{
  DInt64 currValue;

  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DInt32& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DInt64& outValue) const
{
  Get(outValue);
}


void
Int64FieldElOperand::GetValue(DRichReal& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DReal& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DUInt8& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DUInt16& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DUInt32& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DUInt64& outValue) const
{
  DInt64 currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
Int64FieldElOperand::GetValue(DText& outValue) const
{
  DInt64  currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
Int64FieldElOperand::SetValue(const DUInt64& value)
{
  DInt32  v;

  number_convert(value, v);
  Set(v);
}


void
Int64FieldElOperand::SetValue(const DInt64& value)
{
  Set(value);
}


void
Int64FieldElOperand::SelfAdd(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfSub(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfMul(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfMul(const DUInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfDiv(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfDiv(const DUInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfMod(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfMod(const DUInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_mod(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfAnd(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_and(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfXor(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_xor(currValue, value);

  Set(currValue);
}


void
Int64FieldElOperand::SelfOr(const DInt64& value)
{
  DInt64 currValue;
  Get(currValue);

  currValue = internal_or(currValue, value);

  Set(currValue);
}


uint_t
Int64FieldElOperand::GetType()
{
  return T_INT64;
}


StackValue
Int64FieldElOperand::Duplicate() const
{
  DInt64 value;
  Get(value);

  return StackValue(Int64Operand(value));
}



bool
RealFieldElOperand::IsNull() const
{
  DReal currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
RealFieldElOperand::GetValue(DReal& outValue) const
{
  Get(outValue);
}


void
RealFieldElOperand::GetValue(DRichReal& outValue) const
{
  DReal currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
RealFieldElOperand::GetValue(DText& outValue) const
{
  DReal   currValue;
  uint8_t text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
RealFieldElOperand::SetValue(const DRichReal& value)
{
  Set(value.IsNull() ? DReal() : DReal(value.mValue));
}


void
RealFieldElOperand::SelfAdd(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}



void
RealFieldElOperand::SelfAdd(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
RealFieldElOperand::SelfSub(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RealFieldElOperand::SelfSub(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RealFieldElOperand::SelfMul(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RealFieldElOperand::SelfMul(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RealFieldElOperand::SelfDiv(const DInt64& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
RealFieldElOperand::SelfDiv(const DRichReal& value)
{
  DReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


uint_t
RealFieldElOperand::GetType()
{
  return T_REAL;
}


StackValue
RealFieldElOperand::Duplicate() const
{
  DReal value;

  Get(value);

  return StackValue(RealOperand(value));
}


bool
RichRealFieldElOperand::IsNull() const
{
  DRichReal currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
RichRealFieldElOperand::GetValue(DReal& outValue) const
{
  DRichReal currValue;
  Get(currValue);

  number_convert(currValue, outValue);
}


void
RichRealFieldElOperand::GetValue(DRichReal& outValue) const
{
  Get(outValue);
}


void
RichRealFieldElOperand::GetValue(DText& outValue) const
{
  DRichReal currValue;
  uint8_t   text[64];

  Get(currValue);

  Utf8Translator::Write(text, sizeof text, currValue);
  outValue = DText(_RC(char*, text));
}


void
RichRealFieldElOperand::SetValue(const DRichReal& value)
{
  Set(value);
}


void
RichRealFieldElOperand::SelfAdd(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
RichRealFieldElOperand::SelfAdd(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_add(currValue, value);

  Set(currValue);
}


void
RichRealFieldElOperand::SelfSub(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RichRealFieldElOperand::SelfSub(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_sub(currValue, value);

  Set(currValue);
}


void
RichRealFieldElOperand::SelfMul(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RichRealFieldElOperand::SelfMul(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_mul(currValue, value);

  Set(currValue);
}


void
RichRealFieldElOperand::SelfDiv(const DInt64& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


void
RichRealFieldElOperand::SelfDiv(const DRichReal& value)
{
  DRichReal currValue;
  Get(currValue);

  currValue = internal_div(currValue, value);

  Set(currValue);
}


uint_t
RichRealFieldElOperand::GetType()
{
  return T_RICHREAL;
}


StackValue
RichRealFieldElOperand::Duplicate() const
{
  DRichReal value;
  Get(value);

  return StackValue(RichRealOperand(value));
}


bool
TextFieldElOperand::IsNull() const
{
  DText currValue;
  Get(currValue);

  return currValue.IsNull();
}


void
TextFieldElOperand::GetValue(DText& outValue) const
{
  Get(outValue);
}


void
TextFieldElOperand::SetValue(const DText& value)
{
  Set(value);
}


void
TextFieldElOperand::SelfAdd(const DChar& value)
{
  DText currValue;
  Get(currValue);

  currValue.Append(value);

  Set(currValue);
}


void
TextFieldElOperand::SelfAdd(const DText& value)
{
  DText currValue;
  Get(currValue);

  currValue.Append(value);

  Set(currValue);
}


uint_t
TextFieldElOperand::GetType()
{
  return T_TEXT;
}


StackValue
TextFieldElOperand::GetValueAt(const uint64_t index)
{
  return StackValue(CharTextFieldElOperand(mTableRef,
                                             mRow,
                                             mField,
                                             index));
}


StackValue
TextFieldElOperand::Duplicate() const
{
  DText value;
  GetValue(value);

  return StackValue(TextOperand(value));
}


bool
TextFieldElOperand::StartIterate(const bool reverse, StackValue& outStartItem)
{
  if (IsNull())
    return false;

  DText temp;
  GetValue(temp);
  if (temp.IsNull())
    return false;

  outStartItem = GetValueAt(reverse ? temp.Count() - 1 : 0);
  return true;
}



bool
ArrayFieldElOperand::IsNull() const
{
  DArray currValue;

  Get(currValue);

  return currValue.IsNull();
}


void
ArrayFieldElOperand::GetValue(DArray& outValue) const
{
  Get(outValue);
}


void
ArrayFieldElOperand::SetValue(const DArray& value)
{
  Set(value);
}


uint_t
ArrayFieldElOperand::GetType()
{
  uint_t type = 0;

  MARK_ARRAY(type);

  return type;
}


StackValue
ArrayFieldElOperand::GetValueAt(const uint64_t index)
{
  ITable& table = mTableRef->GetTable();

  DBSFieldDescriptor fd = table.DescribeField(mField);

  assert(fd.isArray);

  switch(fd.type)
  {
  case T_BOOL:
    return StackValue(BoolArrayFieldElOperand(mTableRef,
                                                mRow,
                                                mField,
                                                index));

  case T_CHAR:
    return StackValue(CharArrayFieldElOperand(mTableRef,
                                                mRow,
                                                mField,
                                                index));

  case T_DATE:
    return StackValue(DateArrayFieldElOperand(mTableRef,
                                                mRow,
                                                mField,
                                                index));
  case T_DATETIME:
    return StackValue(DateTimeArrayFieldElOperand(mTableRef,
                                                    mRow,
                                                    mField,
                                                    index));

  case T_HIRESTIME:
    return StackValue(HiresTimeArrayFieldElOperand(mTableRef,
                                                     mRow,
                                                     mField,
                                                     index));

  case T_UINT8:
    return StackValue(UInt8ArrayFieldElOperand(mTableRef,
                                                 mRow,
                                                 mField,
                                                 index));

  case T_UINT16:
    return StackValue(UInt16ArrayFieldElOperand(mTableRef,
                                                  mRow,
                                                  mField,
                                                  index));

  case T_UINT32:
    return StackValue(UInt32ArrayFieldElOperand(mTableRef,
                                                  mRow,
                                                  mField,
                                                  index));

  case T_UINT64:
    return StackValue(UInt64ArrayFieldElOperand(mTableRef,
                                                  mRow,
                                                  mField,
                                                  index));

  case T_INT8:
    return StackValue(Int8ArrayFieldElOperand(mTableRef,
                                                mRow,
                                                mField,
                                                index));

  case T_INT16:
    return StackValue(Int16ArrayFieldElOperand(mTableRef,
                                                 mRow,
                                                 mField,
                                                 index));

  case T_INT32:
    return StackValue(Int32ArrayFieldElOperand(mTableRef,
                                                 mRow,
                                                 mField,
                                                 index));

  case T_INT64:
    return StackValue(Int64ArrayFieldElOperand(mTableRef,
                                                 mRow,
                                                 mField,
                                                 index));

  case T_REAL:
    return StackValue(RealArrayFieldElOperand(mTableRef,
                                                mRow,
                                                mField,
                                                index));

  case T_RICHREAL:
    return StackValue(RichRealArrayFieldElOperand(mTableRef,
                                                    mRow,
                                                    mField,
                                                    index));

  default:
    assert(false);
  }

  throw InterException(_EXTRA(InterException::INTERNAL_ERROR));
}


StackValue
ArrayFieldElOperand::Duplicate() const
{
  DArray value;

  GetValue(value);

  return StackValue(ArrayOperand(value));
}


bool
ArrayFieldElOperand::StartIterate(const bool reverse, StackValue& outStartItem)
{
  if (IsNull())
    return false;

  DArray temp;
  GetValue(temp);
  if (temp.IsNull())
    return false;

  assert(temp.Count() > 0);

  outStartItem = GetValueAt(reverse ? temp.Count() - 1 : 0);
  return true;
}

} //namespace prima
} //namespace whais

