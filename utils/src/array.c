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

struct UArray *
init_array_ex (struct UArray *array,
	       size_t item_size, D_UINT increment, D_UINT alignment)
{

  if ((alignment > sizeof (D_UINTMAX)) || (alignment == 0))
    {
      /* We are not made for such things.
       * We need to make sure the memory is proper allocated */
      return NULL;
    }

  if (array != NULL)
    {
      memset (array, 0, sizeof (array[0]));
      array->increment_count = increment;
      array->user_item_size = item_size;

      /* round the item size for proper alignment */
      item_size += (--alignment);
      item_size &= ~(alignment);
      array->item_size = item_size;
    }

  return array;
}

static D_BOOL
increment_array (struct UArray *array)
{
  D_INT8 **temp = mem_realloc (array->arrays,
			       (array->arrays_count + 1) * sizeof (temp[0]));
  if (temp == NULL)
    {
      return FALSE;
    }
  array->arrays = temp;
  temp[array->arrays_count] =
    mem_alloc (array->item_size * array->increment_count);
  if (temp[array->arrays_count] == NULL)
    {
      return FALSE;
    }
  array->arrays_count++;
  array->reserved_items += array->increment_count;
  return TRUE;
}

void *
add_item (struct UArray *array, const void *data)
{
  void *dest_data;

  if ((array->reserved_items <= array->stored_items) &&
      (increment_array (array) == FALSE))
    {
      /* not enough memory space */
      return NULL;
    }
  array->stored_items++;
  dest_data = get_item (array, array->stored_items - 1);

  memcpy (dest_data, data, array->user_item_size);
  return dest_data;
}

void *
get_item (const struct UArray *array, D_UINT index)
{
  D_UINT seg, offset;

  if (index >= array->stored_items)
    {
      /* the array is not so big */
      return NULL;
    }

  seg = index / array->increment_count;
  offset = index % array->increment_count;
  offset *= array->item_size;

  return &(array->arrays[seg][offset]);
}

D_UINT
get_array_count (const struct UArray * array)
{
  return array->stored_items;
}

void
set_array_count (struct UArray *array, D_UINT new_count)
{
  if (new_count < array->stored_items)
    {
      D_UINT start_seg, it;
      array->stored_items = new_count;
      start_seg = new_count / array->increment_count;
      if (new_count % array->increment_count != 0)
	{
	  start_seg++;
	}
      array->reserved_items = start_seg * array->increment_count;
      for (it = start_seg; it < array->arrays_count; it++)
	{
	  mem_free (array->arrays[start_seg]);
	}
      array->arrays_count = start_seg;
    }
  /* else: One should increase it when elements are added */
}

void
destroy_array (struct UArray *array)
{
  D_UINT count = 0;

  for (count = 0; count < array->arrays_count; count++)
    {
      mem_free (array->arrays[count]);
    }
  if (array->arrays != NULL)
    {
      mem_free (array->arrays);
    }
}
