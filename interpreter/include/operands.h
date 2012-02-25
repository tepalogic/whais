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

#ifndef OPERANDS_H_
#define OPERANDS_H_

#include <assert.h>

#include "dbs/include/dbs_mgr.h"
#include "dbs/include/dbs_values.h"

class TableOperand;
class RowOperand;

class I_Operand
{
public:
  I_Operand ()
  {
  }

  virtual ~I_Operand ();

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
  virtual void GetValue (DBSText& outValue) const;
  virtual void GetValue (DBSUInt8& outValue) const;
  virtual void GetValue (DBSUInt16& outValue) const;
  virtual void GetValue (DBSUInt32& outValue) const;
  virtual void GetValue (DBSUInt64& outValue) const;
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
  virtual void SetValue (const DBSText& value);
  virtual void SetValue (const DBSUInt8& value);
  virtual void SetValue (const DBSUInt16& value);
  virtual void SetValue (const DBSUInt32& value);
  virtual void SetValue (const DBSUInt64& value);
  virtual void SetValue (const DBSArray& value);

  virtual TableOperand& GetTableOp ();
  virtual RowOperand&   GetRowOp();
};

class BoolOperand : public I_Operand
{
public:
  explicit BoolOperand (const DBSBool& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~BoolOperand ();

  virtual void GetValue (DBSBool& outValue) const;

  virtual void SetValue (const DBSBool& value);

protected:
  DBSBool m_Value;
};

class CharOperand : public I_Operand
{
public:
  explicit CharOperand (const DBSChar& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~CharOperand ();

  virtual void GetValue (DBSChar& outValue) const;
  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSChar& value);
protected:
  DBSChar m_Value;
};

class DateOperand : public I_Operand
{
public:
  explicit DateOperand (const DBSDate& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~DateOperand ();

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDate& value);

protected:
  DBSDate m_Value;
};

class DateTimeOperand : public I_Operand
{
public:
  explicit DateTimeOperand (const DBSDateTime& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~DateTimeOperand ();

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSDateTime& value);

protected:
  DBSDateTime m_Value;
};

class HiresTimeOperand : public I_Operand
{
public:
  explicit HiresTimeOperand (const DBSHiresTime& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~HiresTimeOperand ();

  virtual void GetValue (DBSDate& outValue) const;
  virtual void GetValue (DBSDateTime& outValue) const;
  virtual void GetValue (DBSHiresTime& outValue) const;

  virtual void SetValue (const DBSHiresTime& value);
protected:
  DBSHiresTime m_Value;
};

class UInt8Operand : public I_Operand
{
public:
  explicit UInt8Operand (const DBSUInt8& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~UInt8Operand ();

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

protected:
  DBSUInt8 m_Value;
};

class UInt16Operand : public I_Operand
{
public:
  explicit UInt16Operand (const DBSUInt16& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~UInt16Operand ();

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

protected:
  DBSUInt16 m_Value;
};

class UInt32Operand : public I_Operand
{
public:
  explicit UInt32Operand (const DBSUInt32& value) :
    I_Operand (),
    m_Value (value)
  {
  }

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

protected:
  DBSUInt32 m_Value;
};

class UInt64Operand : public I_Operand
{
public:
  explicit UInt64Operand (const DBSUInt64& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~UInt64Operand ();

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

protected:
  DBSUInt64 m_Value;
};

class Int8Operand : public I_Operand, public DBSInt8
{
public:
  explicit Int8Operand (const DBSInt8& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~Int8Operand ();

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

protected:
  DBSInt8 m_Value;
};

class Int16Operand : public I_Operand, public DBSInt16
{
public:
  explicit Int16Operand (const DBSInt16& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~Int16Operand ();

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

protected:
  DBSInt16 m_Value;
};

class Int32Operand : public I_Operand
{
public:
  explicit Int32Operand (const DBSInt32& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~Int32Operand ();

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

protected:
  DBSInt32 m_Value;
};

class Int64Operand : public I_Operand
{
public:
  explicit Int64Operand (const DBSInt64& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~Int64Operand ();

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

protected:
  DBSInt64 m_Value;
};


class RealOperand : public I_Operand
{
public:
  explicit RealOperand (const DBSReal& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~RealOperand ();

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSReal& value);

protected:
  DBSReal m_Value;
};

class RichRealOperand : public I_Operand
{
public:
  explicit RichRealOperand (const DBSRichReal& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~RichRealOperand ();

  virtual void GetValue (DBSReal& outValue) const;
  virtual void GetValue (DBSRichReal& outValue) const;

  virtual void SetValue (const DBSRichReal& value);
protected:
  DBSRichReal m_Value;
};

class TextOperand : public I_Operand
{
public:
  explicit TextOperand (const DBSText& value) :
    I_Operand (),
    m_Value (value)
  {
  }

