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

#include "whisper.h"

#include "ps_container.h"
#include "ps_varstorage.h"

namespace whisper {

namespace pastra {

//Just forward class declarations
class TemporalArray;
class RowFieldArray;
}



class IArrayStrategy
{
public:

  IArrayStrategy (const DBS_FIELD_TYPE elemsType);
  virtual ~IArrayStrategy ();

  uint64_t Count () const { return mElementsCount; };

  DBS_FIELD_TYPE Type () const { return mElementsType; };

  virtual uint_t ReferenceCount () const;

  virtual void IncrementReferenceCount ();

  virtual void DecrementReferenceCount ();

  virtual uint_t ShareCount () const;

  virtual void IncrementShareCount ();

  virtual void DecrementShareCount ();

  virtual void RawRead (const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer) = 0;

  virtual void RawWrite (const uint64_t         offset,
                         const uint64_t         size,
                         const uint8_t* const   buffer) = 0;

  virtual void ColapseRaw (const uint64_t offset, const uint64_t count) = 0;

  virtual uint64_t RawSize () const = 0 ;

  void Clone (IArrayStrategy& strategy);

  virtual bool IsRowValue () const;

  virtual pastra::TemporalArray& GetTemporal();

  virtual pastra::RowFieldArray& GetRow();

protected:
  uint64_t              mElementsCount;
  uint_t                mShareCount;
  uint_t                mReferenceCount;
  const DBS_FIELD_TYPE  mElementsType;
  uint_t                mElementRawSize;

private:
  IArrayStrategy (const IArrayStrategy&);
  IArrayStrategy& operator= (const IArrayStrategy&);
};




namespace pastra
{



class NullArray : public IArrayStrategy
{
public:
  NullArray (const DBS_FIELD_TYPE elemsType);

  static NullArray& GetSingletoneInstace (const DBS_FIELD_TYPE type);

  virtual uint_t ReferenceCount () const;

  virtual void IncrementReferenceCount ();

  virtual void DecrementReferenceCount ();

  virtual uint_t ShareCount () const;

  virtual void IncrementShareCount ();

  virtual void DecrementShareCount ();

  virtual void RawRead (const uint64_t      offset,
                        const uint64_t      size,
                        uint8_t* const      buffer);

  virtual void RawWrite (const uint64_t           offset,
                         const uint64_t           size,
                         const uint8_t* const     buffer);

  virtual void ColapseRaw (const uint64_t offset, const uint64_t count);

  virtual uint64_t RawSize () const;
};



class TemporalArray : public IArrayStrategy
{
  friend class PrototypeTable;

public:
  TemporalArray (const DBS_FIELD_TYPE elemsType);
  virtual ~TemporalArray ();

  //Implements of IArrayStrategy
  virtual void RawRead (const uint64_t    offset,
                        const uint64_t    size,
                        uint8_t* const    buffer);

  virtual void RawWrite (const uint64_t       offset,
                         const uint64_t       size,
                         const uint8_t* const buffer);

  virtual void ColapseRaw (const uint64_t offset, const uint64_t count);

  virtual uint64_t RawSize () const;

  virtual TemporalArray& GetTemporal();

  TemporalContainer mStorage;
};

class RowFieldArray : public IArrayStrategy
{
  friend class PrototypeTable;

public:
  RowFieldArray (VariableSizeStore&       storage,
                 const uint64_t           firstRecordEntry,
                 const DBS_FIELD_TYPE     type);
  ~RowFieldArray ();

  virtual void RawRead (const uint64_t      offset,
                        const uint64_t      size,
                        uint8_t* const      buffer);

  virtual void RawWrite (const uint64_t       offset,
                         const uint64_t       size,
                         const uint8_t* const buffer);

  virtual void ColapseRaw (const uint64_t offset, const uint64_t count);

  virtual uint64_t RawSize () const;

  virtual bool IsRowValue () const;

  virtual RowFieldArray& GetRow();

  virtual TemporalArray& GetTemporal();

private:
  RowFieldArray (const RowFieldArray&);
  RowFieldArray& operator= (const RowFieldArray&);

  const uint64_t       mFirstRecordEntry;
  VariableSizeStore&   mStorage;
  TemporalArray*       mTempArray;

  void EnableTemporalStorage ();

  static const uint_t METADATA_SIZE = sizeof (uint64_t);
};

} //namespace pastra
} //namespace whisper

#endif /* PS_ARRAYSTRATEGY_H_ */

