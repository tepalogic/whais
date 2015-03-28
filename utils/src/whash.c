/******************************************************************************
UTILS - Common routines used trough WHAIS project
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

#include "whash.h"

static const uint64_t FNV_OFFSET    = 0xCBF29CE484222325;
static const uint64_t FNV_PRIME     = 0x100000001b3;
static const uint8_t  FNV_ZERO_RPL  = 0x77;


/* Based on FNV-1a hash algorithm. */
uint64_t
wh_hash (const uint8_t* key, const uint_t keySize)
{
  uint64_t result = FNV_OFFSET;
  uint_t   i;

  for (i = 0; i < keySize; ++i)
    {
      result ^= key[i] ? key[i] : 0x77;
      result *= FNV_PRIME;
    }

  return result;
}

