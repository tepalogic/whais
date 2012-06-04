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


static const D_UINT MAX_OP_QWORDS = 3;

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

  //Special treatement for these
  virtual FIELD_INDEX   GetField ();
  virtual DBSArray&     GetArray ();
  virtual DBSText&      GetText ();
  virtual I_DBSTable&   GetTable ();
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

  I_Operand& GetOperand () { return *_RC (I_Operand*, m_Storage);  }

private:
  D_UINT64 m_Storage [MAX_OP_QWORDS];
};

#endif /* OPERANDS_H_ */

