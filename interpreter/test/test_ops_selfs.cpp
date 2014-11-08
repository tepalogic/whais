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
get_line_from_buffer( const char * buffer, uint_t buff_pos)
{
  uint_t count = 0;
  int result = 1;

  if (buff_pos == WHC_IGNORE_BUFFER_POS)
    return -1;

  while( count < buff_pos)
    {
      if (buffer[count] == '\n')
        ++result;
      else if (buffer[count] == 0)
        {
          assert( 0);
        }
      ++count;
    }
  return result;
}

void
my_postman( WH_MESSENGER_CTXT data,
            uint_t            buff_pos,
            uint_t            msg_id,
            uint_t            msgType,
            const char*     pMsgFormat,
            va_list           args)
{
  const char *buffer = (const char *) data;
  int buff_line = get_line_from_buffer( buffer, buff_pos);

  fprintf( stderr, MSG_PREFIX[msgType]);
  fprintf( stderr, "%d : line %d: ", msg_id, buff_line);
  vfprintf( stderr, pMsgFormat, args);
  fprintf( stderr, "\n");
};

static uint_t
w_encode_opcode( W_OPCODE opcode, uint8_t* pOutCode)
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
test_op_saddXX( Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self addition...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);

  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue + second.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_ssubXX( Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self substraction...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);

  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue - second.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_smulXX( Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self multiplication...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);
  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue * second.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sdivXX( Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self division...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);
  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue / second.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_smodXX( Session& session,
                const char* desc,
                const W_OPCODE opcode,
                const DBS_T first,
                const DBS_T second)
{
  std::cout << "Testing " << desc << " self modulo...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);

  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue % second.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sandXX( Session& session,
                const char* desc,
                const W_OPCODE opcode,
                const DBS_T first,
                const DBS_T second)
{
  std::cout << "Testing " << desc << " self and...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);
  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue & second.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sxorXX( Session& session,
                const char* desc,
                const W_OPCODE opcode,
                const DBS_T first,
                const DBS_T second)
{
  std::cout << "Testing " << desc << " self xor...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);
  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue ^ second.mValue))
    return false;

  return true;
}

template <typename DBS_T> bool
test_op_sorXX( Session& session,
               const char* desc,
               const W_OPCODE opcode,
               const DBS_T first,
               const DBS_T second)
{
  std::cout << "Testing " << desc << " self or...\n";
  const uint32_t procId = session.FindProcedure( _RC (const uint8_t*, procName),
                                                 sizeof procName - 1);
  const Procedure& proc   = session.GetProcedure( procId);
  uint8_t* testCode = _CC (uint8_t*, proc.mProcMgr->Code( proc, NULL));
  SessionStack stack;

  uint8_t opSize = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode);
  testCode[opSize++] = 0;
  opSize += w_encode_opcode( W_LDLO8, testCode + opSize);
  testCode[opSize++] = 1;
  opSize += w_encode_opcode( opcode, testCode + opSize);
  w_encode_opcode( W_RET, testCode + opSize);

  stack.Push( first); //This will be modified
  StackValue temp( LocalOperand( stack, 0));
  stack.Push( temp); //Proc args
  stack.Push( second);

  session.ExecuteProcedure( procName, stack);

  if (stack.Size( ) != 2)
    return false;

  DBS_T modified;
  stack[0].Operand( ).GetValue( modified);

  DBS_T result;
  stack[1].Operand( ).GetValue( result);

  if (result != modified)
    return false;

  if (result != DBS_T( first.mValue | second.mValue))
    return false;

  return true;
}





int
main( )
{
  bool success = true;

  {
    DBSInit( DBSSettings( ));
  }

  DBSCreateDatabase( admin);
  InitInterpreter( );

  {
    DBool     val1_b (true), val2_b (false);
    DInt16    val1_i (100), val2_i (26);
    DRichReal val1_rr (123.11), val2_rr (23.12);

    ISession& commonSession = GetInstance( NULL);

    CompiledBufferUnit dummy( dummyProgram,
                               sizeof dummyProgram,
                               my_postman,
                               dummyProgram);

    commonSession.LoadCompiledUnit( dummy);

    success = success && test_op_saddXX( _SC (Session&, commonSession),
                                         "integer",
                                         W_SADD,
                                         val1_i,
                                         val2_i);
    success = success && test_op_saddXX( _SC (Session&, commonSession),
                                         "real",
                                         W_SADDRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_ssubXX( _SC (Session&, commonSession),
                                         "integer",
                                         W_SSUB,
                                         val1_i,
                                         val2_i);

    success = success && test_op_ssubXX( _SC (Session&, commonSession),
                                         "real",
                                         W_SSUBRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_smulXX( _SC (Session&, commonSession),
                                         "integer",
                                         W_SMUL,
                                         val1_i,
                                         val2_i);

    success = success && test_op_smulXX( _SC (Session&, commonSession),
                                         "real",
                                         W_SMULRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_sdivXX( _SC (Session&, commonSession),
                                         "integer",
                                         W_SDIV,
                                         val1_i,
                                         val2_i);

    success = success && test_op_sdivXX( _SC (Session&, commonSession),
                                         "real",
                                         W_SDIVRR,
                                         val1_rr,
                                         val2_rr);
    success = success && test_op_smodXX( _SC (Session&, commonSession),
                                         "integer",
                                         W_SMOD,
                                         val1_i,
                                         val2_i);
    success = success && test_op_sandXX( _SC (Session&, commonSession),
                                         "bool",
                                         W_SANDB,
                                         val1_b,
                                         val2_b);
    success = success && test_op_sandXX( _SC (Session&, commonSession),
                                         "integer",
                                         W_SAND,
                                         val1_i,
                                         val2_i);
    success = success && test_op_sxorXX( _SC (Session&, commonSession),
                                         "bool",
                                         W_SXORB,
                                         val1_b,
                                         val2_b);
    success = success && test_op_sxorXX( _SC (Session&, commonSession),
                                         "integer",
                                         W_SXOR,
                                         val1_i,
                                         val2_i);
    success = success && test_op_sorXX( _SC (Session&, commonSession),
                                        "bool",
                                        W_SORB,
                                        val1_b,
                                        val2_b);
    success = success && test_op_sorXX( _SC (Session&, commonSession),
                                        "integer",
                                        W_SOR,
                                        val1_i,
                                        val2_i);



    ReleaseInstance( commonSession);
  }

  CleanInterpreter( );
  DBSRemoveDatabase( admin);
  DBSShoutdown( );

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
