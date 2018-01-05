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
    "PROCEDURE test_0() RETURN ARRAY\n"
    "DO\n"
    "   a = {TRUE};\n"
    "   RETURN a + {TRUE};\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_1() RETURN ARRAY\n"
    "DO\n"
    "   a = {FALSE};\n"
    "   RETURN a - TRUE;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_2() RETURN ARRAY\n"
    "DO\n"
    "   a = {FALSE};\n"
    "   RETURN a % TRUE;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_3() RETURN ARRAY\n"
    "DO\n"
    "   a = {FALSE};\n"
    "   a += {TRUE};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_4() RETURN ARRAY\n"
    "DO\n"
    "   a = {FALSE};\n"
    "   a -= {FALSE};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_5() RETURN ARRAY\n"
    "DO\n"
    "   a = {FALSE};\n"
    "   a %= {FALSE};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_6() RETURN ARRAY\n"
    "DO\n"
    "   a = {'b'};\n"
    "   RETURN a + {'a'};\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_7() RETURN ARRAY\n"
    "DO\n"
    "   a = {'b'};\n"
    "   RETURN a - 'a';\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_8() RETURN ARRAY\n"
    "DO\n"
    "   a = {'b', 'c'};\n"
    "   RETURN a % {'c'};\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_9() RETURN ARRAY\n"
    "DO\n"
    "   a = {'2', '3'};\n"
    "   a += {'2'};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_10() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1'};\n"
    "   a -= '3';\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_11() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1', '2', '3'};\n"
    "   a %= {'3', '1'};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_12() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1980/12/31'};\n"
    "   RETURN a + '1984/12/31 3:0';\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_13() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1980/12/31', '1984/12/31'};\n"
    "   RETURN a - {'1984/12/31 3:0', '2011/12/31'};\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_14() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1980/12/31', '1984/12/31'};\n"
    "   RETURN a % '1980/12/20 5:31:01.87';\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_15() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1984/12/31 3:0:09'};\n"
    "   a += '1984/12/31';\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_16() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1984/01/2 12:0:09', '2022/1/20' } DATETIME;\n"
    "   a -= {'1984/12/31'};\n"
    "   \n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_17() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1984/01/2 12:0:09', '2022/1/20' } DATETIME;\n"
    "   a %= {'1984/12/31'};\n"
    "   \n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_18() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1984/12/31 3:0:09'} HIRESTIME;\n"
    "   a += '1984/12/31';\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_19() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1984/01/2 12:0:09', '2022/1/20' } HIRESTIME;\n"
    "   a -= {'1984/12/31'};\n"
    "   \n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_20() RETURN ARRAY\n"
    "DO\n"
    "   a = {'1984/01/2 12:0:09', '2022/1/20' } HIRESTIME;\n"
    "   a %= {'1984/12/31'};\n"
    "   \n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_21() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 0} REAL;\n"
    "   a += 3;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_22() RETURN ARRAY\n"
    "DO\n"
    "   a = {10, 11, 0} RICHREAL;\n"
    "   RETURN a - {0.1, 10, 11} REAL;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_23() RETURN ARRAY\n"
    "DO\n"
    "   a = {10, 11, 0.1, 3, 4, 21} RICHREAL;\n"
    "   a %= {2, 11, 4, 90, 10, 0.233} REAL;\n"
    "   \n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_24() RETURN ARRAY\n"
    "DO\n"
    "   VAR v INT64;\n"
    "   \n"
    "   a = {1, 2, 3, 4};\n"
    "   RETURN a + v;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_25() RETURN ARRAY\n"
    "DO\n"
    "   VAR v INT32;  \n"
    "   a = {1, 2, 3, 4} UINT16;\n"
    "   RETURN a - v;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_26() RETURN ARRAY\n"
    "DO\n"
    "   VAR v INT8;  \n"
    "   a = {1, 2, 3, 4} UINT32;\n"
    "   RETURN a % v;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_27() RETURN ARRAY\n"
    "DO\n"
    "   VAR v UINT8 ARRAY;\n"
    "   a = {1, 2, 3, 4} INT64;\n"
    "   a += v;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_28() RETURN ARRAY\n"
    "DO\n"
    "   VAR v UINT16 ARRAY;\n"
    "   a = {1, 2, 3, 4} INT16;\n"
    "   a -= v;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_29() RETURN ARRAY\n"
    "DO\n"
    "   VAR v UINT64 ARRAY;\n"
    "   a = {1, 2, 3, 4} INT32;\n"
    "   a %= v;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_30() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4} INT64;\n"
    "   a += 7;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_31() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4, 7, 3, 5} INT16;\n"
    "   a -= 3;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_32() RETURN ARRAY\n"
    "DO\n"
    "\n"
    "   a = {1, 2, 3, 4} INT16;\n"
    "   a -= 10;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_33() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4} INT32;\n"
    "   a %= 11;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_34() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4, 2, 1} INT32;\n"
    "   a %= 2;\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "\n"
    "PROCEDURE test_35() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4} INT64;\n"
    "   a += {7, 2};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_36() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4, 7, 3, 5} INT16;\n"
    "   a -= {3, 2};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_37() RETURN ARRAY\n"
    "DO\n"
    "\n"
    "   a = {1, 2, 3, 4} INT16;\n"
    "   a -= {10, 12};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_38() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4} INT32;\n"
    "   a %= {11, 0};\n"
    "   RETURN a;\n"
    "ENDPROC\n"
    "\n"
    "PROCEDURE test_39() RETURN ARRAY\n"
    "DO\n"
    "   a = {1, 2, 3, 4, 2, 1} INT32;\n"
    "   RETURN a % {2, 23, 31, 4, 31, 3, 0};\n"
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

    std::cout << "Testing array operations ... \n";

    success = success & test_array_op<DBool>(
        _SC(Session&, commonSession),
        "test_0",
        T_BOOL,
        std::vector<DBool>{DBool{true}, DBool{true}});

    success = success & test_array_op<DBool>(
        _SC(Session&, commonSession),
        "test_1",
        T_BOOL,
        std::vector<DBool>{DBool{false}});

    success = success & test_array_op<DBool>(
        _SC(Session&, commonSession),
        "test_2",
        T_BOOL,
        std::vector<DBool>());

    success = success & test_array_op<DBool>(
        _SC(Session&, commonSession),
        "test_3",
        T_BOOL,
        std::vector<DBool>{DBool{false}, DBool{true}});

    success = success & test_array_op<DBool>(
        _SC(Session&, commonSession),
        "test_4",
        T_BOOL,
        std::vector<DBool>());

    success = success & test_array_op<DBool>(
        _SC(Session&, commonSession),
        "test_5",
        T_BOOL,
        std::vector<DBool>{DBool{false}});

    success = success & test_array_op<DChar>(
        _SC(Session&, commonSession),
        "test_6",
        T_CHAR,
        std::vector<DChar>{DChar{'b'}, DChar{'a'}});

    success = success & test_array_op<DChar>(
        _SC(Session&, commonSession),
        "test_7",
        T_CHAR,
        std::vector<DChar>{DChar{'b'}});

    success = success & test_array_op<DChar>(
        _SC(Session&, commonSession),
        "test_8",
        T_CHAR,
        std::vector<DChar>{DChar{'c'}});

    success = success & test_array_op<DChar>(
        _SC(Session&, commonSession),
        "test_9",
        T_CHAR,
        std::vector<DChar>{DChar{'2'}, DChar{'3'}, DChar{'2'}});

    success = success & test_array_op<DChar>(
        _SC(Session&, commonSession),
        "test_10",
        T_CHAR,
        std::vector<DChar>{DChar{'1'}});

    success = success & test_array_op<DChar>(
        _SC(Session&, commonSession),
        "test_11",
        T_CHAR,
        std::vector<DChar>{DChar{'1'}, DChar{'3'}});

    success = success & test_array_op<DDate>(
        _SC(Session&, commonSession),
        "test_12",
        T_DATE,
        std::vector<DDate>{DDate(1980,12,31), DDate(1984,12,31)});

    success = success & test_array_op<DDate>(
        _SC(Session&, commonSession),
        "test_13",
        T_DATE,
        std::vector<DDate>{DDate(1980,12,31)});

    success = success & test_array_op<DDate>(
        _SC(Session&, commonSession),
        "test_14",
        T_DATE,
        std::vector<DDate>{});

    success = success & test_array_op<DDateTime>(
        _SC(Session&, commonSession),
        "test_15",
        T_DATETIME,
        std::vector<DDateTime>{
          DDateTime(1984, 12, 31, 3, 0, 9),
          DDateTime(1984, 12, 31, 0, 0, 0)});

    success = success & test_array_op<DDateTime>(
        _SC(Session&, commonSession),
        "test_16",
        T_DATETIME,
        std::vector<DDateTime>{
          DDateTime(1984, 1, 2, 12, 0, 9),
          DDateTime(2022, 1, 20, 0, 0, 0)});

    success = success & test_array_op<DDateTime>(
        _SC(Session&, commonSession),
        "test_17",
        T_DATETIME,
        std::vector<DDateTime>{});


    success = success & test_array_op<DHiresTime>(
        _SC(Session&, commonSession),
        "test_18",
        T_HIRESTIME,
        std::vector<DHiresTime>{
          DHiresTime(1984, 12, 31, 3, 0, 9, 0),
          DHiresTime(1984, 12, 31, 0, 0, 0, 0)});

    success = success & test_array_op<DHiresTime>(
        _SC(Session&, commonSession),
        "test_19",
        T_HIRESTIME,
        std::vector<DHiresTime>{
          DHiresTime(1984, 1, 2, 12, 0, 9, 0),
          DHiresTime(2022, 1, 20, 0, 0, 0, 0)});

    success = success & test_array_op<DHiresTime>(
        _SC(Session&, commonSession),
        "test_20",
        T_HIRESTIME,
        std::vector<DHiresTime>{});

    success = success & test_array_op<DReal>(
        _SC(Session&, commonSession),
        "test_21",
        T_REAL,
        std::vector<DReal>{DReal{1}, DReal{2}, DReal{0}, DReal{3}});

    success = success & test_array_op<DRichReal>(
      _SC(Session&, commonSession),
      "test_22",
      T_RICHREAL,
      std::vector<DRichReal>{DRichReal{0}});

    success = success & test_array_op<DRichReal>(
      _SC(Session&, commonSession),
      "test_23",
      T_RICHREAL,
      std::vector<DRichReal>{DRichReal{10}, DRichReal{11}, DRichReal{4}});

    success = success & test_array_op<DUInt64>(
      _SC(Session&, commonSession),
      "test_24",
      T_UINT64,
      std::vector<DUInt64>{DUInt64(1), DUInt64(2), DUInt64(3), DUInt64(4)});

    success = success & test_array_op<DUInt16>(
      _SC(Session&, commonSession),
      "test_25",
      T_UINT16,
      std::vector<DUInt16>{DUInt16(1), DUInt16(2), DUInt16(3), DUInt16(4)});

    success = success & test_array_op<DUInt32>(
      _SC(Session&, commonSession),
      "test_26",
      T_UINT32,
      std::vector<DUInt32>{});

    success = success & test_array_op<DInt64>(
      _SC(Session&, commonSession),
      "test_27",
      T_INT64,
      std::vector<DInt64>{DInt64(1), DInt64(2), DInt64(3), DInt64(4)});

    success = success & test_array_op<DInt16>(
      _SC(Session&, commonSession),
      "test_28",
      T_INT16,
      std::vector<DInt16>{DInt16(1), DInt16(2), DInt16(3), DInt16(4)});

    success = success & test_array_op<DInt32>(
      _SC(Session&, commonSession),
      "test_29",
      T_INT32,
      std::vector<DInt32>{});

    success = success & test_array_op<DInt64>(
      _SC(Session&, commonSession),
      "test_30",
      T_INT64,
      std::vector<DInt64>{DInt64(1), DInt64(2), DInt64(3), DInt64(4), DInt64(7)});

    success = success & test_array_op<DInt16>(
      _SC(Session&, commonSession),
      "test_31",
      T_INT16,
      std::vector<DInt16>{DInt16(01), DInt16(2), DInt16(4), DInt16(7), DInt16(5)});

    success = success & test_array_op<DInt16>(
      _SC(Session&, commonSession),
      "test_32",
      T_INT16,
      std::vector<DInt16>{DInt16(1), DInt16(2), DInt16(3), DInt16(4)});

    success = success & test_array_op<DInt32>(
      _SC(Session&, commonSession),
      "test_33",
      T_INT32,
      std::vector<DInt32>{});

    success = success & test_array_op<DInt32>(
      _SC(Session&, commonSession),
      "test_34",
      T_INT32,
      std::vector<DInt32>{DInt32(2), DInt32(2)});

    success = success & test_array_op<DInt64>(
      _SC(Session&, commonSession),
      "test_35",
      T_INT64,
      std::vector<DInt64>{DInt64(1), DInt64(2), DInt64(3), DInt64(4), DInt64(7), DInt64(2)});

    success = success & test_array_op<DInt16>(
      _SC(Session&, commonSession),
      "test_36",
      T_INT16,
      std::vector<DInt16>{DInt16(1), DInt16(4), DInt16(7), DInt16(5)});

    success = success & test_array_op<DInt16>(
      _SC(Session&, commonSession),
      "test_37",
      T_INT16,
      std::vector<DInt16>{DInt16(1), DInt16(2), DInt16(3), DInt16(4)});

    success = success & test_array_op<DInt32>(
      _SC(Session&, commonSession),
      "test_38",
      T_INT32,
      std::vector<DInt32>{});

    success = success & test_array_op<DInt32>(
      _SC(Session&, commonSession),
      "test_39",
      T_INT32,
      std::vector<DInt32>{DInt32(2), DInt32(3), DInt32(4), DInt32(2)});

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
