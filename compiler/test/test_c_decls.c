#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"

#include "test/test_fmw.h"

extern int yyparse (struct ParserState *);


D_CHAR buffer[] =
  "LET vTable  AS TABLE;\n"
  "LET vTable2, vTable3 AS TABLE OF (v2 AS DATE, t2 AS DATETIME, t3 as INT16);\n";


extern D_BOOL is_type_spec_valid (const struct TypeSpec *spec);

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

static D_BOOL
check_type_spec_fill (const struct TypeSpec *ts,
		      const D_CHAR * fname, const D_UINT type)
{
  const D_UINT8 *it = ts->data;
  D_UINT count = 0;
  if (!is_type_spec_valid (ts))
    {
      return FALSE;
    }

  if (fname == NULL)
    {
      if (ts->data_len != 2)
	{
	  return FALSE;
	}
      else
	{
	  return TRUE;
	}
    }

  while (count < (ts->data_len - 2))
    {
      D_UINT16 temp = strlen ((D_CHAR *) it) + 1;
      if (strcmp ((D_CHAR *) it, fname) != 0)
	{
	  it += temp;
	  count += temp;
	  it += sizeof (D_UINT16);
	  count += sizeof (D_UINT16);
	}
      else
	{
	  D_UINT16 *ts_type = (D_UINT16 *) (it + temp);
	  if (*ts_type == type)
	    {
	      return TRUE;
	    }
	  else
	    {
	      return FALSE;
	    }
	}
    }
  return FALSE;
}

static D_BOOL
check_container_field (struct Statement *stmt,
		       struct DeclaredVar *var, D_CHAR * field, D_UINT32 type)
{
  struct DeclaredVar *extra = var->extra;
  D_CHAR result = TRUE;
  unsigned int f_len = (field != NULL) ? strlen (field) : 0;

  while (extra != NULL)
    {
      result = FALSE;		/* changed to TRUE if everything is good */
      if ((extra->type & T_FIELD_MASK) == 0)
	{
	  break;
	}
      if ((extra->l_label == f_len) &&
	  (strncmp (field, extra->label, f_len) == 0))
	{
	  if (extra->type == (type | T_FIELD_MASK))
	    {
	      result = TRUE;
	    }

	  break;
	}

      /* use the next field */
      extra = extra->extra;
    }

  if (result != FALSE)
    {
      D_UINT8 *const buffer =
	get_buffer_outstream (&stmt->spec.glb.type_desc);
      struct TypeSpec *ts = (struct TypeSpec *) (buffer + var->type_spec_pos);
      result = check_type_spec_fill (ts, field, type);
    }

  return result;
}

static D_BOOL
check_vars_decl (struct ParserState *state)
{
  struct DeclaredVar *decl_var = NULL;
  struct DeclaredVar *table_1 = NULL;
  struct DeclaredVar *table_2 = NULL;

  if (state->global_stmt.type != STMT_GLOBAL ||
      state->global_stmt.parent != NULL)
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt, "vTable", 6, FALSE, FALSE);
  if (decl_var == NULL || decl_var->type != T_TABLE_MASK ||
      decl_var->extra != decl_var)
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->global_stmt, "vTable2", 7, FALSE, FALSE);
  if (decl_var == NULL || decl_var->type != T_TABLE_MASK ||
      decl_var->extra == decl_var)
    {
      return FALSE;
    }
  table_1 = decl_var;

  decl_var = stmt_find_declaration (&state->global_stmt, "vTable3", 7, FALSE, FALSE);
  if (decl_var == NULL || decl_var->type != T_TABLE_MASK ||
      (decl_var->extra != table_1->extra) ||
      (decl_var->type_spec_pos != table_1->type_spec_pos))
    {
      return FALSE;
    }

  table_2 = decl_var;
  if (table_2->extra != table_1->extra)
    {
      return FALSE;
    }

  if (!(check_container_field (&state->global_stmt, decl_var, "v2", T_DATE) &&
	check_container_field (&state->global_stmt, decl_var, "t2",
			       T_DATETIME)
	&& check_container_field (&state->global_stmt, decl_var, "t3",
				  T_INT16)))
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
