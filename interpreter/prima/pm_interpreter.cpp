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
#include <map>

#include "wthread.h"

#include "dbs/include/dbs_mgr.h"

#include "pm_interpreter.h"
#include "pm_processor.h"

using namespace std;
using namespace prima;


typedef pair<string, NameSpaceHolder> NameSpacePair;

static const D_CHAR                 gDBSName[] = "administrator";
static WSynchronizer                gSync;
static map<string, NameSpaceHolder> gmNameSpaces;

void
InitInterpreter ()
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmNameSpaces.size () != 0)
    throw InterException (NULL, _EXTRA (InterException::ALREADY_INITED));

  static I_DBSHandler& glbDbsHnd = DBSRetrieveDatabase (gDBSName);

  NameSpaceHolder handler (new NameSpace (glbDbsHnd));
  gmNameSpaces.insert (NameSpacePair (gDBSName, handler));
}

I_Session&
GetInstance (const D_CHAR* pName)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmNameSpaces.size () == 0)
    throw InterException (NULL, _EXTRA (InterException::NOT_INITED));

  if (pName == NULL)
    pName = gDBSName;
  else if (strcmp (pName, gDBSName) == 0)
    throw InterException (NULL, _EXTRA (InterException::INVALID_SESSION));

  map<string, NameSpaceHolder>::iterator it = gmNameSpaces.find (pName);
  if (it == gmNameSpaces.end ())
    {
      I_DBSHandler& glbDbsHnd = DBSRetrieveDatabase (gDBSName);
      gmNameSpaces.insert (NameSpacePair (
                              pName,
                              NameSpaceHolder (new NameSpace (glbDbsHnd))
                                         ));
    }

  it = gmNameSpaces.find (pName);
  assert (it != gmNameSpaces.end ());

  //TODO: Investigate a potential mechanism to avoid allocating session on heap
  //      You need this in order to handle reqested for force shout down.
  return *(new Session (gmNameSpaces.find (gDBSName)->second, it->second));
}

void
ReleaseInstance (I_Session& hInstance)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmNameSpaces.size () == 0)
    throw InterException (NULL, _EXTRA (InterException::NOT_INITED));

  Session* const inst = _SC (Session*, &hInstance);
  delete inst;
}

void
CleanInterpreter (const bool forced)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmNameSpaces.size () == 0)
    throw InterException (NULL, _EXTRA (InterException::NOT_INITED));

  //TODO: remember to clean the sessions to when the mechanism will
  //be employed
  map<string, NameSpaceHolder>::iterator it = gmNameSpaces.begin ();
  while (it != gmNameSpaces.end ())
    {
      NameSpaceHolder& space = it->second;

      if (forced)
        space.ForceRelease ();
      else if (space.RefsCount() != 0)
        throw InterException (NULL, _EXTRA (InterException::SESSION_IN_USE));

      ++it;
    }

  gmNameSpaces.clear ();
}
////////////////////////////////NameSpace//////////////////////////////////////

NameSpace::NameSpace (I_DBSHandler& dbsHandler)
  : m_DbsHandler (dbsHandler),
    m_TypeManager (*this),
    m_GlbsManager (*this),
    m_ProcsManager (*this),
    m_UnitsManager ()
{
}

NameSpace::~NameSpace ()
{
}

//////////////////////////////////Session//////////////////////////////////////

Session::Session (NameSpaceHolder& globalNames,
                  NameSpaceHolder& privateNames)
  : I_Session (),
    m_GlobalNames (globalNames),
    m_PrivateNames (privateNames)
{
  m_GlobalNames.IncRefsCount ();
  m_PrivateNames.IncRefsCount ();
}

Session::~Session ()
{
  m_GlobalNames.DecRefsCount ();
  m_PrivateNames.DecRefsCount ();
}

