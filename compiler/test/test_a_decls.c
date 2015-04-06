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
  "LET aVar01_ AS ARRAY OF INT32;\n"
  "LET aVar02_ AS ARRAY OF REAL;\n"
  "LET aVar031 AS ARRAY OF REAL;\n"
  "LET aVar04, __array_2_ AS ARRAY OF TEXT;\n";

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
        &(wh_ostream_data( os)[var->typeSpecOff]);
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
  uint_t temp_pos;
  uint_t type;

  if (state->globalStmt.type != STMT_GLOBAL ||
      state->globalStmt.parent != NULL)
    {
      return FALSE;
    }

  count = wh_array_count (&(state->globalStmt.decls));
  if (count != 5)
    {
      /* error: more declarations?!? */
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "aVar01_", 7, FALSE, FALSE);
  type     = T_INT32;
  if (!check_declared_var( &state->globalStmt, decl_var, MARK_ARRAY (type)))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "aVar02_", 7, FALSE, FALSE);
  type     = T_REAL;
  if (!check_declared_var( &state->globalStmt, decl_var, MARK_ARRAY (type)))
    {
      return FALSE;
    }
  temp_pos = decl_var->typeSpecOff;

  decl_var = stmt_find_declaration (&state->globalStmt, "aVar04", 6, FALSE, FALSE);
  type     = T_TEXT;
  if (!check_declared_var( &state->globalStmt, decl_var, MARK_ARRAY ((type))))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "__array_2_", 10, FALSE, FALSE);
  type     = T_TEXT;
  if (!check_declared_var( &state->globalStmt, decl_var, MARK_ARRAY (type)))
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "aVar031", 7, FALSE, FALSE);
  type     = T_REAL;
  if (!check_declared_var( &state->globalStmt, decl_var, MARK_ARRAY (type)) ||
      (temp_pos != decl_var->typeSpecOff))
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

  printf ("Testing declarations...");
  if (check_vars_decl( &state) == FALSE)
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
