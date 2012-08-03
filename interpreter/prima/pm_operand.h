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

#include "utils/include/wthread.h"

#include "interpreter.h"
#include "operands.h"

#include "pm_table.h"

namespace prima
{

template <typename T_DEST, typename T_SRC> T_DEST
internal_add (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value + secondOp.m_Value);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_sub (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value - secondOp.m_Value);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_mul (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value * secondOp.m_Value);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_div (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value / secondOp.m_Value);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_mod (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value / secondOp.m_Value);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_and (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value & secondOp.m_Value);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_xor (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value ^ secondOp.m_Value);
}

template <typename T_DEST, typename T_SRC> T_DEST
internal_or (const T_DEST& firstOp, const T_SRC& secondOp)
{
  if (firstOp.IsNull () || secondOp.IsNull ())
    return firstOp;

  return T_DEST (firstOp.m_Value ^ secondOp.m_Value);
}

template <typename T_SRC, typename T_DEST> void
number_convert (const T_SRC& from, T_DEST& to)
{
  to = from.IsNull () ? T_DEST () : T_DEST (from.m_Value);
}


class TableOperand;
class FieldOperand;

class I_PMOperand : public I_Operand
{
public:

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;
  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;
  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;
  virtual void GetValue (DBSText& outValue) const;
  virtual void GetValue (DBSArray& outValue) const;

  virtual void SetValue (const DBSBool& value);
  virtual void SetValue (const DBSChar& value);
  virtual void SetValue (const DBSDate& value);
  virtual void SetValue (const DBSDateTime& value);
  virtual void SetValue (const DBSHiresTime& value);
  virtual void SetValue (const DBSInt8& value);
  virtual void SetValue (const DBSInt16& value);
  virtual void SetValue (const DBSInt32& value);
  virtual void SetValue (const DBSInt64& value);
  virtual void SetValue (const DBSReal& value);
  virtual void SetValue (const DBSRichReal& value);
  virtual void SetValue (const DBSUInt8& value);
  virtual void SetValue (const DBSUInt16& value);
  virtual void SetValue (const DBSUInt32& value);
  virtual void SetValue (const DBSUInt64& value);
  virtual void SetValue (const DBSText& value);
  virtual void SetValue (const DBSArray& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);
  virtual void SelfAdd (const DBSChar& value);
  virtual void SelfAdd (const DBSText& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfAnd (const DBSBool& value);

  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfXor (const DBSBool& value);

  virtual void SelfOr (const DBSInt64& value);
  virtual void SelfOr (const DBSBool& value);

  //Special treatment for these
  virtual FIELD_INDEX   GetField ();
  virtual I_DBSTable&   GetTable ();
  virtual StackValue    GetValueAt (const D_UINT64 index);

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);


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

  virtual void GetValue (DBSBool& outValue) const;
  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;
  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;
  virtual void GetValue (DBSText& outValue) const;
  virtual void GetValue (DBSArray& outValue) const;

  virtual StackValue CopyValue () const;
};

class BoolOperand : public I_PMOperand
{
public:
  explicit BoolOperand (const DBSBool& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~BoolOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

  virtual void SelfAnd (const DBSBool& value);
  virtual void SelfXor (const DBSBool& value);
  virtual void SelfOr (const DBSBool& value);

  virtual StackValue CopyValue () const;

private:
  DBSBool m_Value;
};

class CharOperand : public I_PMOperand
{
public:
  explicit CharOperand (const DBSChar& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~CharOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

  virtual StackValue CopyValue () const;

private:
  DBSChar m_Value;
};

class DateOperand : public I_PMOperand
{
public:
  explicit DateOperand (const DBSDate& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~DateOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

  virtual StackValue CopyValue () const;

private:
  DBSDate m_Value;
};

class DateTimeOperand : public I_PMOperand
{
public:
  explicit DateTimeOperand (const DBSDateTime& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~DateTimeOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

  virtual StackValue CopyValue () const;

private:
  DBSDateTime m_Value;
};

class HiresTimeOperand : public I_PMOperand
{
public:
  explicit HiresTimeOperand (const DBSHiresTime& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~HiresTimeOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

  virtual StackValue CopyValue () const;

private:
  DBSHiresTime m_Value;
};

class UInt8Operand : public I_PMOperand
{
public:
  explicit UInt8Operand (const DBSUInt8& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~UInt8Operand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt8& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSUInt8 m_Value;
};

class UInt16Operand : public I_PMOperand
{
public:
  explicit UInt16Operand (const DBSUInt16& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }

