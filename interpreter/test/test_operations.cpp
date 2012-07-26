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
test_op_ldnull (Session& session)
{
  std::cout << "Testing ldnull...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDNULL, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0);
  proc.Run ();

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSChar value;

  D_UINT opSize = w_encode_opcode (W_LDC, testCode);
  testCode [opSize + 3] = 0x53;
  testCode [opSize + 2] = 0x52;
  testCode [opSize + 1] = 0x51;
  testCode [opSize + 0] = 0x50;
  Processor proc (session, stack, unusedUnit, testCode, opSize + 4, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSChar (0x53525150))
    return false;

  return true;
}

static bool
test_op_ldi8 (Session& session)
{
  std::cout << "Testing ldi8...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSInt8 value;

  D_UINT opSize = w_encode_opcode (W_LDI8, testCode);
  testCode [opSize + 0] = 0xD8;
  Processor proc (session, stack, unusedUnit, testCode, opSize + 1, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSInt8 (0xD8))
    return false;

  return true;
}

static bool
test_op_ldi16 (Session& session)
{
  std::cout << "Testing ldi16...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSInt16 value;

  D_UINT opSize = w_encode_opcode (W_LDI16, testCode);
  testCode [opSize + 0] = 0xA1;
  testCode [opSize + 1] = 0xB4;
  Processor proc (session, stack, unusedUnit, testCode, opSize + 2, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSInt16 (0xB4A1))
    return false;

  return true;
}

static bool
test_op_ldi32 (Session& session)
{
  std::cout << "Testing ldi32...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSInt32 value;

  D_UINT opSize = w_encode_opcode (W_LDI32, testCode);
  testCode [opSize + 0] = 0xF1;
  testCode [opSize + 1] = 0xD4;
  testCode [opSize + 2] = 0x21;
  testCode [opSize + 3] = 0x34;
  Processor proc (session, stack, unusedUnit, testCode, opSize + 4, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if ((value == DBSInt32 (0x3421D4F1)) == false)
    return false;

  return true;
}

static bool
test_op_ldi64 (Session& session)
{
  std::cout << "Testing ldi64...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSInt64 value;

  D_UINT opSize = w_encode_opcode (W_LDI64, testCode);
  testCode [opSize + 0] = 0xF1;
  testCode [opSize + 1] = 0xD4;
  testCode [opSize + 2] = 0x21;
  testCode [opSize + 3] = 0x34;
  testCode [opSize + 4] = 0x35;
  testCode [opSize + 5] = 0x36;
  testCode [opSize + 6] = 0x37;
  testCode [opSize + 7] = 0x38;
  Processor proc (session, stack, unusedUnit, testCode, opSize + 8, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSInt64 (0x383736353421D4F1))
    return false;

  return true;
}

static bool
test_op_ldd (Session& session)
{
  std::cout << "Testing ldd...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSDate value;

  D_UINT opSize = w_encode_opcode (W_LDD, testCode);
  testCode [opSize + 0] = 0x05;
  testCode [opSize + 1] = 0x06;
  testCode [opSize + 2] = 0xF1;
  testCode [opSize + 3] = 0xD1;
  Processor proc (session, stack, unusedUnit, testCode, opSize + 4, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSDate (0xD1F1, 0x06, 0x05))
    return false;

  return true;
}

static bool
test_op_lddt (Session& session)
{
  std::cout << "Testing lddt...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSDateTime value;

  D_UINT opSize = w_encode_opcode (W_LDDT, testCode);
  testCode [opSize + 0] = 0x23;
  testCode [opSize + 1] = 0x10;
  testCode [opSize + 2] = 0x0B;
  testCode [opSize + 3] = 0x05;
  testCode [opSize + 4] = 0x06;
  testCode [opSize + 5] = 0xF1;
  testCode [opSize + 6] = 0xD1;
  Processor proc (session, stack, unusedUnit, testCode, opSize + 7, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSDateTime (0xD1F1, 0x06, 0x05, 0x0B, 0x10, 0x23))
    return false;

  return true;
}

static bool
test_op_ldht (Session& session)
{
  std::cout << "Testing ldht...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSHiresTime value;

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
  Processor proc (session, stack, unusedUnit, testCode, opSize + 11, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

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
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;
  DBSRichReal value;

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
  Processor proc (session, stack, unusedUnit, testCode, opSize + 9, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSRichReal ((RICHREAL_T)5 / 100))
    return false;

  return true;
}

static bool
test_op_ldt (Session& session)
{
  std::cout << "Testing ldt...\n";
  D_UINT8 testCode[32] = {0,};
  D_UINT32 unitContent[16] = {0,};
  D_CHAR someText[] = "A test to check!\nGood Luck!";

  Unit& unit = *_RC(Unit*, unitContent);
  unit.m_ConstSize = sizeof (someText);

  strcpy ((D_CHAR*)unit.m_UnitData, someText);

  SessionStack stack;
  DBSText      value;

  D_UINT opSize = w_encode_opcode (W_LDT, testCode);
  testCode [opSize + 0]  = 0x02;
  testCode [opSize + 1]  = 0x00;
  testCode [opSize + 2]  = 0x00;
  testCode [opSize + 3]  = 0x00;
  Processor proc (session, stack, unit, testCode, opSize + 4, 0);
  proc.Run ();

  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSText ((D_UINT8*)someText + 2))
    return false;

  return true;
}

static bool
test_op_ldbt (Session& session)
{
  std::cout << "Testing ldbt...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDBT, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0);
  proc.Run ();

  DBSBool value;
  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if ((value == DBSBool (true)) == false)
    return false;

  return true;
}

static bool
test_op_ldbf (Session& session)
{
  std::cout << "Testing ldbf...\n";
  D_UINT8 testCode[32] = {0,};
  SessionStack stack;

  D_UINT opSize = w_encode_opcode (W_LDBF, testCode);
  Processor proc (session, stack, unusedUnit, testCode, opSize, 0);
  proc.Run ();

  DBSBool value;
  if (stack.Size () != 1)
    return false;

  stack[0].GetOperand ().GetValue (value);
  if (value != DBSBool (false))
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

