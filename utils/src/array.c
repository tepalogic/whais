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

#include <string.h>

#include "whisper.h"

#include "array.h"

struct UArray*
init_array_ex (struct UArray* pOutArray,
               size_t         itemSize,
               uint_t         increment,
               uint_t         alignment)
{

  if ((alignment > sizeof (uint64_t)) || (alignment == 0))
    {
      /* We are not made for such things.
       * We need to make sure the memory is proper allocated */
      return NULL;
    }

  if (pOutArray != NULL)
    {
      memset (pOutArray, 0, sizeof (pOutArray[0]));
      pOutArray->incrementCount = increment;
      pOutArray->userItemSize   = itemSize;

      /* round the item size for proper alignment */
      itemSize += (--alignment);
      itemSize &= ~(alignment);

      pOutArray->realItemSize   = itemSize;
    }

  return pOutArray;
}

static bool_t
increment_array (struct UArray* pArray)
{
  int8_t **temp = mem_realloc (pArray->arraysList,
                               (pArray->arraysCount + 1) * sizeof (temp[0]));
  if (temp == NULL)
    return FALSE;

  pArray->arraysList        = temp;
  temp[pArray->arraysCount] = mem_alloc (pArray->realItemSize * pArray->incrementCount);

  if (temp[pArray->arraysCount] == NULL)
    return FALSE;

  pArray->arraysCount++;
  pArray->itemsReserved += pArray->incrementCount;

  return TRUE;
}

void*
add_item (struct UArray* pArray, const void *pData)
{
  void *dest_data;

  if ((pArray->itemsReserved <= pArray->itemsCount) &&
      (increment_array (pArray) == FALSE))
    {
      /* not enough memory space */
      return NULL;
    }

  pArray->itemsCount++;
  dest_data = get_item (pArray, pArray->itemsCount - 1);

  memcpy (dest_data, pData, pArray->userItemSize);
  return dest_data;
}

void*
get_item (const struct UArray* pArray, uint_t index)
{
  uint_t seg, offset;

  if (index >= pArray->itemsCount)
    {
      /* the array is not so big */
      return NULL;
    }

  seg    = index / pArray->incrementCount;
  offset = index % pArray->incrementCount;
  offset *= pArray->realItemSize;

  return &(pArray->arraysList[seg][offset]);
}

uint_t
get_array_count (const struct UArray* pArray)
{
  return pArray->itemsCount;
}

void
set_array_count (struct UArray* pArray, uint_t newCount)
{
  if (newCount < pArray->itemsCount)
    {
      uint_t startSeg, iterator;

      pArray->itemsCount = newCount;
      startSeg           = newCount / pArray->incrementCount;

      if (newCount % pArray->incrementCount != 0)
        startSeg++;

      pArray->itemsReserved = startSeg * pArray->incrementCount;

      for (iterator = startSeg; iterator < pArray->arraysCount; iterator++)
        mem_free (pArray->arraysList[startSeg]);

      pArray->arraysCount = startSeg;
    }
  /* else: One should increase it when elements are added */
}

void
destroy_array (struct UArray* pArray)
{
  uint_t count = 0;

  for (count = 0; count < pArray->arraysCount; count++)
    mem_free (pArray->arraysList[count]);

  if (pArray->arraysList != NULL)
    mem_free (pArray->arraysList);
}
