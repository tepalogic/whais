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

static bool
test_op_stb (Session& session)
{
  std::cout << "Testing bool assignment...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBSBool op;
  DBSBool op2('A');

  D_UINT opSize = w_encode_opcode (W_STB, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBSChar op;
  DBSChar op2('A');

  D_UINT opSize = w_encode_opcode (W_STC, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBSDate op;
  DBSDate op2(1989, 12, 25);

  D_UINT opSize = w_encode_opcode (W_STD, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBSDateTime op;
  DBSDateTime op2(1989, 12, 25, 0, 0, 0);

  D_UINT opSize = w_encode_opcode (W_STDT, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBSHiresTime op;
  DBSHiresTime op2(1989, 12, 25, 0, 0, 0, 0);

  D_UINT opSize = w_encode_opcode (W_STHT, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

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
              const D_CHAR*  pText)
{
  std::cout << "Testing " << pText << " assignment...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBS_T op;
  DBS_T op2(0x61);

  D_UINT opSize = w_encode_opcode (code, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBSText op;
  DBSText op2(_RC (const D_UINT8*, "Testing the best way to future!"));

  D_UINT opSize = w_encode_opcode (W_STT, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  const DBSUInt32 firstVal (0x31);
  const DBSUInt32 secondVal (0x32);

  DBSFieldDescriptor fd = {"first_field",  T_UINT32, false};

  I_DBSTable& firstTable = session.DBSHandler ().CreateTempTable (1, &fd);
  I_DBSTable& secondTable = session.DBSHandler ().CreateTempTable (1, &fd);

  secondTable.SetEntry (secondTable.AddReusedRow (), 0, firstVal);
  secondTable.SetEntry (secondTable.AddReusedRow (), 0, secondVal);

  D_UINT opSize = w_encode_opcode (W_STTA, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (session.DBSHandler (), firstTable);
  stack.Push (session.DBSHandler (), secondTable);

  if (stack[0].GetOperand ().IsNull () == false)
    return false;

  if (stack[1].GetOperand ().IsNull ())
    return false;

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
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

  D_UINT opSize = w_encode_opcode (W_STF, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (sv1);
  stack.Push (sv2);

  if (stack[0].GetOperand ().IsNull () == false)
    return false;

  if (stack[1].GetOperand ().IsNull ())
    return false;

  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  DBSArray op;
  DBSArray op2;

  op2.AddElement (DBSUInt8 (11));
  op2.AddElement (DBSUInt8 (12));

  D_UINT opSize = w_encode_opcode (W_STA, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0, 1);

  stack.Push (op);
  stack.Push (op2);

  proc.Run ();

  if (stack.Size () != 1)
    return false;

  if (stack[0].GetOperand ().IsNull ())
    return false;

  DBSArray result;
  stack[0].GetOperand ().GetValue (result);

  if (result.ElementsCount () != op2.ElementsCount ())
    return false;

  for (D_UINT index = 0; index < result.ElementsCount (); ++index)
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
