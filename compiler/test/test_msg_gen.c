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
  "", "error ", "warning ", "error ", "extra "
};

void
my_postman(WLOG_FUNC_CONTEXT bag,
            uint_t buff_pos,
            uint_t msg_id,
            uint_t msgType, const char * msgFormat, va_list args)
{
  const char *buffer = (const char *) bag;
  int buff_line = get_buffer_line_from_pos(buffer, buff_pos);

  printf(MSG_PREFIX[msgType]);
  printf("%d : line %d: ", msg_id, buff_line);
  vprintf(msgFormat, args);
  printf("\n");

  if (msgType != MSG_EXTRA_EVENT)
    {
      last_msg_code = msg_id;
      last_msg_type = msgType;
    }
}

char test_prog_1[] = ""
  "PROCEDURE Proc() RETURN DATE ARRAY\n"
  "DO \n"
  "VAR some_var DATE; \n"
  "RETURN some_var; \n"
  "ENDPROC \n";

char test_prog_2[] = ""
  "PROCEDURE Proc_1() RETURN DATE \n"
  "DO \n"
  "VAR some_var DATE; \n"
  "RETURN some_var; \n"
  "ENDPROC \n"
  " \n"
  "PROCEDURE Proc() RETURN RICHREAL \n"
  "DO \n"
  "IF (Proc_1()) DO\n"
  "RETURN 0.23; \n" "END \n" "RETURN 0.1; \n" "ENDPROC \n";

char test_prog_3[] = ""
  "PROCEDURE Proc() RETURN DATE ARRAY\n"
  "DO \n" "CONTINUE; \n" "RETURN some_var; \n" "ENDPROC \n";

char test_prog_4[] = ""
  "PROCEDURE Proc() RETURN DATE ARRAY \n"
  "DO \n" "BREAK; \n" "RETURN some_var; \n" "ENDPROC \n";;

char test_prog_5[] = ""
  "PROCEDURE Proc() RETURN DATE ARRAY\n"
  "DO \n"
  "VAR some_var DATE; \n"
  "VAR some_int INT8; \n"
  "SYNC \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n" "ENDSYNC \n" "RETURN some_var; \n" "ENDPROC \n";

char test_prog_6[] = ""
  "PROCEDURE Proc() RETURN DATE ARRAY \n"
  "DO \n"
  "VAR some_var DATE; \n"
  "VAR some_int INT32; \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n"
  " \n"
  "SYNC \n"
  "some_int = 0; \n"
  "ENDSYNC \n" " \n" "RETURN some_var; \n" "ENDPROC \n" " \n";

char test_prog_7[] = ""
  "PROCEDURE Proc() RETURN TABLE(f1 TEXT, f2 DATE) \n"
  "DO \n" "VAR some_var TABLE(v1 TEXT, v2 UINT8); \n" "RETURN some_var; \n" "ENDPROC \n";

char test_prog_8[] = ""
  "PROCEDURE Proc() RETURN TABLE(f1 TEXT, f2 DATE) \n"
  "DO \n" "VAR some_var TABLE(f1 TEXT, f2 UINT8); \n" "RETURN some_var; \n" "ENDPROC \n";

char test_prog_9[] = ""
  "PROCEDURE Proc() RETURN TABLE(f1 TEXT, f2 DATE) \n"
  "DO \n" "VAR some_var TABLE(f1 TEXT, f2 UINT8); \n" "some_var=NULL; \n" "ENDPROC \n";

char test_prog_10[] = ""
  "PROCEDURE Proc() RETURN BOOL \n"
  "DO \n" "VAR some_var TABLE(f1 TEXT, f2 UINT8); \n"
  " IF(some_var != NULL) RETURN TRUE; ELSE RETURN FALSE;\n some_var=NULL; \n" "ENDPROC \n";


char test_prog_11[] = ""
  "PROCEDURE Proc(p BOOL) RETURN BOOL \n"
  "DO \n"
  "  VAR an_array BOOL ARRAY; \n"
  "  FOR(p : an_array) "
  "    IF(p) BREAK;\n"
  "  RETURN an_array[0];\n"
  "ENDPROC\n";

char test_prog_12[] = ""
  "PROCEDURE Proc(b BOOL) RETURN BOOL \n"
  "DO \n"
  "  VAR an_array BOOL ARRAY; \n"
  "  FOR(o : an_array) \n"
  "    FOR(o : an_array) \n"
  "      IF(o) BREAK;\n"
  "  RETURN an_array[0];\n"
  "ENDPROC\n";


char test_prog_13[] = ""
  "PROCEDURE Proc(b BOOL) RETURN UINT64 \n"
  "DO \n"
  "  VAR an_array BOOL ARRAY; \n"
  "  VAR p INT16; \n"
  "  FOR(c : an_array) "
  "    IF(c) RETURN @p;\n"
  "  RETURN 0;\n"
  "ENDPROC\n";


