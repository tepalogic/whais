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

#include "pm_operand_undefined.h"

#include "pm_operand.h"


namespace whisper {


INativeObject::~INativeObject ()
{
}


namespace prima {


NativeObjectOperand::NativeObjectOperand ()
  : mType (T_UNDETERMINED)
{
  mNativeValue = NULL;
}


NativeObjectOperand::NativeObjectOperand (INativeObject& object)
  : mType (T_UNDETERMINED)
{
  mNativeValue = &object;

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const DArray& array)
  : mType (array.Type ())
{
  MARK_ARRAY (mType);

  _placement_new (mArrayValue, array);

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const DText& text)
  : mType (T_TEXT)
{
  _placement_new (mTextValue, text);

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const bool      value)
  : mType (T_BOOL)
{
  mUIntValue.mValue = value ? 1 : 0;
  mUIntValue.mNull  = nullValue;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const uint32_t  value)
  : mType (T_CHAR)
{
  mUIntValue.mValue = value;
  mUIntValue.mNull  = nullValue;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const int64_t   value)
  : mType (T_INT64)
{
  mIntValue.mValue = value;
  mIntValue.mNull  = nullValue;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const uint64_t  value)
  : mType (T_UINT64)
{
  mUIntValue.mValue = value;
  mUIntValue.mNull  = nullValue;
}


NativeObjectOperand::NativeObjectOperand (const bool     nullValue,
                                          const int64_t  valIntPart,
                                          const int64_t  valFracPart)
  : mType (T_RICHREAL)
{
  mRealValue.mIntPart   = valIntPart;
  mRealValue.mFracPart  = valFracPart;
  mRealValue.mNull      = nullValue;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const uint16_t  year,
                                          const uint8_t   month,
                                          const uint8_t   day,
                                          const uint8_t   hours,
                                          const uint8_t   mins,
                                          const uint8_t   secs,
                                          const uint32_t  microsec)
  : mType (T_HIRESTIME)
{
  mTimeValue.mMicrosec    = microsec;
  mTimeValue.mYear        = year;
  mTimeValue.mMonth       = month;
  mTimeValue.mDay         = day;
  mTimeValue.mHours       = hours;
  mTimeValue.mMins        = mins;
  mTimeValue.mSecs        = secs;
  mTimeValue.mNull        = nullValue;
}


NativeObjectOperand::NativeObjectOperand (TableReference& tableRef)
  : mType (T_UNDETERMINED)
{
  MARK_TABLE (mType);

  mTableValue = &tableRef;

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (TableReference* const tableRef,
                                          const uint_t          fieldIndex,
                                          const uint_t          type)
  : mType (type)
{
  assert (IS_FIELD (mType));

  mFieldValue.mTableRef   = tableRef;
  mFieldValue.mFieldIndex = fieldIndex;

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const NativeObjectOperand& source)
{
  if (IS_TABLE (source.mType)
      || IS_FIELD (source.mType))
    {
      memcpy (this, &source, sizeof (*this));
    }
  else if (IS_ARRAY (source.mType))
    {
      mType = source.mType;

      _placement_new (mArrayValue, *_RC (const DArray*, source.mArrayValue));
    }
  else if (source.mType == T_TEXT)
    {
      mType = T_TEXT;

      _placement_new (mTextValue, *_RC (const DText*, source.mTextValue));
    }
  else
    memcpy (this, &source, sizeof (*this));

  Initialise ();
}


NativeObjectOperand::~NativeObjectOperand ()
{
  Cleanup ();
}


NativeObjectOperand&
NativeObjectOperand::operator= (const NativeObjectOperand& source)
{
  if (this == &source)
    return *this;

  Cleanup ();

  if (IS_TABLE (source.mType)
      || IS_FIELD (source.mType))
    {
      memcpy (this, &source, sizeof (*this));
    }
  else if (IS_ARRAY (source.mType))
    {
      mType = source.mType;

      _placement_new (mArrayValue, *_RC (const DArray*, source.mArrayValue));
    }
  else if (source.mType == T_TEXT)
    {
      mType = T_TEXT;

      _placement_new (mTextValue, *_RC (const DText*, source.mTextValue));
    }
  else
    memcpy (this, &source, sizeof (*this));

  Initialise ();

  return *this;
}


void
NativeObjectOperand::Initialise ()
{
  if (mType == T_UNDETERMINED)
    {
      if (mNativeValue)
        {
          assert (IsNull () == false);

          mNativeValue->RegisterUser ();
        }
      else
        {
          assert (IsNull ());
        }
    }
  else if (IS_TABLE (mType))
    {
      assert (mTableValue != NULL);

      mTableValue->IncrementRefCount ();
    }
  else if (IS_FIELD (mType))
    {
      if (mFieldValue.mTableRef)
        {
          assert (IsNull () == false);

          mFieldValue.mTableRef->IncrementRefCount ();
        }
      else
        {
          assert (IsNull ());
        }
    }
  else if (IS_ARRAY (mType))
    {
      assert ((GET_BASIC_TYPE (mType) >= T_BOOL)
              && ((GET_BASIC_TYPE (mType) < T_TEXT)
                  || (GET_BASIC_TYPE (mType) == T_UNDETERMINED)));
    }
  else
    {
      assert ((mType == T_BOOL) || (mType == T_CHAR) || (mType == T_HIRESTIME)
              || (mType == T_INT64) || (mType == T_UINT64)
              || (mType == T_RICHREAL));
    }
}


void
NativeObjectOperand::Cleanup ()
{
  if (mType == T_UNDETERMINED)
    {
      if (mNativeValue)
        {
          assert (IsNull () == false);

          mNativeValue->ReleaseUser ();
        }
      else
        {
          assert (IsNull ());
        }
    }
  else if (IS_TABLE (mType))
    {
      assert (mTableValue != NULL);

      mTableValue->DecrementRefCount ();
    }
  else if (IS_FIELD (mType))
    {
      if (mFieldValue.mTableRef)
        {
          assert (IsNull () == false);

          mFieldValue.mTableRef->DecrementRefCount ();
        }
      else
        {
          assert (IsNull ());
        }
    }
  else if (IS_ARRAY (mType))
    {
      DArray* const array = _RC (DArray*, mArrayValue);

      array->~DArray ();

      assert ((GET_BASIC_TYPE (mType) >= T_BOOL)
              && ((GET_BASIC_TYPE (mType) < T_TEXT)
                  || (GET_BASIC_TYPE (mType) == T_UNDETERMINED)));
    }
  else if (mType == T_TEXT)
    {
      DText* const text = _RC (DText*, mTextValue);

      text->~DText ();
    }
  else
    {
      assert ((mType == T_BOOL) || (mType == T_CHAR) || (mType == T_HIRESTIME)
              || (mType == T_INT64) || (mType == T_UINT64)
              || (mType == T_RICHREAL));
    }
}


bool
NativeObjectOperand::IsNull () const
{
  if (mType == T_UNDETERMINED)
    return mNativeValue == NULL;

  else if (IS_TABLE (mType))
    return mTableValue->GetTable ().AllocatedRows () == 0;

  else if (IS_FIELD (mType))
    return mFieldValue.mTableRef == NULL;

  else if (IS_ARRAY (mType))
    {
      const DArray* const array = _RC (const DArray*, mArrayValue);

      return array->Count () == 0;
    }
  else if (mType == T_TEXT)
    {
      const DText* const text = _RC (const DText*, mTextValue);

      return text->RawSize () == 0;
    }
  else
    {
      switch (mType)
        {
        case T_BOOL:
        case T_CHAR:
        case T_UINT64:
          return mUIntValue.mNull;

        case T_INT64:
          return mIntValue.mNull;

        case T_RICHREAL:
          return mRealValue.mNull;

        case T_HIRESTIME:
          return mTimeValue.mNull;

        default:
          assert (false);
        }
    }

  throw InterException (_EXTRA (InterException::INTERNAL_ERROR));
}


void
NativeObjectOperand::GetValue (DBool& outValue) const
{
  if (mType != T_BOOL)
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DBool ();

  else
    outValue = DBool (mUIntValue.mValue != 0);
}


void
NativeObjectOperand::GetValue (DChar& outValue) const
{
  if (mType != T_CHAR)
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DChar ();

  else
    outValue = DChar (mUIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DDate& outValue) const
{
  if (mType != T_HIRESTIME)
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DDate ();

  else
    outValue = DDate (mTimeValue.mYear, mTimeValue.mMonth, mTimeValue.mDay);
}


void
NativeObjectOperand::GetValue (DDateTime& outValue) const
{
  if (mType != T_HIRESTIME)
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DDateTime ();

  else
    {
      outValue = DDateTime (mTimeValue.mYear,
                            mTimeValue.mMonth,
                            mTimeValue.mDay,
                            mTimeValue.mHours,
                            mTimeValue.mMins,
                            mTimeValue.mSecs);
    }
}


void
NativeObjectOperand::GetValue (DHiresTime& outValue) const
{
  if (mType != T_HIRESTIME)
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DHiresTime ();

  else
    {
      outValue = DHiresTime (mTimeValue.mYear,
                             mTimeValue.mMonth,
                             mTimeValue.mDay,
                             mTimeValue.mHours,
                             mTimeValue.mMins,
                             mTimeValue.mSecs,
                             mTimeValue.mMicrosec);
    }
}


void
NativeObjectOperand::GetValue (DInt8& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DInt8 ();

  else
    outValue = DInt8 (mIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DInt16& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DInt16 ();

  else
    outValue = DInt16 (mIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DInt32& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DInt32 ();

  else
    outValue = DInt32 (mIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DInt64& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DInt64 ();

  else
    outValue = DInt64 (mIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DUInt8& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DUInt8 ();

  else
    outValue = DUInt8 (mUIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DUInt16& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DUInt16 ();

  else
    outValue = DUInt16 (mUIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DUInt32& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DUInt32 ();

  else
    outValue = DUInt32 (mUIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DUInt64& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DUInt64 ();

  else
    outValue = DUInt64 (mUIntValue.mValue);
}


void
NativeObjectOperand::GetValue (DReal& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64) && (mType != T_RICHREAL))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DReal ();

  else if (mType == T_RICHREAL)
    {
      outValue = DReal (REAL_T (mRealValue.mIntPart,
                                mRealValue.mFracPart,
                                DBS_RICHREAL_PREC));
    }
  else
    outValue = DReal (REAL_T (mIntValue.mValue, 0, DBS_REAL_PREC));
}


void
NativeObjectOperand::GetValue (DRichReal& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64) && (mType != T_RICHREAL))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  if (IsNull ())
    outValue = DRichReal ();

  else if (mType == T_RICHREAL)
    {
      outValue = DRichReal (RICHREAL_T (mRealValue.mIntPart,
                                        mRealValue.mFracPart,
                                        DBS_RICHREAL_PREC));
    }
  else
    outValue = DRichReal (RICHREAL_T (mIntValue.mValue, 0, DBS_RICHREAL_PREC));
}


void
NativeObjectOperand::GetValue (DText& outValue) const
{
  if (mType != T_TEXT)
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  outValue = *_RC (const DText*, mTextValue);
}


void
NativeObjectOperand::GetValue (DArray& outValue) const
{
  if ( ! IS_ARRAY (mType))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  outValue = *_RC (const DArray*, mArrayValue);
}


void
NativeObjectOperand::SetValue (const DBool& value)
{
  NativeObjectOperand temp (value.IsNull (), value.mValue);

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DChar& value)
{
  NativeObjectOperand temp (value.IsNull (), value.mValue);

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DDate& value)
{
  NativeObjectOperand temp (value.IsNull (),
                            value.mYear,
                            value.mMonth,
                            value.mDay);

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DDateTime& value)
{
  NativeObjectOperand temp (value.IsNull (),
                            value.mYear,
                            value.mMonth,
                            value.mDay,
                            value.mHour,
                            value.mMinutes,
                            value.mSeconds);
  *this = temp;
}


void
NativeObjectOperand::SetValue (const DHiresTime& value)
{
  NativeObjectOperand temp (value.IsNull (),
                            value.mYear,
                            value.mMonth,
                            value.mDay,
                            value.mHour,
                            value.mMinutes,
                            value.mSeconds,
                            value.mMicrosec);
  *this = temp;
}


void
NativeObjectOperand::SetValue (const DInt8& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (int64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DInt16& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (int64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DInt32& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (int64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DInt64& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (int64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DUInt8& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (uint64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DUInt16& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (uint64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DUInt32& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (uint64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DUInt64& value)
{
  NativeObjectOperand temp (value.IsNull (), _SC (uint64_t, value.mValue));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DReal& value)
{
  NativeObjectOperand temp (value.IsNull (),
                            value.mValue.Integer (),
                            value.mValue.Fractional () *
                              (DBS_RICHREAL_PREC / DBS_REAL_PREC));

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DText& text)
{
  NativeObjectOperand temp (text);

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DArray& array)
{
  NativeObjectOperand temp (array);

  *this = temp;
}


void
NativeObjectOperand::SetValue (const DRichReal& value)
{
  NativeObjectOperand temp (value.IsNull (),
                            value.mValue.Integer (),
                            value.mValue.Fractional ());

  *this = temp;
}


uint_t
NativeObjectOperand::GetType ()
{
  return mType;
}


FIELD_INDEX
NativeObjectOperand::GetField ()
{
  if ( IS_FIELD (mType))
    return mFieldValue.mFieldIndex;

  throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


ITable&
NativeObjectOperand::GetTable ()
{
  if (IS_TABLE (mType))
    return mTableValue->GetTable ();

  else if (IS_FIELD (mType))
    {
      if (IsNull ())
        throw InterException (_EXTRA (InterException::NATIVE_NULL_DEREFERENCE));

      return mFieldValue.mTableRef->GetTable ();
    }

  throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::NativeObject (INativeObject* const object)
{
  if (object == NULL)
    *this = NativeObjectOperand ();

  else
    {
      NativeObjectOperand temp (*object);

      *this = temp;
    }
}


void
NativeObjectOperand::CopyNativeObjectOperand (
                                        const NativeObjectOperand& source
                                             )
{
  *this = source;
}

INativeObject&
NativeObjectOperand::NativeObject ()
{
  if (mType != T_UNDETERMINED)
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  else if (IsNull ())
    throw InterException (_EXTRA (InterException::NATIVE_NULL_DEREFERENCE));

  assert (mNativeValue != NULL);

  return *mNativeValue;
}


StackValue
NativeObjectOperand::Duplicate () const
{
  return StackValue (*this);
}


void
NativeObjectOperand::NotifyCopy ()
{
  Initialise ();
}


TableReference&
NativeObjectOperand::GetTableReference ()
{
  if (! (IS_TABLE (mType) || IS_FIELD (mType)))
    throw InterException (_EXTRA (InterException::INVALID_NATIVE_OP_REQ));

  else if (IS_FIELD (mType) && IsNull ())
      throw InterException (_EXTRA (InterException::NATIVE_NULL_DEREFERENCE));

  if (IS_TABLE (mType))
    return *mTableValue;

  assert (IS_FIELD (mType));

  return *mFieldValue.mTableRef;
}


} // namespace prima
} // namespace whisper

