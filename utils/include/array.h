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

#ifndef UARRAY_H
#define UARRAY_H

#include "whisper.h"

/*
 * UArray - stores an array of arbitrary elements. The goal of using this
 * structure is to optimise the memory access/usage.
 */
struct UArray
{
  size_t realItemSize;
  size_t userItemSize;
  D_UINT itemsCount;
  D_UINT itemsReserved;

  /* Array of pointers to the array's elements */
  D_INT8 **arraysList;
  D_UINT   incrementCount;
  D_UINT   arraysCount;
  D_UINT   arraysUsed;
};

#define ARRAY_INCRMENT_SIZE     512	/* in bytes here */
#define init_array(a, x) init_array_ex((a),\
                                       (x),\
                                       (ARRAY_INCRMENT_SIZE + (x)+1)/(x),\
                                       sizeof(D_UINTMAX))

struct UArray*
init_array_ex (struct UArray* pArray,
               size_t         item_size,
               D_UINT         increment,
               D_UINT         alignmentr);

void*
add_item (struct UArray* pArray, const void* data);

void *
get_item (const struct UArray* pArray, D_UINT item);

D_UINT get_array_count (const struct UArray* pArray);

void set_array_count (struct UArray* pArray, D_UINT new_count);

void destroy_array (struct UArray* pArray);

#endif /* UARRAY_H */
