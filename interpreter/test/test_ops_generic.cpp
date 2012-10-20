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
static const D_UINT8 cts_proc[] = "cts_test";
static const D_UINT8 inull_proc[] = "inull_test";
static const D_UINT8 nnull_proc[] = "nnull_test";
static const D_UINT8 field_proc[] = "field_index";

const D_UINT8 callTestProgram[] = ""
    "PROCEDURE cts_test (n AS INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE inull_test (n AS INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE nnull_test (n AS INT8) RETURN INT8\n"
    "DO\n"
      "n = n +1;"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE p1 (n AS INT8) RETURN INT8\n"
    "DO\n"
      "RETURN n * 2;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE p2 (n AS INT8, n2 AS INT8) RETURN INT8\n"
    "DO\n"
      "RETURN p1 (n + n2);\n"
    "ENDPROC\n"
    "PROCEDURE text_index (t1 AS TEXT) RETURN CHARACTER\n"
    "DO\n"
      "LET temp AS CHARACTER;\n"
      "\n"
      "temp = t1[0];\n"
      "t1[0] = 'B';\n"
      "RETURN temp;\n"
    "ENDPROC\n"
    "PROCEDURE array_index (a1 AS ARRAY OF INT8) RETURN INT8\n"
    "DO\n"
      "LET temp AS INT8;\n"
      "\n"
      "temp = a1[0];\n"
      "a1[0] = 10;\n"
      "RETURN temp;\n"
    "ENDPROC\n"
    "PROCEDURE table_index (tab AS TABLE OF (value AS INT8)) RETURN INT8\n"
    "DO\n"
      "LET temp AS INT8;\n"
      "\n"
      "temp = tab[0, value];\n"
      "tab[0, value] = 10;\n"
      "RETURN temp;\n"
    "ENDPROC\n"
    "PROCEDURE field_index (tab AS TABLE OF (value AS INT8)) RETURN INT8\n"
    "DO\n"
      "LET temp AS INT8;\n"
      "LET tabf AS FIELD OF INT8;"
      "\n"
      "SYNC\n"
        "tabf = tab.value;\n"
        "temp = tabf[0];\n"
        "tabf[0] = 10;\n"
      "ENDSYNC\n"
      "RETURN temp;\n"
    "ENDPROC\n";


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
}

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
test_op_cts (Session& session)
{
  std::cout << "Testing opcode cts...\n";
  const D_UINT32 procId = session.FindProcedure (cts_proc, sizeof cts_proc - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSInt8 op(-10);

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_CTS, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op); //A procedure should return a value!

  session.ExecuteProcedure (cts_proc, stack);

  if (stack.Size () != 1)
    return false;

  return true;
}

static bool
test_op_inull (Session& session)
{
  std::cout << "Testing opcode inull...\n";
  const D_UINT32 procId = session.FindProcedure (inull_proc,
                                                 sizeof inull_proc - 1);

  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSDate op;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_INULL, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);

  session.ExecuteProcedure (inull_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSBool result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSBool (true) )
    return false;

  return true;
}

static bool
test_op_nnull (Session& session)
{
  std::cout << "Testing opcode nnull...\n";
  const D_UINT32 procId = session.FindProcedure (nnull_proc,
                                                 sizeof nnull_proc - 1);
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSDate op;

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_NNULL, testCode + opSize);
  w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op); //A procedure should return a value!

  session.ExecuteProcedure (nnull_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSBool result;
  stack[0].GetOperand ().GetValue (result);

  if (stack.Size () != 1)
    return false;

  if (result == DBSBool (true) )
    return false;

  return true;
}

static bool
test_op_call (Session& session)
{
  std::cout << "Testing opcode call...\n";
  SessionStack stack;

  stack.Push (DBSUInt8 (10));
  stack.Push (DBSUInt16 (2));

  session.ExecuteProcedure (_RC (const D_UINT8*, "p2"), stack);

  if (stack.Size () != 1)
    return false;

  DBSUInt8 result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSUInt8 (24) )
    return false;

  return true;
}

static bool
test_op_text_index (Session& session)
{
  std::cout << "Testing opcode for text indexing...\n";
  SessionStack stack;
  DBSChar value('A');
  LocalOperand localOp(stack, 0);

  DBSText text ("A");
  stack.Push (text);
  stack.Push (StackValue (localOp));

  session.ExecuteProcedure (_RC (const D_UINT8*, "text_index"), stack);

  if (stack.Size () != 2)
    return false;

  DBSChar result;
  stack[1].GetOperand ().GetValue (result);

  if (result != value )
    return false;

  stack[0].GetOperand ().GetValue (text);
  value = text.GetCharAtIndex (0);

  if (value != DBSChar('B') )
    return false;

  return true;
}

static bool
test_op_array_index (Session& session)
{
  std::cout << "Testing opcode for array indexing...\n";
  SessionStack stack;
  DBSInt8 value(0x23);
  LocalOperand localOp(stack, 0);

  DBSArray array;
  array.AddElement (value);

  stack.Push (array);
  stack.Push (StackValue (localOp));

  session.ExecuteProcedure (_RC (const D_UINT8*, "array_index"), stack);

  if (stack.Size () != 2)
    return false;

  DBSInt8 result;
  stack[1].GetOperand ().GetValue (result);

  if (result != value )
    return false;

  stack[0].GetOperand ().GetValue (array);
  array.GetElement (value, 0);

  if (value != DBSInt8 (10) )
    return false;

  return true;
}

