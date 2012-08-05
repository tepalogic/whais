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
static const D_UINT8 procName[] = "p1";

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
test_op_addXX (Session& session,
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " addition...\n";
  const D_UINT32 procId = session.FindProcedure (procName, sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
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
              const D_CHAR* pDesc,
              DBSText first,
              DBSText second)
{
  std::cout << "Testing " << pDesc << " addition...\n";
  const D_UINT32 procId = session.FindProcedure (procName, sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
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
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " substraction...\n";
  const D_UINT32 procId = session.FindProcedure (procName, sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
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
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " multiplication ...\n";
  const D_UINT32 procId = session.FindProcedure (procName, sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
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
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " divide ...\n";
  const D_UINT32 procId = session.FindProcedure (procName, sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
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
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBSUInt64 first,
               const DBSUInt64 second)
{
  std::cout << "Testing " << desc << " modulo ...\n";
  const D_UINT32 procId = session.FindProcedure (procName, sizeof procName - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  D_UINT8 opSize = 0;
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