  virtual ~TextOperand ();

  virtual void GetValue (DBSText& outValue) const;

  virtual void SetValue (const DBSText& value);
protected:
  DBSText m_Value;
};


class CharTextOperand : public I_Operand
{
public:
  explicit CharTextOperand (DBSText &text, const D_UINT64 elIndex);
  virtual ~CharTextOperand ();

  virtual void GetValue (DBSChar& outValue) const;

  virtual void SetValue (const DBSChar& value);

protected:
  const D_UINT64 m_Index;
  DBSText        m_Text;
};

class ArrayOperand : public I_Operand
{
public:
  explicit ArrayOperand (const DBSArray& array) :
    I_Operand (),
    m_Value (array)
    {
    }

  virtual ~ArrayOperand ();

  virtual void GetValue (DBSArray& outValue) const;

  virtual void SetValue (const DBSArray& value);

protected:
  DBSArray m_Value;
};

template <class OP_T, class DBS_T>
class ArrayElement : public I_Operand
{
public:
  ArrayElement (DBSArray& array, const D_UINT64 index) :
    I_Operand (),
    m_ElementIndex (index),
    m_Array (array)
    {
    }

  virtual ~ArrayElement ()
    {
    }

  virtual void GetValue (DBSBool& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSChar& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSDate& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSDateTime& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSHiresTime& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt8& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt16& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt32& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt64& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSReal& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSRichReal& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt8& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt16& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt32& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt64& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void SetValue (const DBS_T& value)
    {
      m_Array.SetElement (value, m_ElementIndex);
    }

protected:
  OP_T GetInternalOp () const
    {
      DBS_T value;
      m_Array.GetElement (value, m_ElementIndex);
      return OP_T (value);
    }

  D_UINT64 m_ElementIndex;
  DBSArray m_Array;
};

typedef ArrayElement<BoolOperand, DBSBool>           IndexedBoolOperand;
typedef ArrayElement<CharOperand, DBSChar>           IndexedCharOperand;
typedef ArrayElement<DateOperand, DBSDate>           IndexedDateOperand;
typedef ArrayElement<DateTimeOperand, DBSDateTime>   IndexedDateTimeOperand;
typedef ArrayElement<HiresTimeOperand, DBSHiresTime> IndexedHiresTimeOperand;
typedef ArrayElement<UInt8Operand, DBSUInt8>         IndexedUInt8Operand;
typedef ArrayElement<UInt16Operand, DBSUInt16>       IndexedUInt16Operand;
typedef ArrayElement<UInt32Operand, DBSUInt32>       IndexedUInt32Operand;
typedef ArrayElement<UInt64Operand, DBSUInt64>       IndexedUInt64Operand;
typedef ArrayElement<Int8Operand, DBSInt8>           IndexedInt8Operand;
typedef ArrayElement<Int16Operand, DBSInt16>         IndexedInt16Operand;
typedef ArrayElement<Int32Operand, DBSInt32>         IndexedInt32Operand;
typedef ArrayElement<Int64Operand, DBSInt64>         IndexedInt64Operand;
typedef ArrayElement<RealOperand, DBSReal>           IndexedRealOperand;
typedef ArrayElement<RichRealOperand, DBSRichReal>   IndexedRichRealOperand;

class TableOperand : public I_Operand
{
public:
  TableOperand (I_DBSHandler* const pDbsHnd, I_DBSTable* const pTable);
  TableOperand (const TableOperand& source);
  virtual ~TableOperand ();

