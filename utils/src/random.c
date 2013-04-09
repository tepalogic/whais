/******************************************************************************
UTILS - Common routines used trough WHISPER project
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

#include "random.h"

#define XORSHIFT_DEFAULT_SEED 858495253484946541

static uint64_t _seed    = XORSHIFT_DEFAULT_SEED;
static uint64_t _current = XORSHIFT_DEFAULT_SEED;

uint64_t
w_rnd_get_seed ()
{
  assert (_seed != 0);
  return _seed;
}

void
w_rnd_set_seed (uint64_t seed)
{
  if (seed == 0)
    _seed = XORSHIFT_DEFAULT_SEED;

  _seed = seed;
}

uint64_t
w_rnd ()
{
  uint64_t current = _current;

  current ^= current << 1;
  current ^= current >> 7;
  current ^= current << 9;

  assert (current != 0);
  _current = current;

  return current - 1;
}

