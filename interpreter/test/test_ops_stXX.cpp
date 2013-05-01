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
    "PROCEDURE p1 (a1 AS INT8, a2 AS INT8) RETURN BOOL\n"
    "DO\n"
      "LET hd1, hd2 AS HIRESTIME\n;"
      "\n"
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
test_op_stb (Session& session)
{
  std::cout << "Testing bool assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBSBool op;
  DBSBool op2(true);

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STB, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSBool result;
  stack[0].GetOperand ().GetValue (result);

  if (result != op2)
    return false;

  return true;
}

static bool
test_op_stc (Session& session)
{
  std::cout << "Testing char assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBSChar op;
  DBSChar op2('A');

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STC, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSChar result;
  stack[0].GetOperand ().GetValue (result);

  if (result != op2)
    return false;

  return true;
}

static bool
test_op_std (Session& session)
{
  std::cout << "Testing date assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBSDate op;
  DBSDate op2(1989, 12, 25);

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STD, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);


  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSDate result;
  stack[0].GetOperand ().GetValue (result);

  if (result != op2)
    return false;

  return true;
}

static bool
test_op_stdt (Session& session)
{
  std::cout << "Testing date time assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBSDateTime op;
  DBSDateTime op2(1989, 12, 25, 12, 0, 0);

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STDT, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSDateTime result;
  stack[0].GetOperand ().GetValue (result);

  if (result != op2)
    return false;

  return true;
}

static bool
test_op_stht (Session& session)
{
  std::cout << "Testing hires date time assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBSHiresTime op;
  DBSHiresTime op2(1989, 12, 25, 12, 0, 0, 1);

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STHT, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSHiresTime result;
  stack[0].GetOperand ().GetValue (result);

  if (result != op2)
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_stXX (Session&       session,
              const W_OPCODE code,
              const char*  pText)
{
  std::cout << "Testing " << pText << " assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBS_T op;
  DBS_T op2(0x61);

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (code, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBS_T result;
  stack[0].GetOperand ().GetValue (result);

  if (result != op2)
    return false;

  return true;
}

static bool
test_op_stt (Session& session)
{
  std::cout << "Testing text assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBSText op;
  DBSText op2(_RC (const uint8_t*, "Testing the best way to future!"));

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STT, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSText result;
  stack[0].GetOperand ().GetValue (result);

  if (result != op2)
    return false;

  return true;
}

static bool
test_op_stta (Session& session)
{
  std::cout << "Testing table assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  const DBSUInt32 firstVal (0x31);
  const DBSUInt32 secondVal (0x32);

  DBSFieldDescriptor fd = {"first_field",  T_UINT32, false};

  I_DBSTable& firstTable = session.DBSHandler ().CreateTempTable (1, &fd);
  I_DBSTable& secondTable = session.DBSHandler ().CreateTempTable (1, &fd);

  secondTable.SetEntry (secondTable.AddReusedRow (), 0, firstVal);
  secondTable.SetEntry (secondTable.AddReusedRow (), 0, secondVal);

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STTA, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);


  stack.Push (session.DBSHandler (), firstTable);
  stack.Push (session.DBSHandler (), secondTable);

  if (stack[0].GetOperand ().IsNull () == false)
    return false;

  if (stack[1].GetOperand ().IsNull ())
    return false;

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  I_DBSTable& result = stack[0].GetOperand ().GetTable ();

  if (&result != &secondTable)
    return false;

  return true;
}

static bool
test_op_stf (Session& session)
{
  std::cout << "Testing field assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  const DBSUInt32 firstVal (0x31);
  const DBSUInt32 secondVal (0x32);

  DBSFieldDescriptor fd = {"first_field",  T_UINT32, false};

  I_DBSTable& firstTable = session.DBSHandler ().CreateTempTable (1, &fd);
  TableOperand tableOp (session.DBSHandler (), firstTable);

  FieldOperand op;
  FieldOperand op2 (tableOp, 0);
  StackValue   sv1 (op);
  StackValue   sv2 (op2);

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STF, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (sv1);
  stack.Push (sv2);

  if (stack[0].GetOperand ().IsNull () == false)
    return false;

  if (stack[1].GetOperand ().IsNull ())
    return false;

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  I_DBSTable& result = stack[0].GetOperand ().GetTable ();

  if (&result != &firstTable)
    return false;

  if (stack[0].GetOperand ().GetField () != 0)
    return false;

  return true;
}

static bool
test_op_sta (Session& session)
{
  std::cout << "Testing attay assignment...\n";

  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  DBSArray op;
  DBSArray op2;

  op2.AddElement (DBSUInt8 (11));
  op2.AddElement (DBSUInt8 (12));

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_STA, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);
  stack.Push (op2);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSArray result;
  stack[0].GetOperand ().GetValue (result);

  if (result.ElementsCount () != op2.ElementsCount ())
    return false;

  for (uint_t index = 0; index < result.ElementsCount (); ++index)
    {
      DBSUInt8 first, second;

      result.GetElement (first, index);
      op2.GetElement (second, index);

      if (first != second)
        return false;
    }

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

    CompiledBufferUnit dummy (dummyProgram,
                               sizeof dummyProgram,
                               my_postman,
                               dummyProgram);

    commonSession.LoadCompiledUnit (dummy);

    success = success && test_op_stb (_SC (Session&, commonSession));
    success = success && test_op_stc (_SC (Session&, commonSession));
    success = success && test_op_std (_SC (Session&, commonSession));
    success = success && test_op_stdt (_SC (Session&, commonSession));
    success = success && test_op_stht (_SC (Session&, commonSession));
    success = success && test_op_stXX<DBSInt8> (
                                          _SC (Session&, commonSession),
                                          W_STI8,
                                          "8 bit integer"
                                                );
    success = success && test_op_stXX<DBSInt16> (
                                          _SC (Session&, commonSession),
                                          W_STI16,
                                          "16 bit integer"
                                                 );
    success = success && test_op_stXX<DBSInt32> (
                                          _SC (Session&, commonSession),
                                          W_STI32,
                                          "32 bit integer"
                                                 );
    success = success && test_op_stXX<DBSInt64> (
                                          _SC (Session&, commonSession),
                                          W_STI64,
                                          "64 bit integer"
                                                 );
    success = success && test_op_stXX<DBSUInt8> (
                                          _SC (Session&, commonSession),
                                          W_STUI8,
                                          "8 bit unsigned integer"
                                                );
    success = success && test_op_stXX<DBSUInt16> (
                                          _SC (Session&, commonSession),
                                          W_STUI16,
                                          "16 bit unsigned integer"
                                                 );
    success = success && test_op_stXX<DBSUInt32> (
                                          _SC (Session&, commonSession),
                                          W_STUI32,
                                          "32 bit unsigned integer"
                                                 );
    success = success && test_op_stXX<DBSUInt64> (
                                           _SC (Session&, commonSession),
                                           W_STUI64,
                                           "64 bit unsigned integer"
                                                 );

    success = success && test_op_stXX<DBSReal> (
                                           _SC (Session&, commonSession),
                                           W_STR,
                                           "real"
                                                );

    success = success && test_op_stXX<DBSRichReal> (
                                           _SC (Session&, commonSession),
                                           W_STRR,
                                           "rich real"
                                                   );
    success = success && test_op_stt (_SC (Session&, commonSession));
    success = success && test_op_stta (_SC (Session&, commonSession));
    success = success && test_op_stf (_SC (Session&, commonSession));
    success = success && test_op_sta (_SC (Session&, commonSession));

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
