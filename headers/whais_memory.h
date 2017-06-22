/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#ifndef WHAIS_MEMORY_H
#define WHAIS_MEMORY_H

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

//These headers are include here because we need the standard
//STL functionality before we override the new operator.
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <new>

extern "C"
{
#endif

#ifdef ENABLE_MEMORY_TRACE

CUSTOM_SHL void*
custom_trace_mem_alloc(size_t          size,
                       const char*     file,
                       uint_t          line);

CUSTOM_SHL void*
custom_trace_mem_realloc(void*           oldPtr,
                         size_t          newSize,
                         const char*     file,
                         uint_t          line);

CUSTOM_SHL void
custom_trace_mem_free(void*              ptr,
                       const char*        file,
                       uint_t             line);

#endif /* ENABLE_MEMORY_TRACE */

CUSTOM_SHL void*
custom_mem_alloc(size_t size);

CUSTOM_SHL void*
custom_mem_realloc(void* oldPtr, size_t newSize);

CUSTOM_SHL void
custom_mem_free(void* ptr);


#ifdef __cplusplus
} /* extern "C" */


#ifdef CXX_CUSTOM_MEMORY_ALLOCATOR
#ifdef ENABLE_MEMORY_TRACE

void*
operator new(std::size_t size, const char* file, uint_t line);

void*
operator new[](std::size_t size, const char* file, uint_t line);

void
operator delete(void* ptr, const char*, uint_t);

void
operator delete[](void* ptr, const char*, uint_t );

#endif /* ENABLE_MEMORY_TRACE */


void*
operator new(std::size_t size);

void*
operator new(std::size_t size, const std::nothrow_t&) noexcept;

void*
operator new[](std::size_t size);

void*
operator new[](std::size_t size, const std::nothrow_t&) noexcept;

void
operator delete(void* ptr) noexcept;

void
operator delete[](void* ptr) noexcept;

#endif /* CXX_CUSTOM_MEMORY_ALLOCATOR */


template <class T> static inline void
_placement_new(void* place, const T& value)
{
  new(place) T(value);
}

template <class T> static inline void
_placement_new(void* place, T& value)
{
  new(place) T(value);
}

template <class T> static inline void
_placement_new(void* place)
{
  new(place) T;
}


#ifdef ENABLE_MEMORY_TRACE

#ifdef CXX_CUSTOM_MEMORY_ALLOCATOR
#define new new(__FILE__, __LINE__)
#endif

#include <iostream>
#include "custom/include/test/test_fmw.h"


#define shared_make(T,...)       std::shared_ptr<T>(new T(__VA_ARGS__))
#define shared_array_make(T,s)   std::shared_ptr<T[]>(new T[(s)])
#define unique_make(T,...)       std::unique_ptr<T>(new T(__VA_ARGS__))
#define unique_array_make(T,s)   std::unique_ptr<T[]>(new T[(s)])

class WMemoryTracker
{
public:

  WMemoryTracker()
  {
    smInitCount++;
  }

  ~WMemoryTracker()
  {
    if (smInitCount == 0)
    {
      int a = 1;
      int b = 0;

      a /= b; //If you are to crash make it loud.
    }

    if ((--smInitCount & 0x7FFFFFFF) == 0)
      PrintMemoryStatistics();
  }

  static size_t MaxMemoryUsage() { return test_get_mem_max(); }
  static void MaxMemoryUsage(const size_t size) { test_set_mem_max(size); }
  static size_t GetMemoryUsagePeak() { return test_get_mem_peak(); }
  static size_t GetCurrentMemoryUsage() { return test_get_mem_used(); }
  static void PrintMemResume(const bool print)
  {
    if (print)
      smInitCount |= 0x80000000;

    else
      smInitCount &= 0x7FFFFFFF;
  }

  static bool PrintMemResume()
  {
    return ((smInitCount & 0x80000000) != 0) || (getenv("WHAIS_TST_MEM") != NULL);
  }

private:
  static void
  WMemoryTracker::PrintMemoryStatistics()
  {
    if (GetCurrentMemoryUsage() != 0 || !PrintMemResume())
    return;

    std::cout << '(' << smModule << ") ";
    if (GetCurrentMemoryUsage() != 0)
    {
      std::cout << "MEMORY: FAILED\n";
      test_print_unfree_mem();
    }
    std::cout << "MEMORY: OK\n";
    std::cout << "Memory peak  : " << GetMemoryUsagePeak() << " bytes.\n";
    std::cout << "Memory in use: " << GetCurrentMemoryUsage() << " bytes.\n";
  }


  static uint32_t smInitCount;
  static const char* smModule;
};

static WMemoryTracker __One_Hidden_Static_For_Compiling_Unit__;
#else
#define shared_make(T,...)       std::make_shared<T>(__VA_ARGS__)
#define shared_array_make(T,s)   std::make_shared<T>((std::size_t)(s))
#define unique_make(T,...)       std::make_unique<T>(__VA_ARGS__)
#define unique_array_make(T,s)   std::make_unique<T[]>((std::size_t)(s))
#endif /* ENABLE_MEMORY_TRACE */

#endif /*  __cplusplus */

#endif /* WHAIS_MEMORY_H */

