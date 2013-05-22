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

class I_PMOperand : public I_Operand
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

  //Special treatment for these
  virtual FIELD_INDEX   GetField ();
  virtual ITable&   GetTable ();
  virtual StackValue    GetFieldAt (const FIELD_INDEX field);
  virtual StackValue    GetValueAt (const uint64_t index);

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);

  virtual void          NotifyCopy ();
};

class NullOperand : public I_PMOperand
{
public:
  NullOperand ()
    : I_PMOperand ()
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

class BoolOperand : public I_PMOperand
{
public:
  explicit BoolOperand (const DBool& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~BoolOperand ();

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

class CharOperand : public I_PMOperand
{
public:
  explicit CharOperand (const DChar& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~CharOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;

private:
  DChar mValue;
};

class DateOperand : public I_PMOperand
{
public:
  explicit DateOperand (const DDate& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~DateOperand ();

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

class DateTimeOperand : public I_PMOperand
{
public:
  explicit DateTimeOperand (const DDateTime& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~DateTimeOperand ();

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

class HiresTimeOperand : public I_PMOperand
{
public:
  explicit HiresTimeOperand (const DHiresTime& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~HiresTimeOperand ();

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

class UInt8Operand : public I_PMOperand
{
public:
  explicit UInt8Operand (const DUInt8& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~UInt8Operand ();

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

class UInt16Operand : public I_PMOperand
{
public:
  explicit UInt16Operand (const DUInt16& value)
    : I_PMOperand (),
      mValue (value)
  {
  }

  virtual ~UInt16Operand ();

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

class UInt32Operand : public I_PMOperand
{
public:
  explicit UInt32Operand (const DUInt32& value)
    : I_PMOperand (),
      mValue (value)
  {
  }

  virtual bool IsNull () const;

  virtual ~UInt32Operand ();

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

class UInt64Operand : public I_PMOperand
{
public:
  explicit UInt64Operand (const DUInt64& value)
    : I_PMOperand (),
      mValue (value)
  {
  }

  virtual ~UInt64Operand ();

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

class Int8Operand : public I_PMOperand, public DInt8
{
public:
  explicit Int8Operand (const DInt8& value)
    : I_PMOperand (),
      mValue (value)
  {
  }

  virtual ~Int8Operand ();

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

class Int16Operand : public I_PMOperand, public DInt16
{
public:
  explicit Int16Operand (const DInt16& value)
    : I_PMOperand (),
      mValue (value)
  {
  }

  virtual ~Int16Operand ();

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

class Int32Operand : public I_PMOperand
{
public:
  explicit Int32Operand (const DInt32& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~Int32Operand ();

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

class Int64Operand : public I_PMOperand
{
public:
  explicit Int64Operand (const DInt64& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual ~Int64Operand ();

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


class RealOperand : public I_PMOperand
{
public:
  explicit RealOperand (const DReal& value)
    : I_PMOperand (),
      mValue (value)
  {
  }

  virtual ~RealOperand ();

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

class RichRealOperand : public I_PMOperand
{
public:
  explicit RichRealOperand (const DRichReal& value)
    : I_PMOperand (),
      mValue (value)
  {
  }

  virtual ~RichRealOperand ();

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

class TextOperand : public I_PMOperand
{
public:
  explicit TextOperand (const DText& value)
    : I_PMOperand (),
      mValue (value)
  {
  }
  virtual bool IsNull () const;

  virtual ~TextOperand ();

  virtual void GetValue (DText& outValue) const;
  virtual void SetValue (const DText& value);

  virtual void SelfAdd (const DChar& value);
  virtual void SelfAdd (const DText& value);

  virtual uint_t GetType ();

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
  virtual void       NotifyCopy ();

private:
  DText mValue;
};

class CharTextElOperand : public I_PMOperand
{
public:
  CharTextElOperand (DText &text, const uint64_t index)
    : I_PMOperand (),
      mIndex (index),
      mText ()
  {
    text.MakeMirror (mText);
  }

  CharTextElOperand (const CharTextElOperand& source)
    : I_PMOperand (),
      mIndex (source.mIndex),
      mText ()
  {
    source.mText.MakeMirror (mText);
  }


  virtual ~CharTextElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DChar& outValue) const;
  virtual void GetValue (DText& outValue) const;

  virtual void SetValue (const DChar& value);

  virtual uint_t GetType ();

  virtual StackValue Duplicate () const;
  virtual void       NotifyCopy ();

private:
  const uint64_t mIndex;
  DText        mText;
};

class ArrayOperand : public I_PMOperand
{
public:
  explicit ArrayOperand (const DArray& array)
    : I_PMOperand (),
      mValue (array),
      mFirstArrayType (array.Type ())
    {
    }
  virtual ~ArrayOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DArray& outValue) const;
  virtual void SetValue (const DArray& value);

  virtual uint_t GetType ();

  virtual StackValue GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
  virtual void       NotifyCopy ();

private:
  DArray mValue;
  uint16_t mFirstArrayType;
};

class BaseArrayElOperand : public I_PMOperand
{
protected:
  BaseArrayElOperand (DArray& array, const uint64_t index)
    : I_PMOperand (),
      mElementIndex (index),
      mArray  ()
  {
    array.MakeMirror (mArray);
  }

  BaseArrayElOperand (const BaseArrayElOperand& source)
    : I_PMOperand (),
      mElementIndex (source.mElementIndex),
      mArray ()
  {
    source.mArray.MakeMirror (mArray);
  }

  virtual ~BaseArrayElOperand ();

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

class TableOperand : public I_PMOperand
{
public:
  TableOperand (I_DBSHandler& dbsHnd, ITable& table)
    : I_PMOperand (),
      mpRefTable (new TableReference (dbsHnd, table))
  {
    mpRefTable->IncrementRefCount ();
  }

  TableOperand (const TableOperand& source)
    : I_PMOperand (),
      mpRefTable (source.mpRefTable)
  {
    mpRefTable->IncrementRefCount ();
  }

  virtual ~TableOperand ();

  const TableOperand& operator= (const TableOperand& pSource)
  {
    if (this != &pSource)
      {
        mpRefTable->DecrementRefCount ();
        mpRefTable = pSource.mpRefTable;
        mpRefTable->IncrementRefCount ();
      }
    return *this;
  }

  virtual bool IsNull () const;

  virtual uint_t GetType ();

  virtual StackValue  GetFieldAt (const FIELD_INDEX field);
  virtual ITable& GetTable ();

  TableReference& GetTableRef () const
  {
    assert (mpRefTable != NULL);

    return *mpRefTable;
  }

  virtual StackValue Duplicate () const;
  virtual void       NotifyCopy ();

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

private:
  TableReference* mpRefTable;
};

class FieldOperand : public I_PMOperand
{
public:
  FieldOperand (const uint32_t fieldType = T_UNDETERMINED)
    : mpRefTable (NULL),
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
  virtual StackValue  GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
  virtual void       NotifyCopy ();

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);

private:
  TableReference* mpRefTable;
  FIELD_INDEX     mField;
  uint32_t        mFieldType;
};

class BaseFieldElOperand : public I_PMOperand
{
protected:
  BaseFieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : mRow (row),
      mpRefTable (pTableRef),
      mField (field)
  {
    mpRefTable->IncrementRefCount ();
  }

  BaseFieldElOperand (const BaseFieldElOperand& source)
    : I_PMOperand (),
      mRow (source.mRow),
      mpRefTable (source.mpRefTable),
      mField (source.mField)
  {
    mpRefTable->IncrementRefCount ();
  }

  virtual ~BaseFieldElOperand ();

  template <typename DBS_T> void Get (DBS_T& out) const
  {
    ITable& table = mpRefTable->GetTable ();

    table.Get (mRow, mField, out);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    ITable& table = mpRefTable->GetTable ();

    table.Set (mRow, mField, value);
  }

  virtual void NotifyCopy ();

private:
  friend class CharTextFieldElOperand;
  friend class TextFieldElOperand;
  friend class ArrayFieldElOperand;

  BaseFieldElOperand& operator= (const BaseFieldElOperand* source);

  const ROW_INDEX   mRow;
  TableReference*   mpRefTable;
  const FIELD_INDEX mField;
};

class BoolFieldElOperand : public BaseFieldElOperand
{
public:
  BoolFieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  CharFieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  DateFieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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

  DateTimeFieldElOperand (TableReference*   pTableRef,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  HiresTimeFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  UInt8FieldElOperand (TableReference*   pTableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  UInt16FieldElOperand (TableReference*   pTableRef,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  UInt32FieldElOperand (TableReference*   pTableRef,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  UInt64FieldElOperand (TableReference*   pTableRef,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  Int8FieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  Int16FieldElOperand (TableReference*   pTableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  Int32FieldElOperand (TableReference*   pTableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  Int64FieldElOperand (TableReference*   pTableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  RealFieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  RichRealFieldElOperand (TableReference*   pTableRef,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  TextFieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  ArrayFieldElOperand (TableReference*   pTableRef,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : BaseFieldElOperand (pTableRef, row, field)
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
  CharTextFieldElOperand (TableReference*   pTableRef,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const uint64_t    index)
    : BaseFieldElOperand (pTableRef, row, field),
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

class BaseArrayFieldElOperand : public I_PMOperand
{
protected:
  BaseArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
     : mIndex (index),
       mRow (row),
       mpRefTable (pTableRef),
       mField (field)
  {
    mpRefTable->IncrementRefCount ();
  }

  BaseArrayFieldElOperand (const BaseArrayFieldElOperand& source)
     : mIndex (source.mIndex),
       mRow (source.mRow),
       mpRefTable (source.mpRefTable),
       mField (source.mField)
  {
    mpRefTable->IncrementRefCount ();
  }

  virtual ~BaseArrayFieldElOperand ();

  template <typename DBS_T> void Get (DBS_T& outValue) const
  {
    ITable& table = mpRefTable->GetTable ();

    DArray array;
    table.Get (mRow, mField, array);

    array.Get (mIndex, outValue);
    assert (outValue.IsNull () == false);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    ITable& table = mpRefTable->GetTable ();

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
  TableReference*   mpRefTable;
  const FIELD_INDEX mField;
};

class BoolArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  BoolArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  CharArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  DateArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  DateTimeArrayFieldElOperand (TableReference*   pTableRef,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  HiresTimeArrayFieldElOperand (TableReference*   pTableRef,
                                const ROW_INDEX   row,
                                const FIELD_INDEX field,
                                const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  UInt8ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  UInt16ArrayFieldElOperand (TableReference*   pTableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  UInt32ArrayFieldElOperand (TableReference*   pTableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  UInt64ArrayFieldElOperand (TableReference*   pTableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  Int8ArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  Int16ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  Int32ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  Int64ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  RealArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
  RichRealArrayFieldElOperand (TableReference*   pTableRef,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const uint64_t    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
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
    const I_PMOperand& compileTest = op;
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
      GetOperand ().~I_PMOperand ();
  }

  bool IsNull ()
  {
    LockRAII dummy (mSync);
    return GetOperand ().IsNull ();
  }

  template <class DBS_T>
  void GetValue (DBS_T& outValue)
  {
    LockRAII dummy (mSync);
    GetOperand ().GetValue (outValue);
  }

  template <class DBS_T>
  void SetValue (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SetValue (value);
  }

  template <class DBS_T>
  void SelfAdd (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfSub (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfSub (value);
  }

  template <class DBS_T>
  void SelfMul (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfDiv (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfMod (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfAnd (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfAnd (value);
  }

  template <class DBS_T>
  void SelfXor (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfXor (value);
  }

  template <class DBS_T>
  void SelfOr (const DBS_T& value)
  {
    LockRAII dummy(mSync);
    GetOperand ().SelfOr (value);
  }

  uint_t GetType ()
  {
    return GetOperand ().GetType ();
  }


  FIELD_INDEX GetField ()
  {
    LockRAII dummy(mSync);
    return GetOperand ().GetField ();
  }

  ITable& GetTable ()
  {
    LockRAII dummy(mSync);
    return GetOperand ().GetTable ();
  }

  StackValue GetFieldAt (const FIELD_INDEX field)
  {
    LockRAII dummy(mSync);
    return GetOperand ().GetFieldAt (field);
  }

  StackValue GetValueAt (const uint64_t index)
  {
    LockRAII dummy(mSync);
    return GetOperand ().GetValueAt (index);
  }

  StackValue Duplicate ()
  {
    LockRAII dummy(mSync);
    return GetOperand ().Duplicate ();
  }

  void NotifyCopy ()
  {
    LockRAII dummy(mSync);
    GetOperand ().NotifyCopy ();
  }

  TableOperand GetTableOp ()
  {
    LockRAII dummy(mSync);
    return GetOperand ().GetTableOp ();
  }

  void CopyTableOp (const TableOperand& source)
  {
    LockRAII dummy(mSync);
    return GetOperand ().CopyTableOp (source);
  }

  FieldOperand  GetFieldOp ()
  {
    LockRAII dummy(mSync);
    return GetOperand ().GetFieldOp ();
  }

  void CopyFieldOp (const FieldOperand& source)
  {
    LockRAII dummy(mSync);
    return GetOperand ().CopyFieldOp (source);
  }

  I_PMOperand& GetOperand () { return *_RC (I_PMOperand*, mStorage);  }

private:
  Lock mSync;
  bool          mOperandOwner;
  uint64_t      mStorage[MAX_OP_QWORDS];
};

class GlobalOperand : public I_PMOperand
{
public:
  GlobalOperand (GlobalValue& global);
  ~GlobalOperand ();

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
  virtual StackValue  GetFieldAt (const FIELD_INDEX field);
  virtual StackValue  GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;
  virtual void       NotifyCopy ();

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);

private:

  GlobalValue& mValue;
};

class LocalOperand : public I_PMOperand
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
  virtual StackValue  GetFieldAt (const FIELD_INDEX field);
  virtual StackValue  GetValueAt (const uint64_t index);

  virtual StackValue Duplicate () const;

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);

private:
  const uint64_t mIndex;
  SessionStack&  mStack;
};

} //namespace prima
} //namespace whisper

#endif /* PM_OPERAND_H_ */

