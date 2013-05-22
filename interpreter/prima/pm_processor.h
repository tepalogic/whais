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

namespace whisper {
namespace prima {

class Processor
{
public:
  Processor (Session& session, SessionStack& stack, const uint32_t procId);

  void Run ();
  void AquireSync (const uint8_t sync);
  void ReleaseSync (const uint8_t sync);

  Session&       GetSession () const { return mSession; }
  SessionStack&  GetStack () const { return mStack; }
  const Unit&    GetUnit () const { return mProcUnit; }

  const uint8_t* Code () const { return mpCode; }
  uint64_t       CurrentOffset () const { return mCodePos; }
  uint64_t       CodeSize () const { return mCodeSize; }

  size_t         LocalsCount () const { return mLocalsCount; }
  size_t         StackBegin () const { return mStackBegin; }

private:
  Session&       mSession;
  SessionStack&  mStack;
  const Unit&    mProcUnit;
  const uint8_t* mpCode;
  const uint64_t mCodeSize;
  uint64_t       mCodePos;
  const uint_t   mLocalsCount;
  const size_t   mStackBegin;
  const uint32_t mProcId;
  uint16_t       mAquiredSync;

  static const uint16_t NO_INDEX = 0xFFFF;
};

} //namespace prima
} //namespace whisper


#endif //PM_PROCESSOR_H_