  virtual ~UInt16Operand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSUInt16 m_Value;
};

class UInt32Operand : public I_PMOperand
{
public:
  explicit UInt32Operand (const DBSUInt32& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }

  virtual bool IsNull () const;

  virtual ~UInt32Operand ();

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSUInt32 m_Value;
};

class UInt64Operand : public I_PMOperand
{
public:
  explicit UInt64Operand (const DBSUInt64& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }

  virtual ~UInt64Operand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSUInt64 m_Value;
};

class Int8Operand : public I_PMOperand, public DBSInt8
{
public:
  explicit Int8Operand (const DBSInt8& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }

  virtual ~Int8Operand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt8& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSInt8 m_Value;
};

class Int16Operand : public I_PMOperand, public DBSInt16
{
public:
  explicit Int16Operand (const DBSInt16& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }

  virtual ~Int16Operand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSInt16 m_Value;
};

class Int32Operand : public I_PMOperand
{
public:
  explicit Int32Operand (const DBSInt32& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~Int32Operand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSInt32 m_Value;
};

class Int64Operand : public I_PMOperand
{
public:
  explicit Int64Operand (const DBSInt64& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual ~Int64Operand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;

private:
  DBSInt64 m_Value;
};


class RealOperand : public I_PMOperand
{
public:
  explicit RealOperand (const DBSReal& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }

  virtual ~RealOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;

private:
  DBSReal m_Value;
};

class RichRealOperand : public I_PMOperand
{
public:
  explicit RichRealOperand (const DBSRichReal& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }

  virtual ~RichRealOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSRichReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;

private:
  DBSRichReal m_Value;
};

class TextOperand : public I_PMOperand
{
public:
  explicit TextOperand (const DBSText& value)
    : I_PMOperand (),
      m_Value (value)
  {
  }
  virtual bool IsNull () const;

  virtual ~TextOperand ();

  virtual void GetValue (DBSText& outValue) const;
  virtual void SetValue (const DBSText& value);

  virtual void SelfAdd (const DBSChar& value);
  virtual void SelfAdd (const DBSText& value);

  virtual StackValue GetValueAt (const D_UINT64 index);

  virtual StackValue CopyValue () const;

private:
  DBSText m_Value;
};

class CharTextElOperand : public I_PMOperand
{
public:
  CharTextElOperand (DBSText &text, const D_UINT64 index)
    : I_PMOperand (),
      m_Index (index),
      m_Text ()
  {
    text.SetMirror (m_Text);
  }

  CharTextElOperand (const CharTextElOperand& source)
    : I_PMOperand (),
      m_Index (source.m_Index),
      m_Text ()
  {
    source.m_Text.SetMirror (m_Text);
  }


  virtual ~CharTextElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

  virtual StackValue CopyValue () const;

private:
  const D_UINT64 m_Index;
  DBSText        m_Text;
};

class ArrayOperand : public I_PMOperand
{
public:
  explicit ArrayOperand (const DBSArray& array)
    : I_PMOperand (),
      m_Value (array)
    {
    }
  virtual ~ArrayOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSArray& outValue) const;
  virtual void SetValue (const DBSArray& value);

  virtual StackValue GetValueAt (const D_UINT64 index);

