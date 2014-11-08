/******************************************************************************
WHAIS - An advanced database system
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

#include "whais.h"
#include "whais_thread.h"

/* Define to choose the POSIX variant when compile under Linux. */
#define __USE_BSD       1

#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>

uint_t
wh_lock_init( WH_LOCK* const lock)
{
  uint_t result;

  do
    result = pthread_mutex_init( lock, NULL);
  while( result == (uint_t)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}


uint_t
wh_lock_destroy( WH_LOCK* const lock)
{
  uint_t result;

  do
    result = pthread_mutex_destroy( lock);
  while( result == (uint_t)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}


uint_t
wh_lock_acquire( WH_LOCK* const lock)
{
  uint_t result;

  do
    result = pthread_mutex_lock( lock);
  while( result == (uint_t)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}


uint_t
wh_lock_release( WH_LOCK* const lock)
{
  uint_t result;

  do
    result = pthread_mutex_unlock( lock);
  while( result == (uint_t)EAGAIN);

  if (result == 0)
    return WOP_OK;

  return result;
}


uint_t
wh_thread_create( WH_THREAD*  const             outThread,
                  const WH_THREAD_ROUTINE       routine,
                  void* const                   args)
{
  uint_t result;

  do
    result = pthread_create( outThread, NULL, (void* (*)(void*))routine, args);
  while( result == (uint_t)EAGAIN);

  if (result == 0)
    result = pthread_detach( *outThread);

  return result;
}


uint_t
wh_thread_free( WH_THREAD thread)
{
  return WOP_OK;
}


void
wh_yield( )
{
  sched_yield( );
}


void
wh_sleep( const uint_t millisecs)
{
  usleep( millisecs * 1000);
}

