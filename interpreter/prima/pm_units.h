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

#ifndef PM_UNITS_H_
#define PM_UNITS_H_

#include <vector>

#include <whisper.h>

namespace whisper {
namespace prima {

struct Unit
{
  uint32_t mGlbsCount;
  uint32_t mProcsCount;
  uint32_t mConstSize;
  uint8_t  mUnitData[1];

  void SetGlobalId (const uint32_t globalIndex, const uint32_t globalId);
  void SetProcId (const uint32_t procIndex, const uint32_t procId);

  uint32_t       GetGlobalId (const uint32_t globalIndex) const;
  uint32_t       GetProcId (const uint32_t procIndex) const;
  const uint8_t* GetConstData (const uint32_t offset) const;
};

class UnitsManager
{
public:
  UnitsManager ()
    : mUnits ()
  {
  }

  ~UnitsManager ();

  uint32_t AddUnit (const uint32_t glbsCount,
                    const uint32_t procsCount,
                    const uint8_t* pConstData,
                    const uint32_t constAreaSize);
  void     RemoveLastUnit ();
  Unit&    GetUnit (const uint32_t unitIndex);
  void     SetGlobalIndex (const uint32_t unitIndex,
                           const uint32_t unitGlbIndex,
                           const uint32_t glbMgrIndex);
  void     SetProcIndex (const uint32_t unitIndex,
                         const uint32_t unitProcIndex,
                         const uint32_t procMgrIndex);
  uint32_t GetGlobalIndex (const uint32_t unitIndx,
                           const uint32_t unitGlbIndex) const;
  uint32_t GetProcIndex (const uint32_t unitIndex,
                         const uint32_t unitProcIndex) const;

private:
  std::vector<Unit*> mUnits;
};

} //namespace prima
} //namespace whisper

#endif /* PM_UNITS_H_ */

