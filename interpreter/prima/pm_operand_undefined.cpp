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
namespace prima {


NativeObjectOperand::NativeObjectOperand ()
  : mType (T_UNDETERMINED),
    mNull (true)
{
}


NativeObjectOperand::NativeObjectOperand (INativeObject& object)
  : mType (T_UNDETERMINED),
    mNull (false)
{
  mNativeValue = &object;

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const DArray& array)
  : mType (array.Type ()),
    mNull (array.Count () == 0)
{
  MARK_ARRAY (mType);

  _placement_new (mArrayValue, array);

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const DText& text)
  : mType (T_TEXT),
    mNull (text.Count () == 0)
{
  _placement_new (mTextValue, text);

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const bool      value)
  : mType (T_BOOL),
    mNull (nullValue)
{
  mUIntValue = value ? 1 : 0;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const uint32_t  value)
  : mType (T_CHAR),
    mNull (nullValue)
{
  mUIntValue = value;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const int64_t   value)
  : mType (T_INT64),
    mNull (nullValue)
{
  mIntValue = value;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const uint64_t  value)
  : mType (T_UINT64),
    mNull (nullValue)
{
  mUIntValue = value;
}


NativeObjectOperand::NativeObjectOperand (const bool     nullValue,
                                          const int64_t  valIntPart,
                                          const int64_t  valFracPart)
  : mType (T_RICHREAL),
    mNull (nullValue)
{
  mRealValue.mIntPart   = valIntPart;
  mRealValue.mFracPart  = valFracPart;
}


NativeObjectOperand::NativeObjectOperand (const bool      nullValue,
                                          const uint16_t  year,
                                          const uint8_t   month,
                                          const uint8_t   day,
                                          const uint8_t   hours,
                                          const uint8_t   mins,
                                          const uint8_t   secs,
                                          const uint32_t  microsec)
  : mType (T_HIRESTIME),
    mNull (nullValue)
{
  mTimeValue.mMicrosec    = microsec;
  mTimeValue.mYear        = year;
  mTimeValue.mMonth       = month;
  mTimeValue.mDay         = day;
  mTimeValue.mHours       = hours;
  mTimeValue.mMins        = mins;
  mTimeValue.mSecs        = secs;
}


NativeObjectOperand::NativeObjectOperand (TableReference* const tableRef)
  : mType (T_UNDETERMINED),
    mNull (tableRef == NULL)
{
  MARK_TABLE (mType);

  mTableValue = tableRef;

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (TableReference* const tableRef,
                                          const uint_t          fieldIndex,
                                          const uint_t          type)
  : mType (type),
    mNull (tableRef == NULL)
{
  assert (IS_FIELD (mType));

  mFieldValue.mTableRef   = tableRef;
  mFieldValue.mFieldIndex = fieldIndex;

  Initialise ();
}


NativeObjectOperand::NativeObjectOperand (const NativeObjectOperand& source)
{
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
          assert (mNull == false);

          mNativeValue->RegisterUser ();
        }
      else
        {
          assert (mNull);
        }
    }
  else if (IS_TABLE (mType))
    {
      if (mTableValue)
        {
          assert (mNull == false);

          mTableValue->IncrementRefCount ();
        }
      else
        {
          assert (mNull);
        }
    }
  else if (IS_FIELD (mType))
    {
      if (mFieldValue.mTableRef)
        {
          assert (mNull == false);

          mFieldValue.mTableRef->IncrementRefCount ();
        }
      else
        {
          assert (mNull);
        }
    }
  else
    {
      assert (IS_ARRAY (mType)
              || ((mType >= T_BOOL) && (mType <= T_TEXT)));
    }
}


void
NativeObjectOperand::Cleanup ()
{
  if (mType == T_UNDETERMINED)
    {
      if (mNativeValue)
        {
          assert (mNull == false);

          mNativeValue->ReleaseUser ();
        }
      else
        {
          assert (mNull);
        }
    }
  else if (IS_TABLE (mType))
    {
      if (mTableValue)
        {
          assert (mNull == false);

          mTableValue->DecrementRefCount ();
        }
      else
        {
          assert (mNull);
        }
    }
  else if (IS_FIELD (mType))
    {
      if (mFieldValue.mTableRef)
        {
          assert (mNull == false);

          mFieldValue.mTableRef->DecrementRefCount ();
        }
      else
        {
          assert (mNull);
        }
    }
  else if (IS_ARRAY (mType))
    {
      DArray* const array = _RC (DArray*, mArrayValue);

      array->~DArray ();
    }
  else if (mType == T_TEXT)
    {
      DText* const text = _RC (DText*, mTextValue);

      text->~DText ();
    }
  else
    {
      assert ((mType >= T_BOOL) && (mType < T_TEXT));
    }
}


bool
NativeObjectOperand::IsNull () const
{
  return mNull;
}


