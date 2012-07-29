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
  Processor (Session&       session,
             SessionStack&  stack,
             const Unit&    procUnit,
             const D_UINT8* pCode,
             const D_UINT64 codeSize,
             const D_UINT   localsCount,
             const D_UINT   procId)
    : m_Session (session),
      m_Stack (stack),
      m_ProcUnit (procUnit),
      m_pCode (pCode),
      m_CodeSize (codeSize),
      m_CodePos (0),
      m_LocalsCount (localsCount),
      m_StackBegin (stack.Size () - localsCount),
      m_ProcId (procId),
      m_AquiredSync (~0)
  {
    if (localsCount > stack.Size ())
      throw InterException (NULL, _EXTRA (InterException::STACK_CORRUPTED));
  }

  void Run ();
  void AquireSync (const D_UINT8 sync);
  void ReleaseSync (const D_UINT8 sync);

  Session&       GetSession () const { return m_Session; }
  SessionStack&  GetStack () const { return m_Stack; }
  const Unit&    GetUnit () const { return m_ProcUnit; }

  const D_UINT8* Code () const { return m_pCode; }
  D_UINT64       CurrentOffset () const { return m_CodePos; }
  D_UINT64       CodeSize () const { return m_CodeSize; }

  size_t         LocalsCount () const { return m_LocalsCount; }
  size_t         StackBegin () const { return m_StackBegin; }

private:
  Session&       m_Session;
  SessionStack&  m_Stack;
  const Unit&    m_ProcUnit;
  const D_UINT8* m_pCode;
  const D_UINT64 m_CodeSize;
  D_UINT64       m_CodePos;
  const D_UINT   m_LocalsCount;
  const size_t   m_StackBegin;
  const D_UINT32 m_ProcId;
  D_UINT16       m_AquiredSync;
};

}

#endif //PM_PROCESSOR_H_

