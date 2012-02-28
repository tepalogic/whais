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

using namespace std;
using namespace prima;


UnitsManager::~UnitsManager ()
{
  for (D_UINT32 index = 0; index < m_Units.size (); ++index)
    delete [] _RC (D_UINT8*, m_Units[index]);
}

D_UINT32
UnitsManager::LoadUnit (const D_UINT32 glbsCount,
                        const D_UINT32 procsCount,
                        const D_UINT8* pConstData,
                        const D_UINT32 constAreaSize)
{

  const D_UINT entrySize = sizeof (UnitEntry) +
                           (glbsCount + procsCount) * sizeof (D_UINT32) +
                           constAreaSize;
  auto_ptr<D_UINT8> apUnitData (new D_UINT8[entrySize]);
  UnitEntry* const  pEntry = _RC (UnitEntry*, apUnitData.get ());

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

void
UnitsManager::SetGlobalIndex (const D_UINT32 unitIndex,
                              const D_UINT32 unitGlbIndex,
                              const D_UINT32 glbMgrIndex)
{
  assert (unitIndex < m_Units.size ());

  UnitEntry* const pEntry = _RC (UnitEntry*, m_Units[unitIndex]);

  assert (unitGlbIndex < pEntry->m_GlbsCount);

 _RC (D_UINT32*, pEntry->m_UnitData)[unitGlbIndex] = glbMgrIndex;

}

void
UnitsManager::SetProcIndex (const D_UINT32 unitIndex,
                            const D_UINT32 unitProcIndex,
                            const D_UINT32 procMgrIndex)
{
  assert (unitIndex < m_Units.size ());

  UnitEntry* const pEntry = _RC (UnitEntry*, m_Units[unitIndex]);

  assert (unitProcIndex < pEntry->m_ProcsCount);

 _RC (D_UINT32*, pEntry->m_UnitData)[pEntry->m_GlbsCount + unitProcIndex] = procMgrIndex;

}

D_UINT32
UnitsManager::GetGlobalIndex (const D_UINT32 unitIndex,
                              const D_UINT32 unitGlbIndex)
{
  assert (unitIndex < m_Units.size ());

  UnitEntry* const pEntry = _RC (UnitEntry*, m_Units[unitIndex]);

  assert (unitGlbIndex < pEntry->m_GlbsCount);

 return _RC (D_UINT32*, pEntry->m_UnitData)[unitGlbIndex];

}

D_UINT32
UnitsManager::GetProcIndex (const D_UINT32 unitIndex,
                            const D_UINT32 unitProcIndex)
{
  assert (unitIndex < m_Units.size ());

  UnitEntry* const pEntry = _RC (UnitEntry*, m_Units[unitIndex]);

  assert (unitProcIndex < pEntry->m_ProcsCount);

  return _RC (D_UINT32*, pEntry->m_UnitData)[pEntry->m_GlbsCount + unitProcIndex];

}
