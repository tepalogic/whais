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

#include <stdlib.h>

#include "whisper.h"

void *
custom_trace_mem_alloc (size_t size, const char* pFile, D_UINT line)
{
  /* avoid unused variable warnings */
  pFile = 0;
  line  = 0;

  return custom_mem_alloc (size);
}

void*
custom_trace_mem_realloc (void*       oldPtr,
                          size_t      new_size,
                          const char* pFile,
                          D_UINT      line)
{
  /* avoid unused variable warnings */
  pFile = 0;
  line  = 0;

  return custom_mem_realloc (oldPtr, new_size);
}

void
custom_trace_mem_free (void *ptr, const char *file, D_UINT line)
{
  /* avoid unused variable warnings */
  file = 0;
  line = 0;

  custom_mem_free (ptr);
}

void*
custom_mem_alloc (size_t size)
{
  return malloc (size);
}

void*
custom_mem_realloc (void* oldPtr, size_t newSize)
{
  return realloc (oldPtr, newSize);
}

void
custom_mem_free (void* ptr)
{
  free (ptr);
}
