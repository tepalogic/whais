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
GlobalsManager::AddGlobal (const D_UINT8 *    pIdentifier,
                           const GlobalValue& value,
                           const D_UINT32     typeOffset)
{
  assert (FindGlobal (pIdentifier) == INVALID_ENTRY);
  assert (m_GlobalsEntrys.size () == m_Storage.size ());

  const D_UINT32 result   = m_GlobalsEntrys.size ();
  const D_UINT32 IdOffset = m_Identifiers.size ();

  do
    m_Identifiers.push_back (*pIdentifier);
  while (*pIdentifier++ != 0);

  m_Storage.push_back (value);

  const GlobalEntry entry = {IdOffset, typeOffset};
  m_GlobalsEntrys.push_back (entry);

  return result;
}

D_UINT32
GlobalsManager::FindGlobal (const D_UINT8* pIdentifier)
{
  assert (m_GlobalsEntrys.size () == m_Storage.size ());
  D_UINT32 elIndex = 0;

  while (elIndex < m_GlobalsEntrys.size ())
    {
      const GlobalEntry& entry          = m_GlobalsEntrys[elIndex];
      const D_CHAR*      pEntIdentifier = _RC (const D_CHAR* , &m_Identifiers [entry.m_IdOffet]);

      if (strcmp (pEntIdentifier, _RC (const D_CHAR*, pIdentifier)) == 0)
        return elIndex;

      ++elIndex;
    }

  return INVALID_ENTRY;
}

GlobalValue&
GlobalsManager::GetGlobal (const D_UINT64 globalIndex)
{
  assert (m_GlobalsEntrys.size () == m_Storage.size ());
  assert (globalIndex < m_GlobalsEntrys.size ());

  if (globalIndex >= m_Storage.size ())
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_GLOBAL_REQUEST));

  return m_Storage[globalIndex];
}

const D_UINT8*
GlobalsManager::GetGlobalTypeDesctiption (const D_UINT64 globalIndex)
{
  assert (m_GlobalsEntrys.size () == m_Storage.size ());
  assert (globalIndex < m_GlobalsEntrys.size ());

  if (globalIndex >= m_GlobalsEntrys.size ())
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_GLOBAL_REQUEST));

  TypeManager&   typeMgr = m_Session.GetTypeManager ();
  const D_UINT8* pType   = typeMgr.GetTypeDescription (m_GlobalsEntrys[globalIndex].m_TypeOffset);

  assert (typeMgr.IsTypeDescriptionValid (pType));

  return pType;
}
