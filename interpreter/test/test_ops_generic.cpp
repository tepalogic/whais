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

using namespace whais;
using namespace prima;

static const char admin[]      = "administrator";
static const char cts_proc[]   = "cts_test";
static const char inull_proc[] = "inull_test";
static const char nnull_proc[] = "nnull_test";
static const char field_proc[] = "field_index";

const uint8_t callTestProgram[] = ""
    "PROCEDURE cts_test(n INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE inull_test(n INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE nnull_test(n INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE p1(n INT8) RETURN INT8\n"
    "DO\n"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE p2(n INT8, n2 INT8) RETURN INT8\n"
    "DO\n"
      "RETURN p1(n + n2);\n"
    "ENDPROC\n"
    "PROCEDURE text_index(t1 TEXT) RETURN CHAR\n"
    "DO\n"
      "VAR temp CHAR;\n"
      "\n"
      "temp = t1[0];\n"
      "t1[0] = 'B';\n"
      "RETURN temp;\n"
    "ENDPROC\n"
    "PROCEDURE array_index(a1 INT8 ARRAY) RETURN INT8\n"
    "DO\n"
      "VAR temp INT8;\n"
      "\n"
      "temp = a1[0];\n"
      "a1[0] = 10;\n"
      "RETURN temp;\n"
    "ENDPROC\n"
    "PROCEDURE table_index(tab TABLE(value INT8)) RETURN INT8\n"
    "DO\n"
      "VAR temp INT8;\n"
      "\n"
      "temp = tab[0, value];\n"
      "tab[0, value] = 10;\n"
      "RETURN temp;\n"
    "ENDPROC\n"
    "PROCEDURE field_index(tab TABLE(value INT8)) RETURN INT8\n"
    "DO\n"
      "VAR temp INT8;\n"
      "VAR tabf INT8 FIELD;"
      "\n"
      "SYNC\n"
        "tabf = tab.value;\n"
        "temp = tabf[0];\n"
        "tabf[0] = 10;\n"
      "ENDSYNC\n"
      "RETURN temp;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE array_parse(ai INT8 ARRAY, count INT8) RETURN INT8\n"
    "DO\n"
    " VAR result INT8;\n"
    "\n"
    " FOR(i : ai)\n"
    " DO\n"
    "   IF(count < 0)\n"
    "     BREAK;\n"
    "   result = i;\n"
    "   count -= 1;\n"
    " END\n"
    "\n"
    " RETURN result;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE array_parse_reverse(ai INT8 ARRAY, count INT8) RETURN INT8\n"
    "DO\n"
    " VAR result INT8;\n"
    "\n"
    " FOR( ! i : ai)\n"
    " DO\n"
    "   IF(count < 0)\n"
    "     BREAK;\n"
    "   result = i;\n"
    "   count -= 1;\n"
    " END\n"
    "\n"
    " RETURN result;\n"
    "ENDPROC\n";



static const char *MSG_PREFIX[] = {
                                      "", "error ", "warning ", "error "
                                    };

static uint_t
get_line_from_buffer(const char * buffer, uint_t buff_pos)
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
          assert(0);
        }
      ++count;
    }
  return result;
}

void
my_postman(WH_MESSENGER_CTXT data,
            uint_t            buff_pos,
            uint_t            msg_id,
            uint_t            msgType,
            const char*     pMsgFormat,
            va_list           args)
{
  const char *buffer = (const char *) data;
  int buff_line = get_line_from_buffer(buffer, buff_pos);

  fprintf(stderr, MSG_PREFIX[msgType]);
  fprintf(stderr, "%d : line %d: ", msg_id, buff_line);
  vfprintf(stderr, pMsgFormat, args);
  fprintf(stderr, "\n");
}

static uint_t
w_encode_opcode(W_OPCODE opcode, uint8_t* pOutCode)
{
  if (opcode > 0x80)
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
test_op_cts(Session& session)
{
  std::cout << "Testing opcode cts...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, cts_proc),
                                                 sizeof cts_proc - 1);

  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DInt8 op(-10);

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_CTS, testCode + opSize);
  w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op); //A procedure should return a value!

  session.ExecuteProcedure(cts_proc, stack);

  if (stack.Size() != 1)
    return false;

  return true;
}

static bool
test_op_inull(Session& session)
{
  std::cout << "Testing opcode inull...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, inull_proc),
                                                 sizeof inull_proc - 1);
  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DDate op;

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_INULL, testCode + opSize);
  w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op);

  session.ExecuteProcedure(inull_proc, stack);

  if (stack.Size() != 1)
    return false;

  DBool result;
  stack[0].Operand().GetValue(result);

  if (result != DBool(true) )
    return false;

  return true;
}

