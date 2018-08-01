/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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
******************************************************************************/

#include <assert.h>
#include <string.h>

#include "pm_globals.h"
#include "pm_interpreter.h"


using namespace std;

namespace whais {
namespace prima {


uint32_t
GlobalsManager::AddGlobal(const uint8_t* const name,
                          const uint_t         nameLength,
                          GlobalValue&&        value,
                          const uint32_t       typeOffset)
{
  assert(FindGlobal(name, nameLength) == INVALID_ENTRY);
  assert(mGlobalsEntrys.size() == mStorage.size());

  const uint32_t result   = mGlobalsEntrys.size();
  const uint32_t IdOffset = mIdentifiers.size();

  mIdentifiers.insert(mIdentifiers.end(), name, name + nameLength);
  mIdentifiers.push_back(0);

  mStorage.push_back(std::move(value));

  const GlobalEntry entry = {IdOffset, typeOffset};

  mGlobalsEntrys.push_back(entry);

  return result;
}

uint32_t
GlobalsManager::FindGlobal(const uint8_t* const name, const uint_t nameLength)
{
  assert(mGlobalsEntrys.size() == mStorage.size());

  uint32_t iterator = 0;
  for (auto& entry : mGlobalsEntrys)
  {
    const auto entryName = _RC(const char*, &mIdentifiers[entry.mIdOffet]);

    if (strlen(entryName) == nameLength
        && memcmp(entryName, name, nameLength) == 0)
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

  return &mIdentifiers[mGlobalsEntrys[index].mIdOffet];
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

  TypeManager& typeMgr = mNames.GetTypeManager();
  const uint8_t* const pType = typeMgr.TypeDescription(mGlobalsEntrys[index].mTypeOffset);

  assert(typeMgr.IsTypeValid(pType));

  return pType;
}


} //namespace prima
} //namespace whais
