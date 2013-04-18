#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"

#include "custom/include/test/test_fmw.h"

extern int yyparse (struct ParserState *);


char buffer[] =
  "LET field2 AS FIELD OF ARRAY OF DATE;\n"
  "LET field3 AS FIELD OF INT8;\n"
  "LET vTable2, vTable3 AS TABLE OF (v2 AS DATE, t2 AS DATETIME, t3 as INT16);\n";


extern bool_t is_type_spec_valid (const struct TypeSpec *spec);

static void
init_state_for_test (struct ParserState *state, const char * buffer)
{
  state->buffer = buffer;
  state->strings = create_string_store ();
  state->bufferSize = strlen (buffer);
  wh_array_init (&state->parsedValues, sizeof (struct SemValue));

  init_glbl_stmt (&state->globalStmt);
  state->pCurrentStmt = &state->globalStmt;
}

static void
free_state (struct ParserState *state)
{
  release_string_store (state->strings);
  clear_glbl_stmt (&(state->globalStmt));
  wh_array_clean (&state->parsedValues);

}

static bool_t
check_used_vals (struct ParserState* pState)
{
  int vals_count = wh_array_count (&pState->parsedValues);
  while (--vals_count >= 0)
    {
      struct SemValue *val = wh_array_get (&pState->parsedValues, vals_count);
      if (val->val_type != VAL_REUSE)
        {
          return TRUE;                /* found value still in use */
        }

    }

  return FALSE;                        /* no value in use */
}

static bool_t
check_type_spec_fill (const struct TypeSpec* ts,
                      const char*          fname,
                      const uint_t           type)
{
  const uint8_t *it = ts->data;
  uint_t count = 0;
  if (!is_type_spec_valid (ts))
    {
      return FALSE;
    }

  if (fname == NULL)
    {
      if (ts->dataSize != 2)
        return FALSE;
      else
        return TRUE;
    }

  while (count < (ts->dataSize - 2))
    {
      uint16_t temp = strlen ((char *) it) + 1;
      if (strcmp ((char *) it, fname) != 0)
        {
          it += temp;
          count += temp;
          it += sizeof (uint16_t);
          count += sizeof (uint16_t);
        }
      else
        {
          uint16_t *ts_type = (uint16_t *) (it + temp);
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

static bool_t
check_container_field (struct Statement *stmt,
                       struct DeclaredVar *var, char * field, uint32_t type)
{
  struct DeclaredVar *extra = var->extra;
  char result = TRUE;
  unsigned int f_len = (field != NULL) ? strlen (field) : 0;

  while (extra != NULL)
    {
      result = FALSE;                /* changed to TRUE if everything is good */
      if (IS_TABLE_FIELD (extra->type) == FALSE)
        {
          break;
        }
      if ((extra->labelLength == f_len) &&
          (strncmp (field, extra->label, f_len) == 0))
        {
          if ( (GET_FIELD_TYPE (extra->type) == type) &&
               IS_TABLE_FIELD (extra->type))
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
      uint8_t *const buffer =
        wh_ostream_data (&stmt->spec.glb.typesDescs);
      struct TypeSpec *ts = (struct TypeSpec *) (buffer + var->typeSpecOff);
      result = check_type_spec_fill (ts, field, type);
    }

  return result;
}

static bool_t
check_vars_decl (struct ParserState *state)
{
  struct DeclaredVar *decl_var = NULL;
  struct DeclaredVar *table_1 = NULL;
  struct DeclaredVar *table_2 = NULL;

  if (state->globalStmt.type != STMT_GLOBAL ||
      state->globalStmt.pParentStmt != NULL)
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "field2", 6, FALSE, FALSE);
  if (decl_var == NULL ||
      (IS_FIELD (decl_var->type) == FALSE) ||
      (IS_TABLE_FIELD (decl_var->type) != FALSE) ||
      (IS_ARRAY (GET_FIELD_TYPE (decl_var->type)) == FALSE) ||
      (GET_BASIC_TYPE (decl_var->type) != T_DATE) ||
      decl_var->extra != NULL)
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "field3", 6, FALSE, FALSE);
  if (decl_var == NULL ||
      (IS_FIELD (decl_var->type) == FALSE) ||
      (IS_TABLE_FIELD (decl_var->type) != FALSE) ||
      (IS_ARRAY (GET_FIELD_TYPE (decl_var->type)) != FALSE) ||
      (GET_BASIC_TYPE (decl_var->type) != T_INT8) ||
      decl_var->extra != NULL)
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "vTable2", 7, FALSE, FALSE);
  if (decl_var == NULL ||
      (IS_TABLE (decl_var->type) == FALSE) ||
      (GET_BASIC_TYPE (decl_var->type) != 0) ||
      decl_var->extra == decl_var)
    {
      return FALSE;
    }
  table_1 = decl_var;

  decl_var = stmt_find_declaration (&state->globalStmt, "vTable3", 7, FALSE, FALSE);
  if (decl_var == NULL ||
      (IS_TABLE (decl_var->type) == FALSE) ||
      (GET_BASIC_TYPE (decl_var->type) != 0) ||
      (decl_var->extra != table_1->extra) ||
      (decl_var->typeSpecOff != table_1->typeSpecOff))
    {
      return FALSE;
    }

  table_2 = decl_var;
  if (table_2->extra != table_1->extra)
    {
      return FALSE;
    }

  if (!(check_container_field (&state->globalStmt, decl_var, "v2", T_DATE) &&
        check_container_field (&state->globalStmt, decl_var, "t2",
                               T_DATETIME)
        && check_container_field (&state->globalStmt, decl_var, "t3",
                                  T_INT16)))
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