  I_DBSTable& GetTable () { return *m_pTable; }

protected:
  I_DBSHandler* const m_pDbsHandler;
  I_DBSTable* const   m_pTable;
};

class RowOperand : public I_Operand
{
public:
  RowOperand (I_DBSTable& table, const D_UINT64 rowIndex);
  virtual ~RowOperand ();

protected:
  I_DBSTable& m_Table;
  D_UINT64    m_Index;
};


template <class OP_T, class DBS_T>
class FieldOperand : public I_Operand
{
public:
  FieldOperand (I_DBSTable& table, const D_UINT64 rowIndex, const D_UINT fieldIndex) :
    m_Table (table),
    m_RowIndex (rowIndex),
    m_FieldIndex (fieldIndex)
  {
  }

  virtual ~FieldOperand ()
  {
  }

  virtual void GetValue (DBSBool& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSChar& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSDate& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSDateTime& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSHiresTime& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt8& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt16& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt32& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSInt64& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSReal& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSRichReal& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt8& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt16& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt32& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSUInt64& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSArray& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void GetValue (DBSText& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }


  virtual void SetValue (const DBS_T& value)
  {
    m_Table.SetEntry (value, m_RowIndex, m_FieldIndex);
  }

protected:
  OP_T GetInternalOp () const
  {
    DBS_T value;
    m_Table.GetEntry (value, m_RowIndex, m_FieldIndex);
    return OP_T (value);
  }

  I_DBSTable&    m_Table;
  const D_UINT64 m_RowIndex;
  const D_UINT   m_FieldIndex;
};

typedef FieldOperand<BoolOperand, DBSBool>           BoolFieldOperand;
typedef FieldOperand<CharOperand, DBSChar>           CharFieldOperand;
typedef FieldOperand<DateOperand, DBSDate>           DateFieldOperand;
typedef FieldOperand<DateTimeOperand, DBSDateTime>   DateTimeFieldOperand;
typedef FieldOperand<HiresTimeOperand, DBSHiresTime> HiresTimeFieldOperand;
typedef FieldOperand<UInt8Operand, DBSUInt8>         UInt8FieldOperand;
typedef FieldOperand<UInt16Operand, DBSUInt16>       UInt16FieldOperand;
typedef FieldOperand<UInt32Operand, DBSUInt32>       UInt32FieldOperand;
typedef FieldOperand<UInt64Operand, DBSUInt64>       UInt64FieldOperand;
typedef FieldOperand<Int8Operand, DBSInt8>           Int8FieldOperand;
typedef FieldOperand<Int16Operand, DBSInt16>         Int16FieldOperand;
typedef FieldOperand<Int32Operand, DBSInt32>         Int32FieldOperand;
typedef FieldOperand<Int64Operand, DBSInt64>         Int64FieldOperand;
typedef FieldOperand<RealOperand, DBSReal>           RealFieldOperand;
typedef FieldOperand<RichRealOperand, DBSRichReal>   RichRealFieldOperand;
typedef FieldOperand<ArrayOperand, DBSArray>         ArrayFieldOperand;
typedef FieldOperand<TextOperand, DBSText>           TextFieldOperand;

class OperandStorage
{
private:
  friend class StackValue;

  static const D_UINT MAX_OP_SIZE = 3;

  D_UINT64 m_Storage [MAX_OP_SIZE];

  I_Operand& GetOperand () { return *_RC (I_Operand*, m_Storage); }
};

class StackValue
{
public:

  template <class OP_T>
  explicit StackValue (const OP_T& op)
  {
    const I_Operand& compileTest = op;
    (void)compileTest;

    assert (sizeof (OP_T) <= sizeof (OperandStorage));

    _placement_new (m_Operand.m_Storage, op);
  }

  ~StackValue ()
  {
  }

  template <class DBS_T>
  void GetValue (DBS_T& outValue) const
  {
    m_Operand.GetOperand ().GetValue (outValue);
  }

  template <class DBS_T>
  void SetValue (const DBS_T& value)
  {
    m_Operand.GetOperand ().SetValue (value);
  }

  I_Operand& GetOperand () { return m_Operand.GetOperand (); }

protected:
  OperandStorage m_Operand;
};


#endif /* OPERANDS_H_ */

