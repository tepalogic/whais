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

#include "pm_interpreter.h"

using namespace std;
using namespace prima;

void
InitInterpreter ()
{
}

I_InterpreterSession&
GetInterpreterInstance ()
{
  static GlobalSession slGlobalSession;

  return slGlobalSession;
}

I_InterpreterSession&
GetInterpreterInstance (I_DBSHandler& dbsHandler)
{
  //TODO: Not implemented yet
  return GetInterpreterInstance ();
}

void
ReleaseInterpreterInstance (I_InterpreterSession& dbsHandler)
{
  //TODO: Not implemented yet
}


void
CleanInterpreter ()
{
  //TODO: Not implemented yet
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
        const D_UINT8* const pTypeDescriptor = typeMgr.GetTypeDescription (typeOffset);
        const D_UINT8* const pIdentifier     = _RC (const D_UINT8*, unit.RetriveGlobalName (glbIndex));
        const bool           external        = unit.IsGlobalExternal (glbIndex);


        const D_UINT32 glbDefIndex = DefineGlobalValue (pIdentifier, pTypeDescriptor, external);
        unitMgr.SetGlobalIndex (unitIndex, glbIndex, glbDefIndex);
      }

    for (D_UINT procIndex = 0; procIndex < unit.GetProceduresCount (); ++procIndex)
      {
        const D_UINT8* const pIdentifier = _RC (const D_UINT8*, unit.RetriveProcName (procIndex));
        const bool           external    = unit.IsProcExternal (procIndex);
        const D_UINT         localsCount = unit.GetProcLocalsCount (procIndex);
        const D_UINT         argsCount   = unit.GetProcParametersCount (procIndex);
        vector<D_UINT32>     typesOffset;
        vector<StackValue>   values;

        for (D_UINT localIndex = 0; localIndex < localsCount; ++localIndex)
          {
            const D_UINT8* const pLocalTypeDesc = unit.RetriveTypeInformation () +
                                                  ( localsCount == 0 ?
                                                    unit.GetProcReturnTypeIndex (procIndex) :
                                                    unit.GetProcLocalTypeIndex (procIndex,
                                                                                localsCount) );
            std::auto_ptr<D_UINT8> apTypeDesc (new D_UINT8 [TypeManager::GetTypeLength (pLocalTypeDesc)]);
            const StackValue value      = typeMgr.CreateLocalValue (apTypeDesc.get ());
            const D_UINT32   typeOffset = typeMgr.AddTypeDescription (apTypeDesc.get ());

            typesOffset.push_back (typeOffset);
            if ((localIndex == 0) || (localIndex >= argsCount))
              values.push_back (value);
          }

        const D_UINT32 procDefIndex = DefineProcedure (pIdentifier,
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
  //TODO: Need to be implemented
}

D_UINT32
Session::DefineGlobalValue (const D_UINT8* pIdentifier,
                            const D_UINT8* pTypeDescriptor,
                            const bool     external)
{
  assert (TypeManager::IsTypeDescriptionValid (pTypeDescriptor));

  if (TypeManager::IsTypeDescriptionValid (pTypeDescriptor) == false)
    {
      string message = "Could not add the global variable ";

      message += "'";
      message += _RC (const D_CHAR*, pIdentifier);
      message += "' do to invalid type description.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_TYPE_DESCRIPTION));
    }

  GlobalsManager& glbsMgr = GetGlobalsManager ();
  TypeManager&    typeMgr = GetTypeManager ();

  auto_ptr<D_UINT8> apTypeDescriptor (new D_UINT8[TypeManager::GetTypeLength (pTypeDescriptor)]);
  memcpy (apTypeDescriptor.get (), pTypeDescriptor, TypeManager::GetTypeLength (pTypeDescriptor));

  const D_UINT32  typeOffset = typeMgr.AddTypeDescription (apTypeDescriptor.get ());
  GlobalValue     value      = typeMgr.CreateGlobalValue (apTypeDescriptor.get ());
  const D_UINT32  glbEntry   = glbsMgr.FindGlobal (pIdentifier);

  if (glbEntry == glbsMgr.INVALID_ENTRY)
    {
      if (external)
        {
          string message = "Couldn't not find the definition for external declaration of global value '";
          message += _RC (const D_CHAR*, pIdentifier);
          message += "'.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterpreterException (NULL, _EXTRA (InterpreterException::EXTERNAL_FIRST));
        }

      return glbsMgr.AddGlobal (pIdentifier, value, typeOffset);
    }
  else if (external)
    {
      if (memcmp (apTypeDescriptor.get (),
                  glbsMgr.GetGlobalTypeDesctiption (glbEntry),
                  TypeManager::GetTypeLength (apTypeDescriptor.get ()) ) != 0)
        {
          string message = "External declaration of global value '";
          message += _RC (const D_CHAR*, pIdentifier);
          message += "' has a different type than its definition.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterpreterException (NULL, _EXTRA (InterpreterException::EXTERNAL_MISMATCH));
        }
    }
  else
    {
      string message = "Duplicate definition of global value '";
      message += _RC (const D_CHAR*, pIdentifier);
      message += "'.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterpreterException (NULL, _EXTRA (InterpreterException::DUPLICATE_DEFINITION));
    }

  return glbEntry;
}

D_UINT32
Session::DefineProcedure (const D_UINT8*    pIdentifier,
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

  TypeManager typeMgr = GetTypeManager ();

  for (D_UINT localIndex = 0; localIndex < localsCount; ++ localIndex)
    {
      const D_UINT8 *pTypeDescriptor = typeMgr.GetTypeDescription (pTypesOffset[localsCount]);
      assert (TypeManager::IsTypeDescriptionValid (pTypeDescriptor));

      if (TypeManager::IsTypeDescriptionValid (pTypeDescriptor) == false)
        {
          string message = "Could not define the procedure ";

          message += "'";
          message += _RC (const D_CHAR*, pIdentifier);
          message += "' do to invalid type description of local value.";

          LogMessage (LOG_INT_ERROR, message);

          throw InterpreterException (NULL, _EXTRA (InterpreterException::INVALID_TYPE_DESCRIPTION));
        }
    }
  ProcedureManager& procMgr = GetProcedureManager ();

  if (external)
    {
      D_UINT32 procIndex = procMgr.GetProcedure (pIdentifier);

      if (procIndex == procMgr.INVALID_ENTRY)
        {
          string message = "Couldn't not find the definition for external procedure '";
          message += _RC (const D_CHAR*, pIdentifier);
          message += "'.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterpreterException (NULL, _EXTRA (InterpreterException::EXTERNAL_FIRST));
        }

      bool argsMatch = (localsCount == procMgr.GetLocalsCount (procIndex)) &&
                       (argsCount == procMgr.GetArgsCount (procIndex));

      for (D_UINT localIndex = 0; (localIndex < localsCount) && argsMatch; ++localIndex)
        {
          const D_UINT8 *pTypeDescriptor = typeMgr.GetTypeDescription (pTypesOffset[localsCount]);
          if (memcmp (pTypeDescriptor,
                      procMgr.GetLocalType (procIndex, localIndex),
                      TypeManager::GetTypeLength (pTypeDescriptor) ) != 0)
            argsMatch = false;
        }

      if (argsMatch == false)
        {
          string message = "External declaration of procedure '";
          message += _RC (const D_CHAR*, pIdentifier);
          message += "' has a different signature than its definition.";

          LogMessage (LOG_INT_ERROR, message);
          throw InterpreterException (NULL, _EXTRA (InterpreterException::EXTERNAL_MISMATCH));

        }

      return procIndex;
    }
  else
    {
      string message = "Duplicate definition of procedure '";
      message += _RC (const D_CHAR*, pIdentifier);
      message += "'.";

      LogMessage (LOG_INT_ERROR, message);

      throw InterpreterException (NULL, _EXTRA (InterpreterException::DUPLICATE_DEFINITION));
    }

  return procMgr.AddProcedure (pIdentifier,
                               localsCount,
                               argsCount,
                               syncCount,
                               pLocalValues,
                               pTypesOffset,
                               pCode,
                               codeSize);
}
