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


template <class OP_T, class DBS_T>
class ArrayElement : public I_Operand
{
public:
  ArrayElement (DBSArray& array, const D_UINT64 index)
    : I_Operand (),
      m_ElementIndex (index),
      m_Array (array)
    {
    }

  virtual ~ArrayElement ()
    {
    }

  virtual bool IsNull () const
  {
    //An array could not hold NULL elements.
    return false;
  }

  virtual void GetValue (DBS_T& outValue) const
    {
      GetInternalOp ().GetValue (outValue);
    }

  virtual void SetValue (const DBS_T& value)
    {
      m_Array.SetElement (value, m_ElementIndex);
    }

private:
  OP_T GetInternalOp () const
    {
      DBS_T     value;

      m_Array.GetElement (value, m_ElementIndex);

      return OP_T (value);
    }

  const D_UINT64 m_ElementIndex;
  DBSArray&      m_Array;
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
      m_Field ()
  {
  }
  FieldOperand (TableOperand& tableOp, const FIELD_INDEX field)
    : I_Operand (),
      m_pRefTable (&tableOp.GetTableRef ()),
      m_Field (field)
  {
    m_pRefTable->IncrementRefCount ();
  }
  FieldOperand (const FieldOperand& source)
    : m_pRefTable (source.m_pRefTable),
      m_Field (source.m_Field)
  {
    m_pRefTable->IncrementRefCount ();
  }
  virtual ~FieldOperand ();

  const FieldOperand& operator= (const FieldOperand& source)
  {
    if (this != &source)
      {
        if (m_pRefTable != NULL)
          m_pRefTable->DecrementRefCount ();

        m_pRefTable = source.m_pRefTable;
        m_Field     = source.m_Field;

        m_pRefTable->IncrementRefCount ();
      }

    return *this;
  }



  virtual bool IsNull () const;

  virtual FIELD_INDEX GetField ();
  virtual I_DBSTable& GetTable ();

private:
  TableReference* m_pRefTable;
  FIELD_INDEX     m_Field;
};

template <class OP_T, class DBS_T>
class TableValueOperand : public I_Operand
{
public:
  TableValueOperand (I_DBSTable&       table,
                     const ROW_INDEX   row,
                     const FIELD_INDEX field)
    : I_Operand(),
      m_Row (row),
      m_Table (table),
      m_Field (field)
  {
  }

  virtual ~TableValueOperand ()
  {
  }

  virtual bool IsNull () const
  {
    DBS_T tableValue;

    GetInternalOp ().GetValue (tableValue);

    return tableValue.IsNull ();
  }


  virtual void GetValue (DBS_T& outValue) const
  {
    GetInternalOp ().GetValue (outValue);
  }

  virtual void SetValue (const DBS_T& value)
  {
    m_Table.SetEntry (value, m_Row, m_Field);
  }

private:
  OP_T GetInternalOp () const
  {
    DBS_T value;

    m_Table.GetEntry (m_Row, m_Field, value);

    return OP_T (value);
  }

  const ROW_INDEX   m_Row;
  I_DBSTable&       m_Table;
  const FIELD_INDEX m_Field;
};

typedef TableValueOperand<BoolOperand, DBSBool>           BoolFieldValueOperand;
typedef TableValueOperand<CharOperand, DBSChar>           CharFieldValueOperand;
typedef TableValueOperand<DateOperand, DBSDate>           DateFieldValueOperand;
typedef TableValueOperand<DateTimeOperand, DBSDateTime>   DateTimeFieldValueOperand;
typedef TableValueOperand<HiresTimeOperand, DBSHiresTime> HiresTimeFieldValueOperand;
typedef TableValueOperand<UInt8Operand, DBSUInt8>         UInt8FieldValueOperand;
typedef TableValueOperand<UInt16Operand, DBSUInt16>       UInt16FieldValueOperand;
typedef TableValueOperand<UInt32Operand, DBSUInt32>       UInt32FieldValueOperand;
typedef TableValueOperand<UInt64Operand, DBSUInt64>       UInt64FieldValueOperand;
typedef TableValueOperand<Int8Operand, DBSInt8>           Int8FieldValueOperand;
typedef TableValueOperand<Int16Operand, DBSInt16>         Int16FieldValueOperand;
typedef TableValueOperand<Int32Operand, DBSInt32>         Int32FieldValueOperand;
typedef TableValueOperand<Int64Operand, DBSInt64>         Int64FieldValueOperand;
typedef TableValueOperand<RealOperand, DBSReal>           RealFieldValueOperand;
typedef TableValueOperand<RichRealOperand, DBSRichReal>   RichRealFieldValueOperand;
typedef TableValueOperand<ArrayOperand, DBSArray>         ArrayFieldValueOperand;
typedef TableValueOperand<TextOperand, DBSText>           TextFieldValueOperand;

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

  virtual FIELD_INDEX   GetField ();
  virtual I_DBSTable&   GetTable ();

private:
  StackValue& m_LocalValue;
};

}

#endif /* PM_OPERAND_H_ */

