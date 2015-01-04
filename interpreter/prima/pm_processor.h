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

#ifndef PM_PROCESSOR_H_
#define PM_PROCESSOR_H_

#include "pm_interpreter.h"
#include "pm_procedures.h"

namespace whais {
namespace prima {

class ProcedureCall
{
public:
  ProcedureCall( Session&              session,
                 SessionStack&         stack,
                 const Procedure&      procedure);

  void AquireSync( const uint8_t sync);

  void ReleaseSync( const uint8_t sync);

  Session& GetSession() const
  {
    return mSession;
  }

  SessionStack& GetStack() const
  {
    return mStack;
  }

  const Unit& GetUnit() const
  {
    assert( mProcedure.mUnit != NULL);

    return *mProcedure.mUnit;
  }

  const uint8_t* Code() const
  {
    return mCode;
  }

  uint32_t CodeSize() const
  {
    return mProcedure.mCodeSize;
  }

  uint32_t CurrentOffset() const
  {
    return mCodePos;
  }

  size_t LocalsCount() const
  {
    return mProcedure.mLocalsCount;
  }

  const StackValue& GetLocalDefault (const uint_t local) const
  {
    return mProcedure.mProcMgr->LocalValue (mProcedure.mId, local);
  }

  uint32_t StackBegin() const
  {
    return mStackBegin;
  }

private:
  void Run ();

  const Procedure&        mProcedure;
  Session&                mSession;
  SessionStack&           mStack;
  const uint8_t*          mCode;
  uint32_t                mStackBegin;
  uint32_t                mCodePos;
  uint16_t                mAquiredSync;

  static const uint16_t NO_INDEX = 0xFFFF;
};

} //namespace prima
} //namespace whais


#endif //PM_PROCESSOR_H_

