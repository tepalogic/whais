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



namespace whais {
namespace prima {



bool
CharTextFieldElOperand::IsNull( ) const
{
  return false;
}


void
CharTextFieldElOperand::GetValue( DChar& outValue) const
{
  ITable& table = mTableRef->GetTable( );

  DText text;

  table.Get (mRow, mField, text);

  outValue = text.CharAt( mIndex);

  assert( outValue.IsNull( ) == false);
}


void
CharTextFieldElOperand::GetValue( DText& outValue) const
{
  DChar ch;

  this->GetValue( ch);

  outValue = DText( );
  outValue.Append( ch);
}


void
CharTextFieldElOperand::SetValue( const DChar& value)
{
  ITable& table = mTableRef->GetTable( );

  DText text;

  table.Get (mRow, mField, text);

  text.CharAt( mIndex, value);
  table.Set (mRow, mField, text);
}


uint_t
CharTextFieldElOperand::GetType( )
{
  return T_CHAR;
}


StackValue
CharTextFieldElOperand::Duplicate( ) const
{
  DChar ch;

  Get (ch);

  return StackValue( CharOperand( ch));
}



BaseArrayFieldElOperand::~BaseArrayFieldElOperand( )
{
  mTableRef->DecrementRefCount( );
}


void
BaseArrayFieldElOperand::NotifyCopy( )
{
  mTableRef->IncrementRefCount( );
}



bool
BoolArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
BoolArrayFieldElOperand::GetValue( DBool& outValue) const
{
  Get (outValue);
}


void
BoolArrayFieldElOperand::SetValue( const DBool& value)
{
  Set (value);
}


void
BoolArrayFieldElOperand::SelfAnd( const DBool& value)
{
  DBool currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
BoolArrayFieldElOperand::SelfXor( const DBool& value)
{
  DBool currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
BoolArrayFieldElOperand::SelfOr( const DBool& value)
{
  DBool currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
BoolArrayFieldElOperand::GetType( )
{
  return T_BOOL;
}


StackValue
BoolArrayFieldElOperand::Duplicate( ) const
{
  DBool value;

  Get (value);

  return StackValue( BoolOperand( value));
}



bool
CharArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
CharArrayFieldElOperand::GetValue( DChar& outValue) const
{
  Get (outValue);
}


void
CharArrayFieldElOperand::GetValue( DText& outValue) const
{
  DChar ch;

  Get (ch);

  outValue = DText( );
  outValue.Append( ch);
}


void
CharArrayFieldElOperand::SetValue( const DChar& value)
{
  Set (value);
}


uint_t
CharArrayFieldElOperand::GetType( )
{
  return T_CHAR;
}


StackValue
CharArrayFieldElOperand::Duplicate( ) const
{
  DChar ch;
  Get (ch);

  return StackValue( CharOperand( ch));
}




bool
DateArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
DateArrayFieldElOperand::GetValue( DDate& outValue) const
{
  Get (outValue);
}


void
DateArrayFieldElOperand::GetValue( DDateTime& outValue) const
{
  DDate currValue;

  Get (currValue);

  outValue = DDateTime( currValue.mYear,
                        currValue.mMonth,
                        currValue.mDay,
                        0,
                        0,
                        0);
}


void
DateArrayFieldElOperand::GetValue( DHiresTime& outValue) const
{
  DDate currValue;
  Get (currValue);

  outValue = DHiresTime( currValue.mYear,
                         currValue.mMonth,
                         currValue.mDay,
                         0,
                         0,
                         0,
                         0);
}


void
DateArrayFieldElOperand::SetValue( const DDate& value)
{
  Set (value);
}


uint_t
DateArrayFieldElOperand::GetType( )
{
  return T_DATE;
}


StackValue
DateArrayFieldElOperand::Duplicate( ) const
{
  DDate value;

  Get (value);

  return StackValue( DateOperand( value));
}



bool
DateTimeArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
DateTimeArrayFieldElOperand::GetValue( DDate& outValue) const
{
  DDateTime currValue;

  Get (currValue);

  outValue = DDate( currValue.mYear, currValue.mMonth, currValue.mDay);
}


void
DateTimeArrayFieldElOperand::GetValue( DDateTime& outValue) const
{
  Get (outValue);
}


void
DateTimeArrayFieldElOperand::GetValue( DHiresTime& outValue) const
{
  DDateTime currValue;
  Get (currValue);

  outValue = DHiresTime( currValue.mYear,
                         currValue.mMonth,
                         currValue.mDay,
                         currValue.mHour,
                         currValue.mMinutes,
                         currValue.mSeconds,
                         0);
}


void
DateTimeArrayFieldElOperand::SetValue( const DDateTime& value)
{
  Set (value);
}


uint_t
DateTimeArrayFieldElOperand::GetType( )
{
  return T_DATETIME;
}


StackValue
DateTimeArrayFieldElOperand::Duplicate( ) const
{
  DDateTime value;
  Get (value);

  return StackValue( DateTimeOperand( value));
}



bool
HiresTimeArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
HiresTimeArrayFieldElOperand::GetValue( DDate& outValue) const
{
  DHiresTime currValue;

  Get (currValue);

  outValue = DDate( currValue.mYear, currValue.mMonth, currValue.mDay);
}


void
HiresTimeArrayFieldElOperand::GetValue( DDateTime& outValue) const
{
  DHiresTime currValue;

  Get (currValue);

  outValue = DDateTime( currValue.mYear,
                        currValue.mMonth,
                        currValue.mDay,
                        currValue.mHour,
                        currValue.mMinutes,
                        currValue.mSeconds);
}


void
HiresTimeArrayFieldElOperand::GetValue( DHiresTime& outValue) const
{
  Get (outValue);
}


void
HiresTimeArrayFieldElOperand::SetValue( const DHiresTime& value)
{
  Set (value);
}


uint_t
HiresTimeArrayFieldElOperand::GetType( )
{
  return T_HIRESTIME;
}


StackValue
HiresTimeArrayFieldElOperand::Duplicate( ) const
{
  DHiresTime value;

  Get (value);

  return StackValue( HiresTimeOperand( value));
}



bool
UInt8ArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
UInt8ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  DUInt8 currValue;
  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  Get (outValue);
}

void
UInt8ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  DUInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt8ArrayFieldElOperand::SetValue( const DUInt8& value)
{
  Set (value);
}


void
UInt8ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
UInt8ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
UInt8ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
UInt8ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
UInt8ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
UInt8ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
UInt8ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
UInt8ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DUInt8 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
UInt8ArrayFieldElOperand::GetType( )
{
  return T_UINT8;
}

StackValue
UInt8ArrayFieldElOperand::Duplicate( ) const
{
  DUInt8 value;

  Get (value);

  return StackValue( UInt8Operand( value));
}




bool
UInt16ArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
UInt16ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  Get (outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  DUInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt16ArrayFieldElOperand::SetValue( const DUInt16& value)
{
  Set (value);
}


void
UInt16ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
UInt16ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
UInt16ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
UInt16ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}

void
UInt16ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
UInt16ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
UInt16ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
UInt16ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DUInt16 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
UInt16ArrayFieldElOperand::GetType( )
{
  return T_UINT16;

}

StackValue
UInt16ArrayFieldElOperand::Duplicate( ) const
{
  DUInt16 value;

  Get (value);

  return StackValue( UInt16Operand( value));
}



bool
UInt32ArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
UInt32ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  Get (outValue);
}


void
UInt32ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  DUInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt32ArrayFieldElOperand::SetValue( const DUInt32& value)
{
  Set (value);
}


void
UInt32ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DUInt32 currValue;
  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
UInt32ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DUInt32 currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
UInt32ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DUInt32 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
UInt32ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DUInt32 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
UInt32ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DUInt32 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
UInt32ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DUInt32 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
UInt32ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DUInt32 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
UInt32ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DUInt32 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
UInt32ArrayFieldElOperand::GetType( )
{
  return T_UINT32;
}


StackValue
UInt32ArrayFieldElOperand::Duplicate( ) const
{
  DUInt32 value;

  Get (value);

  return StackValue( UInt32Operand( value));
}



bool
UInt64ArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
UInt64ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  DUInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
UInt64ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  Get (outValue);
}


void
UInt64ArrayFieldElOperand::SetValue( const DUInt64& value)
{
  Set (value);
}


void
UInt64ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
UInt64ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
UInt64ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
UInt64ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
UInt64ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
UInt64ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
UInt64ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
UInt64ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DUInt64 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
UInt64ArrayFieldElOperand::GetType( )
{
  return T_UINT64;
}


StackValue
UInt64ArrayFieldElOperand::Duplicate( ) const
{
  DUInt64 value;

  Get (value);

  return StackValue( UInt64Operand( value));
}



bool
Int8ArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
Int8ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  Get (outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  DInt8 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int8ArrayFieldElOperand::SetValue( const DInt8& value)
{
  Set (value);

}


void
Int8ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
Int8ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
Int8ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
Int8ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
Int8ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
Int8ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
Int8ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
Int8ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DInt8 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
Int8ArrayFieldElOperand::GetType( )
{
  return T_INT8;
}


StackValue
Int8ArrayFieldElOperand::Duplicate( ) const
{
  DInt8 value;

  Get (value);

  return StackValue( Int8Operand( value));
}



bool
Int16ArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
Int16ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  Get (outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  DInt16 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int16ArrayFieldElOperand::SetValue( const DInt16& value)
{
  Set (value);
}


void
Int16ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DInt16 currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
Int16ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DInt16 currValue;
  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
Int16ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DInt16 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
Int16ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DInt16 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
Int16ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DInt16 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
Int16ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DInt16 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
Int16ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DInt16 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
Int16ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DInt16 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
Int16ArrayFieldElOperand::GetType( )
{
  return T_INT16;
}


StackValue
Int16ArrayFieldElOperand::Duplicate( ) const
{
  DInt16 value;

  Get (value);

  return StackValue( Int16Operand( value));
}



bool
Int32ArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
Int32ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  Get (outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}

void
Int32ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  DInt32 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int32ArrayFieldElOperand::SetValue( const DInt32& value)
{
  Set (value);
}


void
Int32ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
Int32ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
Int32ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
Int32ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
Int32ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
Int32ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
Int32ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
Int32ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DInt32 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
Int32ArrayFieldElOperand::GetType( )
{
  return T_INT32;
}


StackValue
Int32ArrayFieldElOperand::Duplicate( ) const
{
  DInt32 value;

  Get (value);

  return StackValue( Int32Operand( value));
}



bool
Int64ArrayFieldElOperand::IsNull( ) const
{
  return false;
}

void
Int64ArrayFieldElOperand::GetValue( DInt8& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DInt16& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DInt32& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DInt64& outValue) const
{
  Get (outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DUInt8& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DUInt16& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DUInt32& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::GetValue( DUInt64& outValue) const
{
  DInt64 currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
Int64ArrayFieldElOperand::SetValue( const DInt64& value)
{
  Set (value);

}


void
Int64ArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
Int64ArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
Int64ArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
Int64ArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
Int64ArrayFieldElOperand::SelfMod( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_mod( currValue, value);

  Set (currValue);
}


void
Int64ArrayFieldElOperand::SelfAnd( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_and( currValue, value);

  Set (currValue);
}


void
Int64ArrayFieldElOperand::SelfXor( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_xor( currValue, value);

  Set (currValue);
}


void
Int64ArrayFieldElOperand::SelfOr( const DInt64& value)
{
  DInt64 currValue;

  Get (currValue);

  currValue = internal_or( currValue, value);

  Set (currValue);
}


uint_t
Int64ArrayFieldElOperand::GetType( )
{
  return T_INT64;
}


StackValue
Int64ArrayFieldElOperand::Duplicate( ) const
{
  DInt64 value;

  Get (value);

  return StackValue( Int64Operand( value));
}



bool
RealArrayFieldElOperand::IsNull( ) const
{
  return false;
}


void
RealArrayFieldElOperand::GetValue( DReal& outValue) const
{
  Get (outValue);
}


void
RealArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  DReal currValue;

  Get (currValue);

  number_convert( currValue, outValue);
}


void
RealArrayFieldElOperand::SetValue( const DReal& value)
{
  Set (value);
}


void
RealArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
RealArrayFieldElOperand::SelfAdd( const DRichReal& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
RealArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
RealArrayFieldElOperand::SelfSub( const DRichReal& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
RealArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
RealArrayFieldElOperand::SelfMul( const DRichReal& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
RealArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
RealArrayFieldElOperand::SelfDiv( const DRichReal& value)
{
  DReal currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


uint_t
RealArrayFieldElOperand::GetType( )
{
  return T_REAL;
}


StackValue
RealArrayFieldElOperand::Duplicate( ) const
{
  DReal value;

  Get (value);

  return StackValue( RealOperand( value));
}



bool
RichRealArrayFieldElOperand::IsNull( ) const
{
  return false;
}

void
RichRealArrayFieldElOperand::GetValue( DReal& outValue) const
{
  DRichReal currValue;

  Get (currValue);
}


void
RichRealArrayFieldElOperand::GetValue( DRichReal& outValue) const
{
  Get (outValue);
}


void
RichRealArrayFieldElOperand::SetValue( const DRichReal& value)
{
  Set (value);
}


void
RichRealArrayFieldElOperand::SelfAdd( const DInt64& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
RichRealArrayFieldElOperand::SelfAdd( const DRichReal& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_add( currValue, value);

  Set (currValue);
}


void
RichRealArrayFieldElOperand::SelfSub( const DInt64& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
RichRealArrayFieldElOperand::SelfSub( const DRichReal& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_sub( currValue, value);

  Set (currValue);
}


void
RichRealArrayFieldElOperand::SelfMul( const DInt64& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
RichRealArrayFieldElOperand::SelfMul( const DRichReal& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_mul( currValue, value);

  Set (currValue);
}


void
RichRealArrayFieldElOperand::SelfDiv( const DInt64& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


void
RichRealArrayFieldElOperand::SelfDiv( const DRichReal& value)
{
  DRichReal currValue;

  Get (currValue);

  currValue = internal_div( currValue, value);

  Set (currValue);
}


uint_t
RichRealArrayFieldElOperand::GetType( )
{
  return T_RICHREAL;
}


StackValue
RichRealArrayFieldElOperand::Duplicate( ) const
{
  DRichReal value;

  Get (value);

  return StackValue( RichRealOperand( value));
}

} //namespace prima
} //namespace whais

