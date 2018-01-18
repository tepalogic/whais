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
    "PROCEDURE just_null() RETURN BOOL DO RETURN NULL; ENDPROC\n"
    "PROCEDURE just_null_field() RETURN BOOL FIELD DO RETURN NULL; ENDPROC\n"
    "PROCEDURE test_1() RETURN ARRAY\n"
    "DO\n"
      "a = {TRUE};\n"
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_2() RETURN ARRAY\n"
    "DO\n"
      "a = {FALSE, TRUE}; "
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_3() RETURN ARRAY\n"
    "DO\n"
      "a = {TRUE, just_null(), FALSE}; "
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_4() RETURN ARRAY\n"
    "DO\n"
      "a = {'c'};\n"
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_5() RETURN ARRAY\n"
    "DO\n"
      "a = {'1981-12-21', '1982-12-22', '1983-12-23'}; "
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_6() RETURN ARRAY\n"
    "DO\n"
      "a = {'2081-12-21'} DATETIME; "
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_7() RETURN ARRAY\n"
    "DO\n"
      "a = {'3081-12-21'} HIRESTIME; "
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_8() RETURN ARRAY\n"
    "DO\n"
      "a = {1} ;\n"
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_9() RETURN ARRAY\n"
    "DO\n"
      "a = {2} UINT16; "
      "RETURN a;\n"
    "ENDPROC\n"
    "PROCEDURE test_10() RETURN ARRAY\n"
    "DO\n"
      "a = {3} UINT32; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_11() RETURN ARRAY\n"
    "DO\n"
      "a = {4} UINT8; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_12() RETURN ARRAY\n"
    "DO\n"
      "a = {-1} INT8; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_13() RETURN ARRAY\n"
    "DO\n"
      "a = {-2} INT16; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_14() RETURN ARRAY\n"
    "DO\n"
      "a = {-3} INT32; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_15() RETURN ARRAY\n"
    "DO\n"
      "a = {-4} INT64; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_16() RETURN ARRAY\n"
    "DO\n"
      "a = {-1.0}; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_17() RETURN ARRAY\n"
    "DO\n"
      "a = {1} REAL; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_18() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (t1 TEXT, t2 REAL); "
    "  a = {@t.t1}; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_19() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (t1 TEXT, t2 REAL); "
    "  a = {t.t1}; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_20() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (t1 TEXT, t2 REAL); "
    "  a = {t.t2} INT8; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_21() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (t1 TEXT, t2 REAL); "
    "  a = {t.t1} INT16; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_22() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (t1 TEXT, t2 REAL); "
    "  a = {t.t1} INT32; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_23() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (t1 TEXT, t2 REAL); "
    "  a = {t.t1} INT64; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_24() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (f1 TEXT, f2 REAL); "
    "  a = {t.f1, t.f2, t.f1} UINT64; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_25() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (f1 TEXT, f2 REAL); "
    "  a = {t.f1, t.f2, t.f1} UINT16; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_26() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (f1 TEXT, f2 REAL); "
    "  a = {t.f1, t.f2} UINT32; "
      "RETURN a; "
    "ENDPROC\n"
    "PROCEDURE test_27() RETURN ARRAY\n"
    "DO\n"
    "  VAR t TABLE (f1 TEXT, f2 REAL); "
    "  a = {t.f2, just_null_field(), t.f1} UINT8; "
      "RETURN a; "
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

