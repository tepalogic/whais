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

namespace whisper {
namespace prima {

////////////////////////////////Unit///////////////////////////////////////////

void
Unit::SetGlobalId (const uint32_t globalIndex, const uint32_t globalId)
{
  if (globalIndex >= mGlbsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_GLB_INDEX));

  _RC (uint32_t*, mUnitData)[globalIndex] = globalId;
}

void
Unit::SetProcId (const uint32_t procIndex, const uint32_t procId)
{
  if (procIndex >= mProcsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_PROC_INDEX));

  _RC (uint32_t*, mUnitData)[mGlbsCount + procIndex] = procId;
}

uint32_t
Unit::GetGlobalId (const uint32_t globalIndex) const
{
  if (globalIndex >= mGlbsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_GLB_INDEX));

  return _RC (const uint32_t*, mUnitData)[globalIndex];
}

uint32_t
Unit::GetProcId (const uint32_t procIndex) const
{
  if (procIndex >= mProcsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_PROC_INDEX));

  return _RC (const uint32_t*, mUnitData)[mGlbsCount + procIndex];
}

const uint8_t*
Unit::GetConstData (const uint32_t offset) const
{
  if (offset >= mConstSize)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_DATA_OFF));

  const uint8_t* pData = mUnitData;

  pData += (mGlbsCount + mProcsCount) * sizeof (uint32_t) + offset;

  return pData;
}

/////////////////////////////////UnitsManager//////////////////////////////////

UnitsManager::~UnitsManager ()
{
  for (uint32_t index = 0; index < mUnits.size (); ++index)
    delete [] _RC (uint8_t*, mUnits[index]);
}

uint32_t
UnitsManager::AddUnit (const uint32_t glbsCount,
                       const uint32_t procsCount,
                       const uint8_t* pConstData,
                       const uint32_t constAreaSize)
{
  const uint_t entrySize = sizeof (Unit) +
                           (glbsCount + procsCount) * sizeof (uint32_t) +
                           constAreaSize;
  auto_ptr<uint8_t> apUnitData (new uint8_t[entrySize]);
  Unit* const  pEntry = _RC (Unit*, apUnitData.get ());

  pEntry->mGlbsCount  = glbsCount;
  pEntry->mProcsCount = procsCount;
  pEntry->mConstSize  = constAreaSize;

  memcpy (pEntry->mUnitData + ((glbsCount + procsCount) * sizeof (uint32_t)),
          pConstData,
          constAreaSize);

  const uint32_t result = mUnits.size();
  mUnits.push_back (pEntry);
  apUnitData.release ();

  return result;
}

void
UnitsManager::RemoveLastUnit ()
{
  assert (mUnits.size () > 0);

  delete [] _RC (uint8_t*, mUnits[mUnits.size () - 1]);
  mUnits.pop_back ();
}

Unit&
UnitsManager::GetUnit (const uint32_t unitIndex)
{
  assert (unitIndex < mUnits.size ());

  return *mUnits[unitIndex];
}

void
UnitsManager::SetGlobalIndex (const uint32_t unitIndex,
                              const uint32_t unitGlbIndex,
                              const uint32_t glbMgrIndex)
{
  assert (unitIndex < mUnits.size ());

  mUnits[unitIndex]->SetGlobalId (unitGlbIndex, glbMgrIndex);
}

void
UnitsManager::SetProcIndex (const uint32_t unitIndex,
                            const uint32_t unitProcIndex,
                            const uint32_t procMgrIndex)
{
  assert (unitIndex < mUnits.size ());

  mUnits[unitIndex]->SetProcId (unitProcIndex, procMgrIndex);
}

uint32_t
UnitsManager::GetGlobalIndex (const uint32_t unitIndex,
                              const uint32_t unitGlbIndex) const
{
  assert (unitIndex < mUnits.size ());

 return mUnits[unitIndex]->GetGlobalId (unitGlbIndex);
}

uint32_t
UnitsManager::GetProcIndex (const uint32_t unitIndex,
                            const uint32_t unitProcIndex) const
{
  assert (unitIndex < mUnits.size ());

  return mUnits[unitIndex]->GetProcId (unitProcIndex);
}

} //namespace prima
} //namespace whisper
