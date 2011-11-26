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
#include "whisper_sync.h"

#include <assert.h>
#include <errno.h>

void
wh_init_sync (WH_SYNC *hnd)
{
  const D_UINT result = pthread_mutex_init (hnd, NULL);
  assert (result == 0);
}

void
wh_destroy_sync (WH_SYNC *hnd)
{
  const D_UINT result = pthread_mutex_destroy (hnd);
  assert (result == 0);
}

void
wh_enter_sync (WH_SYNC *hnd)
{
  const D_UINT result = pthread_mutex_lock (hnd);
  assert (result == 0);
}

void
wh_leave_sync (WH_SYNC *hnd)
{
  const int result = pthread_mutex_unlock (hnd);
  assert (result == 0);
}
