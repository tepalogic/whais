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
  "", "error ", "warning ", "error ", "extra "
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

  if (msgType != MSG_EXTRA_EVENT)
    {
      last_msg_code = msg_id;
      last_msg_type = msgType;
    }
}

char test_prog_1[] = ""
  "PROCEDURE Proc_1 () RETURN BOOL\n "
  "DO\n "
  "LET v1 as BOOL;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_2() RETURN INT32\n "
  "DO\n "
  "LET v1 as INT32;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_1 () RETURN BOOL\n "
  "DO\n " "LET v2 as BOOL;\n " "RETURUN v2;\n " "ENDPROC\n ";

char test_prog_2[] = ""
  "PROCEDURE Proc_1 () RETURN BOOL\n "
  "DO\n "
  "LET v1 as BOOL;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_2() RETURN INT32\n "
  "DO\n "
  "LET v1 as INT32;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_1 () RETURN DATE\n "
  "DO\n " "LET v2 as DATE;\n " "RETURN v2;\n " "ENDPROC\n ";

char test_prog_3[] = ""
  "PROCEDURE Proc_1 () RETURN BOOL\n "
  "DO\n "
  "LET v1 as BOOL;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_2() RETURN INT32\n "
  "DO\n "
  "LET v1 as INT32;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_1 (v2 AS DATE) RETURN BOOL\n "
  "DO\n " "RETURN TRUE;\n " "ENDPROC\n ";

char test_prog_4[] = ""
  "PROCEDURE Proc_1 (v1 as DATE) RETURN BOOL\n "
  "DO\n "
  "LET v_t as BOOL;\n "
  "RETURN v_t;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_2() RETURN INT32\n "
  "DO\n "
  "LET v1 as INT32;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_1 (v2 AS DATE) RETURN BOOL\n "
  "DO\n " "RETURN TRUE;\n " "ENDPROC\n " "\n ";


char test_prog_5[] = ""
  "EXTERN PROCEDURE Proc_2 () RETURN INT32;\n "
  "PROCEDURE Proc_1 (v1 as DATE) RETURN BOOL\n "
  "DO\n "
  "LET v_t as BOOL;\n "
  "RETURN v_t;\n "
  "ENDPROC\n "
  "\n "
  "PROCEDURE Proc_2() RETURN INT32\n "
  "DO\n "
  "LET v1 as INT32;\n "
  "RETURN v1;\n "
  "ENDPROC\n "
  "\n "
  "EXTERN PROCEDURE Proc_1 (v1 AS DATE) RETURN BOOL;\n ";

  char test_prog_6[] = ""
    "EXTERN PROCEDURE Proc_2 () RETURN ARRAY;\n "
    "PROCEDURE Proc_1 (v1 as DATE) RETURN BOOL\n "
    "DO\n "
    "LET v_t as BOOL;\n "
    "RETURN v_t;\n "
    "ENDPROC\n "
    "\n "
    "PROCEDURE Proc_2() RETURN INT32\n "
    "DO\n "
    "LET v1 as INT32;\n "
    "RETURN v1;\n "
    "ENDPROC\n "
    "\n ";

  char test_prog_7[] = ""
    "EXTERN PROCEDURE Proc_2 (a AS INT16) RETURN INT32;\n "
    "PROCEDURE Proc_1 (v1 as DATE) RETURN BOOL\n "
    "DO\n "
    "LET v_t as BOOL;\n "
    "RETURN v_t;\n "
    "ENDPROC\n "
    "\n "
    "PROCEDURE Proc_2() RETURN INT32\n "
    "DO\n "
    "LET v1 as INT32;\n "
    "RETURN v1;\n "
    "ENDPROC\n "
    "\n ";

  char test_prog_8[] = ""
    "EXTERN PROCEDURE Proc_2 () RETURN INT32;\n "
    "PROCEDURE Proc_1 (v1 as DATE) RETURN BOOL\n "
    "DO\n "
    "LET v_t as BOOL;\n "
    "RETURN v_t;\n "
    "ENDPROC\n "
    "\n "
    "PROCEDURE Proc_2(a AS INT16) RETURN INT32\n "
    "DO\n "
    "LET v1 as INT32;\n "
    "RETURN v1;\n "
    "ENDPROC\n "
    "\n ";

  char test_prog_9[] = ""
    "EXTERN PROCEDURE Proc_2 (b AS TEXT, c AS DATE) RETURN INT32;\n "
    "PROCEDURE Proc_1 (v1 as DATE) RETURN BOOL\n "
    "DO\n "
    "LET v_t as BOOL;\n "
    "RETURN v_t;\n "
    "ENDPROC\n "
    "\n "
    "PROCEDURE Proc_2(b AS TEXT, c AS DATETIME) RETURN INT32\n "
    "DO\n "
    "LET v1 as INT32;\n "
    "RETURN v1;\n "
    "ENDPROC\n "
    "\n ";

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
      wh_compiler_discard (handler);

      if ((last_msg_type != MSG_WARNING_EVENT)
          || (last_msg_type != err_type)
          || (last_msg_code != err_expected))
        {
          test_result = FALSE;
        }
    }
  else
    {
      if ((last_msg_code != err_expected) || (last_msg_type != err_type))
        {
          test_result = FALSE;
        }
    }

  if (test_get_mem_used() != 0)
    {
      printf ("Current memory usage: %u bytes! It should be 0.",
              test_get_mem_used());
      test_result = FALSE;
    }
  return test_result;
}

int
main()
{
  bool_t test_result = TRUE;

  printf ("Testing for received error messages...\n");
  test_result = test_for_error (test_prog_1, MSG_PROC_ADECL, MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_2,
                                                     MSG_PROC_ADECL,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_3,
                                                     MSG_PROC_ADECL,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_4,
                                                     MSG_PROC_ADECL,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_5,
                                                     MSG_PROC_EXT_LATE,
                                                     MSG_WARNING_EVENT);

  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_6,
                                                     MSG_PROC_DECL_RET_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_7,
                                                     MSG_PROC_DECL_LESS,
                                                     MSG_ERROR_EVENT);

   (test_result == FALSE) ? FALSE : test_for_error (test_prog_8,
                                                     MSG_PROC_DECL_MORE,
                                                     MSG_ERROR_EVENT);

   (test_result == FALSE) ? FALSE : test_for_error (test_prog_9,
                                                     MSG_PROC_DECL_PARAM_NA,
                                                     MSG_ERROR_EVENT);
  if (test_result == FALSE)
    {
      printf ("TEST RESULT: FAIL\n");
      return -1;
    }

  printf ("TEST RESULT: PASS\n");
  return 0;
}