static bool
test_op_nnull(Session& session)
{
  std::cout << "Testing opcode nnull...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, nnull_proc),
                                                 sizeof nnull_proc - 1);
  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DDate op;

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_NNULL, testCode + opSize);
  w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op); //A procedure should return a value!

  session.ExecuteProcedure(nnull_proc, stack);

  if (stack.Size() != 1)
    return false;

  DBool result;
  stack[0].Operand().GetValue(result);

  if (stack.Size() != 1)
    return false;

  if (result == DBool(true) )
    return false;

  return true;
}

static bool
test_op_call(Session& session)
{
  std::cout << "Testing opcode call...\n";
  SessionStack stack;

  stack.Push(DUInt8(10));
  stack.Push(DUInt16(2));

  session.ExecuteProcedure("p2", stack);

  if (stack.Size() != 1)
    return false;

  DUInt8 result;
  stack[0].Operand().GetValue(result);

  if (result != DUInt8(24) )
    return false;

  return true;
}

static bool
test_op_text_index(Session& session)
{
  std::cout << "Testing opcode for text indexing...\n";
  SessionStack stack;
  DChar value('A');
  LocalOperand localOp(stack, 0);

  DText text("A");
  stack.Push(text);
  stack.Push(StackValue(localOp));

  session.ExecuteProcedure("text_index", stack);

  if (stack.Size() != 2)
    return false;

  DChar result;
  stack[1].Operand().GetValue(result);

  if (result != value )
    return false;

  stack[0].Operand().GetValue(text);
  value = text.CharAt(0);

  if (value != DChar('B') )
    return false;

  return true;
}

static bool
test_op_array_index(Session& session)
{
  std::cout << "Testing opcode for array indexing...\n";
  SessionStack stack;
  DInt8 value(0x23);
  LocalOperand localOp(stack, 0);

  DArray array;
  array.Add(value);

  stack.Push(array);
  stack.Push(StackValue(localOp));

  session.ExecuteProcedure("array_index", stack);

  if (stack.Size() != 2)
    return false;

  DInt8 result;
  stack[1].Operand().GetValue(result);

  if (result != value )
    return false;

  stack[0].Operand().GetValue(array);
  array.Get(0, value);

  if (value != DInt8(10) )
    return false;

  return true;
}

static bool
test_op_table_index(Session& session)
{
  std::cout << "Testing opcode for table indexing...\n";
  SessionStack stack;

  DBSFieldDescriptor fd = { "value", T_INT8, false};
  DInt8 value(0x23);
  LocalOperand localOp(stack, 0);

  ITable& tempTable = session.DBSHandler().CreateTempTable(1, &fd);
  tempTable.GetReusableRow(true);
  tempTable.Set(0, 0, value);

  stack.Push(tempTable);
  stack.Push(StackValue(localOp));

  session.ExecuteProcedure("table_index", stack);

  if (stack.Size() != 2)
    return false;

  DInt8 result;
  stack[1].Operand().GetValue(result);

  if (result != value )
    return false;

  ITable& stackTable = stack[0].Operand().GetTable();

  if (&stackTable != &tempTable)
    return false;

  stackTable.Get(0, 0, value);
  if (value != DInt8  (10) )
    return false;

  return true;
}

static bool
test_op_field_index(Session& session)
{
  std::cout << "Testing opcode for field indexing...\n";
  SessionStack stack;

  DBSFieldDescriptor fd = { "value", T_INT8, false};
  DInt8 value(0x23);
  LocalOperand localOp(stack, 0);

  ITable& tempTable = session.DBSHandler().CreateTempTable(1, &fd);
  tempTable.GetReusableRow(true);
  tempTable.Set(0, 0, value);

  stack.Push(tempTable);
  stack.Push(StackValue(localOp));

  session.ExecuteProcedure("field_index", stack);

  if (stack.Size() != 2)
    return false;

  DInt8 result;
  stack[1].Operand().GetValue(result);

  if (result != value )
    return false;

  ITable& stackTable = stack[0].Operand().GetTable();

  if (&stackTable != &tempTable)
    return false;

  stackTable.Get(0, 0, value);
  if (value != DInt8  (10) )
    return false;

  return true;
}

static bool
test_op_jfc(Session& session)
{
  std::cout << "Testing opcode jfc...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, field_proc),
                                                 sizeof field_proc - 1);

  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DBool op(false);

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_JFC, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode(W_RET, testCode + opSize);
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

  DInt8 result;
  stack[0].Operand().GetValue(result);

  if (result != DInt8(0) )
    return false;

  stack.Pop(1);

  op = DBool(true);
  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

   stack[0].Operand().GetValue(result);

  if (result != DInt8(1) )
    return false;

  return true;
}

static bool
test_op_jtc(Session& session)
{
  std::cout << "Testing opcode jtc...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, field_proc),
                                                 sizeof field_proc - 1);

  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DBool op(true);

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_JTC, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode(W_RET, testCode + opSize);
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

  DInt8 result;
  stack[0].Operand().GetValue(result);

  if (result != DInt8(0) )
    return false;

  stack.Pop(1);

  op = DBool(false);
  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

   stack[0].Operand().GetValue(result);

  if (result != DInt8(1) )
    return false;

  return true;
}