void
Session::LoadCompiledUnit (WICompiledUnit& unit)
{
  UnitsManager&  unitMgr   = m_PrivateNames.Get ().GetUnitsManager ();
  const D_UINT32 unitIndex = unitMgr.AddUnit (unit.GetGlobalsCount (),
                                              unit.GetProceduresCount (),
                                              unit.RetrieveConstArea (),
                                              unit.GetConstAreaSize ());

  TypeManager& typeMgr = m_PrivateNames.Get ().GetTypeManager ();

  try
  {
    for (D_UINT glbIt = 0; glbIt < unit.GetGlobalsCount(); ++glbIt)
      {
        const D_UINT         typeOff = unit.GetGlobalTypeIndex (glbIt);
        const D_UINT8* const pTI     = unit.RetriveTypeInformation () + typeOff;
        const D_UINT8* const pName   = _RC (const D_UINT8*,
                                            unit.RetriveGlobalName (glbIt));
        const D_UINT nameLength = unit.GetGlobalNameLength (glbIt);
        const bool   external   = (unit.IsGlobalExternal (glbIt) != FALSE);

        const D_UINT32 glbIndex = DefineGlobalValue (pName,
                                                     nameLength,
                                                     pTI,
                                                     external);
        unitMgr.SetGlobalIndex (unitIndex, glbIt, glbIndex);
      }

    for (D_UINT procIt = 0; procIt < unit.GetProceduresCount (); ++procIt)
      {
        const D_UINT8* const pName     = _RC (const D_UINT8*,
                                              unit.RetriveProcName (procIt));
        const D_UINT       nameLength  = unit.GetProcNameSize (procIt);
        const bool         external    = (unit.IsProcExternal (procIt) != FALSE);
        const D_UINT       localsCount = unit.GetProcLocalsCount (procIt);
        const D_UINT       argsCount   = unit.GetProcParametersCount (procIt);
        vector<D_UINT32>   typesOffset;
        vector<StackValue> values;

        for (D_UINT localIt = 0; localIt < localsCount; ++localIt)
          {
            const D_UINT8* const pLocalTI =
                                  unit.RetriveTypeInformation () +
                                  unit.GetProcLocalTypeIndex (procIt, localIt);

            const D_UINT sizeTI = TypeManager::GetTypeLength (pLocalTI);
            auto_ptr<D_UINT8> apTI (new D_UINT8 [sizeTI]);
            memcpy (apTI.get (), pLocalTI, sizeTI);

            //For table type values, the type would be changed to reflect the
            //order of the fields rearranged by the DBS layer.
            StackValue     value   = typeMgr.CreateLocalValue (apTI.get ());
            const D_UINT32 typeOff = typeMgr.AddType (apTI.get ());

            typesOffset.push_back (typeOff);

            //Keep a copy of stack values for locals ( except the ones of the
            //parameters) to avoid construct them every time when the procedure
            //is called.
            if ((localIt == 0) || (localIt > argsCount))
              values.push_back (value);
            else
              value.Clear ();
          }

        try
        {
            const D_UINT32 procIndex = DefineProcedure (
                                    pName,
                                    nameLength,
                                    localsCount,
                                    argsCount,
                                    unit.GetProcSyncStatementsCount (procIt),
                                    values,
                                    &typesOffset[0],
                                    unit.RetriveProcCodeArea (procIt),
                                    unit.GetProcCodeAreaSize (procIt),
                                    external,
                                    unitMgr.GetUnit (unitIndex)
                                                         );

            unitMgr.SetProcIndex (unitIndex, procIt, procIndex);
        }
        catch (...)
        {
            for (vector<StackValue>::iterator it = values.begin ();
                 it != values.end ();
                 ++it)
              {
                it->Clear ();
              }

            throw;
        }
      }
  }
  catch (...)
  {
      unitMgr.RemoveLastUnit ();
      throw;
  }
}


void
Session::ExecuteProcedure (const D_UINT8* const pProcName,
                           SessionStack&        stack)
{
  const D_UINT32 procId = FindProcedure (
                                  pProcName,
                                  strlen (_RC (const D_CHAR*, pProcName))
                                        );

  Processor proc (*this, stack, procId);

  proc.Run ();

  //TODO: 1. Check to see if you need some clean up work!
  //      2. How will be handled the situation when a wrong number of parameters
  //         or with different types will be supplied?
}

void
Session::LogMessage (const LOG_LEVEL level, std::string& message)
{
}

D_UINT32
Session::FindGlobal (const D_UINT8* pName, const D_UINT nameLength)
{
  GlobalsManager* pGlbMgr = &m_PrivateNames.Get ().GetGlobalsManager ();
  D_UINT32        result  = pGlbMgr->FindGlobal (pName, nameLength);

  assert (GlobalsManager::IsGlobalEntry (result) == false);

  if (GlobalsManager::IsValid (result) == false)
    {
      pGlbMgr = &m_GlobalNames.Get ().GetGlobalsManager ();
      result  = pGlbMgr->FindGlobal (pName, nameLength);

      assert (GlobalsManager::IsGlobalEntry (result) == false);

      GlobalsManager::MarkAsGlobalEntry (result);
    }

  return result;
}

