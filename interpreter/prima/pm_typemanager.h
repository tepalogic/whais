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

namespace prima
{

class NameSpace;

class TypeManager
{
public:
  TypeManager (NameSpace& space);
  ~TypeManager ();

  D_UINT32 FindType (const D_UINT8* const pTI);
  D_UINT32 AddType (const D_UINT8* const pTI);

  const D_UINT8* GetType (const D_UINT32 offset) const;

  GlobalValue CreateGlobalValue (D_UINT8* const pInOutTI,
                                 I_DBSTable*    pPersistentTable);
  StackValue  CreateLocalValue (D_UINT8* const pInOutTI);

  static bool   IsTypeValid (const D_UINT8* pTI);
  static D_UINT GetTypeLength (const D_UINT8* pTI);

  static const D_UINT32 INVALID_OFFSET = 0xFFFFFFFF;

private:
  TypeManager (const TypeManager&);
  TypeManager& operator= (const TypeManager&);

  NameSpace&           m_NameSpace;
  std::vector<D_UINT8> m_TypesDescriptions;
};

std::vector<D_UINT8>
compute_table_typeinfo (I_DBSTable& table);

}

#endif /* PR_TYPEMANAGER_H_ */

