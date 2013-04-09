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

#include "utils/wthread.h"
#include "dbs/dbs_mgr.h"

#include "pm_interpreter.h"
#include "pm_processor.h"

using namespace std;
using namespace prima;


typedef pair<string, NameSpaceHolder> NameSpacePair;

static const char                 gDBSName[] = "administrator";
static WSynchronizer                gSync;
static map<string, NameSpaceHolder> gmNameSpaces;

INTERP_SHL void
InitInterpreter (const char* adminDbsDir)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmNameSpaces.size () != 0)
    throw InterException (NULL, _EXTRA (InterException::ALREADY_INITED));

  static I_DBSHandler& glbDbsHnd = DBSRetrieveDatabase (gDBSName, adminDbsDir);

  NameSpaceHolder handler (new NameSpace (glbDbsHnd));
  gmNameSpaces.insert (NameSpacePair (gDBSName, handler));
}

INTERP_SHL I_Session&
GetInstance (const char* pName, I_Logger* pLog)
{
  if (pLog == NULL)
    pLog = &NULL_LOGGER;

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
      I_DBSHandler& hnd = DBSRetrieveDatabase (pName);
      gmNameSpaces.insert (NameSpacePair (
                                        pName,
                                        NameSpaceHolder (new NameSpace (hnd))
                                         ));
    }

  it = gmNameSpaces.find (pName);
  assert (it != gmNameSpaces.end ());

  //TODO: 1. Investigate a potential mechanism to avoid allocating session on heap
  //         You need this in order to handle requested for force shout down.
  //      2. Throw an execution exception when pLog is null at this point
  return *(new Session (*pLog,
                        gmNameSpaces.find (gDBSName)->second, it->second));
}

INTERP_SHL void
ReleaseInstance (I_Session& hInstance)
{
  WSynchronizerRAII syncHolder (gSync);

  if (gmNameSpaces.size () == 0)
    throw InterException (NULL, _EXTRA (InterException::NOT_INITED));

  Session* const inst = _SC (Session*, &hInstance);
  delete inst;
}

INTERP_SHL void
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


////////////////////////////////I_Session//////////////////////////////////////

I_Session::I_Session (I_Logger& log)
  : m_Log (log)
{
}

I_Session::~I_Session ()
{
}

//////////////////////////////////Session//////////////////////////////////////

