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
    return -1;

  while (count < buff_pos)
  {
    if (buffer[count] == '\n')
      ++result;

    else if (buffer[count] == 0)
    {
        assert(0);
    }
    ++count;
  }
  return result;
}

static char *MSG_PREFIX[] = {
  "", "error ", "warning ", "error ", "extra "
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

  if (buff_line != IGNORE_BUFFER_POS)
  {
    printf("%d: %s", err_cnt, MSG_PREFIX[msgType]);
    printf("%d : line %d: ", msg_id, buff_line);
  }
  else
  {
    printf("%d: %s", err_cnt - 1, MSG_PREFIX[msgType]);
    printf("%d : ", msg_id);
  }

  vprintf(msgFormat, args);
  printf("\n");

  if (buff_line != IGNORE_BUFFER_POS)
  {
    err_cnt++;
    last_msg_code = msg_id;
    last_msg_type = msgType;
  }
}


char test_proc_0[] =
    "PROCEDURE test_create_array () RETURN BOOL "
    "DO "
    "  a = {NULL}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_1[] =
    "PROCEDURE test_create_array () RETURN BOOL "
    "DO "
    "  a = {1, 'a'}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_2[] =
    "PROCEDURE test_create_array () RETURN BOOL "
    "DO "
    "  a = {1, 0.1}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_3[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {t.f1, @t.f2}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_4[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {@t.f1, '1970-11-12'}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_5[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {'1970-11-12', t.f2[0]}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_6[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {t.f2[0]}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_7[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {\"Linux\"}; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_8[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {t.f1} TEXT ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_9[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {t.f1} DATE ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_10[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {@t.f1} CHAR ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_11[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {@t.f1} DATETIME ; "
    "  RETURN a == NULL; "
    " ENDPROC ";


char test_proc_12[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {0.1, 0.2} HIRESTIME ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_13[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {1, 2} HIRESTIME ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_14[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {0} BOOL ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_15[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {t} BOOL ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_16[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  a = {t} ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_17[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  VAR b UINT32 ARRAY; "
    "  a = {b} ; "
    "  RETURN a == NULL; "
    " ENDPROC ";

char test_proc_18[] =
    "PROCEDURE test_create_array (t TABLE (f1 DATE, f2 TEXT)) RETURN BOOL "
    "DO "
    "  VAR b UINT32 ARRAY; "
    "  a = {b} UINT32 ; "
    "  RETURN a == NULL; "
    " ENDPROC ";


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
    test_proc_14,
    test_proc_15,
    test_proc_16,
    test_proc_17,
    test_proc_18,
    NULL
};

uint_t test_prog_err_msg[] = {
    MSG_ARR_CONSTRUCT_EXP_TYPE_NA,

    MSG_ARR_CONSTRUCT_EXP_FAIL,
    MSG_ARR_CONSTRUCT_EXP_FAIL,
    MSG_ARR_CONSTRUCT_EXP_FAIL,
    MSG_ARR_CONSTRUCT_EXP_FAIL,

    MSG_ARR_CONSTRUCT_EXP_TEXT,
    MSG_ARR_CONSTRUCT_EXP_TEXT,
    MSG_ARR_CONSTRUCT_EXP_TEXT,
    MSG_ARR_CONSTRUCT_DEF_TEXT,

    MSG_ARR_CONSTRUCT_EXP_FAIL,
    MSG_ARR_CONSTRUCT_EXP_FAIL,
    MSG_ARR_CONSTRUCT_EXP_FAIL,
    MSG_ARR_CONSTRUCT_EXP_FAIL,
    MSG_ARR_CONSTRUCT_EXP_FAIL,

    MSG_ARR_CONSTRUCT_EXP_FAIL,

    MSG_ARR_CONSTRUCT_EXP_TYPE_NA,
    MSG_ARR_CONSTRUCT_EXP_TYPE_NA,
    MSG_ARR_CONSTRUCT_EXP_TYPE_NA,
    MSG_ARR_CONSTRUCT_EXP_TYPE_NA,

    0
};


bool_t
test_for_error(const char *test_buffer, uint_t err_expected, uint_t err_type)
{
  WH_COMPILED_UNIT handler;
  bool_t test_result = TRUE;

  last_msg_code = 0xFF, last_msg_type = 0XFF;
  handler = wh_compiler_load(test_buffer,
                             strlen(test_buffer),
                             &my_postman,
                             (WH_MESSENGER_CTXT)test_buffer);

  if (handler != NULL)
  {
    if (err_type != MSG_WARNING_EVENT)
      test_result = FALSE;

    wh_compiler_discard(handler);
  }

  if (test_result)
  {
    if ((last_msg_code != err_expected) || (last_msg_type != err_type))
      test_result = FALSE;
  }

  if (test_get_mem_used() != 0)
  {
    printf("\nCurrent memory usage: %u bytes! It should be 0.", (uint_t)test_get_mem_used());
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
  {
    const bool_t r = test_for_error(test_prog[i], test_prog_err_msg[i], MSG_ERROR_EVENT);
    if (r == FALSE)
      printf ("FAILED at index %d\n", i);
    test_result &= r;
  }

  if (test_result == FALSE)
    {
      printf("TEST RESULT: FAIL\n");
      return -1;
    }

  printf("TEST RESULT: PASS\n");
  return 0;
}
