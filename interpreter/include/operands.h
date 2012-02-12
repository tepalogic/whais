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

#include "dbs/include/dbs_values.h"


class NullOperand;
class BoolOperand;
class CharOperand;
class DateOperand;
class DateTimeOperand;
class HiresTimeOperand;
class UInt8Operand;
class UInt16Operand;
class UInt32Operand;
class UInt64Operand;
class Int8Operand;
class Int16Operand;
class Int32Operand;
class Int64Operand;
class RealOperand;
class RichRealOperand;
class TextOperand;

class I_Operand
{
public:
  I_Operand ()
  {
  }

  virtual ~I_Operand ();

  virtual NullOperand      GetNullOperand ();
  virtual BoolOperand      GetBoolOperand ();
  virtual CharOperand      GetCharOperand ();
  virtual DateOperand      GetDateOperand ();
  virtual DateTimeOperand  GetDateTimeOperand ();
  virtual HiresTimeOperand GetHiresTimeOperand ();
  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};

class StackedOperand : public I_Operand
{
public:
  StackedOperand ();

  template <class T>
  explicit StackedOperand (T& operand) :
    I_Operand ()
  {
    assert (sizeof (m_OperandPlacement) >= sizeof (T));

    _placement_new (m_OperandPlacement, operand);
  }

  virtual ~StackedOperand ();

  virtual NullOperand      GetNullOperand ();
  virtual BoolOperand      GetBoolOperand ();
  virtual CharOperand      GetCharOperand ();
  virtual DateOperand      GetDateOperand ();
  virtual DateTimeOperand  GetDateTimeOperand ();
  virtual HiresTimeOperand GetHiresTimeOperand ();
  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();

protected:
  static const D_UINT OPERAND_PLACEMENT_SIZE = 24;

  D_UINT8 m_OperandPlacement [OPERAND_PLACEMENT_SIZE];
};

class NullOperand : public I_Operand
{
public:
  NullOperand () :
    I_Operand ()
  {
  }

  virtual ~NullOperand ();

  virtual NullOperand      GetNullOperand ();
  virtual BoolOperand      GetBoolOperand ();
  virtual CharOperand      GetCharOperand ();
  virtual DateOperand      GetDateOperand ();
  virtual DateTimeOperand  GetDateTimeOperand ();
  virtual HiresTimeOperand GetHiresTimeOperand ();
  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();

};

class BoolOperand : public I_Operand, public DBSBool
{
public:
  explicit BoolOperand (const DBSBool& value) :
    I_Operand (),
    DBSBool (value)
  {
  }

  virtual ~BoolOperand ();

  virtual BoolOperand      GetBoolOperand ();
};

class CharOperand : public I_Operand, public DBSChar
{
  explicit CharOperand (const DBSChar& value) :
    I_Operand (),
    DBSChar (value)
  {
  }

  virtual ~CharOperand ();

  virtual CharOperand      GetCharOperand ();
  virtual TextOperand      GetTextOperand ();
};

class DateOperand : public I_Operand, public DBSDate
{
public:
  explicit DateOperand (const DBSDate& value) :
    I_Operand (),
    DBSDate (value)
  {
  }

  virtual ~DateOperand ();

  virtual DateOperand      GetDateOperand ();
  virtual DateTimeOperand  GetDateTimeOperand ();
  virtual HiresTimeOperand GetHiresTimeOperand ();
};

class DateTimeOperand : public I_Operand, public DBSDateTime
{
public:
  explicit DateTimeOperand (const DBSDateTime& value) :
    I_Operand (),
    DBSDateTime (value)
  {
  }

  virtual ~DateTimeOperand ();

  virtual DateOperand      GetDateOperand ();
  virtual DateTimeOperand  GetDateTimeOperand ();
  virtual HiresTimeOperand GetHiresTimeOperand ();
};

class HiresTimeOperand : public I_Operand, public DBSHiresTime
{
public:
  explicit HiresTimeOperand (const DBSHiresTime& value) :
    I_Operand (),
    DBSHiresTime (value)
  {
  }

  virtual ~HiresTimeOperand ();

  virtual DateOperand      GetDateOperand ();
  virtual DateTimeOperand  GetDateTimeOperand ();
  virtual HiresTimeOperand GetHiresTimeOperand ();
};

class UInt8Operand : public I_Operand, public DBSUInt8
{
public:
  explicit UInt8Operand (const DBSUInt8& value) :
    I_Operand (),
    DBSUInt8 (value)
  {
  }

  virtual ~UInt8Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};

class UInt16Operand : public I_Operand, public DBSUInt16
{
public:
  explicit UInt16Operand (const DBSUInt16& value) :
    I_Operand (),
    DBSUInt16 (value)
  {
  }

  virtual ~UInt16Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();

};

class UInt32Operand : public I_Operand, public DBSUInt32
{
public:
  explicit UInt32Operand (const DBSUInt32& value) :
    I_Operand (),
    DBSUInt32 (value)
  {
  }

  virtual ~UInt32Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();

};

class UInt64Operand : public I_Operand, public DBSUInt64
{
public:
  explicit UInt64Operand (const DBSUInt64& value) :
    I_Operand (),
    DBSUInt64 (value)
  {
  }

  virtual ~UInt64Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};

class Int8Operand : public I_Operand, public DBSInt8
{
public:
  explicit Int8Operand (const DBSInt8& value) :
    I_Operand (),
    DBSInt8 (value)
  {
  }

  virtual ~Int8Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};

class Int16Operand : public I_Operand, public DBSInt16
{
public:
  explicit Int16Operand (const DBSInt16& value) :
    I_Operand (),
    DBSInt16 (value)
  {
  }

  virtual ~Int16Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};

class Int32Operand : public I_Operand, public DBSInt32
{
public:
  explicit Int32Operand (const DBSInt32& value) :
    I_Operand (),
    DBSInt32 (value)
  {
  }

  virtual ~Int32Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};

class Int64Operand : public I_Operand, public DBSInt64
{
public:
  explicit Int64Operand (const DBSInt64& value) :
    I_Operand (),
    DBSInt64 (value)
  {
  }

  virtual ~Int64Operand ();

  virtual UInt8Operand     GetUInt8Operand ();
  virtual UInt16Operand    GetUInt16Operand ();
  virtual UInt32Operand    GetUInt32Operand ();
  virtual UInt64Operand    GetUInt64Operand ();
  virtual Int8Operand      GetInt8Operand ();
  virtual Int16Operand     GetInt16Operand ();
  virtual Int32Operand     GetInt32Operand ();
  virtual Int64Operand     GetInt64Operand ();
  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};


class RealOperand : public I_Operand, public DBSReal
{
public:
  explicit RealOperand (const DBSReal& value) :
    I_Operand (),
    DBSReal (value)
  {
  }

  virtual ~RealOperand ();

  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();

};

class RichRealOperand : public I_Operand, public DBSRichReal
{
public:
  explicit RichRealOperand (const DBSRichReal& value) :
    I_Operand (),
    DBSRichReal (value)
  {
  }

  virtual ~RichRealOperand ();

  virtual RealOperand      GetRealOperand ();
  virtual RichRealOperand  GetRichRealOperand ();
  virtual TextOperand      GetTextOperand ();
};

class TextOperand : public I_Operand, public DBSText
{
public:
  explicit TextOperand (const DBSText& value) :
    I_Operand (),
    DBSText (value)
  {
  }

  virtual ~TextOperand ();

  virtual TextOperand      GetTextOperand ();
};



#endif /* OPERANDS_H_ */
