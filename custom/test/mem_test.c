
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test/test_fmw.h"

/* GLOBAL variables for testing */
size_t test_mem_used;
size_t test_mem_peak;

W_ALLOCATED_MEMORY* gpListHead      = NULL;
D_UINT64            gMemAllocations = 0;

/* set the maximum memory usage, 0 for unlimited */
size_t test_max_mem;

void
test_set_mem_max (size_t size)
{
  test_max_mem = size;
}

size_t
test_get_mem_max (void)
{
  return test_max_mem;
}

size_t
test_get_mem_used (void)
{
  return test_mem_used;
}

void
test_add_used_mem (size_t add_size)
{
  test_mem_used += add_size;
  if (test_mem_used > test_mem_peak)
    {
      test_mem_peak = test_mem_used;
    }
}

void
test_free_used_mem (size_t free_size)
{
  test_mem_used -= free_size;
}

size_t
test_get_mem_peak (void)
{
  return test_mem_peak;
}

void
test_print_unfree_mem (void)
{
  W_ALLOCATED_MEMORY *pIt = gpListHead;

  printf ("%10s %10s %4s %s\n",
         "Count(low)", "Size", "Line", "File");

  assert (pIt->size >= (sizeof (W_ALLOCATED_MEMORY) + 1));
  while (pIt != NULL)
    {
      printf ("%10d %10d %4d %s\n",
              (unsigned int) pIt->count,
              (unsigned int)pIt->size - (sizeof (W_ALLOCATED_MEMORY) + 1),
              (unsigned int)pIt->line,
              ((pIt->file == NULL) ? "NULL" : pIt->file));
      pIt = pIt->next;
    }

}
