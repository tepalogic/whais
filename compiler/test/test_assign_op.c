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
  "VAR tab_glb TABLE ( field_t DATE); "
  "VAR tab_glb2 TABLE ( field2 TEXT, field_t DATE); "
  "PROCEDURE ProcId1 (v1 CHAR, v2 CHAR) RETURN CHAR "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId2 (v1 INT8, v2 INT8) RETURN INT8 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId3 (v1 INT8, v2 INT16) RETURN INT8 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId4 (v1 INT8, v2 INT32) RETURN INT8 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId5 (v1 INT8, v2 INT64) RETURN INT8 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId6 (v1 REAL, v2 INT8) RETURN REAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId7 (v1 RICHREAL, v2 INT8) RETURN RICHREAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId8 (v1 INT16, v2 INT16) RETURN INT16 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId9 (v1 INT16, v2 INT32) RETURN INT16 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId10 (v1 INT16, v2 INT64) RETURN INT16 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId11 (v1 REAL, v2 INT16) RETURN REAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId12 (v1 RICHREAL, v2 INT16) RETURN RICHREAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId13 (v1 INT32, v2 INT32) RETURN INT32 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId14 (v1 INT32, v2 INT64) RETURN INT32 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId15 (v1 REAL, v2 INT32) RETURN REAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId16 (v1 RICHREAL, v2 INT32) RETURN RICHREAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId17 (v1 INT64, v2 INT64) RETURN INT64 "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId18 (v1 REAL, v2 INT64) RETURN REAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId19 (v1 RICHREAL, v2 INT64) RETURN RICHREAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId20 (v1 REAL, v2 REAL) RETURN REAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId21 (v1 REAL, v2 RICHREAL) RETURN REAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId22 (v1 RICHREAL, v2 RICHREAL) RETURN RICHREAL "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId23 (v1 TEXT, v2 TEXT) RETURN TEXT "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId24 (v1 DATE, v2 DATE) RETURN DATE "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId25 (v1 DATE, v2 DATETIME) RETURN DATE "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId26 (v1 DATE, v2 HIRESTIME) RETURN DATE "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId27 (v1 DATETIME, v2 DATETIME) RETURN DATETIME "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId28 (v1 DATETIME, v2 HIRESTIME) RETURN DATETIME "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId29 (v1 HIRESTIME, v2 HIRESTIME) RETURN HIRESTIME "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId30 (v1 TABLE, v2 TABLE) RETURN TABLE "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId31 (v1 TABLE (field_t DATE, f2 TEXT), v2 TABLE (field_t DATE, f2 TEXT)) RETURN TABLE "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId32 (v1 ARRAY, v2 ARRAY) RETURN ARRAY "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId33 (v1 ARRAY, v2 DATETIME ARRAY) RETURN ARRAY "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId34 (v1 FIELD, v2 DATETIME ARRAY FIELD) RETURN FIELD "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId35 (v1 DATETIME ARRAY FIELD, v2 DATETIME ARRAY FIELD) RETURN DATETIME ARRAY FIELD "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId36 (v1 FIELD, v2 TEXT FIELD) RETURN FIELD "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId37 (v1 TEXT FIELD, v2 TEXT FIELD) RETURN FIELD "
  "DO "
  "RETURN v1 = v2; "
  "ENDPROC\n\n"
  "";


static bool_t
check_procedure (struct ParserState *state, char * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  struct DeclaredVar *v1 = stmt_find_declaration (stmt, "v1",
                                                  strlen ("v1"),
                                                  FALSE,
                                                  FALSE);
  uint8_t *code = wh_ostream_data (stmt_query_instrs( stmt));
  int code_size = wh_ostream_size (stmt_query_instrs( stmt));
  enum W_OPCODE op_expect = W_NA;

  /* check the opcode based on the return type */
  switch (v1->type)
    {
    case T_CHAR:
      op_expect = W_STC;
      break;
    case T_DATE:
      op_expect = W_STD;
      break;
    case T_DATETIME:
      op_expect = W_STDT;
      break;
    case T_HIRESTIME:
      op_expect = W_STHT;
      break;
    case T_INT8:
      op_expect = W_STI8;
      break;
    case T_INT16:
      op_expect = W_STI16;
      break;
    case T_INT32:
      op_expect = W_STI32;
      break;
    case T_INT64:
      op_expect = W_STI64;
      break;
    case T_REAL:
      op_expect = W_STR;
      break;
    case T_RICHREAL:
      op_expect = W_STRR;
      break;
    case T_TEXT:
      op_expect = W_STT;
      break;
    default:
      if (IS_TABLE( v1->type))
        op_expect = W_STTA;
      else if (IS_FIELD( v1->type))
        op_expect = W_STF;
      else if (IS_ARRAY( v1->type))
        op_expect = W_STA;
      else
        {
          /* we should not be here */
          assert (0);
          return FALSE;
        }
    }

  if (code_size < 5)
    {
      return FALSE;
    }
  else if (decode_opcode( code + 4) != op_expect)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_all_procs (struct ParserState *state)
{
  uint_t count;
  char proc_name[25];

  for (count = 1; count <= 37; ++count)
    {
      sprintf (proc_name, "ProcId%d", count);
      if (check_procedure( state, proc_name) == FALSE)
        {
          return FALSE;
        }
    }

  return TRUE;
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

  printf ("Testing le op usage ...");
  if (check_all_procs( &state))
    {
      printf ("PASSED\n");
    }
  else
    {
      printf ("FAILED\n");
      test_result = FALSE;
    }

  free_state (&state);
  printf ("Memory peak: %u bytes \n", (uint_t)test_get_mem_peak ());
  printf ("Current memory usage: %u bytes...", (uint_t)test_get_mem_used ());
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
