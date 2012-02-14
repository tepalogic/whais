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

using namespace std;
using namespace prima;

template <class T> static inline T
get_op_not_supp ()
{
  assert (false);
  throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_OP_CONVERSION));
}

template <class T, class DBSType> static inline T
get_op_null ()
{
  return T (DBSType ());
}


////////////////////////////I_Operand//////////////////////////////

I_Operand::~I_Operand ()
{
}

NullOperand
I_Operand::GetNullOperand ()
{
  return get_op_not_supp<NullOperand> ();
}

BoolOperand
I_Operand::GetBoolOperand ()
{
  return get_op_not_supp<BoolOperand> ();
}

CharOperand
I_Operand::GetCharOperand ()
{
  return get_op_not_supp<CharOperand> ();
}

DateOperand
I_Operand::GetDateOperand ()
{
  return get_op_not_supp<DateOperand> ();
}

DateTimeOperand
I_Operand::GetDateTimeOperand ()
{
  return get_op_not_supp<DateTimeOperand> ();
}

HiresTimeOperand
I_Operand::GetHiresTimeOperand ()
{
  return get_op_not_supp<HiresTimeOperand> ();
}

UInt8Operand
I_Operand::GetUInt8Operand ()
{
  return get_op_not_supp<UInt8Operand> ();
}

UInt16Operand
I_Operand::GetUInt16Operand ()
{
  return get_op_not_supp<UInt16Operand> ();
}

UInt32Operand
I_Operand::GetUInt32Operand ()
{
  return get_op_not_supp<UInt32Operand> ();
}

UInt64Operand
I_Operand::GetUInt64Operand ()
{
  return get_op_not_supp<UInt64Operand> ();
}

Int8Operand
I_Operand::GetInt8Operand ()
{
  return get_op_not_supp<Int8Operand> ();
}

Int16Operand
I_Operand::GetInt16Operand ()
{
  return get_op_not_supp<Int16Operand> ();
}

Int32Operand
I_Operand::GetInt32Operand ()
{
  return get_op_not_supp<Int32Operand> ();
}

Int64Operand
I_Operand::GetInt64Operand ()
{
  return get_op_not_supp<Int64Operand> ();
}

RealOperand
I_Operand::GetRealOperand ()
{
  return get_op_not_supp<RealOperand> ();
}

RichRealOperand
I_Operand::GetRichRealOperand ()
{
  return get_op_not_supp<RichRealOperand> ();
}

TextOperand
I_Operand::GetTextOperand ()
{
  return get_op_not_supp<TextOperand> ();
}



////////////////StackOperand////////////////////////////////////

StackedOperand::~StackedOperand ()
{
  I_Operand& operand = *_RC (I_Operand*, m_OperandPlacement);
  operand.~I_Operand ();
}

NullOperand
StackedOperand::GetNullOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetNullOperand ();
}

BoolOperand
StackedOperand::GetBoolOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetBoolOperand ();
}

CharOperand
StackedOperand::GetCharOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetCharOperand ();
}

DateOperand
StackedOperand::GetDateOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetDateOperand ();
}

DateTimeOperand
StackedOperand::GetDateTimeOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetDateTimeOperand ();
}

HiresTimeOperand
StackedOperand::GetHiresTimeOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetHiresTimeOperand ();
}

UInt8Operand
StackedOperand::GetUInt8Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetUInt8Operand ();
}

UInt16Operand
StackedOperand::GetUInt16Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetUInt16Operand ();
}

UInt32Operand
StackedOperand::GetUInt32Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetUInt32Operand ();
}

UInt64Operand
StackedOperand::GetUInt64Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetUInt64Operand ();
}

Int8Operand
StackedOperand::GetInt8Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetInt8Operand ();
}

Int16Operand
StackedOperand::GetInt16Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetInt16Operand ();
}

Int32Operand
StackedOperand::GetInt32Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetInt32Operand ();
}

Int64Operand
StackedOperand::GetInt64Operand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetInt64Operand ();
}

RealOperand
StackedOperand::GetRealOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetRealOperand ();
}

