/******************************************************************************
WHISPER - An advanced database system
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
******************************************************************************/

#include <assert.h>

#include "wthread.h"
#include "wfile.h"

void
__internal_thread_routine (const WH_THREAD_ROUTINE routine, void *const args)
{
  WThread* const pThread = _RC(WThread*, args);

  try
  {
    pThread->m_Routine (pThread->m_RoutineArgs);
  }
  catch (WException &e)
  {
    if (pThread->m_IgnoreExceptions == false)
      pThread->m_Exception = e.Clone ();
  }
  catch (WException *pE)
  {
    if (pThread->m_IgnoreExceptions == false)
      pThread->m_Exception = pE;
  }
  catch (...)
  {
    if (pThread->m_IgnoreExceptions == false)
      pThread->m_UnkExceptSignaled = true;
  }

  pThread->m_Ended = true;
}

WThread::WThread (WH_THREAD_ROUTINE routine, void* const args) :
  m_Routine (routine),
  m_RoutineArgs (args),
  m_Exception (NULL),
  m_ThreadHnd (0),
  m_UnkExceptSignaled (false),
  m_IgnoreExceptions (false),
  m_Ended (true)
{
  if (wh_thread_create (&m_ThreadHnd, __internal_thread_routine, this) != WOP_OK)
    throw WThreadException (NULL, _EXTRA (FAILEDOP_EXCEPTION));
}

WThread::~WThread ()
{
  wh_thread_join (m_ThreadHnd);

  delete m_Exception;
  //Do not throw exceptions from a destructor. Is not a smart idea at all.
}

void
WThread::Join ()
{
  wh_thread_join (m_ThreadHnd);
  SignalPendingException ();
}

void
WThread::SignalPendingException ()
{
  assert (m_Ended);

  if (HasExceptionPending () == false)
    return;

  if (m_UnkExceptSignaled)
    {
      DiscardException ();
      throw WThreadException (NULL, _EXTRA(0));
    }

  if (m_Exception != NULL)
    {
      WException* pTemp = m_Exception;
      DiscardException ();

      throw pTemp;
    }
}