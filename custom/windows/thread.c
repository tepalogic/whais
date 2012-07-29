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

#include "whisper.h"
#include "whisper_thread.h"

void
wh_sync_init (WH_SYNC* pSync)
{
  InitializeCriticalSection (pSync);
}

void
wh_sync_destroy (WH_SYNC* pSync)
{
  DeleteCriticalSection (pSync);
}

void
wh_sync_enter (WH_SYNC* pSync)
{
  EnterCriticalSection (pSync);
}

void
wh_sync_leave (WH_SYNC* pSync)
{
  LeaveCriticalSection (pSync);
}

D_INT
wh_thread_create (WH_THREAD* pThread, WH_THREAD_ROUTINE routine, void* args)
{
  *pThread = CreateThread (NULL,
                           0,
                           (LPTHREAD_START_ROUTINE)routine,
                           args,
                           0,
                           NULL);
  if (*pThread == NULL)
    {
      D_INT result = GetLastError ();
      if (result == WOP_OK)
        result = WOP_UNKNOW;

      return result;
    }

  return WOP_OK;
}

D_INT
wh_thread_join (WH_THREAD thread)
{
  D_INT result = WOP_OK;

  if (WaitForSingleObject (thread, INFINITE) != WAIT_OBJECT_0)
    {
      result = GetLastError ();
      if (result == WOP_OK)
        result = WOP_UNKNOW;
    }

  CloseHandle (thread);

  return result;
}

void
wh_yield ()
{
  Sleep (0);
}

void
wh_sleep (D_UINT millisecs)
{
  if (millisecs == 0)
    return;

  Sleep (millisecs);
}
