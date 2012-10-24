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

D_UINT
wh_sync_init (WH_SYNC* pSync)
{
  D_UINT result;

  do
    result = pthread_mutex_init (pSync, NULL);
  while (result == (D_UINT)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}

D_UINT
wh_sync_destroy (WH_SYNC* pSync)
{
  D_UINT result;

  do
    result = pthread_mutex_destroy (pSync);
  while (result == (D_UINT)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}

D_UINT
wh_sync_enter (WH_SYNC* pSync)
{
  D_UINT result;

  do
    result = pthread_mutex_lock (pSync);
  while (result == (D_UINT)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}

D_UINT
wh_sync_leave (WH_SYNC* pSync)
{
  D_UINT result;

  do
    result = pthread_mutex_unlock (pSync);
  while (result == (D_UINT)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}

D_UINT
wh_thread_create (WH_THREAD*       pThread,
                 WH_THREAD_ROUTINE routine,
                 void*             args)
{
  D_UINT result;

  do
    result = pthread_create (pThread, NULL, (void* (*)(void*))routine, args);
  while (result == (D_UINT)EAGAIN);

  if (result == 0)
    result = pthread_detach (*pThread);

  return result;
}

D_UINT
wh_thread_free (WH_THREAD thread)
{
  return WOP_OK;
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
