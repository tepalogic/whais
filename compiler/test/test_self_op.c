#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"
#include "../semantics/opcodes.h"
#include "../semantics/procdecl.h"

#include "utils/endianness.h"

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
  "PROCEDURE ProcId1 (v1 AS TABLE OF ( field_1 AS DATE)) RETURN FIELD "
  "DO "
  "RETURN v1.field_1; "
  "ENDPROC\n\n"
  ""
  "PROCEDURE ProcId2 (v1 AS TABLE OF (field_2 AS DATE)) RETURN DATE "
  "DO "
  "RETURN v1[0, field_2]; "
  "ENDPROC\n\n";

static bool_t
check_procedure_1 (struct ParserState *pState, char* proc_name)
{
  struct Statement* pStmt    = find_proc_decl (pState, proc_name, strlen (proc_name), FALSE);
  uint8_t*          pCode    = wh_ostream_data (stmt_query_instrs( pStmt));
  int             codeSize = wh_ostream_size (stmt_query_instrs( pStmt));
  enum W_OPCODE     opExpect = W_SELF;

  /* check the opcode based on the return type */
  if (codeSize < 8)
    {
      return FALSE;
    }
  else if (decode_opcode( pCode + 2) != opExpect)
    {
      return FALSE;
    }
  else
    {
      uint32_t       index  = load_le_int32 (pCode + 3);
      const uint8_t* buffer = wh_ostream_data (&pState->globalStmt.spec.glb.constsArea);

      buffer += index;
      if (strcmp( (const char *) buffer, "field_1") != 0)
        return FALSE;
    }

  return TRUE;
}

static bool_t
check_procedure_2 (struct ParserState *pState, char* proc_name)
{
  struct Statement* pStmt    = find_proc_decl (pState, proc_name, strlen (proc_name), FALSE);
  uint8_t*          pCode    = wh_ostream_data (stmt_query_instrs( pStmt));
  int             codeSize = wh_ostream_size (stmt_query_instrs( pStmt));
  enum W_OPCODE     opExpect = W_INDTA;

  /* check the opcode based on the return type */
  if (codeSize < 10)
    {
      return FALSE;
    }
  else if (decode_opcode( pCode + 4) != opExpect)
    {
      return FALSE;
    }
  else
    {
      uint32_t       index  = load_le_int32 (pCode + 5);
      const uint8_t* buffer = wh_ostream_data (&pState->globalStmt.spec.glb.constsArea);

      buffer += index;
      if (strcmp( (const char *) buffer, "field_2") != 0)
        return FALSE;
    }

  return TRUE;
}

static bool_t
check_all_procs (struct ParserState *pState)
{
  return check_procedure_1 (pState, "ProcId1") &&
         check_procedure_2 (pState, "ProcId2");
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

  printf ("Testing field select op usage ...");
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
