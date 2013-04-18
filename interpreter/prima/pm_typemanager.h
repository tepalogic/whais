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

#ifndef PR_TYPEMANAGER_H_
#define PR_TYPEMANAGER_H_

#include <vector>

#include "whisper.h"

#include "pm_operand.h"

namespace whisper {
namespace prima {

class NameSpace;

class TypeManager
{
public:
  TypeManager (NameSpace& space);
  ~TypeManager ();

  uint32_t FindType (const uint8_t* const pTI);
  uint32_t AddType (const uint8_t* const pTI);

  const uint8_t* GetType (const uint32_t offset) const;

  GlobalValue CreateGlobalValue (uint8_t* const pInOutTI,
                                 I_DBSTable*    pPersistentTable);
  StackValue  CreateLocalValue (uint8_t* const pInOutTI);

  static bool   IsTypeValid (const uint8_t* pTI);
  static uint_t GetTypeLength (const uint8_t* pTI);

  static const uint32_t INVALID_OFFSET = 0xFFFFFFFF;

private:
  TypeManager (const TypeManager&);
  TypeManager& operator= (const TypeManager&);

  NameSpace&           m_NameSpace;
  std::vector<uint8_t> m_TypesDescriptions;
};

std::vector<uint8_t>
compute_table_typeinfo (I_DBSTable& table);

} //namespace prima
} //namespace whisper

#endif /* PR_TYPEMANAGER_H_ */

