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
            const uint_t msgType, const char * msgFormat, va_list args)
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

char test_prog_1[] = ""
  "PROCEDURE Proc_1 (v1 INT8, v2 DATE) RETURN INT8 "
  "DO\n "
  "RETURN v1 + the_most_long_string_that_needs_to_be_truncated_it_has_to_be_at_least_128_chars_and_I_bored_to_write_all_this_kind_here_just_to_check_if_something_is_working_prperly;\n "
  "ENDPROC\n " "\n ";

char test_prog_2[] = ""
  "PROCEDURE Proc_2 (v1 DATE)\n RETURN DATE "
  "DO\n " "v2 = v1;\n " "RETURN v2;\n " "ENDPROC\n " "\n " "";

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
  test_result = test_for_error (test_prog_1, MSG_VAR_NFOUND, MSG_ERROR_EVENT);
  test_result = test_result && test_for_error (test_prog_2, MSG_VAR_NFOUND, MSG_ERROR_EVENT);

  if (test_result == FALSE)
    {
      printf ("TEST RESULT: FAIL\n");
      return -1;
    }

  printf ("TEST RESULT: PASS\n");
  return 0;
}
