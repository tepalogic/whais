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

#include "interpreter.h"
#include "operands.h"

namespace prima
{

class GlobalOperandStorage
{
private:
  friend class GlobalValue;

  static const D_UINT MAX_OP_SIZE = 3;

  D_UINT64 m_Storage [MAX_OP_SIZE];

  I_Operand& GetOperand () { return *_RC (I_Operand*, m_Storage); }
};

class GlobalValue
{
public:
  template <class OP_T>
  explicit GlobalValue (const OP_T& op)
  {
    const I_Operand& compileTest = op;
    (void)compileTest; //Do nothing! Is here just to make sure OP_T is a valid type!

    assert (sizeof (OP_T) <= sizeof (GlobalOperandStorage));
    _placement_new (m_Operand.m_Storage, op);
  }

  ~GlobalValue ()
  {
  }

  template <class DBS_T>
  void GetValue (DBS_T& outValue)
  {
    //TODO: Make sure this is multi thread safe
    m_Operand.GetOperand ().GetValue (outValue);
  }

  template <class DBS_T>
  void SetValue (const DBS_T& value)
  {
    //TODO: Make sure this is multi thread safe:
    //      Check the DBS_X opertor= to be thread safe by seting null condition LAST
    //      Check the conversion operator by setting the null FIRST
    //      Array and text strategys are not safe!
    m_Operand.GetOperand ().SetValue (value);
  }

  TableOperand& GetTableOp ()
  {
    return m_Operand.GetOperand ().GetTableOp ();
  }

  RowOperand&   GetRowOp ()
  {
    return m_Operand.GetOperand ().GetRowOp ();
  }

protected:
  GlobalOperandStorage m_Operand;
};

class GlobalOperand : public I_Operand
{
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

protected:
  GlobalValue& m_Value;
};

}


#endif /* PM_OPERAND_H_ */
