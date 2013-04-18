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

#ifndef WHISPER_THREAD_H_
#define WHISPER_THREAD_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*WH_THREAD_ROUTINE) (void*);

uint_t
wh_lock_init (WH_LOCK* pSync);

uint_t
wh_lock_destroy (WH_LOCK* pSync);

uint_t
wh_lock_acquire (WH_LOCK* pSync);

uint_t
wh_lock_release (WH_LOCK* pSync);

uint_t
wh_thread_create (WH_THREAD*        pThread,
                  WH_THREAD_ROUTINE routine,
                  void*             args);

uint_t
wh_thread_free (WH_THREAD thread);

void
wh_yield ();

void
wh_sleep (uint_t millisecs);

#ifdef __cplusplus
}
#endif

#endif /* WHISPER_THREAD_H_ */
