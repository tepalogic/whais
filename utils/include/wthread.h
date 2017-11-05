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

#ifndef WTHREAD_H_
#define WTHREAD_H_


#include <assert.h>

#include "whais.h"


namespace whais {


class CUSTOM_SHL Lock
{
public:
  Lock();
  ~Lock();

  void lock();
  bool try_lock();
  void unlock();

private:
  Lock(const Lock&);
  Lock& operator= (const Lock&);

  WH_LOCK mLock;
};

class CUSTOM_SHL SpinLock
{
public:
  SpinLock();

  void lock();
  bool try_lock();
  void unlock();

private:
  SpinLock(const SpinLock&);
  SpinLock& operator= (const SpinLock&);

  volatile int16_t mLock;
};


template<class T>
class LockGuard
{
public:
  explicit LockGuard( T &lock, const bool skipAcquire = false)
    : mLock(lock),
      mIsAcquireed(false)
  {
    if ( ! skipAcquire)
      this->lock();
  }

  ~LockGuard()
  {
    unlock();
  }

  void lock()
  {
    mLock.lock();
    mIsAcquireed = true;
  }

  bool try_lock()
  {
    mIsAcquireed = mLock.try_lock();
    return mIsAcquireed;
  }

  void unlock()
  {
    if (mIsAcquireed)
      {
        mLock.unlock();
        mIsAcquireed = false;
      }
  }

private:
  LockGuard(const LockGuard&);
  LockGuard& operator= (const LockGuard& );

  T&       mLock;
  bool     mIsAcquireed;
};


template<typename T>
class DoubleLockGuard
{
public:
  DoubleLockGuard(T& lock1, T& lock2, const bool skipAcquire = false)
    : mLock1(lock1),
      mLock2(lock2),
      mIsAcquireed1(false),
      mIsAcquireed2(false)
  {
    if ( ! skipAcquire)
      AcquireBoth();
  }

  ~DoubleLockGuard()
  {
    ReleaseBoth();
  }

  void AcquireBoth()
  {
    if (&mLock1 == &mLock2)
      {
        mLock1.lock();
        mIsAcquireed1 = true;
        return ;
      }

    while (true)
      {
        mLock1.lock();
        mIsAcquireed1 = true;

        if (mLock2.try_lock())
          {
            mIsAcquireed2 = true;
            return ;
          }

        mLock1.unlock();
        mIsAcquireed1 = false;

        wh_yield();
      }
  }

  void ReleaseBoth()
  {
    if (mIsAcquireed1)
      mLock1.unlock();

    if (mIsAcquireed2)
      mLock2.unlock();
  }

private:
  DoubleLockGuard(const DoubleLockGuard&);
  DoubleLockGuard& operator= (const DoubleLockGuard& );

  T&       mLock1;
  T&       mLock2;
  bool     mIsAcquireed1;
  bool     mIsAcquireed2;
};


template<typename T>
class TripleLockGuard
{
public:
  TripleLockGuard(T& lock1, T& lock2, T& lock3, const bool skipAcquire = false)
    : mLock1(lock1),
      mLock2(lock2),
      mLock3(lock3),
      mIsAcquireed1(false),
      mIsAcquireed2(false),
      mIsAcquireed3(false)
  {
    if ( ! skipAcquire)
      AcquireAll();
  }

  ~TripleLockGuard()
  {
    ReleaseAll();
  }

  void AcquireAll()
  {
    while (true)
      {
        mLock1.lock();
        mIsAcquireed1 = true;

        if ((&mLock2 == &mLock1)
            || ((mIsAcquireed2 = mLock2.try_lock()) == true))
        {
            if ((&mLock3 == &mLock2)
                || (&mLock3 == &mLock1)
                || ((mIsAcquireed3 = mLock3.try_lock()) == true))
              {
                return;
              }
        }

        ReleaseAll();
        wh_yield();
      }
  }

  void ReleaseAll()
  {
    if (mIsAcquireed1)
      mLock1.unlock();

    if (mIsAcquireed2)
      mLock2.unlock();

    if (mIsAcquireed3)
      mLock3.unlock();

    mIsAcquireed1 = mIsAcquireed2 = mIsAcquireed3 = false;
  }

private:
  TripleLockGuard(const TripleLockGuard&);
  TripleLockGuard& operator= (const TripleLockGuard& );

  T&       mLock1;
  T&       mLock2;
  T&       mLock3;
  bool     mIsAcquireed1;
  bool     mIsAcquireed2;
  bool     mIsAcquireed3;
};



class LockException : public Exception
{
public:
  LockException(const uint32_t    code,
                 const char*       file,
                 uint32_t          line,
                 const char*       fmtMsg = nullptr,
                 ...);

  virtual Exception* Clone() const override;

  virtual EXCEPTION_TYPE Type() const override;

  virtual const char* Description() const override;
};



class CUSTOM_SHL Thread
{
public:
  Thread();
  ~Thread();

  bool Run(WH_THREAD_ROUTINE routine,
            void* const       args,
            const bool        waitPrevEnd = false);

  void WaitToEnd(const bool throwPending = true);

  void ThrowPendingException();

  void IgnoreExceptions(bool ignore)
  {
    mIgnoreExceptions = ignore;
  }

  void DiscardException()
  {
    mUnkExceptSignaled = false;

    delete mException;
    mException = nullptr;
  }

  bool HasExceptionPending()
  {
    return(mUnkExceptSignaled || (mException != nullptr));
  }

private:
  static void ThreadWrapperRoutine(void* const);

  Thread(const Thread&);
  Thread& operator= (const Thread&);

  WH_THREAD_ROUTINE       mRoutine;
  void*                   mRoutineArgs;
  Exception*              mException;
  WH_THREAD               mThread;
  Lock                    mRoutineExecutionLock;
  int32_t                 mEnded;
  bool                    mUnkExceptSignaled;
  bool                    mIgnoreExceptions;
  bool                    mNeedsClean;
};



class CUSTOM_SHL ThreadException : public Exception
{
public:
  ThreadException(const uint32_t    code,
                   const char*       file,
                   uint32_t          line,
                   const char*       fmtMsg = nullptr,
                   ...);


  virtual Exception* Clone() const override;

  virtual EXCEPTION_TYPE Type() const override;

  virtual const char* Description() const override;
};


} //namespace whais

#endif /* WTHREAD_H_ */

