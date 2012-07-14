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

#include "pm_operand.h"

#include "pm_typemanager.h"

using namespace std;
using namespace prima;

/////////////////////////////TableOperand//////////////////////////////////////

TableOperand::~TableOperand ()
{
  m_pRefTable->DecrementRefCount ();
}

bool
TableOperand::IsNull () const
{
  return (m_pRefTable->GetTable ().GetAllocatedRows () == 0);
}

I_DBSTable&
TableOperand::GetTable ()
{
  return m_pRefTable->GetTable ();
}

/////////////////////////FieldOperand//////////////////////////////////////////

FieldOperand::FieldOperand (TableOperand& tableOp, const FIELD_INDEX field)
  : I_Operand (),
    m_pRefTable (&tableOp.GetTableRef ()),
    m_Field (field)
{
  m_pRefTable->IncrementRefCount ();

  I_DBSTable&              table     = m_pRefTable->GetTable ();
  const DBSFieldDescriptor fieldDesc = table.GetFieldDescriptor (field);

  m_FieldType = fieldDesc.m_FieldType;
  assert ((m_FieldType > T_UNKNOWN) && (m_FieldType < T_UNDETERMINED));

  if (fieldDesc.isArray)
    {
      assert (m_FieldType != T_TEXT);
      MARK_ARRAY (m_FieldType);
    }
}

FieldOperand::FieldOperand (const FieldOperand& source)
  : m_pRefTable (source.m_pRefTable),
    m_Field (source.m_Field),
    m_FieldType (source.m_FieldType)
{
  if (m_pRefTable)
    m_pRefTable->IncrementRefCount ();
}

FieldOperand::~FieldOperand ()
{
  if (m_pRefTable)
    m_pRefTable->DecrementRefCount ();
}

const FieldOperand&
FieldOperand::operator= (const FieldOperand& source)
{
  if (this != &source)
    {
      if ((m_FieldType != T_UNDETERMINED) &&
          (m_FieldType != source.m_FieldType))
        {
          throw InterException (NULL,
                                _EXTRA (InterException::FIELD_TYPE_MISMATCH));
        }

      if (m_pRefTable != NULL)
        m_pRefTable->DecrementRefCount ();

      m_pRefTable = source.m_pRefTable;
      m_Field     = source.m_Field;

      if (m_pRefTable)
        m_pRefTable->IncrementRefCount ();
    }

  return *this;
}

bool
FieldOperand::IsNull () const
{
  return (m_pRefTable == NULL);
}

FIELD_INDEX
FieldOperand::GetField ()
{
  return m_Field;
}

I_DBSTable&
FieldOperand::GetTable ()
{

  return m_pRefTable->GetTable ();
}

#if 0
StackValue
FieldOperand::GetValueAt (const D_UINT64 index) const
{
  if (index >= m_pRefTable->GetTable ().GetAllocatedRows ())
    throw InterException (NULL, _EXTRA (InterException::INVALID_ROW_INDEX));

  if ((m_FieldType == T_UNKNOWN) ||
      (m_pRefTable == NULL))
    {
      throw InterException (NULL, _EXTRA (InterException::FIELD_TYPE_MISMATCH));
    }

  I_DBSTable& table = m_pRefTable->GetTable ();

  if (IS_ARRAY (m_FieldType))
    return StackValue (ArrayFieldElOperand (table, index, m_Field));

  switch (m_FieldType)
  {
  case T_BOOL:
    return StackValue (BoolFieldElOperand (table, index, m_Field));
  case T_CHAR:
    return StackValue (CharFieldElOperand (table, index, m_Field));
  case T_DATE:
    return StackValue (DateFieldElOperand (table, index, m_Field));
  case T_DATETIME:
    return StackValue (DateTimeFieldElOperand (table, index, m_Field));
  case T_HIRESTIME:
    return StackValue (HiresTimeFieldElOperand (table, index, m_Field));
  case T_UINT8:
    return StackValue (UInt8FieldElOperand (table, index, m_Field));
  case T_UINT16:
    return StackValue (UInt16FieldElOperand (table, index, m_Field));
  case T_UINT32:
    return StackValue (UInt16FieldElOperand (table, index, m_Field));
  case T_UINT64:
    return StackValue (UInt64FieldElOperand (table, index, m_Field));
  case T_INT8:
    return StackValue (Int8FieldElOperand (table, index, m_Field));
  case T_INT16:
    return StackValue (Int16FieldElOperand (table, index, m_Field));
  case T_INT32:
    return StackValue (Int16FieldElOperand (table, index, m_Field));
  case T_INT64:
    return StackValue (Int64FieldElOperand (table, index, m_Field));
  case T_REAL:
    return StackValue (RealFieldElOperand (table, index, m_Field));
  case T_RICHREAL:
    return StackValue (RichRealFieldElOperand (table, index, m_Field));
  case T_TEXT:
    return StackValue (TextFieldElOperand (table, index, m_Field));
  }

  throw InterException (NULL, _EXTRA (InterException::INTERNAL_ERROR));
}
#endif

