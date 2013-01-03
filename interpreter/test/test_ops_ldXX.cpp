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
static const D_CHAR procName[] = "p1";

const D_UINT8 dummyProgram[] = ""
    "PROCEDURE p1 () RETURN BOOL\n"
    "DO\n"
      "LET t1 AS TEXT;\n"
      "LET hd1, hd2 AS HIRESTIME\n;"
      "\n"
      "t1 = \"A test to check!Good Luck!\";\n"
      "hd1 = '2012/12/12 13:00:00.100';\n"
      "hd2 = '2012/12/11 13:00:00';\n"
      "RETURN hd1 < hd2;\n"
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
};

static D_UINT
w_encode_opcode (W_OPCODE opcode, D_UINT8* pOutCode)
{
  if (opcode >= 0x80)
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
test_op_ldnull (Session& session)
{
  std::cout << "Testing ldnull...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDNULL, testCode);
  w_encode_opcode (W_RET, testCode + opSize);
  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull () == false)
    return false;

  return true;
}

static bool
test_op_ldc (Session& session)
{
  std::cout << "Testing ldc...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDC, testCode);
  testCode [opSize + 3] = 0x53;
  testCode [opSize + 2] = 0x52;
  testCode [opSize + 1] = 0x51;
  testCode [opSize + 0] = 0x50;
  w_encode_opcode (W_RET, testCode + opSize + 4);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSChar value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSChar (0x53525150))
    return false;

  return true;
}

static bool
test_op_ldi8 (Session& session)
{
  std::cout << "Testing ldi8...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDI8, testCode);
  testCode [opSize + 0] = 0xD8;
  w_encode_opcode (W_RET, testCode + opSize + 1);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt8 value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSInt8 (0xD8))
    return false;

  return true;
}

static bool
test_op_ldi16 (Session& session)
{
  std::cout << "Testing ldi16...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDI16, testCode);
  testCode [opSize + 0] = 0xA1;
  testCode [opSize + 1] = 0xB4;
  w_encode_opcode (W_RET, testCode + opSize + 2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt16 value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSInt16 (0xB4A1))
    return false;

  return true;
}

static bool
test_op_ldi32 (Session& session)
{
  std::cout << "Testing ldi32...\n";
  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDI32, testCode);
  testCode [opSize + 0] = 0xF1;
  testCode [opSize + 1] = 0xD4;
  testCode [opSize + 2] = 0x21;
  testCode [opSize + 3] = 0x34;

  session.ExecuteProcedure (procName, stack);
  w_encode_opcode (W_RET, testCode + opSize + 4);

  if (stack.Size () != 1)
    return false;

  DBSInt32 value;
  stack[0].GetOperand ().GetValue (value);
  if ((value == DBSInt32 (0x3421D4F1)) == false)
    return false;

  return true;
}

static bool
test_op_ldi64 (Session& session)
{
  std::cout << "Testing ldi64...\n";
  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDI64, testCode);
  testCode [opSize + 0] = 0xF1;
  testCode [opSize + 1] = 0xD4;
  testCode [opSize + 2] = 0x21;
  testCode [opSize + 3] = 0x34;
  testCode [opSize + 4] = 0x35;
  testCode [opSize + 5] = 0x36;
  testCode [opSize + 6] = 0x37;
  testCode [opSize + 7] = 0x38;
  w_encode_opcode (W_RET, testCode + opSize + 8);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt64 value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSInt64 (0x383736353421D4F1))
    return false;

  return true;
}

static bool
test_op_ldd (Session& session)
{
  std::cout << "Testing ldd...\n";
  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDD, testCode);
  testCode [opSize + 0] = 0x05;
  testCode [opSize + 1] = 0x06;
  testCode [opSize + 2] = 0xF1;
  testCode [opSize + 3] = 0xD1;
  w_encode_opcode (W_RET, testCode + opSize + 4);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSDate value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSDate (0xD1F1, 0x06, 0x05))
    return false;

  return true;
}

static bool
test_op_lddt (Session& session)
{
  std::cout << "Testing lddt...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDDT, testCode);
  testCode [opSize + 0] = 0x23;
  testCode [opSize + 1] = 0x10;
  testCode [opSize + 2] = 0x0B;
  testCode [opSize + 3] = 0x05;
  testCode [opSize + 4] = 0x06;
  testCode [opSize + 5] = 0xF1;
  testCode [opSize + 6] = 0xD1;
  w_encode_opcode (W_RET, testCode + opSize + 7);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSDateTime value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSDateTime (0xD1F1, 0x06, 0x05, 0x0B, 0x10, 0x23))
    return false;

  return true;
}