Session::Session (I_Logger&        log,
                  NameSpaceHolder& globalNames,
                  NameSpaceHolder& privateNames)
  : I_Session (log),
    m_GlobalNames (globalNames),
    m_PrivateNames (privateNames)
{
  m_GlobalNames.IncRefsCount ();
  m_PrivateNames.IncRefsCount ();

  DefineTablesGlobalValues ();
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
  const uint32_t unitIndex = unitMgr.AddUnit (unit.GetGlobalsCount (),
                                              unit.GetProceduresCount (),
                                              unit.RetrieveConstArea (),
                                              unit.GetConstAreaSize ());

  TypeManager& typeMgr = m_PrivateNames.Get ().GetTypeManager ();

  try
  {
    for (uint_t glbIt = 0; glbIt < unit.GetGlobalsCount(); ++glbIt)
      {
        const uint_t         typeOff = unit.GetGlobalTypeIndex (glbIt);
        const uint8_t* const pTI     = unit.RetriveTypeInformation () + typeOff;
        const uint8_t* const pName   = _RC (const uint8_t*,
                                            unit.RetriveGlobalName (glbIt));
        const uint_t nameLength = unit.GetGlobalNameLength (glbIt);
        const bool   external   = (unit.IsGlobalExternal (glbIt) != FALSE);

        const uint32_t glbIndex = DefineGlobalValue (pName,
                                                     nameLength,
                                                     pTI,
                                                     external,
                                                     NULL);
        unitMgr.SetGlobalIndex (unitIndex, glbIt, glbIndex);
      }

    for (uint_t procIt = 0; procIt < unit.GetProceduresCount (); ++procIt)
      {
        const uint8_t* const pName     = _RC (const uint8_t*,
                                              unit.RetriveProcName (procIt));
        const uint_t       nameLength  = unit.GetProcNameSize (procIt);
        const bool         external    = (unit.IsProcExternal (procIt) != FALSE);
        const uint_t       localsCount = unit.GetProcLocalsCount (procIt);
        const uint_t       argsCount   = unit.GetProcParametersCount (procIt);
        vector<uint32_t>   typesOffset;
        vector<StackValue> values;

        for (uint_t localIt = 0; localIt < localsCount; ++localIt)
          {
            const uint8_t* const pLocalTI =
                                  unit.RetriveTypeInformation () +
                                  unit.GetProcLocalTypeIndex (procIt, localIt);

            const uint_t sizeTI = TypeManager::GetTypeLength (pLocalTI);
            auto_ptr<uint8_t> apTI (new uint8_t [sizeTI]);
            memcpy (apTI.get (), pLocalTI, sizeTI);

            //For table type values, the type would be changed to reflect the
            //order of the fields rearranged by the DBS layer.
            StackValue     value   = typeMgr.CreateLocalValue (apTI.get ());
            const uint32_t typeOff = typeMgr.AddType (apTI.get ());

            typesOffset.push_back (typeOff);

            //Keep a copy of stack values for locals to avoid construct them
            //every time when the procedure is called.
            values.push_back (value);
          }

          const uint32_t procIndex = DefineProcedure (
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
  }
  catch (...)
  {
      unitMgr.RemoveLastUnit ();
      throw;
  }
}

void
Session::ExecuteProcedure (const char* const pProcName,
                           SessionStack&       stack)
{
  const uint32_t procId = FindProcedure (_RC (const uint8_t*, pProcName),
                                         strlen (pProcName));

  Processor proc (*this, stack, procId);

  proc.Run ();

  //TODO: 1. Check to see if you need some clean up work!
  //      2. How will be handled the situation when a wrong number of parameters
  //         or with different types will be supplied?
}

uint_t
Session::GlobalValuesCount () const
{
  return m_PrivateNames.Get ().GetGlobalsManager().Count();
}

uint_t
Session::ProceduresCount () const
{
  return m_PrivateNames.Get ().GetProcedureManager ().Count ();
}

const char*
Session::GlobalValueName (const uint_t index) const
{
  return _RC (const char*,
              m_PrivateNames.Get ().GetGlobalsManager ().Name (index));
}

const char*
Session::ProcedureName (const uint_t index) const
{
  return _RC (const char*,
              m_PrivateNames.Get ().GetProcedureManager ().Name (index));
}

uint_t
Session::GlobalValueRawType (const uint32_t glbId)
{
  GlobalsManager& glbMgr = m_PrivateNames.Get ().GetGlobalsManager ();
  GlobalValue&    value  = glbMgr.GetGlobal (glbId);

  I_Operand& glbOp = value.GetOperand ();
  return glbOp.GetType ();
}

uint_t
Session::GlobalValueRawType (const char* const name)
{
  GlobalsManager* pGlbMgr = &m_PrivateNames.Get ().GetGlobalsManager ();
  uint32_t        glbId   = pGlbMgr->FindGlobal (_RC (const uint8_t*, name),
                                                 strlen (name));

  return GlobalValueRawType (glbId);
}

uint_t
Session::GlobalValueFieldsCount (const uint32_t glbId)
{
  GlobalsManager& glbMgr = m_PrivateNames.Get ().GetGlobalsManager ();
  GlobalValue&    value  = glbMgr.GetGlobal (glbId);

  I_Operand& glbOp = value.GetOperand ();

  if (IS_TABLE (glbOp.GetType ()))
    return glbOp.GetTable ().GetFieldsCount ();

  return 0;
}

uint_t
Session::GlobalValueFieldsCount (const char* const name)
{
  GlobalsManager* pGlbMgr = &m_PrivateNames.Get ().GetGlobalsManager ();
  uint32_t        glbId   = pGlbMgr->FindGlobal (_RC (const uint8_t*, name),
                                                 strlen (name));

  return GlobalValueFieldsCount (glbId);
}

const char*
Session::GlobalValueFieldName (const uint32_t glbId, const uint32_t field)
{
  GlobalsManager& glbMgr = m_PrivateNames.Get ().GetGlobalsManager ();
  GlobalValue&    value  = glbMgr.GetGlobal (glbId);

  I_Operand& glbOp = value.GetOperand ();

  DBSFieldDescriptor fd = glbOp.GetTable ().GetFieldDescriptor (field);

  return fd.m_pFieldName;
}

const char*
Session::GlobalValueFieldName (const char* const name, const uint32_t field)
{
  GlobalsManager* pGlbMgr = &m_PrivateNames.Get ().GetGlobalsManager ();
  uint32_t        glbId   = pGlbMgr->FindGlobal (_RC (const uint8_t*, name),
                                                 strlen (name));

  return GlobalValueFieldName (glbId, field);
}

uint_t
Session::GlobalValueFieldType (const uint32_t glbId, const uint32_t field)
{
  GlobalsManager& glbMgr = m_PrivateNames.Get ().GetGlobalsManager ();
  GlobalValue&    value  = glbMgr.GetGlobal (glbId);

  I_Operand& glbOp = value.GetOperand ();

  const DBSFieldDescriptor fd = glbOp.GetTable ().GetFieldDescriptor (field);

  uint16_t type = fd.m_FieldType;
  if (fd.isArray)
    MARK_ARRAY (type);

  return type;
}

uint_t
Session::GlobalValueFieldType (const char* const name, const uint32_t field)
{
  GlobalsManager* pGlbMgr = &m_PrivateNames.Get ().GetGlobalsManager ();
  uint32_t        glbId   = pGlbMgr->FindGlobal (_RC (const uint8_t*, name),
                                                 strlen (name));

  return GlobalValueFieldType (glbId, field);
}

uint_t
Session::ProcedureParametersCount (const uint_t id) const
{
  return m_PrivateNames.Get ().GetProcedureManager ().ArgsCount (id) + 1;
}

uint_t
Session::ProcedureParametersCount (const char* const name) const
{
  ProcedureManager& procMgr = m_PrivateNames.Get ().GetProcedureManager ();
  const uint_t      procId  = procMgr.GetProcedure (_RC (const uint8_t*, name),
                                                    strlen (name));
  return ProcedureParametersCount (procId);
}

uint_t
Session::ProcedurePameterRawType (const uint_t id, const uint_t param)
{
  if (param >= ProcedureParametersCount (id))
    throw InterException (NULL, _EXTRA (InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr = m_PrivateNames.Get ().GetProcedureManager ();
  StackValue&       val = _CC (StackValue&, mgr.LocalValue (id, param));

  return val.GetOperand ().GetType ();
}

uint_t
Session::ProcedurePameterRawType (const char* const name, const uint_t param)
{
  ProcedureManager& procMgr = m_PrivateNames.Get ().GetProcedureManager ();
  const uint_t      procId  = procMgr.GetProcedure (_RC (const uint8_t*, name),
                                                    strlen (name));
  return ProcedurePameterRawType (procId, param);
}

uint_t
Session::ProcedurePameterFieldsCount (const uint_t id, const uint_t param)
{
  if (param >= ProcedureParametersCount (id))
    throw InterException (NULL, _EXTRA (InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr = m_PrivateNames.Get ().GetProcedureManager ();
  StackValue&       val = _CC (StackValue&, mgr.LocalValue (id, param));

  if (IS_TABLE (val.GetOperand ().GetType ()))
    return val.GetOperand ().GetTable().GetFieldsCount ();

  return 0;
}

uint_t
Session::ProcedurePameterFieldsCount (const char* const name,
                                      const uint_t        param)
{
  ProcedureManager& procMgr = m_PrivateNames.Get ().GetProcedureManager ();
  const uint_t      procId  = procMgr.GetProcedure (_RC (const uint8_t*, name),
                                                    strlen (name));
  return ProcedurePameterFieldsCount (procId, param);
}

const char*
Session::ProcedurePameterFieldName (const uint_t id,
                                    const uint_t param,
                                    const uint_t field)
{
  if (param >= ProcedureParametersCount (id))
    throw InterException (NULL, _EXTRA (InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr   = m_PrivateNames.Get ().GetProcedureManager ();
  StackValue&       val   = _CC (StackValue&, mgr.LocalValue (id, param));
  I_DBSTable&       table = val.GetOperand ().GetTable();

  return table.GetFieldDescriptor (field).m_pFieldName;
}

const char*
Session::ProcedurePameterFieldName (const char* const name,
                                    const uint_t        param,
                                    const uint_t        field)
{
  ProcedureManager& procMgr = m_PrivateNames.Get ().GetProcedureManager ();
  const uint_t      procId  = procMgr.GetProcedure (_RC (const uint8_t*, name),
                                                    strlen (name));
  return ProcedurePameterFieldName (procId, param, field);
}

uint_t
Session::ProcedurePameterFieldType (const uint_t id,
                                    const uint_t param,
                                    const uint_t field)
{
  if (param >= ProcedureParametersCount (id))
    throw InterException (NULL, _EXTRA (InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr   = m_PrivateNames.Get ().GetProcedureManager ();
  StackValue&       val   = _CC (StackValue&, mgr.LocalValue (id, param));
  I_DBSTable&       table = val.GetOperand ().GetTable();

  DBSFieldDescriptor fd = table.GetFieldDescriptor (field);

  uint_t type = fd.m_FieldType;
  if (fd.isArray)
    MARK_ARRAY (type);

  return type;
}

uint_t
Session::ProcedurePameterFieldType (const char* const name,
                                    const uint_t        param,
                                    const uint_t        field)
{
  ProcedureManager& procMgr = m_PrivateNames.Get ().GetProcedureManager ();
  const uint_t      procId  = procMgr.GetProcedure (_RC (const uint8_t*, name),
                                                    strlen (name));
  return ProcedurePameterFieldType (procId, param, field);
}

uint32_t
Session::FindGlobal (const uint8_t* pName, const uint_t nameLength)
{
  GlobalsManager* pGlbMgr = &m_PrivateNames.Get ().GetGlobalsManager ();
  uint32_t        result  = pGlbMgr->FindGlobal (pName, nameLength);

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
Session::GetGlobalValue (const uint32_t glbId)
{
  GlobalsManager& pGlbMgr = GlobalsManager::IsGlobalEntry (glbId) ?
                            m_GlobalNames.Get ().GetGlobalsManager () :
                            m_PrivateNames.Get ().GetGlobalsManager ();

  GlobalOperand glbOp (pGlbMgr.GetGlobal (glbId));
  return StackValue (glbOp);
}

const uint8_t*
Session::FindGlobalTI (const uint32_t glbId)
{
  GlobalsManager& pGlbMgr = GlobalsManager::IsGlobalEntry (glbId) ?
                            m_GlobalNames.Get ().GetGlobalsManager () :
                            m_PrivateNames.Get ().GetGlobalsManager ();

  return pGlbMgr.GetGlobalTI (glbId);
}

uint32_t
Session::FindProcedure (const uint8_t* pName, const uint_t nameLength)
{
  ProcedureManager* pProMgr = &m_PrivateNames.Get ().GetProcedureManager ();
  uint32_t          result  = pProMgr->GetProcedure (pName, nameLength);

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

uint32_t
Session::ArgsCount (const uint32_t procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.ArgsCount (procId);
}

uint32_t
Session::LocalsCount (const uint32_t procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.LocalsCount (procId);
}

const uint8_t*
Session::FindLocalTI (const uint32_t procId, const uint32_t local)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.LocalTI (procId, local);
}

Unit&
Session::ProcUnit (const uint32_t procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();
  return pProcMgr.GetUnit (procId);
}

const uint8_t*
Session::ProcCode (const uint32_t procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.Code (procId, NULL);
}

uint64_t
Session::ProcCodeSize (const uint32_t procId)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();
  uint64_t codeSize;
  pProcMgr.Code (procId, &codeSize);

  return codeSize;
}

StackValue
Session::ProcLocalValue (const uint32_t procId, const uint32_t local)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  return pProcMgr.LocalValue (procId, local);
}

void
Session::AquireProcSync (const uint32_t procId, const uint32_t sync)
{
  ProcedureManager& pProcMgr = ProcedureManager::IsGlobalEntry (procId) ?
                               m_GlobalNames.Get ().GetProcedureManager () :
                               m_PrivateNames.Get ().GetProcedureManager ();

  pProcMgr.AquireSync (procId, sync);
}

void
Session::ReleaseProcSync (const uint32_t procId, const uint32_t sync)
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

void
Session::DefineTablesGlobalValues ()
{
  I_DBSHandler& dbs = m_PrivateNames.Get ().GetDBSHandler ();
  const uint_t tablesCount = dbs.PersistentTablesCount ();

  for (uint_t tableId = 0; tableId < tablesCount; ++tableId)
    {
      const char* const pTableName = dbs.TableName (tableId);

      I_DBSTable& table = dbs.RetrievePersistentTable (tableId);

      try
      {
          vector<uint8_t> typeInfo = compute_table_typeinfo (table);
          DefineGlobalValue (_RC (const uint8_t*, pTableName),
                             strlen (pTableName),
                             &typeInfo.front (),
                             false,
                             &table);
      }
      catch (...)
      {
          dbs.ReleaseTable (table);
          throw ;
      }
    }
}

uint32_t
Session::DefineGlobalValue (const uint8_t*    pName,
                            const uint_t      nameLength,
                            const uint8_t*    pTI,
                            const bool        external,
                            I_DBSTable* const pPersistentTable)
{
  assert (TypeManager::IsTypeValid (pTI));

  if (TypeManager::IsTypeValid (pTI) == false)
    {
      string message = "Could not add the global variable ";

      message += "'";
      message.insert (message.size (), _RC (const char*, pName), nameLength);
      message += "' do to invalid type description.";

      m_Log.Log (LOG_ERROR, message);

      throw InterException (NULL, _EXTRA (InterException::INVALID_TYPE_DESC));
    }


  auto_ptr<uint8_t> apTI (new uint8_t[TypeManager::GetTypeLength (pTI)]);
  memcpy (apTI.get (), pTI, TypeManager::GetTypeLength (pTI));

  TypeManager&   typeMgr  = m_PrivateNames.Get ().GetTypeManager ();
  GlobalValue    value    = typeMgr.CreateGlobalValue (apTI.get (),
                                                       pPersistentTable);
  const uint32_t glbEntry = FindGlobal (pName, nameLength);

  if (GlobalsManager::IsValid (glbEntry) == false)
    {
      if (external)
        {
          string message = "Couldn't not find the definition for external "
                           "declaration of global value '";
          message.insert (message.size(),
                          _RC (const char*, pName),
                          nameLength);
          message += "'.";

          m_Log.Log (LOG_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_FIRST));
        }

      const uint32_t  typeOff = typeMgr.AddType (apTI.get ());
      GlobalsManager& glbMgr  = m_PrivateNames.Get ().GetGlobalsManager ();

      return glbMgr.AddGlobal (pName, nameLength, value, typeOff);
    }
  else if (external)
    {
      const uint8_t* pDefinitionTI = FindGlobalTI (glbEntry);

      if (memcmp (apTI.get (),
                  pDefinitionTI,
                  TypeManager::GetTypeLength (apTI.get ()) ) != 0)
        {
          string message = "External declaration of global value '";
          message.insert (message.size(),
                          _RC (const char*, pName),
                          nameLength);
          message += "' has a different type than its definition.";

          m_Log.Log (LOG_ERROR, message);
          throw InterException (NULL,
                                _EXTRA (InterException::EXTERNAL_MISMATCH));
        }
    }
  else
    {
      string message = "Duplicate definition of global value '";
      message.insert (message.size(), _RC (const char*, pName), nameLength);
      message += "'.";

      m_Log.Log (LOG_ERROR, message);
      throw InterException (NULL,
                            _EXTRA (InterException::DUPLICATE_DEFINITION));
    }

  return glbEntry;
}

uint32_t
Session::DefineProcedure (const uint8_t*      pName,
                          const uint_t        nameLength,
                          const uint32_t      localsCount,
                          const uint32_t      argsCount,
                          const uint32_t      syncCount,
                          vector<StackValue>& localValues,
                          const uint32_t*     pTypesOffset,
                          const uint8_t*      pCode,
                          const uint32_t      codeSize,
                          const bool          external,
                          Unit&               unit)
{
  assert (argsCount < localsCount);
  assert (localsCount > 0);
  assert (external || (codeSize > 0));

  TypeManager& typeMgr = m_PrivateNames.Get ().GetTypeManager ();

  for (uint_t localIt = 0; localIt < localsCount; ++ localIt)
    {
      const uint8_t *pTI = typeMgr.GetType (pTypesOffset[localIt]);

      if (TypeManager::IsTypeValid (pTI) == false)
        {
          string message = "Could not define the procedure ";

          message += "'";
          message.insert (message.size(),
                          _RC (const char*, pName),
                          nameLength);
          message += "' do to invalid type description of local value.";

          m_Log.Log (LOG_ERROR, message);
          throw InterException (NULL,
                                _EXTRA (InterException::INVALID_TYPE_DESC));
        }
    }

  ProcedureManager& procMgr   = m_PrivateNames.Get ().GetProcedureManager ();
  uint32_t          procIndex = FindProcedure (pName, nameLength);

  if (external)
    {
      if (ProcedureManager::IsValid (procIndex) == false)
        {
          string message = "Couldn't not find the definition "
                           "for external procedure '";
          message.insert (message.size(),
                          _RC (const char*, pName),
                          nameLength);
          message += "'.";

          m_Log.Log (LOG_ERROR, message);
          throw InterException (NULL, _EXTRA (InterException::EXTERNAL_FIRST));
        }

      bool argsMatch = (argsCount == ArgsCount (procIndex));

      for (uint_t localIt = 0; (localIt <= argsCount) && argsMatch; ++localIt)
        {
          const uint8_t *pTI = typeMgr.GetType (pTypesOffset[localIt]);
          if (memcmp (pTI,
                      FindLocalTI (procIndex, localIt),
                      TypeManager::GetTypeLength (pTI) ) != 0)
            argsMatch = false;
        }

      if (argsMatch == false)
        {
          string message = "External declaration of procedure '";
          message.insert (message.size(),
                          _RC (const char*, pName),
                          nameLength);
          message += "' has a different signature than its definition.";

          m_Log.Log (LOG_ERROR, message);
          throw InterException (NULL,
                                _EXTRA (InterException::EXTERNAL_MISMATCH));

        }

      return procIndex;
    }
  else if (ProcedureManager::IsValid (procIndex))
    {
      string message = "Duplicate definition of procedure '";
      message.insert (message.size(), _RC (const char*, pName), nameLength);
      message += "'.";

      m_Log.Log (LOG_ERROR, message);
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

#if  defined (ENABLE_MEMORY_TRACE) && defined (USE_INTERP_SHL)
uint32_t WMemoryTracker::sm_InitCount = 0;
const char* WMemoryTracker::sm_Module = "PRIMA";
#endif
