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

#include "utils/include/le_converter.h"

#include "dbs/dbs_mgr.h"
#include "dbs_types.h"
#include "dbs_exception.h"

#include "ps_arraystrategy.h"
#include "ps_valintep.h"

using namespace pastra;
using namespace std;

I_ArrayStrategy::I_ArrayStrategy (const DBS_FIELD_TYPE elemsType)
  : m_ElementsCount (0),
    m_ShareCount (0),
    m_ReferenceCount (0),
    m_ElementsType (elemsType),
    m_ElementRawSize (0)
{
  if (elemsType != T_UNDETERMINED)
  {
    assert ((elemsType >= T_BOOL) && (elemsType < T_TEXT));

    m_ElementRawSize = PSValInterp::Alignment (m_ElementsType, false);

    while (m_ElementRawSize < PSValInterp::Size(m_ElementsType, false))
      m_ElementRawSize += PSValInterp::Alignment (m_ElementsType, false);
  }
}

I_ArrayStrategy::~I_ArrayStrategy()
{
  assert (m_ReferenceCount == 0);
  assert (m_ShareCount == 0);
}

D_UINT
I_ArrayStrategy::ReferenceCount() const
{
  return m_ReferenceCount;
}

void
I_ArrayStrategy::IncrementReferenceCount ()
{
  assert (m_ShareCount == 0);
  ++m_ReferenceCount;
}

void
I_ArrayStrategy::DecrementReferenceCount ()
{
  assert (m_ReferenceCount > 0);
  assert (m_ShareCount == 0);

  --m_ReferenceCount;
  if (m_ReferenceCount == 0)
    delete this;
}

D_UINT
I_ArrayStrategy::ShareCount () const
{
  return m_ShareCount;
}

void
I_ArrayStrategy::IncrementShareCount ()
{
  assert (m_ReferenceCount == 1);
  ++m_ShareCount;
}

void
I_ArrayStrategy::DecrementShareCount ()
{
  assert (m_ReferenceCount == 1);
  assert (m_ShareCount > 0);

  --m_ShareCount;
}

void
I_ArrayStrategy::Clone (I_ArrayStrategy& strategy)
{
  assert (strategy.Type() == m_ElementsType);
  assert (this != &strategy);

  D_UINT64 currentPosition = 0;
  D_UINT64 cloneSize = strategy.RawSize();
  while (cloneSize > 0)
    {
      assert (m_ElementRawSize > 0);

      D_UINT8      cloneBuff [128];
      const D_UINT cloneBuffSize = ((sizeof cloneBuff) / m_ElementRawSize) *
                                    m_ElementRawSize;
      D_UINT64     toClone = MIN (cloneBuffSize, cloneSize);

      strategy.ReadRaw (currentPosition, toClone, cloneBuff);
      WriteRaw (currentPosition, toClone, cloneBuff);

      cloneSize -= toClone, currentPosition += toClone;
    }

  m_ElementsCount = strategy.m_ElementsCount;
}

bool
I_ArrayStrategy::IsRowValue() const
{
  return false;
}

TemporalArray&
I_ArrayStrategy::GetTemporal()
{
  throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

  return * _RC(TemporalArray *, this);
}

RowFieldArray&
I_ArrayStrategy::GetRow()
{
  throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));

  return * _RC(RowFieldArray *, this);
}


NullArray::NullArray (const DBS_FIELD_TYPE elemsType)
  : I_ArrayStrategy (elemsType)
{
}

NullArray::~NullArray ()
{
}

D_UINT
NullArray::ReferenceCount () const
{
  return ~0; //To force new allocation when it will be modified.
}

void
NullArray::IncrementReferenceCount ()
{
  return ; //Do nothing as we are a singletone that lifes for ever.
}

void
NullArray::DecrementReferenceCount ()
{
  return ; //Do nothing as we are a singletone that lifes for ever.
}

D_UINT
NullArray::ShareCount () const
{
  return 0;
}

void
NullArray::IncrementShareCount ()
{
  //Someone does not know what is doing!
  throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}

void
NullArray::DecrementShareCount ()
{
  //Someone does not know what is doing!
  throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}

void
NullArray::ReadRaw (const D_UINT64 offset,
                    const D_UINT64 length,
                    D_UINT8*const  pData)
{
  //Someone does not know what is doing!
  throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}
void
NullArray::WriteRaw (const D_UINT64       offset,
                     const D_UINT64       length,
                     const D_UINT8* const pData)
{
  //Someone does not know what is doing!
  throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}

void
NullArray::CollapseRaw (const D_UINT64 offset, const D_UINT64 count)
{
  //Someone does not know what is doing!
  throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
}

D_UINT64
NullArray::RawSize () const
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
    throw DBSException (NULL, _EXTRA (DBSException::INVALID_ARRAY_TYPE));
  default:
    assert (0);
    throw DBSException (NULL, _EXTRA (DBSException::GENERAL_CONTROL_ERROR));
  }
}



TemporalArray::TemporalArray (DBS_FIELD_TYPE type)
  : I_ArrayStrategy (type),
    m_Storage (DBSGetSeettings ().m_TempDir.c_str(),
               DBSGetSeettings ().m_VLValueCacheSize)
{
}

TemporalArray::~TemporalArray ()
{
  assert (m_ReferenceCount == 0);
}

