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
check_container_field (struct DeclaredVar *extra, D_CHAR * field,
                       unsigned type)
{
  D_CHAR result = FALSE;
  unsigned int f_len = strlen (field);
  while (extra != NULL)
    {
      if (IS_TABLE_FIELD (extra->type) == FALSE)
        {
          break;
        }
      if ((extra->labelLength == f_len) &&
          (strncmp (field, extra->label, f_len) == 0))
        {
          if ((GET_FIELD_TYPE (extra->type) == type) &&
              IS_TABLE_FIELD (extra->type))
            {
              result = TRUE;
            }

          break;
        }

      /* use the next field */
      extra = extra->extra;
    }

  return result;
}

D_CHAR proc_decl_buffer[] =
  "PROCEDURE ProcId1 () RETURN TEXT "
  "DO "
  "LET dummy_var1 as REAL; "
  "LET dummy_var2 as FIELD OF INT64; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId02 (Var1 as INT8, Var2 as TEXT, Var3 AS DATETIME) "
  "RETURN ARRAY OF TEXT "
  "DO "
  "LET dummy_var1 as TEXT; "
  "RETURN NULL; "
  "ENDPROC "
  ""
  "PROCEDURE ProcId_3_ (Var1 as REAL, "
  "                   Var2 as TEXT, Var3 as ARRAY, "
  "                   Var4 AS TABLE OF ( f1 AS REAL, f2 as UNSIGNED INT32, "
  "                                      f3 as ARRAY OF INT16),"
  "                   Var5 AS INT64,"
  "                   Var6 AS FIELD OF HIRESTIME) "
  "RETURN TABLE OF (f1 as TEXT, f2 as DATETIME, f3 as REAL) "
  "DO "
  "LET f1 as TEXT; "
  "LET f2 as REAL; "
  "LET f3 as FIELD OF ARRAY OF REAL; "
  "RETURN NULL; "
  "ENDPROC";

static D_BOOL
general_proc_check (struct Statement *glb_stmt,
                    struct Statement *proc_stmt,
                    D_CHAR * proc_name, D_INT parameters, D_INT local_vars)
{
  D_UINT count = 0;
  D_UINT nlocals = 0;
  if ((proc_stmt->type != STMT_PROC) ||
      (proc_stmt->pParentStmt != glb_stmt) ||
      (proc_stmt->spec.proc.nameLength != strlen (proc_name)) ||
      (strncmp (proc_stmt->spec.proc.name, proc_name, proc_stmt->spec.proc.nameLength) != 0))
    {
      /* what is this? */
      return FALSE;
    }

  if (get_array_count (&(proc_stmt->spec.proc.paramsList)) != parameters + 1)
    {
      return FALSE;
    }

  while (count < get_array_count (&(proc_stmt->decls)))
    {
      struct DeclaredVar *var = get_item (&(proc_stmt->decls), count);
      if (IS_TABLE_FIELD (var->type) == FALSE)
        {
          nlocals++;
        }
      count++;
    }

  if (nlocals != local_vars)
    {
      return FALSE;
    }
  return TRUE;
}

