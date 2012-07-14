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

class NullOperand : public I_Operand
{
public:
  NullOperand ()
    : I_Operand ()
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
};

class BoolOperand : public I_Operand
{
public:
  explicit BoolOperand (const DBSBool& value)
    : I_Operand (),
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


private:
  DBSBool m_Value;
};

class CharOperand : public I_Operand
{
public:
  explicit CharOperand (const DBSChar& value)
    : I_Operand (),
      m_Value (value)
  {
  }
  virtual ~CharOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

private:
  DBSChar m_Value;
};

class DateOperand : public I_Operand
{
public:
  explicit DateOperand (const DBSDate& value)
    : I_Operand (),
      m_Value (value)
  {
  }
  virtual ~DateOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

private:
  DBSDate m_Value;
};

class DateTimeOperand : public I_Operand
{
public:
  explicit DateTimeOperand (const DBSDateTime& value)
    : I_Operand (),
      m_Value (value)
  {
  }
  virtual ~DateTimeOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

private:
  DBSDateTime m_Value;
};

class HiresTimeOperand : public I_Operand
{
public:
  explicit HiresTimeOperand (const DBSHiresTime& value)
    : I_Operand (),
      m_Value (value)
  {
  }
  virtual ~HiresTimeOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

private:
  DBSHiresTime m_Value;
};

class UInt8Operand : public I_Operand
{
public:
  explicit UInt8Operand (const DBSUInt8& value)
    : I_Operand (),
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

private:
  DBSUInt8 m_Value;
};

class UInt16Operand : public I_Operand
{
public:
  explicit UInt16Operand (const DBSUInt16& value)
    : I_Operand (),
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

private:
  DBSUInt16 m_Value;
};

class UInt32Operand : public I_Operand
{
public:
  explicit UInt32Operand (const DBSUInt32& value)
    : I_Operand (),
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

private:
  DBSUInt32 m_Value;
};

class UInt64Operand : public I_Operand
{
public:
  explicit UInt64Operand (const DBSUInt64& value)
    : I_Operand (),
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

private:
  DBSUInt64 m_Value;
};

class Int8Operand : public I_Operand, public DBSInt8
{
public:
  explicit Int8Operand (const DBSInt8& value)
    : I_Operand (),
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

private:
  DBSInt8 m_Value;
};

class Int16Operand : public I_Operand, public DBSInt16
{
public:
  explicit Int16Operand (const DBSInt16& value)
    : I_Operand (),
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

private:
  DBSInt16 m_Value;
};

class Int32Operand : public I_Operand
{
public:
  explicit Int32Operand (const DBSInt32& value)
    : I_Operand (),
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

private:
  DBSInt32 m_Value;
};

class Int64Operand : public I_Operand
{
public:
  explicit Int64Operand (const DBSInt64& value)
    : I_Operand (),
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

private:
  DBSInt64 m_Value;
};


class RealOperand : public I_Operand
{
public:
  explicit RealOperand (const DBSReal& value)
    : I_Operand (),
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

private:
  DBSReal m_Value;
};

class RichRealOperand : public I_Operand
{
public:
  explicit RichRealOperand (const DBSRichReal& value)
    : I_Operand (),
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


private:
  DBSRichReal m_Value;
};

class TextOperand : public I_Operand
{
public:
  explicit TextOperand (const DBSText& value)
    : I_Operand (),
      m_Value (value)
  {
  }
  virtual bool IsNull () const;

  virtual ~TextOperand ();

  virtual void GetValue (DBSText& outValue) const;
  virtual void SetValue (const DBSText& value);

  virtual void SelfAdd (const DBSChar& value);
  virtual void SelfAdd (const DBSText& value);

private:
  DBSText m_Value;
};

class CharTextOperand : public I_Operand
{
public:
  CharTextOperand (DBSText &text, const D_UINT64 index)
    : I_Operand (),
      m_Index (index),
      m_Text (text)
  {
  }

  virtual ~CharTextOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

private:
  const D_UINT64 m_Index;
  DBSText&       m_Text;
};

class ArrayOperand : public I_Operand
{
public:
  explicit ArrayOperand (const DBSArray& array)
    : I_Operand (),
      m_Value (array)
    {
    }
  virtual ~ArrayOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSArray& outValue) const;
  virtual void SetValue (const DBSArray& value);

private:
  DBSArray m_Value;
};

class BoolArrayElOperand : public I_Operand
{
public:
  BoolArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~BoolArrayElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