//////////////////////////BoolFieldElOperand///////////////////////////////////

BoolFieldElOperand::~BoolFieldElOperand ()
{
}

bool
BoolFieldElOperand::IsNull () const
{
  DBSBool currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
BoolFieldElOperand::GetValue (DBSBool& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
BoolFieldElOperand::SetValue (const DBSBool& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
BoolFieldElOperand::SelfAnd (const DBSBool& value)
{
  DBSBool currValue;

  m_Table.GetEntry (m_Row, m_Field, currValue);
  currValue = internal_and (currValue, value);
  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
BoolFieldElOperand::SelfXor (const DBSBool& value)
{
  DBSBool currValue;

  m_Table.GetEntry (m_Row, m_Field, currValue);
  currValue = internal_xor (currValue, value);
  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
BoolFieldElOperand::SelfOr (const DBSBool& value)
{
  DBSBool currValue;

  m_Table.GetEntry (m_Row, m_Field, currValue);
  currValue = internal_or (currValue, value);
  m_Table.SetEntry (m_Row, m_Field, currValue);
}

//////////////////////////CharFieldElOperand///////////////////////////////////

CharFieldElOperand::~CharFieldElOperand ()
{
}

bool
CharFieldElOperand::IsNull () const
{
  DBSChar currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
CharFieldElOperand::GetValue (DBSChar& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);

}

void
CharFieldElOperand::GetValue (DBSText& outValue) const
{
  DBSChar currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  const DBSText result;
  outValue = result;

  outValue.Append (currValue);
}

void
CharFieldElOperand::SetValue (const DBSChar& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

//////////////////////////DateFieldElOperand///////////////////////////////////

DateFieldElOperand::~DateFieldElOperand ()
{
}

bool
DateFieldElOperand::IsNull () const
{
  DBSDate currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
DateFieldElOperand::GetValue (DBSDate& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);

}

void
DateFieldElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSDate currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  const DBSDateTime temp (currValue.m_Year,
                          currValue.m_Month,
                          currValue.m_Day,
                          0,
                          0,
                          0);
  outValue = temp;
}

void
DateFieldElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDate currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  const DBSHiresTime temp (currValue.m_Year,
                           currValue.m_Month,
                           currValue.m_Day,
                           0,
                           0,
                           0,
                           0);
  outValue = temp;
}

void
DateFieldElOperand::SetValue (const DBSDate& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

/////////////////////////DateTimeFieldElOperand////////////////////////////////

DateTimeFieldElOperand::~DateTimeFieldElOperand ()
{
}

bool
DateTimeFieldElOperand::IsNull () const
{
  DBSDateTime currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
DateTimeFieldElOperand::GetValue (DBSDate& outValue) const
{
  DBSDateTime currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  const DBSDate temp (currValue.m_Year,
                      currValue.m_Month,
                      currValue.m_Day);
  outValue = temp;
}

void
DateTimeFieldElOperand::GetValue (DBSDateTime& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);

}

void
DateTimeFieldElOperand::GetValue (DBSHiresTime& outValue) const
{
  DBSDateTime currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  const DBSHiresTime temp (currValue.m_Year,
                           currValue.m_Month,
                           currValue.m_Day,
                           currValue.m_Hour,
                           currValue.m_Minutes,
                           currValue.m_Seconds,
                           0);
  outValue = temp;
}

void
DateTimeFieldElOperand::SetValue (const DBSDateTime& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

///////////////////////HiresTimeFieldElOperand/////////////////////////////////

bool
HiresTimeFieldElOperand::IsNull () const
{
  DBSHiresTime currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

HiresTimeFieldElOperand::~HiresTimeFieldElOperand ()
{
}

void
HiresTimeFieldElOperand::GetValue (DBSDate& outValue) const
{
  DBSHiresTime currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  const DBSDate temp (currValue.m_Year,
                      currValue.m_Month,
                      currValue.m_Day);
  outValue = temp;
}

void
HiresTimeFieldElOperand::GetValue (DBSDateTime& outValue) const
{
  DBSHiresTime currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  const DBSDateTime temp (currValue.m_Year,
                          currValue.m_Month,
                          currValue.m_Day,
                          currValue.m_Hour,
                          currValue.m_Minutes,
                          currValue.m_Seconds);
  outValue = temp;
}

void
HiresTimeFieldElOperand::GetValue (DBSHiresTime& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
HiresTimeFieldElOperand::SetValue (const DBSHiresTime& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

//////////////////////////UInt8FieldElOperand//////////////////////////////////

UInt8FieldElOperand::~UInt8FieldElOperand ()
{
}

bool
UInt8FieldElOperand::IsNull () const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
UInt8FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
UInt8FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt8FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt8FieldElOperand::SetValue (const DBSUInt8& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
UInt8FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt8FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////UInt16FieldElOperand//////////////////////////////////

UInt16FieldElOperand::~UInt16FieldElOperand ()
{
}

bool
UInt16FieldElOperand::IsNull () const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
UInt16FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt16FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt16FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt16FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt16FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt16FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
UInt16FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt8 (currValue.m_Value);

}

void
UInt16FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
UInt16FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt16FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt16FieldElOperand::SetValue (const DBSUInt16& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
UInt16FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt16FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////UInt32FieldElOperand//////////////////////////////////

UInt32FieldElOperand::~UInt32FieldElOperand ()
{
}

bool
UInt32FieldElOperand::IsNull () const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
UInt32FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt8 (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt32FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
UInt32FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt64 (currValue.m_Value);
}

void
UInt32FieldElOperand::SetValue (const DBSUInt32& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
UInt32FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt32FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////UInt64FieldElOperand//////////////////////////////////

UInt64FieldElOperand::~UInt64FieldElOperand ()
{
}

bool
UInt64FieldElOperand::IsNull () const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
UInt64FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt8 (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt16 (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt32 (currValue.m_Value);
}

void
UInt64FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
UInt64FieldElOperand::SetValue (const DBSUInt64& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
UInt64FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
UInt64FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSUInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

//////////////////////////Int8FieldElOperand//////////////////////////////////

Int8FieldElOperand::~Int8FieldElOperand ()
{
}

bool
Int8FieldElOperand::IsNull () const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
Int8FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
Int8FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int8FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int8FieldElOperand::SetValue (const DBSInt8& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
Int8FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int8FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt8 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////Int16FieldElOperand//////////////////////////////////

Int16FieldElOperand::~Int16FieldElOperand ()
{
}

bool
Int16FieldElOperand::IsNull () const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
Int16FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
Int16FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
Int16FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
Int16FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
Int16FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
Int16FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
Int16FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt8 (currValue.m_Value);

}

void
Int16FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
Int16FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int16FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int16FieldElOperand::SetValue (const DBSInt16& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
Int16FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int16FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt16 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////Int32FieldElOperand//////////////////////////////////

Int32FieldElOperand::~Int32FieldElOperand ()
{
}

bool
Int32FieldElOperand::IsNull () const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
Int32FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int32FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
Int32FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt64 (currValue.m_Value);
}

void
Int32FieldElOperand::SetValue (const DBSInt32& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
Int32FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int32FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt32 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////Int64FieldElOperand//////////////////////////////////

Int64FieldElOperand::~Int64FieldElOperand ()
{
}

bool
Int64FieldElOperand::IsNull () const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
Int64FieldElOperand::GetValue (DBSInt8& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt8 (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSInt16& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt16 (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSInt32& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt32 (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSInt64& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSInt64 (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSUInt8& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt8 (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSUInt16& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt16 (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSUInt32& outValue) const
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSUInt32 (currValue.m_Value);
}

void
Int64FieldElOperand::GetValue (DBSUInt64& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
Int64FieldElOperand::SetValue (const DBSInt64& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
Int64FieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfMod (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mod (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfAnd (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_and (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfXor (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_xor (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
Int64FieldElOperand::SelfOr (const DBSInt64& value)
{
  DBSInt64 currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_or (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////RealFieldElOperand//////////////////////////////////

RealFieldElOperand::~RealFieldElOperand ()
{
}

bool
RealFieldElOperand::IsNull () const
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
RealFieldElOperand::GetValue (DBSReal& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
RealFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSRichReal (currValue.m_Value);
}

void
RealFieldElOperand::SetValue (const DBSReal& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
RealFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RealFieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RealFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RealFieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RealFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RealFieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RealFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RealFieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////RichRealFieldElOperand////////////////////////////////

RichRealFieldElOperand::~RichRealFieldElOperand ()
{
}

bool
RichRealFieldElOperand::IsNull () const
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
RichRealFieldElOperand::GetValue (DBSReal& outValue) const
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  outValue = DBSReal (currValue.m_Value);
}

void
RichRealFieldElOperand::GetValue (DBSRichReal& outValue) const
{
  m_Table.GetEntry (m_Row, m_Field, outValue);
}

void
RichRealFieldElOperand::SetValue (const DBSRichReal& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
RichRealFieldElOperand::SelfAdd (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RichRealFieldElOperand::SelfAdd (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_add (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RichRealFieldElOperand::SelfSub (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RichRealFieldElOperand::SelfSub (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_sub (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RichRealFieldElOperand::SelfMul (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RichRealFieldElOperand::SelfMul (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_mul (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RichRealFieldElOperand::SelfDiv (const DBSInt64& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
RichRealFieldElOperand::SelfDiv (const DBSRichReal& value)
{
  DBSRichReal currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = internal_div (currValue, value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

/////////////////////////TextFieldElOperand////////////////////////////////

TextFieldElOperand::~TextFieldElOperand ()
{
}

bool
TextFieldElOperand::IsNull () const
{
  DBSText currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
TextFieldElOperand::GetValue (DBSText& outValue) const
{
  DBSText currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = outValue;
}

void
TextFieldElOperand::SetValue (const DBSText& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}

void
TextFieldElOperand::SelfAdd (const DBSChar& value)
{
  DBSText currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue.Append (value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

void
TextFieldElOperand::SelfAdd (const DBSText& value)
{
  DBSText currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue.Append (value);

  m_Table.SetEntry (m_Row, m_Field, currValue);
}

///////////////////////////ArrayFieldElOperand//////////////////////////////////

ArrayFieldElOperand::~ArrayFieldElOperand ()
{
}

bool
ArrayFieldElOperand::IsNull () const
{
  DBSArray currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  return currValue.IsNull ();
}

void
ArrayFieldElOperand::GetValue (DBSArray& outValue) const
{
  DBSArray currValue;
  m_Table.GetEntry (m_Row, m_Field, currValue);

  currValue = outValue;
}

void
ArrayFieldElOperand::SetValue (const DBSArray& value)
{
  m_Table.SetEntry (m_Row, m_Field, value);
}
