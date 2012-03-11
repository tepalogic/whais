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

#include "pm_procedures.h"
#include "pm_interpreter.h"
#include "pm_typemanager.h"

using namespace std;
using namespace prima;

D_UINT32
ProcedureManager::AddProcedure (const D_UINT8*    pIndentifier,
                                const D_UINT      identifierLength,
                                const D_UINT32    localsCount,
                                const D_UINT32    argsCount,
                                const D_UINT32    syncCount,
                                const StackValue* pLocalValues,
                                const D_UINT32*   pTypesOffset,
                                const D_UINT8*    pCode,
                                const D_UINT32    codeSize)
{
  assert (GetProcedure (pIndentifier, identifierLength) == INVALID_ENTRY);
  assert (argsCount < localsCount);

  ProcedureEntry entry;

  entry.m_LocalsCount = localsCount;
  entry.m_ArgsCount   = argsCount;
  entry.m_SyncCount   = syncCount;
  entry.m_SyncIndex   = m_SyncStmts.size ();
  entry.m_LocalsIndex = m_LocalsValues.size ();
  entry.m_IdIndex     = m_Identifiers.size ();
  entry.m_TypeOff     = m_LocalsTypes.size ();
  entry.m_CodeIndex   = m_Definitions.size ();
  entry.m_CodeSize    = codeSize;

  const D_UINT32 result = m_ProcsEntrys.size ();


  m_SyncStmts.insert (m_SyncStmts.end(), syncCount, false);
  m_LocalsValues.insert (m_LocalsValues.end (),
                         pLocalValues,
                         pLocalValues + (localsCount - argsCount));
  m_Identifiers.insert (m_Identifiers.end (), pIndentifier, pIndentifier + identifierLength);
  m_Identifiers.push_back (0);
  m_Definitions.insert (m_Definitions.end (), pCode, pCode + codeSize);
  m_LocalsTypes.insert (m_LocalsTypes.end (), pTypesOffset, pTypesOffset + localsCount);
  m_ProcsEntrys.push_back (entry);


  return result;
}

const D_UINT32
ProcedureManager::GetLocalsCount (const D_UINT procIndex)
{
  assert (procIndex < m_ProcsEntrys.size ());

  if (procIndex >= m_ProcsEntrys.size ())
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_PROC_REQUEST));

  return m_ProcsEntrys[procIndex].m_LocalsCount;
}

const D_UINT32
ProcedureManager::GetArgsCount (const D_UINT procIndex)
{
  assert (procIndex < m_ProcsEntrys.size ());

  if (procIndex >= m_ProcsEntrys.size ())
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_PROC_REQUEST));

  return m_ProcsEntrys[procIndex].m_ArgsCount;
}

D_UINT32
ProcedureManager::GetProcedure (const D_UINT8* pIndetifier, const D_UINT identifierLength)
{
  for (D_UINT32 index = 0; index < m_ProcsEntrys.size (); ++index)
    {
      if (strncmp (_RC (const D_CHAR*, pIndetifier),
                   _RC (const D_CHAR*,
                        &m_Identifiers[m_ProcsEntrys[index].m_IdIndex]),
                   identifierLength) == 0)
        return index;
    }

  return INVALID_ENTRY;
}

const StackValue&
ProcedureManager::GetLocalValue (const D_UINT procIndex, const D_UINT32 localIndex)
{
  assert (procIndex < m_ProcsEntrys.size ());

  if (procIndex >= m_ProcsEntrys.size ())
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_PROC_REQUEST));

  const ProcedureEntry& entry = m_ProcsEntrys[procIndex];

  assert (localIndex < entry.m_LocalsCount);
  assert ((localIndex == 0) || (localIndex >= entry.m_ArgsCount));

  if ((localIndex >= entry.m_LocalsCount) ||
      ((localIndex != 0) && (localIndex < entry.m_ArgsCount)) )
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_PROC_LOCAL_REQUEST));

  if (localIndex == 0)
    return m_LocalsValues[entry.m_LocalsIndex];

  return m_LocalsValues[entry.m_LocalsIndex + (localIndex - entry.m_ArgsCount)];
}

const D_UINT8*
ProcedureManager::GetLocalType (const D_UINT procIndex, const D_UINT32 localIndex)
{
  assert (procIndex < m_ProcsEntrys.size ());

  if (procIndex >= m_ProcsEntrys.size ())
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_PROC_REQUEST));

  const ProcedureEntry& entry = m_ProcsEntrys[procIndex];

  assert (localIndex < entry.m_LocalsCount);

  if (localIndex >= entry.m_LocalsCount)
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_PROC_LOCAL_REQUEST));

  const TypeManager& typeMgr = m_Session.GetTypeManager ();
  return typeMgr.GetTypeDescription (m_LocalsTypes[entry.m_TypeOff + localIndex]);

}

const D_UINT8*
ProcedureManager::GetCode (const D_UINT procIndex, D_UINT64* pOutCodeSize)
{
  assert (procIndex < m_ProcsEntrys.size ());

  if (procIndex >= m_ProcsEntrys.size ())
    throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_PROC_REQUEST));

  const ProcedureEntry& entry = m_ProcsEntrys[procIndex];

  if (pOutCodeSize != NULL)
    *pOutCodeSize = entry.m_CodeSize;

  return &m_Definitions[entry.m_CodeIndex];
}
