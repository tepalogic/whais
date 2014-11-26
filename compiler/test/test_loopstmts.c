
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"
#include "../semantics/opcodes.h"
#include "../semantics/procdecl.h"

#include "custom/include/test/test_fmw.h"

extern int yyparse( struct ParserState *);

static void
init_state_for_test( struct ParserState *state, const char * buffer)
{
  state->buffer = buffer;
  state->strings = create_string_store();
  state->bufferSize = strlen( buffer);
  wh_array_init( &state->values, sizeof( struct SemValue));

  init_glbl_stmt( &state->globalStmt);
  state->pCurrentStmt = &state->globalStmt;
}

static void
free_state( struct ParserState *state)
{
  release_string_store( state->strings);
  clear_glbl_stmt( &(state->globalStmt));
  wh_array_clean( &state->values);

}

static bool_t
check_used_vals( struct ParserState *state)
{
  int vals_count = wh_array_count( &state->values);
  while( --vals_count >= 0)
    {
      struct SemValue *val = wh_array_get( &state->values, vals_count);
      if (val->val_type != VAL_REUSE)
        {
          return TRUE;                /* found value still in use */
        }

    }

  return FALSE;                        /* no value in use */
}

char proc_decl_buffer[] =
    "  PROCEDURE proc1() RETURN BOOL"
    "  DO"
    "       LET a,b,c AS BOOL;"
    " "
    "       a  = TRUE; "
    "       WHILE( a) DO "
    "               IF (b) DO "
    "                       BREAK;"
    "               ELSE IF (c) DO "
    "                       CONTINUE; "
    "               END "
    "       END "
    "       RETURN b; "
    " ENDPROC "
    " "
    " PROCEDURE proc1_b() RETURN BOOL"
    " DO"
    "       LET a,b,c AS BOOL;"
    " "
    "       a  = TRUE; "
    "       WHILE( a) DO "
    "               IF (b)"
    "                       BREAK;"
    "               ELSE IF (c) "
    "                       CONTINUE; "
    "       END "
    "       RETURN b; "
    " ENDPROC "
    " "
    " "
    " PROCEDURE proc1_c() RETURN BOOL"
    " DO"
    "       LET a,b,c AS BOOL;"
    " "
    "       a  = TRUE; "
    "       WHILE( a)"
    "               IF (b)"
    "                       BREAK;"
    "               ELSE IF (c) "
    "                       CONTINUE; "
    "       RETURN b; "
    " ENDPROC "
    " "
    " "
    "  PROCEDURE proc2() RETURN BOOL"
    "  DO"
    "   LET a,b,c AS BOOL;"
    "   a  = TRUE;"
    "   DO"
    "          IF (b) DO "
    "              CONTINUE;"
    "          ELSE IF (c) DO "
    "              BREAK; "
    "          END "
    "   UNTIL( a);"
    "   RETURN c;"
    "  ENDPROC "
    " "
    " "
    "  PROCEDURE proc2_b() RETURN BOOL"
    "  DO"
    "   LET a,b,c AS BOOL;"
    "   a  = TRUE;"
    "   DO"
    "          IF (b) "
    "              CONTINUE;"
    "          ELSE IF (c)"
    "              BREAK; "
    "   UNTIL( a);"
    "   RETURN c;"
    "  ENDPROC "
    " "
    " "
    "PROCEDURE proc3 () RETURN TABLE "
    "DO "
    "       LET a,b as UNSIGNED INT8;"
    "       LET c,d AS BOOL;"
    " "
    "       FOR (a = 0; c; b = b + 2)"
    "       DO"
    "               IF (d)"
    "                       CONTINUE;"
    "               ELSE "
    "                       BREAK;"
    "       END"
    "       RETURN NULL; "
    "ENDPROC"
    " "
    "PROCEDURE proc3_b () RETURN TABLE "
    "DO "
    "       LET a,b as UNSIGNED INT8;"
    "       LET c,d AS BOOL;"
    " "
    "       FOR (a = 0; c; b = b + 2)"
    "       DO"
    "               IF (d) DO"
    "                       CONTINUE;"
    "               ELSE DO "
    "                       BREAK;"
    "               END"
    "       END"
    "       RETURN NULL; "
    "ENDPROC"
    " ";


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
    find_proc_decl( state, proc_name, strlen( proc_name), FALSE);
  uint8_t *code = wh_ostream_data( stmt_query_instrs( stmt));
  int shift = 0;
  int cond_exp_pos = 5;
  int while_end_pos = 0;

  if (decode_opcode( code + cond_exp_pos + 2) != W_JFC)
    {
      return FALSE;
    }
  else
    {
      while_end_pos = get_int32 (code + cond_exp_pos + 3);
      while_end_pos += cond_exp_pos + 2;
      if (decode_opcode( code + while_end_pos - 5) != W_JMP)
        {
          return FALSE;
        }
      else
        {
          int jmp_back = get_int32 (code + while_end_pos - 4);
          jmp_back += while_end_pos - 5;
          if (jmp_back != cond_exp_pos)
            {
              return FALSE;
            }
        }
    }

  /* check the break statement */
  shift = cond_exp_pos + 2 + 5 + 2 + 5;
  if (decode_opcode( code + shift) != W_JMP)
    {
      return FALSE;
    }
  shift += get_int32 (code + shift + 1);
  if (shift != while_end_pos)
    {
      return FALSE;
    }

  /* check the continue statement */
  shift = cond_exp_pos + 2 + 5 + 2;
  if (decode_opcode( code + shift) != W_JFC)
    {
      return FALSE;
    }
  shift += get_int32 (code + shift + 1);
  shift += 2;
  if (decode_opcode( code + shift) != W_JFC)
    {
      return FALSE;
    }
  shift += 5;
  if (decode_opcode( code + shift) != W_JMP)
    {
      return FALSE;
    }
  shift += get_int32 (code + shift + 1);
  if (shift != cond_exp_pos)
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_procedure_2 (struct ParserState *state, char * proc_name)
{
  struct Statement *stmt =
    find_proc_decl( state, proc_name, strlen( proc_name), FALSE);
  uint8_t *code = wh_ostream_data( stmt_query_instrs( stmt));
  int shift = 0;
  int cond_exp_pos = 0;

  /* check continue statement */
  shift = 5 + 2;
  if (decode_opcode( code + shift) != W_JFC)
    {
      return FALSE;
    }
  shift += 5;
  if (decode_opcode( code + shift) != W_JMP)
    {
      return FALSE;
    }
  shift += get_int32 (code + shift + 1);
  cond_exp_pos = shift;
  shift += 2;
  if (decode_opcode( code + shift) != W_JTC)
    {
      return FALSE;
    }
  shift += get_int32 (code + shift + 1);
  if (shift != 5)
    {
      return FALSE;
    }

  /* check the break statement */
  shift = cond_exp_pos - 5;
  if (decode_opcode( code + shift) != W_JMP)
    {
      return FALSE;
    }
  shift += get_int32 (code + shift + 1);
  if (shift != (cond_exp_pos + 2 + 5))
    {
      return FALSE;
    }

  return TRUE;
}

