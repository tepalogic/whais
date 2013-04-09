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

namespace prima
{

class Processor
{
public:
  Processor (Session& session, SessionStack& stack, const uint32_t procId);

  void Run ();
  void AquireSync (const uint8_t sync);
  void ReleaseSync (const uint8_t sync);

  Session&       GetSession () const { return m_Session; }
  SessionStack&  GetStack () const { return m_Stack; }
  const Unit&    GetUnit () const { return m_ProcUnit; }

  const uint8_t* Code () const { return m_pCode; }
  uint64_t       CurrentOffset () const { return m_CodePos; }
  uint64_t       CodeSize () const { return m_CodeSize; }

  size_t         LocalsCount () const { return m_LocalsCount; }
  size_t         StackBegin () const { return m_StackBegin; }

private:
  Session&       m_Session;
  SessionStack&  m_Stack;
  const Unit&    m_ProcUnit;
  const uint8_t* m_pCode;
  const uint64_t m_CodeSize;
  uint64_t       m_CodePos;
  const uint_t   m_LocalsCount;
  const size_t   m_StackBegin;
  const uint32_t m_ProcId;
  uint16_t       m_AquiredSync;

  static const uint16_t NO_INDEX = 0xFFFF;
};

}

#endif //PM_PROCESSOR_H_

