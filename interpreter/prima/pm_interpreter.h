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

#ifndef PM_INTERPRETER_H_
#define PM_INTERPRETER_H_

#include "interpreter.h"

#include "pm_typemanager.h"
#include "pm_globals.h"
#include "pm_procedures.h"
#include "pm_units.h"

namespace prima
{

class NameSpace
{
public:
  NameSpace (I_DBSHandler& dbsHandler);
  ~NameSpace ();

  I_DBSHandler&     GetDBSHandler () { return m_DbsHandler; }
  TypeManager&      GetTypeManager () { return m_TypeManager; }
  GlobalsManager&   GetGlobalsManager () { return m_GlbsManager; }
  ProcedureManager& GetProcedureManager () { return m_ProcsManager; }
  UnitsManager&     GetUnitsManager () { return m_UnitsManager; }

private:
  I_DBSHandler&    m_DbsHandler;
  TypeManager      m_TypeManager;
  GlobalsManager   m_GlbsManager;
  ProcedureManager m_ProcsManager;
  UnitsManager     m_UnitsManager;
};

class NameSpaceHolder
{
public:
  explicit NameSpaceHolder (NameSpace* pSpace = NULL)
    : m_pSpace (pSpace),
      m_RefsCount (0)
  {
    assert (m_pSpace != NULL);
  }

  ~NameSpaceHolder ()
  {
    assert (m_RefsCount == 0);
    if (m_pSpace != NULL)
      {
        DBSReleaseDatabase (m_pSpace->GetDBSHandler());
        delete m_pSpace;
      }
  }

  NameSpaceHolder (const NameSpaceHolder& source)
    : m_pSpace (source.m_pSpace),
      m_RefsCount (source.m_RefsCount)
  {
    _CC (NameSpace*&, source.m_pSpace) = NULL;
    _CC (D_UINT64&, source.m_RefsCount)  = 0;
  }

  NameSpace& Get () { assert (m_RefsCount > 0); return *m_pSpace; }
  D_UINT64   RefsCount () { return m_RefsCount; }
  void       IncRefsCount () { ++m_RefsCount; }
  void       DecRefsCount () { assert (m_RefsCount > 0); --m_RefsCount; }
  void       ForceRelease () { m_RefsCount = 0; }

private:
  const NameSpaceHolder& operator= (const NameSpaceHolder& source);

  NameSpace* m_pSpace;
  D_UINT64   m_RefsCount;
};

class Session : public I_Session
{
public:
  Session (NameSpaceHolder& globalNames, NameSpaceHolder& privateNames);
  virtual ~Session ();

  virtual void LoadCompiledUnit (WICompiledUnit& unit);
  virtual void ExecuteProcedure (const D_CHAR* const pProcName,
                                 StackValue&         stack);
  virtual void LogMessage (const LOG_LEVEL level, std::string& message);

  D_UINT32       FindGlobal (const D_UINT8* pName, const D_UINT nameLength);
  StackValue     GetGlobalValue (const D_UINT32 globalId);
  const D_UINT8* FindGlobalTI (const D_UINT32 globalId);

  D_UINT32       FindProcedure (const D_UINT8* pName, const D_UINT nameLength);
  D_UINT32       ArgsCount (const D_UINT32 procId);
  D_UINT32       LocalsCount (const D_UINT32 procId);
  const D_UINT8* FindLocalTI (const D_UINT32 procId, const D_UINT32 local);
  Unit&          ProcUnit (const D_UINT32 procId);
  const D_UINT8* ProcCode (const D_UINT32 procId);
  D_UINT64       ProcCodeSize (const D_UINT32 procId);
  StackValue     ProcLocalValue (const D_UINT32 procId, const D_UINT32 local);

private:
  D_UINT32 DefineGlobalValue (const D_UINT8* pName,
                              const D_UINT   nameLength,
                              const D_UINT8* pTI,
                              const bool     external);
  D_UINT32 DefineProcedure (const D_UINT8*    pName,
                            const D_UINT      nameLength,
                            const D_UINT32    localsCount,
                            const D_UINT32    argsCount,
                            const D_UINT32    syncCount,
                            const StackValue* pLocalValues,
                            const D_UINT32*   pTypesOffset,
                            const D_UINT8*    pCode,
                            const D_UINT32    codeSize,
                            const bool        external,
                            Unit&             unit);

  NameSpaceHolder& m_GlobalNames;
  NameSpaceHolder& m_PrivateNames;
};

}

#endif /* PM_INTERPRETER_H_ */

