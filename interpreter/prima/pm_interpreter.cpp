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
  TypeManager& typeMgr = GetTypeManager ();

  for (D_UINT glbIndex = 0; glbIndex < unit.GetGlobalsCount(); ++glbIndex)
    {
      const D_UINT typeOffset              = unit.GetGlobalTypeIndex (glbIndex);
      const D_UINT8* const pTypeDescriptor = typeMgr.GetTypeDescription (typeOffset);
      const D_UINT8* const pIdentifier     = _RC (const D_UINT8*, unit.RetriveGlobalName (glbIndex));
      const bool           external        = unit.IsGlobalExternal (glbIndex);

      DefineGlobalValue (pIdentifier, pTypeDescriptor, external);
    }
}

void
Session::LogMessage (const LOG_LEVEL level, std::string& message)
{
  //TODO: Need to be implemented
}

void
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
  const D_UINT64  glbEntry   = glbsMgr.FindGlobal (pIdentifier);

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
      else
        glbsMgr.AddGlobal (pIdentifier, value, typeOffset);
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
}
