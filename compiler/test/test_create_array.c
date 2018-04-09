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
  "PROCEDURE ProcId0(v1 DATE) RETURN INT8 "
  "DO "
  "   a = {v1}; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId1(v1 DATE, v2 HIRESTIME) RETURN UINT16 "
  "DO "
  "   a = {v1, v2}; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId2(v1 DATE, v2 HIRESTIME) RETURN UINT16 "
  "DO "
  "   a = {v1, v2} DATETIME; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId3(v1 DATE, v2 HIRESTIME) RETURN UINT16 "
  "DO "
  "   a = {v2, v1}; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId4(v1 DATE, v2 HIRESTIME) RETURN UINT16 "
  "DO "
  "   a = {v2, v1} DATE; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId5(t TABLE (f1 DATE, f2 HIRESTIME)) RETURN UINT16 "
  "DO "
  "   a = {t.f1}; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId6(t TABLE (f1 DATE, f2 HIRESTIME)) RETURN UINT16 "
  "DO "
  "   a = {t.f1, t.f2}; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId7(t TABLE (f1 DATE, f2 HIRESTIME)) RETURN UINT16 "
  "DO "
  "   a = {t.f1, t.f2} INT8; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId8(t TABLE(f1 DATE, f2 HIRESTIME)) RETURN UINT16 "
  "DO "
  "   a = {t.f1, t.f2} INT16; "
  "RETURN NULL; "
  "ENDPROC\n\n"
  "";

struct CARROpArgs {
  uint8_t type;
  uint16_t count;
  bool_t field;
};

const struct CARROpArgs _expected[] = {
                                          {T_DATE, 1, FALSE},
                                          {T_HIRESTIME, 2, FALSE},
                                          {T_DATETIME, 2, FALSE},
                                          {T_HIRESTIME, 2, FALSE},
                                          {T_DATE, 2, FALSE},
                                          {T_UINT16, 1, TRUE},
                                          {T_UINT16, 2, TRUE},
                                          {T_INT8, 2, TRUE},
                                          {T_INT16, 2, TRUE},
                                          {0, }
                                       };
static bool_t
check_procedure(struct ParserState* state,
                 const char*       proc_name,
                 const struct CARROpArgs e)
{
  struct Statement *stmt = find_proc_decl(state, proc_name, strlen(proc_name), FALSE);
  uint8_t * const code = wh_ostream_data(stmt_query_instrs( stmt));
  const struct DeclaredVar* const v = stmt_find_declaration(stmt, "a", 1, FALSE, FALSE);

  const uint_t LDSIZE = opcode_bytes(W_LDLO8) + 1;
  const uint_t SELFSIZE =  opcode_bytes(W_SELF) + 4;
  const uint_t opCodeOffset = LDSIZE
                              + (e.field
                                 ? (e.count * (LDSIZE + SELFSIZE))
                                 : (e.count * LDSIZE));

  if (e.field)
  {
    if (decode_opcode(code + opCodeOffset) != W_CARR)
      return FALSE;

    if (code[opCodeOffset + opcode_bytes(W_CARR)] != (0x80 | e.type))
      return FALSE;

    if (load_le_int16(code + (opCodeOffset + opcode_bytes(W_CARR) + 1)) != e.count)
      return FALSE;
  }
  else
  {
    if (decode_opcode(code + opCodeOffset) != W_CARR)
      return FALSE;

    if (code[opCodeOffset + opcode_bytes(W_CARR)] != e.type)
      return FALSE;

    if (load_le_int16(code + (opCodeOffset + opcode_bytes(W_CARR) + 1)) != e.count)
      return FALSE;
  }

  if ((v->extra != NULL)
      || IS_FIELD(v->type) || IS_TABLE(v->type)
      || ! IS_ARRAY(v->type)
      || GET_BASE_TYPE(v->type) != e.type)
  {
    return FALSE;
  }

  return TRUE;
}

static bool_t
check_all_procs(struct ParserState *state)
{
  uint_t count;
  char proc_name[25];

  for (count = 0; count < 9; ++count)
    {
      sprintf(proc_name, "ProcId%d", count);
      if (check_procedure( state, proc_name, _expected[count]) == FALSE)
        {
          return FALSE;
        }
    }

  return TRUE;
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

  printf("Testing create array construction usage ...");
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

