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

class Session : public I_Session
{
public:
  Session (I_DBSHandler& dbsHandler);
  virtual ~Session ();

  I_DBSHandler&     GetDBSHandler () { return m_DbsHandler; }
  TypeManager&      GetTypeManager () { return m_TypeManager; }
  GlobalsManager&   GetGlobalsManager () { return m_GlbsManager; }
  ProcedureManager& GetProcedureManager () { return m_ProcsManager; }
  UnitsManager&     GetUnitsManager () { return m_UnitsManager; }

  virtual void LoadCompiledUnit (WICompiledUnit& unit);
  virtual void LogMessage (const LOG_LEVEL level, std::string& message);

protected:
  D_UINT32 DefineGlobalValue (const D_UINT8* pIdentifier,
                              const D_UINT   identifierLength,
                              const D_UINT8* pTypeDescriptor,
                              const bool     external);
  D_UINT32 DefineProcedure (const D_UINT8*    pIdentfier,
                            const D_UINT      identifierLength,
                            const D_UINT32    localsCount,
                            const D_UINT32    argsCount,
                            const D_UINT32    syncCount,
                            const StackValue* pLocalValues,
                            const D_UINT32*   pTypesOffset,
                            const D_UINT8*    pCode,
                            const D_UINT32    codeSize,
                            const bool        external);

  I_DBSHandler&    m_DbsHandler;
  TypeManager      m_TypeManager;
  GlobalsManager   m_GlbsManager;
  ProcedureManager m_ProcsManager;
  UnitsManager     m_UnitsManager;
};

class CommonSession : public Session
{
public:
  CommonSession (I_DBSHandler& dbsHandler);
  virtual ~CommonSession ();

  virtual void LogMessage (const LOG_LEVEL level, std::string& message);
};

}

#endif /* PM_INTERPRETER_H_ */

