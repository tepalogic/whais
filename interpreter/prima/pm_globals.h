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

class NameSpace;

struct GlobalEntry
{
  D_UINT32  m_IdOffet;
  D_UINT32  m_TypeOffset;
};

class GlobalsManager
{
public:
  GlobalsManager (NameSpace& space)
    : m_Names (space),
      m_Identifiers (),
      m_Storage (),
      m_GlobalsEntrys ()
  {
  }

  ~GlobalsManager ();

  D_UINT Count () const { return m_GlobalsEntrys.size (); };

  D_UINT32           AddGlobal (const D_UINT8*     pName,
                                const D_UINT       nameLength,
                                GlobalValue&       value,
                                const D_UINT32     tiOffset);
  D_UINT32           FindGlobal (const D_UINT8 *const pName,
                                 const D_UINT         nameLength);

  GlobalValue&       GetGlobal (const D_UINT32 glbId);
  const D_UINT8*     Name (const D_UINT index) const;
  const D_UINT8*     GetGlobalTI (const D_UINT32 glbId);

  static bool IsValid (const D_UINT32 glbId)
  {
    return glbId != INVALID_ENTRY;
  }

  static bool IsGlobalEntry (const D_UINT32 glbId)
  {
    return IsValid (glbId) && ((glbId & GLOBAL_ID) != 0);
  }

  static void MarkAsGlobalEntry (D_UINT32& glbId)
  {
    glbId |= GLOBAL_ID;
  }

private:
  GlobalsManager (const GlobalsManager&);
  GlobalsManager& operator= (const GlobalsManager);

  static const D_UINT32 GLOBAL_ID     = 0x80000000;
  static const D_UINT32 INVALID_ENTRY = 0xFFFFFFFF;

  NameSpace&               m_Names;
  std::vector<D_UINT8>     m_Identifiers;
  std::vector<GlobalValue> m_Storage;
  std::vector<GlobalEntry> m_GlobalsEntrys;
};

}

#endif /* PM_GLOBALS_H_ */
