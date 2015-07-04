
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
    "      PROCEDURE proc1 () RETURN BOOL "
    "      DO "
    "             VAR a,b AS BOOL; "
    "              "
    "             IF (a) DO"
    "                   a = FALSE; "
    "             END"
    "       "
    "             RETURN b; "
    "    "
    "      ENDPROC "
    "    "
    "      PROCEDURE proc1_b () RETURN BOOL "
    "      DO "
    "             VAR a,b AS BOOL; "
    "              "
    "             IF (a) "
    "                   a = FALSE; "
    "       "
    "             RETURN b; "
    "    "
    "      ENDPROC "
    "       "
    "      PROCEDURE proc2 () RETURN BOOL "
    "      DO "
    "             VAR a,b AS BOOL; "
    "              "
    "             IF (a) DO "
    "                     a = FALSE;"
    "             ELSE DO"
    "                     b = TRUE; "
    "             END"
    "              "
    "             RETURN b; "
    "      ENDPROC "
    "    "
    "      PROCEDURE proc2_b () RETURN BOOL "
    "      DO "
    "             VAR a,b AS BOOL; "
    "              "
    "             IF (a) "
    "                     a = FALSE; "
    "             ELSE "
    "                     b = TRUE; "
    "              "
    "             RETURN b; "
    "      ENDPROC"
    "       "
    "      PROCEDURE proc3 () RETURN BOOL "
    "      DO "
    "             VAR a,b AS BOOL; "
    "              "
    "             IF (a) DO "
    "                     a = FALSE; "
    "             ELSE IF (b) DO "
    "                   b = FALSE; "
    "             END "
    "              "
    "             RETURN b; "
    "      ENDPROC "
    "    "
    "      PROCEDURE proc3_b () RETURN BOOL "
    "      DO "
    "             VAR a,b AS BOOL; "
    "              "
    "             IF (a) "
    "                     a = FALSE; "
    "             ELSE IF (b) "
    "                   b = FALSE; "
    "              "
    "             RETURN b; "
    "      ENDPROC "
    "       "
    "      PROCEDURE proc4 () RETURN BOOL "
    "      DO "
    "           VAR a,b AS BOOL; "
    "       "
    "           IF (a) DO "
    "                   a = FALSE; "
    "           ELSE IF (b) DO "
    "                   b = FALSE; "
    "           ELSE DO"
    "                   b = a = TRUE;"
    "           END"
    "           RETURN b;"
    "      ENDPROC  "
    "    "
    "      PROCEDURE proc4_b () RETURN BOOL "
    "      DO "
    "           VAR a,b AS BOOL; "
    "       "
    "           IF (a) "
    "                   a = FALSE; "
    "           ELSE IF (b) "
    "                   b = FALSE; "
    "           ELSE "
    "                   b = a = TRUE;"
    "    "
    "           RETURN b;"
    "    "
    "      ENDPROC  "
    "";

static int32_t
get_int32 (uint8_t * buffer)
{
  int32_t value = 0;
  value = buffer[3];
  value <<= 8;
  value += buffer[2];
  value <<= 8;
  value += buffer[1];
  value <<= 8;
  value += buffer[0];

  return value;
}

static bool_t
check_procedure_1 (struct ParserState *state, char * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  uint8_t *code = wh_ostream_data (stmt_query_instrs( stmt));
  int code_size = wh_ostream_size (stmt_query_instrs( stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;

  if (decode_opcode( code + 2) != op_expect)
    {
      return FALSE;
    }
  shift = get_int32 (code + 3);

  if ((shift <= 5) || (code_size <= 3 + shift))
    {
      return FALSE;
    }
  else
    {
      shift += 2;
    }

  if (decode_opcode( code + shift + 2) != W_RET)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_procedure_2 (struct ParserState *state, char * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  uint8_t *code = wh_ostream_data (stmt_query_instrs( stmt));
  int code_size = wh_ostream_size (stmt_query_instrs( stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;
  int shift_exit = 0;

  if (decode_opcode( code + 2) != op_expect)
    {
      return FALSE;
    }

  shift = get_int32 (code + 3);

  if ((shift <= 5) || (code_size <= 3 + shift))
    {
      return FALSE;
    }
  shift -= 5;                        /* jmp uint32 */

  if (decode_opcode( code + 2 + shift) != W_JMP)

    {
      return FALSE;
    }

  shift_exit = get_int32 (code + 3 + shift);

  if (decode_opcode( code + 2 + shift + shift_exit + 2) != W_RET)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_procedure_3 (struct ParserState *state, char * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  uint8_t *code = wh_ostream_data (stmt_query_instrs( stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;
  int if_exit = 0;
  int elseif_skip = 0;

  if (decode_opcode( code + 2) != op_expect)
    {
      return FALSE;
    }

  shift = get_int32 (code + 3);
  shift += 2;

  shift -= 5;                        /* jmp uint32 */

  if (decode_opcode( code + shift) != W_JMP)

    {
      return FALSE;
    }

  if_exit = get_int32 (code + shift + 1) + shift;

  if (decode_opcode( code + if_exit + 2) != W_RET)
    {
      return FALSE;
    }

  shift += 5 + 2;
  if (decode_opcode( code + shift) != op_expect)
    {
      return FALSE;
    }

  elseif_skip = get_int32 (code + shift + 1) + shift;
  if (elseif_skip != if_exit)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_procedure_4 (struct ParserState *state, char * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  uint8_t *code = wh_ostream_data (stmt_query_instrs( stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;
  int if_exit = 0;
  int elseif_skip = 0;
  int elseif_exit = 0;

  if (decode_opcode( code + 2) != op_expect)
    {
      return FALSE;
    }

  shift = get_int32 (code + 3);
  shift += 2;

  shift -= 5;                        /* jmp uint32 */

  if (decode_opcode( code + shift) != W_JMP)

    {
      return FALSE;
    }

  if_exit = get_int32 (code + shift + 1) + shift;

  if (decode_opcode( code + if_exit + 2) != W_RET)
    {
      return FALSE;
    }

  shift += 5 + 2;
  if (decode_opcode( code + shift) != op_expect)
    {
      return FALSE;
    }

  elseif_skip = get_int32 (code + shift + 1) + shift;
  elseif_skip -= 5;
  elseif_exit = get_int32 (code + elseif_skip + 1) + elseif_skip;

  if (elseif_exit != if_exit)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_all_procs (struct ParserState *state)
{

  return check_procedure_1 (state, "proc1")
         && check_procedure_2 (state, "proc2")
         && check_procedure_2 (state, "proc2_b")
         && check_procedure_3 (state, "proc3")
         && check_procedure_3 (state, "proc3_b")
         && check_procedure_4 (state, "proc4")
         && check_procedure_4 (state, "proc4_b");
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

  printf ("Testing if statements ...");
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
