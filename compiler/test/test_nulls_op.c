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
init_state_for_test (struct ParserState *state, const D_CHAR * buffer)
{
  state->buffer = buffer;
  state->strings = create_string_store ();
  state->bufferSize = strlen (buffer);
  init_array (&state->parsedValues, sizeof (struct SemValue));

  init_glbl_stmt (&state->globalStmt);
  state->pCurrentStmt = &state->globalStmt;
}

static void
free_state (struct ParserState *state)
{
  release_string_store (state->strings);
  clear_glbl_stmt (&(state->globalStmt));
  destroy_array (&state->parsedValues);

}

static D_BOOL
check_used_vals (struct ParserState *state)
{
  D_INT vals_count = get_array_count (&state->parsedValues);
  while (--vals_count >= 0)
    {
      struct SemValue *val = get_item (&state->parsedValues, vals_count);
      if (val->val_type != VAL_REUSE)
        {
          return TRUE;                /* found value still in use */
        }

    }

  return FALSE;                        /* no value in use */
}

D_CHAR proc_null_check[] =
    "PROCEDURE ProcId1 (arg AS BOOL) RETURN BOOL "
    "DO "
    "RETURN arg == NULL; "
    "ENDPROC\n\n"
    ""
    "PROCEDURE ProcId2 (arg AS TEXT) RETURN BOOL "
    "DO "
    "RETURN NULL == arg; "
    "ENDPROC\n\n"
    ""
    "PROCEDURE ProcId3 (arg AS ARRAY) RETURN BOOL "
    "DO "
    "RETURN NULL == arg; "
    "ENDPROC\n\n"
    ""
    "PROCEDURE ProcId4 (arg AS FIELD OF DATE) RETURN BOOL "
    "DO "
    "RETURN NULL == arg; "
    "ENDPROC\n\n"
    ""
    "PROCEDURE ProcId5 (arg AS BOOL) RETURN BOOL "
    "DO "
    "RETURN arg != NULL; "
    "ENDPROC\n\n"
    ""
    "PROCEDURE ProcId6 (arg AS TEXT) RETURN BOOL "
    "DO "
    "RETURN NULL != arg; "
    "ENDPROC\n\n"
    ""
    "PROCEDURE ProcId7 (arg AS ARRAY) RETURN BOOL "
    "DO "
    "RETURN NULL != arg; "
    "ENDPROC\n\n"
    ""
    "PROCEDURE ProcId8 (arg AS FIELD OF DATE) RETURN BOOL "
    "DO "
    "RETURN NULL != arg; "
    "ENDPROC\n\n"
    "";

static D_BOOL
check_procedure (struct ParserState* state,
                 D_CHAR*             proc_name,
                 const enum W_OPCODE opcode)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  D_UINT8 *code = get_buffer_outstream (stmt_query_instrs (stmt));
  D_INT code_size = get_size_outstream (stmt_query_instrs (stmt));

  if (code_size != 4)
    {
      return FALSE;
    }
  else if ((w_opcode_decode (code) != W_LDLO8) ||
           (code[1] != 0) ||
           (w_opcode_decode (code + 2) != opcode))
    {
      return FALSE;
    }

  return TRUE;
}

static D_BOOL
check_all_procs (struct ParserState *state)
{
  D_UINT count;
  D_CHAR proc_name[25];

  for (count = 1; count < 5; ++count)
    {
      sprintf (proc_name, "ProcId%d", count);
      if (check_procedure (state, proc_name, W_INULL) == FALSE)
        {
          return FALSE;
        }
    }

  for (count = 5; count <= 8; ++count)
    {
      sprintf (proc_name, "ProcId%d", count);
      if (check_procedure (state, proc_name, W_NNULL) == FALSE)
        {
          return FALSE;
        }
    }

  return TRUE;
}

int
main ()
{
  D_BOOL test_result = TRUE;
  struct ParserState state = { 0, };

  init_state_for_test (&state, proc_null_check);

  printf ("Testing parse..");
  if (yyparse (&state) != 0)
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
      if (check_used_vals (&state))
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

  printf ("Testing nulls op usage ...");
  if (check_all_procs (&state))
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
