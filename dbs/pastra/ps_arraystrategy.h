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

#include "dbs_types.h"

#include "ps_container.h"
#include "ps_varstorage.h"

namespace whisper {

namespace pastra {
  //Just forward class declarations
  class TemporalArray;
  class RowFieldArray;
}

class I_ArrayStrategy
{
public:

  I_ArrayStrategy (const DBS_FIELD_TYPE elemsType);
  virtual ~I_ArrayStrategy ();

  uint64_t  Count () const { return m_ElementsCount; };
#if 0
  void      IncrementCount () { ++m_ElementsCount; };
  void      DecrementCount () { --m_ElementsCount; };
#endif

  DBS_FIELD_TYPE Type () const { return m_ElementsType; };

  virtual uint_t ReferenceCount () const;
  virtual void   IncrementReferenceCount ();
  virtual void   DecrementReferenceCount ();
  virtual uint_t ShareCount () const;
  virtual void   IncrementShareCount ();
  virtual void   DecrementShareCount ();

  //Implements of I_ArrayStrategy
  virtual void     ReadRaw (const uint64_t offset,
                            const uint64_t length,
                            uint8_t *const pData) = 0;
  virtual void     WriteRaw (const uint64_t       offset,
                             const uint64_t       length,
                             const uint8_t *const pData) = 0;
  virtual void     CollapseRaw (const uint64_t offset,
                                const uint64_t count) = 0;
  virtual uint64_t RawSize () const = 0 ;

  void Clone (I_ArrayStrategy& strategy);

  virtual bool IsRowValue () const;

  virtual pastra::TemporalArray& GetTemporal();
  virtual pastra::RowFieldArray& GetRow();

protected:
  uint64_t              m_ElementsCount;
  uint_t                m_ShareCount;
  uint_t                m_ReferenceCount;
  const DBS_FIELD_TYPE  m_ElementsType;
  int                 m_ElementRawSize;

private:
  I_ArrayStrategy (const I_ArrayStrategy&);
  I_ArrayStrategy& operator= (const I_ArrayStrategy&);
};

namespace pastra
{

class NullArray : public I_ArrayStrategy
{
public:
  NullArray (const DBS_FIELD_TYPE elemsType);
  virtual ~NullArray ();

  static NullArray& GetSingletoneInstace (const DBS_FIELD_TYPE type);

  //Overwrites of I_ArrayStrategy
  virtual uint_t ReferenceCount () const;
  virtual void   IncrementReferenceCount ();
  virtual void   DecrementReferenceCount ();
  virtual uint_t ShareCount () const;
  virtual void   IncrementShareCount ();
  virtual void   DecrementShareCount ();

  //Implements of I_ArrayStrategy
  //Implements of I_ArrayStrategy
  virtual void     ReadRaw (const uint64_t offset,
                            const uint64_t length,
                            uint8_t *const pData);
  virtual void     WriteRaw (const uint64_t       offset,
                             const uint64_t       length,
                             const uint8_t *const pData);
  virtual void     CollapseRaw (const uint64_t offset, const uint64_t count);
  virtual uint64_t RawSize () const;
};

class TemporalArray : public I_ArrayStrategy
{
  friend class PrototypeTable;
public:
  TemporalArray (const DBS_FIELD_TYPE elemsType);
  virtual ~TemporalArray ();

  //Implements of I_ArrayStrategy
  virtual void     ReadRaw (const uint64_t offset,
                            const uint64_t length,
                            uint8_t *const pData);
  virtual void     WriteRaw (const uint64_t       offset,
                             const uint64_t       length,
                             const uint8_t *const pData);
  virtual void     CollapseRaw (const uint64_t offset, const uint64_t count);
  virtual uint64_t RawSize () const;

  virtual TemporalArray& GetTemporal();

  TempContainer m_Storage;
};

class RowFieldArray : public I_ArrayStrategy
{
  friend class PrototypeTable;
public:
  RowFieldArray (VLVarsStore& storage,
                 uint64_t firstRecordEntry,
                 DBS_FIELD_TYPE type);
  ~RowFieldArray ();

  //Implements of I_ArrayStrategy
  virtual void     ReadRaw (const uint64_t offset,
                            const uint64_t length,
                            uint8_t *const pData);
  virtual void     WriteRaw (const uint64_t       offset,
                             const uint64_t       length,
                             const uint8_t *const pData);
  virtual void     CollapseRaw (const uint64_t offset, const uint64_t count);
  virtual uint64_t RawSize () const;

  virtual bool           IsRowValue () const;
  virtual RowFieldArray& GetRow();
  virtual TemporalArray& GetTemporal();

  const uint64_t       m_FirstRecordEntry;
  VLVarsStore&         m_Storage;
  TemporalArray*       m_TempArray;

private:
  RowFieldArray (const RowFieldArray&);
  RowFieldArray& operator= (const RowFieldArray&);

  void EnableTemporalStorage ();

  static const uint_t METADATA_SIZE = sizeof (uint64_t);
};

} //namespace pastra
} //namespace whisper

#endif /* PS_ARRAYSTRATEGY_H_ */

