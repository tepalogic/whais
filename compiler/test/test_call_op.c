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
  "PROCEDURE ProcId0 () "
  "RETURN BOOL "
  "DO "
  "RETURN FALSE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId1 (v1 AS BOOL, "
  "          v2 AS CHAR, "
  "          v3 AS DATE, "
  "          v4 AS DATETIME, "
  "          v5 AS HIRESTIME, "
  "          v6 AS INT8, "
  "          v7 AS INT16, "
  "          v8 AS INT32, "
  "          v9 AS INT64, "
  "          v10 AS REAL, "
  "          v11 AS RICHREAL,         "
  "          v12 AS TEXT, "
  "          v13 AS UINT8, "
  "          v14 AS UINT16, "
  "          v15 AS UINT32, "
  "          v16 AS UINT64) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId2 (v1 AS ARRAY, v2 AS ARRAY OF TEXT) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId3 (v1 AS TABLE OF ( v1 AS DATE, v2 AS INT8, v3 as ARRAY OF INT8), "
  "          v2 AS TABLE) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst0 () "
  "RETURN BOOL "
  "DO "
  "RETURN ProcId0 (); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst1 () "
  "RETURN BOOL "
  "DO "
  "LET v1 AS BOOL; "
  "LET v2 AS CHAR; "
  "LET v3 AS DATE; "
  "LET v4 AS DATETIME; "
  "LET v5 AS HIRESTIME; "
  "LET v6 AS INT8; "
  "LET v7 AS INT8; "
  "LET v8 AS INT8; "
  "LET v9 AS INT8; "
  "LET v10 AS REAL; "
  "LET v11 AS RICHREAL; "
  "LET v12 AS TEXT; "
  "LET v13 AS UINT64; "
  "LET v14 AS UINT64; "
  "LET v15 AS UINT64; "
  "LET v16 AS UINT64; "
  "RETURN ProcId1 (v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst2 () "
  "RETURN BOOL "
  "DO "
  "LET v1 AS ARRAY OF DATE; "
  "LET v2 AS ARRAY OF TEXT; "
  "RETURN ProcId2(v1, v2); "
  "ENDPROC "
  " "
  " "
  "PROCEDURE ProcIdTst3 () "
  "RETURN BOOL "
  "DO "
  "LET v1 AS TABLE OF (v1 AS DATE, v2 AS INT8, v3 as ARRAY OF INT8); "
  "LET v2 AS TABLE OF (v1 AS HIRESTIME, v2 AS TEXT, v3 AS ARRAY OF BOOL); "
  "RETURN ProcId3(v1, v2); " "ENDPROC " "\n" "";

static bool_t
check_procedure (struct ParserState *state,
                 char * proc_name, char * called_proc, uint_t nargs)
{
  struct Statement *stmt = find_proc_decl (state, proc_name,
                                           strlen (proc_name), FALSE);
  struct Statement *called_stmt = find_proc_decl (state, called_proc,
                                                  strlen (called_proc), FALSE);
  uint8_t *code = wh_ostream_data (stmt_query_instrs( stmt));
  uint_t code_size = wh_ostream_size (stmt_query_instrs( stmt));
  uint_t count = 0;
  uint32_t linkid = stmt_get_import_id (called_stmt);
  uint_t temp = 0;

  if (code_size < ((nargs * 2) + 5))
    {
      return FALSE;
    }
  for (count = 0; count < nargs; ++count)
    {
      const uint_t offset = count * 2;
      if ((decode_opcode( code + offset) != W_LDLO8)
          || (code[offset + 1] != count))
        {
          return FALSE;
        }
    }
  count = nargs * 2;
  if ((code[count] != W_CALL))
    {
      return 0;
    }
  count += 4;
  temp = code[count--];
  temp <<= 8;
  temp |= code[count--];
  temp <<= 8;
  temp |= code[count--];
  temp <<= 8;
  temp |= code[count];

  if (temp != linkid)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_procedure_calls (struct ParserState *state)
{
  bool_t result = TRUE;
  result &= check_procedure (state, "ProcIdTst0", "ProcId0", 0);

  result &= check_procedure (state, "ProcIdTst1", "ProcId1", 16);
  result &= check_procedure (state, "ProcIdTst2", "ProcId2", 2);
  result &= check_procedure (state, "ProcIdTst3", "ProcId3", 2);


  return result;

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
  printf ("Testing function calls...");
  if (check_procedure_calls( &state) == FALSE)
    {
      printf ("FAILED\n");
      test_result = FALSE;
    }
  else
    {
      printf ("PASSED\n");
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