static bool
test_op_ldht (Session& session)
{
  std::cout << "Testing ldht...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDHT, testCode);
  testCode [opSize + 0]  = 0x04;
  testCode [opSize + 1]  = 0x03;
  testCode [opSize + 2]  = 0x02;
  testCode [opSize + 3]  = 0x01;
  testCode [opSize + 4]  = 0x23;
  testCode [opSize + 5]  = 0x10;
  testCode [opSize + 6]  = 0x0B;
  testCode [opSize + 7]  = 0x05;
  testCode [opSize + 8]  = 0x06;
  testCode [opSize + 9]  = 0xF1;
  testCode [opSize + 10] = 0xD1;
  w_encode_opcode (W_RET, testCode + opSize + 11);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSHiresTime value;
  stack[0].GetOperand ().GetValue (value);
  if (value !=
      DBSHiresTime (0xD1F1, 0x06, 0x05, 0x0B, 0x10, 0x23, 0x01020304))
    return false;

  return true;
}

static bool
test_op_ldr (Session& session)
{
  std::cout << "Testing ldr...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDR, testCode);
  testCode [opSize + 0]  = 0x05;
  testCode [opSize + 1]  = 0x00;
  testCode [opSize + 2]  = 0x00;
  testCode [opSize + 3]  = 0x00;
  testCode [opSize + 4]  = 0x00;
  testCode [opSize + 5]  = 0x00;
  testCode [opSize + 6]  = 0x00;
  testCode [opSize + 7]  = 0x00;
  testCode [opSize + 8]  = 0xFE;

  w_encode_opcode (W_RET, testCode + opSize + 9);
  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSRichReal value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSRichReal ((RICHREAL_T)5 / 100))
    return false;

  return true;
}

static bool
test_op_ldt (Session& session)
{
  std::cout << "Testing ldt...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_CHAR someText[] = "A test to check!Good Luck!";

  D_UINT opSize = w_encode_opcode (W_LDT, testCode);
  testCode [opSize + 0]  = 0x02;
  testCode [opSize + 1]  = 0x00;
  testCode [opSize + 2]  = 0x00;
  testCode [opSize + 3]  = 0x00;

  w_encode_opcode (W_RET, testCode + opSize + 4);
  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSText      value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSText ((D_UINT8*)someText + 2))
    return false;

  return true;
}

static bool
test_op_ldbt (Session& session)
{
  std::cout << "Testing ldbt...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDBT, testCode);
  w_encode_opcode (W_RET, testCode + opSize);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSBool value;
  stack[0].GetOperand ().GetValue (value);
  if ((value == DBSBool (true)) == false)
    return false;

  return true;
}

static bool
test_op_ldbf (Session& session)
{
  std::cout << "Testing ldbf...\n";

  const D_UINT32 procId = session.FindProcedure (
                                              _RC (const D_UINT8*, procName),
                                              sizeof procName - 1
                                                );
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));

  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDBF, testCode);
  w_encode_opcode (W_RET, testCode + opSize);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSBool value;
  stack[0].GetOperand ().GetValue (value);
  if (value != DBSBool (false))
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
    I_Session& commonSession = GetInstance (NULL);

    WBufferCompiledUnit dummy (dummyProgram,
                               sizeof dummyProgram,
                               my_postman,
                               dummyProgram);

    commonSession.LoadCompiledUnit (dummy);

    success = success && test_op_ldnull (_SC (Session&, commonSession));
    success = success && test_op_ldc (_SC (Session&, commonSession));
    success = success && test_op_ldi8 (_SC (Session&, commonSession));
    success = success && test_op_ldi16 (_SC (Session&, commonSession));
    success = success && test_op_ldi32 (_SC (Session&, commonSession));
    success = success && test_op_ldi64 (_SC (Session&, commonSession));
    success = success && test_op_ldd (_SC (Session&, commonSession));
    success = success && test_op_lddt (_SC (Session&, commonSession));
    success = success && test_op_ldht (_SC (Session&, commonSession));
    success = success && test_op_ldr (_SC (Session&, commonSession));
    success = success && test_op_ldt (_SC (Session&, commonSession));
    success = success && test_op_ldbt (_SC (Session&, commonSession));
    success = success && test_op_ldbf (_SC (Session&, commonSession));

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

