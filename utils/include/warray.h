/******************************************************************************
UTILS - Common routines used trough WHISPER project
Copyright (C) 2009  Iulian Popa

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

#ifndef WARRAY_H
#define WARRAY_H

#include "whisper.h"



/*
 * WArray - a generic way to manipulate array of elements.
 */
struct WArray
{
  size_t   realItemSize;
  size_t   userItemSize;
  uint_t   itemsCount;
  uint_t   itemsReserved;

  /* Array of pointers to the array's elements */
  int8_t** arraysList;
  uint_t   incrementCount;
  uint_t   arraysCount;
  uint_t   arraysUsed;
};



/* The size of allocated memory chunk when the array has to be extended. */
#define ARRAY_INCRMENT_SIZE     512
#define wh_array_init(a, x) wh_array_init_ex ( \
                                       (a),\
                                       (x),\
                                       (ARRAY_INCRMENT_SIZE + (x)+1)/(x),\
                                       sizeof(uint64_t)\
                                             )

struct  WArray*
wh_array_init_ex (struct WArray* const outArray,
                  size_t               itemSize,
                  uint_t               increment,
                  uint_t               alignment);


void*
wh_array_add (struct WArray* const array, const void* const data);


void*
wh_array_get (const struct WArray* const array, const uint_t index);


uint_t
wh_array_count (const struct WArray* const array);


void
wh_array_resize (struct WArray* const array, const uint_t count);


void
wh_array_clean (struct WArray* const array);

#endif /* WARRAY_H */
