#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "../parser/parser.h"
#include "../semantics/vardecl.h"
#include "../semantics/opcodes.h"
#include "../semantics/procdecl.h"
#include "../semantics/wlog.h"
#include "compiler/whaisc.h"

#include "custom/include/test/test_fmw.h"

extern int yyparse (struct ParserState *);

uint_t last_msg_code = 0xFF, last_msg_type = 0XFF;

static int
get_buffer_line_from_pos (const char *buffer, uint_t buff_pos)
{
  uint_t count = 0;
  int result = 1;

  if (buff_pos == IGNORE_BUFFER_POS)
    {
      return -1;
    }

  while (count < buff_pos)
    {
      if (buffer[count] == '\n')
        {
          ++result;
        }
      else if (buffer[count] == 0)
        {
          assert (0);
        }
      ++count;
    }
  return result;
}

static char *MSG_PREFIX[] = {
  "", "error ", "warning ", "error "
};

void
my_postman (WLOG_FUNC_CONTEXT bag,
            uint_t buff_pos,
            uint_t msg_id,
            uint_t msgType, const char * msgFormat, va_list args)
{
  const char *buffer = (const char *) bag;
  int buff_line = get_buffer_line_from_pos (buffer, buff_pos);

  printf (MSG_PREFIX[msgType]);
  printf ("%d : line %d: ", msg_id, buff_line);
  vprintf (msgFormat, args);
  printf ("\n");

  last_msg_code = msg_id;
  last_msg_type = msgType;
}

char test_prog_0[] = ""
  "PROCEDURE Proc_1 (arg HIRESTIME) RETURN HIRESTIME\n "
  "DO\n " "RETURN arg += arg;\n " "ENDPROC\n ";

char test_prog_1[] = ""
  "PROCEDURE Proc_1 (arg INT8) RETURN INT8\n "
  "DO\n " "RETURN( arg + arg) += 10;\n " "ENDPROC\n ";

char test_prog_2[] = ""
  "PROCEDURE Proc_1 (arg DATE) RETURN DATE\n "
  "DO\n " "RETURN arg -= arg;\n " "ENDPROC\n ";

char test_prog_3[] = ""
  "PROCEDURE Proc_1 (arg INT16) RETURN INT16\n "
  "DO\n " "RETURN( arg + arg) -= 10;\n " "ENDPROC\n ";

char test_prog_4[] = ""
  "PROCEDURE Proc_1 (arg CHAR) RETURN CHAR\n "
  "DO\n " "RETURN arg *= arg;\n " "ENDPROC\n ";

char test_prog_5[] = ""
  "PROCEDURE Proc_1 (arg UINT32) RETURN UINT32\n "
  "DO\n " "RETURN( arg + arg) *= 10;\n " "ENDPROC\n ";

char test_prog_6[] = ""
  "PROCEDURE Proc_1 (arg DATETIME) RETURN DATETIME\n "
  "DO\n " "RETURN arg /= arg;\n " "ENDPROC\n ";

char test_prog_7[] = ""
  "PROCEDURE Proc_1 (arg REAL) RETURN REAL\n "
  "DO\n " "RETURN( arg + arg) /= 10;\n " "ENDPROC\n ";

char test_prog_8[] = ""
  "PROCEDURE Proc_1 (arg DATETIME) RETURN DATETIME\n "
  "DO\n " "RETURN arg %= arg;\n " "ENDPROC\n ";

char test_prog_9[] = ""
  "PROCEDURE Proc_1 (arg UINT8) RETURN UINT8\n "
  "DO\n " "RETURN( arg + arg) %= 10;\n " "ENDPROC\n ";

char test_prog_10[] = ""
  "PROCEDURE Proc_1 (arg TEXT) RETURN TEXT\n "
  "DO\n " "RETURN arg &= arg;\n " "ENDPROC\n ";

char test_prog_11[] = ""
  "PROCEDURE Proc_1 (arg UINT8) RETURN UINT8\n "
  "DO\n " "RETURN( arg + arg) &= 10;\n " "ENDPROC\n ";

char test_prog_12[] = ""
  "PROCEDURE Proc_1 (arg TEXT) RETURN TEXT\n "
  "DO\n " "RETURN arg ^= arg;\n " "ENDPROC\n ";

char test_prog_13[] = ""
  "PROCEDURE Proc_1 (arg UINT8) RETURN UINT8\n "
  "DO\n " "RETURN( arg + arg) ^= 10;\n " "ENDPROC\n ";

char test_prog_14[] = ""
  "PROCEDURE Proc_1 (arg TEXT) RETURN TEXT\n "
  "DO\n " "RETURN arg |= arg;\n " "ENDPROC\n ";

char test_prog_15[] = ""
  "PROCEDURE Proc_1 (arg UINT8) RETURN UINT8\n "
  "DO\n " "RETURN( arg + arg) |= 10;\n " "ENDPROC\n ";


bool_t
test_for_error (const char *test_buffer, uint_t err_expected, uint_t err_type)
{
  WH_COMPILED_UNIT handler;
  bool_t test_result = TRUE;

  last_msg_code = 0xFF, last_msg_type = 0XFF;
  handler = wh_compiler_load (test_buffer,
                            strlen (test_buffer),
                            &my_postman, (WH_MESSENGER_CTXT) test_buffer);

  if (handler != NULL)
    {
      test_result = FALSE;
      wh_compiler_discard (handler);
    }
  else
    {
      if ((last_msg_code != err_expected) || (last_msg_type != err_type))
        {
          test_result = FALSE;
        }
    }

  if (test_get_mem_used () != 0)
    {
      printf ("Current memory usage: %u bytes! It should be 0.",
              test_get_mem_used ());
      test_result = FALSE;
    }
  return test_result;
}

int
main ()
{
  bool_t test_result = TRUE;

  printf ("Testing for received error messages...\n");
  test_result = test_for_error (test_prog_0, MSG_SADD_NA, MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_1, MSG_SADD_ELV,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_2, MSG_SSUB_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_3, MSG_SSUB_ELV,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_4, MSG_SMUL_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_5, MSG_SMUL_ELV,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_6, MSG_SDIV_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_7, MSG_SDIV_ELV,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_8, MSG_SMOD_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_9, MSG_SMOD_ELV,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_10, MSG_SAND_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_11, MSG_SAND_ELV,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_12, MSG_SXOR_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_13, MSG_SXOR_ELV,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_14, MSG_SOR_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_15, MSG_SOR_ELV,
                                                     MSG_ERROR_EVENT);

  if (test_result == FALSE)
    {
      printf ("TEST RESULT: FAIL\n");
      return -1;
    }

  printf ("TEST RESULT: PASS\n");
  return 0;
}
