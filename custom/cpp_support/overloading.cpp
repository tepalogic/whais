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

//Undefine 'new' because we need to implement the operators
#undef new

void*
operator new (size_t size, const D_CHAR *file, D_UINT line)
{
#ifndef ENABLE_MEMORY_TRACE
  (void)file;
  (void)line;
  void *ptr = custom_mem_alloc (size);
#else
  void *ptr = custom_trace_mem_alloc (size, file, line);
#endif

  if (ptr == NULL)
    throw std::bad_alloc ();
  return ptr;
}

void*
operator new [] (size_t size, const D_CHAR *file, D_UINT line)
{
#ifndef ENABLE_MEMORY_TRACE
  (void)file;
  (void)line;
  void *ptr = custom_mem_alloc (size);
#else
  void *ptr = custom_trace_mem_alloc (size, file, line);
#endif

  if (ptr == NULL)
    throw std::bad_alloc ();
  return ptr;
}

void*
operator new (size_t size)
{
#ifndef ENABLE_MEMORY_TRACE
  void *ptr = custom_mem_alloc (size);
#else
  void *ptr = custom_trace_mem_alloc (size, NULL, 0);
#endif

  if (ptr == NULL)
    throw std::bad_alloc ();
  return ptr;
}

void*
operator new [] (size_t size)
{
#ifndef ENABLE_MEMORY_TRACE
  void *ptr = custom_mem_alloc (size);
#else
  void *ptr = custom_trace_mem_alloc (size, NULL, 0);
#endif

  if (ptr == NULL)
    throw std::bad_alloc ();
  return ptr;
}


void
operator delete (void *ptr)
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
  custom_trace_mem_free(ptr, NULL, 0);
#endif
}

void
operator delete (void *ptr, const D_CHAR*, D_UINT)
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
  custom_trace_mem_free(ptr, NULL, 0);
#endif
}

void
operator delete [] (void *ptr)
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
  custom_trace_mem_free(ptr, NULL, 0);
#endif
}

void
operator delete[] (void *ptr, const D_CHAR*, D_UINT )
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
    custom_trace_mem_free(ptr, NULL, 0);
#endif
}

