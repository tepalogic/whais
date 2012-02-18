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

#include <string.h>

#include "pm_globals.h"

using namespace prima;


D_UINT
GlobalManager::AddGlobal (const D_UINT8 *const pIdentifier,
                          const D_UINT32       typeOffset,
                          bool                 external)
{
  //TODO: Finish this
  return 0;
}

D_UINT
GlobalManager::FindGlobal (const D_UINT8* const pIdentifier)
{
  D_UINT iterator = 0;

  while (iterator < m_GlobalsEntrys.size ())
    {
      const D_UINT8* const pItIdentfier = &m_Identifiers[m_GlobalsEntrys[iterator].m_IdOffet];
      if (strcmp (_RC (const D_CHAR*, pIdentifier),
                  _RC (const D_CHAR*, pItIdentfier)) == 0)
        return iterator;
    }

  return INVALID_ENTRY;
}

bool
GlobalManager::IsResolved (const D_UINT glbEntry)
{
  assert (glbEntry < m_Storage.size ());
  assert (m_Storage.size () == m_GlobalsEntrys.size ());

  return m_GlobalsEntrys[glbEntry].m_Resolved;
}

StackedOperand&
GlobalManager::GetGlobalValue (const D_UINT glbEntry)
{
  assert (glbEntry < m_Storage.size ());
  assert (m_Storage.size () == m_GlobalsEntrys.size ());

  return m_Storage[glbEntry];
}

const GlobalEntry&
GlobalManager::GetGlobalDescriptor (const D_UINT glbEntry)
{
  assert (glbEntry < m_Storage.size ());
  assert (m_Storage.size () == m_GlobalsEntrys.size ());

  return m_GlobalsEntrys[glbEntry];
}
