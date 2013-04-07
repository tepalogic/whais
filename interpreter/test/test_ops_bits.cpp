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

using namespace prima;

static const D_CHAR admin[] = "administrator";

static const D_CHAR bool_not[] = "bool_not";
static const D_CHAR bool_and[] = "bool_and";
static const D_CHAR bool_xor[] = "bool_xor";
static const D_CHAR bool_or[]  = "bool_or";
static const D_CHAR int_not[]  = "int_not";
static const D_CHAR int_and[]  = "int_and";
static const D_CHAR int_xor[]  = "int_xor";
static const D_CHAR int_or[]   = "int_or";

const D_UINT8 callTestProgram[] = ""
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


static const D_CHAR *MSG_PREFIX[] = {
                                      "", "error ", "warning ", "error "
                                    };

static D_UINT
get_line_from_buffer (const D_CHAR * buffer, D_UINT buff_pos)
{
  D_UINT count = 0;
  D_INT result = 1;

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
my_postman (WHC_MESSENGER_ARG data,
            D_UINT            buff_pos,
            D_UINT            msg_id,
            D_UINT            msgType,
            const D_CHAR*     pMsgFormat,
            va_list           args)
{
  const D_CHAR *buffer = (const D_CHAR *) data;
  D_INT buff_line = get_line_from_buffer (buffer, buff_pos);

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

  const DBSBool value(false);

  stack.Push (value);

  session.ExecuteProcedure (bool_not, stack);

  if (stack.Size () != 1)
    return false;

  DBSBool result;
  stack[0].GetOperand ().GetValue (result);

  if (result.IsNull () || (result.m_Value == value.m_Value))
    return false;

  return true;
}

static bool
test_op_not (Session& session)
{
  std::cout << "Testing opcode not...\n";
  SessionStack stack;

  const DBSInt16 value(0x21);

  stack.Push (value);

  session.ExecuteProcedure (int_not, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt16 result;
  stack[0].GetOperand ().GetValue (result);

  if (result.IsNull () || (result.m_Value != ~value.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_andXX (Session&       session,
               const D_CHAR*  opDesc,
               const D_CHAR*  procName,
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
  stack[0].GetOperand ().GetValue (result);

  if (result.IsNull () || (result != DBS_T (val1.m_Value & val2.m_Value)))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_orXX (Session&        session,
               const D_CHAR*  opDesc,
               const D_CHAR*  procName,
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
  stack[0].GetOperand ().GetValue (result);

  if (result.IsNull () || (result != DBS_T (val1.m_Value | val2.m_Value)))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_xorXX (Session&       session,
               const D_CHAR*  opDesc,
               const D_CHAR*  procName,
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
  stack[0].GetOperand ().GetValue (result);

  if (result.IsNull () || (result != DBS_T (val1.m_Value ^ val2.m_Value)))
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
    const DBSBool  val1_b (true), val2_b (false);
    const DBSInt64 val1_i (11), val2_i (23);

    I_Session& commonSession = GetInstance (NULL);

    WBufferCompiledUnit callBuf (callTestProgram,
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
D_UINT32 WMemoryTracker::sm_InitCount = 0;
const D_CHAR* WMemoryTracker::sm_Module = "T";
#endif
