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

#include <string.h>

#include "warray.h"

struct WArray*
wh_array_init_ex( struct WArray* const outArray,
                  size_t               itemSize,
                  uint_t               increment,
                  uint_t               alignment)
{
  /* Check if the reuired alignment is supported. */
  if ((alignment > sizeof( uint64_t)) || (alignment == 0))
    return NULL;

  if (outArray != NULL)
    {
      memset( outArray, 0, sizeof( outArray[0]));
      outArray->incrementCount = increment;
      outArray->userItemSize   = itemSize;

      /* round the item size for proper alignment */
      itemSize += (--alignment);
      itemSize &= ~(alignment);

      outArray->realItemSize = itemSize;
    }

  return outArray;
}


static bool_t
increment_array( struct WArray* const array)
{
  int8_t **temp = mem_realloc( array->arraysList,
                               (array->arraysCount + 1) * sizeof( temp[0]));
  if (temp == NULL)
    return FALSE;

  array->arraysList        = temp;
  temp[array->arraysCount] = mem_alloc( array->realItemSize *
                                          array->incrementCount);
  if (temp[array->arraysCount] == NULL)
    return FALSE;

  array->arraysCount++;
  array->itemsReserved += array->incrementCount;

  return TRUE;
}


void*
wh_array_add( struct WArray* const array, const void *data)
{
  void *dest_data;

  if ((array->itemsReserved <= array->itemsCount) &&
      ( ! increment_array( array)))
    {
      /* Failed to allocated memory. */
      return NULL;
    }

  array->itemsCount++;
  dest_data = wh_array_get( array, array->itemsCount - 1);

  memcpy( dest_data, data, array->userItemSize);
  return dest_data;
}


void*
wh_array_get( const struct WArray* const array, const uint_t index)
{
  uint_t seg, offset;

  if (index >= array->itemsCount)
    return NULL;

  seg     = index / array->incrementCount;
  offset  = index % array->incrementCount;
  offset *= array->realItemSize;

  return &(array->arraysList[seg][offset]);
}


uint_t
wh_array_count( const struct WArray* const array)
{
  return array->itemsCount;
}


void
wh_array_resize( struct WArray* const array, const uint_t count)
{
  if (count < array->itemsCount)
    {
      uint_t startSeg, iterator;

      array->itemsCount = count;
      startSeg          = count / array->incrementCount;

      if (count % array->incrementCount != 0)
        startSeg++;

      array->itemsReserved = startSeg * array->incrementCount;

      for (iterator = startSeg; iterator < array->arraysCount; iterator++)
        mem_free( array->arraysList[startSeg]);

      array->arraysCount = startSeg;
    }
  /* else: One should increase it when elements are added */
}


void
wh_array_clean( struct WArray* const array)
{
  uint_t count = 0;

  for (count = 0; count < array->arraysCount; count++)
    mem_free( array->arraysList[count]);

  if (array->arraysList != NULL)
    mem_free( array->arraysList);
}

