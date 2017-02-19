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

#ifndef PR_TYPEMANAGER_H_
#define PR_TYPEMANAGER_H_

#include <vector>

#include "whais.h"

#include "pm_operand.h"

namespace whais {
namespace prima {


class NameSpace;



class TypeManager
{

public:
  TypeManager(NameSpace& space);

  uint32_t FindType(const uint8_t* const typeDesc);

  uint32_t AddType(const uint8_t* const typeDesc);

  const uint8_t* TypeDescription(const uint32_t offset) const;

  static bool IsTypeValid(const uint8_t* const typeDesc);

  static uint_t GetTypeLength(const uint8_t* const typeDesc);

  GlobalValue CreateGlobalValue(uint8_t* const inoutTypeDesc,
                                 ITable*        persitentTable);

  StackValue  CreateLocalValue(uint8_t* const inoutTypeDesc);


  static const uint32_t INVALID_OFFSET = 0xFFFFFFFF;

private:
  TypeManager(const TypeManager&);
  TypeManager& operator= (const TypeManager&);


  NameSpace&           mNameSpace;
  std::vector<uint8_t> mTypesDescriptions;
};



std::vector<uint8_t>
compute_table_typeinfo(ITable& table);


} //namespace prima
} //namespace whais

#endif /* PR_TYPEMANAGER_H_ */

