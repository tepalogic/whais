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

#include "whisper.h"
#include "whisper_thread.h"

/* Define to choose the POSIX variant when compile under Linux. */
#define __USE_BSD       1

#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>

void
wh_sync_init (WH_SYNC* pSync)
{
  const D_UINT result = pthread_mutex_init (pSync, NULL);
  assert (result == 0);
}

void
wh_sync_destroy (WH_SYNC* pSync)
{
  const D_UINT result = pthread_mutex_destroy (pSync);
  assert (result == 0);
}

void
wh_sync_enter (WH_SYNC* pSync)
{
  const D_UINT result = pthread_mutex_lock (pSync);
  assert (result == 0);
}

void
wh_sync_leave (WH_SYNC* pSync)
{
  const int result = pthread_mutex_unlock (pSync);
  assert (result == 0);
}

D_INT
wh_thread_create (WH_THREAD*       pThread,
                 WH_THREAD_ROUTINE routine,
                 void*             args)
{
  return pthread_create (pThread, NULL, (void* (*)(void*))routine, args);
}

D_INT
wh_thread_join (WH_THREAD thread)
{
  return pthread_join (thread, NULL);
}

void
wh_yield ()
{
  sched_yield ();
}

void
wh_sleep (D_UINT millisecs)
{
  usleep (millisecs * 1000);
}
