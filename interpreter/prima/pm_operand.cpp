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

////////////////////////////I_Operand//////////////////////////////

bool
I_Operand::IsNull () const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSBool& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSChar& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSDate& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSDateTime& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSHiresTime& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSInt8& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSInt16& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSInt32& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSInt64& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSReal& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSRichReal& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSUInt8& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSUInt16& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSUInt32& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::GetValue (DBSUInt64& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSBool& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSChar& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSDate& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSDateTime& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSHiresTime& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSInt8& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSInt16& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSInt32& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSInt64& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSReal& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSRichReal& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSUInt8& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSUInt16& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSUInt32& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
I_Operand::SetValue (const DBSUInt64& outValue)
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}


FIELD_INDEX
I_Operand::GetField ()
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

DBSArray&
I_Operand::GetArray ()
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

DBSText&
I_Operand::GetText ()
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

I_DBSTable&
I_Operand::GetTable ()
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

//////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
static void assign_null (T& output)
{
  output = T();
}

NullOperand::~NullOperand ()
{
}

bool
NullOperand::IsNull () const
{
  return true;
}

void
NullOperand::GetValue (DBSBool& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSChar& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSDate& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSDateTime& outValue) const
{
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_REQUEST));
}

void
NullOperand::GetValue (DBSHiresTime& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSInt8& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSInt16& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSInt32& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSInt64& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSReal& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSRichReal& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSUInt8& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSUInt16& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSUInt32& outValue) const
{
  assign_null (outValue);
}

void
NullOperand::GetValue (DBSUInt64& outValue) const
{
  assign_null (outValue);
}


////////////////////BoolOperand///////////////////////////////////

BoolOperand::~BoolOperand ()
{
}

bool
BoolOperand::IsNull () const
{
  return m_Value.IsNull ();
}

void
BoolOperand::GetValue (DBSBool& outValue) const
{
  outValue = m_Value;
}

void
BoolOperand::SetValue (const DBSBool& value)
{
  m_Value = value;
}

////////////////////CharOperand////////////////////////////////////

CharOperand::~CharOperand ()
{
}

bool
CharOperand::IsNull () const
{
  return m_Value.IsNull ();
}

void
CharOperand::GetValue (DBSChar& outValue) const
{
  outValue = m_Value;
}

void
CharOperand::GetValue (DBSText& outValue) const
{
  DBSText text;

  text.Append (m_Value);

  outValue = text;
}

void
CharOperand::SetValue (const DBSChar& value)
{
  m_Value = value;
}

///////////////////////DateOperand///////////////////////////////////

DateOperand::~DateOperand ()
{
}

bool
DateOperand::IsNull () const
{
  return m_Value.IsNull ();
}

void
DateOperand::GetValue (DBSDate& outValue) const
{
  outValue = m_Value;
}

void
DateOperand::GetValue (DBSDateTime& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSDateTime temp;
      outValue = temp;
    }
  else
    {
      const DBSDateTime temp (m_Value.m_Year,
                              m_Value.m_Month,
                              m_Value.m_Day,
                              0,
                              0,
                              0);
      outValue = temp;
    }
}

void
DateOperand::GetValue (DBSHiresTime& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSHiresTime temp;
      outValue = temp;
    }
  else
    {
      const DBSHiresTime temp (m_Value.m_Year,
                              m_Value.m_Month,
                              m_Value.m_Day,
                              0,
                              0,
                              0,
                              0);
      outValue = temp;
    }
}

void
DateOperand::SetValue (const DBSDate& value)
{
  m_Value = value;
}

////////////////////////////////DateTimeOperand/////////////////////////////

DateTimeOperand::~DateTimeOperand ()
{
}

bool
DateTimeOperand::IsNull () const
{
  return m_Value.IsNull ();
}

void
DateTimeOperand::GetValue (DBSDate& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSDate temp;
      outValue = temp;
    }
  else
    {
      const DBSDate temp (m_Value.m_Year,
                          m_Value.m_Month,
                          m_Value.m_Day);
      outValue = temp;
    }
}

