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
#include <iostream>

#include "wthread.h"

#include "dbs/include/dbs_mgr.h"

#include "pm_interpreter.h"

using namespace std;
using namespace prima;

static const D_CHAR            gDBSName[] = "administrator";
static WSynchronizer           gSync;


class SessionHandler
{
public:
  explicit SessionHandler (Session* session) :
    m_pSession (session),
    m_RefsCount (0)
  {
    assert (m_pSession != NULL);
  }

  ~SessionHandler ()
  {
    assert (m_RefsCount == 0);
    if (m_pSession != NULL)
      {
        DBSReleaseDatabase (m_pSession->GetDBSHandler());
        delete m_pSession;
      }
  }

  SessionHandler (const SessionHandler& source) :
    m_pSession (source.m_pSession),
    m_RefsCount (source.m_RefsCount)
  {
    _CC(Session*&, source.m_pSession) = NULL;
    _CC(D_UINT64&, source.m_RefsCount)  = 0;
  }

  Session& GetSession () { assert (m_RefsCount > 0); return *m_pSession; }
  D_UINT64 GetRefCount () { return m_RefsCount; }
  void     IncreaseRefCount () { ++m_RefsCount; }
  void     DecreaseRefCount () { assert (m_RefsCount > 0); --m_RefsCount; }
  void     ForceSessionRelease () { delete m_pSession; m_pSession = NULL, m_RefsCount = 0; }

protected:
  Session* m_pSession;
  D_UINT64 m_RefsCount;
};

static map<string, SessionHandler> gmSessions;

typedef pair<string, SessionHandler> SessionPair;

void
InitInterpreter ()
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmSessions.size () != 0)
    throw InterException (NULL, _EXTRA (InterException::ALREADY_INITED));

  static I_DBSHandler& glbDbsHnd = DBSRetrieveDatabase (gDBSName);

  gmSessions.insert (SessionPair (gDBSName, SessionHandler (new CommonSession (glbDbsHnd))));
}

I_Session&
GetInstance (const D_CHAR* const pName)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmSessions.size () == 0)
    throw InterException (NULL, _EXTRA (InterException::NOT_INITED));


  if (pName == NULL)
    {
      SessionHandler& session = gmSessions.begin ()->second;

      session.IncreaseRefCount ();
      return session.GetSession ();
    }
  else if (strcmp (pName, gDBSName) == 0)
    throw InterException (NULL, _EXTRA (InterException::INVALID_SESSION));

  map<string, SessionHandler>::iterator it = gmSessions.find (pName);
  if (it != gmSessions.end ())
    {
      SessionHandler& session = it->second;

      session.IncreaseRefCount ();
      return session.GetSession ();
    }

  I_DBSHandler& glbDbsHnd = DBSRetrieveDatabase (gDBSName);
  gmSessions.insert (SessionPair (pName, SessionHandler (new Session (glbDbsHnd))));

  SessionHandler& session = gmSessions.find (pName)->second;

  session.IncreaseRefCount ();
  return session.GetSession ();
}

void
ReleaseInstance (I_Session& hInstance)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmSessions.size () == 0)
    throw InterException (NULL, _EXTRA (InterException::NOT_INITED));

  map<string, SessionHandler>::iterator it = gmSessions.begin ();
  while (it != gmSessions.end ())
    {
      SessionHandler& session = it->second;

      if (&session.GetSession () == &hInstance)
        {
          assert (session.GetRefCount () > 0);

          session.DecreaseRefCount ();
          return ;
        }
      ++it;
    }
}

void
CleanInterpreter (const bool forced)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmSessions.size () == 0)
    throw InterException (NULL, _EXTRA (InterException::NOT_INITED));

  map<string, SessionHandler>::iterator it = gmSessions.begin ();

  while (it != gmSessions.end ())
    {
      SessionHandler& session = it->second;

      if (forced)
        session.ForceSessionRelease ();
      else if (session.GetRefCount() != 0)
        throw InterException (NULL, _EXTRA (InterException::SESSION_IN_USE));

      ++it;
    }

  gmSessions.clear ();
}

//////////////////////////////////Session//////////////////////////////////////////////////
Session::Session (I_DBSHandler& dbsHandler) :
    I_Session (),
    m_DbsHandler (dbsHandler),
    m_TypeManager (*this),
    m_GlbsManager (*this),
    m_ProcsManager (*this),
    m_UnitsManager (*this)
{
}

Session::~Session ()
{
}

