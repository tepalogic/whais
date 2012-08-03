#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "dbs_mgr.h"
#include "interpreter.h"
#include "test/test_fmw.h"

#include "interpreter/prima/pm_interpreter.h"
#include "interpreter/prima/pm_processor.h"
#include "compiler/include/whisperc/wopcodes.h"

using namespace prima;

static const D_CHAR admin[] = "administrator";
static const D_UINT8 cts_proc[] = "cts_test";
static const D_UINT8 inull_proc[] = "inull_test";
static const D_UINT8 nnull_proc[] = "nnull_test";

const D_UINT8 callTestProgram[] = ""
    "PROCEDURE cts_test (n AS INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE inull_test (n AS INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE nnull_test (n AS INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE p1 (n AS INT8) RETURN INT8\n"
    "DO\n"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE p2 (n AS INT8, n2 AS INT8) RETURN INT8\n"
    "DO\n"
      "RETURN p1 (n + n2);\n"
    "ENDPROC\n";

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

static D_UINT
w_encode_opcode (W_OPCODE opcode, D_UINT8* pOutCode)
{
  if (opcode > 0x80)
    {
      D_UINT16 temp = opcode;
      temp |= 0x8000;
      pOutCode [0] = (temp & 0xFF);
      pOutCode [1] = ((temp >> 8) & 0xFF);

      return 2;
    }
  pOutCode [0] = opcode;
  return 1;
}

static bool
test_op_cts (Session& session)
{
  std::cout << "Testing opcode cts...\n";
  const D_UINT32 procId = session.FindProcedure (cts_proc, sizeof cts_proc - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSInt8 op(-10);

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_CTS, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op); //A procedure should return a value!

  session.ExecuteProcedure (cts_proc, stack);

  if (stack.Size () != 1)
    return false;

  return true;
}

static bool
test_op_inull (Session& session)
{
  std::cout << "Testing opcode inull...\n";
  const D_UINT32 procId = session.FindProcedure (inull_proc,
                                                 sizeof inull_proc - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSDate op;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_INULL, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);

  session.ExecuteProcedure (inull_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSBool result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSBool (true) )
    return false;

  return true;
}

static bool
test_op_nnull (Session& session)
{
  std::cout << "Testing opcode nnull...\n";
  const D_UINT32 procId = session.FindProcedure (nnull_proc,
                                                 sizeof nnull_proc - 1);
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSDate op;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_NNULL, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op); //A procedure should return a value!

  session.ExecuteProcedure (nnull_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSBool result;
  stack[0].GetOperand ().GetValue (result);

  if (stack.Size () != 1)
    return false;

  if (result == DBSBool (true) )
    return false;

  return true;
}


static bool
test_op_call (Session& session)
{
  std::cout << "Testing opcode call...\n";
  SessionStack stack;

  stack.Push (DBSUInt8 (10));
  stack.Push (DBSUInt16 (2));

  session.ExecuteProcedure (_RC (const D_UINT8*, "p2"), stack);

  if (stack.Size () != 1)
    return false;

  DBSUInt8 result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSUInt8 (24) )
    return false;

  return true;
}

int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialized
  // We need not to test against it!
  D_UINT prealloc_mem = test_get_mem_used ();
  bool success = true;

  {
    std::string dir = ".";
    dir += whc_get_directory_delimiter ();

    DBSInit (dir.c_str (), dir.c_str ());
  }

  DBSCreateDatabase (admin, NULL);
  InitInterpreter ();

  {
    I_Session& commonSession = GetInstance (NULL);

    WBufferCompiledUnit callBuf (callTestProgram,
                                 sizeof callTestProgram,
                                 my_postman,
                                 callTestProgram);

    commonSession.LoadCompiledUnit (callBuf);

    success = success && test_op_cts (_SC (Session&, commonSession));
    success = success && test_op_inull (_SC (Session&, commonSession));
    success = success && test_op_nnull (_SC (Session&, commonSession));
    success = success && test_op_call (_SC (Session&, commonSession));

    ReleaseInstance (commonSession);
  }

  CleanInterpreter ();
  DBSRemoveDatabase (admin);
  DBSShoutdown ();

  D_UINT mem_usage = test_get_mem_used () - prealloc_mem;

  if (mem_usage)
    {
      success = false;
      test_print_unfree_mem();
    }

  std::cout << "Memory peak (no prealloc): " <<
            test_get_mem_peak () - prealloc_mem << " bytes." << std::endl;
  std::cout << "Preallocated mem: " << prealloc_mem << " bytes." << std::endl;
  std::cout << "Current memory usage: " << mem_usage << " bytes." << std::endl;
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return -1;
    }
  else
    std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}
