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

#ifndef PM_OPERAND_H_
#define PM_OPERAND_H_


#include "utils/wthread.h"
#include "interpreter.h"
#include "operands.h"
#include "pm_table.h"
#include "pm_store.h"


namespace whais {
namespace prima {


template <typename T_DEST, typename T_SRC> T_DEST
internal_add(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return _SC(T_DEST, firstOp.mValue + secondOp.mValue);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_sub(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return T_DEST(firstOp.mValue - secondOp.mValue);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_mul(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return T_DEST(firstOp.mValue * secondOp.mValue);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_div(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return T_DEST(firstOp.mValue / secondOp.mValue);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_mod(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return T_DEST(firstOp.mValue % secondOp.mValue);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_and(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return T_DEST(firstOp.mValue & secondOp.mValue);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_xor(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return T_DEST(firstOp.mValue ^ secondOp.mValue);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_or(const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull() || secondOp.IsNull())
    return firstOp;

  return T_DEST(firstOp.mValue ^ secondOp.mValue);
}

template <typename T_SRC, typename T_DEST> void
number_convert(const T_SRC& from, T_DEST& to)
{
  to = from.IsNull() ? T_DEST() : _SC(T_DEST, from.mValue);
}


class TableOperand;
class FieldOperand;
class UndefinedOperand;
class LocalOperand;
class GlobalOperand;


class BaseOperand : public IOperand
{
public:
  virtual bool IsNull() const override;

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;
  virtual void GetValue(DArray& outValue) const override;

  virtual void SetValue(const DBool& value) override;
  virtual void SetValue(const DChar& value) override;
  virtual void SetValue(const DHiresTime& value) override;
  virtual void SetValue(const DInt64& value) override;
  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DRichReal& value) override;
  virtual void SetValue(const DText& value) override;
  virtual void SetValue(const DArray& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;
  virtual void SelfAdd(const DChar& value) override;
  virtual void SelfAdd(const DText& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;
  virtual void SelfAnd(const DBool& value) override;

  virtual void SelfXor(const DInt64& value) override;
  virtual void SelfXor(const DBool& value) override;

  virtual void SelfOr(const DInt64& value) override;
  virtual void SelfOr(const DBool& value) override;

  virtual FIELD_INDEX GetField() override;

  virtual ITable& GetTable() override;

  virtual StackValue GetTableValue() override;
  virtual StackValue GetFieldAt(const FIELD_INDEX field) override;
  virtual StackValue GetValueAt(const uint64_t index) override;

  virtual bool StartIterate(const bool  reverse, StackValue& outStartItem) override;
  virtual bool Iterate(const bool reverse) override;
  virtual uint64_t IteratorOffset() override;
  virtual bool CustomCopyIncomplete(void* const dest) override { return true; }
  virtual void NativeObject(INativeObject* const value) override;
  virtual INativeObject& NativeObject() override;

  virtual TableOperand GetTableOp();
  virtual void CopyTableOp(const TableOperand& source);
  virtual FieldOperand GetFieldOp();
  virtual void CopyFieldOp(const FieldOperand& source);
  virtual TableReference& GetTableReference();
  virtual void RedifineValue(StackValue& source);
};


class NullOperand : public BaseOperand
{
public:
  NullOperand() = default;
  virtual ~NullOperand() = default;

  virtual bool IsNull() const override;

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;
  virtual void GetValue(DArray& outValue) const override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class BoolOperand : public BaseOperand
{
public:
  explicit BoolOperand(const DBool& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DBool& value) override;

  virtual void SelfAnd(const DBool& value) override;

  virtual void SelfXor(const DBool& value) override;

  virtual void SelfOr(const DBool& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DBool mValue;
};


class CharOperand : public BaseOperand
{
public:
  explicit CharOperand(const DChar& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DChar& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DChar mValue;
};


class DateOperand : public BaseOperand
{
public:
  explicit DateOperand(const DDate& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DDate mValue;
};


class DateTimeOperand : public BaseOperand
{
public:
  explicit DateTimeOperand(const DDateTime& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DDateTime mValue;
};


class HiresTimeOperand : public BaseOperand
{
public:
  explicit HiresTimeOperand(const DHiresTime& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DHiresTime mValue;
};


class UInt8Operand : public BaseOperand
{
public:
  explicit UInt8Operand(const DUInt8& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DUInt8 mValue;
};


class UInt16Operand : public BaseOperand
{
public:
  explicit UInt16Operand(const DUInt16& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;
  virtual void SelfXor(const DInt64& value) override;
  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DUInt16 mValue;
};


class UInt32Operand : public BaseOperand
{
public:
  explicit UInt32Operand(const DUInt32& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DUInt32 mValue;
};


class UInt64Operand : public BaseOperand
{
public:
  explicit UInt64Operand(const DUInt64& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DUInt64 mValue;
};


class Int8Operand : public BaseOperand
{
public:
  explicit Int8Operand(const DInt8& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DInt8 mValue;
};


class Int16Operand : public BaseOperand
{
public:
  explicit Int16Operand(const DInt16& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DInt16 mValue;
};


class Int32Operand : public BaseOperand
{
public:
  explicit Int32Operand(const DInt32& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DInt32 mValue;
};


class Int64Operand : public BaseOperand
{
public:
  explicit Int64Operand(const DInt64& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DInt64 mValue;
};


class RealOperand : public BaseOperand
{
public:
  explicit RealOperand(const DReal& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DReal mValue;
};


class RichRealOperand : public BaseOperand
{
public:
  explicit RichRealOperand(const DRichReal& value)
    : mValue(value)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

private:
  DRichReal mValue;
};


class TextOperand : public BaseOperand
{
public:
  explicit TextOperand(const DText& value)
    : mValue(SharedTextStore::Instance().Alloc())
  {
    *mValue = shared_make(DText, value);
  }

  TextOperand(const TextOperand& source)
    : TextOperand(**source.mValue)
  {
  }

  TextOperand(TextOperand&& source)
    : mValue(source.mValue)
  {
    source.mValue = nullptr;
  }

  ~TextOperand()
  {
    if (mValue)
      SharedTextStore::Instance().Free(mValue);
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DText& outValue) const override;
  virtual void SetValue(const DText& value) override;

  virtual void SelfAdd(const DChar& value) override;
  virtual void SelfAdd(const DText& value) override;

  virtual uint_t GetType() override;

  virtual StackValue GetValueAt(const uint64_t index) override;
  virtual StackValue Clone() const override;

  virtual bool StartIterate(const bool reverse, StackValue& outStartItem) override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

private:
  std::shared_ptr<DText>* mValue;
};


class CharTextElOperand : public BaseOperand
{
public:
  CharTextElOperand(std::shared_ptr<DText> text, const uint64_t index)
    : mIndex(index),
      mText(SharedTextStore::Instance().Alloc())
  {
    *mText = text;
  }

  CharTextElOperand(const CharTextElOperand& source)
    : mIndex(source.mIndex),
      mText(SharedTextStore::Instance().Alloc())
  {
    *mText = *source.mText;
  }

  CharTextElOperand(CharTextElOperand&& source)
    : mIndex(source.mIndex),
      mText(source.mText)
  {
    source.mText = nullptr;
  }

  ~CharTextElOperand()
  {
    if (mText)
      SharedTextStore::Instance().Free(mText);
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DChar& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool Iterate(const bool reverse) override;
  virtual uint64_t IteratorOffset() override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

private:
  const uint64_t mIndex;
  std::shared_ptr<DText>* mText;
};


class ArrayOperand : public BaseOperand
{
public:
  explicit ArrayOperand(const DArray& array)
    : mValue(SharedArrayStore::Instance().Alloc()),
      mFirstArrayType(array.Type())
  {
    *mValue = shared_make(DArray, array);
  }

  ArrayOperand(const ArrayOperand& src)
    : ArrayOperand(**src.mValue)
  {
  }

  ArrayOperand(ArrayOperand&& src)
    : mValue(src.mValue),
      mFirstArrayType(src.mFirstArrayType)
  {
    src.mValue = nullptr;
  }

  ~ArrayOperand()
  {
    if (mValue)
      SharedArrayStore::Instance().Free(mValue);
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DArray& outValue) const override;
  virtual void SetValue(const DArray& value) override;

  virtual uint_t GetType() override;

  virtual StackValue GetValueAt(const uint64_t index) override;
  virtual StackValue Clone() const override;

  virtual bool StartIterate(const bool reverse, StackValue& outStartItem) override;
  virtual bool CustomCopyIncomplete(void* const) override;

private:
  std::shared_ptr<DArray>* mValue;
  uint16_t mFirstArrayType;
};


class BaseArrayElOperand : public BaseOperand
{
protected:
  BaseArrayElOperand(std::shared_ptr<DArray>* array, const uint64_t index)
    : mIndex(index),
      mArray(SharedArrayStore::Instance().Alloc())
  {
    *mArray = *array;
  }

  BaseArrayElOperand(const BaseArrayElOperand& source)
    : mIndex(source.mIndex),
      mArray(SharedArrayStore::Instance().Alloc())
  {
    *mArray = *source.mArray;
  }

  BaseArrayElOperand(BaseArrayElOperand&& source)
    : mIndex(source.mIndex),
      mArray(source.mArray)
  {
    source.mArray = nullptr;
  }

  ~BaseArrayElOperand()
  {
    if (mArray)
      SharedArrayStore::Instance().Free(mArray);
  }

  BaseArrayElOperand& operator= (const BaseArrayElOperand& source) = delete;

  template <typename DBS_T> void Get(DBS_T& outValue) const
  {
    if (mIndex < (*mArray)->Count())
    {
      (*mArray)->Get(mIndex, outValue);
      assert ( ! outValue.IsNull ());
    }
    else
      outValue = DBS_T();
  }

  template <typename DBS_T> void Set(const DBS_T& value)
  {
    (*mArray)->Set(mIndex, value);
  }

  virtual bool IsNull() const override;
  virtual bool Iterate(const bool reverse) override;
  virtual uint64_t IteratorOffset() override;

private:
  const uint64_t          mIndex;
  std::shared_ptr<DArray>* mArray;
};


class BoolArrayElOperand : public BaseArrayElOperand
{
public:
  BoolArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DBool& value) override;

  virtual void SelfAnd(const DBool& value) override;

  virtual void SelfXor(const DBool& value) override;

  virtual void SelfOr(const DBool& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class CharArrayElOperand : public BaseArrayElOperand
{
public:
  CharArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DChar& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class DateArrayElOperand : public BaseArrayElOperand
{
public:
  DateArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class DateTimeArrayElOperand : public BaseArrayElOperand
{
public:
  DateTimeArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class HiresTimeArrayElOperand : public BaseArrayElOperand
{
public:
  HiresTimeArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class UInt8ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt8ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;


  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class UInt16ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt16ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class UInt32ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt32ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class UInt64ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt64ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class Int8ArrayElOperand : public BaseArrayElOperand
{
public:
  Int8ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};

class Int16ArrayElOperand : public BaseArrayElOperand
{
public:
  Int16ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class Int32ArrayElOperand : public BaseArrayElOperand
{
public:
  Int32ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class Int64ArrayElOperand : public BaseArrayElOperand
{
public:
  Int64ArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class RealArrayElOperand : public BaseArrayElOperand
{
public:
  RealArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class RichRealArrayElOperand : public BaseArrayElOperand
{
public:
  RichRealArrayElOperand(std::shared_ptr<DArray>* const array, const uint64_t index)
    : BaseArrayElOperand(array, index)
  {
  }

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;
};


class TableOperand : public BaseOperand
{
public:
  TableOperand(ITable& table, const bool changeable)
    : mTableRef(new TableReference(table)),
      mChangeable(changeable)
  {
    mTableRef->IncrementRefCount();
  }

  TableOperand(const TableOperand& source)
    : mTableRef(source.mTableRef),
      mChangeable(source.mChangeable)
  {
    mTableRef->IncrementRefCount();
  }

  TableOperand(TableReference& tableRef)
    : mTableRef(&tableRef),
      mChangeable(true)
  {
    mTableRef->IncrementRefCount();
  }

  virtual ~TableOperand() override;

  const TableOperand& operator= (const TableOperand& source)
  {
    if (this != &source)
    {
      mTableRef->DecrementRefCount();
      mTableRef = source.mTableRef;
      mTableRef->IncrementRefCount();

      mChangeable = source.mChangeable;
    }
    return *this;
  }

  virtual bool IsNull() const override;

  virtual uint_t GetType() override;

  virtual StackValue GetFieldAt(const FIELD_INDEX field) override;

  virtual ITable& GetTable() override;

  virtual StackValue Clone() const override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

  virtual TableOperand GetTableOp() override;

  virtual void CopyTableOp(const TableOperand& source) override;

  virtual TableReference& GetTableReference() override;

private:
  TableReference* mTableRef;
  bool            mChangeable;
};


class FieldOperand : public BaseOperand
{
public:
  FieldOperand(const uint32_t fieldType = T_UNDETERMINED)
    : mTableRef(nullptr),
      mField(~0),
      mFieldType(fieldType)
  {
    assert(GET_BASIC_TYPE(fieldType) <= T_UNDETERMINED);
    assert(GET_BASIC_TYPE(fieldType) > T_UNKNOWN);
  }

  FieldOperand(TableOperand& tableOp, const FIELD_INDEX field);
  FieldOperand(TableReference& tableRef, const FIELD_INDEX field);
  FieldOperand(const FieldOperand& source);
  virtual ~FieldOperand() override;

  const FieldOperand& operator= (const FieldOperand& source);

  virtual bool IsNull() const override;

  virtual uint_t GetType() override;

  virtual FIELD_INDEX GetField() override;

  virtual ITable& GetTable() override;

  virtual StackValue GetValueAt(const uint64_t index) override;
  virtual StackValue Clone() const override;

  virtual bool StartIterate(const bool reverse, StackValue& outStartItem) override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

  virtual FieldOperand GetFieldOp() override;

  virtual void CopyFieldOp(const FieldOperand& source) override;

  virtual TableReference& GetTableReference() override;

private:
  TableReference*   mTableRef;
  FIELD_INDEX       mField;
  uint32_t          mFieldType;
};


class BaseFieldElOperand : public BaseOperand
{
  friend class CharTextFieldElOperand;
  friend class TextFieldElOperand;
  friend class ArrayFieldElOperand;

protected:
  BaseFieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : mTableRef(tableRef),
      mRow(row),
      mField(field)
  {
    mTableRef->IncrementRefCount();
  }

  BaseFieldElOperand(const BaseFieldElOperand& source)
    : BaseOperand(),
      mTableRef(source.mTableRef),
      mRow(source.mRow),
      mField(source.mField)
  {
    mTableRef->IncrementRefCount();
  }

  virtual ~BaseFieldElOperand() override;

  template<typename DBS_T> void Get(DBS_T& out) const
  {
    ITable& table = mTableRef->GetTable();

    if (table.AllocatedRows() <= mRow)
    {
      out = DBS_T();
      return;
    }

    table.Get(mRow, mField, out);
  }

  template <typename DBS_T> void Set(const DBS_T& value)
  {
    ITable& table = mTableRef->GetTable();

    table.Set(mRow, mField, value);
  }

  virtual bool Iterate(const bool reverse) override;
  virtual uint64_t IteratorOffset() override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

private:
  BaseFieldElOperand& operator= (const BaseFieldElOperand* source);

  TableReference*       mTableRef;
  const ROW_INDEX       mRow;
  const FIELD_INDEX     mField;
};


class BoolFieldElOperand : public BaseFieldElOperand
{
public:
  BoolFieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DBool& value) override;

  virtual void SelfAnd(const DBool& value) override;

  virtual void SelfXor(const DBool& value) override;

  virtual void SelfOr(const DBool& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class CharFieldElOperand : public BaseFieldElOperand
{
public:
  CharFieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DChar& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class DateFieldElOperand : public BaseFieldElOperand
{
public:
  DateFieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class DateTimeFieldElOperand : public BaseFieldElOperand
{
public:

  DateTimeFieldElOperand(TableReference* const tableRef,
                         const ROW_INDEX row,
                         const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class HiresTimeFieldElOperand : public BaseFieldElOperand
{
public:
  HiresTimeFieldElOperand(TableReference* const tableRef,
                          const ROW_INDEX row,
                          const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class UInt8FieldElOperand : public BaseFieldElOperand
{
public:
  UInt8FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class UInt16FieldElOperand : public BaseFieldElOperand
{
public:
  UInt16FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
      : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class UInt32FieldElOperand : public BaseFieldElOperand
{
public:
  UInt32FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class UInt64FieldElOperand : public BaseFieldElOperand
{
public:
  UInt64FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class Int8FieldElOperand : public BaseFieldElOperand
{
public:
  Int8FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
      : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class Int16FieldElOperand : public BaseFieldElOperand
{
public:
  Int16FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class Int32FieldElOperand : public BaseFieldElOperand
{
public:
  Int32FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class Int64FieldElOperand : public BaseFieldElOperand
{
public:
  Int64FieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;


  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class RealFieldElOperand : public BaseFieldElOperand
{
public:
  RealFieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class RichRealFieldElOperand : public BaseFieldElOperand
{
public:
  RichRealFieldElOperand(TableReference* const tableRef,
                         const ROW_INDEX row,
                         const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class TextFieldElOperand : public BaseFieldElOperand
{
public:
  TextFieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DText& outValue) const override;
  virtual void SetValue(const DText& value) override;

  virtual void SelfAdd(const DChar& value) override;
  virtual void SelfAdd(const DText& value) override;

  virtual uint_t GetType() override;

  virtual StackValue GetValueAt(const uint64_t index) override;
  virtual StackValue Clone() const override;

  virtual bool StartIterate(const bool reverse, StackValue& outStartItem) override;
};


class ArrayFieldElOperand : public BaseFieldElOperand
{
public:
  ArrayFieldElOperand(TableReference* const tableRef, const ROW_INDEX row, const FIELD_INDEX field)
    : BaseFieldElOperand(tableRef, row, field)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DArray& outValue) const override;
  virtual void SetValue(const DArray& value) override;

  virtual uint_t GetType() override;

  virtual StackValue GetValueAt(const uint64_t index) override;
  virtual StackValue Clone() const override;

  virtual bool StartIterate(const bool reverse, StackValue& outStartItem) override;
};


class CharTextFieldElOperand : public BaseFieldElOperand
{
public:
  CharTextFieldElOperand(TableReference* const tableRef,
                         const ROW_INDEX row,
                         const FIELD_INDEX field,
                         const uint64_t index)
    : BaseFieldElOperand(tableRef, row, field),
      mIndex(index)
  {
  }

  virtual bool IsNull() const override;

  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DChar& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;

  virtual bool Iterate(const bool reverse) override;
  virtual uint64_t IteratorOffset() override;

private:
  const uint64_t    mIndex;
};


class BaseArrayFieldElOperand : public BaseOperand
{
protected:
  BaseArrayFieldElOperand(TableReference* const tableRef,
                          const ROW_INDEX row,
                          const FIELD_INDEX field,
                          const uint64_t index)
     : mIndex(index),
       mTableRef(tableRef),
       mRow(row),
       mField(field)
  {
    mTableRef->IncrementRefCount();
  }

  BaseArrayFieldElOperand(const BaseArrayFieldElOperand& source)
     : mIndex(source.mIndex),
       mTableRef(source.mTableRef),
       mRow(source.mRow),
       mField(source.mField)
  {
    mTableRef->IncrementRefCount();
  }

  virtual ~BaseArrayFieldElOperand() override;

  template <typename DBS_T> void Get(DBS_T& outValue) const
  {
    ITable& table = mTableRef->GetTable();

    if (table.AllocatedRows() <= mRow)
    {
      outValue = DBS_T();
      return;
    }

    DArray array;
    table.Get(mRow, mField, array);
    if (array.Count() <= mIndex)
    {
      outValue = DBS_T();
      return;
    }

    array.Get(mIndex, outValue);

    assert( !outValue.IsNull());
  }

  template <typename DBS_T> void Set(const DBS_T& value)
  {
    ITable& table = mTableRef->GetTable();

    DArray array;

    if (mRow < table.AllocatedRows() )
      table.Get(mRow, mField, array);

    array.Set(mIndex, value);
    table.Set(mRow, mField, array);
  }

  virtual bool IsNull() const override;

  virtual bool Iterate(const bool reverse) override;
  virtual uint64_t IteratorOffset() override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

private:
  BaseArrayFieldElOperand& operator= (const BaseArrayFieldElOperand&);

  const uint64_t    mIndex;
  TableReference*   mTableRef;
  const ROW_INDEX   mRow;
  const FIELD_INDEX mField;
};


class BoolArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  BoolArrayFieldElOperand(TableReference* const tableRef,
                          const ROW_INDEX row,
                          const FIELD_INDEX field,
                          const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DBool& value) override;

  virtual void SelfAnd(const DBool& value) override;

  virtual void SelfXor(const DBool& value) override;

  virtual void SelfOr(const DBool& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class CharArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  CharArrayFieldElOperand(TableReference* const tableRef,
                          const ROW_INDEX row,
                          const FIELD_INDEX field,
                          const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DChar& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class DateArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  DateArrayFieldElOperand(TableReference* const tableRef,
                          const ROW_INDEX row,
                          const FIELD_INDEX field,
                          const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};

class DateTimeArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  DateTimeArrayFieldElOperand(TableReference* const tableRef,
                              const ROW_INDEX row,
                              const FIELD_INDEX field,
                              const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};

class HiresTimeArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  HiresTimeArrayFieldElOperand(TableReference* const tableRef,
                               const ROW_INDEX row,
                               const FIELD_INDEX field,
                               const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DHiresTime& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};

class UInt8ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt8ArrayFieldElOperand(TableReference* const tableRef,
                           const ROW_INDEX row,
                           const FIELD_INDEX field,
                           const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class UInt16ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt16ArrayFieldElOperand(TableReference* const tableRef,
                            const ROW_INDEX row,
                            const FIELD_INDEX field,
                            const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;
  virtual void SelfXor(const DInt64& value) override;
  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class UInt32ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt32ArrayFieldElOperand(TableReference* const tableRef,
                            const ROW_INDEX row,
                            const FIELD_INDEX field,
                            const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class UInt64ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt64ArrayFieldElOperand(TableReference* const tableRef,
                            const ROW_INDEX row,
                            const FIELD_INDEX field,
                            const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};

class Int8ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int8ArrayFieldElOperand(TableReference* const tableRef,
                          const ROW_INDEX row,
                          const FIELD_INDEX field,
                          const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class Int16ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int16ArrayFieldElOperand(TableReference* const tableRef,
                           const ROW_INDEX row,
                           const FIELD_INDEX field,
                           const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class Int32ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int32ArrayFieldElOperand(TableReference* const tableRef,
                           const ROW_INDEX row,
                           const FIELD_INDEX field,
                           const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class Int64ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int64ArrayFieldElOperand(TableReference* const tableRef,
                           const ROW_INDEX row,
                           const FIELD_INDEX field,
                           const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DInt64& value) override;

  virtual void SelfAdd(const DInt64& value) override;

  virtual void SelfSub(const DInt64& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;

  virtual void SelfXor(const DInt64& value) override;

  virtual void SelfOr(const DInt64& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class RealArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  RealArrayFieldElOperand(TableReference* const tableRef,
                          const ROW_INDEX row,
                          const FIELD_INDEX field,
                          const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class RichRealArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  RichRealArrayFieldElOperand(TableReference* const tableRef,
                              const ROW_INDEX row,
                              const FIELD_INDEX field,
                              const uint64_t index)
    : BaseArrayFieldElOperand(tableRef, row, field, index)
  {
  }

  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DText& outValue) const override;

  virtual void SetValue(const DRichReal& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual uint_t GetType() override;

  virtual StackValue Clone() const override;
};


class GlobalValue
{
public:
  template <class OP_T>
  explicit GlobalValue(const OP_T& op)
  {
    const BaseOperand& compileTest = op;
    (void)compileTest; //Just to make sure OP_T is a valid type!

    static_assert(sizeof(OP_T) <= sizeof(mStorage), "GLobal value needs more storage!");
    _placement_new(mStorage, op);
  }

  GlobalValue(GlobalValue&& source)
  {
    for (uint_t i = 0; i < sizeof(mStorage) / sizeof(mStorage[0]); ++i)
      mStorage[i] = source.mStorage[i];

    _placement_new(source.mStorage, NullOperand());
  }

  GlobalValue& operator= (GlobalValue&& source)
  {
    if (this == &source)
      return *this;

    for (uint_t i = 0; i < sizeof(mStorage) / sizeof(mStorage[0]); ++i)
      mStorage[i] = source.mStorage[i];

    _placement_new(source.mStorage, NullOperand());

    return *this;
  }

  ~GlobalValue() { Operand().~BaseOperand(); }

  bool IsNull()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().IsNull();
  }

  template <class DBS_T>
  void GetValue(DBS_T& outValue)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().GetValue(outValue);
  }

  template <class DBS_T>
  void SetValue(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SetValue(value);
  }

  template <class DBS_T>
  void SelfAdd(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfAdd(value);
  }

  template <class DBS_T>
  void SelfSub(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfSub(value);
  }

  template <class DBS_T>
  void SelfMul(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfMul(value);
  }

  template <class DBS_T>
  void SelfDiv(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfDiv(value);
  }

  template <class DBS_T>
  void SelfMod(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfMod(value);
  }

  template <class DBS_T>
  void SelfAnd(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfAnd(value);
  }

  template <class DBS_T>
  void SelfXor(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfXor(value);
  }

  template <class DBS_T>
  void SelfOr(const DBS_T& value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().SelfOr(value);
  }

  uint_t GetType()
  {
    return Operand().GetType();
  }


  FIELD_INDEX GetField()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().GetField();
  }

  ITable& GetTable()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().GetTable();
  }

  StackValue GetFieldAt(const FIELD_INDEX field)
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().GetFieldAt(field);

  }

  StackValue GetValueAt(const uint64_t index)
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().GetValueAt(index);
  }

  StackValue Clone()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().Clone();
  }

  bool StartIterate(const bool  reverse, StackValue& outStartItem)
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().StartIterate(reverse, outStartItem);
  }

  bool PrepareToCopy(void* const dest)
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().CustomCopyIncomplete(dest);
  }

  TableOperand GetTableOp()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().GetTableOp();
  }

  void CopyTableOp(const TableOperand& source)
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().CopyTableOp(source);
  }

  FieldOperand  GetFieldOp()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().GetFieldOp();
  }

  void CopyFieldOp(const FieldOperand& source)
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().CopyFieldOp(source);
  }

  void NativeObject(INativeObject* const value)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().NativeObject(value);
  }

  INativeObject& NativeObject()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().NativeObject();
  }

  TableReference& GetTableReference()
  {
    LockRAII<Lock> dummy(mSync);
    return Operand().GetTableReference();
  }


  void RedfineValue(StackValue& source)
  {
    LockRAII<Lock> dummy(mSync);
    Operand().RedifineValue(source);
  }

  BaseOperand& Operand()
  {
    return *_RC(BaseOperand*, _RC(void*, mStorage));
  }

private:
  Lock          mSync;
  uint64_t      mStorage[QWORDS_PER_OP];
};


class GlobalOperand : public BaseOperand
{
public:
  GlobalOperand(GlobalValue& global);

  virtual bool IsNull() const override;

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;
  virtual void GetValue(DArray& outValue) const override;

  virtual void SetValue(const DBool& value) override;
  virtual void SetValue(const DChar& value) override;
  virtual void SetValue(const DHiresTime& value) override;
  virtual void SetValue(const DInt64& value) override;
  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DRichReal& value) override;
  virtual void SetValue(const DText& value) override;
  virtual void SetValue(const DArray& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;
  virtual void SelfAdd(const DChar& value) override;
  virtual void SelfAdd(const DText& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;
  virtual void SelfAnd(const DBool& value) override;

  virtual void SelfXor(const DInt64& value) override;
  virtual void SelfXor(const DBool& value) override;

  virtual void SelfOr(const DInt64& value) override;
  virtual void SelfOr(const DBool& value) override;

  virtual uint_t GetType() override;

  virtual FIELD_INDEX GetField() override;

  virtual ITable& GetTable() override;

  virtual StackValue GetFieldAt(const FIELD_INDEX field) override;
  virtual StackValue GetValueAt(const uint64_t index) override;
  virtual StackValue Clone() const override;

  virtual bool StartIterate(const bool  reverse, StackValue& outStartItem) override;

  virtual bool CustomCopyIncomplete(void* const) override;

  virtual TableReference& GetTableReference() override;
  virtual TableOperand GetTableOp() override;
  virtual void CopyTableOp(const TableOperand& source) override;
  virtual FieldOperand GetFieldOp() override;
  virtual void CopyFieldOp(const FieldOperand& source) override;
  virtual void RedifineValue(StackValue& source) override;


  virtual void           NativeObject(INativeObject* const value) override;
  virtual INativeObject& NativeObject() override;

private:
  GlobalValue&    mValue;
};


class LocalOperand : public BaseOperand
{
public:
  LocalOperand(SessionStack& stack, const uint64_t index);

  virtual bool IsNull() const override;

  virtual void GetValue(DBool& outValue) const override;
  virtual void GetValue(DChar& outValue) const override;
  virtual void GetValue(DDate& outValue) const override;
  virtual void GetValue(DDateTime& outValue) const override;
  virtual void GetValue(DHiresTime& outValue) const override;
  virtual void GetValue(DInt8& outValue) const override;
  virtual void GetValue(DInt16& outValue) const override;
  virtual void GetValue(DInt32& outValue) const override;
  virtual void GetValue(DInt64& outValue) const override;
  virtual void GetValue(DReal& outValue) const override;
  virtual void GetValue(DRichReal& outValue) const override;
  virtual void GetValue(DUInt8& outValue) const override;
  virtual void GetValue(DUInt16& outValue) const override;
  virtual void GetValue(DUInt32& outValue) const override;
  virtual void GetValue(DUInt64& outValue) const override;
  virtual void GetValue(DText& outValue) const override;
  virtual void GetValue(DArray& outValue) const override;

  virtual void SetValue(const DBool& value) override;
  virtual void SetValue(const DChar& value) override;
  virtual void SetValue(const DHiresTime& value) override;
  virtual void SetValue(const DInt64& value) override;
  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DRichReal& value) override;
  virtual void SetValue(const DText& value) override;
  virtual void SetValue(const DArray& value) override;

  virtual void SelfAdd(const DInt64& value) override;
  virtual void SelfAdd(const DRichReal& value) override;
  virtual void SelfAdd(const DChar& value) override;
  virtual void SelfAdd(const DText& value) override;

  virtual void SelfSub(const DInt64& value) override;
  virtual void SelfSub(const DRichReal& value) override;

  virtual void SelfMul(const DInt64& value) override;
  virtual void SelfMul(const DUInt64& value) override;
  virtual void SelfMul(const DRichReal& value) override;

  virtual void SelfDiv(const DInt64& value) override;
  virtual void SelfDiv(const DUInt64& value) override;
  virtual void SelfDiv(const DRichReal& value) override;

  virtual void SelfMod(const DInt64& value) override;
  virtual void SelfMod(const DUInt64& value) override;

  virtual void SelfAnd(const DInt64& value) override;
  virtual void SelfAnd(const DBool& value) override;

  virtual void SelfXor(const DInt64& value) override;
  virtual void SelfXor(const DBool& value) override;

  virtual void SelfOr(const DInt64& value) override;
  virtual void SelfOr(const DBool& value) override;

  virtual uint_t GetType() override;

  virtual FIELD_INDEX GetField() override;

  virtual ITable& GetTable() override;

  virtual StackValue GetFieldAt(const FIELD_INDEX field) override;
  virtual StackValue GetValueAt(const uint64_t index) override;
  virtual StackValue Clone() const override;

  virtual bool StartIterate(const bool  reverse, StackValue& outStartItem) override;
  virtual bool Iterate(const bool reverse) override;
  virtual uint64_t IteratorOffset() override;

  virtual TableOperand GetTableOp() override;

  virtual TableReference& GetTableReference() override;
  virtual void CopyTableOp(const TableOperand& source) override;
  virtual FieldOperand GetFieldOp() override;
  virtual void CopyFieldOp(const FieldOperand& source) override;
  virtual void RedifineValue(StackValue& source) override;

  virtual INativeObject& NativeObject() override;


private:
  const uint64_t      mIndex;
  SessionStack&       mStack;
};


} //namespace prima
} //namespace whais


#endif /* PM_OPERAND_H_ */
