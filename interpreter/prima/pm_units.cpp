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
Unit::SetGlobalId (const D_UINT32 globalIndex, const D_UINT32 globalId)
{
  if (globalIndex >= m_GlbsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_GLB_INDEX));

  _RC (D_UINT32*, m_UnitData)[globalIndex] = globalId;
}

void
Unit::SetProcId (const D_UINT32 procIndex, const D_UINT32 procId)
{
  if (procIndex >= m_ProcsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_PROC_INDEX));

  _RC (D_UINT32*, m_UnitData)[m_GlbsCount + procIndex] = procId;
}

D_UINT32
Unit::GetGlobalId (const D_UINT32 globalIndex) const
{
  if (globalIndex >= m_GlbsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_GLB_INDEX));

  return _RC (const D_UINT32*, m_UnitData)[globalIndex];
}

D_UINT32
Unit::GetProcId (const D_UINT32 procIndex) const
{
  if (procIndex >= m_ProcsCount)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_PROC_INDEX));

  return _RC (const D_UINT32*, m_UnitData)[m_GlbsCount + procIndex];
}

const D_UINT8*
Unit::GetConstData (const D_UINT32 offset) const
{
  if (offset >= m_ConstSize)
    throw InterException (NULL,
                          _EXTRA (InterException::INVALID_UNIT_DATA_OFF));

  const D_UINT8* pData = m_UnitData;

  pData += (m_GlbsCount + m_ProcsCount) * sizeof (D_UINT32) + offset;

  return pData;
}

/////////////////////////////////UnitsManager//////////////////////////////////

UnitsManager::~UnitsManager ()
{
  for (D_UINT32 index = 0; index < m_Units.size (); ++index)
    delete [] _RC (D_UINT8*, m_Units[index]);
}

D_UINT32
UnitsManager::AddUnit (const D_UINT32 glbsCount,
                       const D_UINT32 procsCount,
                       const D_UINT8* pConstData,
                       const D_UINT32 constAreaSize)
{
  const D_UINT entrySize = sizeof (Unit) +
                           (glbsCount + procsCount) * sizeof (D_UINT32) +
                           constAreaSize;
  auto_ptr<D_UINT8> apUnitData (new D_UINT8[entrySize]);
  Unit* const  pEntry = _RC (Unit*, apUnitData.get ());

  pEntry->m_GlbsCount  = glbsCount;
  pEntry->m_ProcsCount = procsCount;
  pEntry->m_ConstSize  = constAreaSize;

  memcpy (pEntry->m_UnitData + ((glbsCount + procsCount) * sizeof (D_UINT32)),
          pConstData,
          constAreaSize);

  const D_UINT32 result = m_Units.size();
  m_Units.push_back (pEntry);
  apUnitData.release ();

  return result;
}

void
UnitsManager::RemoveLastUnit ()
{
  assert (m_Units.size () > 0);

  delete [] _RC (D_UINT8*, m_Units[m_Units.size () - 1]);
  m_Units.pop_back ();
}

Unit&
UnitsManager::GetUnit (const D_UINT32 unitIndex)
{
  assert (unitIndex < m_Units.size ());

  return *m_Units[unitIndex];
}

void
UnitsManager::SetGlobalIndex (const D_UINT32 unitIndex,
                              const D_UINT32 unitGlbIndex,
                              const D_UINT32 glbMgrIndex)
{
  assert (unitIndex < m_Units.size ());

  m_Units[unitIndex]->SetGlobalId (unitGlbIndex, glbMgrIndex);
}

void
UnitsManager::SetProcIndex (const D_UINT32 unitIndex,
                            const D_UINT32 unitProcIndex,
                            const D_UINT32 procMgrIndex)
{
  assert (unitIndex < m_Units.size ());

  m_Units[unitIndex]->SetProcId (unitProcIndex, procMgrIndex);
}

D_UINT32
UnitsManager::GetGlobalIndex (const D_UINT32 unitIndex,
                              const D_UINT32 unitGlbIndex) const
{
  assert (unitIndex < m_Units.size ());

 return m_Units[unitIndex]->GetGlobalId (unitGlbIndex);
}

D_UINT32
UnitsManager::GetProcIndex (const D_UINT32 unitIndex,
                            const D_UINT32 unitProcIndex) const
{
  assert (unitIndex < m_Units.size ());

  return m_Units[unitIndex]->GetProcId (unitProcIndex);
}

