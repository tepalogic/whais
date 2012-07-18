#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "dbs_mgr.h"
#include "interpreter.h"
#include "test/test_fmw.h"

#include "interpreter/prima/pm_interpreter.h"
#include "interpreter/prima/pm_operand.h"
#include "compiler/include/whisperc/wopcodes.h"

using namespace prima;

static const D_CHAR admin[] = "administrator";

DBSArray
get_test_array ()
{
  DBSArray array;

  array.AddElement (DBSBool (true));
  array.AddElement (DBSBool (false));

  return array;
}

template <typename DBS_T> bool
test_operand_for_exceptions (I_Operand& op)
{
  bool result = false;

  try
  {
      DBS_T value;
      op.GetValue (value);
  }
  catch (InterException& e)
  {
      if (e.GetExtra () == InterException::INVALID_OP_REQ)
        result = true;
  }
  catch (...)
  {
  }

  return result;
}

bool
test_op_invalid_conv (I_Operand& op)
{
  bool result = true;

  result = result && test_operand_for_exceptions<DBSChar> (op);
  result = result && test_operand_for_exceptions<DBSDate> (op);
  result = result && test_operand_for_exceptions<DBSDateTime> (op);
  result = result && test_operand_for_exceptions<DBSHiresTime> (op);
  result = result && test_operand_for_exceptions<DBSUInt8> (op);
  result = result && test_operand_for_exceptions<DBSUInt16> (op);
  result = result && test_operand_for_exceptions<DBSUInt32> (op);
  result = result && test_operand_for_exceptions<DBSUInt64> (op);
  result = result && test_operand_for_exceptions<DBSInt8> (op);
  result = result && test_operand_for_exceptions<DBSInt16> (op);
  result = result && test_operand_for_exceptions<DBSInt32> (op);
  result = result && test_operand_for_exceptions<DBSInt64> (op);
  result = result && test_operand_for_exceptions<DBSReal> (op);
  result = result && test_operand_for_exceptions<DBSRichReal> (op);
  result = result && test_operand_for_exceptions<DBSText> (op);
  result = result && test_operand_for_exceptions<DBSArray> (op);

  try
  {
      op.GetField ();
      result = false;
  }
  catch (InterException e)
  {
      if (e.GetExtra() != InterException::INVALID_OP_REQ)
        result = false;
  }
  catch (...)
  {
      result = false;
  }

  try
  {
      op.GetTable ();
      result = false;
  }
  catch (InterException e)
  {
      if (e.GetExtra() != InterException::INVALID_OP_REQ)
        result = false;
  }
  catch (...)
  {
      result = false;
  }

  try
  {
      op.GetValueAt (0);
      result = false;
  }
  catch (InterException e)
  {
      if (e.GetExtra() != InterException::INVALID_OP_REQ)
        result = false;
  }
  catch (...)
  {
      result = false;
  }

  return result;
}

template <typename DBS_T> bool
test_null_write (I_Operand& op, DBS_T value)
{
  bool result = true;

  op.SetValue (DBSBool ());
  op.GetValue (value);

  if (value.IsNull () == false)
    result = false;

  return result;
}

template <typename DBS_T> bool
test_array_read_value (DBSArray array)
{
  bool  result = true;
  ArrayOperand arrayOp (array);
  NullOperand nullOp;
  StackValue sv (nullOp);
  I_Operand* pOp = NULL;

  for (D_UINT index = 0;
       (index < array.ElementsCount ()) && result;
       ++index)
    {
      sv = arrayOp.GetValueAt (index);
      pOp = &sv.GetOperand ();


      DBS_T first, second;
      pOp->GetValue (first);
      array.GetElement (second, index);

      if ( ( ! (first == second)) && ( ! test_op_invalid_conv (*pOp) ))
        result = false;

      sv.Clear ();
    }

  return result;
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

  {
    BoolOperand op (DBSBool (true));
    success = success && test_op_invalid_conv (op);
    success = success && test_null_write<DBSBool> (op, DBSBool (true));
    success = success && test_array_read_value<DBSBool> (get_test_array ());
  }

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