template<typename T> bool
test_array_op(Session& session,
              const char* proc,
              const DBS_BASIC_TYPE type,
              const std::vector<T> expected)
{
  std::cout << "Testing procedure " << proc << " ... ";

  SessionStack stack;
  session.ExecuteProcedure(proc, stack);
  if (stack.Size() != 1)
    return false;

  DArray result (_SC(const T*, nullptr));
  stack[0].Operand().GetValue(result);

  if (result.Type() != type)
    return false;

  else if (result.Count() != expected.size())
    return false;

  for (size_t i = 0; i < expected.size(); ++i)
  {
    T temp;
    result.Get(i, temp);
    if (temp != expected[i])
      return false;
  }

  std::cout << "PASS\n";
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


    success = success & test_array_op<DBool>(
        _SC(Session&, commonSession),
        "test_1",
        T_BOOL,
        std::vector<DBool>{DBool{true}});

    success = success & test_array_op<DBool>(
            _SC(Session&, commonSession),
            "test_2",
            T_BOOL,
            std::vector<DBool>{DBool{false}, DBool{true}});

    success = success & test_array_op<DBool>(
            _SC(Session&, commonSession),
            "test_3",
            T_BOOL,
            std::vector<DBool>{DBool{true}, DBool{false}});

    success = success & test_array_op<DChar>(
            _SC(Session&, commonSession),
            "test_4",
            T_CHAR,
            std::vector<DChar>{DChar{'c'}});

    success = success & test_array_op<DDate>(
            _SC(Session&, commonSession),
            "test_5",
            T_DATE,
            std::vector<DDate>{DDate(1981,12,21), DDate(1982,12,22), DDate(1983,12,23)});

    success = success & test_array_op<DDateTime>(
            _SC(Session&, commonSession),
            "test_6",
            T_DATETIME,
            std::vector<DDateTime>{DDateTime(2081,12,21, 0, 0, 0)});

    success = success & test_array_op<DHiresTime>(
            _SC(Session&, commonSession),
            "test_7",
            T_HIRESTIME,
            std::vector<DHiresTime>{DHiresTime(3081,12,21, 0, 0, 0, 0)});

    success = success & test_array_op<DUInt64>(
            _SC(Session&, commonSession),
            "test_8",
            T_UINT64,
            std::vector<DUInt64>{DUInt64(1)});

    success = success & test_array_op<DUInt16>(
            _SC(Session&, commonSession),
            "test_9",
            T_UINT16,
            std::vector<DUInt16>{DUInt16(2)});

    success = success & test_array_op<DUInt32>(
            _SC(Session&, commonSession),
            "test_10",
            T_UINT32,
            std::vector<DUInt32>{DUInt32(3)});

    success = success & test_array_op<DUInt8>(
            _SC(Session&, commonSession),
            "test_11",
            T_UINT8,
            std::vector<DUInt8>{DUInt8(4)});

    success = success & test_array_op<DInt8>(
            _SC(Session&, commonSession),
            "test_12",
            T_INT8,
            std::vector<DInt8>{DInt8(-1)});

    success = success & test_array_op<DInt16>(
            _SC(Session&, commonSession),
            "test_13",
            T_INT16,
            std::vector<DInt16>{DInt16(-2)});

    success = success & test_array_op<DInt32>(
            _SC(Session&, commonSession),
            "test_14",
            T_INT32,
            std::vector<DInt32>{DInt32(-3)});

    success = success & test_array_op<DInt64>(
            _SC(Session&, commonSession),
            "test_15",
            T_INT64,
            std::vector<DInt64>{DInt64(-4)});

    success = success & test_array_op<DRichReal>(
            _SC(Session&, commonSession),
            "test_16",
            T_RICHREAL,
            std::vector<DRichReal>{DRichReal(-1)});

    success = success & test_array_op<DReal>(
            _SC(Session&, commonSession),
            "test_17",
            T_REAL,
            std::vector<DReal>{DReal(1)});

    success = success & test_array_op<DUInt16>(
            _SC(Session&, commonSession),
            "test_18",
            T_UINT16,
            std::vector<DUInt16>{DUInt16(0)});

    success = success & test_array_op<DUInt16>(
            _SC(Session&, commonSession),
            "test_19",
            T_UINT16,
            std::vector<DUInt16>{DUInt16(0)});

    success = success & test_array_op<DInt8>(
            _SC(Session&, commonSession),
            "test_20",
            T_INT8,
            std::vector<DInt8>{DInt8(1)});

    success = success & test_array_op<DInt16>(
            _SC(Session&, commonSession),
            "test_21",
            T_INT16,
            std::vector<DInt16>{DInt16(0)});

    success = success & test_array_op<DInt32>(
            _SC(Session&, commonSession),
            "test_22",
            T_INT32,
            std::vector<DInt32>{DInt32(0)});

    success = success & test_array_op<DInt64>(
            _SC(Session&, commonSession),
            "test_23",
            T_INT64,
            std::vector<DInt64>{DInt64(0)});

    success = success & test_array_op<DUInt64>(
            _SC(Session&, commonSession),
            "test_24",
            T_UINT64,
            std::vector<DUInt64>{DUInt64(0), DUInt64(1), DUInt64(0)});

    success = success & test_array_op<DUInt16>(
            _SC(Session&, commonSession),
            "test_25",
            T_UINT16,
            std::vector<DUInt16>{DUInt16(0), DUInt16(1), DUInt16(0)});

    success = success & test_array_op<DUInt32>(
            _SC(Session&, commonSession),
            "test_26",
            T_UINT32,
            std::vector<DUInt32>{DUInt32(0), DUInt32(1)});

    success = success & test_array_op<DUInt8>(
            _SC(Session&, commonSession),
            "test_27",
            T_UINT8,
            std::vector<DUInt8>{DUInt8(1), DUInt8(0)});

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
