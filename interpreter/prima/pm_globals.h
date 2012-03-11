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

#ifndef PM_GLOBALS_H_
#define PM_GLOBALS_H_

#include <vector>

#include "whisper.h"

#include "pm_operand.h"

namespace prima
{

class Session;

struct GlobalEntry
{
  D_UINT32  m_IdOffet;
  D_UINT32  m_TypeOffset;
};

class GlobalsManager
{
public:
  GlobalsManager (Session& session) :
    m_Session (session),
    m_Identifiers (),
    m_Storage (),
    m_GlobalsEntrys ()
  {
  }

  ~GlobalsManager ()
  {
  }

  D_UINT32           AddGlobal (const D_UINT8*     pIdentifier,
                                const D_UINT       identifierLength,
                                const GlobalValue& value,
                                const D_UINT32     typeOffset);
  D_UINT32           FindGlobal (const D_UINT8 *const pIdentifier,
                                 const D_UINT         identifierLength);

  GlobalValue&       GetGlobal (const D_UINT64 globalIndex);
  const D_UINT8*     GetGlobalTypeDesctiption (const D_UINT64 globalIndex);

  static const D_UINT32 INVALID_ENTRY = ~0;

private:
  GlobalsManager (const GlobalsManager&);
  GlobalsManager& operator= (const GlobalsManager);

protected:
  Session&                 m_Session;
  std::vector<D_UINT8>     m_Identifiers;
  std::vector<GlobalValue> m_Storage;
  std::vector<GlobalEntry> m_GlobalsEntrys;
};


}

#endif /* PM_GLOBALS_H_ */
