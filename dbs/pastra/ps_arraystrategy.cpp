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

#include <assert.h>

#include "utils/endianness.h"
#include "dbs/dbs_mgr.h"
#include "dbs/dbs_types.h"
#include "dbs/dbs_exception.h"

#include "ps_arraystrategy.h"
#include "ps_serializer.h"



using namespace std;



namespace whais {



IArrayStrategy::IArrayStrategy( const DBS_FIELD_TYPE elemsType)
  : mElementsCount( 0),
    mShareCount( 0),
    mReferenceCount( 0),
    mElementsType( elemsType),
    mElementRawSize( 0)
{
  if (elemsType != T_UNDETERMINED)
  {
    assert( (elemsType >= T_BOOL) && (elemsType < T_TEXT));

    mElementRawSize = pastra::Serializer::Size( mElementsType, false);
  }
}


IArrayStrategy::~IArrayStrategy()
{
  assert( mReferenceCount == 0);
  assert( mShareCount == 0);
}


uint_t
IArrayStrategy::ReferenceCount() const
{
  return mReferenceCount;
}


void
IArrayStrategy::IncrementReferenceCount()
{
  assert( mShareCount == 0);

  ++mReferenceCount;
}


void
IArrayStrategy::DecrementReferenceCount()
{
  assert( mReferenceCount > 0);
  assert( mShareCount == 0);

  if (--mReferenceCount == 0)
    delete this;
}


uint_t
IArrayStrategy::ShareCount() const
{
  return mShareCount;
}


void
IArrayStrategy::IncrementShareCount()
{
  assert( mReferenceCount == 1);

  ++mShareCount;
}


void
IArrayStrategy::DecrementShareCount()
{
  assert( mReferenceCount == 1);
  assert( mShareCount > 0);

  --mShareCount;
}


void
IArrayStrategy::Clone( IArrayStrategy& strategy)
{
  assert( strategy.Type() == mElementsType);
  assert( this != &strategy);

  uint64_t currentPosition = 0;
  uint64_t cloneSize       = strategy.RawSize();

  while( cloneSize > 0)
    {
      assert( mElementRawSize > 0);

      uint8_t cloneBuff [128];

      const uint_t cloneBuffSize = ((sizeof cloneBuff) / mElementRawSize) *
                                      mElementRawSize;
      const uint64_t toClone = MIN (cloneBuffSize, cloneSize);

      strategy.RawRead( currentPosition, toClone, cloneBuff);
      RawWrite( currentPosition, toClone, cloneBuff);

      cloneSize -= toClone, currentPosition += toClone;
    }

  mElementsCount = strategy.mElementsCount;
}


bool
IArrayStrategy::IsRowValue() const
{
  return false;
}


pastra::TemporalArray&
IArrayStrategy::GetTemporal()
{
  throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));

  return *_RC(pastra::TemporalArray*, this);
}


pastra::RowFieldArray&
IArrayStrategy::GetRow()
{
  throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));

  return *_RC(pastra::RowFieldArray*, this);
}



namespace pastra {



NullArray::NullArray( const DBS_FIELD_TYPE elemsType)
  : IArrayStrategy( elemsType)
{
}


uint_t
NullArray::ReferenceCount() const
{
  return ~0; //To force new allocation when it will be modified.
}


void
NullArray::IncrementReferenceCount()
{
  return ; //Do nothing as we are a singletone that lifes for ever.
}


void
NullArray::DecrementReferenceCount()
{
  return ; //Do nothing as we are a singletone that lifes for ever.
}


uint_t
NullArray::ShareCount() const
{
  return 0;
}


void
NullArray::IncrementShareCount()
{
  //Someone does not know what is doing!
  throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));
}


void
NullArray::DecrementShareCount()
{
  //Someone does not know what is doing!
  throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));
}


void
NullArray::RawRead( const uint64_t      offset,
                    const uint64_t      size,
                    uint8_t* const      buffer)
{
  //Someone does not know what is doing!
  throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));
}


void
NullArray::RawWrite( const uint64_t           offset,
                     const uint64_t           size,
                     const uint8_t* const     buffer)
{
  //Someone does not know what is doing!
  throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));
}


void
NullArray::ColapseRaw( const uint64_t offset, const uint64_t count)
{
  //Someone does not know what is doing!
  throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));
}


uint64_t
NullArray::RawSize() const
{
  return 0;
}


NullArray&
NullArray::GetSingletoneInstace( const DBS_FIELD_TYPE type)
{
  static NullArray _boolInstance( T_BOOL);
  static NullArray _charInstance( T_CHAR);
  static NullArray _dateInstance( T_DATE);
  static NullArray _datetimeInstance( T_DATETIME);
  static NullArray _hirestimeInstance( T_HIRESTIME);
  static NullArray _uint8Instance( T_UINT8);
  static NullArray _uint16Instance( T_UINT16);
  static NullArray _uint32Instance( T_UINT32);
  static NullArray _uint64Instance( T_UINT64);
  static NullArray _realInstance( T_REAL);
  static NullArray _richrealInstance( T_RICHREAL);
  static NullArray _int8Instance( T_INT8);
  static NullArray _int16Instance( T_INT16);
  static NullArray _int32Instance( T_INT32);
  static NullArray _int64Instance( T_INT64);
  static NullArray _genericInstance( T_UNDETERMINED);

  switch( type)
  {
  case T_UNDETERMINED:
    return _genericInstance;

  case T_BOOL:
    return _boolInstance;

  case T_CHAR:
    return _charInstance;

  case T_DATE:
    return _dateInstance;

  case T_DATETIME:
    return _datetimeInstance;

  case T_HIRESTIME:
    return _hirestimeInstance;

  case T_UINT8:
    return _uint8Instance;

  case T_UINT16:
    return _uint16Instance;

  case T_UINT32:
    return _uint32Instance;

  case T_UINT64:
    return _uint64Instance;

  case T_REAL:
    return _realInstance;

  case T_RICHREAL:
    return _richrealInstance;

  case T_INT8:
    return _int8Instance;

  case T_INT16:
    return _int16Instance;

  case T_INT32:
    return _int32Instance;

  case T_INT64:
    return _int64Instance;

  case T_TEXT:
    throw DBSException( _EXTRA( DBSException::INVALID_ARRAY_TYPE));

  default:

    assert( false);

    throw DBSException( _EXTRA( DBSException::GENERAL_CONTROL_ERROR));
  }
}



