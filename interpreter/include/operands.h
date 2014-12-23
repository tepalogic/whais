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

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_values.h"

namespace whais {

//TODO: Make sure you compute this limit properly for all architectures
static const uint_t MAX_OP_QWORDS = 6;

class StackValue;
class INativeObject;



class INTERP_SHL INativeObject
{
public:
  virtual ~INativeObject();

  virtual void RegisterUser() = 0;
  virtual void ReleaseUser() = 0;
};


class INTERP_SHL IOperand
{
  friend class StackValue;

public:
  IOperand()
  {
  }

  virtual ~IOperand();

  virtual bool IsNull() const = 0;

  virtual void GetValue( DBool& outValue) const = 0;
  virtual void GetValue( DChar& outValue) const = 0;
  virtual void GetValue( DDate& outValue) const = 0;
  virtual void GetValue( DDateTime& outValue) const = 0;
  virtual void GetValue( DHiresTime& outValue) const = 0;
  virtual void GetValue( DInt8& outValue) const = 0;
  virtual void GetValue( DInt16& outValue) const = 0;
  virtual void GetValue( DInt32& outValue) const = 0;
  virtual void GetValue( DInt64& outValue) const = 0;
  virtual void GetValue( DReal& outValue) const = 0;
  virtual void GetValue( DRichReal& outValue) const = 0;
  virtual void GetValue( DUInt8& outValue) const = 0;
  virtual void GetValue( DUInt16& outValue) const = 0;
  virtual void GetValue( DUInt32& outValue) const = 0;
  virtual void GetValue( DUInt64& outValue) const = 0;
  virtual void GetValue( DText& outValue) const = 0;
  virtual void GetValue( DArray& outValue) const = 0;

  virtual void SetValue( const DBool& value) = 0;
  virtual void SetValue( const DChar& value) = 0;
  virtual void SetValue( const DHiresTime& value) = 0;
  virtual void SetValue( const DInt64& value) = 0;
  virtual void SetValue( const DUInt64& value) = 0;
  virtual void SetValue( const DRichReal& value) = 0;
  virtual void SetValue( const DText& value) = 0;
  virtual void SetValue( const DArray& value) = 0;


  virtual void SelfAdd( const DInt64& value) = 0;
  virtual void SelfAdd( const DRichReal& value) = 0;
  virtual void SelfAdd( const DChar& value) = 0;
  virtual void SelfAdd( const DText& value) = 0;

  virtual void SelfSub( const DInt64& value) = 0;
  virtual void SelfSub( const DRichReal& value) = 0;

  virtual void SelfMul( const DInt64& value) = 0;
  virtual void SelfMul( const DRichReal& value) = 0;

  virtual void SelfDiv( const DInt64& value) = 0;
  virtual void SelfDiv( const DRichReal& value) = 0;

  virtual void SelfMod( const DInt64& value) = 0;

  virtual void SelfAnd( const DInt64& value) = 0;
  virtual void SelfAnd( const DBool& value) = 0;

  virtual void SelfXor( const DInt64& value) = 0;
  virtual void SelfXor( const DBool& value) = 0;

  virtual void SelfOr( const DInt64& value) = 0;
  virtual void SelfOr( const DBool& value) = 0;

  virtual uint_t GetType() = 0;

  virtual FIELD_INDEX GetField() = 0;

  virtual ITable& GetTable() = 0;

  virtual StackValue GetTableValue() = 0;

  virtual StackValue GetFieldAt( const FIELD_INDEX field) = 0;

  virtual StackValue GetValueAt( const uint64_t index) = 0;

  virtual StackValue Duplicate() const = 0;

  virtual void           NativeObject( INativeObject* const value) = 0;
  virtual INativeObject& NativeObject() = 0;

protected:
  virtual void NotifyCopy() = 0;
};


class StackValue
{
public:
  template <class OP_T>
  explicit StackValue( const OP_T& op)
  {
    const IOperand& compileTest = op;
    (void)compileTest;  //Just to make sure the right type is used.

    assert( sizeof( OP_T) <= sizeof( mStorage));

    _placement_new( mStorage, op);
  }

  StackValue( const StackValue& source)
  {
    IOperand& op = _CC (StackValue&, source).Operand();

    op.NotifyCopy();

    memcpy( &mStorage, &source.mStorage, sizeof mStorage);
  }

  ~StackValue()
  {
    Clear();
  }

  StackValue&
  operator= (const StackValue& source)
  {
    IOperand& op = _CC (StackValue&, source).Operand();

    op.NotifyCopy();

    Clear();
    memcpy( &mStorage, &source.mStorage, sizeof mStorage);

    return *this;
  }

  IOperand& Operand()
  {
    return *_RC (IOperand*, mStorage);
  }

private:
  void Clear()
  {
    Operand().~IOperand();
  }

  uint64_t mStorage[MAX_OP_QWORDS];
};



class INTERP_SHL SessionStack
{
public:
  SessionStack();
  ~SessionStack();

  void  Push();
  void  Push( const DBool& value);
  void  Push( const DChar& value);
  void  Push( const DDate& value);
  void  Push( const DDateTime& value);
  void  Push( const DHiresTime& value);
  void  Push( const DInt8& value);
  void  Push( const DInt16& value);
  void  Push( const DInt32& value);
  void  Push( const DInt64& value);
  void  Push( const DReal& value);
  void  Push( const DRichReal& value);
  void  Push( const DUInt8& value);
  void  Push( const DUInt16& value);
  void  Push( const DUInt32& value);
  void  Push( const DUInt64& value);
  void  Push( const DText& value);
  void  Push( const DArray& value);
  void  Push( IDBSHandler& dbs, ITable& table);
  void  Push( INativeObject& object);
  void  Push( const StackValue& value);

  void  Pop (const uint_t count);

  size_t Size() const;

  StackValue& operator[] (const uint_t index);

private:

#pragma warning( disable: 4251)
  std::vector<StackValue> mStack;
#pragma warning( default: 4251)

};


} //namespace whais

#endif /* OPERANDS_H_ */

