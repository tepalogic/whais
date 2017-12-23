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

extern int yyparse(struct ParserState *);

uint_t last_msg_code = 0xFF, last_msg_type = 0XFF;

static int
get_buffer_line_from_pos(const char *buffer, uint_t buff_pos)
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
          assert(0);
        }
      ++count;
    }
  return result;
}

static char *MSG_PREFIX[] = {
  "", "error ", "warning ", "error "
};

void
my_postman(WLOG_FUNC_CONTEXT bag,
            uint_t buff_pos,
            uint_t msg_id,
            uint_t msgType, const char * msgFormat, va_list args)
{
  static int err_cnt = 0;
  const char *buffer = (const char *) bag;
  int buff_line = get_buffer_line_from_pos(buffer, buff_pos);

  printf("%d: %s", err_cnt++, MSG_PREFIX[msgType]);
  printf("%d : line %d: ", msg_id, buff_line);
  vprintf(msgFormat, args);
  printf("\n");

  last_msg_code = msg_id;
  last_msg_type = msgType;
}

char test_proc_0[] =
"PROCEDURE Proc(v1 TEXT, v2 TABLE) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_1[] =
"PROCEDURE Proc(v1 DATE ARRAY, v2 TABLE) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_2[] =
"PROCEDURE Proc(v1 DATE FIELD, v2 TABLE) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_3[] =
"PROCEDURE Proc(v1 DATE FIELD, v2 TABLE) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_4[] =
"PROCEDURE Proc(v1 DATE ARRAY FIELD, v2 TABLE) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_5[] =
"PROCEDURE Proc(v1 DATE, v2 DATE ARRAY FIELD) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_6[] =
"PROCEDURE Proc(v1 DATE, v2 DATE FIELD) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_7[] =
"PROCEDURE Proc(v1 DATE, v2 DATE ARRAY) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_8[] =
"PROCEDURE Proc(v1 DATE, v2 ARRAY) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";


char test_proc_9[] =
"PROCEDURE Proc(v1 DATE, v2 CHAR) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_10[] =
"PROCEDURE Proc(v1 TABLE (f1 TEXT), v2 TABLE (f2 TEXT)) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_11[] =
"PROCEDURE Proc(v1 TABLE (f1 DATE), v2 TABLE (f1 CHAR)) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_12[] =
"PROCEDURE Proc(v1 TABLE (f1 DATE), v2 TABLE (f1 DATETIME)) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_13[] =
"PROCEDURE Proc(v1 TABLE (f UINT8, f1 DATE, t DATE), v2 TABLE (p CHAR, f UINT8, f2 DATE)) RETURN BOOL\n\n"
"DO\n\n"
"   a = (1 == 0) ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char test_proc_bool[] =
"PROCEDURE Proc(v1 DATE, v2 CHAR) RETURN BOOL\n\n"
"DO\n\n"
"   a = v1 ? v1 : v2;\n\n"
"   RETURN a == NULL; "
"ENDPROC\n\n";

char* test_prog[] = {
    test_proc_0,
    test_proc_1,
    test_proc_2,
    test_proc_3,
    test_proc_4,
    test_proc_5,
    test_proc_6,
    test_proc_7,
    test_proc_8,
    test_proc_9,
    test_proc_10,
    test_proc_11,
    test_proc_12,
    test_proc_13,
    NULL
};

bool_t
test_for_error(const char *test_buffer, uint_t err_expected, uint_t err_type)
{
  WH_COMPILED_UNIT handler;
  bool_t test_result = TRUE;

  last_msg_code = 0xFF, last_msg_type = 0XFF;
  handler = wh_compiler_load(test_buffer,
                            strlen(test_buffer),
                            &my_postman, (WH_MESSENGER_CTXT) test_buffer);

  if (handler != NULL)
    {
      test_result = FALSE;
      wh_compiler_discard(handler);
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
      printf("Current memory usage: %u bytes! It should be 0.",
              (uint_t)test_get_mem_used());
      test_result = FALSE;
    }
  return test_result;
}

int
main()
{
  bool_t test_result = TRUE;
  int i;

  printf("Testing for received error messages...\n");
  for (i = 0; i < (sizeof(test_prog) / sizeof (test_prog[0])) - 1; ++i)
    test_result &= test_for_error(test_prog[i], MSG_SEL_EXP_NOT_EQ, MSG_ERROR_EVENT);

  test_result &= test_for_error(test_proc_bool, MSG_SEL_NO_BOOL, MSG_ERROR_EVENT);

  if (test_result == FALSE)
    {
      printf("TEST RESULT: FAIL\n");
      return -1;
    }

  printf("TEST RESULT: PASS\n");
  return 0;
}
