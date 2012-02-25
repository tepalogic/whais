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

class Session;

class TypeManager
{
public:
  TypeManager (Session& session);
  ~TypeManager ();

  D_UINT32 FindTypeDescription (const D_UINT8* const pTypeDescription);
  D_UINT32 AddTypeDescription (const D_UINT8* const pTypeDescription);

  const D_UINT8* GetTypeDescription (const D_UINT32 offset) const;

  GlobalValue CreateGlobalValue (D_UINT8* pInOutTypeDescription);
  StackValue  CreateLocalValue (D_UINT8* pInOutTypeDescription);

  static bool   IsTypeDescriptionValid (const D_UINT8* pTypeDescription);
  static D_UINT GetTypeLength (const D_UINT8* pTypeDescription);

  static const D_UINT32 INVALID_OFFSET = 0xFFFFFFFF;

protected:

  Session&             m_Session;
  std::vector<D_UINT8> m_TypesDescriptions;
};





}


#endif /* PR_TYPEMANAGER_H_ */
