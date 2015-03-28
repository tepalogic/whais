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
#include "utils/wsort.h"
#include "dbs/dbs_mgr.h"
#include "dbs/dbs_types.h"
#include "dbs/dbs_exception.h"

#include "ps_arraystrategy.h"




using namespace std;



namespace whais {



IArrayStrategy::IArrayStrategy (const DBS_FIELD_TYPE elemsType)
  : mElementsCount (0),
    mMirrorsCount (0),
    mCopyReferences (1),
    mElementsType (elemsType),
    mElementRawSize (0)
{
  if (elemsType != T_UNDETERMINED)
  {
    assert ((elemsType >= T_BOOL) && (elemsType < T_TEXT));

    mElementRawSize = pastra::Serializer::Size (mElementsType, false);
  }
}


IArrayStrategy::~IArrayStrategy()
{
}


uint64_t
IArrayStrategy::Count ()
{
  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  return mElementsCount;
}


DBS_BASIC_TYPE
IArrayStrategy::Type ()
{
  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  return mElementsType;
}

uint_t
IArrayStrategy::Get (const uint64_t index,
                     uint8_t* const dest)
{
  LockRAII<Lock> _l (mLock);

  if (index == mElementsCount)
    return 0;

  else if (index > mElementsCount)
    throw DBSException (_EXTRA (DBSException::ARRAY_INDEX_TOO_BIG));

  RawRead (index * mElementRawSize, mElementRawSize, dest);

  return mElementRawSize;
}

IArrayStrategy*
IArrayStrategy::Set (const DBS_BASIC_TYPE type,
                     const uint8_t* const rawValue,
                     const uint64_t       index)
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  std::auto_ptr<IArrayStrategy> array (NULL);
  IArrayStrategy* r = this;

  if (type != mElementsType)
    {
      if ((mElementsType != T_UNDETERMINED) && (index > 0))
        throw DBSException (_EXTRA (DBSException::INVALID_ARRAY_TYPE));

      array.reset (new pastra::TemporalArray (type));
      r = array.get ();
    }
  else if (index > mElementsCount)
    throw DBSException (_EXTRA (DBSException::ARRAY_INDEX_TOO_BIG));

  if (r->ReferenceCount () > 1)
    {
      array.reset (r->Clone ());
      r = array.get ();

      assert (r->mElementsCount == mElementsCount);
      assert (r->mElementRawSize == mElementRawSize);
      assert (r->mElementsType == mElementsType);
    }

  r->RawWrite (index * r->mElementRawSize,
                    r->mElementRawSize,
                    rawValue);

  if (index == r->mElementsCount)
    r->mElementsCount++;

  assert (r->RawSize() % r->mElementsCount == 0);
  assert (r->RawSize() / r->mElementsCount == r->mElementRawSize);

  array.release ();
  return r;
}


IArrayStrategy*
IArrayStrategy::Add (const DBS_BASIC_TYPE type,
                     const uint8_t* const rawValue,
                     uint64_t* const      outIndex)
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  std::auto_ptr<IArrayStrategy> array (NULL);
  IArrayStrategy* r = this;

  assert ((T_UNKNOWN < type) && (type < T_TEXT));

  if (type != mElementsType)
    {
      if (mElementsType != T_UNDETERMINED)
        throw DBSException (_EXTRA (DBSException::INVALID_ARRAY_TYPE));

      array.reset (new pastra::TemporalArray (type));
      r = array.get ();
    }

  if (r->ReferenceCount () > 1)
    {
      array.reset (r->Clone ());
      r = array.get ();

      assert (r->mElementsCount == mElementsCount);
      assert (r->mElementRawSize == mElementRawSize);
      assert (r->mElementsType == mElementsType);
    }

  r->RawWrite (r->mElementsCount * r->mElementRawSize,
                    r->mElementRawSize,
                    rawValue);
  *outIndex = r->mElementsCount++;

  assert (r->RawSize() % r->mElementsCount == 0);
  assert (r->RawSize() / r->mElementsCount == r->mElementRawSize);

  array.release ();

  return r;
}