StackValue
Session::GetGlobalValue (const D_UINT32 glbId)
{
  GlobalsManager& pGlbMgr = GlobalsManager::IsGlobalEntry (glbId) ?
                            m_GlobalNames.Get ().GetGlobalsManager () :
                            m_PrivateNames.Get ().GetGlobalsManager ();

  GlobalOperand glbOp (pGlbMgr.GetGlobal (glbId));
  return StackValue (glbOp);
}

const D_UINT8*
Session::FindGlobalTI (const D_UINT32 glbId)
{
  GlobalsManager& pGlbMgr = GlobalsManager::IsGlobalEntry (glbId) ?
                            m_GlobalNames.Get ().GetGlobalsManager () :
                            m_PrivateNames.Get ().GetGlobalsManager ();

  return pGlbMgr.GetGlobalTI (glbId);
}

D_UINT32
Session::FindProcedure (const D_UINT8* pName, const D_UINT nameLength)
{
  ProcedureManager* pProMgr = &m_PrivateNames.Get ().GetProcedureManager ();
  D_UINT32          result  = pProMgr->GetProcedure (pName, nameLength);

  assert (ProcedureManager::IsGlobalEntry (result) == false);

  if ( ! ProcedureManager::IsValid (result))
    {
      pProMgr = &m_GlobalNames.Get ().GetProcedureManager ();
      result  = pProMgr->GetProcedure (pName, nameLength);

      assert (ProcedureManager::IsGlobalEntry (result) == false);

      ProcedureManager::MarkAsGlobalEntry (result);
    }

  return result;
}

D_UINT32
Session::ArgsCount (const D_UINT32 procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.ArgsCount (procId);
}

D_UINT32
Session::LocalsCount (const D_UINT32 procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.LocalsCount (procId);
}

const D_UINT8*
Session::FindLocalTI (const D_UINT32 procId, const D_UINT32 local)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.LocalTI (procId, local);
}

Unit&
Session::ProcUnit (const D_UINT32 procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();
  return pProcMgr.GetUnit (procId);
}

const D_UINT8*
Session::ProcCode (const D_UINT32 procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.Code (procId, NULL);
}

D_UINT64
Session::ProcCodeSize (const D_UINT32 procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();
  D_UINT64 codeSize;
  pProcMgr.Code (procId, &codeSize);

  return codeSize;
}

StackValue
Session::ProcLocalValue (const D_UINT32 procId, const D_UINT32 local)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.LocalValue (procId, local);
}

void
Session::AquireProcSync (const D_UINT32 procId, const D_UINT32 sync)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  pProcMgr.AquireSync (procId, sync);
}

void
Session::ReleaseProcSync (const D_UINT32 procId, const D_UINT32 sync)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  pProcMgr.ReleaseSync (procId, sync);
}

I_DBSHandler&
Session::DBSHandler ()
{
  return m_PrivateNames.Get ().GetDBSHandler ();
}