static bool_t
check_procedure_3 (struct ParserState *state, char * proc_name)
{
  struct Statement *stmt =
    find_proc_decl( state, proc_name, strlen( proc_name), FALSE);
  uint8_t *code = wh_ostream_data( stmt_query_instrs( stmt));
  int shift = 0;
  int cond_exp_pos = 6;
  int step_exp_pos = 18;
  int for_end_pos  = 0;

  if (decode_opcode( code + cond_exp_pos - 1) != W_CTS)
    return FALSE;
  else if (decode_opcode( code + cond_exp_pos + 2) != W_JFC)
    return FALSE;
  else
    {
      for_end_pos = get_int32 (code + cond_exp_pos + 3)
                    + cond_exp_pos + 2;
      if (decode_opcode( code + for_end_pos - 5) != W_JMP)
        return FALSE;
      else
        {
          const int jmp_back = get_int32 (code + for_end_pos - 4)
                               + for_end_pos - 5;
          if (jmp_back != step_exp_pos)
            return FALSE;
        }
    }

  if (decode_opcode( code + cond_exp_pos + 2 + 5) != W_JMP)
    return FALSE;
  else
    {
      /* Check if the jump is exactly after the step expression */
      if (get_int32 (code + cond_exp_pos + 2 + 5 + 1) != 19)
        return FALSE;
    }

  /* check the break statement */
  shift = 49;
  if (decode_opcode( code + shift) != W_JMP)
    return FALSE;
  shift += get_int32 (code + shift + 1);
  if (shift != for_end_pos)
    return FALSE;

  /* check the continue statement */
  shift = 39;
  if (decode_opcode( code + shift) != W_JMP)
    return FALSE;
  shift += get_int32 (code + shift + 1);
  if (shift != step_exp_pos)
    return FALSE;

  return TRUE;
}

static bool_t
check_all_procs( struct ParserState *state)
{
  return check_procedure_1 (state, "proc1")
         && check_procedure_1 (state, "proc1_b")
         && check_procedure_1 (state, "proc1_c")
         && check_procedure_2 (state, "proc2")
         && check_procedure_2 (state, "proc2_b")
         && check_procedure_3 (state, "proc3")
         && check_procedure_3 (state, "proc3_b");
}

int
main()
{
  bool_t test_result = TRUE;
  struct ParserState state = { 0, };

  init_state_for_test( &state, proc_decl_buffer);

  printf( "Testing parse..");
  if (yyparse( &state) != 0)
    {
      printf( "FAILED\n");
      test_result = FALSE;
    }
  else
    {
      printf( "PASSED\n");
    }

  if (test_result)
    {
      printf( "Testing garbage vals...");
      if (check_used_vals( &state))
        {
          /* those should no be here */
          printf( "FAILED\n");
          test_result = FALSE;
        }
      else
        {
          printf( "PASSED\n");
        }
    }

  printf( "Testing loop statements ...");
  if (check_all_procs( &state))
    {
      printf( "PASSED\n");
    }
  else
    {
      printf( "FAILED\n");
      test_result = FALSE;
    }

  free_state( &state);
  printf( "Memory peak: %u bytes \n", test_get_mem_peak());
  printf( "Current memory usage: %u bytes...", test_get_mem_used());
  if (test_get_mem_used() != 0)
    {
      test_result = FALSE;
      printf( "FAILED\n");
    }
  else
    {
      printf( "PASSED\n");
    }

  if (test_result == FALSE)
    {
      printf( "TEST RESULT: FAIL\n");
      return -1;
    }

  printf( "TEST RESULT: PASS\n");
  return 0;
}
