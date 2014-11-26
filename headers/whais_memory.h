/******************************************************************************
WHAIS - An advanced database system
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

#define mem_alloc( x)      custom_mem_alloc( (x))
#define mem_realloc( x, y) custom_mem_realloc( (x), (y))
#define mem_free( x)       custom_mem_free( (x))

#else

#define mem_alloc( x) custom_trace_mem_alloc( (x), __FILE__, __LINE__)
#define mem_realloc( x, y) custom_trace_mem_realloc( (x), (y), __FILE__, __LINE__)
#define mem_free( x) custom_trace_mem_free( (x), __FILE__, __LINE__)

#endif                                /* ENABLE_MEMORY_TRACE */

#ifdef __cplusplus

//These headers are include here because we need the standard
//STL functionality before we override the new operator.
#include <cstddef>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <new>

extern "C"
{
#endif

#ifdef ENABLE_MEMORY_TRACE

void*
custom_trace_mem_alloc( size_t          size,
                        const char*     file,
                        uint_t          line);

void*
custom_trace_mem_realloc( void*           oldPtr,
                          size_t          newSize,
                          const char*     file,
                          uint_t          line);

void
custom_trace_mem_free( void*              ptr,
                       const char*        file,
                       uint_t             line);

#endif /* ENABLE_MEMORY_TRACE */

void* custom_mem_alloc( size_t size);

void* custom_mem_realloc( void* oldPtr, size_t newSize);

void custom_mem_free( void* ptr);


#ifdef __cplusplus
} /* extern "C" */


void*
operator new (std::size_t size) throw( std::bad_alloc);

void*
operator new (std::size_t size, const std::nothrow_t&) throw ();

void*
operator new (std::size_t size, const char* file, uint_t line);

void*
operator new[] (std::size_t size) throw( std::bad_alloc);

void*
operator new[] (std::size_t size, const std::nothrow_t&) throw ();

void*
operator new[] (std::size_t size, const char* file, uint_t line);

void
operator delete( void* ptr) throw ();

void
operator delete( void* ptr, const char*, uint_t);

void
operator delete[] (void* ptr) throw ();

void
operator delete[] (void* ptr, const char*, uint_t );


template <class T> static inline void
_placement_new( void* place, const T& value)
{
  new (place) T (value);
}

template <class T> static inline void
_placement_new( void* place, T& value)
{
  new (place) T (value);
}

template <class T> static inline void
_placement_new( void* place)
{
  new (place) T;
}


#ifdef ENABLE_MEMORY_TRACE

#define new new(__FILE__, __LINE__)

#include <iostream>
#include "custom/include/test/test_fmw.h"

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

  static size_t MaxMemoryUsage()
  {
    return test_get_mem_max();
  }
  static void MaxMemoryUsage( const size_t size)
  {
    test_set_mem_max( size);
  }
  static size_t GetMemoryUsagePeak()
  {
    return test_get_mem_peak();
  }

  static size_t GetCurrentMemoryUsage()
  {
    return test_get_mem_used();
  }

  static void PrintMemResume( const bool print)
  {
    if (print)
      smInitCount |= 0x80000000;

    else
      smInitCount &= 0x7FFFFFFF;
  }

  static bool PrintMemResume()
  {
    return( smInitCount & 0x80000000) != 0;
  }

private:
  static void PrintMemoryStatistics()
  {
    if ((GetCurrentMemoryUsage() == 0) && (! PrintMemResume()))
      return ;

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

  static uint32_t       smInitCount;
  static const char*    smModule;
};

static WMemoryTracker __One_Hidden_Static_For_Compiling_Unit__;

#endif /* ENABLE_MEMORY_TRACE */
#endif /*  __cplusplus */

#endif /* WHAIS_MEMORY_H */

