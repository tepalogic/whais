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
  "PROCEDURE ProcId1 (v1 BOOL, "
  "          v2 CHAR, "
  "          v3 DATE, "
  "          v4 DATETIME, "
  "          v5 HIRESTIME, "
  "          v6 INT8, "
  "          v7 INT16, "
  "          v8 INT32, "
  "          v9 INT64, "
  "          v10 REAL, "
  "          v11 RICHREAL,         "
  "          v12 TEXT, "
  "          v13 UINT8, "
  "          v14 UINT16, "
  "          v15 UINT32, "
  "          v16 UINT64) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId2 (v1 ARRAY, v2 TEXT ARRAY) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId3 (v1 TABLE ( v1 DATE, v2 INT8, v3 INT8 ARRAY), "
  "          v2 TABLE) "
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
  "VAR v1 BOOL; "
  "VAR v2 CHAR; "
  "VAR v3 DATE; "
  "VAR v4 DATETIME; "
  "VAR v5 HIRESTIME; "
  "VAR v6 INT8; "
  "VAR v7 INT8; "
  "VAR v8 INT8; "
  "VAR v9 INT8; "
  "VAR v10 REAL; "
  "VAR v11 RICHREAL; "
  "VAR v12 TEXT; "
  "VAR v13 UINT64; "
  "VAR v14 UINT64; "
  "VAR v15 UINT64; "
  "VAR v16 UINT64; "
  "RETURN ProcId1 (v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst2 () "
  "RETURN BOOL "
  "DO "
  "VAR v1 DATE ARRAY; "
  "VAR v2 TEXT ARRAY; "
  "RETURN ProcId2(v1, v2); "
  "ENDPROC "
  " "
  " "
  "PROCEDURE ProcIdTst3 () "
  "RETURN BOOL "
  "DO "
  "VAR v1 TABLE (v1 DATE, v2 INT8, v3 INT8 ARRAY); "
  "VAR v2 TABLE (v1 HIRESTIME, v2 TEXT, v3 BOOL ARRAY); "
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
