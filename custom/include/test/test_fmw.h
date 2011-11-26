#ifndef TEST_FMW_H_
#define TEST_FMW_H_

#ifndef BUILD_TESTS
#error("This header file is only intended to be used in a test environment");
#endif

#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ParserState;

typedef struct _W_ALLOCATED_MEMORY
{
  struct _W_ALLOCATED_MEMORY *prev;
  struct _W_ALLOCATED_MEMORY *next;
  size_t      size;
  const char *file;
  D_UINT32    line;
} W_ALLOCATED_MEMORY;


/* in case if you want to go on your own */
extern int yyparse (struct ParserState *);

/* limit the memory usage, 0 for unlimited */
void test_set_mem_max (size_t size);

/* get the current limit of memory usage, 0 for unlimited */
size_t test_get_mem_max (void);

/* get the memory that is used */
size_t test_get_mem_used (void);

/* add memory usage */
void test_add_used_mem (size_t add_size);

/*  decrease memory usage */
void test_free_used_mem (size_t free_size);

/* get the used memory peak */
size_t test_get_mem_peak (void);

/* print the unallocated memory blocks */
void test_print_unfree_mem (void);

#ifdef __cplusplus
}
#endif

#endif /*TEST_FMW_H_ */
