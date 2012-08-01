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

static Unit unusedUnit = {0, };

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

template <typename DBS_T> bool
test_op_addXX (Session& session,
               const D_CHAR* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " addition...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (opcode, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (first);
  stack.Push (second);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_ADDT, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (first);
  stack.Push (second);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (opcode, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (first);
  stack.Push (second);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (opcode, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (first);
  stack.Push (second);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (opcode, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (first);
  stack.Push (second);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (opcode, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (first);
  stack.Push (second);

  proc.Run ();

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
