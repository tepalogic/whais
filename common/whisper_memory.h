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

#endif                                /* ENABLE_MEMORY_TRACE */


#ifdef __cplusplus
  extern "C"
  {
#endif

CUSTOM_SHL void*
custom_trace_mem_alloc (size_t      size,
                        const char* pFile,
                        D_UINT      line);

CUSTOM_SHL void*
custom_trace_mem_realloc (void*       oldPtr,
                          size_t      newSize,
                          const char* pFile,
                          D_UINT      line);

CUSTOM_SHL void
custom_trace_mem_free (void*       ptr,
                       const char* pFile,
                       D_UINT      line);

CUSTOM_SHL void*
custom_mem_alloc (size_t size);

CUSTOM_SHL void*
custom_mem_realloc (void* oldPtr, size_t newSize);

CUSTOM_SHL void
custom_mem_free (void* ptr);

#ifdef __cplusplus
  } /* extern "C" */

inline void*
operator new (size_t size, const D_CHAR* pFile, D_UINT line)
{
#ifndef ENABLE_MEMORY_TRACE
  (void)pFile;
  (void)line;
  void *ptr = custom_mem_alloc (size);
#else
  void *ptr = custom_trace_mem_alloc (size, pFile, line);
#endif

  if (ptr == NULL)
    throw std::bad_alloc ();
  return ptr;
}

inline void*
operator new [] (size_t size, const D_CHAR* pFile, D_UINT line)
{
#ifndef ENABLE_MEMORY_TRACE
  (void)pFile;
  (void)line;
  void *ptr = custom_mem_alloc (size);
#else
  void *ptr = custom_trace_mem_alloc (size, pFile, line);
#endif

  if (ptr == NULL)
    throw std::bad_alloc ();
  return ptr;
}

inline void*
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

inline void*
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


inline void
operator delete (void* ptr)
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
  custom_trace_mem_free(ptr, NULL, 0);
#endif
}

inline void
operator delete (void* ptr, const D_CHAR*, D_UINT)
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
  custom_trace_mem_free(ptr, NULL, 0);
#endif
}

inline void
operator delete [] (void* ptr)
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
  custom_trace_mem_free(ptr, NULL, 0);
#endif
}

inline void
operator delete[] (void* ptr, const D_CHAR*, D_UINT )
{
  if (ptr != NULL)
#ifndef ENABLE_MEMORY_TRACE
    custom_mem_free(ptr);
#else
    custom_trace_mem_free(ptr, NULL, 0);
#endif
}

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

#endif

#endif /* WHISPER_MEMORY_H */