  virtual StackValue CopyValue () const;

private:
  DBSArray m_Value;
};

class BaseArrayElOperand : public I_PMOperand
{
protected:
  BaseArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_PMOperand (),
      m_ElementIndex (index),
      m_Array  ()
  {
    array.SetMirror (m_Array);
  }

  BaseArrayElOperand (const BaseArrayElOperand& source)
    : I_PMOperand (),
      m_ElementIndex (source.m_ElementIndex),
      m_Array ()
  {
    source.m_Array.SetMirror (m_Array);
  }

  virtual ~BaseArrayElOperand ();

  template <typename DBS_T> void Get (DBS_T& out) const
  {
    if (m_Array.ElementsCount () < m_ElementIndex)
      throw InterException (NULL, _EXTRA (InterException::ARRAY_INDEX_BIGGER));

    m_Array.GetElement (out, m_ElementIndex);

    assert (out.IsNull () == false);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    if (m_Array.ElementsCount () < m_ElementIndex)
      throw InterException (NULL, _EXTRA (InterException::ARRAY_INDEX_BIGGER));

    m_Array.SetElement (value, m_ElementIndex);
  }

private:
  BaseArrayElOperand& operator= (const BaseArrayElOperand& source);

  const D_UINT64 m_ElementIndex;
  DBSArray       m_Array;
};

class BoolArrayElOperand : public BaseArrayElOperand
{
public:
  BoolArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

  virtual void SelfAnd (const DBSBool& value);
  virtual void SelfXor (const DBSBool& value);
  virtual void SelfOr (const DBSBool& value);

  virtual StackValue CopyValue () const;
};

class CharArrayElOperand : public BaseArrayElOperand
{
public:
  CharArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

  virtual StackValue CopyValue () const;
};

class DateArrayElOperand : public BaseArrayElOperand
{
public:
  DateArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

  virtual StackValue CopyValue () const;
};

class DateTimeArrayElOperand : public BaseArrayElOperand
{
public:
  DateTimeArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

  virtual StackValue CopyValue () const;
};

class HiresTimeArrayElOperand : public BaseArrayElOperand
{
public:
  HiresTimeArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