void
DateTimeOperand::GetValue (DBSDateTime& outValue) const
{
  outValue = m_Value;
}

void
DateTimeOperand::GetValue (DBSHiresTime& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSHiresTime temp;
      outValue = temp;
    }
  else
    {
      const DBSHiresTime temp (m_Value.m_Year,
                              m_Value.m_Month,
                              m_Value.m_Day,
                              m_Value.m_Hour,
                              m_Value.m_Minutes,
                              m_Value.m_Seconds,
                              0);
      outValue = temp;
    }
}

void
DateTimeOperand::SetValue (const DBSDateTime& value)
{
  m_Value = value;
}

///////////////////////////HiresTimeOperand/////////////////////////////////

bool
HiresTimeOperand::IsNull () const
{
  return m_Value.IsNull ();
}

HiresTimeOperand::~HiresTimeOperand ()
{
}

void
HiresTimeOperand::GetValue (DBSDate& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSDate temp;
      outValue = temp;
    }
  else
    {
      const DBSDate temp (m_Value.m_Year,
                          m_Value.m_Month,
                          m_Value.m_Day);
      outValue = temp;
    }
}

void
HiresTimeOperand::GetValue (DBSDateTime& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSDateTime temp;
      outValue = temp;
    }
  else
    {
      const DBSDateTime temp (m_Value.m_Year,
                              m_Value.m_Month,
                              m_Value.m_Day,
                              m_Value.m_Hour,
                              m_Value.m_Minutes,
                              m_Value.m_Seconds);
      outValue = temp;
    }
}

void
HiresTimeOperand::GetValue (DBSHiresTime& outValue) const
{
  outValue = m_Value;
}

void
HiresTimeOperand::SetValue (const DBSHiresTime& value)
{
  m_Value = value;
}

////////////////////////////UInt8Operand//////////////////////////////////

bool
UInt8Operand::IsNull () const
{
  return m_Value.IsNull ();
}

UInt8Operand::~UInt8Operand ()
{
}

void
UInt8Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
UInt8Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
UInt8Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
UInt8Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
UInt8Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt8Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt8Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
UInt8Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
UInt8Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
UInt8Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
UInt8Operand::SetValue (const DBSUInt8& value)
{
  m_Value = value;
}


////////////////////////////UInt16Operand//////////////////////////////////

UInt16Operand::~UInt16Operand ()
{
}

bool
UInt16Operand::IsNull () const
{
  return m_Value.IsNull ();
}


void
UInt16Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
UInt16Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
UInt16Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
UInt16Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
UInt16Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt16Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt16Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
UInt16Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
UInt16Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
UInt16Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
UInt16Operand::SetValue (const DBSUInt16& value)
{
  m_Value = value;
}


////////////////////////////UInt32Operand//////////////////////////////////

UInt32Operand::~UInt32Operand ()
{
}

bool
UInt32Operand::IsNull () const
{
  return m_Value.IsNull ();
}


void
UInt32Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
UInt32Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
UInt32Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
UInt32Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
UInt32Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt32Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt32Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
UInt32Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
UInt32Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
UInt32Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
UInt32Operand::SetValue (const DBSUInt32& value)
{
  m_Value = value;
}

////////////////////////////UInt64Operand//////////////////////////////////

UInt64Operand::~UInt64Operand ()
{
}

bool
UInt64Operand::IsNull () const
{
  return m_Value.IsNull ();
}


void
UInt64Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
UInt64Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
UInt64Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
UInt64Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
UInt64Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt64Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
UInt64Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
UInt64Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
UInt64Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
UInt64Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
UInt64Operand::SetValue (const DBSUInt64& value)
{
  m_Value = value;
}

///////////////////////////Int8Operand//////////////////////////////////

Int8Operand::~Int8Operand ()
{
}

bool
Int8Operand::IsNull () const
{
  return m_Value.IsNull ();
}

void
Int8Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
Int8Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
Int8Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
Int8Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
Int8Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int8Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int8Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
Int8Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
Int8Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
Int8Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
Int8Operand::SetValue (const DBSInt8& value)
{
  m_Value = value;
}

////////////////////////////Int16Operand//////////////////////////////////

Int16Operand::~Int16Operand ()
{
}

bool
Int16Operand::IsNull () const
{
  return m_Value.IsNull ();
}

void
Int16Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
Int16Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
Int16Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
Int16Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
Int16Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int16Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int16Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
Int16Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
Int16Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
Int16Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
Int16Operand::SetValue (const DBSInt16& value)
{
  m_Value = value;
}

////////////////////////////Int32Operand//////////////////////////////////

Int32Operand::~Int32Operand ()
{
}

bool
Int32Operand::IsNull () const
{
  return m_Value.IsNull ();
}

void
Int32Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
Int32Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
Int32Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
Int32Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
Int32Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int32Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int32Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
Int32Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
Int32Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
Int32Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
Int32Operand::SetValue (const DBSInt32& value)
{
  m_Value = value;
}

////////////////////////////Int64Operand//////////////////////////////////

Int64Operand::~Int64Operand ()
{
}

bool
Int64Operand::IsNull () const
{
  return m_Value.IsNull ();
}

void
Int64Operand::GetValue (DBSInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT8&, outValue.m_Value)  = _SC (D_INT8, m_Value.m_Value);
}

void
Int64Operand::GetValue (DBSInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT16&, outValue.m_Value) = _SC (D_INT16, m_Value.m_Value);
}

void
Int64Operand::GetValue (DBSInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT32&, outValue.m_Value) = _SC (D_INT32, m_Value.m_Value);
}

void
Int64Operand::GetValue (DBSInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)   = m_Value.m_IsNull;
  _CC (D_INT64&, outValue.m_Value) = _SC (D_INT64, m_Value.m_Value);
}

void
Int64Operand::GetValue (DBSRichReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSRichReal temp;
      outValue = temp;
    }
  else
    {
      const DBSRichReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int64Operand::GetValue (DBSReal& outValue) const
{
  if (m_Value.IsNull ())
    {
      const DBSReal temp;
      outValue = temp;
    }
  else
    {
      const DBSReal temp (m_Value.m_Value);
      outValue = temp;
    }
}

void
Int64Operand::GetValue (DBSUInt8& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT8&, outValue.m_Value)  = _SC (D_UINT8, m_Value.m_Value);
}

void
Int64Operand::GetValue (DBSUInt16& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT16&, outValue.m_Value) = _SC (D_UINT16, m_Value.m_Value);
}

void
Int64Operand::GetValue (DBSUInt32& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT32&, outValue.m_Value) = _SC (D_UINT32, m_Value.m_Value);
}

void
Int64Operand::GetValue (DBSUInt64& outValue) const
{
  _CC (bool&, outValue.m_IsNull)    = m_Value.m_IsNull;
  _CC (D_UINT64&, outValue.m_Value) = _SC (D_UINT64, m_Value.m_Value);
}

void
Int64Operand::SetValue (const DBSInt64& value)
{
  m_Value = value;
}

/////////////////////RealOperand///////////////////////////////////

RealOperand::~RealOperand ()
{
}

bool
RealOperand::IsNull () const
{
  return m_Value.IsNull ();
}

void
RealOperand::GetValue (DBSReal& outValue) const
{
  outValue = m_Value;
}

void
RealOperand::GetValue (DBSRichReal& outValue) const
{
  _CC (bool&, outValue.m_IsNull)      = m_Value.m_IsNull;
  _CC (RICHREAL_T&, outValue.m_Value) = _SC (RICHREAL_T, m_Value.m_Value);
}

void
RealOperand::SetValue (const DBSReal& value)
{
  m_Value = value;
}


////////////////////RichRealOperand/////////////////////////////////

RichRealOperand::~RichRealOperand ()
{
}

bool
RichRealOperand::IsNull () const
{
  return m_Value.IsNull ();
}

