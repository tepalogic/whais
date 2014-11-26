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
#include <sstream>
#include <map>

#include "utils/wthread.h"
#include "dbs/dbs_mgr.h"
#include "stdlib/interface.h"

#include "pm_interpreter.h"
#include "pm_processor.h"


using namespace std;



namespace whais {



typedef pair<string, prima::NameSpaceHolder> NameSpacePair;

static const char            gDBSName[] = "administrator";
static Lock                  gSync;

static map<string, prima::NameSpaceHolder> gmNameSpaces;



INTERP_SHL void
InitInterpreter( const char* adminDbsDir)
{
  LockRAII syncHolder( gSync);

  if (gmNameSpaces.size() != 0)
    throw InterException( _EXTRA( InterException::ALREADY_INITED));

  static IDBSHandler& glbDbsHnd = DBSRetrieveDatabase( gDBSName, adminDbsDir);

  prima::NameSpaceHolder handler( new prima::NameSpace( glbDbsHnd));

  gmNameSpaces.insert( NameSpacePair( gDBSName, handler));
}


INTERP_SHL ISession&
GetInstance( const char* name, Logger* log)
{
  if (log == NULL)
    log = &NULL_LOGGER;

  LockRAII syncHolder( gSync);

  if (gmNameSpaces.size() == 0)
    throw InterException( _EXTRA( InterException::NOT_INITED));

  if (name == NULL)
    name = gDBSName;

  else if (strcmp( name, gDBSName) == 0)
    {
      throw InterException( _EXTRA( InterException::INVALID_SESSION),
                            "Cannot find the instance of session '%s'.",
                            name);
    }

  map<string, prima::NameSpaceHolder>::iterator it = gmNameSpaces.find( name);
  if (it == gmNameSpaces.end ())
    {
      IDBSHandler& hnd = DBSRetrieveDatabase( name);
      gmNameSpaces.insert( NameSpacePair( 
                        name,
                        prima::NameSpaceHolder( new prima::NameSpace( hnd))
                                         ));
      it = gmNameSpaces.find( name);
    }

  assert( it != gmNameSpaces.end ());

  //TODO: 1. Investigate a potential mechanism to avoid allocating session on heap
  //         You need this in order to handle requested for force shout down.
  //      2. Throw an execution exception when pLog is null at this point
  return *(new prima::Session( 
                        *log,
                        gmNameSpaces.find( gDBSName)->second,
                        it->second)
                              );
}


INTERP_SHL void
ReleaseInstance( ISession& instance)
{
  LockRAII syncHolder( gSync);

  if (gmNameSpaces.size() == 0)
    throw InterException( _EXTRA( InterException::NOT_INITED));

  prima::Session* const inst = _SC (prima::Session*, &instance);

  delete inst;
}


INTERP_SHL void
CleanInterpreter( const bool forced)
{
  LockRAII syncHolder( gSync);

  if (gmNameSpaces.size() == 0)
    throw InterException( _EXTRA( InterException::NOT_INITED));

  //TODO: Remember to clean the sessions to when the mechanism will
  //      be employed
  map<string, prima::NameSpaceHolder>::iterator it = gmNameSpaces.begin();
  while( it != gmNameSpaces.end ())
    {
      prima::NameSpaceHolder& space = it->second;

      if (forced)
        space.ForceRelease();

      else if (space.RefsCount() != 0)
        throw InterException( _EXTRA( InterException::SESSION_IN_USE));

      ++it;
    }
  gmNameSpaces.clear();
}





ISession::ISession( Logger& log)
  : mLog( log)
{
}


ISession::~ISession()
{
}



namespace prima {




NameSpace::NameSpace( IDBSHandler& dbsHandler)
  : mDbsHandler( dbsHandler),
    mTypeManager( *this),
    mGlbsManager( *this),
    mProcsManager( *this),
    mUnitsManager()
{
}




Session::Session( Logger&           log,
                  NameSpaceHolder&  globalNames,
                  NameSpaceHolder&  privateNames)
  : ISession( log),
    mGlobalNames( globalNames),
    mPrivateNames( privateNames)
{
  mGlobalNames.IncRefsCount();
  mPrivateNames.IncRefsCount();

  DefineTablesGlobalValues();
}


Session::~Session()
{
  mGlobalNames.DecRefsCount();
  mPrivateNames.DecRefsCount();

  for (vector<WH_SHLIB>::iterator it = mNativeLibs.begin();
       it != mNativeLibs.end ();
       ++it)
    {
      WLIB_END_LIB_FUNC end = _RC (WLIB_END_LIB_FUNC,
                                   wh_shl_symbol( *it, WSTDLIB_END_FUNC));

      if (end != NULL)
        end ();

      wh_shl_release( *it);
    }
}


void
Session::LoadCompiledUnit( WIFunctionalUnit& unit)
{
  UnitsManager&  unitMgr   = mPrivateNames.Get ().GetUnitsManager();
  const uint32_t unitIndex = unitMgr.AddUnit( unit.GlobalsCount(),
                                              unit.ProceduresCount(),
                                              unit.RetrieveConstArea(),
                                              unit.ConstsAreaSize());
  try
  {
    TypeManager& typeMgr = mPrivateNames.Get ().GetTypeManager();
    for (uint_t glbIt = 0; glbIt < unit.GlobalsCount(); ++glbIt)
      {
        const uint_t         typeOff  = unit.GlobalTypeOff( glbIt);
        const uint8_t* const typeDesc = unit.RetriveTypeArea() + typeOff;
        const uint8_t* const name     = _RC (const uint8_t*,
                                            unit.RetriveGlobalName( glbIt));
        const uint_t nameLength = unit.GlobalNameLength( glbIt);
        const bool   external   = (unit.IsGlobalExternal( glbIt) != FALSE);

        const uint32_t glbIndex = DefineGlobalValue( name,
                                                     nameLength,
                                                     typeDesc,
                                                     external,
                                                     NULL);
        unitMgr.SetGlobalIndex( unitIndex, glbIt, glbIndex);
      }

    for (uint_t procIt = 0; procIt < unit.ProceduresCount(); ++procIt)
      {
        const uint8_t* const name = _RC (const uint8_t*,
                                         unit.RetriveProcName( procIt));

        const uint_t  nameLength  = unit.GetProcNameSize( procIt);
        const bool    external    = (unit.IsProcExternal( procIt) != FALSE);
        const uint_t  localsCount = unit.ProcLocalsCount( procIt);
        const uint_t  argsCount   = unit.ProcParametersCount( procIt);

        vector<uint32_t>   typesOffset;
        vector<StackValue> values;

        for (uint_t localIt = 0; localIt < localsCount; ++localIt)
          {
            const uint8_t* const localTypeDesc =
                                  unit.RetriveTypeArea() +
                                    unit.GetProcLocalTypeOff( procIt, localIt);

            const uint_t tdSize = TypeManager::GetTypeLength( localTypeDesc);
            auto_ptr<uint8_t> tdBuffer( new uint8_t [tdSize]);
            memcpy( tdBuffer.get (), localTypeDesc, tdSize);

            //For table type values, the type would be changed to reflect the
            //order of the fields rearranged by the DBS layer.
            StackValue     value   = typeMgr.CreateLocalValue( tdBuffer.get ());
            const uint32_t typeOff = typeMgr.AddType( tdBuffer.get ());

            typesOffset.push_back( typeOff);

            //Keep a copy of stack values for locals to avoid construct them
            //every time when the procedure is called.
            values.push_back( value);
          }

          const uint32_t procIndex = DefineProcedure( 
                                  name,
                                  nameLength,
                                  localsCount,
                                  argsCount,
                                  unit.ProcSyncStatementsCount( procIt),
                                  values,
                                  &typesOffset[0],
                                  unit.RetriveProcCodeArea( procIt),
                                  unit.ProcCodeAreaSize( procIt),
                                  external,
                                  &unitMgr.GetUnit( unitIndex)
                                                    );

          unitMgr.SetProcedureIndex( unitIndex, procIt, procIndex);
      }
  }
  catch( ...)
  {
      unitMgr.RemoveLastUnit();
      throw;
  }
}


bool
Session::LoadSharedLib( WH_SHLIB shl)
{
  assert( shl != INVALID_SHL);


  mNativeLibs.push_back( shl);


  if (wh_shl_symbol( shl, WSTDLIB_END_FUNC) == NULL)
    {
      ostringstream logEntry;

      logEntry << "The shared library does not provide a '";
      logEntry << WSTDLIB_END_FUNC << "' function.";
      mLog.Log (LOG_WARNING, logEntry.str ());
    }

  WLIB_START_LIB_FUNC start = _RC (WLIB_START_LIB_FUNC,
                                   wh_shl_symbol( shl, WSTDLIB_START_FUNC));
  if (start == NULL)
    {
      ostringstream logEntry;

      logEntry << "The shared library does not provide a '";
      logEntry << WSTDLIB_START_FUNC << "' function.";
      mLog.Log (LOG_WARNING, logEntry.str ());
    }
  else if (start() != WOP_OK)
    {
      mLog.Log (LOG_ERROR, "Failed to initialize the library.");

      return false;
    }

  WLIB_DESC_LIB_FUNC describe = _RC (WLIB_DESC_LIB_FUNC,
                                     wh_shl_symbol( shl, WSTDLIB_DESC_FUNC));
  if (describe == NULL)
    {
      ostringstream logEntry;

      logEntry << "The shared library does not provide a '";
      logEntry << WSTDLIB_DESC_FUNC << "' function.";

      mLog.Log (LOG_ERROR, logEntry.str ());

      return false;
    }

  const WLIB_DESCRIPTION* const lib = describe();
  if (lib == NULL)
    {
      mLog.Log (LOG_ERROR, "The shared lib doesn't provide its content.");

      return false;

    }

  TypeManager& typeMgr = mPrivateNames.Get ().GetTypeManager();
  for (uint_t procIt = 0; procIt < lib->procsCount; ++procIt)
    {
      if (lib->procsDescriptions[procIt] == NULL)
        {
          assert( false);

          mLog.Log (LOG_WARNING, "Found a NULL procedure description!");

          continue;
        }

      const WLIB_PROC_DESCRIPTION& proc = *lib->procsDescriptions[procIt];

      ostringstream logEntry;

      logEntry << "Instaling native procedure '" << proc.name << "'...";

      mLog.Log (LOG_INFO, logEntry.str ());

      vector<uint32_t>   typesOffset;
      vector<StackValue> values;

      for (uint_t local = 0; local < proc.localsCount; ++local)
        {
          const uint8_t* const localType = proc.localsTypes[local];

          const uint_t tdSize = TypeManager::GetTypeLength( localType);
          auto_ptr<uint8_t> td (new uint8_t [tdSize]);
          memcpy( td.get (), localType, tdSize);

          //For table type values, the type would be changed to reflect the
          //order of the fields rearranged by the DBS layer.
          StackValue     value   = typeMgr.CreateLocalValue( td.get ());
          const uint32_t typeOff = typeMgr.AddType( td.get ());

          typesOffset.push_back( typeOff);

          //Keep a copy of stack values for locals to avoid construct them
          //every time when the procedure is called.
          values.push_back( value);
        }

      DefineProcedure( _RC (const uint8_t*, proc.name),
                       strlen( proc.name),
                       proc.localsCount,
                       proc.localsCount - 1,
                       0,
                       values,
                       &typesOffset[0],
                       _RC (const uint8_t*, proc.code),
                       0,
                       false,
                       NULL);
    }

  return true;
}


void
Session::ExecuteProcedure( const char* const   procedure,
                           SessionStack&       stack)
{
  const uint32_t procId = FindProcedure( _RC (const uint8_t*, procedure),
                                         strlen( procedure));

  if ( ! ProcedureManager::IsValid( procId))
    {
      throw InterException( _EXTRA( InterException::INVALID_PROC_REQ),
                            "Cannot find procedure '%s' to execute.",
                            procedure);
    }

  const Procedure& proc = GetProcedure( procId);

  ProcedureCall( *this, stack, proc);
}


uint_t
Session::GlobalValuesCount() const
{
  return mPrivateNames.Get ().GetGlobalsManager().Count();
}


uint_t
Session::ProceduresCount() const
{
  return mPrivateNames.Get ().GetProcedureManager().Count();
}


const char*
Session::GlobalValueName( const uint_t index) const
{
  return _RC (const char*,
              mPrivateNames.Get ().GetGlobalsManager().Name( index));
}


const char*
Session::ProcedureName( const uint_t index) const
{
  return _RC (const char*,
              mPrivateNames.Get ().GetProcedureManager().Name( index));
}


uint_t
Session::GlobalValueRawType( const uint32_t glbId)
{
  GlobalsManager& glbMgr = mPrivateNames.Get ().GetGlobalsManager();
  GlobalValue&    value  = glbMgr.Value( glbId);

  IOperand& glbOp = value.Operand();

  return glbOp.GetType();
}


uint_t
Session::GlobalValueRawType( const char* const name)
{
  GlobalsManager* glbMgr = &mPrivateNames.Get ().GetGlobalsManager();
  const uint32_t  glbId  = glbMgr->FindGlobal( _RC (const uint8_t*, name),
                                               strlen( name));
  return GlobalValueRawType( glbId);
}


uint_t
Session::GlobalValueFieldsCount( const uint32_t glbId)
{
  GlobalsManager& glbMgr = mPrivateNames.Get ().GetGlobalsManager();
  GlobalValue&    value  = glbMgr.Value( glbId);

  IOperand& glbOp = value.Operand();

  if (IS_TABLE( glbOp.GetType()))
    return glbOp.GetTable().FieldsCount();

  return 0;
}


uint_t
Session::GlobalValueFieldsCount( const char* const name)
{
  GlobalsManager* glbMgr = &mPrivateNames.Get ().GetGlobalsManager();
  uint32_t        glbId  = glbMgr->FindGlobal( _RC (const uint8_t*, name),
                                              strlen( name));
  return GlobalValueFieldsCount( glbId);
}


const char*
Session::GlobalValueFieldName( const uint32_t glbId, const uint32_t field)
{
  GlobalsManager& glbMgr = mPrivateNames.Get ().GetGlobalsManager();
  GlobalValue&    value  = glbMgr.Value( glbId);

  IOperand& glbOp = value.Operand();

  const DBSFieldDescriptor fd = glbOp.GetTable().DescribeField( field);

  return fd.name;
}


const char*
Session::GlobalValueFieldName( const char* const name, const uint32_t field)
{
  GlobalsManager* glbMgr = &mPrivateNames.Get ().GetGlobalsManager();
  uint32_t        glbId  = glbMgr->FindGlobal( _RC (const uint8_t*, name),
                                               strlen( name));

  return GlobalValueFieldName( glbId, field);
}


uint_t
Session::GlobalValueFieldType( const uint32_t glbId, const uint32_t field)
{
  GlobalsManager& glbMgr = mPrivateNames.Get ().GetGlobalsManager();
  GlobalValue&    value  = glbMgr.Value( glbId);

  IOperand& glbOp = value.Operand();

  const DBSFieldDescriptor fd = glbOp.GetTable().DescribeField( field);

  uint16_t type = fd.type;
  if (fd.isArray)
    MARK_ARRAY( type);

  return type;
}


uint_t
Session::GlobalValueFieldType( const char* const name, const uint32_t field)
{
  GlobalsManager* glbMgr = &mPrivateNames.Get ().GetGlobalsManager();
  uint32_t        glbId  = glbMgr->FindGlobal( _RC (const uint8_t*, name),
                                               strlen( name));

  return GlobalValueFieldType( glbId, field);
}


uint_t
Session::ProcedureParametersCount( const uint_t id) const
{
  return mPrivateNames.Get ().GetProcedureManager().ArgsCount( id) + 1;
}


uint_t
Session::ProcedureParametersCount( const char* const name) const
{
  ProcedureManager& procMgr = mPrivateNames.Get ().GetProcedureManager();
  const uint_t      procId  = procMgr.GetProcedure( _RC (const uint8_t*, name),
                                                    strlen( name));
  return ProcedureParametersCount( procId);
}


uint_t
Session::ProcedurePameterRawType( const uint_t id, const uint_t param)
{
  if (param >= ProcedureParametersCount( id))
    throw InterException( _EXTRA( InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr = mPrivateNames.Get ().GetProcedureManager();
  StackValue&       val = _CC (StackValue&, mgr.LocalValue( id, param));

  return val.Operand().GetType();
}


uint_t
Session::ProcedurePameterRawType( const char* const name, const uint_t param)
{
  ProcedureManager& procMgr = mPrivateNames.Get ().GetProcedureManager();
  const uint_t      procId  = procMgr.GetProcedure( _RC (const uint8_t*, name),
                                                    strlen( name));
  return ProcedurePameterRawType( procId, param);
}


uint_t
Session::ProcedurePameterFieldsCount( const uint_t id, const uint_t param)
{
  if (param >= ProcedureParametersCount( id))
    throw InterException( _EXTRA( InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr = mPrivateNames.Get ().GetProcedureManager();
  StackValue&       val = _CC (StackValue&, mgr.LocalValue( id, param));

  if (IS_TABLE( val.Operand().GetType()))
    return val.Operand().GetTable().FieldsCount();

  return 0;
}


uint_t
Session::ProcedurePameterFieldsCount( const char* const name,
                                      const uint_t      param)
{
  ProcedureManager& procMgr = mPrivateNames.Get ().GetProcedureManager();
  const uint_t      procId  = procMgr.GetProcedure( _RC (const uint8_t*, name),
                                                    strlen( name));
  return ProcedurePameterFieldsCount( procId, param);
}


const char*
Session::ProcedurePameterFieldName( const uint_t id,
                                    const uint_t param,
                                    const uint_t field)
{
  if (param >= ProcedureParametersCount( id))
    throw InterException( _EXTRA( InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr   = mPrivateNames.Get ().GetProcedureManager();
  StackValue&       val   = _CC (StackValue&, mgr.LocalValue( id, param));
  ITable&           table = val.Operand().GetTable();

  return table.DescribeField( field).name;
}


const char*
Session::ProcedurePameterFieldName( const char* const name,
                                    const uint_t        param,
                                    const uint_t        field)
{
  ProcedureManager& procMgr = mPrivateNames.Get ().GetProcedureManager();
  const uint_t      procId  = procMgr.GetProcedure( _RC (const uint8_t*, name),
                                                    strlen( name));
  return ProcedurePameterFieldName( procId, param, field);
}


uint_t
Session::ProcedurePameterFieldType( const uint_t id,
                                    const uint_t param,
                                    const uint_t field)
{
  if (param >= ProcedureParametersCount( id))
    throw InterException( _EXTRA( InterException::INVALID_LOCAL_REQ));

  ProcedureManager& mgr   = mPrivateNames.Get ().GetProcedureManager();
  StackValue&       val   = _CC (StackValue&, mgr.LocalValue( id, param));
  ITable&           table = val.Operand().GetTable();

  DBSFieldDescriptor fd = table.DescribeField( field);

  uint_t type = fd.type;
  if (fd.isArray)
    MARK_ARRAY( type);

  return type;
}


uint_t
Session::ProcedurePameterFieldType( const char* const name,
                                    const uint_t        param,
                                    const uint_t        field)
{
  ProcedureManager& procMgr = mPrivateNames.Get ().GetProcedureManager();
  const uint_t      procId  = procMgr.GetProcedure( _RC (const uint8_t*, name),
                                                    strlen( name));
  return ProcedurePameterFieldType( procId, param, field);
}


uint32_t
Session::FindGlobal( const uint8_t* pName, const uint_t nameLength)
{
  GlobalsManager* glbMgr  = &mPrivateNames.Get ().GetGlobalsManager();
  uint32_t        result  = glbMgr->FindGlobal( pName, nameLength);

  assert( GlobalsManager::IsGlobalEntry( result) == false);

  if (GlobalsManager::IsValid( result) == false)
    {
      glbMgr = &mGlobalNames.Get ().GetGlobalsManager();
      result = glbMgr->FindGlobal( pName, nameLength);

      assert( GlobalsManager::IsGlobalEntry( result) == false);

      GlobalsManager::MarkAsGlobalEntry( result);
    }

  return result;
}

StackValue
Session::GetGlobalValue( const uint32_t glbId)
{
  GlobalsManager& glbMgr = GlobalsManager::IsGlobalEntry( glbId) ?
                             mGlobalNames.Get ().GetGlobalsManager() :
                             mPrivateNames.Get ().GetGlobalsManager();

  GlobalOperand glbOp( glbMgr.Value( glbId));

  return StackValue( glbOp);
}


const uint8_t*
Session::FindGlobalTI( const uint32_t glbId)
{
  GlobalsManager& glbMgr = GlobalsManager::IsGlobalEntry( glbId) ?
                             mGlobalNames.Get ().GetGlobalsManager() :
                             mPrivateNames.Get ().GetGlobalsManager();

  return glbMgr.TypeDescription( glbId);
}


uint32_t
Session::FindProcedure( const uint8_t* name, const uint_t nameLength)
{
  ProcedureManager* procMgr = &mPrivateNames.Get ().GetProcedureManager();
  uint32_t          result  = procMgr->GetProcedure( name, nameLength);

  assert( ProcedureManager::IsGlobalEntry( result) == false);

  if ( ! ProcedureManager::IsValid( result))
    {
      procMgr = &mGlobalNames.Get ().GetProcedureManager();
      result  = procMgr->GetProcedure( name, nameLength);

      assert( ProcedureManager::IsGlobalEntry( result) == false);

      ProcedureManager::MarkAsGlobalEntry( result);
    }

  return result;
}


uint32_t
Session::ArgsCount( const uint32_t procId)
{
  ProcedureManager& procMgr = ProcedureManager::IsGlobalEntry( procId) ?
                                mGlobalNames.Get ().GetProcedureManager() :
                                mPrivateNames.Get ().GetProcedureManager();
  return procMgr.ArgsCount( procId);
}


const uint8_t*
Session::FindLocalTI( const uint32_t procId, const uint32_t local)
{
  ProcedureManager& procMgr = ProcedureManager::IsGlobalEntry( procId) ?
                                mGlobalNames.Get ().GetProcedureManager() :
                                mPrivateNames.Get ().GetProcedureManager();

  return procMgr.LocalTypeDescription( procId, local);
}

IDBSHandler&
Session::DBSHandler()
{
  return mPrivateNames.Get ().GetDBSHandler();
}


const Procedure&
Session::GetProcedure( const uint32_t procId)
{
  ProcedureManager& procMgr = ProcedureManager::IsGlobalEntry( procId) ?
                                mGlobalNames.Get ().GetProcedureManager() :
                                mPrivateNames.Get ().GetProcedureManager();

  const Procedure& procedure = procMgr.GetProcedure( procId);

  assert( procedure.mProcMgr == &procMgr);

  return procedure;
}


void
Session::DefineTablesGlobalValues()
{
  IDBSHandler& dbs = mPrivateNames.Get ().GetDBSHandler();

  const uint_t tablesCount = dbs.PersistentTablesCount();

  for (uint_t tableId = 0; tableId < tablesCount; ++tableId)
    {
      const char* const tableName = dbs.TableName( tableId);

      ITable& table = dbs.RetrievePersistentTable( tableId);

      try
      {
          vector<uint8_t> typeInfo = compute_table_typeinfo( table);

          DefineGlobalValue( _RC (const uint8_t*, tableName),
                             strlen( tableName),
                             &typeInfo.front(),
                             false,
                             &table);
      }
      catch( ...)
      {
          dbs.ReleaseTable( table);
          throw ;
      }
    }
}

uint32_t
Session::DefineGlobalValue( const uint8_t* const   name,
                            const uint_t           nameLength,
                            const uint8_t* const   typeDesc,
                            const bool             external,
                            ITable* const          persistentTable)
{
  assert( TypeManager::IsTypeValid( typeDesc));

  if (TypeManager::IsTypeValid( typeDesc) == false)
    {
      string message = "Could not add the global variable ";

      message += "'";
      message.insert( message.size(), _RC (const char*, name), nameLength);
      message += "' do to invalid type description.";

      mLog.Log (LOG_ERROR, message);

      throw InterException( _EXTRA( InterException::INVALID_TYPE_DESC));
    }


  auto_ptr<uint8_t> tdBuff( new uint8_t[TypeManager::GetTypeLength( typeDesc)]);
  memcpy( tdBuff.get (), typeDesc, TypeManager::GetTypeLength( typeDesc));

  TypeManager& typeMgr = mPrivateNames.Get ().GetTypeManager();
  GlobalValue  value   = typeMgr.CreateGlobalValue( tdBuff.get (),
                                                    persistentTable);
  const uint32_t glbEntry = FindGlobal( name, nameLength);

  if (GlobalsManager::IsValid( glbEntry) == false)
    {
      if (external)
        {
          string message = "Couldn't not find the definition for external "
                           "declaration of global value '";
          message.insert( message.size(), _RC (const char*, name), nameLength);
          message += "'.";

          mLog.Log (LOG_ERROR, message);

          throw InterException( _EXTRA( InterException::EXTERNAL_FIRST));
        }

      const uint32_t  typeOff = typeMgr.AddType( tdBuff.get ());
      GlobalsManager& glbMgr  = mPrivateNames.Get ().GetGlobalsManager();

      return glbMgr.AddGlobal( name, nameLength, value, typeOff);
    }
  else if (external)
    {
      const uint8_t* extTypeDesc = FindGlobalTI( glbEntry);

      if (memcmp( tdBuff.get (),
                  extTypeDesc,
                  TypeManager::GetTypeLength( tdBuff.get ()) ) != 0)
        {
          string message = "External declaration of global value '";
          message.insert( message.size(), _RC (const char*, name), nameLength);
          message += "' has a different type than its definition.";

          mLog.Log (LOG_ERROR, message);

          throw InterException( _EXTRA( InterException::EXTERNAL_MISMATCH));
        }
    }
  else
    {
      string message = "Duplicate definition of global value '";
      message.insert( message.size(), _RC (const char*, name), nameLength);
      message += "'.";

      mLog.Log (LOG_ERROR, message);

      throw InterException( _EXTRA( InterException::DUPLICATE_DEFINITION));
    }

  return glbEntry;
}


uint32_t
Session::DefineProcedure( const uint8_t* const   name,
                          const uint_t           nameLength,
                          const uint32_t         localsCount,
                          const uint32_t         argsCount,
                          const uint32_t         syncCount,
                          vector<StackValue>&    localValues,
                          const uint32_t* const  typesOffset,
                          const uint8_t* const   code,
                          const uint32_t         codeSize,
                          const bool             external,
                          Unit* const            unit)
{
  assert( argsCount < localsCount);
  assert( localsCount > 0);

  TypeManager& typeMgr = mPrivateNames.Get ().GetTypeManager();

  for (uint_t localIt = 0; localIt < localsCount; ++ localIt)
    {
      const uint8_t* const typeDesc = typeMgr.TypeDescription( 
                                                        typesOffset[localIt]
                                                              );

      if (TypeManager::IsTypeValid( typeDesc) == false)
        {
          string message = "Could not define the procedure ";

          message += "'";
          message.insert( message.size(), _RC (const char*, name), nameLength);
          message += "' do to invalid type description of local value.";

          mLog.Log (LOG_ERROR, message);

          throw InterException( _EXTRA( InterException::INVALID_TYPE_DESC));
        }
    }

  ProcedureManager& procMgr   = mPrivateNames.Get ().GetProcedureManager();
  const uint32_t    procIndex = FindProcedure( name, nameLength);

  if (external)
    {
      if (ProcedureManager::IsValid( procIndex) == false)
        {
          string message = "Couldn't not find the definition "
                           "for external procedure '";
          message.insert( message.size(),
                          _RC (const char*, name),
                          nameLength);
          message += "'.";

          mLog.Log (LOG_ERROR, message);

          throw InterException( _EXTRA( InterException::EXTERNAL_FIRST));
        }

      bool argsMatch = (argsCount == ArgsCount( procIndex));

      for (uint_t localIt = 0; (localIt <= argsCount) && argsMatch; ++localIt)
        {
          const uint8_t* const typeDesc = typeMgr.TypeDescription( 
                                                        typesOffset[localIt]
                                                                  );
          if (memcmp( typeDesc,
                      FindLocalTI( procIndex, localIt),
                      TypeManager::GetTypeLength( typeDesc) ) != 0)
            {
              argsMatch = false;
            }
        }

      if (argsMatch == false)
        {
          string message = "External declaration of procedure '";
          message.insert( message.size(), _RC (const char*, name), nameLength);
          message += "' has a different signature than its definition.";

          mLog.Log (LOG_ERROR, message);

          throw InterException( _EXTRA( InterException::EXTERNAL_MISMATCH));

        }

      return procIndex;
    }
  else if (ProcedureManager::IsValid( procIndex))
    {
      string message = "Duplicate definition of procedure '";
      message.insert( message.size(), _RC (const char*, name), nameLength);
      message += "'.";

      mLog.Log (LOG_ERROR, message);

      throw InterException( _EXTRA( InterException::DUPLICATE_DEFINITION));
    }

  return procMgr.AddProcedure( name,
                               nameLength,
                               localsCount,
                               argsCount,
                               syncCount,
                               localValues,
                               typesOffset,
                               code,
                               codeSize,
                               unit);
}

} //namespace prima
} //namespace whais

#if  defined( ENABLE_MEMORY_TRACE) && defined( USE_INTERP_SHL)
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "PRIMA";
#endif

