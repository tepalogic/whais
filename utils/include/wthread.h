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

#ifndef WTHREAD_H_
#define WTHREAD_H_

#include <assert.h>

#include "whisper.h"

namespace whisper {

class EXCEP_SHL Lock
{
public:
  Lock();
  ~Lock();

  void Acquire ();
  void Release ();

private:
  Lock (const Lock&);
  Lock& operator= (const Lock&);

  WH_LOCK mLock;
};


class LockRAII
{
public:
  explicit LockRAII (Lock &lock) :
    mLock (lock),
    mIsAcquireed (true)
  {
    mLock.Acquire ();
  }

  ~LockRAII ()
  {
    if (mIsAcquireed)
      mLock.Release ();
  }

  void Acquire ()
  {
    mLock.Acquire ();
    mIsAcquireed = true;
  }

  void Release ()
  {
    mIsAcquireed = false;
    mLock.Release ();
  }

private:
  Lock&       mLock;
  bool        mIsAcquireed;
};


class EXCEP_SHL LockException : public Exception
{
public:
  LockException (const char*   message,
                 const char*   file,
                 uint32_t      line,
                 uint32_t      extra);

  virtual Exception* Clone () const;

  virtual EXCEPTION_TYPE Type () const;

  virtual const char* Description () const;
};



class EXCEP_SHL Thread
{
public:
  Thread ();
  ~Thread ();

  void Run (WH_THREAD_ROUTINE routine, void* const args);

  void WaitToEnd (const bool throwPending = true);

  void ThrowPendingException ();

  void IgnoreExceptions (bool ignore)
  {
    mIgnoreExceptions = ignore;
  }

  void DiscardException ()
  {
    mUnkExceptSignaled = false;

    delete mException;
    mException = NULL;
  }

  bool IsEnded () const
  {
    return mEnded;
  }

  bool HasExceptionPending ()
  {
    return (mUnkExceptSignaled || (mException != NULL));
  }

private:
  static void ThreadWrapperRoutine (void* const);

  Thread (const Thread&);
  Thread& operator= (const Thread&);

  WH_THREAD_ROUTINE       mRoutine;
  void*                   mRoutineArgs;
  Exception*              mException;
  WH_THREAD               mThread;
  Lock                    mLock;
  bool                    mUnkExceptSignaled;
  bool                    mIgnoreExceptions;
  bool                    mEnded;
  bool                    mNeedsClean;
};



class EXCEP_SHL ThreadException : public Exception
{
public:
  ThreadException (const char*   message,
                   const char*   file,
                   uint32_t      line,
                   uint32_t      extra);


  virtual Exception* Clone () const;

  virtual EXCEPTION_TYPE Type () const;

  virtual const char* Description () const;
};


} //namespace whisper

#endif /* WTHREAD_H_ */