TemporalArray::TemporalArray( const DBS_FIELD_TYPE type)
  : IArrayStrategy( type),
    mStorage()
{
}


TemporalArray::~TemporalArray()
{
  assert( mReferenceCount == 0);
}


void
TemporalArray::RawRead( const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer)
{
  mStorage.Read( offset, size, buffer);
}


void
TemporalArray::RawWrite( const uint64_t       offset,
                         const uint64_t       size,
                         const uint8_t* const buffer)
{
  assert( (mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert( mElementRawSize > 0);
  assert( (size % mElementRawSize) == 0);

  mStorage.Write( offset, size, buffer);

  mElementsCount = mStorage.Size() / mElementRawSize;
}


uint64_t
TemporalArray::RawSize() const
{
  return mStorage.Size() ;
}


TemporalArray&
TemporalArray::GetTemporal()
{
  return *this;
}


void
TemporalArray::ColapseRaw( const uint64_t offset, const uint64_t count)
{
  assert( (mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert( mElementRawSize > 0);
  assert( (count % mElementRawSize) == 0);

  mStorage.Colapse( offset, offset + count);

  mElementsCount = mStorage.Size() / mElementRawSize;
}



RowFieldArray::RowFieldArray( VariableSizeStore&    storage,
                              const uint64_t        firstRecordEntry,
                              const DBS_FIELD_TYPE  type)
  : IArrayStrategy( type),
    mFirstRecordEntry( firstRecordEntry),
    mStorage( storage),
    mTempArray( NULL)
{
  assert( mFirstRecordEntry > 0);
  assert( (mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert( mElementRawSize > 0);

  mStorage.RegisterReference();
  mStorage.IncrementRecordRef( mFirstRecordEntry);

  uint8_t elemetsCount[METADATA_SIZE];

  mStorage.GetRecord( firstRecordEntry, 0, sizeof elemetsCount, elemetsCount);

  mElementsCount = load_le_int64 (elemetsCount);
}


RowFieldArray::~RowFieldArray()
{
  assert( mReferenceCount == 0);

  if (mTempArray == NULL)
    {
      mStorage.DecrementRecordRef( mFirstRecordEntry);
      mStorage.Flush();
      mStorage.ReleaseReference();
    }
  else
    mTempArray->DecrementReferenceCount();
}


bool
RowFieldArray::IsRowValue() const
{
  return( mTempArray == NULL);
}


RowFieldArray&
RowFieldArray::GetRow()
{
  return *this;
}


TemporalArray&
RowFieldArray::GetTemporal()
{
  assert( mTempArray != NULL);

  return *mTempArray;
}


void
RowFieldArray::RawRead( const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer)
{

  if (mTempArray)
    mTempArray->RawRead( offset, size, buffer);

  else
    {
      mStorage.GetRecord( mFirstRecordEntry,
                          offset + METADATA_SIZE,
                          size,
                          buffer);
    }
}


void
RowFieldArray::RawWrite( const uint64_t       offset,
                         const uint64_t       size,
                         const uint8_t* const buffer)
{
  if (mTempArray)
    {
      mTempArray->RawWrite( offset, size, buffer);
      mElementsCount = RawSize() / mElementRawSize;
    }
  else
    {
      assert( (size % mElementRawSize) == 0);

      mStorage.UpdateRecord( mFirstRecordEntry,
                             offset + METADATA_SIZE,
                             size,
                             buffer);

      if ((offset + size) > RawSize())
        {
          assert( ((offset + size - RawSize()) % mElementRawSize) == 0);

          mElementsCount += (offset + size - RawSize()) / mElementRawSize;

          uint8_t temp[METADATA_SIZE];

          store_le_int64 (mElementsCount, temp);
          mStorage.UpdateRecord( mFirstRecordEntry, 0, sizeof( temp), temp);
        }
    }
}

uint64_t
RowFieldArray::RawSize() const
{
  if (mTempArray)
    return mTempArray->RawSize();

  assert( mElementRawSize > 0);

  return mElementsCount * mElementRawSize;
}


void
RowFieldArray::ColapseRaw( const uint64_t offset, const uint64_t count)
{
  if (mTempArray)
    {
      mTempArray->ColapseRaw( offset, count);
      return;
    }
  else
    EnableTemporalStorage();

  mTempArray->ColapseRaw( offset, count);

  assert( mElementRawSize > 0);
  assert( (mTempArray->RawSize() % mElementRawSize) == 0);

  mElementsCount = mTempArray->RawSize() / mElementRawSize;
}


void
RowFieldArray::EnableTemporalStorage()
{
  assert( mTempArray == NULL);

  auto_ptr<TemporalArray> tempArray( new TemporalArray( mElementsType));

  tempArray->Clone( *this);
  tempArray->IncrementReferenceCount();

  mTempArray = tempArray.release();

  mStorage.DecrementRecordRef( mFirstRecordEntry);
  mStorage.ReleaseReference();
}


} //namespace pastra
} //namespace whais