  virtual void SelfAnd (const DBSBool& value);
  virtual void SelfXor (const DBSBool& value);
  virtual void SelfOr (const DBSBool& value);


private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class CharArrayElOperand : public I_Operand
{
public:
  CharArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~CharArrayElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class DateArrayElOperand : public I_Operand
{
public:
  DateArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~DateArrayElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class DateTimeArrayElOperand : public I_Operand
{
public:
  DateTimeArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~DateTimeArrayElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class HiresTimeArrayElOperand : public I_Operand
{
public:
  HiresTimeArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~HiresTimeArrayElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class UInt8ArrayElOperand : public I_Operand
{
  UInt8ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~UInt8ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class UInt16ArrayElOperand : public I_Operand
{
  UInt16ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~UInt16ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class UInt32ArrayElOperand : public I_Operand
{
  UInt32ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~UInt32ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class UInt64ArrayElOperand : public I_Operand
{
  UInt64ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~UInt64ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class Int8ArrayElOperand : public I_Operand
{
  Int8ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~Int8ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class Int16ArrayElOperand : public I_Operand
{
  Int16ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~Int16ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class Int32ArrayElOperand : public I_Operand
{
  Int32ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~Int32ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class Int64ArrayElOperand : public I_Operand
{
  Int64ArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~Int64ArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class RealArrayElOperand : public I_Operand
{
  RealArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~RealArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};

class RichRealArrayElOperand : public I_Operand
{
  RichRealArrayElOperand (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
  {
  }

  virtual ~RichRealArrayElOperand ();

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

private:
  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
};


class TableOperand : public I_Operand
{
public:
  TableOperand (I_DBSHandler& dbsHnd, I_DBSTable& table)
    : I_Operand (),
      m_pRefTable (new TableReference (dbsHnd, table))
  {
    m_pRefTable->IncrementRefCount ();
  }

  TableOperand (const TableOperand& source)
    : I_Operand (),
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


private:
  TableReference* m_pRefTable;
};

class FieldOperand : public I_Operand
{
public:
  FieldOperand ()
    : m_pRefTable (NULL),
      m_Field (),
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

private:
  TableReference* m_pRefTable;
  FIELD_INDEX     m_Field;
  D_UINT32        m_FieldType;
};

class BoolFieldElOperand : public I_Operand
{
public:
  BoolFieldElOperand (I_DBSTable&       table,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~BoolFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

  virtual void SelfAnd (const DBSBool& value);
  virtual void SelfXor (const DBSBool& value);
  virtual void SelfOr (const DBSBool& value);

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class CharFieldElOperand : public I_Operand
{
public:
  CharFieldElOperand (I_DBSTable&       table,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~CharFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class DateFieldElOperand : public I_Operand
{
public:
  DateFieldElOperand (I_DBSTable&       table,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~DateFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class DateTimeFieldElOperand : public I_Operand
{
public:
  DateTimeFieldElOperand (I_DBSTable&       table,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~DateTimeFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class HiresTimeFieldElOperand : public I_Operand
{
public:
  HiresTimeFieldElOperand (I_DBSTable&       table,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~HiresTimeFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt8FieldElOperand : public I_Operand
{
public:
  UInt8FieldElOperand (I_DBSTable&       table,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt8FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt16FieldElOperand : public I_Operand
{
public:
  UInt16FieldElOperand (I_DBSTable&       table,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt16FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt32FieldElOperand : public I_Operand
{
public:
  UInt32FieldElOperand (I_DBSTable&       table,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt32FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt64FieldElOperand : public I_Operand
{
public:
  UInt64FieldElOperand (I_DBSTable&       table,
                        const ROW_INDEX   row,
                        const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt64FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int8FieldElOperand : public I_Operand
{
public:
  Int8FieldElOperand (I_DBSTable&       table,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int8FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int16FieldElOperand : public I_Operand
{
public:
  Int16FieldElOperand (I_DBSTable&       table,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int16FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int32FieldElOperand : public I_Operand
{
public:
  Int32FieldElOperand (I_DBSTable&       table,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int32FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int64FieldElOperand : public I_Operand
{
public:
  Int64FieldElOperand (I_DBSTable&       table,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int64FieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class RealFieldElOperand : public I_Operand
{
public:
  RealFieldElOperand (I_DBSTable&       table,
                       const ROW_INDEX   row,
                       const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~RealFieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class RichRealFieldElOperand : public I_Operand
{
public:
  RichRealFieldElOperand (I_DBSTable&       table,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~RichRealFieldElOperand ();

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

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class TextFieldElOperand : public I_Operand
{
public:
  TextFieldElOperand (I_DBSTable&       table,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }
  virtual ~TextFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSText& outValue) const;
  virtual void SetValue (const DBSText& value);

  virtual void SelfAdd (const DBSChar& value);
  virtual void SelfAdd (const DBSText& value);

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class ArrayFieldElOperand : public I_Operand
{
public:
  ArrayFieldElOperand (I_DBSTable&      table,
                      const ROW_INDEX   row,
                      const FIELD_INDEX field)
    : m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~ArrayFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSArray& outValue) const;
  virtual void SetValue (const DBSArray& value);

private:
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class CharTextFieldElOperand : public I_Operand
{
public:
  CharTextFieldElOperand (I_DBSTable&       table,
                          const ROW_INDEX   row,
                          const FIELD_INDEX field,
                          const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~CharTextFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};


class BoolArrayFieldElOperand : public I_Operand
{
public:
  BoolArrayFieldElOperand (I_DBSTable&       table,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~BoolArrayFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

  virtual void SelfAnd (const DBSBool& value);
  virtual void SelfXor (const DBSBool& value);
  virtual void SelfOr (const DBSBool& value);

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class CharArrayFieldElOperand : public I_Operand
{
public:
  CharArrayFieldElOperand (I_DBSTable&       table,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~CharArrayFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class DateArrayFieldElOperand : public I_Operand
{
public:
  DateArrayFieldElOperand (I_DBSTable&       table,
                                const ROW_INDEX   row,
                                const FIELD_INDEX field,
                                const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~DateArrayFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class DateTimeArrayFieldElOperand : public I_Operand
{
public:
  DateTimeArrayFieldElOperand (I_DBSTable&       table,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~DateTimeArrayFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class HiresTimeArrayFieldElOperand : public I_Operand
{
public:
  HiresTimeArrayFieldElOperand (I_DBSTable&       table,
                                const ROW_INDEX   row,
                                const FIELD_INDEX field,
                                const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~HiresTimeArrayFieldElOperand ();

  virtual bool IsNull () const;

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt8ArrayFieldElOperand : public I_Operand
{
public:
  UInt8ArrayFieldElOperand (I_DBSTable&       table,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt8ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt16ArrayFieldElOperand : public I_Operand
{
public:
  UInt16ArrayFieldElOperand (I_DBSTable&       table,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt16ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt32ArrayFieldElOperand : public I_Operand
{
public:
  UInt32ArrayFieldElOperand (I_DBSTable&       table,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt32ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class UInt64ArrayFieldElOperand : public I_Operand
{
public:
  UInt64ArrayFieldElOperand (I_DBSTable&       table,
                             const ROW_INDEX   row,
                             const FIELD_INDEX field,
                             const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~UInt64ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int8ArrayFieldElOperand : public I_Operand
{
public:
  Int8ArrayFieldElOperand (I_DBSTable&       table,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int8ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int16ArrayFieldElOperand : public I_Operand
{
public:
  Int16ArrayFieldElOperand (I_DBSTable&       table,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int16ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int32ArrayFieldElOperand : public I_Operand
{
public:
  Int32ArrayFieldElOperand (I_DBSTable&       table,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int32ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class Int64ArrayFieldElOperand : public I_Operand
{
public:
  Int64ArrayFieldElOperand (I_DBSTable&       table,
                            const ROW_INDEX   row,
                            const FIELD_INDEX field,
                            const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~Int64ArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class RealArrayFieldElOperand : public I_Operand
{
public:
  RealArrayFieldElOperand (I_DBSTable&   table,
                           const ROW_INDEX   row,
                           const FIELD_INDEX field,
                           const D_UINT64    index)
    : m_Index (index),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~RealArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

class RichRealArrayFieldElOperand : public I_Operand
{
public:
  RichRealArrayFieldElOperand (I_DBSTable&       table,
                               const ROW_INDEX   row,
                               const FIELD_INDEX field,
                               const D_UINT64    index)
    : m_Index (index),
      m_Table (table),
      m_Row (row),
      m_Field (field)
  {
  }

  virtual ~RichRealArrayFieldElOperand ();

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

private:
  const D_UINT64    m_Index;
  I_DBSTable&       m_Table;
  const ROW_INDEX   m_Row;
  const FIELD_INDEX m_Field;
};


class GlobalValue
{
public:
  template <class OP_T>
  explicit GlobalValue (const OP_T& op)
    : m_Sync (),
      m_Storage ()
  {
    const I_Operand& compileTest = op;
    (void)compileTest; //Just to make sure OP_T is a valid type!

    assert (sizeof (OP_T) <= sizeof (m_Storage));
    _placement_new (m_Storage, op);
  }

  GlobalValue (const GlobalValue& source)
    : m_Sync (),
      m_Storage ()
  {
    for (D_UINT i = 0; i < sizeof (m_Storage) / sizeof (m_Storage[0]); ++i)
      m_Storage[i] = source.m_Storage[i];
  }

  const GlobalValue& operator= (const GlobalValue& source)
  {
    if (this == &source)
      return *this;

    for (D_UINT i = 0; i < sizeof (m_Storage) / sizeof (m_Storage[0]); ++i)
      m_Storage[i] = source.m_Storage[i];

    return *this;
  }

  ~GlobalValue ()
  {
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


  I_Operand& GetOperand () { return *_RC (I_Operand*, m_Storage);  }

private:
  WSynchronizer m_Sync;
  D_UINT64      m_Storage[MAX_OP_QWORDS];
};

class GlobalOperand : public I_Operand
{
public:
  GlobalOperand (GlobalValue& global);
  ~GlobalOperand ();

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

  virtual FIELD_INDEX   GetField ();
  virtual I_DBSTable&   GetTable ();

private:
  GlobalValue& m_Value;
};

class LocalOperand : public I_Operand
{
public:
  LocalOperand (StackValue& localValue);
  ~LocalOperand ();

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

  virtual FIELD_INDEX   GetField ();
  virtual I_DBSTable&   GetTable ();

private:
  StackValue& m_LocalValue;
};

}

#endif /* PM_OPERAND_H_ */

