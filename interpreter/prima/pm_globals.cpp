/******************************************************************************
 PASTRA - A light database one file system and more.
 Copyright(C) 2008  Iulian Popa

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

#include "pm_globals.h"
#include "pm_interpreter.h"

using namespace std;



namespace whais {
namespace prima {



GlobalsManager::~GlobalsManager()
{
#if 0
  for (vector<GlobalValue>::iterator it = mStorage.begin();
       it != mStorage.end();
       ++it)
    {
      it->Operand().~BaseOperand();
    }
#endif
}


uint32_t
GlobalsManager::AddGlobal(const uint8_t* const name,
                           const uint_t         nameLength,
                           const GlobalValue&   value,
                           const uint32_t       typeOffset)
{
  assert(FindGlobal(name, nameLength) == INVALID_ENTRY);
  assert(mGlobalsEntrys.size() == mStorage.size());

  const uint32_t result   = mGlobalsEntrys.size();
  const uint32_t IdOffset = mIdentifiers.size();

  mIdentifiers.insert(mIdentifiers.end(), name, name + nameLength);
  mIdentifiers.push_back(0);

  mStorage.push_back(value);

  const GlobalEntry entry = {IdOffset, typeOffset};

  mGlobalsEntrys.push_back(entry);

  return result;
}


uint32_t
GlobalsManager::FindGlobal(const uint8_t* const name,
                            const uint_t         nameLength)
{
  assert(mGlobalsEntrys.size() == mStorage.size());

  uint32_t iterator = 0;

  while(iterator < mGlobalsEntrys.size())
    {
      const GlobalEntry& entry     = mGlobalsEntrys[iterator];
      const char* const  entryName = _RC(const char*,
                                          &mIdentifiers[entry.mIdOffet]);

      if ((strlen(entryName) == nameLength)
          && (memcmp(entryName, name, nameLength) == 0))
        {
          return iterator;
        }

      ++iterator;
    }

  return INVALID_ENTRY;
}


const uint8_t*
GlobalsManager::Name(const uint_t index) const
{
  if (index >= mStorage.size())
    throw InterException(_EXTRA(InterException::INVALID_GLOBAL_REQ));

  const GlobalEntry& entry = mGlobalsEntrys[index];

  return  &mIdentifiers [entry.mIdOffet];
}

GlobalValue&
GlobalsManager::Value(const uint32_t glbId)
{
  const uint32_t index = glbId & ~GLOBAL_ID;

  assert(mGlobalsEntrys.size() == mStorage.size());

  if ((IsValid(glbId) == false) || (index >= mStorage.size()))
    throw InterException(_EXTRA(InterException::INVALID_GLOBAL_REQ));

  return mStorage[index];
}

const uint8_t*
GlobalsManager::TypeDescription(const uint32_t glbId)
{
  const uint32_t index = glbId & ~GLOBAL_ID;

  assert(IsValid(glbId));
  assert(mGlobalsEntrys.size() == mStorage.size());
  assert(index < mGlobalsEntrys.size());

  if (index >= mGlobalsEntrys.size())
    throw InterException(_EXTRA(InterException::INVALID_GLOBAL_REQ));

  TypeManager&   typeMgr = mNames.GetTypeManager();
  const uint8_t* pType   = typeMgr.TypeDescription(
                                          mGlobalsEntrys[index].mTypeOffset
                                                   );

  assert(typeMgr.IsTypeValid(pType));

  return pType;
}

} //namespace prima
} //namespace whais