IArrayStrategy*
IArrayStrategy::Remove (const uint64_t index)
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  if (index > mElementsCount)
    throw DBSException (_EXTRA (DBSException::ARRAY_INDEX_TOO_BIG));

  else if (index == mElementsCount)
    return this;

  if (ReferenceCount () == 1)
    {
      ColapseRaw (mElementRawSize * index, mElementRawSize);
      --mElementsCount;

      return this;
    }

  std::auto_ptr<IArrayStrategy> r (new pastra::TemporalArray (mElementsType));

  assert (r->mElementRawSize == mElementRawSize);
  assert (r->mElementsType == mElementsType);

  uint64_t offset = 0;
  uint8_t buffer[256];
  while (offset < mElementRawSize * index)
    {
      uint_t chunkSize = MIN ( sizeof buffer, mElementRawSize * index);

      RawRead (offset, chunkSize, buffer);
      r->RawWrite (offset, chunkSize, buffer);

      offset += chunkSize;
    }

  assert (offset == mElementRawSize * index);

  offset += mElementRawSize;
  while (offset < mElementRawSize * mElementsCount)
    {
      uint_t chunkSize = MIN (sizeof buffer,
                              mElementRawSize * mElementsCount - offset);
      RawRead (offset, chunkSize, buffer);
      r->RawWrite (offset - mElementRawSize, chunkSize, buffer);

      offset += chunkSize;
    }

  assert (offset == mElementRawSize * mElementsCount);

  r->mElementsCount = mElementsCount - 1;

  return r.release ();
}