char test_prog_14[] = ""
  "PROCEDURE Proc(b BOOL) RETURN UINT64 \n"
  "DO \n"
  "  VAR an_array BOOL ARRAY; \n"
  "  VAR p INT16; \n"
  "  FOR(c : an_array) "
  "    IF(c) RETURN @(p + 1);\n"
  "  RETURN 0;\n"
  "ENDPROC\n";


char test_prog_15[] = ""
  "PROCEDURE Proc(b BOOL) RETURN UINT64 \n"
  "DO \n"
  "  VAR a_value BOOL; \n"
  "  FOR(c : a_value) "
  "    IF(c) RETURN @p;\n"
  "  RETURN 0;\n"
  "ENDPROC\n";

char test_prog_16[] = ""
  "PROCEDURE Proc(b BOOL) RETURN UINT64 \n"
  "DO \n"
  "  VAR a_table TABLE(f2 BOOL); \n"
  "  FOR(c : a_table) "
  "    IF(c) RETURN @p;\n"
  "  RETURN 0;\n"
  "ENDPROC\n";


char test_prog_17[] = ""
  "PROCEDURE Proc(b BOOL) RETURN UINT64 \n"
  "DO \n"
  "  RETURN (UINT64)b;\n"
  "ENDPROC\n";


char test_prog_18[] = ""
  "PROCEDURE Proc(b ARRAY INT64) RETURN UINT64 \n"
  "DO \n"
  "  RETURN (UINT64)b;\n"
  "ENDPROC\n";

char test_prog_19[] = ""
  "PROCEDURE Proc(b ARRAY INT64) RETURN UINT64 \n"
  "DO \n"
  "  RETURN (UINT64)b;\n"
  "ENDPROC\n";

char test_prog_20[] = ""
  "PROCEDURE Proc(b UINT64 FIELD) RETURN UINT64 \n"
  "DO \n"
  "  RETURN (UINT64)b;\n"
  "ENDPROC\n";

char test_prog_21[] = ""
  "PROCEDURE Proc(b UINT64 FIELD) RETURN ARRAY UINT64 \n"
  "DO \n"
  "  RETURN (ARRAY UINT64)b;\n"
  "ENDPROC\n";

char test_prog_22[] = ""
  "PROCEDURE Proc(b UINT64 ARRAY FIELD) RETURN ARRAY UINT64 \n"
  "DO \n"
  "  RETURN (ARRAY UINT64)b;\n"
  "ENDPROC\n";

char test_prog_23[] = ""
  "PROCEDURE Proc(b UINT64 ARRAY FIELD) RETURN UINT64 \n"
  "DO \n"
  "  RETURN (UINT64)b;\n"
  "ENDPROC\n";

char test_prog_24[] = ""
  "PROCEDURE Proc(b DATE FIELD) RETURN FIELD UINT64 \n"
  "DO \n"
  "  RETURN (UINT64 FIELD)b;\n"
  "ENDPROC\n";

char test_prog_25[] = ""
  "PROCEDURE Proc(b DATE ARRAY FIELD) RETURN FIELD ARRAY UINT64 \n"
  "DO \n"
  "  RETURN (UINT64 ARRAY FIELD)b;\n"
  "ENDPROC\n";

char test_prog_26[] = ""
  "PROCEDURE Proc(b TABLE(d DATE)) RETURN FIELD ARRAY UINT64 \n"
  "DO \n"
  "  RETURN (FIELD ARRAY UINT64)b;\n"
  "ENDPROC\n";

char test_prog_27[] = ""
  "PROCEDURE Proc(d DATE) RETURN TABLE (p DATE)\n"
  "DO \n"
  "  RETURN (TABLE (p DATE))d;\n"
  "ENDPROC\n";

bool_t
test_for_error(const char *test_buffer, uint_t err_expected, uint_t err_type)
{
  WH_COMPILED_UNIT handler;
  bool_t test_result = TRUE;

  last_msg_code = 0xFF, last_msg_type = 0XFF;
  handler = wh_compiler_load(test_buffer,
                            strlen(test_buffer),
                            &my_postman, (WH_MESSENGER_CTXT) test_buffer);


  if (err_type == MSG_WARNING_EVENT)
    {
      if (handler == NULL)
        {
          test_result = FALSE;
          printf("The expected error code is actually a warning message. "
                  "The buffer should have been compiled anyway, but it "
                  "failed\n");
          return test_result;
        }
      else
        wh_compiler_discard(handler);

      return test_result ;
    }
  else if (handler != NULL)
    {
      printf("Looks like the buffer was compiled succefully, though an error "
              "was expected.\n");
      test_result = FALSE;
      wh_compiler_discard(handler);

      return test_result ;
    }

    {
      if ((last_msg_code != err_expected) || (last_msg_type != err_type))
        {
          test_result = FALSE;
        }
    }

  if (test_get_mem_used() != 0)
    {
      printf("Current memory usage: %u bytes! It should be 0.\n",
              (uint_t)test_get_mem_used());
      test_print_unfree_mem();
      test_result = FALSE;
    }

  return test_result;
}

