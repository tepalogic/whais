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
#include <vector>

#include "dbs/include/dbs_mgr.h"
#include "dbs/include/dbs_values.h"

static const D_UINT MAX_OP_QWORDS = sizeof (DBSRichReal) + 2 * sizeof (void*);

class StackValue;

class I_Operand
{
public:
  I_Operand ()
  {
  }

  virtual ~I_Operand ()
  {
  }

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
};

class StackValue
{
public:
  template <class OP_T>
  explicit StackValue (const OP_T& op)
  {
    const I_Operand& compileTest = op;
    (void)compileTest;  //Just to make sure the right type is used.

    assert (sizeof (OP_T) <= sizeof (m_Storage));

    _placement_new (m_Storage, op);
  }

  ~StackValue ()
  {
  }

  void Clear ()
  {
    GetOperand ().~I_Operand ();
  }

  I_Operand& GetOperand () { return *_RC (I_Operand*, m_Storage);  }

private:
  D_UINT64 m_Storage [MAX_OP_QWORDS];
};

class SessionStack
{
public:
  SessionStack ();
  ~SessionStack ();

  void  Push ();
  void  Push (DBSBool& value);
  void  Push (DBSChar& value);
  void  Push (DBSDate& value);
  void  Push (DBSDateTime& value);
  void  Push (DBSHiresTime& value);
  void  Push (DBSInt8& value);
  void  Push (DBSInt16& value);
  void  Push (DBSInt32& value);
  void  Push (DBSInt64& value);
  void  Push (DBSReal& value);
  void  Push (DBSRichReal& value);
  void  Push (DBSUInt8& value);
  void  Push (DBSUInt16& value);
  void  Push (DBSUInt32& value);
  void  Push (DBSUInt64& value);
  void  Push (DBSText& value);
  void  Push (DBSArray& value);
  void  Push (I_DBSHandler& dbs, I_DBSTable& table);

  void  Push (StackValue& value);

  void  Pop (const D_UINT count);

  size_t Size () const;

  StackValue& operator[] (const D_UINT index);

private:
  std::vector<StackValue> m_Stack;
};

#endif /* OPERANDS_H_ */
