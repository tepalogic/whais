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
    "PROCEDURE p1 (a1 AS INT8, a2 AS INT8) RETURN BOOL\n"
    "DO\n"
      "LET hd1, hd2 AS HIRESTIME\n;"
      "\n"
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

template <typename DBS_T> bool
test_op_saddXX (Session& session,
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self addition...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value + second.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_ssubXX (Session& session,
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self substraction...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value - second.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_smulXX (Session& session,
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self multiplication...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value * second.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sdivXX (Session& session,
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self division...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value / second.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_smodXX (Session& session,
                const D_CHAR* desc,
                const W_OPCODE opcode,
                const DBS_T first,
                const DBS_T second)
{
  std::cout << "Testing " << desc << " self modulo...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value % second.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sandXX (Session& session,
                const D_CHAR* desc,
                const W_OPCODE opcode,
                const DBS_T first,
                const DBS_T second)
{
  std::cout << "Testing " << desc << " self and...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value & second.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sxorXX (Session& session,
                const D_CHAR* desc,
                const W_OPCODE opcode,
                const DBS_T first,
                const DBS_T second)
{
  std::cout << "Testing " << desc << " self xor...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value ^ second.m_Value))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sorXX (Session& session,
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self or...\n";
  const D_UINT32 procId = session.FindProcedure (_RC (const D_UINT8*, procName),
                                                 sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (opcode, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (first); //This will be modified
  StackValue temp (LocalOperand (stack, 0));
  stack.Push (temp); //Proc args
  stack.Push (second);

  session.ExecuteProcedure (procName, stack);

  if (stack.Size () != 2)
    return false;

  DBS_T modified;
  stack[0].GetOperand ().GetValue (modified);

  DBS_T result;
  stack[1].GetOperand ().GetValue (result);

  if (result != modified)
    return false;

  if (result != DBS_T (first.m_Value | second.m_Value))
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
    DBSBool     val1_b (true), val2_b (false);
    DBSInt16    val1_i (100), val2_i (26);
    DBSRichReal val1_rr (123.11), val2_rr (23.12);

    I_Session& commonSession = GetInstance (NULL);

    WBufferCompiledUnit dummy (dummyProgram,
                               sizeof dummyProgram,
                               my_postman,
                               dummyProgram);

    commonSession.LoadCompiledUnit (dummy);

    success = success && test_op_saddXX (_SC (Session&, commonSession),
                                         "integer",
                                         W_SADD,
                                         val1_i,
                                         val2_i);
    success = success && test_op_saddXX (_SC (Session&, commonSession),
                                         "real",
                                         W_SADDRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_ssubXX (_SC (Session&, commonSession),
                                         "integer",
                                         W_SSUB,
                                         val1_i,
                                         val2_i);

    success = success && test_op_ssubXX (_SC (Session&, commonSession),
                                         "real",
                                         W_SSUBRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_smulXX (_SC (Session&, commonSession),
                                         "integer",
                                         W_SMUL,
                                         val1_i,
                                         val2_i);

    success = success && test_op_smulXX (_SC (Session&, commonSession),
                                         "real",
                                         W_SMULRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_sdivXX (_SC (Session&, commonSession),
                                         "integer",
                                         W_SDIV,
                                         val1_i,
                                         val2_i);

    success = success && test_op_sdivXX (_SC (Session&, commonSession),
                                         "real",
                                         W_SDIVRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_smodXX (_SC (Session&, commonSession),
                                         "integer",
                                         W_SMOD,
                                         val1_i,
                                         val2_i);
    success = success && test_op_sandXX (_SC (Session&, commonSession),
                                         "bool",
                                         W_SANDB,
                                         val1_b,
                                         val2_b);
    success = success && test_op_sandXX (_SC (Session&, commonSession),
                                         "integer",
                                         W_SAND,
                                         val1_i,
                                         val2_i);
    success = success && test_op_sxorXX (_SC (Session&, commonSession),
                                         "bool",
                                         W_SXORB,
                                         val1_b,
                                         val2_b);
    success = success && test_op_sxorXX (_SC (Session&, commonSession),
                                         "integer",
                                         W_SXOR,
                                         val1_i,
                                         val2_i);
    success = success && test_op_sorXX (_SC (Session&, commonSession),
                                        "bool",
                                        W_SORB,
                                        val1_b,
                                        val2_b);
    success = success && test_op_sorXX (_SC (Session&, commonSession),
                                        "integer",
                                        W_SOR,
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
