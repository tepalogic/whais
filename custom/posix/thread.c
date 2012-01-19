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

#include <assert.h>
#include <errno.h>

void
wh_sync_init (WH_SYNC *hnd)
{
  const D_UINT result = pthread_mutex_init (hnd, NULL);
  assert (result == 0);
}

void
wh_sync_destroy (WH_SYNC *hnd)
{
  const D_UINT result = pthread_mutex_destroy (hnd);
  assert (result == 0);
}

void
wh_sync_enter (WH_SYNC *hnd)
{
  const D_UINT result = pthread_mutex_lock (hnd);
  assert (result == 0);
}

void
wh_sync_leave (WH_SYNC *hnd)
{
  const int result = pthread_mutex_unlock (hnd);
  assert (result == 0);
}

D_INT
wh_cond_value_init (WH_COND_VALUE* pCondValue)
{
  assert (pCondValue != NULL);
  return WOP_OK;
}

D_INT
wh_cond_value_wait (WH_COND_VALUE* pCondValue, WH_SYNC* pSync)
{
  return pthread_cond_wait (pCondValue, pSync);
}

D_INT
wh_cond_value_signal (WH_COND_VALUE* pCondValue)
{
  return pthread_cond_signal (pCondValue);
}

D_INT
wh_cond_value_broadcast (WH_COND_VALUE* pCondValue)
{
  return pthread_cond_broadcast (pCondValue);
}

D_INT
wh_cond_value_destroy (WH_COND_VALUE* pCondValue)
{
  assert (pCondValue != NULL);
  return WOP_OK;
}

D_INT
wh_thread_create (WH_THREAD* pThread, W_THREAD_ROUTINE routine, W_THREAD_ROUTINE_ARGS args)
{
  return pthread_create (pThread, NULL, routine, args);
}

D_INT
wh_thread_exit (W_THREAD_ROUTINE_STATUS status)
{
  pthread_exit (status);

  return WOP_OK;
}

D_INT
wh_thread_detach (WH_THREAD thread)
{
  return pthread_detach (thread);
}

D_INT
wh_thread_join (WH_THREAD thread, W_THREAD_ROUTINE_ARGS* pOutArgs)
{
  return pthread_join (thread, pOutArgs);
}
