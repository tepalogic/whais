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

#include "utils/wthread.h"

namespace whisper
{

Lock::Lock ()
{
  const uint_t result = wh_lock_init (&mLock);
  if (result != WOP_OK)
    throw LockException (NULL, _EXTRA (result));
}

Lock::~Lock ()
{
  const uint_t result = wh_lock_destroy (&mLock);
  assert (result == WOP_OK);
}

void
Lock::Acquire ()
{
  const uint_t result = wh_lock_acquire (&mLock);
  if (result != WOP_OK)
    throw LockException (NULL, _EXTRA (result));
}

void
Lock::Release ()
{
  const uint_t result = wh_lock_release (&mLock);

  assert (result == WOP_OK);
}

Thread::Thread ()
  : mRoutine (NULL),
    mRoutineArgs (NULL),
    mException (NULL),
    mThread (0),
    mLock (),
    mUnkExceptSignaled (false),
    mIgnoreExceptions (false),
    mEnded (true),
    mNeedsClean (false)
{
}

void
Thread::Run (WH_THREAD_ROUTINE routine, void* const args)
{
  //Wait for the the previous thread to be cleared.
  WaitToEnd ();

  mLock.Acquire ();

  assert (mEnded);
  assert (mNeedsClean == false);

  mEnded       = false;
  mRoutine     = routine;
  mRoutineArgs = args;

  const uint_t res = wh_thread_create (&mThread,
                                       Thread::ThreadWrapperRoutine,
                                       this);
  if (res != WOP_OK)
    {
      mEnded = true;
      mLock.Release ();
      throw ThreadException (NULL, _EXTRA (errno));
    }

  mNeedsClean = true;
}

Thread::~Thread ()
{
  WaitToEnd (false);

  assert (mNeedsClean == false);

  //If you did not throwed the exception until now,
  //do not do it from during class destructor.
  delete mException;
}

void
Thread::WaitToEnd (const bool throwPending)
{
  LockRAII holder (mLock);

  assert (mEnded );

  if (mNeedsClean)
    wh_thread_free (mThread);

  mNeedsClean = false;

  if (throwPending)
    ThrowPendingException ();
}

void
Thread::ThrowPendingException ()
{
  assert (mEnded);

  if (HasExceptionPending () == false)
    return;

  if (mUnkExceptSignaled)
    {
      DiscardException ();
      throw ThreadException (NULL, _EXTRA (WOP_UNKNOW));
    }

  if (mException != NULL)
    {
      Exception* pTemp = mException->Clone ();
      DiscardException ();

      throw pTemp;
    }
}

void
Thread::ThreadWrapperRoutine (void* const args)
{
  Thread* const pThread = _RC(Thread*, args);

  try
  {
    pThread->mRoutine (pThread->mRoutineArgs);
  }
  catch (Exception &e)
  {
    if (pThread->mIgnoreExceptions == false)
      pThread->mException = e.Clone ();
  }
  catch (Exception* pE)
  {
    if (pThread->mIgnoreExceptions == false)
      pThread->mException = pE;
  }
  catch (...)
  {
    if (pThread->mIgnoreExceptions == false)
      pThread->mUnkExceptSignaled = true;
  }

  assert (pThread->mEnded == false);

  pThread->mEnded = true;
  pThread->mLock.Release ();
}

} //namespace whisper