IArrayStrategy*
IArrayStrategy::Sort (const bool reverse)
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  if (mElementsCount == 0)
    return this;

  std::auto_ptr<IArrayStrategy> array (NULL);
  IArrayStrategy* r = this;

  assert ((T_UNKNOWN < mElementsType) && (mElementsType < T_TEXT));

  if (r->ReferenceCount () > 1)
    {
      array.reset (r->Clone ());
      r = array.get ();

      assert (r->mElementsCount == mElementsCount);
      assert (r->mElementRawSize == mElementRawSize);
      assert (r->mElementsType == mElementsType);
    }


  switch (mElementsType)
  {
    case T_BOOL:
      {
        ArrayContainer<DBool> temp (*r);
        quick_sort<DBool, ArrayContainer<DBool> > (0,
                                                   mElementsCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_CHAR:
     {
        ArrayContainer<DChar> temp (*r);
        quick_sort<DChar, ArrayContainer<DChar> > (0,
                                                   mElementsCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_DATE:
     {
        ArrayContainer<DDate> temp (*r);
        quick_sort<DDate, ArrayContainer<DDate> > (0,
                                                   mElementsCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_DATETIME:
     {
        ArrayContainer<DDateTime> temp (*r);
        quick_sort<DDateTime, ArrayContainer<DDateTime> > (0,
                                                           mElementsCount - 1,
                                                           reverse,
                                                           temp);
      }
    break;

  case T_HIRESTIME:
     {
        ArrayContainer<DHiresTime> temp (*r);
        quick_sort<DHiresTime, ArrayContainer<DHiresTime> > (0,
                                                             mElementsCount - 1,
                                                             reverse,
                                                             temp);
      }
    break;

  case T_UINT8:
     {
        ArrayContainer<DUInt8> temp (*r);
        quick_sort<DUInt8, ArrayContainer<DUInt8> > (0,
                                                     mElementsCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;

  case T_UINT16:
     {
        ArrayContainer<DUInt16> temp (*r);
        quick_sort<DUInt16, ArrayContainer<DUInt16> > (0,
                                                       mElementsCount - 1,
                                                       reverse,
                                                       temp);
      }
    break;

  case T_UINT32:
     {
        ArrayContainer<DUInt32> temp (*r);
        quick_sort<DUInt32, ArrayContainer<DUInt32> > (0,
                                                       mElementsCount - 1,
                                                       reverse,
                                                       temp);
      }
    break;

  case T_UINT64:
     {
        ArrayContainer<DUInt64> temp (*r);
        quick_sort<DUInt64, ArrayContainer<DUInt64> > (0,
                                                       mElementsCount - 1,
                                                       reverse,
                                                       temp);
      }
    break;

  case T_REAL:
     {
        ArrayContainer<DReal> temp (*r);
        quick_sort<DReal, ArrayContainer<DReal> > (0,
                                                   mElementsCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_RICHREAL:
     {
        ArrayContainer<DRichReal> temp (*r);
        quick_sort<DRichReal, ArrayContainer<DRichReal> > (0,
                                                           mElementsCount - 1,
                                                           reverse,
                                                           temp);
      }
    break;

  case T_INT8:
     {
        ArrayContainer<DInt8> temp (*r);
        quick_sort<DInt8, ArrayContainer<DInt8> > (0,
                                                   mElementsCount - 1,
                                                   reverse,
                                                   temp);
      }
    break;

  case T_INT16:
     {
        ArrayContainer<DInt16> temp (*r);
        quick_sort<DInt16, ArrayContainer<DInt16> > (0,
                                                     mElementsCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;

  case T_INT32:
     {
        ArrayContainer<DInt32> temp (*r);
        quick_sort<DInt32, ArrayContainer<DInt32> > (0,
                                                     mElementsCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;

  case T_INT64:
     {
        ArrayContainer<DInt64> temp (*r);
        quick_sort<DInt64, ArrayContainer<DInt64> > (0,
                                                     mElementsCount - 1,
                                                     reverse,
                                                     temp);
      }
    break;
  default:
    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
  }

  array.release ();
  return r;
}


void
IArrayStrategy::ReleaseReference ()
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  if (MirrorsCount () > 0)
    --mMirrorsCount;

  else
    --mCopyReferences;

  if (ReferenceCount () == 0)
    {
      _l.Release ();
      delete this;
    }
}


uint32_t
IArrayStrategy::ReferenceCount () const
{
  return mCopyReferences;
}

uint32_t
IArrayStrategy::MirrorsCount () const
{
  return mMirrorsCount;
}

IArrayStrategy*
IArrayStrategy::MakeMirrorCopy ()
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  if (ReferenceCount() > 1)
    return Clone ()->MakeMirrorCopy ();

  ++mMirrorsCount;

  assert (mCopyReferences == 1);
  assert (mMirrorsCount > 0);

  return this;
}

IArrayStrategy*
IArrayStrategy::MakeClone ()
{
  LockRAII<Lock> _l (mLock);

  assert ((MirrorsCount () == 0) || (ReferenceCount() == 1));
  assert (ReferenceCount () > 0);

  if (MirrorsCount() > 0)
    {
      ++mMirrorsCount;
      return this;
    }

  ++mCopyReferences;
  return this;
}


IArrayStrategy*
IArrayStrategy::Clone ()
{
  std::auto_ptr<IArrayStrategy> r (new pastra::TemporalArray (mElementsType));

  assert (r->mElementRawSize == mElementRawSize);
  assert (r->mElementsType == mElementsType);

  uint64_t offset = 0;
  uint8_t buffer[256];
  while (offset < mElementRawSize * mElementsCount)
   {
     uint_t chunkSize = MIN (sizeof buffer,
                             mElementRawSize * mElementsCount - offset);

     RawRead (offset, chunkSize, buffer);
     r->RawWrite (offset, chunkSize, buffer);

     offset += chunkSize;
   }
  assert (offset == mElementRawSize * mElementsCount);

  r->mElementsCount = mElementsCount;

  return r.release ();
}

pastra::TemporalContainer&
IArrayStrategy::GetTemporalContainer ()
{
  throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


pastra::VariableSizeStore&
IArrayStrategy::GetRowStorage ()
{
  throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


namespace pastra {



NullArray::NullArray (const DBS_FIELD_TYPE elemsType)
  : IArrayStrategy (elemsType)
{
}


void
NullArray::ReleaseReference ()
{
  return ;
}


uint32_t
NullArray::ReferenceCount() const
{
  return 10; //To force new allocation when it will be modified.
}

uint32_t
NullArray::MirrorsCount () const
{
  return 0;
}


void
NullArray::RawRead (const uint64_t      offset,
                    const uint64_t      size,
                    uint8_t* const      buffer)
{
  if ((offset != 0) || (size != 0))
    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


void
NullArray::RawWrite (const uint64_t           offset,
                     const uint64_t           size,
                     const uint8_t* const     buffer)
{
  if ((offset != 0) || (size != 0))
    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


void
NullArray::ColapseRaw (const uint64_t offset, const uint64_t count)
{
  if ((offset != 0) || (count != 0))
    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}


uint64_t
NullArray::RawSize() const
{
  return 0;
}


NullArray&
NullArray::GetSingletoneInstace (const DBS_FIELD_TYPE type)
{
  static NullArray _boolInstance (T_BOOL);
  static NullArray _charInstance (T_CHAR);
  static NullArray _dateInstance (T_DATE);
  static NullArray _datetimeInstance (T_DATETIME);
  static NullArray _hirestimeInstance (T_HIRESTIME);
  static NullArray _uint8Instance (T_UINT8);
  static NullArray _uint16Instance (T_UINT16);
  static NullArray _uint32Instance (T_UINT32);
  static NullArray _uint64Instance (T_UINT64);
  static NullArray _realInstance (T_REAL);
  static NullArray _richrealInstance (T_RICHREAL);
  static NullArray _int8Instance (T_INT8);
  static NullArray _int16Instance (T_INT16);
  static NullArray _int32Instance (T_INT32);
  static NullArray _int64Instance (T_INT64);
  static NullArray _genericInstance (T_UNDETERMINED);

  switch (type)
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
    throw DBSException (_EXTRA (DBSException::INVALID_ARRAY_TYPE));

  default:

    assert (false);

    throw DBSException (_EXTRA (DBSException::GENERAL_CONTROL_ERROR));
  }
}



TemporalArray::TemporalArray (const DBS_FIELD_TYPE type)
  : IArrayStrategy (type),
    mStorage()
{
}


TemporalArray::~TemporalArray()
{
  assert (mCopyReferences == 0);
}


void
TemporalArray::RawRead (const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer)
{
  mStorage.Read (offset, size, buffer);
}


void
TemporalArray::RawWrite (const uint64_t       offset,
                         const uint64_t       size,
                         const uint8_t* const buffer)
{
  assert ((mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert (mElementRawSize > 0);
  assert ((size % mElementRawSize) == 0);

  mStorage.Write (offset, size, buffer);
}


void
TemporalArray::ColapseRaw (const uint64_t offset, const uint64_t count)
{
  assert ((mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert (mElementRawSize > 0);
  assert ((count % mElementRawSize) == 0);

  mStorage.Colapse (offset, offset + count);
}


uint64_t
TemporalArray::RawSize() const
{
  assert (mStorage.Size () == mElementRawSize * mElementsCount);

  return mElementRawSize * mElementsCount;
}


TemporalContainer&
TemporalArray::GetTemporalContainer ()
{
  return mStorage;
}



RowFieldArray::RowFieldArray (VariableSizeStore&    storage,
                              const uint64_t        firstRecordEntry,
                              const DBS_FIELD_TYPE  type)
  : IArrayStrategy (type),
    mFirstRecordEntry (firstRecordEntry),
    mStorage (storage)
{
  assert (mFirstRecordEntry > 0);
  assert ((mElementsType >= T_BOOL) && (mElementsType < T_TEXT));
  assert (mElementRawSize > 0);

  mStorage.RegisterReference();
  mStorage.IncrementRecordRef (mFirstRecordEntry);

  uint8_t elemetsCount[METADATA_SIZE];

  mStorage.GetRecord (firstRecordEntry, 0, sizeof elemetsCount, elemetsCount);

  mElementsCount = load_le_int64 (elemetsCount);

  assert (mElementsCount > 0);
  assert (mFirstRecordEntry > 0);
}


RowFieldArray::~RowFieldArray()
{
  if (mFirstRecordEntry > 0)
    {
      mStorage.DecrementRecordRef (mFirstRecordEntry);
      //mStorage.Flush ();
      mStorage.ReleaseReference ();
    }
}




void
RowFieldArray::RawRead (const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer)
{
  if (mFirstRecordEntry > 0)
    {
      mStorage.GetRecord (mFirstRecordEntry,
                          offset + METADATA_SIZE,
                          size,
                          buffer);
      return ;
    }

  mTempStorage.Read (offset, size, buffer);
}


void
RowFieldArray::RawWrite (const uint64_t       offset,
                         const uint64_t       size,
                         const uint8_t* const buffer)
{
  if (mFirstRecordEntry == 0)
    {
      mTempStorage.Write (offset, size, buffer);
      return ;
    }

  uint8_t tbuffer[256];
  uint64_t coff = 0;
  while (coff < mElementsCount * mElementRawSize)
    {
      uint_t chunkSize = MIN (sizeof buffer,
                              mElementsCount * mElementRawSize - coff);
      mStorage.GetRecord (mFirstRecordEntry,
                          coff + METADATA_SIZE,
                          chunkSize,
                          tbuffer);
      mTempStorage.Write (coff, chunkSize, tbuffer);

      coff += chunkSize;
    }

  assert (coff == mElementsCount * mElementRawSize);

  mStorage.DecrementRecordRef (mFirstRecordEntry);
  mFirstRecordEntry = 0;
  //mStorage.Flush ();
  mStorage.ReleaseReference ();

  mTempStorage.Write (offset, size, buffer);
}


void
RowFieldArray::ColapseRaw (const uint64_t offset, const uint64_t count)
{

  if (mFirstRecordEntry == 0)
    {
      mTempStorage.Colapse (offset, count);
      return ;
    }

  uint8_t buffer[256];
  uint64_t coff = 0;
  while (coff < offset)
    {
      uint_t chunkSize = MIN (sizeof buffer, offset - coff);

      mStorage.GetRecord (mFirstRecordEntry,
                          coff + METADATA_SIZE,
                          chunkSize,
                          buffer);
      mTempStorage.Write (coff, chunkSize, buffer);

      coff += chunkSize;
    }

  assert (coff == offset);

  while (coff + count < mElementsCount * mElementRawSize)
    {
      uint_t chunkSize = MIN (sizeof buffer,
                              mElementsCount * mElementRawSize - coff - count);
      mStorage.GetRecord (mFirstRecordEntry,
                          coff + count + METADATA_SIZE,
                          chunkSize,
                          buffer);
      mTempStorage.Write (coff, chunkSize, buffer);

      coff += chunkSize;
    }

  assert (coff + count == mElementsCount * mElementRawSize);

  mStorage.DecrementRecordRef (mFirstRecordEntry);
  mFirstRecordEntry = 0;
  //mStorage.Flush ();
  mStorage.ReleaseReference ();
}


uint64_t
RowFieldArray::RawSize() const
{
  return mElementsCount * mElementRawSize;
}


TemporalContainer&
RowFieldArray::GetTemporalContainer ()
{
  return mTempStorage;
}


VariableSizeStore&
RowFieldArray::GetRowStorage ()
{
  return mStorage;
}




} //namespace pastra
} //namespace whais