  virtual StackValue CopyValue () const;
};

class UInt8ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt8ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt8& value);


  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class UInt16ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt16ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class UInt32ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt32ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class UInt64ArrayElOperand : public BaseArrayElOperand
{
public:
  UInt64ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int8ArrayElOperand : public BaseArrayElOperand
{
public:
  Int8ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt8& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int16ArrayElOperand : public BaseArrayElOperand
{
public:
  Int16ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int32ArrayElOperand : public BaseArrayElOperand
{
public:
  Int32ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int64ArrayElOperand : public BaseArrayElOperand
{
public:
  Int64ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class RealArrayElOperand : public BaseArrayElOperand
{
public:
  RealArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;
};

class RichRealArrayElOperand : public BaseArrayElOperand
{
public:
  RichRealArrayElOperand (DBSArray& array, const D_UINT64 index)
    : BaseArrayElOperand (array, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSRichReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;
};

class TableOperand : public I_PMOperand
{
public:
  TableOperand (I_DBSHandler& dbsHnd, I_DBSTable& table)
    : I_PMOperand (),
      m_pRefTable (new TableReference (dbsHnd, table))
  {
    m_pRefTable->IncrementRefCount ();
  }

  TableOperand (const TableOperand& source)
    : I_PMOperand (),
      m_pRefTable (source.m_pRefTable)
  {
    m_pRefTable->IncrementRefCount ();
  }

  virtual ~TableOperand ();

  const TableOperand& operator= (const TableOperand& pSource)
  {
    if (this != &pSource)
      {
        m_pRefTable->DecrementRefCount ();
        m_pRefTable = pSource.m_pRefTable;
        m_pRefTable->IncrementRefCount ();
      }
    return *this;
  }

  virtual bool IsNull () const;

  virtual I_DBSTable& GetTable ();

  TableReference& GetTableRef () const
  {
    assert (m_pRefTable != NULL);

    return *m_pRefTable;
  }

  virtual StackValue CopyValue () const;

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

private:
  TableReference* m_pRefTable;
};

class FieldOperand : public I_PMOperand
{
public:
  FieldOperand ()
    : m_pRefTable (NULL),
      m_Field (~0),
      m_FieldType (T_UNDETERMINED)
  {
  }

  FieldOperand (TableOperand& tableOp, const FIELD_INDEX field);
  FieldOperand (const FieldOperand& source);
  virtual ~FieldOperand ();

  const FieldOperand& operator= (const FieldOperand& source);

  virtual bool IsNull () const;

  virtual FIELD_INDEX GetField ();
  virtual I_DBSTable& GetTable ();
  virtual StackValue  GetValueAt (const D_UINT64 index);

  virtual StackValue CopyValue () const;

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);

private:
  TableReference* m_pRefTable;
  FIELD_INDEX     m_Field;
  D_UINT32        m_FieldType;
};

class BaseFieldElOperand : public I_PMOperand
{
protected:
  BaseFieldElOperand (TableReference*   pTableRef,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : m_Row (row),
      m_pRefTable (pTableRef),
      m_Field (field)
  {
    m_pRefTable->IncrementRefCount ();
  }

  BaseFieldElOperand (const BaseFieldElOperand& source)
    : I_PMOperand (),
      m_Row (source.m_Row),
      m_pRefTable (source.m_pRefTable),
      m_Field (source.m_Field)
  {
    m_pRefTable->IncrementRefCount ();
  }

  virtual ~BaseFieldElOperand ();

  template <typename DBS_T> void Get (DBS_T& out) const
  {
    I_DBSTable& table = m_pRefTable->GetTable ();

    if (table.GetAllocatedRows () <= m_Row)
      throw InterException (NULL, _EXTRA (InterException::ROW_INDEX_BIGGER));

    table.GetEntry (m_Row, m_Field, out);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    I_DBSTable& table = m_pRefTable->GetTable ();

    if (table.GetAllocatedRows () <= m_Row)
      throw InterException (NULL, _EXTRA (InterException::ROW_INDEX_BIGGER));

    table.SetEntry (m_Row, m_Field, value);
  }

private:
  friend class CharTextFieldElOperand;
  friend class TextFieldElOperand;
  friend class ArrayFieldElOperand;

  BaseFieldElOperand& operator= (const BaseFieldElOperand* source);

  const ROW_INDEX   m_Row;
  TableReference*   m_pRefTable;
  const FIELD_INDEX m_Field;
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

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

  virtual void SelfAnd (const DBSBool& value);
  virtual void SelfXor (const DBSBool& value);
  virtual void SelfOr (const DBSBool& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt8& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt8& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSRichReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSText& outValue) const;
  virtual void SetValue (const DBSText& value);

  virtual void SelfAdd (const DBSChar& value);
  virtual void SelfAdd (const DBSText& value);

  virtual StackValue GetValueAt (const D_UINT64 index);

  virtual StackValue CopyValue () const;
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

  virtual void GetValue (DBSArray& outValue) const;
  virtual void SetValue (const DBSArray& value);

  virtual StackValue GetValueAt (const D_UINT64 index);

  virtual StackValue CopyValue () const;
};

class CharTextFieldElOperand : public BaseFieldElOperand
{
public:
  CharTextFieldElOperand (TableReference*   pTableRef,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const D_UINT64    index)
    : BaseFieldElOperand (pTableRef, row, field),
      m_Index (index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

  virtual StackValue CopyValue () const;

private:
  const D_UINT64    m_Index;
};

class BaseArrayFieldElOperand : public I_PMOperand
{
protected:
  BaseArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
     : m_Index (index),
       m_Row (row),
       m_pRefTable (pTableRef),
       m_Field (field)
  {
    m_pRefTable->IncrementRefCount ();
  }

  BaseArrayFieldElOperand (const BaseArrayFieldElOperand& source)
     : m_Index (source.m_Index),
       m_Row (source.m_Row),
       m_pRefTable (source.m_pRefTable),
       m_Field (source.m_Field)
  {
    m_pRefTable->IncrementRefCount ();
  }

  virtual ~BaseArrayFieldElOperand ();

  template <typename DBS_T> void Get (DBS_T& outValue) const
  {
    I_DBSTable& table = m_pRefTable->GetTable ();

    if (table.GetAllocatedRows () <= m_Row)
      throw InterException (NULL, _EXTRA (InterException::ROW_INDEX_BIGGER));

    DBSArray array;
    table.GetEntry (m_Row, m_Field, array);

    if (array.ElementsCount () <= m_Index)
      throw InterException (NULL, _EXTRA (InterException::ARRAY_INDEX_BIGGER));

    array.GetElement (outValue, m_Index);
    assert (outValue.IsNull () == false);
  }

  template <typename DBS_T> void Set (const DBS_T& value)
  {
    I_DBSTable& table = m_pRefTable->GetTable ();

    if (table.GetAllocatedRows () <= m_Row)
      throw InterException (NULL, _EXTRA (InterException::ROW_INDEX_BIGGER));

    DBSArray array;
    table.GetEntry (m_Row, m_Field, array);

    if (array.ElementsCount () <= m_Index)
      throw InterException (NULL, _EXTRA (InterException::ARRAY_INDEX_BIGGER));

    array.SetElement (value, m_Index);
    table.SetEntry (m_Row, m_Field, array);
  }

private:
  BaseArrayFieldElOperand& operator= (const BaseArrayFieldElOperand&);

  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  TableReference*   m_pRefTable;
  const FIELD_INDEX m_Field;
};

class BoolArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  BoolArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

  virtual void SelfAnd (const DBSBool& value);
  virtual void SelfXor (const DBSBool& value);
  virtual void SelfOr (const DBSBool& value);

  virtual StackValue CopyValue () const;
};

class CharArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  CharArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

  virtual StackValue CopyValue () const;
};

class DateArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  DateArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

  virtual StackValue CopyValue () const;
};

class DateTimeArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  DateTimeArrayFieldElOperand (TableReference*   pTableRef,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

  virtual StackValue CopyValue () const;
};

class HiresTimeArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  HiresTimeArrayFieldElOperand (TableReference*   pTableRef,
                                const ROW_INDEX   row,
                                const FIELD_INDEX field,
                                const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

  virtual StackValue CopyValue () const;
};

class UInt8ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt8ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt8& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class UInt16ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt16ArrayFieldElOperand (TableReference*   pTableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class UInt32ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt32ArrayFieldElOperand (TableReference*   pTableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class UInt64ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  UInt64ArrayFieldElOperand (TableReference*   pTableRef,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSUInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int8ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int8ArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt8& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int16ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int16ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt16& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int32ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int32ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt32& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class Int64ArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  Int64ArrayFieldElOperand (TableReference*   pTableRef,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;

  virtual void SetValue (const DBSInt64& value);

  virtual void SelfAdd (const DBSInt64& value);

  virtual void SelfSub (const DBSInt64& value);

  virtual void SelfMul (const DBSInt64& value);

  virtual void SelfDiv (const DBSInt64& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfOr (const DBSInt64& value);

  virtual StackValue CopyValue () const;
};

class RealArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  RealArrayFieldElOperand (TableReference*   pTableRef,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;
};

class RichRealArrayFieldElOperand : public BaseArrayFieldElOperand
{
public:
  RichRealArrayFieldElOperand (TableReference*   pTableRef,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const D_UINT64    index)
    : BaseArrayFieldElOperand (pTableRef, row, field, index)
  {
  }

  virtual bool IsNull () const;

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSRichReal& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual StackValue CopyValue () const;
};

class GlobalValue
{
public:
  template <class OP_T>
  explicit GlobalValue (const OP_T& op)
    : m_Sync (),
      m_OperandOwner (true)
  {
    const I_PMOperand& compileTest = op;
    (void)compileTest; //Just to make sure OP_T is a valid type!

    assert (sizeof (OP_T) <= sizeof (m_Storage));
    _placement_new (m_Storage, op);
  }

  GlobalValue (const GlobalValue& source)
    : m_Sync (),
      m_OperandOwner (source.m_OperandOwner)
  {
    for (D_UINT i = 0; i < sizeof (m_Storage) / sizeof (m_Storage[0]); ++i)
      m_Storage[i] = source.m_Storage[i];
  }

  const GlobalValue& operator= (const GlobalValue& source)
  {
    if (this == &source)
      return *this;

    m_OperandOwner = source.m_OperandOwner;
    for (D_UINT i = 0; i < sizeof (m_Storage) / sizeof (m_Storage[0]); ++i)
      m_Storage[i] = source.m_Storage[i];

    return *this;
  }

  ~GlobalValue ()
  {
    if (m_OperandOwner)
      GetOperand ().~I_PMOperand ();
  }

  bool IsNull ()
  {
    WSynchronizerRAII dummy (m_Sync);
    return GetOperand ().IsNull ();
  }

  template <class DBS_T>
  void GetValue (DBS_T& outValue)
  {
    WSynchronizerRAII dummy (m_Sync);
    GetOperand ().GetValue (outValue);
  }

  template <class DBS_T>
  void SetValue (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SetValue (value);
  }

  template <class DBS_T>
  void SelfAdd (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfSub (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfSub (value);
  }

  template <class DBS_T>
  void SelfMul (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfDiv (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfMod (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfAdd (value);
  }

  template <class DBS_T>
  void SelfAnd (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfAnd (value);
  }

  template <class DBS_T>
  void SelfXor (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfXor (value);
  }

  template <class DBS_T>
  void SelfOr (const DBS_T& value)
  {
    WSynchronizerRAII dummy(m_Sync);
    GetOperand ().SelfOr (value);
  }

  FIELD_INDEX GetField ()
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().GetField ();
  }

  I_DBSTable& GetTable ()
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().GetTable ();
  }

  StackValue GetValueAt (const D_UINT64 index)
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().GetValueAt (index);
  }

  StackValue CopyValue ()
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().CopyValue ();
  }

  TableOperand GetTableOp ()
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().GetTableOp ();
  }

  void CopyTableOp (const TableOperand& source)
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().CopyTableOp (source);
  }

  FieldOperand  GetFieldOp ()
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().GetFieldOp ();
  }

  void CopyFieldOp (const FieldOperand& source)
  {
    WSynchronizerRAII dummy(m_Sync);
    return GetOperand ().CopyFieldOp (source);
  }

  I_PMOperand& GetOperand () { return *_RC (I_PMOperand*, m_Storage);  }

  void Release () { assert (m_OperandOwner); m_OperandOwner = false; }

private:
  WSynchronizer m_Sync;
  bool          m_OperandOwner;
  D_UINT64      m_Storage[MAX_OP_QWORDS];
};

class GlobalOperand : public I_PMOperand
{
public:
  GlobalOperand (GlobalValue& global);
  ~GlobalOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;
  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;
  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;
  virtual void GetValue (DBSText& outValue) const;
  virtual void GetValue (DBSArray& outValue) const;

  virtual void SetValue (const DBSBool& value);
  virtual void SetValue (const DBSChar& value);
  virtual void SetValue (const DBSDate& value);
  virtual void SetValue (const DBSDateTime& value);
  virtual void SetValue (const DBSHiresTime& value);
  virtual void SetValue (const DBSInt8& value);
  virtual void SetValue (const DBSInt16& value);
  virtual void SetValue (const DBSInt32& value);
  virtual void SetValue (const DBSInt64& value);
  virtual void SetValue (const DBSReal& value);
  virtual void SetValue (const DBSRichReal& value);
  virtual void SetValue (const DBSUInt8& value);
  virtual void SetValue (const DBSUInt16& value);
  virtual void SetValue (const DBSUInt32& value);
  virtual void SetValue (const DBSUInt64& value);
  virtual void SetValue (const DBSText& value);
  virtual void SetValue (const DBSArray& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);
  virtual void SelfAdd (const DBSChar& value);
  virtual void SelfAdd (const DBSText& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfAnd (const DBSBool& value);

  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfXor (const DBSBool& value);

  virtual void SelfOr (const DBSInt64& value);
  virtual void SelfOr (const DBSBool& value);

  virtual FIELD_INDEX GetField ();
  virtual I_DBSTable& GetTable ();
  virtual StackValue  GetValueAt (const D_UINT64 index);

  virtual StackValue CopyValue () const;

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);

private:
  GlobalValue& m_Value;

};

class LocalOperand : public I_PMOperand
{
public:
  LocalOperand (SessionStack& stack, const D_UINT64 index);

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;
  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;
  virtual void GetValue (DBSInt8& outValue) const;
  virtual void GetValue (DBSInt16& outValue) const;
  virtual void GetValue (DBSInt32& outValue) const;
  virtual void GetValue (DBSInt64& outValue) const;
  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;
  virtual void GetValue (DBSText& outValue) const;
  virtual void GetValue (DBSArray& outValue) const;

  virtual void SetValue (const DBSBool& value);
  virtual void SetValue (const DBSChar& value);
  virtual void SetValue (const DBSDate& value);
  virtual void SetValue (const DBSDateTime& value);
  virtual void SetValue (const DBSHiresTime& value);
  virtual void SetValue (const DBSInt8& value);
  virtual void SetValue (const DBSInt16& value);
  virtual void SetValue (const DBSInt32& value);
  virtual void SetValue (const DBSInt64& value);
  virtual void SetValue (const DBSReal& value);
  virtual void SetValue (const DBSRichReal& value);
  virtual void SetValue (const DBSUInt8& value);
  virtual void SetValue (const DBSUInt16& value);
  virtual void SetValue (const DBSUInt32& value);
  virtual void SetValue (const DBSUInt64& value);
  virtual void SetValue (const DBSText& value);
  virtual void SetValue (const DBSArray& value);

  virtual void SelfAdd (const DBSInt64& value);
  virtual void SelfAdd (const DBSRichReal& value);
  virtual void SelfAdd (const DBSChar& value);
  virtual void SelfAdd (const DBSText& value);

  virtual void SelfSub (const DBSInt64& value);
  virtual void SelfSub (const DBSRichReal& value);

  virtual void SelfMul (const DBSInt64& value);
  virtual void SelfMul (const DBSRichReal& value);

  virtual void SelfDiv (const DBSInt64& value);
  virtual void SelfDiv (const DBSRichReal& value);

  virtual void SelfMod (const DBSInt64& value);

  virtual void SelfAnd (const DBSInt64& value);
  virtual void SelfAnd (const DBSBool& value);

  virtual void SelfXor (const DBSInt64& value);
  virtual void SelfXor (const DBSBool& value);

  virtual void SelfOr (const DBSInt64& value);
  virtual void SelfOr (const DBSBool& value);

  virtual FIELD_INDEX GetField ();
  virtual I_DBSTable& GetTable ();
  virtual StackValue  GetValueAt (const D_UINT64 index);

  virtual StackValue CopyValue () const;

  virtual TableOperand  GetTableOp ();
  virtual void          CopyTableOp (const TableOperand& source);

  virtual FieldOperand  GetFieldOp ();
  virtual void          CopyFieldOp (const FieldOperand& source);

private:
  const D_UINT64 m_Index;
  SessionStack&  m_Stack;
};

}

#endif /* PM_OPERAND_H_ */

