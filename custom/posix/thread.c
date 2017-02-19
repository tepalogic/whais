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

#include "whais.h"
#include "whais_thread.h"


#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>

uint_t
wh_lock_init(WH_LOCK* const lock)
{
#ifdef NDEBUG
  uint_t result = pthread_mutex_init(lock, NULL);
#else
  pthread_mutexattr_t attrs;

  uint_t result = pthread_mutexattr_init(&attrs);
  if (result != 0)
    return result;

  result = pthread_mutexattr_settype(&attrs, PTHREAD_MUTEX_ERRORCHECK);
  if (result != 0)
    return result;

  do
    result = pthread_mutex_init(lock, &attrs);
  while (result == EAGAIN);


  if (result == 0)
    return WOP_OK;
#endif /* NDEBUG */

  return result;
}


uint_t
wh_lock_destroy(WH_LOCK* const lock)
{
  uint_t result;

  do
    result = pthread_mutex_destroy(lock);
  while (result == EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}


uint_t
wh_lock_acquire(WH_LOCK* const lock)
{
  uint_t result;

  do
    result = pthread_mutex_lock(lock);
  while (result == EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}


uint_t
wh_lock_try_acquire(WH_LOCK* const lock,
                     bool_t* const  outAcquired)
{
  int result;

  do
    result = pthread_mutex_trylock(lock);
  while (result == EAGAIN);

  if (result == 0)
    {
      *outAcquired = TRUE;
      return WOP_OK;
    }
  else if (result == EBUSY)
    {
      *outAcquired = FALSE;
      return WOP_OK;
    }

  return result;
}


uint_t
wh_lock_release(WH_LOCK* const lock)
{
  uint_t result;

  do
    result = pthread_mutex_unlock(lock);
  while (result == EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}


uint_t
wh_thread_create(WH_THREAD*  const             outThread,
                  const WH_THREAD_ROUTINE       routine,
                  void* const                   args)
{
  uint_t result;

  do
    result = pthread_create(outThread, NULL, (void* (*)(void*))routine, args);
  while (result == (uint_t)EAGAIN);

  if (result == 0)
    result = pthread_detach(*outThread);

  return result;
}


uint_t
wh_thread_free(WH_THREAD thread)
{
  return WOP_OK;
}


void
wh_yield()
{
  sched_yield();
}


void
wh_sleep(const uint_t millisecs)
{
  usleep(millisecs * 1000);
}


#ifdef __GNUC__

int16_t
wh_atomic_fetch_inc16(volatile int16_t* const value)
{
  return __sync_fetch_and_add(value, (int16_t)1);
}

int16_t
wh_atomic_fetch_dec16(volatile int16_t* const value)
{
  return __sync_fetch_and_sub(value, (int16_t)1);
}

int32_t
wh_atomic_fetch_inc32(volatile int32_t* const value)
{
  return __sync_fetch_and_add(value, (int32_t)1);
}

int32_t
wh_atomic_fetch_dec32(volatile int32_t* const value)
{
  return __sync_fetch_and_sub(value, (int32_t)1);
}

int64_t
wh_atomic_fetch_inc64(volatile int64_t* const value)
{
  return __sync_fetch_and_add(value, (int64_t)1);
}

int64_t
wh_atomic_fetch_dec64(volatile int64_t* const value)
{
  return __sync_fetch_and_sub(value, (int64_t)1);
}


#if defined(ARCH_PPC)
/* These functions are required by GCC compiler for PPC target processor as
 * for these it does not support 8 bit atomic operations. */
int64_t
__sync_fetch_and_add_8(volatile int64_t* const value, int64_t v)
{
  static int32_t m;

  int64_t result;

  while (__sync_fetch_and_add(&m, 1) != 0)
    {
      __sync_fetch_and_sub(&m, 1);
      sched_yield();
    }

  result  = *value;
  *value += v;

  __sync_fetch_and_sub(&m, 1);

  return result;
}

int64_t
__sync_fetch_and_sub_8(volatile int64_t* const value, int64_t v)
{
  static int32_t m;

  int64_t result;

  while (__sync_fetch_and_add(&m, 1) != 0)
    {
      __sync_fetch_and_sub(&m, 1);
      sched_yield();
    }

  result  = *value;
  *value -= v;

  __sync_fetch_and_sub(&m, 1);

  return result;
}
#endif

#else

#error "You need to define the functions for atomic increment and decrement."

#endif

