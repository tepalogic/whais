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

#ifndef WHISPER_MEMORY_H
#define WHISPER_MEMORY_H

#ifdef __cplusplus

//These headers are include here because we need the standard
//STL functionality before we override the null operator.
#include <string>
#include <vector>
#include <map>

#include <new>
#include <memory>


void* operator new (size_t size, const D_CHAR *file, D_UINT line);
void* operator new (size_t size);
void  operator delete (void *ptr);
void  operator delete (void *ptr, const D_CHAR *, D_UINT);
void* operator new [] (size_t size, const D_CHAR *file, D_UINT line);
void* operator new [] (size_t size);
void  operator delete [] (void *ptr);
void  operator delete [] (void *ptr, const D_CHAR *, D_UINT);

template <class T> static inline void
_placement_new (void* place, const T& value)
{
  new (place) T (value);
}

template <class T> static inline void
_placement_new (void* place, T& value)
{
  new (place) T (value);
}

#ifdef ENABLE_MEMORY_TRACE
  #define new new(__FILE__, __LINE__)
#endif

extern "C"
{
#endif

/*
 * The memory could be allocated/freed in different ways
 * depending on the environment you use.
 * The following functions are wrappers for the real functions
 * which are allocating/freeing memory and must be implemented
 * for each environment.
 */

#ifndef ENABLE_MEMORY_TRACE

#define mem_alloc(x)      custom_mem_alloc((x))
#define mem_realloc(x, y) custom_mem_realloc((x), (y))
#define mem_free(x)       custom_mem_free((x))

#else

#define mem_alloc(x) custom_trace_mem_alloc((x), __FILE__, __LINE__)
#define mem_realloc(x, y) custom_trace_mem_realloc((x), (y), __FILE__, __LINE__)
#define mem_free(x) custom_trace_mem_free((x), __FILE__, __LINE__)

#endif				/* ENABLE_MEMORY_TRACE */

void *custom_trace_mem_alloc (size_t size, const char *file, D_UINT line);

void *custom_trace_mem_realloc (void *old_ptr,
                                size_t new_size,
                                const char *file, D_UINT line);

void custom_trace_mem_free (void *ptr, const char *file, D_UINT line);

void *custom_mem_alloc (size_t size);
void *custom_mem_realloc (void *old_ptr, size_t new_size);
void custom_mem_free (void *ptr);

#ifdef __cplusplus
}
#endif


#endif				/* WHISPER_MEMORY_H */
