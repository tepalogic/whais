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

class EXCEP_SHL WSynchronizer
{
public:
  WSynchronizer();
  ~WSynchronizer();

  void Enter();
  void Leave();

private:
  //Does not support any kind of copy or assignment!
  WSynchronizer(const WSynchronizer&);
  WSynchronizer& operator=(const WSynchronizer&);

  WH_SYNC m_Sync;
};

class EXCEP_SHL WSynchronizerRAII
{
public:
  explicit WSynchronizerRAII (WSynchronizer &rSync) :
    m_Sync(rSync),
    m_IsEntered (true)
  {
    m_Sync.Enter();
  }

  void Enter () { m_IsEntered = true; m_Sync.Enter (); }

  void Leave () { m_IsEntered = false; m_Sync.Leave(); }

  ~WSynchronizerRAII ()
  {
    if (m_IsEntered)
      m_Sync.Leave();
  }

private:
  WSynchronizer& m_Sync;
  bool           m_IsEntered;
};

class EXCEP_SHL WSynchException : public WException
{
public:
  WSynchException (const D_CHAR* message,
                   const D_CHAR* file,
                   D_UINT32      line,
                   D_UINT32      extra);
  virtual WException*     Clone () const;
  virtual EXPCEPTION_TYPE Type () const;
  virtual const D_CHAR*   Description () const;
};


class EXCEP_SHL WThread
{
public:
  WThread ();
  ~WThread ();

  void Run (WH_THREAD_ROUTINE routine, void* const args);
  void WaitToEnd (const bool throwPending = true);
  void ThrowPendingException ();

  void IgnoreExceptions (bool ignore)
  {
    m_IgnoreExceptions = ignore;
  }
  void DiscardException ()
  {
    m_UnkExceptSignaled = false;
    delete m_Exception;
    m_Exception = NULL;
  }
  bool IsEnded () const { return m_Ended; }
  bool HasExceptionPending ()
  {
    return ( m_UnkExceptSignaled || (m_Exception != NULL));
  }

private:
  static void ThreadWrapperRoutine (void* const);

  WH_THREAD_ROUTINE       m_Routine;
  void*                   m_RoutineArgs;
  WException*             m_Exception;
  WH_THREAD               m_ThreadHnd;
  bool                    m_UnkExceptSignaled;
  bool                    m_IgnoreExceptions;
  bool                    m_Ended;
  bool                    m_NeedsClean;

  WThread (const WThread&);
  WThread& operator= (const WThread&);
};


class EXCEP_SHL WThreadException : public WException
{
public:
  WThreadException (const D_CHAR* message,
                    const D_CHAR* file,
                    D_UINT32      line,
                    D_UINT32      extra);
  virtual WException*     Clone () const;
  virtual EXPCEPTION_TYPE Type () const;
  virtual const D_CHAR*   Description () const;
};


#endif /* WTHREAD_H_ */
