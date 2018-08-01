/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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

#include "whais.h"
#include "whais_thread.h"


uint_t
wh_lock_init(WH_LOCK* const lock)
{
  InitializeCriticalSection(lock);

  return WOP_OK;
}


uint_t
wh_lock_destroy(WH_LOCK* const lock)
{
  DeleteCriticalSection(lock);

  return WOP_OK;
}


uint_t
wh_lock_acquire(WH_LOCK* const lock)
{
  EnterCriticalSection(lock);

  return WOP_OK;
}


uint_t
wh_lock_try_acquire(WH_LOCK* const lock,
                     bool_t* const  outAcquired)
{
  *outAcquired = (TryEnterCriticalSection(lock) != 0);
  return WOP_OK;
}



uint_t
wh_lock_release(WH_LOCK* const lock)
{
  LeaveCriticalSection(lock);

  return WOP_OK;
}


uint_t
wh_thread_create(WH_THREAD* const              outThread,
                  const WH_THREAD_ROUTINE       routine,
                  void* const                   args)
{
  *outThread = CreateThread(NULL,
                           0,
                           (LPTHREAD_START_ROUTINE)routine,
                           args,
                           0,
                           NULL);
  if (*outThread == NULL)
    {
      const uint_t result = GetLastError();

      return(result == WOP_OK) ? WOP_UNKNOW : result;
    }

  return WOP_OK;
}


uint_t
wh_thread_free(WH_THREAD thread)
{
  //Give a chance of the thread to finish!
  WaitForSingleObject(thread, INFINITE);
  CloseHandle(thread);

  return WOP_OK;
}


void
wh_yield()
{
  Sleep(0);
}


void
wh_sleep(const uint_t millisecs)
{
  if (millisecs == 0)
    return;

  Sleep(millisecs);
}


int16_t
wh_atomic_fetch_inc16(volatile int16_t* const value)
{
  return _InterlockedIncrement16(value) - 1;
}

int16_t
wh_atomic_fetch_dec16(volatile int16_t* const value)
{
  return _InterlockedDecrement16(value) + 1;
}

int32_t
wh_atomic_fetch_inc32(volatile int32_t* const value)
{
  return InterlockedIncrement(value) - 1;
}

int32_t
wh_atomic_fetch_dec32(volatile int32_t* const value)
{
  return InterlockedDecrement(value) + 1;
}

int64_t
wh_atomic_fetch_inc64(volatile int64_t* const value)
{
  return InterlockedIncrement64(value) - 1;
}

int64_t
wh_atomic_fetch_dec64(volatile int64_t* const value)
{
  return InterlockedDecrement64(value) + 1;
}