void
TemporalArray::ReadRaw (const D_UINT64 offset,
                        const D_UINT64 length,
                        D_UINT8 *const pData)
{
  m_Storage.Read (offset, length, pData);
}
void
TemporalArray::WriteRaw (const D_UINT64       offset,
                         const D_UINT64       length,
                         const D_UINT8 *const pData)
{
  assert ((m_ElementsType >= T_BOOL) && (m_ElementsType < T_TEXT));
  assert (m_ElementRawSize > 0);
  assert ((length % m_ElementRawSize) == 0);

  m_Storage.Write (offset, length, pData);

  m_ElementsCount = m_Storage.Size () / m_ElementRawSize;
}

D_UINT64
TemporalArray::RawSize () const
{
  return m_Storage.Size() ;
}

TemporalArray&
TemporalArray::GetTemporal ()
{
  return *this;
}

void
TemporalArray::CollapseRaw (const D_UINT64 offset, const D_UINT64 count)
{
  assert ((m_ElementsType >= T_BOOL) && (m_ElementsType < T_TEXT));
  assert (m_ElementRawSize > 0);
  assert ((count % m_ElementRawSize) == 0);

  m_Storage.Colapse (offset, offset + count);

  m_ElementsCount = m_Storage.Size () / m_ElementRawSize;
}


RowFieldArray::RowFieldArray (VLVarsStore&   storage,
                              D_UINT64       firstRecordEntry,
                              DBS_FIELD_TYPE type)
  : I_ArrayStrategy (type),
    m_FirstRecordEntry (firstRecordEntry),
    m_Storage (storage),
    m_TempArray (NULL)
{
  assert (m_FirstRecordEntry > 0);

  assert ((m_ElementsType >= T_BOOL) && (m_ElementsType < T_TEXT));
  assert (m_ElementRawSize > 0);

  m_Storage.RegisterReference ();
  m_Storage.IncrementRecordRef (m_FirstRecordEntry);

  D_UINT8   elemetsCount[METADATA_SIZE];

  m_Storage.GetRecord (firstRecordEntry,
                       0,
                       sizeof elemetsCount,
                       elemetsCount);

  m_ElementsCount = from_le_int64 (elemetsCount);
}

RowFieldArray::~RowFieldArray ()
{
  assert (m_ReferenceCount == 0);

  if (m_TempArray == NULL)
    {
      m_Storage.DecrementRecordRef (m_FirstRecordEntry);
      m_Storage.Flush ();
      m_Storage.ReleaseReference();
    }
  else
    m_TempArray->DecrementReferenceCount ();
}

bool
RowFieldArray::IsRowValue () const
{
  return (m_TempArray == NULL);
}

RowFieldArray&
RowFieldArray::GetRow()
{
  return *this;
}

TemporalArray&
RowFieldArray::GetTemporal ()
{
  assert (m_TempArray != NULL);
  return *m_TempArray;
}

void
RowFieldArray::ReadRaw (const D_UINT64 offset,
                        const D_UINT64 length,
                        D_UINT8* const pData)
{

  if (m_TempArray)
    m_TempArray->ReadRaw (offset, length, pData);
  else
    {
      m_Storage.GetRecord (m_FirstRecordEntry,
                           offset + METADATA_SIZE,
                           length,
                           pData);
    }
}
void
RowFieldArray::WriteRaw (const D_UINT64       offset,
                         const D_UINT64       length,
                         const D_UINT8 *const pData)
{
  if (m_TempArray)
    {
      m_TempArray->WriteRaw (offset, length, pData);
      m_ElementsCount = RawSize () / m_ElementRawSize;
    }
  else
    {
      assert ((length % m_ElementRawSize) == 0);
      m_Storage.UpdateRecord (m_FirstRecordEntry,
                              offset + METADATA_SIZE,
                              length,
                              pData);
      if ((offset + length) > RawSize ())
        {
          assert (((offset + length - RawSize ()) % m_ElementRawSize) == 0);

          m_ElementsCount += (offset + length - RawSize ()) /
                             m_ElementRawSize;

          D_UINT8 temp[METADATA_SIZE];
          store_le_int64 (m_ElementsCount, temp);
          m_Storage.UpdateRecord (m_FirstRecordEntry,
                                  0,
                                  sizeof (temp),
                                  temp);

        }
    }


}

D_UINT64
RowFieldArray::RawSize () const
{
  if (m_TempArray)
    return m_TempArray->RawSize ();

  assert (m_ElementRawSize > 0);

  return m_ElementsCount * m_ElementRawSize;
}

void
RowFieldArray::CollapseRaw (const D_UINT64 offset, const D_UINT64 count)
{
  if (m_TempArray)
    {
      m_TempArray->CollapseRaw (offset, count);
      return;
    }
  else
    EnableTemporalStorage ();

  m_TempArray->CollapseRaw (offset, count);

  assert (m_ElementRawSize > 0);
  assert ((m_TempArray->RawSize () % m_ElementRawSize) == 0);

  m_ElementsCount = m_TempArray->RawSize () / m_ElementRawSize;
}

void
RowFieldArray::EnableTemporalStorage ()
{
  assert (m_TempArray == NULL);

  auto_ptr<TemporalArray> tempArray (new TemporalArray (m_ElementsType));

  tempArray->Clone (*this);
  tempArray->IncrementReferenceCount ();

  m_TempArray = tempArray.release ();

  m_Storage.DecrementRecordRef (m_FirstRecordEntry);
  m_Storage.ReleaseReference();
}
