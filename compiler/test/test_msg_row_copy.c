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
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   i = 0;\n"
"   i![0] = v1[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_1[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   i = 0;\n"
"   i*[0] = v1[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_2[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   i = 0;\n"
"   i{f2, f1}[0] = v1[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_3[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   i = 0;\n"
"   v1![0] = i[2];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";


char test_proc_4[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   i = 0;\n"
"   v1{f3}[0] = i{f3}[2];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_5[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   VAR d DATE;\n"
"   v1*[d] = v2[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_6[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   VAR d DATE;\n"
"   v1*[0] = v2[d];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_7[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v1{f2,f4,f1}[0] = v2[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";


char test_proc_8[] =
  "PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
  "                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
  "RETURN BOOL\n"
  "DO\n"
  "   v1{f4}[0] = v2[1];\n"
  "  \n"
  "  RETURN NULL;\n"
  "ENDPROC\n";


char test_proc_9[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v2{f4}[0] = v1[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_10[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v2{f2, f3}[0] = v1{f4, f3}[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_11[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v2*[0] = v1[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_12[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 DATE, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v1*[0] = v2[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_13[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 DATE, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v1{f1}[0] = v2{f4}[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";


char test_proc_14[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 DATE, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v1![0] = v2[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_15[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (zf2 INT32 ARRAY, zf1 INT32, zf3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v1![0] = v2[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

char test_proc_16[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v1{f1, f2}[0] = v2{f1}[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";


char test_proc_17[] =
"PROCEDURE test_tabe_rowcopy(v1 TABLE (f2 INT32 ARRAY, f1 INT32, f3 TEXT), \n"
"                            v2 TABLE (f1 INT32, f2 INT32 ARRAY, f3 TEXT, f4 INT32 ARRAY))\n"
"RETURN BOOL\n"
"DO\n"
"   v1{f1}[0] = v2{f1, f2}[1];\n"
"  \n"
"  RETURN NULL;\n"
"ENDPROC\n";

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
    NULL
};

uint_t test_prog_err_msg[] = {
    MSG_ROW_COPY_NOTABLE_L,
    MSG_ROW_COPY_NOTABLE_L,
    MSG_ROW_COPY_NOTABLE_L,

    MSG_ROW_COPY_NOTABLE_R,
    MSG_ROW_COPY_NOTABLE_R,

    MSG_ROW_COPY_NOINDEX_L,
    MSG_ROW_COPY_NOINDEX_R,

    MSG_ROW_COPY_NOFIELD_L,
    MSG_ROW_COPY_NOFIELD_L,

    MSG_ROW_COPY_NOFIELD_R,
    MSG_ROW_COPY_NOFIELD_R,
    MSG_ROW_COPY_NOFIELD_R,

    MSG_ROW_COPY_TYPEFIELD_NA,
    MSG_ROW_COPY_TYPEFIELD_NA,
    MSG_ROW_COPY_TYPEFIELD_NA,

    MSG_ROW_COPY_NOFIELD_SEL,

    MSG_ROW_COPY_NOFIELDS_CNT,
    MSG_ROW_COPY_NOFIELDS_CNT,

    0
};


uint_t test_prog_msg_types[] = {
    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,

    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,

    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,

    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,

    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,

    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,

    MSG_WARNING_EVENT,

    MSG_ERROR_EVENT,
    MSG_ERROR_EVENT,

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
    const bool_t r = test_for_error(test_prog[i], test_prog_err_msg[i], test_prog_msg_types[i]);
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
