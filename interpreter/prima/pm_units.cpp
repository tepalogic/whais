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
#include <memory.h>

#include "pm_units.h"
#include "interpreter.h"


using namespace std;




namespace whais {
namespace prima {



void
Unit::SetGlobalId( const uint32_t index, const uint32_t id)
{
  if (index >= mGlbsCount)
    throw InterException( _EXTRA( InterException::INVALID_UNIT_GLB_INDEX));

  _RC (uint32_t*, mUnitData)[index] = id;
}


void
Unit::SetProcedureId( const uint32_t index, const uint32_t id)
{
  if (index >= mProcsCount)
    throw InterException( _EXTRA( InterException::INVALID_UNIT_PROC_INDEX));

  _RC (uint32_t*, mUnitData)[mGlbsCount + index] = id;
}


uint32_t
Unit::GetGlobalId( const uint32_t index) const
{
  if (index >= mGlbsCount)
    throw InterException( _EXTRA( InterException::INVALID_UNIT_GLB_INDEX));

  return _RC (const uint32_t*, mUnitData)[index];
}


uint32_t
Unit::GetProcedureId( const uint32_t index) const
{
  if (index >= mProcsCount)
    throw InterException( _EXTRA( InterException::INVALID_UNIT_PROC_INDEX));

  return _RC (const uint32_t*, mUnitData)[mGlbsCount + index];
}


const uint8_t*
Unit::GetConstData( const uint32_t offset) const
{
  if (offset >= mConstSize)
    throw InterException( _EXTRA( InterException::INVALID_UNIT_DATA_OFF));

  const uint8_t* data = mUnitData;

  data += (mGlbsCount + mProcsCount) * sizeof( uint32_t) + offset;

  return data;
}




UnitsManager::~UnitsManager()
{
  for (uint32_t index = 0; index < mUnits.size(); ++index)
    delete [] _RC (uint8_t*, mUnits[index]);
}


uint32_t
UnitsManager::AddUnit( const uint32_t     glbsCount,
                       const uint32_t     procsCount,
                       const uint8_t*     constData,
                       const uint32_t     constDataSize)
{
  const uint_t entrySize = sizeof( Unit) +
                           (glbsCount + procsCount) * sizeof( uint32_t) +
                           constDataSize;

  auto_ptr<uint8_t> entryBuff( new uint8_t[entrySize]);
  Unit* const       entry = _RC (Unit*, entryBuff.get ());

  entry->mGlbsCount  = glbsCount;
  entry->mProcsCount = procsCount;
  entry->mConstSize  = constDataSize;

  memcpy( entry->mUnitData + ((glbsCount + procsCount) * sizeof( uint32_t)),
          constData,
          constDataSize);

  const uint32_t result = mUnits.size();
  mUnits.push_back( entry);

  entryBuff.release();

  return result;
}


void
UnitsManager::RemoveLastUnit()
{
  assert( mUnits.size() > 0);

  delete [] _RC (uint8_t*, mUnits[mUnits.size() - 1]);
  mUnits.pop_back();
}


Unit&
UnitsManager::GetUnit( const uint32_t unitIndex)
{
  assert( unitIndex < mUnits.size());

  return *mUnits[unitIndex];
}


void
UnitsManager::SetGlobalIndex( const uint32_t unitIndex,
                              const uint32_t unitGlbIndex,
                              const uint32_t glbMgrIndex)
{
  assert( unitIndex < mUnits.size());

  mUnits[unitIndex]->SetGlobalId( unitGlbIndex, glbMgrIndex);
}


void
UnitsManager::SetProcedureIndex( const uint32_t unitIndex,
                                 const uint32_t unitProcIndex,
                                 const uint32_t procMgrIndex)
{
  assert( unitIndex < mUnits.size());

  mUnits[unitIndex]->SetProcedureId( unitProcIndex, procMgrIndex);
}


uint32_t
UnitsManager::GetGlobalIndex( const uint32_t unitIndex,
                              const uint32_t unitGlbIndex) const
{
  assert( unitIndex < mUnits.size());

 return mUnits[unitIndex]->GetGlobalId( unitGlbIndex);
}


uint32_t
UnitsManager::GetProcedureIndex( const uint32_t     unitIndex,
                                 const uint32_t     unitProcIndex) const
{
  assert( unitIndex < mUnits.size());

  return mUnits[unitIndex]->GetProcedureId( unitProcIndex);
}


} //namespace prima
} //namespace whais