static D_BOOL
check_procs_decl (struct ParserState *state)
{
  struct Statement *const glb_stmt = &(state->globalStmt);
  struct UArray *const proc_decls = &(glb_stmt->spec.glb.procsDecls);
  struct Statement *proc = NULL;
  struct DeclaredVar *tmp_var = NULL;
  struct DeclaredVar *tmp_table = NULL;
  D_UINT type = T_INT16;

  if ((glb_stmt->pParentStmt != NULL) || (glb_stmt->type != STMT_GLOBAL))
    {
      /* not a global statement */
      return FALSE;
    }

  if (get_array_count (proc_decls) != 3)
    {
      /* more or less procedures declared! */
      return FALSE;
    }

  proc = get_item (proc_decls, 0);
  if (!general_proc_check (glb_stmt, proc, "ProcId1", 0, 2))
    {
      return FALSE;
    }
  /*check return type */
  tmp_var = get_item (&(proc->spec.proc.paramsList), 0);
  if ((tmp_var->label != NULL) || (tmp_var->labelLength != 0))
    {
      /* return type not properly encoded */
      return FALSE;
    }
  if ((tmp_var->type != T_TEXT) || (tmp_var->extra != NULL))
    {
      return FALSE;
    }
  /* check local declarations */
  tmp_var = stmt_find_declaration (proc, "dummy_var1",
                                   strlen ("dummy_var1"), FALSE, FALSE);
  if (tmp_var->type != T_REAL)
    {
      return FALSE;
    }

  tmp_var = stmt_find_declaration (proc, "dummy_var2",
                                   strlen ("dummy_var2"), FALSE, FALSE);
  if ((IS_FIELD(tmp_var->type) == FALSE) ||
       (GET_FIELD_TYPE (tmp_var->type) != T_INT64))
    {
      return FALSE;
    }

  proc = get_item (proc_decls, 1);
  if (!general_proc_check (glb_stmt, proc, "ProcId02", 3, 1))
    {
      return FALSE;
    }
  /*check return type */
  tmp_var = get_item (&(proc->spec.proc.paramsList), 0);
  if ((tmp_var->label != NULL) || (tmp_var->labelLength != 0))
    {
      /* return type not properly encoded */
      return FALSE;
    }
  if ((GET_BASIC_TYPE (tmp_var->type) != T_TEXT) ||
      (IS_ARRAY (tmp_var->type) == FALSE) ||
      (tmp_var->extra != NULL))
    {
      return FALSE;
    }
  /* check local declarations */
  tmp_var = stmt_find_declaration (proc, "dummy_var1",
                                   strlen ("dummy_var1"), FALSE, FALSE);
  if (tmp_var->type != T_TEXT)
    {
      return FALSE;
    }

  /* check parameters */
  tmp_var = get_item (&(proc->spec.proc.paramsList), 1);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var1", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) || (tmp_var->type != T_INT8))
    {
      return FALSE;
    }

  tmp_var = get_item (&(proc->spec.proc.paramsList), 2);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var2", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) || (tmp_var->type != T_TEXT))
    {
      return FALSE;
    }

  tmp_var = get_item (&(proc->spec.proc.paramsList), 3);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var3", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) || (tmp_var->type != T_DATETIME))
    {
      return FALSE;
    }

  proc = get_item (proc_decls, 2);
  if (!general_proc_check (glb_stmt, proc, "ProcId_3_", 6, 3))
    {
      return FALSE;
    }
  /*check return type */
  tmp_var = get_item (&(proc->spec.proc.paramsList), 0);
  if ((tmp_var->label != NULL) || (tmp_var->labelLength != 0))
    {
      /* return type not properly encoded */
      return FALSE;
    }
  if ((IS_TABLE (tmp_var->type) == FALSE) ||
      (GET_BASIC_TYPE (tmp_var->type) != 0) ||
      (tmp_var->extra == NULL))
    {
      return FALSE;
    }
  if (!(check_container_field (tmp_var->extra, "f1", T_TEXT) &&
        check_container_field (tmp_var->extra, "f2", T_DATETIME)))
    {
      return FALSE;
    }

  /*check parameters  declarations */
  tmp_var = get_item (&(proc->spec.proc.paramsList), 1);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var1", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) || (tmp_var->type != T_REAL))
    {
      return FALSE;
    }
  tmp_var = get_item (&(proc->spec.proc.paramsList), 2);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var2", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) || (tmp_var->type != T_TEXT))
    {
      return FALSE;
    }
  tmp_var = get_item (&(proc->spec.proc.paramsList), 3);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var3", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) ||
      (GET_BASIC_TYPE (tmp_var->type) != T_UNDETERMINED) ||
      (IS_ARRAY (tmp_var->type) == FALSE))
    {
      return FALSE;
    }

  tmp_var = get_item (&(proc->spec.proc.paramsList), 4);
  tmp_table = tmp_var;
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var4", tmp_var->labelLength) != 0) ||
      (IS_TABLE (tmp_var->type) == FALSE) ||
      (GET_BASIC_TYPE (tmp_var->type) != 0))
    {
      return FALSE;
    }
  if (!(check_container_field (tmp_var->extra, "f1", T_REAL) &&
        check_container_field (tmp_var->extra, "f2", T_UINT32) &&
        check_container_field (tmp_var->extra, "f3", MARK_ARRAY (type))))
    {
      return FALSE;
    }
  tmp_var = get_item (&(proc->spec.proc.paramsList), 5);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var5", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) || (tmp_var->type != T_INT64))
    {
      return FALSE;
    }

  tmp_var = get_item (&(proc->spec.proc.paramsList), 6);
  if ((tmp_var->labelLength != 4) ||
      (strncmp (tmp_var->label, "Var6", tmp_var->labelLength) != 0) ||
      (tmp_var->extra != NULL) ||
      (IS_FIELD (tmp_var->type) == FALSE) ||
      (GET_FIELD_TYPE (tmp_var->type) != T_HIRESTIME))
    {
      return FALSE;
    }


  /* check local declarations */
  if (tmp_table !=
      stmt_find_declaration (proc, "Var4", strlen ("Var4"), FALSE, FALSE))
    {
      return FALSE;                /* no transparency between local vars and parameters */
    }

  tmp_var = stmt_find_declaration (proc, "f1", strlen ("f1"), FALSE, FALSE);
  if (tmp_var->type != T_TEXT)
    {
      return FALSE;
    }
  tmp_var = stmt_find_declaration (proc, "f2", strlen ("f2"), FALSE, FALSE);
  if (tmp_var->type != T_REAL)
    {
      return FALSE;
    }

  tmp_var = stmt_find_declaration (proc, "f3", strlen ("f3"), FALSE, FALSE);
  if ((IS_FIELD(tmp_var->type) == FALSE) ||
       (IS_ARRAY (GET_FIELD_TYPE (tmp_var->type)) == FALSE) ||
       (GET_BASIC_TYPE (tmp_var->type) != T_REAL))
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

  printf ("Testing procedure declarations...");
  if (check_procs_decl (&state) == FALSE)
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