static bool
test_op_jf(Session& session)
{
  std::cout << "Testing opcode jf...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, field_proc),
                                                 sizeof field_proc - 1);
  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DBool op(false);

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_JF, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode(W_RET, testCode + opSize);
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

  DInt8 result;
  stack[0].Operand().GetValue(result);

  if (result != DInt8(0) )
    return false;

  stack.Pop(1);

  op = DBool(true);
  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

   stack[0].Operand().GetValue(result);

  if (result != DInt8(1) )
    return false;

  return true;
}

static bool
test_op_jt(Session& session)
{
  std::cout << "Testing opcode jt...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, field_proc),
                                                 sizeof field_proc - 1);
  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DBool op(true);

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_JT, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode(W_RET, testCode + opSize);
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

  DInt8 result;
  stack[0].Operand().GetValue(result);

  if (result != DInt8(0) )
    return false;

  stack.Pop(1);

  op = DBool(false);
  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

   stack[0].Operand().GetValue(result);

  if (result != DInt8(1) )
    return false;

  return true;
}


static bool
test_op_jmp(Session& session)
{
  std::cout << "Testing opcode jmp...\n";
  const uint32_t procId = session.FindProcedure(_RC(const uint8_t*, field_proc),
                                                 sizeof field_proc - 1);
  const Procedure& proc   = session.GetProcedure(procId);
  uint8_t* testCode = _CC(uint8_t*, proc.mProcMgr->Code(proc, nullptr));
  SessionStack stack;

  DBool op(true);

  uint8_t opSize = 0;
  opSize += w_encode_opcode(W_JMP, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode(W_RET, testCode + opSize);
  opSize += w_encode_opcode(W_LDI8, testCode + opSize);
  testCode[opSize++] = 10;
  opSize += w_encode_opcode(W_RET, testCode + opSize);

  stack.Push(op);

  session.ExecuteProcedure(field_proc, stack);

  if (stack.Size() != 1)
    return false;

  DInt8 result;
  stack[0].Operand().GetValue(result);

  if (result != DInt8(10) )
    return false;

  return true;
}

static bool
test_op_array_parse(Session& session, const bool reverse)
{
  if (reverse)
    std::cout << "Testing opcode for array parse(reverse)...\n";
  else
    std::cout << "Testing opcode for array parse...\n";

  SessionStack stack;
  DInt8 value;
  DArray array;

  stack.Push(array);
  stack.Push(value);

  if (reverse)
    session.ExecuteProcedure("array_parse_reverse", stack);
  else
    session.ExecuteProcedure("array_parse", stack);

  if (stack.Size() != 1)
    return false;

  DInt8 result;
  stack[0].Operand().GetValue(result);

  if (! result.IsNull())
    return false;

  stack.Pop(1);

  if (stack.Size() != 0)
    return false;

  array.Add(DInt8(10));
  array.Add(DInt8(-25));

  stack.Push(array);
  stack.Push(DInt8(0));

  if (reverse)
    session.ExecuteProcedure("array_parse_reverse", stack);
  else
    session.ExecuteProcedure("array_parse", stack);

  if (stack.Size() != 1)
    return false;

  stack[0].Operand().GetValue(result);
  if (reverse)
    {
      if (result != DInt8(-25))
        return false;
    }
  else
    {
      if (result != DInt8(10))
        return false;
     }

  return true;
}


int
main()
{
  bool success = true;

  {
    DBSInit(DBSSettings());
  }

  DBSCreateDatabase(admin);
  InitInterpreter();

  {
    ISession& commonSession = GetInstance(nullptr);

    CompiledBufferUnit callBuf(callTestProgram,
                                 sizeof callTestProgram,
                                 my_postman,
                                 callTestProgram);

    commonSession.LoadCompiledUnit(callBuf);

    success = success && test_op_cts(_SC(Session&, commonSession));
    success = success && test_op_inull(_SC(Session&, commonSession));
    success = success && test_op_nnull(_SC(Session&, commonSession));
    success = success && test_op_call(_SC(Session&, commonSession));
    success = success && test_op_text_index(_SC(Session&, commonSession));
    success = success && test_op_array_index(_SC(Session&, commonSession));
    success = success && test_op_table_index(_SC(Session&, commonSession));
    success = success && test_op_field_index(_SC(Session&, commonSession));
    success = success && test_op_jfc(_SC(Session&, commonSession));
    success = success && test_op_jtc(_SC(Session&, commonSession));
    success = success && test_op_jf(_SC(Session&, commonSession));
    success = success && test_op_jt(_SC(Session&, commonSession));
    success = success && test_op_jmp(_SC(Session&, commonSession));
    success = success && test_op_array_parse(_SC(Session&, commonSession), false);
    success = success && test_op_array_parse(_SC(Session&, commonSession), true);

    ReleaseInstance(commonSession);
  }

  CleanInterpreter();
  DBSRemoveDatabase(admin);
  DBSShoutdown();
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
