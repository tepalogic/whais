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

#ifndef PM_OPERAND_H_
#define PM_OPERAND_H_

#include "utils/wthread.h"

#include "interpreter.h"
#include "operands.h"

#include "pm_table.h"



namespace whisper {
namespace prima {



template <typename T_DEST, typename T_SRC> T_DEST
internal_add (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return _SC (T_DEST, firstOp.mValue + secondOp.mValue);
}


template <typename T_DEST, typename T_SRC> T_DEST
internal_sub (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.mValue - secondOp.mValue);
}


template <typename T_DEST, typename T_SRC> T_DEST
internal_mul (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.mValue * secondOp.mValue);
}


template <typename T_DEST, typename T_SRC> T_DEST
internal_div (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.mValue / secondOp.mValue);
}


template <typename T_DEST, typename T_SRC> T_DEST
internal_mod (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.mValue % secondOp.mValue);
}


template <typename T_DEST, typename T_SRC> T_DEST
internal_and (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.mValue & secondOp.mValue);
}


template <typename T_DEST, typename T_SRC> T_DEST
internal_xor (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.mValue ^ secondOp.mValue);
}


template <typename T_DEST, typename T_SRC> T_DEST
internal_or (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.mValue ^ secondOp.mValue);
}


template <typename T_SRC, typename T_DEST> void
number_convert (const T_SRC& from, T_DEST& to)
{
  to = from.IsNull () ? T_DEST () : _SC (T_DEST, from.mValue);
}




class TableOperand;
class FieldOperand;



class BaseOperand : public IOperand
{
public:

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;
  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;
  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;
  virtual void GetValue (DText& outValue) const;
  virtual void GetValue (DArray& outValue) const;

  virtual void SetValue (const DBool& value);
  virtual void SetValue (const DChar& value);
  virtual void SetValue (const DDate& value);
  virtual void SetValue (const DDateTime& value);
  virtual void SetValue (const DHiresTime& value);
  virtual void SetValue (const DInt8& value);
  virtual void SetValue (const DInt16& value);
  virtual void SetValue (const DInt32& value);
  virtual void SetValue (const DInt64& value);
  virtual void SetValue (const DReal& value);
  virtual void SetValue (const DRichReal& value);
  virtual void SetValue (const DUInt8& value);
  virtual void SetValue (const DUInt16& value);
  virtual void SetValue (const DUInt32& value);
  virtual void SetValue (const DUInt64& value);
  virtual void SetValue (const DText& value);
  virtual void SetValue (const DArray& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);
  virtual void SelfAdd (const DChar& value);
  virtual void SelfAdd (const DText& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);
  virtual void SelfAnd (const DBool& value);

  virtual void SelfXor (const DInt64& value);
  virtual void SelfXor (const DBool& value);

  virtual void SelfOr (const DInt64& value);
  virtual void SelfOr (const DBool& value);

  virtual FIELD_INDEX GetField ();

  virtual ITable& GetTable ();

  virtual StackValue GetFieldAt (const FIELD_INDEX field);

  virtual StackValue GetValueAt (const uint64_t index);

  virtual TableOperand GetTableOp ();

  virtual void CopyTableOp (const TableOperand& source);

  virtual FieldOperand GetFieldOp ();

  virtual void CopyFieldOp (const FieldOperand& source);

  virtual void NotifyCopy ();
};



class NullOperand : public BaseOperand
{
public:
  NullOperand ()
    : BaseOperand ()
  {
  }

