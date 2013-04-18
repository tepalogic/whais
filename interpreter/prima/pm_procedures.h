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

#ifndef PM_PROCEDURES_H_
#define PM_PROCEDURES_H_

#include <vector>

#include "whisper.h"

#include "pm_operand.h"

namespace whisper {
namespace prima {

struct Unit;
struct ProcedureEntry
{
  uint32_t m_LocalsCount;
  uint32_t m_ArgsCount;
  uint32_t m_SyncCount;
  uint32_t m_SyncIndex;
  uint32_t m_LocalsIndex;
  uint32_t m_IdIndex;
  uint32_t m_TypeOff;
  uint32_t m_CodeIndex;
  uint32_t m_CodeSize;
  Unit*    m_pUnit;
};

class NameSpace;

class ProcedureManager
{
public:
  ProcedureManager (NameSpace& space)
    : m_NameSpace (space),
      m_ProcsEntrys (),
      m_Identifiers (),
      m_LocalsValues (),
      m_LocalsTypes (),
      m_Definitions (),
      m_SyncStmts (),
      m_Sync ()
  {
  }

  uint_t Count () const { return m_ProcsEntrys.size (); };

  uint32_t AddProcedure (const uint8_t*           pName,
                         const uint_t             nameLength,
                         const uint32_t           localsCount,
                         const uint32_t           argsCount,
                         const uint32_t           syncCount,
                         std::vector<StackValue>& localValues,
                         const uint32_t*          pTypesOffset,
                         const uint8_t*           pCode,
                         const uint32_t           codeSize,
                         Unit&                    unit);
  uint32_t GetProcedure (const uint8_t* pName,
                         const uint_t   nameLength) const;

  const uint8_t* Name (const uint_t procEntry) const;
  Unit&          GetUnit (const uint_t procEntry) const;
  uint32_t       LocalsCount (const uint_t procEntry) const;
  uint32_t       ArgsCount (const uint_t procEntry) const;

  const StackValue& LocalValue (const uint_t   procEntry,
                                const uint32_t local) const;
  const uint8_t*    LocalTI (const uint_t   procEntry,
                             const uint32_t local) const;
  const uint8_t*    Code (const uint_t procEntry, uint64_t* pOutCodeSize) const;

  void AquireSync (const uint_t procEntry, const uint32_t sync);
  void ReleaseSync (const uint_t procEntry, const uint32_t sync);

  static bool IsValid (const uint32_t entry)
  {
    return entry != INVALID_ENTRY;
  }

  static bool IsGlobalEntry (const uint32_t entry)
  {
    return IsValid (entry) && ((entry & GLOBAL_ID) != 0);
  }

  static void MarkAsGlobalEntry (uint32_t& entry)
  {
    entry |= GLOBAL_ID;
  }

private:
  ProcedureManager (const ProcedureManager&);
  ProcedureManager& operator= (const ProcedureManager&);

  static const uint32_t GLOBAL_ID     = 0x80000000;
  static const uint32_t INVALID_ENTRY = 0xFFFFFFFF;

  NameSpace&                  m_NameSpace;
  std::vector<ProcedureEntry> m_ProcsEntrys;
  std::vector<uint8_t>        m_Identifiers;
  std::vector<StackValue>     m_LocalsValues;
  std::vector<uint32_t>       m_LocalsTypes;
  std::vector<uint8_t>        m_Definitions;
  std::vector<bool>           m_SyncStmts;
  Lock               m_Sync;
};

} //namespace prima
} //namespace whisper

#endif /* PM_PROCEDURES_H_ */

