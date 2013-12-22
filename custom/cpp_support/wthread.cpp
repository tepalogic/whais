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

  (void)result;
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

  (void)result;
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
    mStarted (true),
    mNeedsClean (false)
{
}


void
Thread::Run (WH_THREAD_ROUTINE routine, void* const args)
{
  //Wait for the the previous thread to be cleared.
  WaitToEnd ();

  assert (mEnded);
  assert (mNeedsClean == false);

  mEnded       = true;
  mStarted     = false;
  mRoutine     = routine;
  mRoutineArgs = args;

  const uint_t res = wh_thread_create (&mThread,
                                       Thread::ThreadWrapperRoutine,
                                       this);
  if (res != WOP_OK)
    {
      assert (mEnded);

      mStarted = true;
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
  //Give a chance for the thread it owns to execute. Make sure it had
  //acquired the lock (if it did not then spin), in case this method was
  //called to soon.
  while ( ! mStarted)
    wh_yield ();

  //Wait till the spawned thread releases the lock.
  LockRAII holder (mLock);

  assert (mEnded);

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
      Exception* clone = mException->Clone ();
      DiscardException ();

      throw clone;
    }
}


void
Thread::ThreadWrapperRoutine (void* const args)
{
  Thread* const th = _RC (Thread*, args);

  th->mEnded = false;
  th->mLock.Acquire ();
  th->mStarted = true; //Signal the we grabbed the lock

  try
  {
      th->mRoutine (th->mRoutineArgs);
  }
  catch (Exception &e)
  {
    if (th->mIgnoreExceptions == false)
      th->mException = e.Clone ();
  }
  catch (Exception* pE)
  {
    if (th->mIgnoreExceptions == false)
      th->mException = pE;
  }
  catch (...)
  {
    if (th->mIgnoreExceptions == false)
      th->mUnkExceptSignaled = true;
  }

  assert (th->mEnded == false);

  th->mEnded = true;
  th->mLock.Release ();
}


} //namespace whisper

