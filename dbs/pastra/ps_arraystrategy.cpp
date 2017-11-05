/******************************************************************************
 PASTRA - A light database one file system and more.
 Copyright(C) 2008  Iulian Popa

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
#include "utils/wsort.h"
#include "dbs/dbs_mgr.h"
#include "dbs/dbs_types.h"
#include "dbs/dbs_exception.h"
#include "ps_arraystrategy.h"

using namespace std;


namespace whais {


IArrayStrategy::IArrayStrategy(const DBS_FIELD_TYPE elemsType)
  : mElementsCount(0),
    mElementRawSize(0),
    mElementsType(elemsType)
{
  if (elemsType != T_UNDETERMINED)
  {
    assert((elemsType >= T_BOOL) && (elemsType < T_TEXT));

    mElementRawSize = pastra::Serializer::Size(mElementsType, false);
  }
}

uint64_t
IArrayStrategy::Count()
{
  assert (mSelfShare.lock().get() == this);

  return mElementsCount;
}

DBS_BASIC_TYPE
IArrayStrategy::Type()
{
  assert (mSelfShare.lock().get() == this);

  return mElementsType;
}

uint_t
IArrayStrategy::Get(const uint64_t index, uint8_t* const dest)
{
  LockRAII<Lock> _l(mLock);

  assert (mSelfShare.lock().get() == this);

  if (index == mElementsCount)
    return 0;

  else if (index > mElementsCount)
    throw DBSException(_EXTRA(DBSException::ARRAY_INDEX_TOO_BIG));

  RawRead(index * mElementRawSize, mElementRawSize, dest);

  return mElementRawSize;
}

shared_ptr<IArrayStrategy>
IArrayStrategy::Set(const DBS_BASIC_TYPE type,
                       const uint8_t* const rawValue,
                       const uint64_t index)
{
  LockRAII<Lock> _l(mLock);

  shared_ptr<IArrayStrategy> r = mSelfShare.lock();
  assert (r.get() == this);

  if (type != mElementsType)
  {
    if ((mElementsType != T_UNDETERMINED) && (index > 0))
      throw DBSException(_EXTRA(DBSException::INVALID_ARRAY_TYPE));

    r = shared_make(pastra::TemporalArray, type);
    r->SetSelfReference(r);
  }
  else if (index > mElementsCount)
    throw DBSException(_EXTRA(DBSException::ARRAY_INDEX_TOO_BIG));

  if (r->IsShared())
  {
    assert (r.get() == this);

    r = r->Clone();

    assert(r->mElementsCount == mElementsCount);
    assert(r->mElementRawSize == mElementRawSize);
    assert(r->mElementsType == mElementsType);

    assert (r.get() != mSelfShare.lock().get());
  }

  r->RawWrite(index * r->mElementRawSize, r->mElementRawSize, rawValue);

  if (index == r->mElementsCount)
    r->mElementsCount++;

  assert(r->RawSize() % r->mElementsCount == 0);
  assert(r->RawSize() / r->mElementsCount == r->mElementRawSize);

  return r;
}

shared_ptr<IArrayStrategy>
IArrayStrategy::Add(const DBS_BASIC_TYPE type,
                    const uint8_t* const rawValue,
                    uint64_t* const outIndex)
{
  LockRAII<Lock> _l(mLock);

  shared_ptr<IArrayStrategy> r = mSelfShare.lock();
  assert (r.get() == this);

  assert((T_UNKNOWN < type) && (type < T_TEXT));

  if (type != mElementsType)
  {
    if (mElementsType != T_UNDETERMINED)
      throw DBSException(_EXTRA(DBSException::INVALID_ARRAY_TYPE));

    r = shared_make(pastra::TemporalArray, type);
    r->SetSelfReference(r);
  }

  if (r->IsShared())
  {
    assert (r.get() == this);

    r = r->Clone();

    assert(r->mElementsCount == mElementsCount);
    assert(r->mElementRawSize == mElementRawSize);
    assert(r->mElementsType == mElementsType);
  }

  r->RawWrite(r->mElementsCount * r->mElementRawSize, r->mElementRawSize, rawValue);
  *outIndex = r->mElementsCount++;

  assert(r->RawSize() % r->mElementsCount == 0);
  assert(r->RawSize() / r->mElementsCount == r->mElementRawSize);

  return r;
}

shared_ptr<IArrayStrategy>
IArrayStrategy::Remove(const uint64_t index)
{
  LockRAII<Lock> _l(mLock);

  shared_ptr<IArrayStrategy> r = mSelfShare.lock();
  assert (r.get() == this);

  if (index > mElementsCount)
    throw DBSException(_EXTRA(DBSException::ARRAY_INDEX_TOO_BIG));

  else if (index == mElementsCount)
    return r;

  if ( ! IsShared())
  {
    ColapseRaw(mElementRawSize * index, mElementRawSize);
    --mElementsCount;

    return r;
  }

  r = shared_make(pastra::TemporalArray, mElementsType);
  r->SetSelfReference(r);

  assert(r->mElementRawSize == mElementRawSize);
  assert(r->mElementsType == mElementsType);

  uint64_t offset = 0;
  uint8_t buffer[256];
  while (offset < mElementRawSize * index)
  {
    uint_t chunkSize = MIN(sizeof buffer, mElementRawSize * index);

    RawRead(offset, chunkSize, buffer);
    r->RawWrite(offset, chunkSize, buffer);

    offset += chunkSize;
  }

  assert(offset == mElementRawSize * index);

  offset += mElementRawSize;
  while (offset < mElementRawSize * mElementsCount)
  {
    uint_t chunkSize = MIN(sizeof buffer, mElementRawSize * mElementsCount - offset);
    RawRead(offset, chunkSize, buffer);
    r->RawWrite(offset - mElementRawSize, chunkSize, buffer);

    offset += chunkSize;
  }

  assert(offset == mElementRawSize * mElementsCount);

  r->mElementsCount = mElementsCount - 1;

  return r;
}

shared_ptr<IArrayStrategy>
IArrayStrategy::Sort(const bool reverse)
{
  LockRAII<Lock> _l(mLock);

  shared_ptr<IArrayStrategy> r = mSelfShare.lock();
  assert (r.get() == this);

  if (mElementsCount == 0)
    return r;

  assert((T_UNKNOWN < mElementsType) && (mElementsType < T_TEXT));

  if (r->IsShared())
  {
    r = r->Clone();

    assert(r->mElementsCount == mElementsCount);
    assert(r->mElementRawSize == mElementRawSize);
    assert(r->mElementsType == mElementsType);
  }

  switch (mElementsType)
  {
  case T_BOOL:
  {
    ArrayContainer<DBool> temp(*r);
    quick_sort<DBool, ArrayContainer<DBool> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_CHAR:
  {
    ArrayContainer<DChar> temp(*r);
    quick_sort<DChar, ArrayContainer<DChar> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_DATE:
  {
    ArrayContainer<DDate> temp(*r);
    quick_sort<DDate, ArrayContainer<DDate> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_DATETIME:
  {
    ArrayContainer<DDateTime> temp(*r);
    quick_sort<DDateTime, ArrayContainer<DDateTime> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_HIRESTIME:
  {
    ArrayContainer<DHiresTime> temp(*r);
    quick_sort<DHiresTime, ArrayContainer<DHiresTime> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_UINT8:
  {
    ArrayContainer<DUInt8> temp(*r);
    quick_sort<DUInt8, ArrayContainer<DUInt8> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_UINT16:
  {
    ArrayContainer<DUInt16> temp(*r);
    quick_sort<DUInt16, ArrayContainer<DUInt16> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_UINT32:
  {
    ArrayContainer<DUInt32> temp(*r);
    quick_sort<DUInt32, ArrayContainer<DUInt32> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_UINT64:
  {
    ArrayContainer<DUInt64> temp(*r);
    quick_sort<DUInt64, ArrayContainer<DUInt64> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_REAL:
  {
    ArrayContainer<DReal> temp(*r);
    quick_sort<DReal, ArrayContainer<DReal> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_RICHREAL:
  {
    ArrayContainer<DRichReal> temp(*r);
    quick_sort<DRichReal, ArrayContainer<DRichReal> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_INT8:
  {
    ArrayContainer<DInt8> temp(*r);
    quick_sort<DInt8, ArrayContainer<DInt8> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_INT16:
  {
    ArrayContainer<DInt16> temp(*r);
    quick_sort<DInt16, ArrayContainer<DInt16> >(0, mElementsCount - 1, reverse, temp);
  }
    break;

  case T_INT32:
  {
    ArrayContainer<DInt32> temp(*r);
    quick_sort<DInt32, ArrayContainer<DInt32> >(0, mElementsCount - 1, reverse,
        temp);
  }
    break;

  case T_INT64:
  {
    ArrayContainer<DInt64> temp(*r);
    quick_sort<DInt64, ArrayContainer<DInt64> >(0, mElementsCount - 1, reverse, temp);
  }
    break;
  default:
    throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
  }

  return r;
}


shared_ptr<IArrayStrategy>
IArrayStrategy::Clone()
{
  shared_ptr<IArrayStrategy> r = shared_make(pastra::TemporalArray, mElementsType);
  r->SetSelfReference(r);

  assert(r->mElementRawSize == mElementRawSize);
  assert(r->mElementsType == mElementsType);

  uint64_t offset = 0;
  uint8_t buffer[256];
  while (offset < mElementRawSize * mElementsCount)
  {
    uint_t chunkSize = MIN(sizeof buffer, mElementRawSize * mElementsCount - offset);

    RawRead(offset, chunkSize, buffer);
    r->RawWrite(offset, chunkSize, buffer);

    offset += chunkSize;
  }

  assert(offset == mElementRawSize * mElementsCount);

  r->mElementsCount = mElementsCount;

  return r;
}

pastra::TemporalContainer&
IArrayStrategy::GetTemporalContainer()
{
  throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

pastra::VariableSizeStore&
IArrayStrategy::GetRowStorage()
{
  throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}


namespace pastra {


NullArray::NullArray(const DBS_FIELD_TYPE elemsType)
  : IArrayStrategy(elemsType)
{
}



bool
NullArray::IsShared() const
{
  return true;
}

void
NullArray::RawRead(const uint64_t offset, const uint64_t size, uint8_t* const buffer)
{
  if ((offset != 0) || (size != 0))
    throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

void
NullArray::RawWrite(const uint64_t offset, const uint64_t size, const uint8_t* const buffer)
{
  if ((offset != 0) || (size != 0))
    throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

void
NullArray::ColapseRaw(const uint64_t offset, const uint64_t count)
{
  if ((offset != 0) || (count != 0))
    throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
}

uint64_t
NullArray::RawSize() const
{
  return 0;
}

shared_ptr<IArrayStrategy>
NullArray::GetSingletoneInstace(const DBS_FIELD_TYPE type)
{
  static shared_ptr<IArrayStrategy> _boolInstance = shared_make(NullArray, T_BOOL);
  static shared_ptr<IArrayStrategy> _charInstance = shared_make(NullArray, T_CHAR);
  static shared_ptr<IArrayStrategy> _dateInstance = shared_make(NullArray, T_DATE);
  static shared_ptr<IArrayStrategy> _datetimeInstance = shared_make(NullArray, T_DATETIME);
  static shared_ptr<IArrayStrategy> _hirestimeInstance = shared_make(NullArray, T_HIRESTIME);
  static shared_ptr<IArrayStrategy> _uint8Instance = shared_make(NullArray, T_UINT8);
  static shared_ptr<IArrayStrategy> _uint16Instance = shared_make(NullArray, T_UINT16);
  static shared_ptr<IArrayStrategy> _uint32Instance = shared_make(NullArray, T_UINT32);
  static shared_ptr<IArrayStrategy> _uint64Instance = shared_make(NullArray, T_UINT64);
  static shared_ptr<IArrayStrategy> _realInstance = shared_make(NullArray, T_REAL);
  static shared_ptr<IArrayStrategy> _richrealInstance = shared_make(NullArray, T_RICHREAL);
  static shared_ptr<IArrayStrategy> _int8Instance = shared_make(NullArray, T_INT8);
  static shared_ptr<IArrayStrategy> _int16Instance = shared_make(NullArray, T_INT16);
  static shared_ptr<IArrayStrategy> _int32Instance = shared_make(NullArray, T_INT32);
  static shared_ptr<IArrayStrategy> _int64Instance = shared_make(NullArray, T_INT64);
  static shared_ptr<IArrayStrategy> _genericInstance = shared_make(NullArray, T_UNDETERMINED);
  static bool inited = false;
  static SpinLock initLock;

  LockRAII<decltype(initLock)> _l(initLock);

  if ( ! inited)
  {
    _boolInstance->SetSelfReference(_boolInstance);
    _charInstance->SetSelfReference(_charInstance);
    _dateInstance->SetSelfReference(_dateInstance);
    _datetimeInstance->SetSelfReference(_datetimeInstance);
    _hirestimeInstance->SetSelfReference(_hirestimeInstance);
    _uint8Instance->SetSelfReference(_uint8Instance);
    _uint16Instance->SetSelfReference(_uint16Instance);
    _uint32Instance->SetSelfReference(_uint32Instance);
    _uint64Instance->SetSelfReference(_uint64Instance);
    _realInstance->SetSelfReference(_realInstance);
    _richrealInstance->SetSelfReference(_richrealInstance);
    _int8Instance->SetSelfReference(_int8Instance);
    _int16Instance->SetSelfReference(_int16Instance);
    _int32Instance->SetSelfReference(_int32Instance);
    _int64Instance->SetSelfReference(_int64Instance);
    _genericInstance->SetSelfReference(_genericInstance);

    inited = true;
  }
  _l.unlock();


  switch(type)
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
    throw DBSException(_EXTRA(DBSException::INVALID_ARRAY_TYPE));

  default:

    assert(false);

    throw DBSException(_EXTRA(DBSException::GENERAL_CONTROL_ERROR));
  }
}


TemporalArray::TemporalArray(const DBS_FIELD_TYPE type)
  : IArrayStrategy(type)
{
}

void
TemporalArray::RawRead(const uint64_t offset, const uint64_t size, uint8_t* const buffer)
{
  mStorage.Read(offset, size, buffer);
}

bool
TemporalArray::IsShared() const
{
  return mSelfShare.use_count() > 3;
}

void
TemporalArray::RawWrite(const uint64_t       offset,
                        const uint64_t       size,
                        const uint8_t* const buffer)
{
  assert((mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert(mElementRawSize > 0);

  mStorage.Write(offset, size, buffer);
}


void
TemporalArray::ColapseRaw(const uint64_t offset, const uint64_t count)
{
  assert((mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert(mElementRawSize > 0);
  assert((count % mElementRawSize) == 0);

  mStorage.Colapse(offset, offset + count);
}


uint64_t
TemporalArray::RawSize() const
{
  assert(mStorage.Size() == mElementRawSize * mElementsCount);

  return mElementRawSize * mElementsCount;
}

TemporalContainer&
TemporalArray::GetTemporalContainer()
{
  return mStorage;
}


RowFieldArray::RowFieldArray(VariableSizeStoreSPtr  storage,
                             const uint64_t         firstRecordEntry,
                             const DBS_FIELD_TYPE   type)
  : IArrayStrategy(type),
    mFirstRecordEntry(firstRecordEntry),
    mStorage(storage)
{
  assert(mFirstRecordEntry > 0);
  assert((mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert(mElementRawSize > 0);

  mStorage->IncrementRecordRef(mFirstRecordEntry);

  uint8_t elemetsCount[METADATA_SIZE];
  mStorage->GetRecord(firstRecordEntry, 0, sizeof elemetsCount, elemetsCount);

  mElementsCount = load_le_int64(elemetsCount);

  assert(mElementsCount > 0);
  assert(mFirstRecordEntry > 0);
}


RowFieldArray::~RowFieldArray()
{
  if (mFirstRecordEntry > 0)
    mStorage->DecrementRecordRef(mFirstRecordEntry);
}

bool
RowFieldArray::IsShared() const
{
  return true;
}

void
RowFieldArray::RawRead(const uint64_t offset, const uint64_t size, uint8_t* const buffer)
{
  if (mStorage)
  {
    mStorage->GetRecord(mFirstRecordEntry, offset + METADATA_SIZE, size, buffer);
    return;
  }
  mTempStorage.Read(offset, size, buffer);
}


void
RowFieldArray::RawWrite(const uint64_t offset, const uint64_t size, const uint8_t* const buffer)
{
  if ( ! mStorage)
  {
    mTempStorage.Write(offset, size, buffer);
    return;
  }

  uint8_t tbuffer[256];
  uint64_t coff = 0;
  while (coff < mElementsCount * mElementRawSize)
  {
    uint_t chunkSize = MIN(sizeof buffer, mElementsCount * mElementRawSize - coff);

    mStorage->GetRecord(mFirstRecordEntry, coff + METADATA_SIZE, chunkSize, tbuffer);
    mTempStorage.Write(coff, chunkSize, tbuffer);

    coff += chunkSize;
  }

  assert(coff == mElementsCount * mElementRawSize);
  mTempStorage.Write(offset, size, buffer);

  mStorage->DecrementRecordRef(mFirstRecordEntry);
  mFirstRecordEntry = 0;
  mStorage.reset();
}

void
RowFieldArray::ColapseRaw(const uint64_t offset, const uint64_t count)
{
  if ( ! mStorage)
  {
    mTempStorage.Colapse(offset, count);
    return;
  }

  uint8_t buffer[256];
  uint64_t coff = 0;
  while (coff < offset)
  {
    uint_t chunkSize = MIN(sizeof buffer, offset - coff);

    mStorage->GetRecord(mFirstRecordEntry, coff + METADATA_SIZE, chunkSize, buffer);
    mTempStorage.Write(coff, chunkSize, buffer);

    coff += chunkSize;
  }

  assert(coff == offset);

  while (coff + count < mElementsCount * mElementRawSize)
  {
    uint_t chunkSize = MIN(sizeof buffer, mElementsCount * mElementRawSize - coff - count);
    mStorage->GetRecord(mFirstRecordEntry, coff + count + METADATA_SIZE, chunkSize, buffer);
    mTempStorage.Write(coff, chunkSize, buffer);

    coff += chunkSize;
  }

  assert(coff + count == mElementsCount * mElementRawSize);

  mStorage->DecrementRecordRef(mFirstRecordEntry);
  mFirstRecordEntry = 0;
  mStorage.reset();
}

uint64_t
RowFieldArray::RawSize() const
{
  return mElementsCount * mElementRawSize;
}

TemporalContainer&
RowFieldArray::GetTemporalContainer()
{
  return mTempStorage;
}

VariableSizeStore&
RowFieldArray::GetRowStorage()
{
  return *mStorage.get();
}


} //namespace pastra
} //namespace whais
