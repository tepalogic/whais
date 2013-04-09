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
#include <cstring>

#include "dbs/include/dbs_mgr.h"
#include "dbs/include/dbs_values.h"

//TODO: Make sure you compute this limit properly  for all architectures
static const uint_t MAX_OP_QWORDS = 6;

class StackValue;

class INTERP_SHL I_Operand
{
public:
  I_Operand ()
  {
  }

  virtual ~I_Operand ()
  {
  }

  virtual bool IsNull () const = 0;

  virtual void GetValue (DBSBool& outValue) const = 0;
  virtual void GetValue (DBSChar& outValue) const = 0;
  virtual void GetValue (DBSDate& outValue) const = 0;
  virtual void GetValue (DBSDateTime& outValue) const = 0;
  virtual void GetValue (DBSHiresTime& outValue) const = 0;
  virtual void GetValue (DBSInt8& outValue) const = 0;
  virtual void GetValue (DBSInt16& outValue) const = 0;
  virtual void GetValue (DBSInt32& outValue) const = 0;
  virtual void GetValue (DBSInt64& outValue) const = 0;
  virtual void GetValue (DBSReal& outValue) const = 0;
  virtual void GetValue (DBSRichReal& outValue) const = 0;
  virtual void GetValue (DBSUInt8& outValue) const = 0;
  virtual void GetValue (DBSUInt16& outValue) const = 0;
  virtual void GetValue (DBSUInt32& outValue) const = 0;
  virtual void GetValue (DBSUInt64& outValue) const = 0;
  virtual void GetValue (DBSText& outValue) const = 0;
  virtual void GetValue (DBSArray& outValue) const = 0;

  virtual void SetValue (const DBSBool& value) = 0;
  virtual void SetValue (const DBSChar& value) = 0;
  virtual void SetValue (const DBSDate& value) = 0;
  virtual void SetValue (const DBSDateTime& value) = 0;
  virtual void SetValue (const DBSHiresTime& value) = 0;
  virtual void SetValue (const DBSInt8& value) = 0;
  virtual void SetValue (const DBSInt16& value) = 0;
  virtual void SetValue (const DBSInt32& value) = 0;
  virtual void SetValue (const DBSInt64& value) = 0;
  virtual void SetValue (const DBSReal& value) = 0;
  virtual void SetValue (const DBSRichReal& value) = 0;
  virtual void SetValue (const DBSUInt8& value) = 0;
  virtual void SetValue (const DBSUInt16& value) = 0;
  virtual void SetValue (const DBSUInt32& value) = 0;
  virtual void SetValue (const DBSUInt64& value) = 0;
  virtual void SetValue (const DBSText& value) = 0;
  virtual void SetValue (const DBSArray& value) = 0;

  virtual void SelfAdd (const DBSInt64& value) = 0;
  virtual void SelfAdd (const DBSRichReal& value) = 0;
  virtual void SelfAdd (const DBSChar& value) = 0;
  virtual void SelfAdd (const DBSText& value) = 0;

  virtual void SelfSub (const DBSInt64& value) = 0;
  virtual void SelfSub (const DBSRichReal& value) = 0;

  virtual void SelfMul (const DBSInt64& value) = 0;
  virtual void SelfMul (const DBSRichReal& value) = 0;

  virtual void SelfDiv (const DBSInt64& value) = 0;
  virtual void SelfDiv (const DBSRichReal& value) = 0;

  virtual void SelfMod (const DBSInt64& value) = 0;

  virtual void SelfAnd (const DBSInt64& value) = 0;
  virtual void SelfAnd (const DBSBool& value) = 0;

  virtual void SelfXor (const DBSInt64& value) = 0;
  virtual void SelfXor (const DBSBool& value) = 0;

  virtual void SelfOr (const DBSInt64& value) = 0;
  virtual void SelfOr (const DBSBool& value) = 0;

  virtual uint_t GetType () = 0;

  virtual FIELD_INDEX   GetField () = 0;
  virtual I_DBSTable&   GetTable () = 0;
  virtual StackValue    GetFieldAt (const FIELD_INDEX field) = 0;
  virtual StackValue    GetValueAt (const uint64_t index) = 0;

  virtual StackValue Duplicate () const = 0;

protected:
  friend class StackValue;
  virtual void NotifyCopy () = 0;
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

  StackValue (const StackValue& source)
  {
    I_Operand& op = _CC (I_Operand&,
                         _RC (const I_Operand&, source.m_Storage));
    op.NotifyCopy ();

    memcpy (&m_Storage, &source.m_Storage, sizeof m_Storage);
  }

  StackValue&
  operator= (const StackValue& source)
  {
    I_Operand& op = _CC (I_Operand&,
                         _RC (const I_Operand&, source.m_Storage));
    op.NotifyCopy ();

    Clear ();
    memcpy (&m_Storage, &source.m_Storage, sizeof m_Storage);

    return *this;
  }

  ~StackValue ()
  {
    Clear ();
  }

  I_Operand& GetOperand () { return *_RC (I_Operand*, m_Storage);  }

private:
  void Clear ()
  {
    GetOperand ().~I_Operand ();
  }

  uint64_t m_Storage [MAX_OP_QWORDS];
};

class INTERP_SHL SessionStack
{
public:
  SessionStack ();
  ~SessionStack ();

  void  Push ();
  void  Push (const DBSBool& value);
  void  Push (const DBSChar& value);
  void  Push (const DBSDate& value);
  void  Push (const DBSDateTime& value);
  void  Push (const DBSHiresTime& value);
  void  Push (const DBSInt8& value);
  void  Push (const DBSInt16& value);
  void  Push (const DBSInt32& value);
  void  Push (const DBSInt64& value);
  void  Push (const DBSReal& value);
  void  Push (const DBSRichReal& value);
  void  Push (const DBSUInt8& value);
  void  Push (const DBSUInt16& value);
  void  Push (const DBSUInt32& value);
  void  Push (const DBSUInt64& value);
  void  Push (const DBSText& value);
  void  Push (const DBSArray& value);
  void  Push (I_DBSHandler& dbs, I_DBSTable& table);

  void  Push (const StackValue& value);

  void  Pop (const uint_t count);

  size_t Size () const;

  StackValue& operator[] (const uint_t index);

private:
  std::vector<StackValue> m_Stack;
};

#endif /* OPERANDS_H_ */
