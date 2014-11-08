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

#include <assert.h>
#include <string.h>

#include "pm_procedures.h"
#include "pm_interpreter.h"
#include "pm_typemanager.h"


using namespace std;


namespace whais {
namespace prima {


uint32_t
ProcedureManager::AddProcedure( const uint8_t* const  name,
                                const uint_t          nameLength,
                                const uint32_t        localsCount,
                                const uint32_t        argsCount,
                                const uint32_t        syncCount,
                                vector<StackValue>&   localValues,
                                const uint32_t*       typesOffset,
                                const uint8_t*        code,
                                const uint32_t        codeSize,
                                Unit* const           unit)
{
  assert( GetProcedure( name, nameLength) == INVALID_ENTRY);
  assert( localsCount > 0);
  assert( code != NULL);
  assert( argsCount < localsCount);

  Procedure entry;

  entry.mId          = mProcsEntrys.size( );
  entry.mLocalsCount = localsCount;
  entry.mArgsCount   = argsCount;
  entry.mSyncCount   = syncCount;
  entry.mSyncIndex   = mSyncStmts.size( );
  entry.mLocalsIndex = mLocalsValues.size( );
  entry.mIdIndex     = mIdentifiers.size( );
  entry.mTypeOff     = mLocalsTypes.size( );
  entry.mCodeIndex   = mDefinitions.size( );
  entry.mCodeSize    = codeSize;
  entry.mUnit        = unit;
  entry.mProcMgr     = this;
  entry.mNativeCode  = (unit == NULL) ? _RC (WLIB_PROCEDURE, code) : NULL;

  const uint32_t result = mProcsEntrys.size( );

  mSyncStmts.insert( mSyncStmts.end(), syncCount, false);
  mLocalsValues.insert( mLocalsValues.end (),
                         &localValues[0],
                         &localValues[0] + localsCount);
  mIdentifiers.insert( mIdentifiers.end (), name, name + nameLength);
  mIdentifiers.push_back( 0);
  mDefinitions.insert( mDefinitions.end (), code, code + codeSize);
  mLocalsTypes.insert( mLocalsTypes.end (),
                       typesOffset,
                       typesOffset + localsCount);

  mProcsEntrys.push_back( entry);

  return result;
}


uint32_t
ProcedureManager::GetProcedure( const uint8_t* const name,
                                const uint_t         nameLength) const
{
  for (uint32_t index = 0; index < mProcsEntrys.size( ); ++index)
    {
      const char* const entryName =
              _RC (const char*, &mIdentifiers[mProcsEntrys[index].mIdIndex]);

      if ((strlen( entryName) == nameLength)
          && (memcmp( name, entryName, nameLength) == 0))
        {
          return index;
        }
    }

  return INVALID_ENTRY;
}

const Procedure&
ProcedureManager::GetProcedure( const uint32_t procId)
{
  const uint32_t procedure = procId & ~GLOBAL_ID;

  assert( procedure < mProcsEntrys.size( ));

  if (procedure >= mProcsEntrys.size( ))
    throw InterException( _EXTRA( InterException::INVALID_PROC_REQ));

  return mProcsEntrys[procedure];
}


const uint8_t*
ProcedureManager::Name( const uint_t procId) const
{
  if (procId >= mProcsEntrys.size( ))
    throw InterException( _EXTRA( InterException::INVALID_PROC_REQ));

  return &mIdentifiers[mProcsEntrys[procId].mIdIndex];
}

uint32_t
ProcedureManager::ArgsCount( const uint_t procId) const
{
  const uint32_t procedure = procId & ~GLOBAL_ID;

  if (procedure >= mProcsEntrys.size( ))
    throw InterException( _EXTRA( InterException::INVALID_PROC_REQ));

  return mProcsEntrys[procedure].mArgsCount;
}


const StackValue&
ProcedureManager::LocalValue( const uint_t   procId,
                              const uint32_t local) const
{
  const uint32_t procedure = procId & ~GLOBAL_ID;

  if (procedure >= mProcsEntrys.size( ))
    throw InterException( _EXTRA( InterException::INVALID_PROC_REQ));

  const Procedure& entry = mProcsEntrys[procedure];

  if (local >= entry.mLocalsCount)
    throw InterException( _EXTRA( InterException::INVALID_LOCAL_REQ));

  return mLocalsValues[entry.mLocalsIndex + local];
}


const uint8_t*
ProcedureManager::LocalTypeDescription( const uint_t   procId,
                                        const uint32_t local) const
{
  const uint32_t procedure = procId & ~GLOBAL_ID;

  assert( procedure < mProcsEntrys.size( ));

  if (procedure >= mProcsEntrys.size( ))
    throw InterException( _EXTRA( InterException::INVALID_PROC_REQ));

  const Procedure& entry = mProcsEntrys[procedure];

  assert( local < entry.mLocalsCount);

  if (local >= entry.mLocalsCount)
    throw InterException( _EXTRA( InterException::INVALID_LOCAL_REQ));

  const TypeManager& typeMgr = mNameSpace.GetTypeManager( );

  return typeMgr.TypeDescription( mLocalsTypes[entry.mTypeOff + local]);
}


const uint8_t*
ProcedureManager::Code( const Procedure&      proc,
                        uint_t* const         outCodeSize) const
{
  assert( proc.mProcMgr == this);

  if (outCodeSize != NULL)
    *outCodeSize = proc.mCodeSize;

  return &mDefinitions[proc.mCodeIndex];
}


void
ProcedureManager::AquireSync( const Procedure& proc, const uint32_t sync)
{
  assert( proc.mProcMgr == this);
  assert( sync < proc.mSyncCount);

  if (sync >= proc.mSyncCount)
    throw InterException( _EXTRA( InterException::INVALID_SYNC_REQ));

  do
    {
      LockRAII holder( mSync);
      const bool aquired = mSyncStmts[proc.mSyncIndex + sync];
      if (aquired)
        {
          //Some one has taken this before us! Prepare to try again!
          holder.Release( );
          wh_yield( );
        }
      else
        {
          mSyncStmts[proc.mSyncIndex + sync] = true;
          break;
        }
    }
  while( true);
}


void
ProcedureManager::ReleaseSync( const Procedure& proc, const uint32_t sync)
{
  assert( proc.mProcMgr == this);
  assert( sync < proc.mSyncCount);

  if (sync >= proc.mSyncCount)
    throw InterException(  _EXTRA( InterException::INVALID_SYNC_REQ));

  assert( mSyncStmts[proc.mSyncIndex + sync] == true);

  mSyncStmts[proc.mSyncIndex + sync] = false;
}


} //namespace prima
} //namespace whais

