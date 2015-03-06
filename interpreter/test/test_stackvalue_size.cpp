#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "dbs/dbs_mgr.h"
#include "interpreter.h"
#include "custom/include/test/test_fmw.h"

#include "interpreter/prima/pm_interpreter.h"
#include "interpreter/prima/pm_operand.h"
#include "interpreter/prima/pm_operand_undefined.h"
#include "compiler/wopcodes.h"

using namespace whais;
using namespace prima;

static size_t sgMaxValue;

template<typename T>bool test_stack_type (const char* optype)
{
  std::cout.width (30);
  std::cout << std::left<< optype << std::right;
  std::cout.width (0);
  std::cout << " " << sizeof (T) << " ... " ;

  if (sgMaxValue < sizeof (T))
    sgMaxValue = sizeof (T);

  const bool result = (sizeof (T) <= sizeof (StackValue));
  std::cout << (result ? "OK" : "FAIL") << std::endl;

  return result;
}

int
main()
{
  bool success = true;

  sgMaxValue = 0;

  success &= test_stack_type<DText> ("DText");
  success &= test_stack_type<DArray> ("DArray");
  success &= test_stack_type<BoolOperand> ("BoolOperand");
  success &= test_stack_type<BoolArrayElOperand> ("BoolArrayElOperand");
  success &= test_stack_type<BoolFieldElOperand> ("BoolFieldElOperand");
  success &= test_stack_type<BoolArrayFieldElOperand> ("BoolArrayFieldElOperand");

  success &= test_stack_type<CharOperand> ("CharOperand");
  success &= test_stack_type<CharTextElOperand> ("CharTextElOperand");
  success &= test_stack_type<CharArrayElOperand> ("CharArrayElOperand");
  success &= test_stack_type<CharFieldElOperand> ("CharFieldElOperand");
  success &= test_stack_type<CharTextFieldElOperand> ("CharTextFieldElOperand");
  success &= test_stack_type<CharArrayFieldElOperand> ("CharArrayFieldElOperand");

  success &= test_stack_type<DateOperand> ("DateOperand");
  success &= test_stack_type<DateArrayElOperand> ("DateArrayElOperand");
  success &= test_stack_type<DateFieldElOperand> ("DateFieldElOperand");
  success &= test_stack_type<DateArrayFieldElOperand> ("DateArrayFieldElOperand");

  success &= test_stack_type<DateTimeOperand> ("DateTimeOperand");
  success &= test_stack_type<DateTimeArrayElOperand> ("DateTimeArrayElOperand");
  success &= test_stack_type<DateTimeFieldElOperand> ("DateTimeFieldElOperand");
  success &= test_stack_type<DateTimeArrayFieldElOperand> ("DateTimeArrayFieldElOperand");

  success &= test_stack_type<HiresTimeOperand> ("HiresTimeOperand");
  success &= test_stack_type<HiresTimeArrayElOperand> ("HiresTimeArrayElOperand");
  success &= test_stack_type<HiresTimeFieldElOperand> ("HiresTimeFieldElOperand");
  success &= test_stack_type<HiresTimeArrayFieldElOperand> ("HiresTimeArrayFieldElOperand");

  success &= test_stack_type<Int8Operand> ("Int8Operand");
  success &= test_stack_type<Int8ArrayElOperand> ("Int8ArrayElOperand");
  success &= test_stack_type<Int8FieldElOperand> ("Int8FieldElOperand");
  success &= test_stack_type<Int8ArrayFieldElOperand> ("Int8ArrayFieldElOperand");

  success &= test_stack_type<Int16Operand> ("Int16Operand");
  success &= test_stack_type<Int16ArrayElOperand> ("Int16ArrayElOperand");
  success &= test_stack_type<Int16FieldElOperand> ("Int16FieldElOperand");
  success &= test_stack_type<Int16ArrayFieldElOperand> ("Int16ArrayFieldElOperand");

  success &= test_stack_type<Int32Operand> ("Int32Operand");
  success &= test_stack_type<Int32ArrayElOperand> ("Int32ArrayElOperand");
  success &= test_stack_type<Int32FieldElOperand> ("Int32FieldElOperand");
  success &= test_stack_type<Int32ArrayFieldElOperand> ("Int32ArrayFieldElOperand");

  success &= test_stack_type<Int64Operand> ("Int64Operand");
  success &= test_stack_type<Int64ArrayElOperand> ("Int64ArrayElOperand");
  success &= test_stack_type<Int64FieldElOperand> ("Int64FieldElOperand");
  success &= test_stack_type<Int64ArrayFieldElOperand> ("Int64ArrayFieldElOperand");

  success &= test_stack_type<Int64Operand> ("Int64Operand");
  success &= test_stack_type<Int64ArrayElOperand> ("Int64ArrayElOperand");
  success &= test_stack_type<Int64FieldElOperand> ("Int64FieldElOperand");
  success &= test_stack_type<Int64ArrayFieldElOperand> ("Int64ArrayFieldElOperand");

  success &= test_stack_type<UInt8Operand> ("UInt8Operand");
  success &= test_stack_type<UInt8ArrayElOperand> ("UInt8ArrayElOperand");
  success &= test_stack_type<UInt8FieldElOperand> ("UInt8FieldElOperand");
  success &= test_stack_type<UInt8ArrayFieldElOperand> ("UInt8ArrayFieldElOperand");

  success &= test_stack_type<UInt16Operand> ("UInt16Operand");
  success &= test_stack_type<UInt16ArrayElOperand> ("UInt16ArrayElOperand");
  success &= test_stack_type<UInt16FieldElOperand> ("UInt16FieldElOperand");
  success &= test_stack_type<UInt16ArrayFieldElOperand> ("UInt16ArrayFieldElOperand");

  success &= test_stack_type<UInt32Operand> ("UInt32Operand");
  success &= test_stack_type<UInt32ArrayElOperand> ("UInt32ArrayElOperand");
  success &= test_stack_type<UInt32FieldElOperand> ("UInt32FieldElOperand");
  success &= test_stack_type<UInt32ArrayFieldElOperand> ("UInt32ArrayFieldElOperand");

  success &= test_stack_type<UInt64Operand> ("UInt64Operand");
  success &= test_stack_type<UInt64ArrayElOperand> ("UInt64ArrayElOperand");
  success &= test_stack_type<UInt64FieldElOperand> ("UInt64FieldElOperand");
  success &= test_stack_type<UInt64ArrayFieldElOperand> ("UInt64ArrayFieldElOperand");

  success &= test_stack_type<UInt64Operand> ("UInt64Operand");
  success &= test_stack_type<UInt64ArrayElOperand> ("UInt64ArrayElOperand");
  success &= test_stack_type<UInt64FieldElOperand> ("UInt64FieldElOperand");
  success &= test_stack_type<UInt64ArrayFieldElOperand> ("UInt64ArrayFieldElOperand");

  success &= test_stack_type<RealOperand> ("RealOperand");
  success &= test_stack_type<RealArrayElOperand> ("RealArrayElOperand");
  success &= test_stack_type<RealFieldElOperand> ("RealFieldElOperand");
  success &= test_stack_type<RealArrayFieldElOperand> ("RealArrayFieldElOperand");

  success &= test_stack_type<RichRealOperand> ("RichRealOperand");
  success &= test_stack_type<RichRealArrayElOperand> ("RichRealArrayElOperand");
  success &= test_stack_type<RichRealFieldElOperand> ("RichRealFieldElOperand");
  success &= test_stack_type<RichRealArrayFieldElOperand> ("RichRealArrayFieldElOperand");

  success &= test_stack_type<TextOperand> ("TextOperand");
  success &= test_stack_type<TextFieldElOperand> ("TextArrayFieldElOperand");

  success &= test_stack_type<FieldOperand> ("FieldOperand");
  success &= test_stack_type<TableOperand> ("TableOperand");

  success &= test_stack_type<GlobalOperand> ("GlobalOperand");
  success &= test_stack_type<LocalOperand> ("LocalOperand");

  success &= test_stack_type<NativeObjectOperand> ("NativeObjectOperand");

  success &= (sgMaxValue <= sizeof (StackValue));

  std::cout << "Largest operand is: " << sgMaxValue << std::endl;
  std::cout << "Stack value size set at " <<  sizeof (StackValue) << " ... ";
  std::cout << (success ? "OK\n" : "FAIL\n");

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