void
RichRealOperand::GetValue (DBSReal& outValue) const
{
  _CC (bool&, outValue.m_IsNull) = m_Value.m_IsNull;
  _CC (REAL_T&, outValue.m_Value) = _SC (REAL_T, m_Value.m_Value);
}

void
RichRealOperand::GetValue (DBSRichReal& outValue) const
{
  outValue = m_Value;
}

void
RichRealOperand::SetValue (const DBSRichReal& value)
{
  m_Value = value;
}
////////////////////TextOperand//////////////////////////////////////

TextOperand::~TextOperand ()
{
}

bool
TextOperand::IsNull () const
{
  return m_Value.IsNull ();
}

DBSText&
TextOperand::GetText ()
{
  return m_Value;
}


//////////////////////CharTextOperand////////////////////////////////
CharTextOperand::~CharTextOperand ()
{
}

bool
CharTextOperand::IsNull () const
{
  //A text could not hold NULL characters!
  return false;
}

void
CharTextOperand::GetValue (DBSChar& outValue) const
{
  outValue = m_Text.GetCharAtIndex (m_Index);
}

void
CharTextOperand::GetValue (DBSText& outValue) const
{
  DBSChar ch = m_Text.GetCharAtIndex (m_Index);
  DBSText text;

  text.Append (ch);
  outValue = text;
}

void
CharTextOperand::SetValue (const DBSChar& value)
{
  m_Text.SetCharAtIndex (value, m_Index);
}


//////////////////////ArrayOperand///////////////////////////////////

ArrayOperand::~ArrayOperand ()
{
}

bool
ArrayOperand::IsNull () const
{
  return m_Value.IsNull ();
}

DBSArray&
ArrayOperand::GetArray ()
{
  return m_Value;
}

//////////////////////TableOperand//////////////////////////////////////
TableOperand::~TableOperand ()
{
  m_DbsHandler.ReleaseTable (m_Table);
}

bool
TableOperand::IsNull () const
{
  return (m_Table.GetAllocatedRows () == 0);
}

I_DBSTable&
TableOperand::GetTable ()
{
  return m_Table;
}

////////////////////FieldOperand//////////////////////////////////////////

FieldOperand::~FieldOperand ()
{
}


bool
FieldOperand::IsNull () const
{
  return (m_pTable == NULL);
}

FIELD_INDEX
FieldOperand::GetField ()
{
  return m_Field;
}

I_DBSTable&
FieldOperand::GetTable ()
{
  assert (m_pTable != NULL);

  return *m_pTable;
}

//////////////////GlobalOperand///////////////////////////////////////////

GlobalOperand::GlobalOperand (GlobalValue& value) :
    I_Operand (),
    m_Value (value)
{
}

GlobalOperand::~GlobalOperand ()
{
}


void
GlobalOperand::GetValue (DBSBool& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSChar& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSDate& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSDateTime& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSHiresTime& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSInt8& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSInt16& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSInt32& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSInt64& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSReal& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSRichReal& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSUInt8& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSUInt16& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSUInt32& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::GetValue (DBSUInt64& outValue) const
{
  m_Value.GetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSBool& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSChar& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSDate& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSDateTime& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSHiresTime& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSInt8& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSInt16& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSInt32& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSInt64& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSReal& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSRichReal& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSUInt8& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSUInt16& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSUInt32& outValue)
{
  m_Value.SetValue (outValue);
}

void
GlobalOperand::SetValue (const DBSUInt64& outValue)
{
  m_Value.SetValue (outValue);
}

FIELD_INDEX
GlobalOperand::GetField ()
{
  return m_Value.GetField ();
}

DBSArray&
GlobalOperand::GetArray ()
{
  return m_Value.GetArray ();
}

DBSText&
GlobalOperand::GetText ()
{
  return m_Value.GetText ();
}

I_DBSTable&
GlobalOperand::GetTable ()
{
  return m_Value.GetTable ();
}

