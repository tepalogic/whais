#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"

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

D_CHAR buffer[] =
  "LET aVar01_ AS ARRAY OF INT32;\n"
  "LET aVar02_ AS ARRAY OF REAL;\n"
  "LET aVar03, __array__ AS ARRAY;\n"
  "LET aVar031 AS ARRAY OF REAL;\n"
  "LET aVar04, __array_2_ AS ARRAY OF TEXT;\n";

static D_BOOL
check_declared_var (struct Statement *stm,
		    struct DeclaredVar *var, D_UINT type)
{
  struct OutStream *os = &stm->spec.glb.type_desc;
  if ((var == NULL) ||		/* var not found */
      (var->type != type) ||	/* invalid type */
      ((var->var_id & GLOBAL_DECLARED)) == 0)
    {
      return FALSE;
    }

  if ((var->type_spec_pos >= get_size_outstream (os)))
    {
      return FALSE;
    }
  else
    {
      struct TypeSpec *ts = (struct TypeSpec *)
	&(get_buffer_outstream (os)[var->type_spec_pos]);
      if ((ts->type != type) ||
	  (ts->data_len != 2) || (ts->data[0] != TYPE_SPEC_END_MARK))
	{
	  return FALSE;
	}
    }

  return TRUE;
}

static D_BOOL
check_vars_decl (struct ParserState *state)
{
  struct DeclaredVar *decl_var = NULL;
  D_UINT count;
  D_UINT temp_pos;

  if (state->global_stmt.type != STMT_GLOBAL ||
      state->global_stmt.parent != NULL)
    {
      return FALSE;
    }

  count = get_array_count (&(state->global_stmt.decls));
  if (count != 7)
    {
      /* error: more declarations?!? */
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt, "aVar01_", 7, FALSE);
  if (!check_declared_var
      (&state->global_stmt, decl_var, (T_INT32 | T_ARRAY_MASK)))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt, "aVar02_", 7, FALSE);
  if (!check_declared_var
      (&state->global_stmt, decl_var, (T_REAL | T_ARRAY_MASK)))
    {
      return FALSE;
    }
  temp_pos = decl_var->type_spec_pos;

  decl_var = stmt_find_declaration (&state->global_stmt, "aVar03", 6, FALSE);
  if (!check_declared_var
      (&state->global_stmt, decl_var, (T_UNDETERMINED | T_ARRAY_MASK)))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt, "aVar04", 6, FALSE);
  if (!check_declared_var
      (&state->global_stmt, decl_var, (T_TEXT | T_ARRAY_MASK)))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt,
				    "__array__", 9, FALSE);
  if (!check_declared_var
      (&state->global_stmt, decl_var, (T_UNDETERMINED | T_ARRAY_MASK)))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt,
				    "__array_2_", 10, FALSE);
  if (!check_declared_var
      (&state->global_stmt, decl_var, (T_TEXT | T_ARRAY_MASK)))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt, "aVar031", 7, FALSE);
  if (!check_declared_var
      (&state->global_stmt, decl_var, (T_REAL | T_ARRAY_MASK))
      || temp_pos != decl_var->type_spec_pos)
    {
      return FALSE;
    }

  return TRUE;
}

int
main ()
{
  D_BOOL test_result = TRUE;
  struct ParserState state = { 0, };

  init_state_for_test (&state, buffer);

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

  printf ("Testing declarations...");
  if (check_vars_decl (&state) == FALSE)
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
