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

#ifndef PS_ARRAYSTRATEGY_H_
#define PS_ARRAYSTRATEGY_H_

#include "whais.h"

#include "ps_container.h"
#include "ps_varstorage.h"
#include "ps_serializer.h"

namespace whais {

namespace pastra {

//Just forward class declarations
class TemporalArray;
class RowFieldArray;
class NullArray;
class PrototypeTable;
}

class IArrayStrategy
{
  friend class std::auto_ptr<IArrayStrategy>;
  friend class pastra::PrototypeTable;

public:
  uint64_t Count();
  DBS_BASIC_TYPE Type();

  uint_t          Get (const uint64_t index, uint8_t* const dest);
  IArrayStrategy* Set (const DBS_BASIC_TYPE type,
                       const uint8_t* const rawValue,
                       const uint64_t       index);
  IArrayStrategy* Add (const DBS_BASIC_TYPE type,
                       const uint8_t* const rawValue,
                       uint64_t* const      outIndex);

  IArrayStrategy* Remove (const uint64_t index);

  IArrayStrategy* Sort (const bool reverse);

  IArrayStrategy*  MakeMirrorCopy ();
  IArrayStrategy*  MakeClone ();

  virtual void  ReleaseReference ();

  virtual pastra::TemporalContainer& GetTemporalContainer ();
  virtual pastra::VariableSizeStore& GetRowStorage ();


  template<typename TE>
  class ArrayContainer
  {
  public:
    ArrayContainer( IArrayStrategy& array)
      : mArray( array)
    {
      assert (mArray.mElementsCount > 0);
      assert (mArray.mElementRawSize > 0);
    }

    const TE operator[] (const int64_t index) const
    {
      assert (index < mArray.mElementsCount);

      uint8_t rawValue[pastra::Serializer::MAX_VALUE_RAW_SIZE];
      mArray.RawRead (index * mArray.mElementRawSize,
                      mArray.mElementRawSize,
                      rawValue);

      TE result;
      pastra::Serializer::Load (rawValue, &result);

      return result;
    }

    void Exchange( const int64_t pos1, const int64_t pos2)
    {
      uint8_t rawValue1[pastra::Serializer::MAX_VALUE_RAW_SIZE];
      uint8_t rawValue2[pastra::Serializer::MAX_VALUE_RAW_SIZE];

      mArray.RawRead (pos1 * mArray.mElementRawSize,
                      mArray.mElementRawSize,
                      rawValue1);
      mArray.RawRead (pos2 * mArray.mElementRawSize,
                      mArray.mElementRawSize,
                      rawValue2);

      mArray.RawWrite (pos2 * mArray.mElementRawSize,
                       mArray.mElementRawSize,
                       rawValue1);
      mArray.RawWrite (pos1 * mArray.mElementRawSize,
                       mArray.mElementRawSize,
                       rawValue2);
    }

    uint64_t Count() const
    {
      return mArray.mElementsCount;
    }

    void Pivot (const int64_t index)
    {
      mPivot = this->operator[] (index);
    }

    const TE& Pivot() const
    {
      return mPivot;
    }

  private:
    IArrayStrategy&     mArray;
    TE                  mPivot;
  };

protected:
  IArrayStrategy( const DBS_BASIC_TYPE elemsType);
  virtual ~IArrayStrategy();

  virtual uint32_t ReferenceCount() const;
  virtual uint32_t MirrorsCount() const;

  virtual IArrayStrategy* Clone ();

  virtual void RawRead( const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer) = 0;
  virtual void RawWrite( const uint64_t         offset,
                         const uint64_t         size,
                         const uint8_t* const   buffer) = 0;
  virtual void ColapseRaw( const uint64_t offset, const uint64_t count) = 0;
  virtual uint64_t RawSize() const = 0;

  uint64_t              mElementsCount;
  uint_t                mMirrorsCount;
  uint_t                mCopyReferences;
  const DBS_BASIC_TYPE  mElementsType;
  uint_t                mElementRawSize;
  Lock                  mLock;

private:
  IArrayStrategy( const IArrayStrategy&);
  IArrayStrategy& operator= (const IArrayStrategy&);
};




namespace pastra
{



class NullArray : public IArrayStrategy
{
public:
  NullArray( const DBS_BASIC_TYPE elemsType);

  static NullArray& GetSingletoneInstace( const DBS_BASIC_TYPE type);
  virtual void  ReleaseReference ();

protected:
  virtual uint32_t ReferenceCount() const;
  virtual uint32_t MirrorsCount() const;

  virtual void RawRead( const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer);
  virtual void RawWrite( const uint64_t         offset,
                         const uint64_t         size,
                         const uint8_t* const   buffer);
  virtual void ColapseRaw( const uint64_t offset, const uint64_t count);
  virtual uint64_t RawSize() const;
};



class TemporalArray : public IArrayStrategy
{
  friend class PrototypeTable;

public:
  TemporalArray( const DBS_BASIC_TYPE elemsType);

protected:
  virtual ~TemporalArray();

  virtual void RawRead( const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer);
  virtual void RawWrite( const uint64_t         offset,
                         const uint64_t         size,
                         const uint8_t* const   buffer);
  virtual void ColapseRaw( const uint64_t offset, const uint64_t count);
  virtual uint64_t RawSize() const;

  virtual pastra::TemporalContainer& GetTemporalContainer ();

private:
  TemporalArray( const TemporalArray&);
  TemporalArray& operator= (const TemporalArray&);

  TemporalContainer mStorage;
};

class RowFieldArray : public IArrayStrategy
{
  friend class IArrayStrategy;
  friend class PrototypeTable;

public:
  RowFieldArray( VariableSizeStore&       storage,
                 const uint64_t           firstRecordEntry,
                 const DBS_FIELD_TYPE     type);

protected:
  ~RowFieldArray();

  virtual void RawRead( const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer);
  virtual void RawWrite( const uint64_t         offset,
                         const uint64_t         size,
                         const uint8_t* const   buffer);
  virtual void ColapseRaw( const uint64_t offset, const uint64_t count);
  virtual uint64_t RawSize() const;

  virtual pastra::TemporalContainer& GetTemporalContainer ();
  virtual pastra::VariableSizeStore& GetRowStorage ();

private:
  RowFieldArray( const RowFieldArray&);
  RowFieldArray& operator= (const RowFieldArray&);

  uint64_t             mFirstRecordEntry;
  VariableSizeStore&   mStorage;
  TemporalContainer    mTempStorage;

  static const uint_t METADATA_SIZE = sizeof (uint64_t);
};

} //namespace pastra
} //namespace whais

#endif /* PS_ARRAYSTRATEGY_H_ */