void
Session::LoadCompiledUnit (WICompiledUnit& unit)
{
  UnitsManager&  unitMgr   = GetUnitsManager ();
  const D_UINT32 unitIndex = unitMgr.LoadUnit (unit.GetGlobalsCount (),
                                               unit.GetProceduresCount (),
                                               unit.RetrieveConstArea (),
                                               unit.GetConstAreaSize());

  TypeManager& typeMgr = GetTypeManager ();

  try
  {
    for (D_UINT glbIndex = 0; glbIndex < unit.GetGlobalsCount(); ++glbIndex)
      {
        const D_UINT         typeOffset      = unit.GetGlobalTypeIndex (glbIndex);
        const D_UINT8* const pTypeDescriptor = unit.RetriveTypeInformation () + typeOffset;
        const D_UINT8* const pIdentifier     = _RC (const D_UINT8*, unit.RetriveGlobalName (glbIndex));
        const D_UINT         idLength        = unit.GetGlobalNameLength (glbIndex);
        const bool           external        = (unit.IsGlobalExternal (glbIndex) != FALSE);


        const D_UINT32 glbDefIndex = DefineGlobalValue (pIdentifier,
                                                        idLength,
                                                        pTypeDescriptor,
                                                        external);
        unitMgr.SetGlobalIndex (unitIndex, glbIndex, glbDefIndex);
      }

    for (D_UINT procIndex = 0; procIndex < unit.GetProceduresCount (); ++procIndex)
      {
        const D_UINT8* const pIdentifier = _RC (const D_UINT8*, unit.RetriveProcName (procIndex));
        const D_UINT         idLength    = unit.GetProcNameSize (procIndex);
        const bool           external    = (unit.IsProcExternal (procIndex) != FALSE);
        const D_UINT         localsCount = unit.GetProcLocalsCount (procIndex);
        const D_UINT         argsCount   = unit.GetProcParametersCount (procIndex);
        vector<D_UINT32>     typesOffset;
        vector<StackValue>   values;

        for (D_UINT localIndex = 0; localIndex < localsCount; ++localIndex)
          {
            const D_UINT8* const pLocalTypeDesc = unit.RetriveTypeInformation () +
                                                  unit.GetProcLocalTypeIndex (procIndex,
                                                                              localIndex);
            const D_UINT      typeDescSize = TypeManager::GetTypeLength (pLocalTypeDesc);
            auto_ptr<D_UINT8> apTypeDesc (new D_UINT8 [typeDescSize]);
            memcpy (apTypeDesc.get (), pLocalTypeDesc, typeDescSize);

            const StackValue value      = typeMgr.CreateLocalValue (apTypeDesc.get ());
            const D_UINT32   typeOffset = typeMgr.AddType (apTypeDesc.get ());

            typesOffset.push_back (typeOffset);

            //Keep a copy of stack values for locals (the parameters shall be already on stack)
            //to avoid construct them every time when the procedure is called.
            if ((localIndex == 0) || (localIndex >= argsCount))
              values.push_back (value);
          }

        const D_UINT32 procDefIndex = DefineProcedure (pIdentifier,
                                                       idLength,
                                                       localsCount,
                                                       argsCount,
                                                       unit.GetProcSyncStatementsCount (procIndex),
                                                       &values[0],
                                                       &typesOffset[0],
                                                       unit.RetriveProcCodeArea (procIndex),
                                                       unit.GetProcCodeAreaSize (procIndex),
                                                       external);

        unitMgr.SetProcIndex (unitIndex, procIndex, procDefIndex);
      }
  }
  catch (...)
  {
      unitMgr.RemoveLastUnit ();
      throw;
  }
}

void
Session::LogMessage (const LOG_LEVEL level, std::string& message)
{

}