int
main()
{
  bool_t test_result = TRUE;

  printf("Testing for received error messages...\n");

  test_result =
    test_for_error(test_prog_1, MSG_PROC_RET_NA_EXT, MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_2,
                                                     MSG_EXP_NOT_BOOL,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_3,
                                                     MSG_CONTINUE_NOLOOP,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_4,
                                                     MSG_BREAK_NOLOOP,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_5,
                                                     MSG_SYNC_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_6,
                                                     MSG_SYNC_MANY,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_7,
                                                     MSG_PROC_RET_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_8,
                                                     MSG_PROC_RET_NA,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_9,
                                                     MSG_PROC_NO_RET,
                                                     MSG_ERROR_EVENT);
  test_result =
    (test_result == FALSE) ? FALSE : test_for_error(test_prog_10,
                                                     MSG_DEAD_STMT,
                                                     MSG_WARNING_EVENT);

  test_result =
      (test_result == FALSE) ? FALSE : test_for_error(test_prog_11,
                                                       MSG_IT_VARIABLE,
                                                       MSG_WARNING_EVENT);

  test_result =
      (test_result == FALSE) ? FALSE : test_for_error(test_prog_12,
                                                       MSG_IT_ALREADY,
                                                       MSG_ERROR_EVENT);

  test_result =
      (test_result == FALSE) ? FALSE : test_for_error(test_prog_13,
                                                       MSG_IT_ID_TYPE_NA,
                                                       MSG_ERROR_EVENT);


  test_result =
      (test_result == FALSE) ? FALSE : test_for_error(test_prog_14,
                                                       MSG_IT_EXP_TYPE_NA,
                                                       MSG_ERROR_EVENT);

  test_result =
      (test_result == FALSE) ? FALSE : test_for_error(test_prog_15,
                                                       MSG_EXP_NOT_ITERABLE,
                                                       MSG_ERROR_EVENT);

  test_result =
      (test_result == FALSE) ? FALSE : test_for_error(test_prog_16,
                                                       MSG_EXP_NOT_ITERABLE,
                                                       MSG_ERROR_EVENT);
  test_result =
        (test_result == FALSE) ? FALSE : test_for_error(test_prog_17,
                                                         MSG_CAST_NOT_POSSIBLE,
                                                         MSG_ERROR_EVENT);
  test_result =
        (test_result == FALSE) ? FALSE : test_for_error(test_prog_18,
                                                         MSG_CAST_NOT_POSSIBLE,
                                                         MSG_ERROR_EVENT);
  test_result =
        (test_result == FALSE) ? FALSE : test_for_error(test_prog_19,
                                                         MSG_CAST_NOT_POSSIBLE,
                                                         MSG_ERROR_EVENT);
  test_result =
        (test_result == FALSE) ? FALSE : test_for_error(test_prog_20,
                                                         MSG_CAST_NOT_POSSIBLE,
                                                         MSG_ERROR_EVENT);
  test_result =
          (test_result == FALSE) ? FALSE : test_for_error(test_prog_21,
                                                           MSG_CAST_NOT_POSSIBLE,
                                                           MSG_ERROR_EVENT);
  test_result =
            (test_result == FALSE) ? FALSE : test_for_error(test_prog_22,
                                                             MSG_CAST_NOT_POSSIBLE,
                                                             MSG_ERROR_EVENT);
  test_result =
            (test_result == FALSE) ? FALSE : test_for_error(test_prog_23,
                                                             MSG_CAST_NOT_POSSIBLE,
                                                             MSG_ERROR_EVENT);
  test_result =
            (test_result == FALSE) ? FALSE : test_for_error(test_prog_24,
                                                             MSG_CAST_NOT_POSSIBLE,
                                                             MSG_ERROR_EVENT);
  test_result =
            (test_result == FALSE) ? FALSE : test_for_error(test_prog_25,
                                                             MSG_CAST_NOT_POSSIBLE,
                                                             MSG_ERROR_EVENT);

  test_result =
            (test_result == FALSE) ? FALSE : test_for_error(test_prog_26,
                                                             MSG_CAST_NOT_POSSIBLE,
                                                             MSG_ERROR_EVENT);

  test_result =
            (test_result == FALSE) ? FALSE : test_for_error(test_prog_27,
                                                             MSG_CAST_NOT_POSSIBLE,
                                                             MSG_ERROR_EVENT);
  if (test_result == FALSE)
    {
      printf("TEST RESULT: FAIL\n");
      return -1;
    }

  printf("TEST RESULT: PASS\n");
  return 0;
}
