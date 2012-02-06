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

namespace pastra
{
  class TemporalArray;
  class RowFieldArray;
}

class I_ArrayStrategy
{
public:

  I_ArrayStrategy (const DBS_FIELD_TYPE elemsType);
  virtual ~I_ArrayStrategy ();

  D_UINT64  GetElementsCount () const { return m_ElementsCount; };
  void      IncrementElementsCount () { ++ m_ElementsCount; };
  void      DecrementElementsCount () { -- m_ElementsCount; };

  DBS_FIELD_TYPE GetElementsType () const { return m_ElementsType; };

  virtual D_UINT GetReferenceCount () const;
  virtual void   IncrementReferenceCount ();
  virtual void   DecrementReferenceCount ();

  virtual void     ReadRawData (const D_UINT64 offset, const D_UINT64 length, D_UINT8 *const pData) = 0;
  virtual void     WriteRawData (const D_UINT64 offset, const D_UINT64 length, const D_UINT8 *const pData) = 0;
  virtual void     CollapseRawData (const D_UINT64 offset, const D_UINT64 count) = 0;
  virtual D_UINT64 GetRawDataSize () const = 0 ;

  void Clone (I_ArrayStrategy &strategy);

  virtual bool IsRowValue () const;

  virtual pastra::TemporalArray& GetTemporal();
  virtual pastra::RowFieldArray& GetRowValue();

protected:
  D_UINT64              m_ElementsCount;
  D_UINT                m_ReferenceCount;
  const DBS_FIELD_TYPE  m_ElementsType;

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

protected:
  //Overwrites of I_ArrayStrategy
  virtual D_UINT GetReferenceCount () const;
  virtual void   IncrementReferenceCount ();
  virtual void   DecrementReferenceCount ();

  //Implements of I_ArrayStrategy
  virtual void     ReadRawData (const D_UINT64 offset, const D_UINT64 length, D_UINT8 *const pData);
  virtual void     WriteRawData (const D_UINT64 offset, const D_UINT64 length, const D_UINT8 *const pData);
  virtual void     CollapseRawData (const D_UINT64 offset, const D_UINT64 count);
  virtual D_UINT64 GetRawDataSize () const;
};

class TemporalArray : public I_ArrayStrategy
{
  friend class PSTable;
public:
  TemporalArray (const DBS_FIELD_TYPE elemsType);
  virtual ~TemporalArray ();

protected:
   //Implements of I_ArrayStrategy
  virtual void     ReadRawData (const D_UINT64 offset, const D_UINT64 length, D_UINT8 *const pData);
  virtual void     WriteRawData (const D_UINT64 offset, const D_UINT64 length, const D_UINT8 *const pData);
  virtual void     CollapseRawData (const D_UINT64 offset, const D_UINT64 count);
  virtual D_UINT64 GetRawDataSize () const;

  virtual TemporalArray& GetTemporal();

  TempContainer m_Storage;
};

class RowFieldArray : public I_ArrayStrategy
{
  friend class PSTable;
public:
  RowFieldArray (VariableLengthStore &storage, D_UINT64 firstRecordEntry, DBS_FIELD_TYPE type);
  ~RowFieldArray ();

protected:
  //Overwrites of I_ArrayStrategy
  virtual D_UINT GetReferenceCount () const;
  virtual bool   IsRowValue () const;
  virtual pastra::RowFieldArray& GetRowValue();

  //Implements of I_ArrayStrategy
  virtual void     ReadRawData (const D_UINT64 offset, const D_UINT64 length, D_UINT8 *const pData);
  virtual void     WriteRawData (const D_UINT64 offset, const D_UINT64 length, const D_UINT8 *const pData);
  virtual void     CollapseRawData (const D_UINT64 offset, const D_UINT64 count);
  virtual D_UINT64 GetRawDataSize () const;

  const D_UINT64       m_FirstRecordEntry;
  VariableLengthStore& m_Storage;
private:
  RowFieldArray (const RowFieldArray&);
  RowFieldArray& operator= (const RowFieldArray&);

};

}
#endif /* PS_ARRAYSTRATEGY_H_ */