  virtual ~NullOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;
  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;
  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;
  virtual void GetValue (DText& outValue) const;
  virtual void GetValue (DArray& outValue) const;

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class BoolOperand : public BaseOperand
{
public:
  explicit BoolOperand (const DBool& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;

  virtual void SetValue (const DBool& value);

  virtual void SelfAnd (const DBool& value);

  virtual void SelfXor (const DBool& value);

  virtual void SelfOr (const DBool& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DBool mValue;
};


class CharOperand : public BaseOperand
{
public:
  explicit CharOperand (const DChar& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DChar mValue;
};


class DateOperand : public BaseOperand
{
public:
  explicit DateOperand (const DDate& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDate& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DDate mValue;
};


class DateTimeOperand : public BaseOperand
{
public:
  explicit DateTimeOperand (const DDateTime& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDateTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DDateTime mValue;
};


class HiresTimeOperand : public BaseOperand
{
public:
  explicit HiresTimeOperand (const DHiresTime& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DHiresTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DHiresTime mValue;
};


class UInt8Operand : public BaseOperand
{
public:
  explicit UInt8Operand (const DUInt8& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt8& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DUInt8 mValue;
};


class UInt16Operand : public BaseOperand
{
public:
  explicit UInt16Operand (const DUInt16& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);
  virtual void SelfXor (const DInt64& value);
  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DUInt16 mValue;
};


class UInt32Operand : public BaseOperand
{
public:
  explicit UInt32Operand (const DUInt32& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DUInt32 mValue;
};


class UInt64Operand : public BaseOperand
{
public:
  explicit UInt64Operand (const DUInt64& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DUInt64 mValue;
};


class Int8Operand : public BaseOperand, public DInt8
{
public:
  explicit Int8Operand (const DInt8& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt8& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DInt8 mValue;
};


class Int16Operand : public BaseOperand, public DInt16
{
public:
  explicit Int16Operand (const DInt16& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DInt16 mValue;
};


class Int32Operand : public BaseOperand
{
public:
  explicit Int32Operand (const DInt32& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DInt32 mValue;
};


class Int64Operand : public BaseOperand
{
public:
  explicit Int64Operand (const DInt64& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DInt64 mValue;
};


class RealOperand : public BaseOperand
{
public:
  explicit RealOperand (const DReal& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DReal mValue;
};


class RichRealOperand : public BaseOperand
{
public:
  explicit RichRealOperand (const DRichReal& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DRichReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DRichReal mValue;
};


class TextOperand : public BaseOperand
{
public:
  explicit TextOperand (const DText& value)
    : BaseOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DText& value);

  virtual void SelfAdd (const DChar& value);
  virtual void SelfAdd (const DText& value);

  virtual uint_t GetType ();

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;

  virtual void NotifyCopy ();

private:
  DText mValue;
};


class CharTextElOperand : public BaseOperand
{
public:
  CharTextElOperand (DText &text, const uint64_t index)
    : BaseOperand (),
      mIndex (index),
      mText ()
  {
    text.MakeMirror (mText);
  }

  CharTextElOperand (const CharTextElOperand& source)
    : BaseOperand (),
      mIndex (source.mIndex),
      mText ()
  {
    source.mText.MakeMirror (mText);
  }

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

  virtual void NotifyCopy ();

private:
  const uint64_t mIndex;
  DText          mText;
};


class ArrayOperand : public BaseOperand
{
public:
  explicit ArrayOperand (const DArray& array)
    : BaseOperand (),
      mValue (array),
      mFirstArrayType (array.Type ())
    {
    }

  virtual bool IsNull () const;

  virtual void GetValue (DArray& outValue) const;

  virtual void SetValue (const DArray& value);

  virtual uint_t GetType ();

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;

  virtual void NotifyCopy ();

private:
  DArray    mValue;
  uint16_t  mFirstArrayType;
};


class BaseArrayElOperand : public BaseOperand
{
protected:
  BaseArrayElOperand (DArray& array, const uint64_t index)
    : BaseOperand (),
      mElementIndex (index),
      mArray  ()
  {
    array.MakeMirror (mArray);
  }

  BaseArrayElOperand (const BaseArrayElOperand& source)
    : BaseOperand (),
      mElementIndex (source.mElementIndex),
      mArray ()
  {
    source.mArray.MakeMirror (mArray);
  }


  template <typename DBS_T> void Get (DBS_T& out) const
  {
    mArray.Get (mElementIndex, out);

    assert (out.IsNull () == false);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    mArray.Set (mElementIndex, value);
  }

  virtual void NotifyCopy ();

private:
  BaseArrayElOperand& operator= (const BaseArrayElOperand& source);

  const uint64_t mElementIndex;
  DArray       mArray;
};


class BoolArrayElOperand : public BaseArrayElOperand
{
public:
  BoolArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;

  virtual void SetValue (const DBool& value);

  virtual void SelfAnd (const DBool& value);

  virtual void SelfXor (const DBool& value);

  virtual void SelfOr (const DBool& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class CharArrayElOperand : public BaseArrayElOperand
{
public:
  CharArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class DateArrayElOperand : public BaseArrayElOperand
{
public:
  DateArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDate& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class DateTimeArrayElOperand : public BaseArrayElOperand
{
public:
  DateTimeArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDateTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class HiresTimeArrayElOperand : public BaseArrayElOperand
{
public:
  HiresTimeArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DHiresTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt8ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt8ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt8& value);


  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt16ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt16ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt32ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt32ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt64ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt64ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int8ArrayElOperand : public BaseArrayElOperand
{
public:
  Int8ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt8& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};

class Int16ArrayElOperand : public BaseArrayElOperand
{
public:
  Int16ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int32ArrayElOperand : public BaseArrayElOperand
{
public:
  Int32ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int64ArrayElOperand : public BaseArrayElOperand
{
public:
  Int64ArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class RealArrayElOperand : public BaseArrayElOperand
{
public:
  RealArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class RichRealArrayElOperand : public BaseArrayElOperand
{
public:
  RichRealArrayElOperand (DArray& array, const uint64_t index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DRichReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class TableOperand : public BaseOperand
{
public:
  TableOperand (IDBSHandler& dbsHnd, ITable& table)
    : BaseOperand (),
      mTableRef (new TableReference (dbsHnd, table))
  {
    mTableRef->IncrementRefCount ();
  }

  TableOperand (const TableOperand& source)
    : BaseOperand (),
      mTableRef (source.mTableRef)
  {
    mTableRef->IncrementRefCount ();
  }

  virtual ~TableOperand ();

  const TableOperand& operator= (const TableOperand& pSource)
  {
    if (this != &pSource)
      {
        mTableRef->DecrementRefCount ();
        mTableRef = pSource.mTableRef;
        mTableRef->IncrementRefCount ();
      }
    return *this;
  }

  virtual bool IsNull () const;

  virtual uint_t GetType ();

  virtual StackValue GetFieldAt (const FIELD_INDEX field);

  virtual ITable& GetTable ();

  TableReference& GetTableRef () const
  {
    assert (mTableRef != NULL);

    return *mTableRef;
  }

  virtual StackValue Duplicate () const;

  virtual void NotifyCopy ();

  virtual TableOperand GetTableOp ();

  virtual void CopyTableOp (const TableOperand& source);

private:
  TableReference* mTableRef;
};


class FieldOperand : public BaseOperand
{
public:
  FieldOperand (const uint32_t fieldType = T_UNDETERMINED)
    : mTableRef (NULL),
      mField (~0),
      mFieldType (fieldType)
  {
    assert (GET_BASIC_TYPE (fieldType) <= T_UNDETERMINED);
    assert (GET_BASIC_TYPE (fieldType) > T_UNKNOWN);
  }

  FieldOperand (TableOperand& tableOp, const FIELD_INDEX field);
  FieldOperand (const FieldOperand& source);
  virtual ~FieldOperand ();

  const FieldOperand& operator= (const FieldOperand& source);

  virtual bool IsNull () const;

  virtual uint_t GetType ();

  virtual FIELD_INDEX GetField ();

  virtual ITable& GetTable ();

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
  virtual void NotifyCopy ();

  virtual FieldOperand GetFieldOp ();

  virtual void CopyFieldOp (const FieldOperand& source);

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
  BaseFieldElOperand (TableReference*   tableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : mRow (row),
      mTableRef (tableRef),
      mField (field)
  {
    mTableRef->IncrementRefCount ();
  }

  BaseFieldElOperand (const BaseFieldElOperand& source)
    : BaseOperand (),
      mRow (source.mRow),
      mTableRef (source.mTableRef),
      mField (source.mField)
  {
    mTableRef->IncrementRefCount ();
  }

  virtual ~BaseFieldElOperand ();

  template <typename DBS_T> void Get (DBS_T& out) const
  {
    ITable& table = mTableRef->GetTable ();

    table.Get (mRow, mField, out);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    ITable& table = mTableRef->GetTable ();

    table.Set (mRow, mField, value);
  }

  virtual void NotifyCopy ();

private:
  BaseFieldElOperand& operator= (const BaseFieldElOperand* source);

  const ROW_INDEX       mRow;
  TableReference*       mTableRef;
  const FIELD_INDEX     mField;
};


class BoolFieldElOperand : public BaseFieldElOperand
{
public:
  BoolFieldElOperand (TableReference*   tableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;

  virtual void SetValue (const DBool& value);

  virtual void SelfAnd (const DBool& value);

  virtual void SelfXor (const DBool& value);

  virtual void SelfOr (const DBool& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class CharFieldElOperand : public BaseFieldElOperand
{
public:
  CharFieldElOperand (TableReference*   tableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class DateFieldElOperand : public BaseFieldElOperand
{
public:
  DateFieldElOperand (TableReference*   tableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDate& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class DateTimeFieldElOperand : public BaseFieldElOperand
{
public:

  DateTimeFieldElOperand (TableReference*   tableRef,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDateTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class HiresTimeFieldElOperand : public BaseFieldElOperand
{
public:
  HiresTimeFieldElOperand (TableReference*   tableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DHiresTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt8FieldElOperand : public BaseFieldElOperand
{
public:
  UInt8FieldElOperand (TableReference*   tableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt8& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt16FieldElOperand : public BaseFieldElOperand
{
public:
  UInt16FieldElOperand (TableReference*   tableRef,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt32FieldElOperand : public BaseFieldElOperand
{
public:
  UInt32FieldElOperand (TableReference*   tableRef,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt64FieldElOperand : public BaseFieldElOperand
{
public:
  UInt64FieldElOperand (TableReference*   tableRef,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int8FieldElOperand : public BaseFieldElOperand
{
public:
  Int8FieldElOperand (TableReference*   tableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt8& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int16FieldElOperand : public BaseFieldElOperand
{
public:
  Int16FieldElOperand (TableReference*   tableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int32FieldElOperand : public BaseFieldElOperand
{
public:
  Int32FieldElOperand (TableReference*   tableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int64FieldElOperand : public BaseFieldElOperand
{
public:
  Int64FieldElOperand (TableReference*   tableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class RealFieldElOperand : public BaseFieldElOperand
{
public:
  RealFieldElOperand (TableReference*   tableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class RichRealFieldElOperand : public BaseFieldElOperand
{
public:
  RichRealFieldElOperand (TableReference*   tableRef,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DRichReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class TextFieldElOperand : public BaseFieldElOperand
{
public:
  TextFieldElOperand (TableReference*   tableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DText& outValue) const;
  virtual void SetValue (const DText& value);

  virtual void SelfAdd (const DChar& value);
  virtual void SelfAdd (const DText& value);

  virtual uint_t GetType ();

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
};


class ArrayFieldElOperand : public BaseFieldElOperand
{
public:
  ArrayFieldElOperand (TableReference*   tableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (tableRef, row, field)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DArray& outValue) const;
  virtual void SetValue (const DArray& value);

  virtual uint_t GetType ();

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
};


class CharTextFieldElOperand : public BaseFieldElOperand
{
public:
  CharTextFieldElOperand (TableReference*   tableRef,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const uint64_t    index)
    : BaseFieldElOperand (tableRef, row, field),
      mIndex (index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  const uint64_t    mIndex;
};


class BaseArrayFieldElOperand : public BaseOperand
{
protected:
  BaseArrayFieldElOperand (TableReference*   tableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
     : mIndex (index),
       mRow (row),
       mTableRef (tableRef),
       mField (field)
  {
    mTableRef->IncrementRefCount ();
  }

  BaseArrayFieldElOperand (const BaseArrayFieldElOperand& source)
     : mIndex (source.mIndex),
       mRow (source.mRow),
       mTableRef (source.mTableRef),
       mField (source.mField)
  {
    mTableRef->IncrementRefCount ();
  }

  virtual ~BaseArrayFieldElOperand ();

  template <typename DBS_T> void Get (DBS_T& outValue) const
  {
    ITable& table = mTableRef->GetTable ();

    DArray array;
    table.Get (mRow, mField, array);

    array.Get (mIndex, outValue);
    assert (outValue.IsNull () == false);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    ITable& table = mTableRef->GetTable ();

    DArray array;

    if (mRow < table.AllocatedRows () )
      table.Get (mRow, mField, array);

    array.Set (mIndex, value);
    table.Set (mRow, mField, array);
  }

  virtual void NotifyCopy ();

private:
  BaseArrayFieldElOperand& operator= (const BaseArrayFieldElOperand&);

  const uint64_t    mIndex;
  const ROW_INDEX   mRow;
  TableReference*   mTableRef;
  const FIELD_INDEX mField;
};


class BoolArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  BoolArrayFieldElOperand (TableReference*   tableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;

  virtual void SetValue (const DBool& value);

  virtual void SelfAnd (const DBool& value);

  virtual void SelfXor (const DBool& value);

  virtual void SelfOr (const DBool& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class CharArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  CharArrayFieldElOperand (TableReference*   tableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class DateArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  DateArrayFieldElOperand (TableReference*   tableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDate& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};

class DateTimeArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  DateTimeArrayFieldElOperand (TableReference*   tableRef,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DDateTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};

class HiresTimeArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  HiresTimeArrayFieldElOperand (TableReference*   tableRef,
                                const ROW_INDEX   row,
                                const FIELD_INDEX field,
                                const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;

  virtual void SetValue (const DHiresTime& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};

class UInt8ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt8ArrayFieldElOperand (TableReference*   tableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt8& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt16ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt16ArrayFieldElOperand (TableReference*   tableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);
  virtual void SelfXor (const DInt64& value);
  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt32ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt32ArrayFieldElOperand (TableReference*   tableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class UInt64ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt64ArrayFieldElOperand (TableReference*   tableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DUInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};

class Int8ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int8ArrayFieldElOperand (TableReference*   tableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt8& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int16ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int16ArrayFieldElOperand (TableReference*   tableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt16& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int32ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int32ArrayFieldElOperand (TableReference*   tableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt32& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class Int64ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int64ArrayFieldElOperand (TableReference*   tableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;

  virtual void SetValue (const DInt64& value);

  virtual void SelfAdd (const DInt64& value);

  virtual void SelfSub (const DInt64& value);

  virtual void SelfMul (const DInt64& value);

  virtual void SelfDiv (const DInt64& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);

  virtual void SelfXor (const DInt64& value);

  virtual void SelfOr (const DInt64& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class RealArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  RealArrayFieldElOperand (TableReference*   tableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class RichRealArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  RichRealArrayFieldElOperand (TableReference*   tableRef,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const uint64_t    index)
    : BaseArrayFieldElOperand (tableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;

  virtual void SetValue (const DRichReal& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
};


class GlobalValue
{
public:
  template <class OP_T>
  explicit GlobalValue (const OP_T& op)
    : mSync (),
      mOperandOwner (true)
  {
    const BaseOperand& compileTest = op;
    (void)compileTest; //Just to make sure OP_T is a valid type!

    assert (sizeof (OP_T) <= sizeof (mStorage));
    _placement_new (mStorage, op);
  }

  GlobalValue (const GlobalValue& source)
    : mSync (),
      mOperandOwner (source.mOperandOwner)
  {
    for (uint_t i = 0; i < sizeof (mStorage) / sizeof (mStorage[0]); ++i)
      mStorage[i] = source.mStorage[i];

    _CC (bool&, source.mOperandOwner) = false;
  }

  const GlobalValue& operator= (const GlobalValue& source)
  {
    if (this == &source)
      return *this;

    mOperandOwner = source.mOperandOwner;
    for (uint_t i = 0; i < sizeof (mStorage) / sizeof (mStorage[0]); ++i)
      mStorage[i] = source.mStorage[i];

    _CC (bool&, source.mOperandOwner) = false;
    return *this;
  }

  ~GlobalValue ()
  {
    if (mOperandOwner)
      Operand ().~BaseOperand ();
  }

  bool IsNull ()
  {
    LockRAII dummy (mSync);

    return Operand ().IsNull ();
  }

  template <class DBS_T>
  void GetValue (DBS_T& outValue)
  {
    LockRAII dummy (mSync);

    Operand ().GetValue (outValue);
  }

  template <class DBS_T>
  void SetValue (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SetValue (value);
  }

  template <class DBS_T>
  void SelfAdd (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfSub (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfSub (value);
  }

  template <class DBS_T>
  void SelfMul (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfDiv (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfMod (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfAnd (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfAnd (value);
  }

  template <class DBS_T>
  void SelfXor (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfXor (value);
  }

  template <class DBS_T>
  void SelfOr (const DBS_T& value)
  {
    LockRAII dummy(mSync);

    Operand ().SelfOr (value);
  }

  uint_t GetType ()
  {
    return Operand ().GetType ();
  }


  FIELD_INDEX GetField ()
  {
    LockRAII dummy(mSync);

    return Operand ().GetField ();
  }

  ITable& GetTable ()
  {
    LockRAII dummy(mSync);

    return Operand ().GetTable ();
  }

  StackValue GetFieldAt (const FIELD_INDEX field)
  {
    LockRAII dummy(mSync);
    return Operand ().GetFieldAt (field);

  }

  StackValue GetValueAt (const uint64_t index)
  {
    LockRAII dummy(mSync);

    return Operand ().GetValueAt (index);
  }

  StackValue Duplicate ()
  {
    LockRAII dummy(mSync);

    return Operand ().Duplicate ();
  }

  void NotifyCopy ()
  {
    LockRAII dummy(mSync);
    Operand ().NotifyCopy ();
  }

  TableOperand GetTableOp ()
  {
    LockRAII dummy(mSync);

    return Operand ().GetTableOp ();
  }

  void CopyTableOp (const TableOperand& source)
  {
    LockRAII dummy(mSync);

    return Operand ().CopyTableOp (source);
  }

  FieldOperand  GetFieldOp ()
  {
    LockRAII dummy(mSync);

    return Operand ().GetFieldOp ();
  }

  void CopyFieldOp (const FieldOperand& source)
  {
    LockRAII dummy(mSync);

    return Operand ().CopyFieldOp (source);
  }

  BaseOperand& Operand ()
  {
    return *_RC (BaseOperand*, _RC (void*, mStorage));
  }

private:
  Lock          mSync;
  bool          mOperandOwner;

  uint64_t      mStorage[MAX_OP_QWORDS];
};


class GlobalOperand : public BaseOperand
{
public:
  GlobalOperand (GlobalValue& global);

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;
  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;
  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;
  virtual void GetValue (DText& outValue) const;
  virtual void GetValue (DArray& outValue) const;

  virtual void SetValue (const DBool& value);
  virtual void SetValue (const DChar& value);
  virtual void SetValue (const DDate& value);
  virtual void SetValue (const DDateTime& value);
  virtual void SetValue (const DHiresTime& value);
  virtual void SetValue (const DInt8& value);
  virtual void SetValue (const DInt16& value);
  virtual void SetValue (const DInt32& value);
  virtual void SetValue (const DInt64& value);
  virtual void SetValue (const DReal& value);
  virtual void SetValue (const DRichReal& value);
  virtual void SetValue (const DUInt8& value);
  virtual void SetValue (const DUInt16& value);
  virtual void SetValue (const DUInt32& value);
  virtual void SetValue (const DUInt64& value);
  virtual void SetValue (const DText& value);
  virtual void SetValue (const DArray& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);
  virtual void SelfAdd (const DChar& value);
  virtual void SelfAdd (const DText& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);
  virtual void SelfAnd (const DBool& value);

  virtual void SelfXor (const DInt64& value);
  virtual void SelfXor (const DBool& value);

  virtual void SelfOr (const DInt64& value);
  virtual void SelfOr (const DBool& value);

  virtual uint_t GetType ();

  virtual FIELD_INDEX GetField ();

  virtual ITable& GetTable ();

  virtual StackValue GetFieldAt (const FIELD_INDEX field);

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
  virtual void NotifyCopy ();

  virtual TableOperand GetTableOp ();

  virtual void CopyTableOp (const TableOperand& source);

  virtual FieldOperand GetFieldOp ();

  virtual void CopyFieldOp (const FieldOperand& source);

private:
  GlobalValue&    mValue;
};


class LocalOperand : public BaseOperand
{
public:
  LocalOperand (SessionStack& stack, const uint64_t index);

  virtual bool IsNull () const;

  virtual void GetValue (DBool& outValue) const;
  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DDate& outValue) const;
  virtual void GetValue (DDateTime& outValue) const;
  virtual void GetValue (DHiresTime& outValue) const;
  virtual void GetValue (DInt8& outValue) const;
  virtual void GetValue (DInt16& outValue) const;
  virtual void GetValue (DInt32& outValue) const;
  virtual void GetValue (DInt64& outValue) const;
  virtual void GetValue (DReal& outValue) const;
  virtual void GetValue (DRichReal& outValue) const;
  virtual void GetValue (DUInt8& outValue) const;
  virtual void GetValue (DUInt16& outValue) const;
  virtual void GetValue (DUInt32& outValue) const;
  virtual void GetValue (DUInt64& outValue) const;
  virtual void GetValue (DText& outValue) const;
  virtual void GetValue (DArray& outValue) const;

  virtual void SetValue (const DBool& value);
  virtual void SetValue (const DChar& value);
  virtual void SetValue (const DDate& value);
  virtual void SetValue (const DDateTime& value);
  virtual void SetValue (const DHiresTime& value);
  virtual void SetValue (const DInt8& value);
  virtual void SetValue (const DInt16& value);
  virtual void SetValue (const DInt32& value);
  virtual void SetValue (const DInt64& value);
  virtual void SetValue (const DReal& value);
  virtual void SetValue (const DRichReal& value);
  virtual void SetValue (const DUInt8& value);
  virtual void SetValue (const DUInt16& value);
  virtual void SetValue (const DUInt32& value);
  virtual void SetValue (const DUInt64& value);
  virtual void SetValue (const DText& value);
  virtual void SetValue (const DArray& value);

  virtual void SelfAdd (const DInt64& value);
  virtual void SelfAdd (const DRichReal& value);
  virtual void SelfAdd (const DChar& value);
  virtual void SelfAdd (const DText& value);

  virtual void SelfSub (const DInt64& value);
  virtual void SelfSub (const DRichReal& value);

  virtual void SelfMul (const DInt64& value);
  virtual void SelfMul (const DRichReal& value);

  virtual void SelfDiv (const DInt64& value);
  virtual void SelfDiv (const DRichReal& value);

  virtual void SelfMod (const DInt64& value);

  virtual void SelfAnd (const DInt64& value);
  virtual void SelfAnd (const DBool& value);

  virtual void SelfXor (const DInt64& value);
  virtual void SelfXor (const DBool& value);

  virtual void SelfOr (const DInt64& value);
  virtual void SelfOr (const DBool& value);

  virtual uint_t GetType ();

  virtual FIELD_INDEX GetField ();

  virtual ITable& GetTable ();

  virtual StackValue GetFieldAt (const FIELD_INDEX field);

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;

  virtual TableOperand GetTableOp ();

  virtual void CopyTableOp (const TableOperand& source);

  virtual FieldOperand GetFieldOp ();

  virtual void CopyFieldOp (const FieldOperand& source);

private:
  const uint64_t      mIndex;
  SessionStack&       mStack;
};


} //namespace prima
} //namespace whisper

#endif /* PM_OPERAND_H_ */