RichRealOperand
StackedOperand::GetRichRealOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetRichRealOperand ();
}

TextOperand
StackedOperand::GetTextOperand ()
{
  I_Operand& operand = *_RC(I_Operand*, m_OperandPlacement);
  return operand.GetTextOperand ();
}


///////////////////////NullOperand////////////////////////////////
NullOperand::~NullOperand ()
{
}

NullOperand
NullOperand::GetNullOperand ()
{
  return *this;
}

BoolOperand
NullOperand::GetBoolOperand ()
{
  return get_op_null<BoolOperand, DBSBool> ();
}

CharOperand
NullOperand::GetCharOperand ()
{
  return get_op_null<CharOperand, DBSChar> ();
}

DateOperand
NullOperand::GetDateOperand ()
{
  return get_op_null<DateOperand, DBSDate> ();
}

DateTimeOperand
NullOperand::GetDateTimeOperand ()
{
  return get_op_null<DateTimeOperand, DBSDateTime> ();
}

HiresTimeOperand
NullOperand::GetHiresTimeOperand ()
{
  return get_op_null<HiresTimeOperand, DBSHiresTime> ();
}

UInt8Operand
NullOperand::GetUInt8Operand ()
{
  return get_op_null<UInt8Operand, DBSUInt8> ();
}

UInt16Operand
NullOperand::GetUInt16Operand ()
{
  return get_op_null<UInt16Operand, DBSUInt16> ();
}

UInt32Operand
NullOperand::GetUInt32Operand ()
{
  return get_op_null<UInt32Operand, DBSUInt32> ();
}

UInt64Operand
NullOperand::GetUInt64Operand ()
{
  return get_op_null<UInt64Operand, DBSUInt64> ();
}

Int8Operand
NullOperand::GetInt8Operand ()
{
  return get_op_null<Int8Operand, DBSInt8> ();
}

Int16Operand
NullOperand::GetInt16Operand ()
{
  return get_op_null<Int16Operand, DBSInt16> ();
}

Int32Operand
NullOperand::GetInt32Operand ()
{
  return get_op_null<Int32Operand, DBSInt32> ();
}

Int64Operand
NullOperand::GetInt64Operand ()
{
  return get_op_null<Int64Operand, DBSInt64> ();
}

RealOperand
NullOperand::GetRealOperand ()
{
  return get_op_null<RealOperand, DBSReal> ();
}

RichRealOperand
NullOperand::GetRichRealOperand ()
{
  return get_op_null<RichRealOperand, DBSRichReal> ();
}

TextOperand
NullOperand::GetTextOperand ()
{
  return get_op_null<TextOperand, DBSText> ();
}


////////////////////BoolOperand///////////////////////////////////

BoolOperand::~BoolOperand ()
{
}

BoolOperand
BoolOperand::GetBoolOperand ()
{
  return *this;
}

////////////////////CharOperand////////////////////////////////////

CharOperand::~CharOperand ()
{
}

CharOperand
CharOperand::GetCharOperand ()
{
  return *this;
}

TextOperand
CharOperand::GetTextOperand ()
{
  DBSText text;

  text.Append (*this);

  return TextOperand (text);
}

///////////////////////DateOperand///////////////////////////////////

DateOperand::~DateOperand ()
{
}

DateOperand
DateOperand::GetDateOperand ()
{
  return *this;
}

DateTimeOperand
DateOperand::GetDateTimeOperand ()
{
  DBSDateTime time;

  //Avoid validy checks
  _CC (D_INT16&, time.m_Year)    = this->m_Year;
  _CC (D_UINT8&, time.m_Month)   = this->m_Month;
  _CC (D_UINT8&, time.m_Day)     = this->m_Day;
  _CC (D_UINT8&, time.m_Hour)    = 0;
  _CC (D_UINT8&, time.m_Minutes) = 0;
  _CC (D_UINT8&, time.m_Seconds) = 0;
  _CC (bool&,    time.m_IsNull)  = false;

  return DateTimeOperand (time);
}

