/******************************************************************************
WHAIS - An advanced database system
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
******************************************************************************/

#include <assert.h>
#include <errno.h>

#include "utils/wthread.h"



namespace whais
{


Lock::Lock()
{
  const uint_t result = wh_lock_init(&mLock);

  if (result != WOP_OK)
  {
    assert(false);

    throw LockException(_EXTRA(result), "Failed to initialize a lock.");
  }
}


Lock::~Lock()
{
  const uint_t result = wh_lock_destroy(&mLock);

  (void)result;
  assert(result == WOP_OK);
}


void
Lock::lock()
{
  const uint_t result = wh_lock_acquire(&mLock);
  if (result != WOP_OK)
  {
    assert(false);

    throw LockException(_EXTRA(result), "Failed to acquire a lock.");
  }
}


bool
Lock::try_lock()
{
  bool_t acquired;

  const int result = wh_lock_try_acquire(&mLock, &acquired);

  if (result != WOP_OK)
  {
    assert(false);

    throw LockException(_EXTRA(result), "Failed to try to acquire a lock.");
  }

  return acquired != FALSE;
}



void
Lock::unlock()
{
  const uint_t result = wh_lock_release(&mLock);

  (void)result;
  assert(result == WOP_OK);
}




SpinLock::SpinLock()
  : mLock(0)
{
}


void
SpinLock::lock()
{
  while (true)
    {
      assert(mLock >= 0);

      if (wh_atomic_fetch_inc16(&mLock) == 0)
        break;

      wh_atomic_fetch_dec16(&mLock);
      wh_yield();
    }
}


bool
SpinLock::try_lock()
{
  assert(mLock >= 0);

  if (wh_atomic_fetch_inc16(&mLock) == 0)
    return true;

  wh_atomic_fetch_dec16(&mLock);
  return false;
}

void
SpinLock::unlock()
{
  assert(mLock > 0);

  wh_atomic_fetch_dec16(&mLock);
}



Thread::Thread()
  : mRoutine(nullptr),
    mRoutineArgs(nullptr),
    mException(nullptr),
    mThread(0),
    mEnded(0),
    mUnkExceptSignaled(false),
    mIgnoreExceptions(false),
    mNeedsClean(false)
{
}


bool
Thread::Run(WH_THREAD_ROUTINE routine, void* const args, const bool waitPrevEnd)
{
  while (true)
  {
    if (wh_atomic_fetch_inc32( &mEnded) == 0)
      break;

    wh_atomic_fetch_dec32( &mEnded);

    if ( !waitPrevEnd)
      return false;

    WaitToEnd(true);
  }

  if (mNeedsClean)
  {
    wh_thread_free(mThread);
    mNeedsClean = false;

    ThrowPendingException();
  }

  assert(mEnded > 0);
  assert(mNeedsClean == false);

  mRoutine = routine;
  mRoutineArgs = args;

  const uint_t res = wh_thread_create( &mThread, Thread::ThreadWrapperRoutine, this);
  if (res != WOP_OK)
  {
    assert(mEnded > 0);

    throw ThreadException(_EXTRA(errno), "Failed to create a thread.");
  }

  return true;
}


Thread::~Thread()
{
  WaitToEnd(false);

  assert(mNeedsClean == false);

  //If you did not have thrown the exception until now,
  //do not do it now from destructor.
  delete mException;
}


void
Thread::WaitToEnd(const bool throwPending)
{
  while (wh_atomic_fetch_inc32( &mEnded) != 0)
  {
    wh_yield();
    LockRAII < Lock > _l(mRoutineExecutionLock); //Avoid spin locks!
    wh_atomic_fetch_dec32 (&mEnded);
  }

  if (mNeedsClean)
  {
    wh_thread_free (mThread);
    mNeedsClean = false;

    if (throwPending)
      ThrowPendingException();
  }

  wh_atomic_fetch_dec32(&mEnded);
}


void
Thread::ThrowPendingException()
{
  assert(mEnded > 0);

  if (HasExceptionPending() == false)
    return;

  if (mUnkExceptSignaled)
  {
    DiscardException();

    wh_atomic_fetch_dec32 (&mEnded);

    throw ThreadException(_EXTRA(WOP_UNKNOW));
  }

  if (mException != nullptr)
  {
    Exception* clone = mException->Clone();
    DiscardException();

    wh_atomic_fetch_dec32 (&mEnded);

    throw clone;
  }
}


void
Thread::ThreadWrapperRoutine(void* const args)
{
  Thread* const th = _RC(Thread*, args);

  assert(th->mEnded > 0);

  th->mNeedsClean = true;

  LockRAII<Lock> _l(th->mRoutineExecutionLock);
  try
  {
      th->mRoutine(th->mRoutineArgs);
  }
  catch(Exception &e)
  {
    if (th->mIgnoreExceptions == false)
      th->mException = e.Clone();
  }
  catch(Exception* pE)
  {
    if (th->mIgnoreExceptions == false)
      th->mException = pE;
  }
  catch(...)
  {
    if (th->mIgnoreExceptions == false)
      th->mUnkExceptSignaled = true;
  }

  wh_atomic_fetch_dec32(&th->mEnded);
}


} //namespace whais

