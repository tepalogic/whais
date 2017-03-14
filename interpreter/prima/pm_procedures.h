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

#ifndef PM_PROCEDURES_H_
#define PM_PROCEDURES_H_

#include <vector>

#include "whais.h"
#include "stdlib/interface.h"
#include "pm_operand.h"


namespace whais {
namespace prima {


class  NameSpace;
struct Unit;
class  ProcedureManager;

struct Procedure
{
  uint32_t          mId;
  uint32_t          mLocalsCount;
  uint32_t          mArgsCount;
  uint32_t          mSyncCount;
  uint32_t          mSyncIndex;
  uint32_t          mLocalsIndex;
  uint32_t          mIdIndex;
  uint32_t          mTypeOff;
  uint32_t          mCodeIndex;
  uint32_t          mCodeSize;
  WLIB_PROCEDURE    mNativeCode;
  Unit*             mUnit;
  ProcedureManager* mProcMgr;
};

class ProcedureManager
{
public:
  ProcedureManager(NameSpace& space)
    : mNameSpace(space)
  {
  }
  ProcedureManager(const ProcedureManager&) = delete;
  ProcedureManager& operator= (const ProcedureManager&) = delete;

  uint_t Count() const { return mProcsEntrys.size(); }

  uint32_t AddProcedure(const uint8_t* const name,
                        const uint_t nameLength,
                        const uint32_t localsCount,
                        const uint32_t argsCount,
                        const uint32_t syncCount,
                        std::vector<StackValue>& localValues,
                        const uint32_t* typesOffset,
                        const uint8_t* code,
                        const uint32_t codeSize,
                        Unit* const unit);

  uint32_t GetProcedure(const uint8_t* const name, const uint_t nameLength) const;
  const Procedure& GetProcedure(const uint32_t procId);

  const uint8_t* Name(const uint_t procId) const;
  uint32_t ArgsCount(const uint_t procId) const;
  const StackValue& LocalValue(const uint_t procId, const uint32_t local) const;
  const uint8_t* LocalTypeDescription(const uint_t procId, const uint32_t local) const;
  const uint8_t* Code(const Procedure& proc, uint_t* const outCodeSize) const;

  void AquireSync(const Procedure& proc, const uint32_t sync);
  void ReleaseSync(const Procedure& proc, const uint32_t sync);

  static bool IsValid(const uint32_t entry) { return entry != INVALID_ENTRY; }
  static bool IsGlobalEntry(const uint32_t entry)
  {
    return IsValid(entry) && ((entry & GLOBAL_ID) != 0);
  }
  static void MarkAsGlobalEntry(uint32_t& entry) { entry |= GLOBAL_ID; }


private:
  static const uint32_t GLOBAL_ID     = 0x80000000;
  static const uint32_t INVALID_ENTRY = 0xFFFFFFFF;

  NameSpace&                  mNameSpace;
  std::vector<Procedure>      mProcsEntrys;
  std::vector<uint8_t>        mIdentifiers;
  std::vector<StackValue>     mLocalsValues;
  std::vector<uint32_t>       mLocalsTypes;
  std::vector<uint8_t>        mDefinitions;
  std::vector<bool>           mSyncStmts;
  Lock                        mSync;
};


} //namespace prima
} //namespace whais

#endif /* PM_PROCEDURES_H_ */

