#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "dbs/dbs_mgr.h"
#include "interpreter.h"
#include "custom/include/test/test_fmw.h"

#include "interpreter/prima/pm_interpreter.h"
#include "interpreter/prima/pm_processor.h"
#include "compiler//wopcodes.h"

using namespace whais;
using namespace prima;

static const char admin[] = "administrator";

static const char bool_not[] = "bool_not";
static const char bool_and[] = "bool_and";
static const char bool_xor[] = "bool_xor";
static const char bool_or[]  = "bool_or";
static const char int_not[]  = "int_not";
static const char int_and[]  = "int_and";
static const char int_xor[]  = "int_xor";
static const char int_or[]   = "int_or";

const uint8_t callTestProgram[] = ""
    "PROCEDURE bool_not (val AS BOOL) RETURN BOOL\n"
    "DO\n"
      "RETURN NOT val;\n"
    "ENDPROC\n"
    "PROCEDURE bool_and (val1 AS BOOL, val2 AS BOOL) RETURN BOOL\n"
    "DO\n"
      "RETURN val1 and val2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE bool_or (val1 AS BOOL, val2 AS BOOL) RETURN BOOL\n"
    "DO\n"
      "RETURN val1 or val2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE bool_xor (val1 AS BOOL, val2 AS BOOL) RETURN BOOL\n"
    "DO\n"
      "RETURN val1 xor val2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE int_not (val AS INT64) RETURN INT64\n"
    "DO\n"
      "RETURN NOT val;\n"
    "ENDPROC\n"
    "PROCEDURE int_and (val1 AS INT64, val2 AS INT64) RETURN INT64\n"
    "DO\n"
      "RETURN val1 and val2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE int_or (val1 AS INT64, val2 AS INT64) RETURN INT64\n"
    "DO\n"
      "RETURN val1 or val2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE int_xor (val1 AS INT64, val2 AS INT64) RETURN INT64\n"
    "DO\n"
      "RETURN val1 xor val2;\n"
    "ENDPROC\n"
    "\n";


static const char *MSG_PREFIX[] = {
                                      "", "error ", "warning ", "error "
                                    };

static uint_t
get_line_from_buffer (const char * buffer, uint_t buff_pos)
{
  uint_t count = 0;
  int result = 1;

  if (buff_pos == WHC_IGNORE_BUFFER_POS)
    return -1;

  while (count < buff_pos)
    {
      if (buffer[count] == '\n')
        ++result;
      else if (buffer[count] == 0)
        {
          assert (0);
        }
      ++count;
    }
  return result;
}

void
my_postman (WH_MESSENGER_CTXT data,
            uint_t            buff_pos,
            uint_t            msg_id,
            uint_t            msgType,
            const char*     pMsgFormat,
            va_list           args)
{
  const char *buffer = (const char *) data;
  int buff_line = get_line_from_buffer (buffer, buff_pos);

  fprintf (stderr, MSG_PREFIX[msgType]);
  fprintf (stderr, "%d : line %d: ", msg_id, buff_line);
  vfprintf (stderr, pMsgFormat, args);
  fprintf (stderr, "\n");
}

static bool
test_op_notb (Session& session)
{
  std::cout << "Testing opcode notb...\n";
  SessionStack stack;

  const DBool value (false);

  stack.Push (value);

  session.ExecuteProcedure (bool_not, stack);

  if (stack.Size () != 1)
    return false;

  DBool result;
  stack[0].Operand ().GetValue (result);

  if (result.IsNull () || (result.mValue == value.mValue))
    return false;

  return true;
}

static bool
test_op_not (Session& session)
{
  std::cout << "Testing opcode not...\n";
  SessionStack stack;

  const DInt16 value (0x21);

  stack.Push (value);

  session.ExecuteProcedure (int_not, stack);

  if (stack.Size () != 1)
    return false;

  DInt16 result;
  stack[0].Operand ().GetValue (result);

  if (result.IsNull () || (result.mValue != ~value.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_andXX (Session&       session,
               const char*  opDesc,
               const char*  procName,
               const DBS_T    val1,
               const DBS_T    val2)
{
  std::cout << "Testing opcode " << opDesc <<" ...\n";
  SessionStack stack;

  stack.Push (val1);
  stack.Push (val2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBS_T result;
  stack[0].Operand ().GetValue (result);

  if (result.IsNull () || (result != DBS_T (val1.mValue & val2.mValue)))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_orXX (Session&        session,
               const char*  opDesc,
               const char*  procName,
               const DBS_T    val1,
               const DBS_T    val2)
{
  std::cout << "Testing opcode " << opDesc <<" ...\n";
  SessionStack stack;

  stack.Push (val1);
  stack.Push (val2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBS_T result;
  stack[0].Operand ().GetValue (result);

  if (result.IsNull () || (result != DBS_T (val1.mValue | val2.mValue)))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_xorXX (Session&       session,
               const char*  opDesc,
               const char*  procName,
               const DBS_T    val1,
               const DBS_T    val2)
{
  std::cout << "Testing opcode " << opDesc <<" ...\n";
  SessionStack stack;

  stack.Push (val1);
  stack.Push (val2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBS_T result;
  stack[0].Operand ().GetValue (result);

  if (result.IsNull () || (result != DBS_T (val1.mValue ^ val2.mValue)))
    return false;

  return true;
}


int
main ()
{
  bool success = true;

  {
    DBSInit (DBSSettings ());
  }

  DBSCreateDatabase (admin);
  InitInterpreter ();

  {
    const DBool  val1_b (true), val2_b (false);
    const DInt64 val1_i (11), val2_i (23);

    ISession& commonSession = GetInstance (NULL);

    CompiledBufferUnit callBuf (callTestProgram,
                                 sizeof callTestProgram,
                                 my_postman,
                                 callTestProgram);

    commonSession.LoadCompiledUnit (callBuf);

    success = success && test_op_notb (_SC (Session&, commonSession));
    success = success && test_op_andXX (_SC (Session&, commonSession),
                                        "andb",
                                        bool_and,
                                        val1_b,
                                        val2_b);
    success = success && test_op_xorXX (_SC (Session&, commonSession),
                                        "xorb",
                                        bool_xor,
                                        val1_b,
                                        val2_b);
    success = success && test_op_orXX (_SC (Session&, commonSession),
                                        "orb",
                                        bool_or,
                                        val2_b,
                                        val1_b);
    success = success && test_op_not (_SC (Session&, commonSession));
    success = success && test_op_andXX (_SC (Session&, commonSession),
                                        "and",
                                        int_and,
                                        val1_i,
                                        val2_i);
    success = success && test_op_xorXX (_SC (Session&, commonSession),
                                        "xor",
                                        int_xor,
                                        val1_i,
                                        val2_i);
    success = success && test_op_orXX (_SC (Session&, commonSession),
                                        "or",
                                        int_or,
                                        val1_i,
                                        val2_i);

    ReleaseInstance (commonSession);
  }

  CleanInterpreter ();
  DBSRemoveDatabase (admin);
  DBSShoutdown ();

  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
