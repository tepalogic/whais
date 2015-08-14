#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"
#include "../semantics/opcodes.h"
#include "../semantics/procdecl.h"

#include "custom/include/test/test_fmw.h"

extern int yyparse (struct ParserState *);

static void
init_state_for_test (struct ParserState *state, const char * buffer)
{
  state->buffer = buffer;
  state->strings = create_string_store ();
  state->bufferSize = strlen (buffer);
  wh_array_init (&state->values, sizeof (struct SemValue));

  init_glbl_stmt (&state->globalStmt);
  state->pCurrentStmt = &state->globalStmt;
}

static void
free_state (struct ParserState *state)
{
  release_string_store (state->strings);
  clear_glbl_stmt (&(state->globalStmt));
  wh_array_clean (&state->values);
}

static bool_t
check_used_vals (struct ParserState *state)
{
  int vals_count = wh_array_count (&state->values);
  while (--vals_count >= 0)
    {
      struct SemValue *val = wh_array_get (&state->values, vals_count);
      if (val->val_type != VAL_REUSE)
        {
          return TRUE;                /* found value still in use */
        }

    }

  return FALSE;                        /* no value in use */
}

char proc_decl_buffer[] =
  "PROCEDURE ProcId0 (v1 INT8, v2 UINT8) RETURN INT8 "
  "DO "
  "RETURN v1 |= v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId1 (v1 UINT16, v2 INT16) RETURN UINT16 "
  "DO "
  "RETURN v1 |= v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId2 (v1 INT32, v2 INT32) RETURN INT32 "
  "DO "
  "RETURN v1 |= v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId3 (v1 INT64, v2 UINT64) RETURN INT64 "
  "DO "
  "RETURN v1 |= v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId4 (v1 BOOL, v2 BOOL) RETURN BOOL "
  "DO "
  "RETURN v1 |= v2; "
  "ENDPROC\n\n"
  "";

const enum W_OPCODE _opcodes_expected [] = {
                                              W_SOR,
                                              W_SOR,
                                              W_SOR,
                                              W_SOR,
                                              W_SORB
                                            };
static bool_t
check_procedure (struct ParserState* state,
                 const char*       proc_name,
                 const enum W_OPCODE op_expect)
{
  struct Statement *stmt = find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  uint8_t *code = wh_ostream_data (stmt_query_instrs( stmt));
  int code_size = wh_ostream_size (stmt_query_instrs( stmt));

  if (code_size < 5)
    {
      return FALSE;
    }
  else if (decode_opcode( code + 4) != op_expect)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_all_procs (struct ParserState *state)
{
  uint_t count;
  char proc_name[25];

  for (count = 0; count < 5; ++count)
    {
      sprintf (proc_name, "ProcId%d", count);
      if (check_procedure( state, proc_name, _opcodes_expected [count]) == FALSE)
        {
          return FALSE;
        }
    }

  return TRUE;
}

int
main ()
{
  bool_t test_result = TRUE;
  struct ParserState state = { 0, };

  init_state_for_test (&state, proc_decl_buffer);

  printf ("Testing parse..");
  if (yyparse( &state) != 0)
    {
      printf ("FAILED\n");
      test_result = FALSE;
    }
  else
    {
      printf ("PASSED\n");
    }

  if (test_result)
    {
      printf ("Testing garbage vals...");
      if (check_used_vals( &state))
        {
          /* those should no be here */
          printf ("FAILED\n");
          test_result = FALSE;
        }
      else
        {
          printf ("PASSED\n");
        }
    }

  printf ("Testing '^=' operator usage ...");
  if (check_all_procs( &state))
    {
      printf ("PASSED\n");
    }
  else
    {
      printf ("FAILED\n");
      test_result = FALSE;
    }

  free_state (&state);
  printf ("Memory peak: %u bytes \n", test_get_mem_peak ());
  printf ("Current memory usage: %u bytes...", test_get_mem_used ());
  if (test_get_mem_used () != 0)
    {
      test_result = FALSE;
      printf ("FAILED\n");
    }
  else
    {
      printf ("PASSED\n");
    }

  if (test_result == FALSE)
    {
      printf ("TEST RESULT: FAIL\n");
      return -1;
    }

  printf ("TEST RESULT: PASS\n");
  return 0;
}

