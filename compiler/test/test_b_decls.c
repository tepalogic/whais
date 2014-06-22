#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/endianness.h"

#include "../parser/parser.h"
#include "../semantics/vardecl.h"

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

char buffer[] =
  "LET _var_01 AS BOOL;\n"
  "LET _var_02 AS CHARACTER;\n"
  "LET _var_03 AS DATE;\n"
  "LET _var_04 AS DATETIME;\n"
  "LET _var_05 AS HIRESTIME;\n"
  "LET _var_06 AS INT8;\n"
  "LET _var_07 AS INT16;\n"
  "LET _var_08 AS INT32;\n"
  "LET _var_09, second_var AS INT64;\n"
  "LET _var_10, _1, a2, _a3 AS REAL;\n"
  "LET _var_11, b4c AS RICHREAL;\n"
  "LET _var_12 AS TEXT;\n"
  "LET _var_13 AS UNSIGNED INT8;\n"
  "LET _var_14 AS UNSIGNED INT16;\n"
  "LET _var_15 AS UNSIGNED INT32;\n" "LET _var_16 AS UNSIGNED INT64;\n";

static bool_t
check_declared_var (struct Statement *stm,
                    struct DeclaredVar *var, uint_t type)
{
  struct WOutputStream *os = &stm->spec.glb.typesDescs;
  if ((var == NULL) ||                /* var not found */
      (var->type != type) ||        /* invalid type */
      ((var->varId & GLOBAL_DECL)) == 0)
    {
      return FALSE;
    }

  if ((var->typeSpecOff >= wh_ostream_size (os)))
    {
      return FALSE;
    }
  else
    {
      struct TypeSpec *ts = (struct TypeSpec *)
        &(wh_ostream_data (os)[var->typeSpecOff]);

      if ((load_le_int16 (ts->type) != type)
          || (load_le_int16 (ts->dataSize) != 2)
          || (ts->data[0] != TYPE_SPEC_END_MARK)
          || (ts->data[1] != 0))
        {
          return FALSE;
        }
    }

  return TRUE;
}

static bool_t
check_vars_decl (struct ParserState *state)
{
  struct DeclaredVar *decl_var = NULL;
  uint_t count;

  if (state->globalStmt.type != STMT_GLOBAL ||
      state->globalStmt.parent != NULL)
    {
      return FALSE;
    }

  count = wh_array_count (&(state->globalStmt.decls));
  if (count != 21)
    {
      /* error: more declarations! */
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_01", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_BOOL))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_02", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_CHAR))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_03", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_DATE))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_04", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_DATETIME))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_05", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_HIRESTIME))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_06", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_INT8))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_07", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_INT16))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_08", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_INT32))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_09", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_INT64))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_10", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_REAL))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_11", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_RICHREAL))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_12", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_TEXT))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_13", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_UINT8))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_14", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_UINT16))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_15", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_UINT32))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_16", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_UINT64))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_var_16", 7, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_UINT64))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt,
                                    "second_var", 10, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_INT64))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_1", 2, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_REAL))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "a2", 2, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_REAL))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "_a3", 3, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_REAL))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "b4c", 3, FALSE, FALSE);
  if (!check_declared_var (&state->globalStmt, decl_var, T_RICHREAL))
    {
      return FALSE;
    }

  return TRUE;
}

int
main ()
{
  bool_t test_result = TRUE;
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