static bool
test_op_table_index (Session& session)
{
  std::cout << "Testing opcode for table indexing...\n";
  SessionStack stack;

  DBSFieldDescriptor fd = { "value", T_INT8, false};
  DBSInt8 value(0x23);
  LocalOperand localOp(stack, 0);

  I_DBSTable& tempTable = session.DBSHandler ().CreateTempTable (1, &fd);
  tempTable.AddReusedRow ();
  tempTable.SetEntry (0, 0, value);

  stack.Push (session.DBSHandler (), tempTable);
  stack.Push (StackValue (localOp));

  session.ExecuteProcedure (_RC (const D_UINT8*, "table_index"), stack);

  if (stack.Size () != 2)
    return false;

  DBSInt8 result;
  stack[1].GetOperand ().GetValue (result);

  if (result != value )
    return false;

  I_DBSTable& stackTable = stack[0].GetOperand ().GetTable ();

  if (&stackTable != &tempTable)
    return false;

  stackTable.GetEntry (0, 0, value);
  if (value != DBSInt8  (10) )
    return false;

  return true;
}

static bool
test_op_field_index (Session& session)
{
  std::cout << "Testing opcode for field indexing...\n";
  SessionStack stack;

  DBSFieldDescriptor fd = { "value", T_INT8, false};
  DBSInt8 value(0x23);
  LocalOperand localOp(stack, 0);

  I_DBSTable& tempTable = session.DBSHandler ().CreateTempTable (1, &fd);
  tempTable.AddReusedRow ();
  tempTable.SetEntry (0, 0, value);

  stack.Push (session.DBSHandler (), tempTable);
  stack.Push (StackValue (localOp));

  session.ExecuteProcedure (_RC (const D_UINT8*, "field_index"), stack);

  if (stack.Size () != 2)
    return false;

  DBSInt8 result;
  stack[1].GetOperand ().GetValue (result);

  if (result != value )
    return false;

  I_DBSTable& stackTable = stack[0].GetOperand ().GetTable ();

  if (&stackTable != &tempTable)
    return false;

  stackTable.GetEntry (0, 0, value);
  if (value != DBSInt8  (10) )
    return false;

  return true;
}

static bool
test_op_jfc (Session& session)
{
  std::cout << "Testing opcode jfc...\n";
  const D_UINT32 procId = session.FindProcedure (field_proc,
                                                 sizeof field_proc - 1);
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSBool op(false);

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_JFC, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_RET, testCode + opSize);
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt8 result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (0) )
    return false;

  stack.Pop (1);

  op = DBSBool (true);
  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

   stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (1) )
    return false;

  return true;
}

static bool
test_op_jtc (Session& session)
{
  std::cout << "Testing opcode jtc...\n";
  const D_UINT32 procId = session.FindProcedure (field_proc,
                                                 sizeof field_proc - 1);
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSBool op(true);

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_JTC, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_RET, testCode + opSize);
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt8 result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (0) )
    return false;

  stack.Pop (1);

  op = DBSBool (false);
  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

   stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (1) )
    return false;

  return true;
}


static bool
test_op_jf (Session& session)
{
  std::cout << "Testing opcode jf...\n";
  const D_UINT32 procId = session.FindProcedure (field_proc,
                                                 sizeof field_proc - 1);
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSBool op(false);

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_JF, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_RET, testCode + opSize);
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt8 result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (0) )
    return false;

  stack.Pop (1);

  op = DBSBool (true);
  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

   stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (1) )
    return false;

  return true;
}

static bool
test_op_jt (Session& session)
{
  std::cout << "Testing opcode jt...\n";
  const D_UINT32 procId = session.FindProcedure (field_proc,
                                                 sizeof field_proc - 1);
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSBool op(true);

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_JT, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_RET, testCode + opSize);
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt8 result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (0) )
    return false;

  stack.Pop (1);

  op = DBSBool (false);
  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

   stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (1) )
    return false;

  return true;
}


static bool
test_op_jmp (Session& session)
{
  std::cout << "Testing opcode jmp...\n";
  const D_UINT32 procId = session.FindProcedure (field_proc,
                                                 sizeof field_proc - 1);
  D_UINT8* testCode = _CC (D_UINT8*, session.ProcCode (procId));
  SessionStack stack;

  DBSBool op(true);

  D_UINT8 opSize = 0;
  opSize += w_encode_opcode (W_JMP, testCode + opSize);
  testCode[opSize++] = 8;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  testCode[opSize++] = 0;
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode (W_RET, testCode + opSize);
  opSize += w_encode_opcode (W_LDI8, testCode + opSize);
  testCode[opSize++] = 10;
  opSize += w_encode_opcode (W_RET, testCode + opSize);

  stack.Push (op);

  session.ExecuteProcedure (field_proc, stack);

  if (stack.Size () != 1)
    return false;

  DBSInt8 result;
  stack[0].GetOperand ().GetValue (result);

  if (result != DBSInt8 (10) )
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

    WBufferCompiledUnit callBuf (callTestProgram,
                                 sizeof callTestProgram,
                                 my_postman,
                                 callTestProgram);

    commonSession.LoadCompiledUnit (callBuf);

    success = success && test_op_cts (_SC (Session&, commonSession));
    success = success && test_op_inull (_SC (Session&, commonSession));
    success = success && test_op_nnull (_SC (Session&, commonSession));
    success = success && test_op_call (_SC (Session&, commonSession));
    success = success && test_op_text_index (_SC (Session&, commonSession));
    success = success && test_op_array_index (_SC (Session&, commonSession));
    success = success && test_op_table_index (_SC (Session&, commonSession));
    success = success && test_op_field_index (_SC (Session&, commonSession));
    success = success && test_op_jfc (_SC (Session&, commonSession));
    success = success && test_op_jtc (_SC (Session&, commonSession));
    success = success && test_op_jf (_SC (Session&, commonSession));
    success = success && test_op_jt (_SC (Session&, commonSession));
    success = success && test_op_jmp (_SC (Session&, commonSession));

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
