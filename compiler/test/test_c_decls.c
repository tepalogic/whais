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
      if (ts->dataSize != 2)
        return FALSE;
      else
        return TRUE;
    }

  while (count < (ts->dataSize - 2))
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
      D_UINT8 *const buffer =
        get_buffer_outstream (&stmt->spec.glb.typesDescs);
      struct TypeSpec *ts = (struct TypeSpec *) (buffer + var->typeSpecOff);
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

  if (state->globalStmt.type != STMT_GLOBAL ||
      state->globalStmt.pParentStmt != NULL)
    {
      return FALSE;
    }

  decl_var = stmt_find_declaration (&state->globalStmt, "vTable", 6, FALSE, FALSE);
  if (decl_var == NULL ||
      (IS_TABLE (decl_var->type) == FALSE) ||
      (GET_BASIC_TYPE (decl_var->type) != 0) ||
      decl_var->extra != decl_var)
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
