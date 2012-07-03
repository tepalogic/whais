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


D_UINT32
GlobalsManager::AddGlobal (const D_UINT8*     pName,
                           const D_UINT       nameLength,
                           const GlobalValue& value,
                           const D_UINT32     typeOffset)
{
  assert (FindGlobal (pName, nameLength) == INVALID_ENTRY);
  assert (m_GlobalsEntrys.size () == m_Storage.size ());

  const D_UINT32 result   = m_GlobalsEntrys.size ();
  const D_UINT32 IdOffset = m_Identifiers.size ();

  m_Identifiers.insert (m_Identifiers.end (), pName, pName + nameLength);
  m_Identifiers.push_back (0);

  m_Storage.push_back (value);

  const GlobalEntry entry = {IdOffset, typeOffset};
  m_GlobalsEntrys.push_back (entry);

  return result;
}

D_UINT32
GlobalsManager::FindGlobal (const D_UINT8* pName,
                            const D_UINT   nameLength)
{
  assert (m_GlobalsEntrys.size () == m_Storage.size ());

  D_UINT32 iterator = 0;

  while (iterator < m_GlobalsEntrys.size ())
    {
      const GlobalEntry& entry      = m_GlobalsEntrys[iterator];
      const D_CHAR*      pEntryName = _RC (const D_CHAR*,
                                           &m_Identifiers [entry.m_IdOffet]);

      if (strncmp (pEntryName, _RC (const D_CHAR*, pName), nameLength) == 0)
        return iterator;

      ++iterator;
    }

  return INVALID_ENTRY;
}

GlobalValue&
GlobalsManager::GetGlobal (const D_UINT32 glbId)
{
  const D_UINT32 index = glbId & ~GLOBAL_ID;

  assert (IsValid (glbId));
  assert (m_GlobalsEntrys.size () == m_Storage.size ());
  assert (index < m_GlobalsEntrys.size ());

  if (index >= m_Storage.size ())
    throw InterException (NULL, _EXTRA (InterException::INVALID_GLOBAL_REQ));

  return m_Storage[index];
}

const D_UINT8*
GlobalsManager::GetGlobalTI (const D_UINT32 glbId)
{
  const D_UINT32 index = glbId & ~GLOBAL_ID;

  assert (IsValid (glbId));
  assert (m_GlobalsEntrys.size () == m_Storage.size ());
  assert (index < m_GlobalsEntrys.size ());

  if (index >= m_GlobalsEntrys.size ())
    throw InterException (NULL, _EXTRA (InterException::INVALID_GLOBAL_REQ));

  TypeManager&   typeMgr = m_Names.GetTypeManager ();
  const D_UINT8* pType   = typeMgr.GetType (m_GlobalsEntrys[index].m_TypeOffset);

  assert (typeMgr.IsTypeValid (pType));

  return pType;
}

