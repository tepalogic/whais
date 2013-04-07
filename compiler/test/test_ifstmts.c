
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

D_CHAR proc_decl_buffer[] =
  "PROCEDURE proc1() RETURN BOOL "
  "DO "
  "LET a,b AS BOOL; "
  " "
  "IF (a) THEN "
  "a = FALSE; "
  "END "
  " "
  "RETURN b; "
  "ENDPROC\n "
  " "
  "PROCEDURE proc2() RETURN BOOL "
  "DO "
  "LET a,b AS BOOL; "
  " "
  "IF (a) THEN "
  "a = FALSE; "
  "ELSE "
  "b = TRUE; "
  "END "
  " "
  "RETURN b; "
  "ENDPROC\n "
  " "
  "PROCEDURE proc3() RETURN BOOL "
  "DO "
  "LET a,b AS BOOL; "
  " "
  "IF (a) THEN "
  "a = FALSE; "
  "ELSEIF (b) THEN "
  "b = FALSE; "
  "END "
  " "
  "RETURN b; "
  "ENDPROC "
  " "
  "PROCEDURE proc4() RETURN BOOL "
  "DO "
  "LET a,b AS BOOL; "
  " "
  "IF (a) THEN "
  "a = FALSE; "
  "ELSEIF (b) THEN "
  "b = FALSE; "
  "ELSE " "b = a = TRUE; " "END " " " "RETURN b; " "ENDPROC\n " " " " ";

static D_INT32
get_int32 (D_UINT8 * buffer)
{
  D_INT32 value = 0;
  value = buffer[3];
  value <<= 8;
  value += buffer[2];
  value <<= 8;
  value += buffer[1];
  value <<= 8;
  value += buffer[0];

  return value;
}

static D_BOOL
check_procedure_1 (struct ParserState *state, D_CHAR * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  D_UINT8 *code = get_buffer_outstream (stmt_query_instrs (stmt));
  D_INT code_size = get_size_outstream (stmt_query_instrs (stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;

  if (w_opcode_decode (code + 2) != op_expect)
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

  if (w_opcode_decode (code + shift + 2) != W_RET)
    {
      return FALSE;
    }

  return TRUE;
}

static D_BOOL
check_procedure_2 (struct ParserState *state, D_CHAR * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  D_UINT8 *code = get_buffer_outstream (stmt_query_instrs (stmt));
  D_INT code_size = get_size_outstream (stmt_query_instrs (stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;
  int shift_exit = 0;

  if (w_opcode_decode (code + 2) != op_expect)
    {
      return FALSE;
    }

  shift = get_int32 (code + 3);

  if ((shift <= 5) || (code_size <= 3 + shift))
    {
      return FALSE;
    }
  shift -= 5;                        /* jmp uint32 */

  if (w_opcode_decode (code + 2 + shift) != W_JMP)

    {
      return FALSE;
    }

  shift_exit = get_int32 (code + 3 + shift);

  if (w_opcode_decode (code + 2 + shift + shift_exit + 2) != W_RET)
    {
      return FALSE;
    }

  return TRUE;
}

static D_BOOL
check_procedure_3 (struct ParserState *state, D_CHAR * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  D_UINT8 *code = get_buffer_outstream (stmt_query_instrs (stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;
  int if_exit = 0;
  int elseif_skip = 0;

  if (w_opcode_decode (code + 2) != op_expect)
    {
      return FALSE;
    }

  shift = get_int32 (code + 3);
  shift += 2;

  shift -= 5;                        /* jmp uint32 */

  if (w_opcode_decode (code + shift) != W_JMP)

    {
      return FALSE;
    }

  if_exit = get_int32 (code + shift + 1) + shift;

  if (w_opcode_decode (code + if_exit + 2) != W_RET)
    {
      return FALSE;
    }

  shift += 5 + 2;
  if (w_opcode_decode (code + shift) != op_expect)
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

static D_BOOL
check_procedure_4 (struct ParserState *state, D_CHAR * proc_name)
{
  struct Statement *stmt =
    find_proc_decl (state, proc_name, strlen (proc_name), FALSE);
  D_UINT8 *code = get_buffer_outstream (stmt_query_instrs (stmt));
  enum W_OPCODE op_expect = W_JFC;
  int shift = 0;
  int if_exit = 0;
  int elseif_skip = 0;
  int elseif_exit = 0;

  if (w_opcode_decode (code + 2) != op_expect)
    {
      return FALSE;
    }

  shift = get_int32 (code + 3);
  shift += 2;

  shift -= 5;                        /* jmp uint32 */

  if (w_opcode_decode (code + shift) != W_JMP)

    {
      return FALSE;
    }

  if_exit = get_int32 (code + shift + 1) + shift;

  if (w_opcode_decode (code + if_exit + 2) != W_RET)
    {
      return FALSE;
    }

  shift += 5 + 2;
  if (w_opcode_decode (code + shift) != op_expect)
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

static D_BOOL
check_all_procs (struct ParserState *state)
{

  return check_procedure_1 (state, "proc1") &&
    check_procedure_2 (state, "proc2") &&
    check_procedure_3 (state, "proc3") && check_procedure_4 (state, "proc4");
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

  printf ("Testing if statements ...");
  if (check_all_procs (&state))
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
