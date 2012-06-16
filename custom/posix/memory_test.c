/******************************************************************************
WHISPERC - A compiler for whisper programs
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


#include "assert.h"
#include "stdlib.h"
#include "string.h"

#include "whisper.h"

#include "test/test_fmw.h"

#define CONTROL_BYTE    0xAC    /* all correct */

extern W_ALLOCATED_MEMORY* gpListHead;

void *
custom_trace_mem_alloc (size_t size, const char *file, D_UINT line)
{
  W_ALLOCATED_MEMORY *result = (W_ALLOCATED_MEMORY *)custom_mem_alloc (size);

  if (result != NULL)
    --result;
  else
    return result;

  result->line = line;
  result->file = file;

  if (gpListHead == NULL)
    {
      result->prev = NULL;
      result->next = NULL;
    }
  else
    {
      result->next = gpListHead;
      result->prev = NULL;

      assert (gpListHead->prev == NULL);
      gpListHead->prev = result;
    }

  gpListHead = result;

  return result + 1;
}

void *
custom_trace_mem_realloc (void *old_ptr,
                          size_t new_size, const char *file, D_UINT line)
{
  W_ALLOCATED_MEMORY *const result = custom_trace_mem_alloc (new_size, file, line);
  W_ALLOCATED_MEMORY *const pOldMem = (W_ALLOCATED_MEMORY *)old_ptr;
  D_UINT64 size;

  if ((result == NULL) || (old_ptr == NULL))
    return result;


  size = pOldMem[-1].size;

  assert (size >= sizeof (W_ALLOCATED_MEMORY) + 1);
  size -= (sizeof (W_ALLOCATED_MEMORY) + 1);

  if (size > new_size)
    size = new_size;

  memcpy (result, pOldMem, size);

  custom_trace_mem_free (old_ptr, file, line);

  return result;

}

void
custom_trace_mem_free (void *ptr, const char *file, D_UINT line)
{
  W_ALLOCATED_MEMORY *pMem = (W_ALLOCATED_MEMORY *)ptr;

  pMem--;
  if (pMem == gpListHead)
    {
      gpListHead = gpListHead->next;
      if (gpListHead != NULL)
        gpListHead->prev = NULL;
    }
  else
    {
      if (pMem->next != NULL)
        pMem->next->prev = pMem->prev;

      assert (pMem->prev != NULL);
      pMem->prev->next = pMem->next;
    }

  custom_mem_free (ptr);
}

void *
custom_mem_alloc (size_t size)
{
  W_ALLOCATED_MEMORY *result = NULL;

  size += sizeof (W_ALLOCATED_MEMORY);
  size++;                       /* control byte */
  if (test_get_mem_max () &&
      ((test_get_mem_used () + size) > test_get_mem_max ()))
    {
      /* do not allow allocation */
      return NULL;
    }

  result = (W_ALLOCATED_MEMORY *) malloc (size);
  if (result)
    {
      /* store the size */
      result->size = size;
      test_add_used_mem (size);
      ((D_UINT8* )result)[size - 1] = CONTROL_BYTE;
      result++;
    }

  return (void *)result;
}

void *
custom_mem_realloc (void *old_ptr, size_t new_size)
{
  W_ALLOCATED_MEMORY *result = (W_ALLOCATED_MEMORY *)custom_mem_alloc (new_size);
  W_ALLOCATED_MEMORY *old_mem = (W_ALLOCATED_MEMORY *)old_ptr;

  if ((result != NULL) && (old_ptr != NULL))
    {
      size_t size = old_mem[-1].size;

      assert (size >= (sizeof (W_ALLOCATED_MEMORY) + 1));

      size -= (sizeof (W_ALLOCATED_MEMORY) + 1);
      if (new_size > size)
        size = new_size;

      assert (result[-1].size == (new_size + 1 + sizeof (W_ALLOCATED_MEMORY)));

      memcpy (result, old_mem, size);

      custom_mem_free (old_mem);
    }

  return result;
}

void
custom_mem_free (void *ptr)
{
  size_t size = 0;
  W_ALLOCATED_MEMORY *real_ptr = (W_ALLOCATED_MEMORY *) ptr;

  real_ptr--;
  size = real_ptr->size;

  if (((D_UINT8* )real_ptr)[size - 1] != CONTROL_BYTE)
    abort ();                   /* blow it up */

  free (real_ptr);
  test_free_used_mem (size);
}
