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
#include <errno.h>

#include "utils/include/wthread.h"

WSynchronizer::WSynchronizer ()
{
  const D_UINT result = wh_sync_init (&m_Sync);
  if (result != WOP_OK)
    throw WSynchException (NULL, _EXTRA (result));
}

WSynchronizer::~WSynchronizer ()
{
  const D_UINT result = wh_sync_destroy (&m_Sync);
  assert (result == WOP_OK);
}

void
WSynchronizer::Enter ()
{
  const D_UINT result = wh_sync_enter (&m_Sync);
  if (result != WOP_OK)
    throw WSynchException (NULL, _EXTRA (result));
}

void
WSynchronizer::Leave ()
{
  const D_UINT result = wh_sync_leave (&m_Sync);

  assert (result == WOP_OK);
}

WThread::WThread ()
  : m_Routine (NULL),
    m_RoutineArgs (NULL),
    m_Exception (NULL),
    m_ThreadHnd (0),
    m_UnkExceptSignaled (false),
    m_IgnoreExceptions (false),
    m_Ended (true),
    m_NeedsClean (false)
{
}

void
WThread::Run (WH_THREAD_ROUTINE routine, void* const args)
{
  assert (m_Ended);
  //Wait for the the previous thread to be cleared
  //But this should not happen.
  WaitToEnd ();

  assert (m_NeedsClean == false);

  m_Ended       = false;
  m_Routine     = routine;
  m_RoutineArgs = args;

  const D_UINT res = wh_thread_create (&m_ThreadHnd,
                                       WThread::ThreadWrapperRoutine,
                                       this);
  if (res != WOP_OK)
    {
      m_Ended = true;
      throw WThreadException (NULL, _EXTRA (errno));
    }

  m_NeedsClean = true;
}

WThread::~WThread ()
{
  WaitToEnd (false);

  assert (m_NeedsClean == false);

  //If you did not throwed the exception until now,
  //do not do it from during class destructor.
  delete m_Exception;
}

void
WThread::WaitToEnd (const bool throwPending)
{
  while (! m_Ended)
    wh_yield ();

  if (m_NeedsClean)
    wh_thread_free (m_ThreadHnd);

  m_NeedsClean = false;

  if (throwPending)
    ThrowPendingException ();
}

void
WThread::ThrowPendingException ()
{
  assert (m_Ended);

  if (HasExceptionPending () == false)
    return;

  if (m_UnkExceptSignaled)
    {
      DiscardException ();
      throw WThreadException (NULL, _EXTRA (WOP_UNKNOW));
    }

  if (m_Exception != NULL)
    {
      WException* pTemp = m_Exception->Clone ();
      DiscardException ();

      throw pTemp;
    }
}

void
WThread::ThreadWrapperRoutine (void* const args)
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
  catch (WException* pE)
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
