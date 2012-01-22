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


#include "whisper.h"

class WSynchronizer
{
public:
  WSynchronizer()
  {
    wh_sync_init(&mSync);
  }

  ~WSynchronizer()
  {
    wh_sync_destroy(&mSync);
  }

  void Enter() { wh_sync_enter(&mSync); }

  void Leave() { wh_sync_leave(&mSync); }

private:
  //Does not support any kind of copy or assignment!
  WSynchronizer(const WSynchronizer&);
  WSynchronizer& operator=(const WSynchronizer&);

  WH_SYNC mSync;
};

class WSynchronizerHolder
  {
public:
  explicit WSynchronizerHolder(WSynchronizer &rSync) :
    mSync(rSync),
    m_IsEntered (true)
  {
    mSync.Enter();
  }

  void Enter () { m_IsEntered = true; mSync.Enter (); }

  void Leave () { m_IsEntered = false; mSync.Leave(); }

  ~WSynchronizerHolder()
  {
    if (m_IsEntered)
      mSync.Leave();
  }

private:
  WSynchronizer &mSync;
  bool           m_IsEntered;
};

class WThread
{
public:

  WThread (WH_THREAD_ROUTINE routine, void* const args);
  ~WThread ();

  void Join ();
  void SignalPendingException ();
  void IgnoreExptionSignals (bool ignore) { m_IgnoreExceptions = ignore; }
  void DiscardException () { m_UnkExceptSignaled = false; delete m_Exception; m_Exception = NULL;}

  bool IsEnded () const { return m_Ended; }
  bool HasExceptionPending () { return ( m_UnkExceptSignaled || (m_Exception != NULL)); }


protected:
  static void ThreadWrapperRoutine (void *const);

  const WH_THREAD_ROUTINE m_Routine;
  void* const             m_RoutineArgs;
  WException*             m_Exception;
  WH_THREAD               m_ThreadHnd;
  bool                    m_UnkExceptSignaled;
  bool                    m_IgnoreExceptions;
  bool                    m_Ended;
  bool                    m_Joined;

private:
  friend void __internal_thread_routine (WThread *const, const WH_THREAD_ROUTINE, void *const);
  WThread (const WThread&);
  WThread& operator= (const WThread&);
};


class WThreadException : public WException
{
public:
  explicit WThreadException (const D_CHAR *message,
                             const D_CHAR *file,
                             D_UINT32      line,
                             D_UINT32      extra)  :
           WException (message, file, line, extra) {}
  virtual ~WThreadException () {};

  virtual WException*     Clone () { return new WThreadException (*this); }
  virtual EXPCEPTION_TYPE GetType () { return THREAD_EXCEPTION; }

  enum
  {
    UNKNOWN_EXCEPTION,
    FAILEDOP_EXCEPTION
  };
};


#endif /* WTHREAD_H_ */
