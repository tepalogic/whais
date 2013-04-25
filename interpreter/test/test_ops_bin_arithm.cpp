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

template <typename DBS_T> bool
test_op_addXX (Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " addition...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );

  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first);
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBS_T result;
  stack[0].GetOperand ().GetValue (result);

  if (first.IsNull ())
    return result == second;
  else if (second.IsNull ())
    return result == first;

  return result == DBS_T (first.m_Value + second.m_Value);
}

static bool
test_op_addt (Session& session,
              const char* pDesc,
              DBSText first,
              DBSText second)
{
  std::cout << "Testing " << pDesc << " addition...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );

  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_ADDT, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first);
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSText result;
  stack[0].GetOperand ().GetValue (result);

  if (result.IsNull ())
    return false;

  first.Append (second);

  return result == first;
}

template <typename DBS_T> bool
test_op_subXX (Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " substraction...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );

  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first);
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBS_T result;
  stack[0].GetOperand ().GetValue (result);

  if (first.IsNull ())
    return result == second;
  else if (second.IsNull ())
    return result == first;

  return result == DBS_T (first.m_Value - second.m_Value);
}

template <typename DBS_T> bool
test_op_mulXX (Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " multiplication ...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );

  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first);
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBS_T result;
  stack[0].GetOperand ().GetValue (result);

  if (first.IsNull ())
    return result == second;
  else if (second.IsNull ())
    return result == first;

  return result == DBS_T (first.m_Value * second.m_Value);
}

template <typename DBS_T> bool
test_op_divXX (Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " divide ...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );

  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first);
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBS_T result;
  stack[0].GetOperand ().GetValue (result);

  if (first.IsNull ())
    return result == second;
  else if (second.IsNull ())
    return result == first;

  return result == DBS_T (first.m_Value / second.m_Value);
}

