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
using namespace prima;

////////////////////////////////Unit///////////////////////////////////////////

void
Unit::SetGlobalId (const uint32_t globalIndex, const uint32_t globalId)
{
  if (globalIndex >= m_GlbsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_GLB_INDEX));

  _RC (uint32_t*, m_UnitData)[globalIndex] = globalId;
}

void
Unit::SetProcId (const uint32_t procIndex, const uint32_t procId)
{
  if (procIndex >= m_ProcsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_PROC_INDEX));

  _RC (uint32_t*, m_UnitData)[m_GlbsCount + procIndex] = procId;
}

uint32_t
Unit::GetGlobalId (const uint32_t globalIndex) const
{
  if (globalIndex >= m_GlbsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_GLB_INDEX));

  return _RC (const uint32_t*, m_UnitData)[globalIndex];
}

uint32_t
Unit::GetProcId (const uint32_t procIndex) const
{
  if (procIndex >= m_ProcsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_PROC_INDEX));

  return _RC (const uint32_t*, m_UnitData)[m_GlbsCount + procIndex];
}

const uint8_t*
Unit::GetConstData (const uint32_t offset) const
{
  if (offset >= m_ConstSize)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_DATA_OFF));

  const uint8_t* pData = m_UnitData;

  pData += (m_GlbsCount + m_ProcsCount) * sizeof (uint32_t) + offset;

  return pData;
}

/////////////////////////////////UnitsManager//////////////////////////////////

UnitsManager::~UnitsManager ()
{
  for (uint32_t index = 0; index < m_Units.size (); ++index)
    delete [] _RC (uint8_t*, m_Units[index]);
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

  pEntry->m_GlbsCount  = glbsCount;
  pEntry->m_ProcsCount = procsCount;
  pEntry->m_ConstSize  = constAreaSize;

  memcpy (pEntry->m_UnitData + ((glbsCount + procsCount) * sizeof (uint32_t)),
          pConstData,
          constAreaSize);

  const uint32_t result = m_Units.size();
  m_Units.push_back (pEntry);
  apUnitData.release ();

  return result;
}

void
UnitsManager::RemoveLastUnit ()
{
  assert (m_Units.size () > 0);

  delete [] _RC (uint8_t*, m_Units[m_Units.size () - 1]);
  m_Units.pop_back ();
}

Unit&
UnitsManager::GetUnit (const uint32_t unitIndex)
{
  assert (unitIndex < m_Units.size ());

  return *m_Units[unitIndex];
}

void
UnitsManager::SetGlobalIndex (const uint32_t unitIndex,
                              const uint32_t unitGlbIndex,
                              const uint32_t glbMgrIndex)
{
  assert (unitIndex < m_Units.size ());

  m_Units[unitIndex]->SetGlobalId (unitGlbIndex, glbMgrIndex);
}

void
UnitsManager::SetProcIndex (const uint32_t unitIndex,
                            const uint32_t unitProcIndex,
                            const uint32_t procMgrIndex)
{
  assert (unitIndex < m_Units.size ());

  m_Units[unitIndex]->SetProcId (unitProcIndex, procMgrIndex);
}

uint32_t
UnitsManager::GetGlobalIndex (const uint32_t unitIndex,
                              const uint32_t unitGlbIndex) const
{
  assert (unitIndex < m_Units.size ());

 return m_Units[unitIndex]->GetGlobalId (unitGlbIndex);
}

uint32_t
UnitsManager::GetProcIndex (const uint32_t unitIndex,
                            const uint32_t unitProcIndex) const
{
  assert (unitIndex < m_Units.size ());

  return m_Units[unitIndex]->GetProcId (unitProcIndex);
}

