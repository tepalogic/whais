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
#include <string.h>

#include "pm_globals.h"
#include "pm_interpreter.h"

using namespace std;
using namespace prima;

GlobalsManager::~GlobalsManager ()
{
  for (vector<GlobalValue>::iterator it = m_Storage.begin ();
       it != m_Storage.end ();
       ++it)
    {
      it->GetOperand ().~I_PMOperand();
    }
}

uint32_t
GlobalsManager::AddGlobal (const uint8_t*     pName,
                           const uint_t       nameLength,
                           GlobalValue&       value,
                           const uint32_t     typeOffset)
{
  assert (FindGlobal (pName, nameLength) == INVALID_ENTRY);
  assert (m_GlobalsEntrys.size () == m_Storage.size ());

  const uint32_t result   = m_GlobalsEntrys.size ();
  const uint32_t IdOffset = m_Identifiers.size ();

  m_Identifiers.insert (m_Identifiers.end (), pName, pName + nameLength);
  m_Identifiers.push_back (0);

  m_Storage.push_back (value);

  const GlobalEntry entry = {IdOffset, typeOffset};
  m_GlobalsEntrys.push_back (entry);

  return result;
}

uint32_t
GlobalsManager::FindGlobal (const uint8_t* pName,
                            const uint_t   nameLength)
{
  assert (m_GlobalsEntrys.size () == m_Storage.size ());

  uint32_t iterator = 0;

  while (iterator < m_GlobalsEntrys.size ())
    {
      const GlobalEntry& entry      = m_GlobalsEntrys[iterator];
      const char*      pEntryName = _RC (const char*,
                                           &m_Identifiers [entry.m_IdOffet]);

      if (strncmp (pEntryName, _RC (const char*, pName), nameLength) == 0)
        return iterator;

      ++iterator;
    }

  return INVALID_ENTRY;
}

GlobalValue&
GlobalsManager::GetGlobal (const uint32_t glbId)
{
  const uint32_t index = glbId & ~GLOBAL_ID;

  assert (m_GlobalsEntrys.size () == m_Storage.size ());

  if ((IsValid (glbId) == false) || (index >= m_Storage.size ()))
    throw InterException (NULL, _EXTRA (InterException::INVALID_GLOBAL_REQ));

  return m_Storage[index];
}

const uint8_t*
GlobalsManager::Name (const uint_t index) const
{
  if (index >= m_Storage.size ())
    throw InterException (NULL, _EXTRA (InterException::INVALID_GLOBAL_REQ));

  const GlobalEntry& entry = m_GlobalsEntrys[index];

  return  &m_Identifiers [entry.m_IdOffet];
}

const uint8_t*
GlobalsManager::GetGlobalTI (const uint32_t glbId)
{
  const uint32_t index = glbId & ~GLOBAL_ID;

  assert (IsValid (glbId));
  assert (m_GlobalsEntrys.size () == m_Storage.size ());
  assert (index < m_GlobalsEntrys.size ());

  if (index >= m_GlobalsEntrys.size ())
    throw InterException (NULL, _EXTRA (InterException::INVALID_GLOBAL_REQ));

  TypeManager&   typeMgr = m_Names.GetTypeManager ();
  const uint8_t* pType   = typeMgr.GetType (m_GlobalsEntrys[index].m_TypeOffset);

  assert (typeMgr.IsTypeValid (pType));

  return pType;
}