static bool
test_op_mod (Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBSUInt64 first,
               const DBSUInt64 second)
{
  std::cout << "Testing " << desc << " modulo ...\n";
  const uint32_t procId = session.FindProcedure (
                                              _RC (const uint8_t*, procName),
                                              sizeof procName - 1
                                                );
  uint8_t* testCode = _CC (uint8_t*, session.ProcCode (procId));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first);
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 1)
    return false;

  DBSUInt64 result;
  stack[0].GetOperand ().GetValue (result);

  if (first.IsNull ())
    return result == second;
  else if (second.IsNull ())
    return result == first;

  return result == DBSUInt64 (first.m_Value % second.m_Value);
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

    //Addition

    success = success && test_op_addt (_SC (Session&, commonSession),
                                        "text (first null)",
                                        DBSText (),
                                        DBSText ("World!"));
    success = success && test_op_addt (_SC (Session&, commonSession),
                                        "text (second null)",
                                        DBSText ("Hello, "),
                                        DBSText ());

    success = success && test_op_addt (_SC (Session&, commonSession),
                                        "text (no nulls)",
                                        DBSText ("Hello, "),
                                        DBSText ("World!"));

    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "integer",
                                        W_ADD,
                                        DBSUInt64 (10),
                                        DBSUInt64 (20));

    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "integer with first null",
                                        W_ADD,
                                        DBSUInt8 (),
                                        DBSUInt8 (20));

    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "integer with second null",
                                        W_ADD,
                                        DBSInt16 (10),
                                        DBSInt16 ());
    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "real",
                                        W_ADDR,
                                        DBSReal (10.00),
                                        DBSReal (20.00));
    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "real with first null",
                                        W_ADDR,
                                        DBSReal (),
                                        DBSReal (20.00));
    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "real with second null",
                                        W_ADDR,
                                        DBSReal (10.00),
                                        DBSReal ());

    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "richreal",
                                        W_ADDRR,
                                        DBSRichReal (10.00),
                                        DBSRichReal (20.00));
    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "richreal with first null",
                                        W_ADDRR,
                                        DBSRichReal (),
                                        DBSRichReal (20.00));
    success = success && test_op_addXX (_SC (Session&, commonSession),
                                        "richreal with second null",
                                        W_ADDR,
                                        DBSRichReal (10.00),
                                        DBSRichReal ());


    //Substraction

    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "integer",
                                        W_SUB,
                                        DBSUInt64 (10),
                                        DBSUInt64 (20));

    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "integer with first null",
                                        W_SUB,
                                        DBSUInt8 (),
                                        DBSUInt8 (20));

    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "integer with second null",
                                        W_SUB,
                                        DBSInt16 (10),
                                        DBSInt16 ());
    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "real",
                                        W_SUBR,
                                        DBSReal (10.00),
                                        DBSReal (20.00));
    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "real with first null",
                                        W_SUBR,
                                        DBSReal (),
                                        DBSReal (20.00));
    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "real with second null",
                                        W_SUBR,
                                        DBSReal (10.00),
                                        DBSReal ());

    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "richreal",
                                        W_SUBRR,
                                        DBSRichReal (10.00),
                                        DBSRichReal (20.00));
    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "richreal with first null",
                                        W_SUBRR,
                                        DBSRichReal (),
                                        DBSRichReal (20.00));
    success = success && test_op_subXX (_SC (Session&, commonSession),
                                        "richreal with second null",
                                        W_SUBR,
                                        DBSRichReal (10.00),
                                        DBSRichReal ());

    //Multiplication

    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "integer",
                                        W_MUL,
                                        DBSUInt64 (10),
                                        DBSUInt64 (20));

    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "integer with first null",
                                        W_MUL,
                                        DBSUInt8 (),
                                        DBSUInt8 (20));

    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "integer with second null",
                                        W_MUL,
                                        DBSInt16 (10),
                                        DBSInt16 ());
    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "real",
                                        W_MULR,
                                        DBSReal (10.00),
                                        DBSReal (20.00));
    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "real with first null",
                                        W_MULR,
                                        DBSReal (),
                                        DBSReal (20.00));
    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "real with second null",
                                        W_MULR,
                                        DBSReal (10.00),
                                        DBSReal ());

    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "richreal",
                                        W_MULRR,
                                        DBSRichReal (10.00),
                                        DBSRichReal (20.00));
    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "richreal with first null",
                                        W_MULRR,
                                        DBSRichReal (),
                                        DBSRichReal (20.00));
    success = success && test_op_mulXX (_SC (Session&, commonSession),
                                        "richreal with second null",
                                        W_MULRR,
                                        DBSRichReal (10.00),
                                        DBSRichReal ());

    //Divide

    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "integer",
                                        W_DIV,
                                        DBSUInt64 (10),
                                        DBSUInt64 (20));

    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "integer with first null",
                                        W_DIV,
                                        DBSUInt8 (),
                                        DBSUInt8 (20));

    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "integer with second null",
                                        W_DIV,
                                        DBSInt16 (10),
                                        DBSInt16 ());

    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "real",
                                        W_DIVR,
                                        DBSReal (10.00),
                                        DBSReal (20.00));
    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "real with first null",
                                        W_DIVR,
                                        DBSReal (),
                                        DBSReal (20.00));
    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "real with second null",
                                        W_DIVR,
                                        DBSReal (10.00),
                                        DBSReal ());

    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "richreal",
                                        W_DIVRR,
                                        DBSRichReal (10.00),
                                        DBSRichReal (20.00));
    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "richreal with first null",
                                        W_DIVRR,
                                        DBSRichReal (),
                                        DBSRichReal (20.00));
    success = success && test_op_divXX (_SC (Session&, commonSession),
                                        "richreal with second null",
                                        W_DIVRR,
                                        DBSRichReal (10.00),
                                        DBSRichReal ());


    //Modulo

    success = success && test_op_mod (_SC (Session&, commonSession),
                                      "integer",
                                      W_MOD,
                                      DBSUInt64 (10),
                                      DBSUInt64 (20));

    success = success && test_op_mod (_SC (Session&, commonSession),
                                      "integer with first null",
                                      W_MOD,
                                      DBSUInt64 (),
                                      DBSUInt64 (20));

    success = success && test_op_mod (_SC (Session&, commonSession),
                                      "integer with second null",
                                      W_MOD,
                                      DBSUInt64 (10),
                                      DBSUInt64 ());

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
