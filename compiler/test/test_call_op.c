#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"
#include "../semantics/opcodes.h"
#include "../semantics/procdecl.h"

#include "test/test_fmw.h"

extern int yyparse (struct ParserState *);

static void
init_state_for_test (struct ParserState *state, const D_CHAR * buffer)
{
  state->buffer = buffer;
  state->strs = create_string_store ();
  state->buffer_len = strlen (buffer);
  init_array (&state->vals, sizeof (struct SemValue));

  init_glbl_stmt (&state->global_stmt);
  state->current_stmt = &state->global_stmt;
}

static void
free_state (struct ParserState *state)
{
  release_string_store (state->strs);
  clear_glbl_stmt (&(state->global_stmt));
  destroy_array (&state->vals);

}

static D_BOOL
check_used_vals (struct ParserState *state)
{
  D_INT vals_count = get_array_count (&state->vals);
  while (--vals_count >= 0)
    {
      struct SemValue *val = get_item (&state->vals, vals_count);
      if (val->val_type != VAL_REUSE)
	{
	  return TRUE;		/* found value still in use */
	}

    }

  return FALSE;			/* no value in use */
}

D_CHAR proc_decl_buffer[] =
  "PROCEDURE ProcId0 () "
  "RETURN BOOL "
  "DO "
  "RETURN FALSE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId1( v1 AS BOOL, "
  "          v2 AS CHARACTER, "
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
  "          v13 AS UNSIGNED INT8, "
  "          v14 AS UNSIGNED INT16, "
  "          v15 AS UNSIGNED INT32, "
  "          v16 AS UNSIGNED INT64) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId2( v1 AS ARRAY, v2 AS ARRAY OF TEXT) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId3( v1 AS RECORD, "
  "          v2 AS RECORD WITH ( var1 AS REAL, "
  "                      var2 AS ARRAY, "
  "                      var3 AS ARRAY OF UNSIGNED INT16)) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcId4 (v1 AS TABLE WITH ( v1 AS DATE, v2 AS INT8, v3 as ARRAY OF INT8), "
  "          v2 AS TABLE) "
  "RETURN BOOL "
  "DO "
  "RETURN TRUE; "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst0() "
  "RETURN BOOL "
  "DO "
  "RETURN ProcId0(); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst1() "
  "RETURN BOOL "
  "DO "
  "LET v1 AS BOOL; "
  "LET v2 AS CHARACTER; "
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
  "LET v13 AS UNSIGNED INT64; "
  "LET v14 AS UNSIGNED INT64; "
  "LET v15 AS UNSIGNED INT64; "
  "LET v16 AS UNSIGNED INT64; "
  "RETURN ProcId1 (v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst2() "
  "RETURN BOOL "
  "DO "
  "LET v1 AS ARRAY; "
  "LET v2 AS ARRAY OF TEXT; "
  "RETURN ProcId2(v1, v2); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst2_1() "
  "RETURN BOOL "
  "DO "
  "LET v1 AS ARRAY OF DATETIME; "
  "LET v2 AS ARRAY OF TEXT; "
  "RETURN ProcId2(v1, v2); "
  "ENDPROC "
  " "
  " "
  "PROCEDURE ProcIdTst3() "
  "RETURN BOOL "
  "DO "
  "LET v1 AS RECORD WITH (f1 AS INT8, f2 AS INT64); "
  "LET v2 AS RECORD WITH (var1 AS REAL, var2 AS ARRAY, var3 AS ARRAY OF UNSIGNED INT16); "
  "RETURN ProcId3(v1, v2); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst3_1() "
  "RETURN BOOL "
  "DO "
  "LET v1 AS RECORD; "
  "LET v2 AS RECORD WITH (var1 AS RICHREAL, var2 AS ARRAY OF INT8, var3 AS ARRAY OF UNSIGNED INT32); "
  "RETURN ProcId3(v1, v2); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst4() "
  "RETURN BOOL "
  "DO "
  "LET v1 AS TABLE WITH (v1 AS DATE, v2 AS INT8, v3 as ARRAY OF INT8); "
  "LET v2 AS TABLE; "
  "RETURN ProcId4(v1, v2); "
  "ENDPROC "
  " "
  "PROCEDURE ProcIdTst4_1() "
  "RETURN BOOL "
  "DO "
  "LET v1 AS TABLE WITH (v1 AS DATE, v2 AS INT8, v3 as ARRAY OF INT8); "
  "LET v2 AS TABLE WITH (v1 AS HIRESTIME, v2 AS TEXT, v3 AS ARRAY OF BOOL); "
  "RETURN ProcId4(v1, v2); " "ENDPROC " "\n" "";

static D_BOOL
check_procedure (struct ParserState *state,
		 D_CHAR * proc_name, D_CHAR * called_proc, D_UINT nargs)
{
  struct Statement *stmt = find_proc_decl (state, proc_name,
					   strlen (proc_name));
  struct Statement *called_stmt = find_proc_decl (state, called_proc,
						  strlen (called_proc));
  D_UINT8 *code = get_buffer_outstream (stmt_query_instrs (stmt));
  D_UINT code_size = get_size_outstream (stmt_query_instrs (stmt));
  D_UINT count = 0;
  D_UINT32 linkid = stmt_get_import_id (called_stmt);
  D_UINT temp = 0;

  if (code_size < ((nargs * 2) + 5))
    {
      return FALSE;
    }
  for (count = 0; count < nargs; ++count)
    {
      const D_UINT offset = count * 2;
      if ((w_opcode_decode (code + offset) != W_LDLO8) ||
	  (code[offset + 1] != (count + 1)))
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

static D_BOOL
check_procedure_calls (struct ParserState *state)
{
  D_BOOL result = TRUE;
  result &= check_procedure (state, "ProcIdTst0", "ProcId0", 0);

  result &= check_procedure (state, "ProcIdTst1", "ProcId1", 16);

  result &= check_procedure (state, "ProcIdTst2", "ProcId2", 2);
  result &= check_procedure (state, "ProcIdTst2_1", "ProcId2", 2);

  result &= check_procedure (state, "ProcIdTst3", "ProcId3", 2);
  result &= check_procedure (state, "ProcIdTst3_1", "ProcId3", 2);

  result &= check_procedure (state, "ProcIdTst4", "ProcId4", 2);
  result &= check_procedure (state, "ProcIdTst4_1", "ProcId4", 2);

  return result;

}

int
main ()
{
  D_BOOL test_result = TRUE;
  struct ParserState state = { 0, };

  init_state_for_test (&state, proc_decl_buffer);

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
  printf ("Testing function calls...");
  if (check_procedure_calls (&state) == FALSE)
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
