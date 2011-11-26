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
  "PROCEDURE ProcId1 (v1 AS INT8, v2 AS INT8) RETURN INT32 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId2 (v1 AS INT8, v2 AS INT16) RETURN INT16 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId3 (v1 AS INT8, v2 AS INT32) RETURN INT32 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId4 (v1 AS INT8, v2 AS INT64) RETURN INT64 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId5 (v1 AS INT16, v2 AS INT16) RETURN INT16 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId6 (v1 AS INT16, v2 AS INT32) RETURN INT32 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId7 (v1 AS INT16, v2 AS INT64) RETURN INT64 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId8 (v1 AS INT32, v2 AS INT32) RETURN INT32 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId9 (v1 AS INT32, v2 AS INT64) RETURN INT64 "
  "DO "
  "RETURN v1 % v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId10 (v1 AS INT64, v2 AS INT64) RETURN INT64 "
  "DO " "RETURN v1 % v2; " "ENDPROC\n\n" "" "";

static D_BOOL
check_op_symmetry ()
{
  D_INT i, j;
  for (i = T_END_OF_TYPES - 1, j = 0; (i >= 0) && (j < T_END_OF_TYPES);
       i--, j++)
    {
      if (mod_op[i][j] != mod_op[j][i])
	{
	  return FALSE;
	}
    }

  if ((i >= 0) || (j != T_END_OF_TYPES))
    {
      /* weird error */
      return FALSE;
    }
  return TRUE;
}

static D_BOOL
check_procedure (struct ParserState *state, D_CHAR * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name));
  struct DeclaredVar *var =
    (struct DeclaredVar *) get_item (&stmt->spec.proc.param_list, 0);
  D_UINT8 *code = get_buffer_outstream (stmt_query_instrs (stmt));
  D_INT code_size = get_size_outstream (stmt_query_instrs (stmt));
  enum W_OPCODE op_expect = W_NA;

  /* check the opcode based on the return type */
  switch (var->type)
    {
    case T_INT8:
    case T_INT16:
    case T_INT32:
    case T_INT64:
      op_expect = W_MOD;
      break;
    default:
      /* we should no be here */
      return FALSE;
    }

  if (code_size < 4)
    {
      return FALSE;
    }
  else if (w_opcode_decode (code + 4) != op_expect)
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

  for (count = 1; count <= 10; ++count)
    {
      sprintf (proc_name, "ProcId%d", count);
      if (check_procedure (state, proc_name) == FALSE)
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

  printf ("Testing mod op symmetry...");
  if (check_op_symmetry ())
    {
      printf ("PASSED\n");
    }
  else
    {
      printf ("FAILED\n");
      test_result = FALSE;
    }

  printf ("Testing mod op usage ...");
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
