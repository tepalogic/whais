#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"
#include "../semantics/opcodes.h"
#include "../semantics/procdecl.h"
#include "../semantics/statement.h"

#include "custom/include/test/test_fmw.h"

extern int yyparse(struct ParserState *);

static void
init_state_for_test(struct ParserState *state, const char * buffer)
{
  state->buffer = buffer;
  state->strings = create_string_store();
  state->bufferSize = strlen(buffer);
  wh_array_init(&state->values, sizeof(struct SemValue));

  init_glbl_stmt(&state->globalStmt);
  state->pCurrentStmt = &state->globalStmt;
}

static void
free_state(struct ParserState *state)
{
  release_string_store(state->strings);
  clear_glbl_stmt(&(state->globalStmt));
  wh_array_clean(&state->values);
}

static bool_t
check_used_vals(struct ParserState *state)
{
  int vals_count = wh_array_count(&state->values);
  while (--vals_count >= 0)
    {
      struct SemValue *val = wh_array_get(&state->values, vals_count);
      if (val->val_type != VAL_REUSE)
        {
          return TRUE;                /* found value still in use */
        }

    }

  return FALSE;                        /* no value in use */
}

char proc_decl_buffer[] =
  "PROCEDURE ProcId0(v1 UINT8, v2 UINT8) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId1(v1 INT16, v2 INT8) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId2(v1 CHAR, v2 CHAR) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId3(v1 UINT8, v2 UINT32) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId4(v1 BOOL, v2 BOOL) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId5(v1 DATE, v2 HIRESTIME) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId6(v1 DATE, v2 DATETIME) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId7(v1 INT64, v2 INT32) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId8(v1 INT32, v2 INT16) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId9(v1 REAL, v2 RICHREAL) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId10(v1 REAL, v2 REAL) RETURN BOOL "
  "DO "
  "a = 1 == 0 ? v1 : v2; "
  "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId11(v1 TEXT, v2 TEXT) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId12(v1 INT16, v2 DATETIME) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? NULL : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId13(v1 INT64, v2 TEXT) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : NULL; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId14(v1 FIELD, v2 TEXT) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : NULL; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"
  "PROCEDURE ProcId15(v1 INT64, v2 TABLE) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? NULL : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"

  "PROCEDURE ProcId16(v1 ARRAY, v2 INT64 ARRAY) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"

  "PROCEDURE ProcId17(v1 INT64 ARRAY, v2 INT64 ARRAY) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"

  "PROCEDURE ProcId18(v1 DATE ARRAY FIELD, v2 DATE ARRAY FIELD) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"

  "PROCEDURE ProcId19(v1 FIELD, v2 HIRESTIME FIELD) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"

  "PROCEDURE ProcId20(v1 TEXT FIELD, v2 TEXT FIELD) RETURN BOOL "
    "DO "
    "a = 1 == 0 ? v1 : v2; "
    "RETURN a == NULL;"
  "ENDPROC\n\n"
  "";

const uint_t typesExpected[] = {
                                T_UINT8,
                                T_INT16,
                                T_CHAR,
                                T_UINT32,
                                T_BOOL,
                                T_HIRESTIME,
                                T_DATETIME,
                                T_INT64,
                                T_INT32,
                                T_RICHREAL,
                                T_REAL,
                                T_TEXT,
                                T_DATETIME,
                                T_INT64,
                                T_FIELD_MASK | T_UNDETERMINED,
                                T_TABLE_MASK,
                                T_ARRAY_MASK | T_UNDETERMINED,
                                T_ARRAY_MASK | T_INT64,
                                T_FIELD_MASK | T_ARRAY_MASK | T_DATE,
                                T_FIELD_MASK | T_UNDETERMINED,
                                T_FIELD_MASK | T_TEXT
                            };


static bool_t
check_procedure(struct ParserState* state,
                const char*       proc_name,
                const uint_t    expected_type)
{
  struct Statement *stmt = find_proc_decl(state, proc_name, strlen(proc_name), FALSE);
  uint8_t *code = wh_ostream_data(stmt_query_instrs( stmt));
  int code_size = wh_ostream_size(stmt_query_instrs( stmt));

  const struct DeclaredVar* const localValue = wh_array_get(&stmt->decls, 0);

  static uint8_t code_expected[] = {
                                  0x0E, 0x02,
                                  0x03, 0x01,
                                  0x03, 0x00,
                                  0x35,
                                  0x6F, 0x0C, 0x00, 0x00, 0x00,
                                  0x0E, 0x00,
                                  0x72, 0x07, 0x00, 0x00, 0x00,
                                  0x0E, 0x01,
                                  0x15,
                                  0x14, 0x01,
                                  0x0E, 0x02,
                                  0x29,
                                  0x2C
                              };


  if (expected_type == T_BOOL
      && ((memcmp(code, code_expected, sizeof code_expected) != 0)
          || (code_size != sizeof code_expected)))
  {
    return FALSE;
  }

  if (localValue->type != expected_type)
    return FALSE;

  return TRUE;
}

static bool_t
check_all_procs(struct ParserState *state)
{
  const uint_t procsCount = sizeof (typesExpected) / sizeof (typesExpected[0]);
  bool_t result = TRUE;
  int i = 0;

  for (i = 0; i < procsCount; ++i)
  {
    char procName[32];

    sprintf(procName, "ProcId%i", i);
    result &= check_procedure(state, procName, typesExpected[i]);
  }

  return result;
}

int
main()
{
  bool_t test_result = TRUE;
  struct ParserState state = { 0, };

  init_state_for_test(&state, proc_decl_buffer);

  printf("Testing parse..");
  if (yyparse( &state) != 0)
    {
      printf("FAILED\n");
      test_result = FALSE;
    }
  else
    {
      printf("PASSED\n");
    }

  if (test_result)
    {
      printf("Testing garbage vals...");
      if (check_used_vals( &state))
        {
          /* those should no be here */
          printf("FAILED\n");
          test_result = FALSE;
        }
      else
        {
          printf("PASSED\n");
        }
    }

  printf("Testing '?=' operator usage ...");
  if (check_all_procs( &state))
    {
      printf("PASSED\n");
    }
  else
    {
      printf("FAILED\n");
      test_result = FALSE;
    }

  free_state(&state);
  printf("Memory peak: %u bytes \n", (uint_t)test_get_mem_peak());
  printf("Current memory usage: %u bytes...",  (uint_t)test_get_mem_used());
  if (test_get_mem_used() != 0)
    {
      test_result = FALSE;
      printf("FAILED\n");
    }
  else
    {
      printf("PASSED\n");
    }

  if (test_result == FALSE)
    {
      printf("TEST RESULT: FAIL\n");
      return -1;
    }

  printf("TEST RESULT: PASS\n");
  return 0;
}

/*
 * test_sxor_op.c
 *
 *  Created on: Jul 6, 2012
 *      Author: iupo
 */