HiresTimeOperand
DateOperand::GetHiresTimeOperand ()
{
  DBSHiresTime time;

  //Avoid validy checks

  _CC (D_INT16&,  time.m_Year)     = this->m_Year;
  _CC (D_UINT8&,  time.m_Month)    = this->m_Month;
  _CC (D_UINT8&,  time.m_Day)      = this->m_Day;
  _CC (D_UINT8&,  time.m_Hour)     = 0;
  _CC (D_UINT8&,  time.m_Minutes)  = 0;
  _CC (D_UINT8&,  time.m_Seconds)  = 0;
  _CC (D_UINT32&, time.m_Microsec) = 0;
  _CC (bool&,     time.m_IsNull)   = false;

  return HiresTimeOperand (time);
}

////////////////////////////////DateTimeOperand/////////////////////////////

DateTimeOperand::~DateTimeOperand ()
{
}

DateOperand
DateTimeOperand::GetDateOperand ()
{
  DBSDate date;

  //Avoid validy checks
  _CC (D_INT16&, date.m_Year)    = this->m_Year;
  _CC (D_UINT8&, date.m_Month)   = this->m_Month;
  _CC (D_UINT8&, date.m_Day)     = this->m_Day;
  _CC (bool&,    date.m_IsNull)  = false;

  return DateOperand (date);
}

DateTimeOperand
DateTimeOperand::GetDateTimeOperand ()
{
  return *this;
}

HiresTimeOperand
DateTimeOperand::GetHiresTimeOperand ()
{
  DBSHiresTime time;

  //Avoid validy checks

  _CC (D_INT16&,  time.m_Year)     = this->m_Year;
  _CC (D_UINT8&,  time.m_Month)    = this->m_Month;
  _CC (D_UINT8&,  time.m_Day)      = this->m_Day;
  _CC (D_UINT8&,  time.m_Hour)     = this->m_Hour;
  _CC (D_UINT8&,  time.m_Minutes)  = this->m_Minutes;
  _CC (D_UINT8&,  time.m_Seconds)  = this->m_Seconds;
  _CC (D_UINT32&, time.m_Microsec) = 0;
  _CC (bool&,     time.m_IsNull)   = false;

  return HiresTimeOperand (time);
}

///////////////////////////HiresTimeOperand/////////////////////////////////

HiresTimeOperand::~HiresTimeOperand ()
{
}

DateOperand
HiresTimeOperand::GetDateOperand ()
{
  DBSDate date;

  //Avoid validy checks
  _CC (D_INT16&, date.m_Year)    = this->m_Year;
  _CC (D_UINT8&, date.m_Month)   = this->m_Month;
  _CC (D_UINT8&, date.m_Day)     = this->m_Day;
  _CC (bool&,    date.m_IsNull)  = false;

  return DateOperand (date);
}

DateTimeOperand
HiresTimeOperand::GetDateTimeOperand ()
{
  DBSDateTime time;

  //Avoid validy checks

  _CC (D_INT16&,  time.m_Year)    = this->m_Year;
  _CC (D_UINT8&,  time.m_Month)   = this->m_Month;
  _CC (D_UINT8&,  time.m_Day)     = this->m_Day;
  _CC (D_UINT8&,  time.m_Hour)    = this->m_Hour;
  _CC (D_UINT8&,  time.m_Minutes) = this->m_Minutes;
  _CC (D_UINT8&,  time.m_Seconds) = this->m_Seconds;
  _CC (bool&,     time.m_IsNull)  = false;

  return DateTimeOperand (time);
}

HiresTimeOperand
HiresTimeOperand::GetHiresTimeOperand ()
{
  return *this;
}

////////////////////////////UInt8Operand//////////////////////////////////

UInt8Operand::~UInt8Operand ()
{
}

UInt8Operand
UInt8Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
UInt8Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
UInt8Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
UInt8Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
UInt8Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
UInt8Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
UInt8Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
UInt8Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
UInt8Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
UInt8Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}


////////////////////////////UInt16Operand//////////////////////////////////

