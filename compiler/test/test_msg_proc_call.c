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

  if (msgType != MSG_EXTRA_EVENT)
    {
      last_msg_code = msg_id;
      last_msg_type = msgType;
    }
}

char test_prog_1[] = ""
  "LET table_2 AS TABLE OF ( f1 as DATE); \n"
  " \n"
  "PROCEDURE Proc_1_2 () RETURN DATE \n"
  "DO \n"
  "Proc_1 ( 0 ); \n"
  "RETURN table_2[10, f1]; \n"
  "ENDPROC \n";

char test_prog_2[] = ""
  "LET table_1 AS TABLE OF ( f1 as DATE, f2 as INT16); \n"
  " \n"
  "PROCEDURE Proc_1 ( proc_arg AS INT16) RETURN INT16 \n"
  "DO \n"
  "RETURN proc_arg; \n"
  "ENDPROC \n"
  " \n"
  "PROCEDURE Proc_1_2 () RETURN DATE \n"
  "DO \n"
  "LET proc_arg  AS INT16; \n"
  "LET some_arg AS TEXT; \n"
  "Proc_1 ( proc_arg, some_arg ); \n"
  "RETURN table_1[0, f1]; \n"
  "ENDPROC \n";

char test_prog_3[] = ""
  "LET table_1 AS TABLE OF ( f1 as DATE, f2 as INT16); \n"
  "LET table_2 AS TABLE OF ( f1 as DATE); \n"
  " \n"
  "PROCEDURE Proc_1 ( proc_arg AS INT16) RETURN INT16 \n"
  "DO \n"
  "RETURN proc_arg; \n"
  "ENDPROC \n"
  " \n"
  "PROCEDURE Proc_1_2 () RETURN DATE \n"
  "DO \n"
  "Proc_1 (); \n" "RETURN '2010/01/01'; \n" "ENDPROC \n";

char test_prog_4[] = ""
  "PROCEDURE Proc_1 ( v1 as DATE, v2 as INT16) RETURN INT16 \n"
  "DO \n"
  "RETURN v2; \n"
  "ENDPROC \n"
  " \n"
  "PROCEDURE Proc_1_2 () RETURN DATE \n"
  "DO \n"
  "LET some_var as DATE;"
  "Proc_1 ( some_var, TRUE ); \n" "RETURN some_var; \n" "ENDPROC \n";

char test_prog_5[] = ""
  "PROCEDURE Proc_1 ( v1 as FIELD OF DATE, v2 as INT16) RETURN INT16 \n"
  "DO \n"
  "RETURN v2; \n"
  "ENDPROC \n"
  " \n"
  "PROCEDURE Proc_1_2 () RETURN DATE \n"
  "DO \n"
  "LET some_var as FIELD OF ARRAY OF DATE;"
  "Proc_1 ( some_var, 10 ); \n" "RETURN some_var; \n" "ENDPROC \n";

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
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_1, MSG_NO_PROC,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_2,
                                                     MSG_PROC_MORE_ARGS,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_3,
                                                     MSG_PROC_LESS_ARGS,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_4,
                                                     MSG_PROC_ARG_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error (test_prog_5,
                                                     MSG_PROC_ARG_NA,
                                                     MSG_ERROR_EVENT);
  if (test_result == FALSE)
    {
      printf ("TEST RESULT: FAIL\n");
      return -1;
    }

  printf ("TEST RESULT: PASS\n");
  return 0;
}
