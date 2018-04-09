#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"
#include "../semantics/opcodes.h"
#include "../semantics/procdecl.h"

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
  "PROCEDURE ProcId0(v1 INT8, v2 UINT8) RETURN INT8 "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId1(v1 UINT16, v2 INT16) RETURN UINT16 "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId2(v1 INT32, v2 INT32) RETURN INT32 "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId3(v1 INT64, v2 UINT64) RETURN INT64 "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId4(v1 REAL, v2 UINT8) RETURN REAL "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  "PROCEDURE ProcId5(v1 RICHREAL, v2 UINT8) RETURN RICHREAL "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  "PROCEDURE ProcId6(v1 REAL, v2 REAL) RETURN REAL "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  "PROCEDURE ProcId7(v1 REAL, v2 RICHREAL) RETURN REAL "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  "PROCEDURE ProcId8(v1 RICHREAL, v2 REAL) RETURN RICHREAL "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  "PROCEDURE ProcId9(v1 RICHREAL, v2 RICHREAL) RETURN RICHREAL "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  "PROCEDURE ProcId10(v1 TEXT, v2 CHAR) RETURN TEXT "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  "PROCEDURE ProcId11(v1 TEXT, v2 TEXT) RETURN TEXT "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE AProcId0(v1 UINT8 ARRAY, v2 INT32) RETURN UINT8 ARRAY "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"

  "PROCEDURE AProcId1(v1 DATE ARRAY, v2 DATETIME ARRAY) RETURN DATE ARRAY "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"

  "PROCEDURE AProcId2(v1 RICHREAL ARRAY, v2 INT8) RETURN RICHREAL ARRAY "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n"

  "PROCEDURE AProcId3(v1 REAL ARRAY, v2 UINT32 ARRAY) RETURN REAL ARRAY "
  "DO "
  "RETURN v1 += v2; "
  "ENDPROC\n\n";

const enum W_OPCODE _opcodes_expected [] = {
                                              W_SADD,
                                              W_SADD,
                                              W_SADD,
                                              W_SADD,
                                              W_SADD,
                                              W_SADD,
                                              W_SADDRR,
                                              W_SADDRR,
                                              W_SADDRR,
                                              W_SADDRR,
                                              W_SADDC,
                                              W_SADDT
                                            };
static bool_t
check_procedure(struct ParserState* state,
                 const char*       proc_name,
                 const enum W_OPCODE op_expect)
{
  struct Statement *stmt = find_proc_decl(state, proc_name, strlen(proc_name), FALSE);
  uint8_t *code = wh_ostream_data(stmt_query_instrs( stmt));
  int code_size = wh_ostream_size(stmt_query_instrs( stmt));

  const uint_t opcodeOff = 2 * (opcode_bytes(W_LDLO8) + 1);
  uint_t codeSize = opcodeOff + opcode_bytes(op_expect) + opcode_bytes(W_RET);

  if (code_size < codeSize)
    return FALSE;

  else if (decode_opcode(code + opcodeOff) != op_expect)
    return FALSE;

  return TRUE;
}

static bool_t
check_array_procedure(struct ParserState *state,
                      char * proc_name,
                      uint_t retType,
                      bool_t opBArray)
{
  struct Statement *stmt =
    find_proc_decl(state, proc_name, strlen(proc_name), FALSE);
  uint8_t *code = wh_ostream_data(stmt_query_instrs( stmt));
  const uint_t opCodeOffset = 2 * (opcode_bytes(W_LDLO8) + 1);

  if (decode_opcode(code + opCodeOffset) != W_AJOIN)
    return FALSE;

  if (opBArray ^ ((code[opCodeOffset + opcode_bytes(W_AJOIN)] & A_OPB_A_MASK) != 0))
    return FALSE;

  if ((code[opCodeOffset + opcode_bytes(W_AJOIN)] & A_SELF_MASK) == 0)
    return FALSE;

  if (GET_BASE_TYPE(code[opCodeOffset + opcode_bytes(W_AJOIN)]) != retType)
    return FALSE;

  return TRUE;
}

static bool_t
check_all_procs(struct ParserState *state)
{
  uint_t count;
  char proc_name[25];

  bool_t result = TRUE;
  for (count = 0; count < 12; ++count)
    {
      sprintf(proc_name, "ProcId%d", count);
      if (check_procedure( state, proc_name, _opcodes_expected[count]) == FALSE)
        {
          result &=  FALSE;
        }
    }

  result = result & check_array_procedure(state, "AProcId0", T_UINT8, FALSE);
  result = result & check_array_procedure(state, "AProcId1", T_DATE, TRUE);
  result = result & check_array_procedure(state, "AProcId2", T_RICHREAL, FALSE);
  result = result & check_array_procedure(state, "AProcId3", T_REAL, TRUE);

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

  printf("Testing '+=' operator usage ...");
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
