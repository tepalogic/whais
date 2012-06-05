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

#ifndef PM_PROCEDURES_H_
#define PM_PROCEDURES_H_

#include "whisper.h"

#include "pm_operand.h"

namespace prima
{

struct ProcedureEntry
{
  D_UINT32 m_LocalsCount;
  D_UINT32 m_ArgsCount;
  D_UINT32 m_SyncCount;
  D_UINT32 m_SyncIndex;
  D_UINT32 m_LocalsIndex;
  D_UINT32 m_IdIndex;
  D_UINT32 m_TypeOff;
  D_UINT32 m_CodeIndex;
  D_UINT32 m_CodeSize;
};

class Session;
class ProcedureManager
{
public:
  ProcedureManager (Session& session) :
    m_Session (session)
    {

    }

  ~ProcedureManager ()
    {
    }

  D_UINT32 AddProcedure (const D_UINT8*    pName,
                         const D_UINT      nameLength,
                         const D_UINT32    localsCount,
                         const D_UINT32    argsCount,
                         const D_UINT32    syncCount,
                         const StackValue* pLocalValues,
                         const D_UINT32*   pTypesOffset,
                         const D_UINT8*    pCode,
                         const D_UINT32    codeSize);
  D_UINT32 GetProcedure (const D_UINT8* pName,
                         const D_UINT   nameLength);

  const D_UINT32    LocalsCount (const D_UINT procedure);
  const D_UINT32    ArgsCount (const D_UINT procedure);
  const StackValue& LocalValue (const D_UINT procedure, const D_UINT32 local);
  const D_UINT8*    LocalType (const D_UINT procedure, const D_UINT32 local);
  const D_UINT8*    Code (const D_UINT procedure, D_UINT64* pOutCodeSize);

  static const D_UINT32 INVALID_ENTRY = ~0;

private:
  ProcedureManager (const ProcedureManager&);
  ProcedureManager& operator= (const ProcedureManager&);

protected:
  Session& m_Session;

  std::vector<ProcedureEntry> m_ProcsEntrys;
  std::vector<D_UINT8>        m_Identifiers;
  std::vector<StackValue>     m_LocalsValues;
  std::vector<D_UINT32>       m_LocalsTypes;
  std::vector<D_UINT8>        m_Definitions;
  std::vector<bool>           m_SyncStmts;
};

}

#endif /* PM_PROCEDURES_H_ */