D_UINT32
Session::DefineGlobalValue (const D_UINT8* pName,
                            const D_UINT   nameLength,
                            const D_UINT8* pTI,
                            const bool     external)
{
  assert (TypeManager::IsTypeValid (pTI));

  if (TypeManager::IsTypeValid (pTI) == false)
    {
      string message = "Could not add the global variable ";

      message += "'";
      message.insert (message.size(), _RC (const D_CHAR*, pName), nameLength);
      message += "' do to invalid type description.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterException (NULL, _EXTRA (InterException::INVALID_TYPE_DESC));
    }


  auto_ptr<D_UINT8> apTI (new D_UINT8[TypeManager::GetTypeLength (pTI)]);
  memcpy (apTI.get (), pTI, TypeManager::GetTypeLength (pTI));

  TypeManager&   typeMgr  = m_PrivateNames.Get ().GetTypeManager ();
  GlobalValue    value    = typeMgr.CreateGlobalValue (apTI.get ());
  const D_UINT32 glbEntry = FindGlobal (pName, nameLength);

  if (GlobalsManager::IsValid (glbEntry) == false)
    {
      if (external)
        {
          string message = "Couldn't not find the definition for external "
                           "declaration of global value '";
          message.insert (message.size(),
                          _RC (const D_CHAR*, pName),
                          nameLength);
          message += "'.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_FIRST));
        }

      const D_UINT32  typeOff = typeMgr.AddType (apTI.get ());
      GlobalsManager& glbMgr  = m_PrivateNames.Get ().GetGlobalsManager ();

      return glbMgr.AddGlobal (pName, nameLength, value, typeOff);
    }
  else if (external)
    {
      const D_UINT8* pDefinitionTI = FindGlobalTI (glbEntry);

      if (memcmp (apTI.get (),
                  pDefinitionTI,
                  TypeManager::GetTypeLength (apTI.get ()) ) != 0)
        {
          string message = "External declaration of global value '";
          message.insert (message.size(),
                          _RC (const D_CHAR*, pName),
                          nameLength);
          message += "' has a different type than its definition.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL,
                                _EXTRA (InterException::EXTERNAL_MISMATCH));
        }
    }
  else
    {
      string message = "Duplicate definition of global value '";
      message.insert (message.size(), _RC (const D_CHAR*, pName), nameLength);
      message += "'.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterException (NULL,
                            _EXTRA (InterException::DUPLICATE_DEFINITION));
    }

  return glbEntry;
}

D_UINT32
Session::DefineProcedure (const D_UINT8*      pName,
                          const D_UINT        nameLength,
                          const D_UINT32      localsCount,
                          const D_UINT32      argsCount,
                          const D_UINT32      syncCount,
                          vector<StackValue>& localValues,
                          const D_UINT32*     pTypesOffset,
                          const D_UINT8*      pCode,
                          const D_UINT32      codeSize,
                          const bool          external,
                          Unit&               unit)
{
  assert (argsCount < localsCount);
  assert (localsCount > 0);
  assert (external || (codeSize > 0));

  TypeManager& typeMgr = m_PrivateNames.Get ().GetTypeManager ();

  for (D_UINT localIt = 0; localIt < localsCount; ++ localIt)
    {
      const D_UINT8 *pTI = typeMgr.GetType (pTypesOffset[localIt]);

      if (TypeManager::IsTypeValid (pTI) == false)
        {
          string message = "Could not define the procedure ";

          message += "'";
          message.insert (message.size(),
                          _RC (const D_CHAR*, pName),
                          nameLength);
          message += "' do to invalid type description of local value.";

          LogMessage (LOG_INT_ERROR, message);

          throw InterException (NULL,
                                _EXTRA (InterException::INVALID_TYPE_DESC));
        }
    }

  ProcedureManager& procMgr   = m_PrivateNames.Get ().GetProcedureManager ();
  D_UINT32          procIndex = FindProcedure (pName, nameLength);

  if (external)
    {
      if (ProcedureManager::IsValid (procIndex) == false)
        {
          string message = "Couldn't not find the definition "
                           "for external procedure '";
          message.insert (message.size(),
                          _RC (const D_CHAR*, pName),
                          nameLength);
          message += "'.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_FIRST));
        }

      bool argsMatch = (argsCount == ArgsCount (procIndex));

      for (D_UINT localIt = 0; (localIt <= argsCount) && argsMatch; ++localIt)
        {
          const D_UINT8 *pTI = typeMgr.GetType (pTypesOffset[localIt]);
          if (memcmp (pTI,
                      FindLocalTI (procIndex, localIt),
                      TypeManager::GetTypeLength (pTI) ) != 0)
            argsMatch = false;
        }

      if (argsMatch == false)
        {
          string message = "External declaration of procedure '";
          message.insert (message.size(),
                          _RC (const D_CHAR*, pName),
                          nameLength);
          message += "' has a different signature than its definition.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterException (NULL,
                                _EXTRA (InterException::EXTERNAL_MISMATCH));

        }

      return procIndex;
    }
  else if (ProcedureManager::IsValid (procIndex))
    {
      string message = "Duplicate definition of procedure '";
      message.insert (message.size(), _RC (const D_CHAR*, pName), nameLength);
      message += "'.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterException (NULL,
                            _EXTRA (InterException::DUPLICATE_DEFINITION));
    }

  return procMgr.AddProcedure (pName,
                               nameLength,
                               localsCount,
                               argsCount,
                               syncCount,
                               localValues,
                               pTypesOffset,
                               pCode,
                               codeSize,
                               unit);
}

