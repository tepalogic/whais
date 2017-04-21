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

#ifndef PM_OPERAND_UNDEFINED_H_
#define PM_OPERAND_UNDEFINED_H_

#include "interpreter.h"
#include "operands.h"

#include "pm_table.h"
#include "pm_operand.h"


namespace whais {
namespace prima {


class UndefinedOperand final : public BaseOperand
{
public:
  UndefinedOperand();
  explicit UndefinedOperand(INativeObject& object);
  explicit UndefinedOperand(const DArray& array);
  explicit UndefinedOperand(const DText& text);

  UndefinedOperand(const bool nullValue, const bool value);
  UndefinedOperand(const bool nullValue, const uint32_t value);
  UndefinedOperand(const bool nullValue, const int64_t value);
  UndefinedOperand(const bool nullValue, const uint64_t value);
  UndefinedOperand(const bool nullValue, const int64_t valIntPart, const int64_t valFracPart);
  UndefinedOperand(const bool nullValue,
                   const uint16_t year,
                   const uint8_t month,
                   const uint8_t day,
                   const uint8_t hours = 0,
                   const uint8_t mins = 0,
                   const uint8_t secs = 0,
                   const uint32_t microsec = 0);

  explicit UndefinedOperand(TableReference& tableRef);
  UndefinedOperand(TableReference& tableRef, const uint_t fieldIndex, const uint_t type);
  UndefinedOperand(TableReference* const tableRef, const uint_t fieldIndex, const uint_t type);

  UndefinedOperand(const UndefinedOperand& source);
  virtual ~UndefinedOperand() override;

  UndefinedOperand& operator=(const UndefinedOperand& source);

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
  virtual void SetValue(const DDate& value);
  virtual void SetValue(const DDateTime& value);
  virtual void SetValue(const DHiresTime& value) override;
  virtual void SetValue(const DInt8& value);
  virtual void SetValue(const DInt16& value);
  virtual void SetValue(const DInt32& value);
  virtual void SetValue(const DInt64& value) override;
  virtual void SetValue(const DReal& value);
  virtual void SetValue(const DRichReal& value) override;
  virtual void SetValue(const DUInt8& value);
  virtual void SetValue(const DUInt16& value);
  virtual void SetValue(const DUInt32& value);
  virtual void SetValue(const DUInt64& value) override;
  virtual void SetValue(const DText& value) override;
  virtual void SetValue(const DArray& value) override;

  virtual uint_t GetType() override;
  virtual FIELD_INDEX GetField() override;
  virtual ITable& GetTable() override;
  virtual StackValue Duplicate() const override;

  virtual void CopyUndefinedOperand(const UndefinedOperand& source) override;

  virtual void NativeObject(INativeObject* const value) override;
  virtual INativeObject& NativeObject() override;

  virtual bool CustomCopyIncomplete(void* const dest) override;

  virtual TableReference& GetTableReference() override;

private:
  struct RealValue
  {
    int64_t   mIntPart  : 64;
    int64_t   mFracPart : 63;
    bool      mNull     : 1;
  };

  struct FieldValue
  {
    TableReference*   mTableRef;
    FIELD_INDEX       mFieldIndex;
    bool              mNull;
  };

  struct TimeValue
  {
    uint32_t  mMicrosec;
    int16_t   mYear;
    uint8_t   mMonth;
    uint8_t   mDay;
    uint8_t   mHours;
    uint8_t   mMins;
    uint8_t   mSecs;
    bool      mNull;
  };

  struct IntValue
  {
    int64_t   mValue;
    bool      mNull;
  };

  struct UIntValue
  {
    uint64_t   mValue;
    bool       mNull;
  };

  void Initialise();
  void Cleanup();

  union
  {
    UIntValue          mUIntValue;
    IntValue           mIntValue;

    INativeObject*     mNativeValue;
    TableReference*    mTableValue;
    FieldValue         mFieldValue;
    RealValue          mRealValue;
    TimeValue          mTimeValue;

    uint8_t            mArrayValue[sizeof(DArray)];
    uint8_t            mTextValue[sizeof(DText)];
  };
  uint16_t mType;
};


} // namespace prima
} // namespace whais

#endif /* PM_OPERAND_UNDEFINED_H_ */