D_UINT32
Session::DefineGlobalValue (const D_UINT8* pIdentifier,
                            const D_UINT   identifierLength,
                            const D_UINT8* pTypeDescriptor,
                            const bool     external)
{
  assert (TypeManager::IsTypeValid (pTypeDescriptor));

  if (TypeManager::IsTypeValid (pTypeDescriptor) == false)
    {
      string message = "Could not add the global variable ";

      message += "'";
      message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
      message += "' do to invalid type description.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterException (NULL, _EXTRA (InterException::INVALID_TYPE_DESC));
    }

  GlobalsManager& glbsMgr = GetGlobalsManager ();
  TypeManager&    typeMgr = GetTypeManager ();

  auto_ptr<D_UINT8> apTypeDescriptor (new D_UINT8[TypeManager::GetTypeLength (pTypeDescriptor)]);
  memcpy (apTypeDescriptor.get (), pTypeDescriptor, TypeManager::GetTypeLength (pTypeDescriptor));

  GlobalValue    value      = typeMgr.CreateGlobalValue (apTypeDescriptor.get ());
  const D_UINT32 typeOffset = typeMgr.AddType (apTypeDescriptor.get ());
  const D_UINT32 glbEntry   = glbsMgr.FindGlobal (pIdentifier, identifierLength);

  if (glbEntry == glbsMgr.INVALID_ENTRY)
    {
      if (external)
        {
          string message = "Couldn't not find the definition for external "
                           "declaration of global value '";
          message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
          message += "'.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_FIRST));
        }

      return glbsMgr.AddGlobal (pIdentifier, identifierLength, value, typeOffset);
    }
  else if (external)
    {
      if (memcmp (apTypeDescriptor.get (),
                  glbsMgr.GetGlobalTI (glbEntry),
                  TypeManager::GetTypeLength (apTypeDescriptor.get ()) ) != 0)
        {
          string message = "External declaration of global value '";
          message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
          message += "' has a different type than its definition.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_MISMATCH));
        }
    }
  else
    {
      string message = "Duplicate definition of global value '";
      message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
      message += "'.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterException (NULL, _EXTRA (InterException::DUPLICATE_DEFINITION));
    }

  return glbEntry;
}

D_UINT32
Session::DefineProcedure (const D_UINT8*    pIdentifier,
                          const D_UINT      identifierLength,
                          const D_UINT32    localsCount,
                          const D_UINT32    argsCount,
                          const D_UINT32    syncCount,
                          const StackValue* pLocalValues,
                          const D_UINT32*   pTypesOffset,
                          const D_UINT8*    pCode,
                          const D_UINT32    codeSize,
                          const bool        external)
{
  assert (argsCount < localsCount);
  assert (localsCount > 0);
  assert (external || (codeSize > 0));

  TypeManager& typeMgr = GetTypeManager ();

  for (D_UINT localIndex = 0; localIndex < localsCount; ++ localIndex)
    {
      const D_UINT8 *pTypeDescriptor = typeMgr.GetType (pTypesOffset[localIndex]);
      assert (TypeManager::IsTypeValid (pTypeDescriptor));

      if (TypeManager::IsTypeValid (pTypeDescriptor) == false)
        {
          string message = "Could not define the procedure ";

          message += "'";
          message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
          message += "' do to invalid type description of local value.";

          LogMessage (LOG_INT_ERROR, message);

          throw InterException (NULL, _EXTRA (InterException::INVALID_TYPE_DESC));
        }
    }
  ProcedureManager& procMgr   = GetProcedureManager ();
  D_UINT32          procIndex = procMgr.GetProcedure (pIdentifier, identifierLength);

  if (external)
    {
      if (procIndex == procMgr.INVALID_ENTRY)
        {
          string message = "Couldn't not find the definition for external procedure '";
          message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
          message += "'.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_FIRST));
        }

      bool argsMatch = (argsCount == procMgr.ArgsCount (procIndex));

      for (D_UINT localIndex = 0; (localIndex <= argsCount) && argsMatch; ++localIndex)
        {
          const D_UINT8 *pTypeDescriptor = typeMgr.GetType (pTypesOffset[localIndex]);
          if (memcmp (pTypeDescriptor,
                      procMgr.LocalType (procIndex, localIndex),
                      TypeManager::GetTypeLength (pTypeDescriptor) ) != 0)
            argsMatch = false;
        }

      if (argsMatch == false)
        {
          string message = "External declaration of procedure '";
          message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
          message += "' has a different signature than its definition.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_MISMATCH));

        }

      return procIndex;
    }
  else if (procIndex != procMgr.INVALID_ENTRY)
    {
      string message = "Duplicate definition of procedure '";
      message.insert (message.size(), _RC (const D_CHAR*, pIdentifier), identifierLength);
      message += "'.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterException (NULL, _EXTRA (InterException::DUPLICATE_DEFINITION));
    }

  return procMgr.AddProcedure (pIdentifier,
                               identifierLength,
                               localsCount,
                               argsCount,
                               syncCount,
                               pLocalValues,
                               pTypesOffset,
                               pCode,
                               codeSize);
}

CommonSession::CommonSession (I_DBSHandler& dbsHandler) :
    Session (dbsHandler)
{
}

CommonSession::~CommonSession ()
{
}

void
CommonSession::LogMessage (const LOG_LEVEL level, std::string& message)
{
  std::cerr << "Level: " << level << ": " << message << std::endl;
}