void
NativeObjectOperand::GetValue (DBool& outValue) const
{
  if (mType != T_BOOL)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DBool ();

  else
    outValue = DBool (mUIntValue != 0);
}


void
NativeObjectOperand::GetValue (DChar& outValue) const
{
  if (mType != T_CHAR)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DChar ();

  else
    outValue = DChar (mUIntValue);
}


void
NativeObjectOperand::GetValue (DDate& outValue) const
{
  if (mType != T_HIRESTIME)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DDate ();

  else
    outValue = DDate (mTimeValue.mYear, mTimeValue.mMonth, mTimeValue.mDay);
}


void
NativeObjectOperand::GetValue (DDateTime& outValue) const
{
  if (mType != T_HIRESTIME)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
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
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
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
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DInt8 ();

  else
    outValue = DInt8 (mIntValue);
}


void
NativeObjectOperand::GetValue (DInt16& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DInt16 ();

  else
    outValue = DInt16 (mIntValue);
}


void
NativeObjectOperand::GetValue (DInt32& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DInt32 ();

  else
    outValue = DInt32 (mIntValue);
}


void
NativeObjectOperand::GetValue (DInt64& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DInt64 ();

  else
    outValue = DInt64 (mIntValue);
}


void
NativeObjectOperand::GetValue (DUInt8& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DUInt8 ();

  else
    outValue = DUInt8 (mUIntValue);
}


void
NativeObjectOperand::GetValue (DUInt16& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DUInt16 ();

  else
    outValue = DUInt16 (mUIntValue);
}


void
NativeObjectOperand::GetValue (DUInt32& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DUInt32 ();

  else
    outValue = DUInt32 (mUIntValue);
}


void
NativeObjectOperand::GetValue (DUInt64& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DUInt64 ();

  else
    outValue = DUInt64 (mUIntValue);
}


void
NativeObjectOperand::GetValue (DReal& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64) && (mType != T_RICHREAL))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DReal ();

  else if (mType == T_RICHREAL)
    {
      outValue = DReal (REAL_T (mRealValue.mIntPart,
                                mRealValue.mFracPart,
                                DBS_RICHREAL_PREC));
    }
  else
    outValue = DReal (REAL_T (mIntValue, 0, DBS_REAL_PREC));
}


void
NativeObjectOperand::GetValue (DRichReal& outValue) const
{
  if ((mType != T_INT64) && (mType != T_UINT64) && (mType != T_RICHREAL))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  if (mNull)
    outValue = DRichReal ();

  else if (mType == T_RICHREAL)
    {
      outValue = DRichReal (RICHREAL_T (mRealValue.mIntPart,
                                        mRealValue.mFracPart,
                                        DBS_RICHREAL_PREC));
    }
  else
    outValue = DRichReal (RICHREAL_T (mIntValue, 0, DBS_RICHREAL_PREC));
}


void
NativeObjectOperand::GetValue (DText& outValue) const
{
  if (mType != T_TEXT)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

  outValue = *_RC (const DText*, mTextValue);
}


void
NativeObjectOperand::GetValue (DArray& outValue) const
{
  if ( ! IS_ARRAY (mType))
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }

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
                              DBS_RICHREAL_PREC / DBS_REAL_PREC);

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


void
NativeObjectOperand::SelfAdd (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfAdd (const DRichReal& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfAdd (const DChar& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfAdd (const DText& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfSub (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfSub (const DRichReal& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfMul (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfMul (const DRichReal& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfDiv (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfDiv (const DRichReal& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfMod (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfAnd (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfAnd (const DBool& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfXor (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfXor (const DBool& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfOr (const DInt64& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


void
NativeObjectOperand::SelfOr (const DBool& value)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
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

  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


ITable&
NativeObjectOperand::GetTable ()
{
  assert (mNull == false);

  if (IS_TABLE (mType))
    return mTableValue->GetTable ();

  else if (IS_FIELD (mType))
    return mFieldValue.mTableRef->GetTable ();

  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


StackValue
NativeObjectOperand::GetFieldAt (const FIELD_INDEX field)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
}


StackValue
NativeObjectOperand::GetValueAt (const uint64_t field)
{
  throw InterException (NULL, _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
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


INativeObject&
NativeObjectOperand::NativeObject ()
{
  if (mType != T_UNDETERMINED)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }
  else if (mNull)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::NATIVE_NULL_DEREFERENCE));
    }

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
    {
      throw InterException (NULL,
                            _EXTRA (InterException::INVALID_NATIVE_OP_REQ));
    }
  else if (mNull)
    {
      throw InterException (NULL,
                            _EXTRA (InterException::NATIVE_NULL_DEREFERENCE));
    }

  if (IS_TABLE (mType))
    return *mTableValue;

  assert (IS_FIELD (mType));

  return *mFieldValue.mTableRef;
}


} // namespace prima
} // namespace whisper