UInt16Operand::~UInt16Operand ()
{
}

UInt8Operand
UInt16Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
UInt16Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
UInt16Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
UInt16Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
UInt16Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
UInt16Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
UInt16Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
UInt16Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
UInt16Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
UInt16Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}


////////////////////////////UInt32Operand//////////////////////////////////

UInt32Operand::~UInt32Operand ()
{
}

UInt8Operand
UInt32Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
UInt32Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
UInt32Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
UInt32Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
UInt32Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
UInt32Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
UInt32Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
UInt32Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
UInt32Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
UInt32Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}


////////////////////////////UInt64Operand//////////////////////////////////

UInt64Operand::~UInt64Operand ()
{
}

UInt8Operand
UInt64Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
UInt64Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
UInt64Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
UInt64Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
UInt64Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
UInt64Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
UInt64Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
UInt64Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
UInt64Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
UInt64Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}

////////////////////////////Int8Operand//////////////////////////////////

Int8Operand::~Int8Operand ()
{
}

UInt8Operand
Int8Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
Int8Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
Int8Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
Int8Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
Int8Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
Int8Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
Int8Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
Int8Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
Int8Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
Int8Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}

////////////////////////////Int16Operand//////////////////////////////////

Int16Operand::~Int16Operand ()
{
}

UInt8Operand
Int16Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
Int16Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
Int16Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
Int16Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
Int16Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
Int16Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
Int16Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
Int16Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
Int16Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
Int16Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}


////////////////////////////Int32Operand//////////////////////////////////

Int32Operand::~Int32Operand ()
{
}

UInt8Operand
Int32Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
Int32Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
Int32Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
Int32Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
Int32Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
Int32Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
Int32Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
Int32Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
Int32Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
Int32Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}

////////////////////////////Int64Operand//////////////////////////////////

Int64Operand::~Int64Operand ()
{
}

UInt8Operand
Int64Operand::GetUInt8Operand ()
{
  return UInt8Operand (DBSUInt8 (_SC (D_UINT8, this->m_Value)));
}

UInt16Operand
Int64Operand::GetUInt16Operand ()
{
  return UInt16Operand (DBSUInt16 (_SC (D_UINT16, this->m_Value)));
}

UInt32Operand
Int64Operand::GetUInt32Operand ()
{
  return UInt32Operand (DBSUInt32 (_SC (D_UINT32, this->m_Value)));
}

UInt64Operand
Int64Operand::GetUInt64Operand ()
{
  return UInt64Operand (DBSUInt64 (_SC (D_UINT64, this->m_Value)));
}

Int8Operand
Int64Operand::GetInt8Operand ()
{
  return Int8Operand (DBSInt8 (_SC (D_INT8, this->m_Value)));
}

Int16Operand
Int64Operand::GetInt16Operand ()
{
  return Int16Operand (DBSInt16 (_SC (D_INT16, this->m_Value)));
}

Int32Operand
Int64Operand::GetInt32Operand ()
{
  return Int32Operand (DBSInt32 (_SC (D_INT32, this->m_Value)));
}

Int64Operand
Int64Operand::GetInt64Operand ()
{
  return Int64Operand (DBSInt64 (_SC (D_INT64, this->m_Value)));
}

RealOperand
Int64Operand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
Int64Operand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}

/////////////////////RealOperand///////////////////////////////////

RealOperand::~RealOperand ()
{
}

RealOperand
RealOperand::GetRealOperand ()
{
  return *this;
}

RichRealOperand
RealOperand::GetRichRealOperand ()
{
  return RichRealOperand (DBSRichReal (_SC (long double, this->m_Value)));
}

////////////////////RichRealOperand/////////////////////////////////

RichRealOperand::~RichRealOperand ()
{
}

RealOperand
RichRealOperand::GetRealOperand ()
{
  return RealOperand (DBSReal (_SC (float, this->m_Value)));
}

RichRealOperand
RichRealOperand::GetRichRealOperand ()
{
  return *this;
}

////////////////////TextOperand//////////////////////////////////////

TextOperand::~TextOperand ()
{
}

TextOperand
TextOperand::GetTextOperand ()
{
  return *this;
}
