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

using namespace whisper;
using namespace prima;

static const char admin[] = "administrator";
static const char procName[] = "p1";

const uint8_t dummyProgram[] = ""
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
};

static uint_t
w_encode_opcode (W_OPCODE opcode, uint8_t* pOutCode)
{
  if (opcode >= 0x80)
    {
      uint16_t temp = opcode;
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

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDNULL, testCode);
  w_encode_opcode (W_RET, testCode + opSize);
  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].Operand ().IsNull () == false)
    return false;

  return true;
}

static bool
test_op_ldc (Session& session)
{
  std::cout << "Testing ldc...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDC, testCode);
  testCode [opSize + 3] = 0x53;
  testCode [opSize + 2] = 0x52;
  testCode [opSize + 1] = 0x51;
  testCode [opSize + 0] = 0x50;
  w_encode_opcode (W_RET, testCode + opSize + 4);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DChar value;
  stack[0].Operand ().GetValue (value);
  if (value != DChar (0x53525150))
    return false;

  return true;
}

static bool
test_op_ldi8 (Session& session)
{
  std::cout << "Testing ldi8...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDI8, testCode);
  testCode [opSize + 0] = 0x58;
  w_encode_opcode (W_RET, testCode + opSize + 1);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DInt8 value;
  stack[0].Operand ().GetValue (value);
  if (value != DInt8 (0x58))
    return false;

  return true;
}

static bool
test_op_ldi16 (Session& session)
{
  std::cout << "Testing ldi16...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDI16, testCode);
  testCode [opSize + 0] = 0xA1;
  testCode [opSize + 1] = 0x34;
  w_encode_opcode (W_RET, testCode + opSize + 2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DInt16 value;
  stack[0].Operand ().GetValue (value);
  if (value != DInt16 (0x34A1))
    return false;

  return true;
}

static bool
test_op_ldi32 (Session& session)
{
  std::cout << "Testing ldi32...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDI32, testCode);
  testCode [opSize + 0] = 0xF1;
  testCode [opSize + 1] = 0xD4;
  testCode [opSize + 2] = 0x21;
  testCode [opSize + 3] = 0x34;

  session.ExecuteProcedure (procName, stack);
  w_encode_opcode (W_RET, testCode + opSize + 4);

  if (stack.Size () != 1)
    return false;

  DInt32 value;
  stack[0].Operand ().GetValue (value);
  if ((value == DInt32 (0x3421D4F1)) == false)
    return false;

  return true;
}

static bool
test_op_ldi64 (Session& session)
{
  std::cout << "Testing ldi64...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDI64, testCode);
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

  DInt64 value;
  stack[0].Operand ().GetValue (value);
  if (value != DInt64 (0x383736353421D4F1))
    return false;

  return true;
}

static bool
test_op_ldd (Session& session)
{
  std::cout << "Testing ldd...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDD, testCode);
  testCode [opSize + 0] = 0x05;
  testCode [opSize + 1] = 0x06;
  testCode [opSize + 2] = 0xF1;
  testCode [opSize + 3] = 0xD1;
  w_encode_opcode (W_RET, testCode + opSize + 4);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DDate value;
  stack[0].Operand ().GetValue (value);
  if (value != DDate (0xD1F1, 0x06, 0x05))
    return false;

  return true;
}

static bool
test_op_lddt (Session& session)
{
  std::cout << "Testing lddt...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDDT, testCode);
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

  DDateTime value;
  stack[0].Operand ().GetValue (value);
  if (value != DDateTime (0xD1F1, 0x06, 0x05, 0x0B, 0x10, 0x23))
    return false;

  return true;
}

static bool
test_op_ldht (Session& session)
{
  std::cout << "Testing ldht...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDHT, testCode);
  testCode [opSize + 0]  = 0x04;
  testCode [opSize + 1]  = 0x03;
  testCode [opSize + 2]  = 0x02;
  testCode [opSize + 3]  = 0x00;
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

  DHiresTime value;
  stack[0].Operand ().GetValue (value);
  if (value !=
      DHiresTime (0xD1F1, 0x06, 0x05, 0x0B, 0x10, 0x23, 0x20304))
    return false;

  return true;
}

static bool
test_op_ldrr (Session& session)
{
  std::cout << "Testing ldrr...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDRR, testCode);
  testCode [opSize + 0]   = 0xFB;
  testCode [opSize + 1]   = 0xFF;
  testCode [opSize + 2]   = 0xFF;
  testCode [opSize + 3]   = 0xFF;
  testCode [opSize + 4]   = 0xFF;
  testCode [opSize + 5]   = 0xFF;
  testCode [opSize + 6]   = 0xFF;
  testCode [opSize + 7]   = 0xFF;
  testCode [opSize + 8]   = 0xE0;
  testCode [opSize + 9]   = 0xE6;
  testCode [opSize + 10]  = 0xA7;
  testCode [opSize + 11]  = 0x58;
  testCode [opSize + 12]  = 0x4C;
  testCode [opSize + 13]  = 0x49;
  testCode [opSize + 14]  = 0x1F;
  testCode [opSize + 15]  = 0xF2;

  w_encode_opcode (W_RET, testCode + opSize + 16);
  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DRichReal value;
  stack[0].Operand ().GetValue (value);
  if (value != DRichReal (RICHREAL_T (-5,
                                        -99999999999922,
                                        DBS_RICHREAL_PREC)))
    {
      return false;
    }

  return true;
}

static bool
test_op_ldt (Session& session)
{
  std::cout << "Testing ldt...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  char someText[] = "A test to check!Good Luck!";

  uint_t opSize = w_encode_opcode (W_LDT, testCode);
  testCode [opSize + 0]  = 0x02;
  testCode [opSize + 1]  = 0x00;
  testCode [opSize + 2]  = 0x00;
  testCode [opSize + 3]  = 0x00;

  w_encode_opcode (W_RET, testCode + opSize + 4);
  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DText      value;
  stack[0].Operand ().GetValue (value);
  if (value != DText ((uint8_t*)someText + 2))
    return false;

  return true;
}

static bool
test_op_ldbt (Session& session)
{
  std::cout << "Testing ldbt...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDBT, testCode);
  w_encode_opcode (W_RET, testCode + opSize);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBool value;
  stack[0].Operand ().GetValue (value);
  if ((value == DBool (true)) == false)
    return false;

  return true;
}

static bool
test_op_ldbf (Session& session)
{
  std::cout << "Testing ldbf...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));

  SessionStack stack;

  uint_t opSize = w_encode_opcode (W_LDBF, testCode);
  w_encode_opcode (W_RET, testCode + opSize);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBool value;
  stack[0].Operand ().GetValue (value);
  if (value != DBool (false))
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
    ISession& commonSession = GetInstance (NULL);

    CompiledBufferUnit dummy (dummyProgram,
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
    success = success && test_op_ldrr (_SC (Session&, commonSession));
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
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif

