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

namespace prima
{


class Session : public I_InterpreterSession
{
public:
  Session () :
    I_InterpreterSession ()
    {
    }

  virtual ~Session ()
  {
  }

  virtual I_DBSHandler&   GetDBSHandler ()     = 0;
  virtual TypeManager&    GetTypeManager ()    = 0;
  virtual GlobalsManager& GetGlobalsManager () = 0;

  //Default implement for I_InterpreterSession
  virtual void LoadCompiledUnit (WICompiledUnit& unit);
  virtual void LogMessage (const LOG_LEVEL level, std::string& message);

protected:
  void DefineGlobalValue (const D_UINT8* pIdentifier,
                          const D_UINT8* pTypeDescriptor,
                          const bool     external);
};

class GlobalSession : public Session
{
public:
  GlobalSession ();
  virtual ~GlobalSession ();

  virtual I_DBSHandler&   GetDBSHandler () { return m_DbsHandler; }
  virtual TypeManager&    GetTypeManager () { return m_TypeManager; }
  virtual GlobalsManager& GetGlobalsManager () { return m_GlbsManager; }

protected:
  I_DBSHandler&  m_DbsHandler;
  TypeManager    m_TypeManager;
  GlobalsManager m_GlbsManager;

};

}

#endif /* PM_INTERPRETER_H_ */