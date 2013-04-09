#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include "dbs/dbs_mgr.h"
#include "interpreter.h"
#include "custom/include/test/test_fmw.h"

#include "interpreter/prima/pm_interpreter.h"
#include "interpreter/prima/pm_operand.h"
#include "compiler//wopcodes.h"

using namespace prima;

static const char admin[] = "administrator";

DBSArray
get_test_array ()
{
  DBSArray array;

  array.AddElement (DBSReal (1.00));
  array.AddElement (DBSReal (2.00));
  array.AddElement (DBSReal (3.0));
  array.AddElement (DBSReal (5.17));

  return array;
}

template <typename DBS_T> bool
test_self_add (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfAdd (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
}

template <typename DBS_T> bool
test_self_sub (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfSub (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
}

template <typename DBS_T> bool
test_self_mul (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfMul (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
}

template <typename DBS_T> bool
test_self_div (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfDiv (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
}

template <typename DBS_T> bool
test_self_mod (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfMod (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
}

template <typename DBS_T> bool
test_self_and (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfAnd (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
}

template <typename DBS_T> bool
test_self_xor (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfXor (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
}

template <typename DBS_T> bool
test_self_or (I_Operand& op)
{
  bool result = true;

  try
  {
    DBS_T nullValue;
    op.SelfOr (nullValue);
  }
  catch (InterException& e)
  {
      assert (e.Extra () == InterException::INVALID_OP_REQ);
      result = false;
  }
  catch (...)
  {
      assert (false);
  }

  return result;
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
      if (e.Extra () == InterException::INVALID_OP_REQ)
        result = true;
  }
  catch (...)
  {
  }

  return result;
}

template <typename DBS_T> bool
test_operand_for_conv (I_Operand& op)
{
  bool result = true;

  try
  {
      DBS_T value;
      op.GetValue (value);
  }
  catch (...)
  {
      result = false;
  }

  return result;
}

bool
test_op_invalid_conv (I_Operand& op)
{
  bool result = true;

  result = result && test_operand_for_conv<DBSReal> (op);
  result = result && test_operand_for_conv<DBSRichReal> (op);

  result = result && test_operand_for_exceptions<DBSUInt8> (op);
  result = result && test_operand_for_exceptions<DBSUInt16> (op);
  result = result && test_operand_for_exceptions<DBSUInt32> (op);
  result = result && test_operand_for_exceptions<DBSUInt64> (op);
  result = result && test_operand_for_exceptions<DBSInt8> (op);
  result = result && test_operand_for_exceptions<DBSInt16> (op);
  result = result && test_operand_for_exceptions<DBSInt32> (op);
  result = result && test_operand_for_exceptions<DBSInt64> (op);
  result = result && test_operand_for_exceptions<DBSBool> (op);
  result = result && test_operand_for_exceptions<DBSChar> (op);
  result = result && test_operand_for_exceptions<DBSDate> (op);
  result = result && test_operand_for_exceptions<DBSDateTime> (op);
  result = result && test_operand_for_exceptions<DBSHiresTime> (op);
  result = result && test_operand_for_exceptions<DBSText> (op);
  result = result && test_operand_for_exceptions<DBSArray> (op);

  result = result && (! test_self_add<DBSChar> (op));
  result = result && test_self_add<DBSInt64> (op);
  result = result && test_self_add<DBSRichReal> (op);

  result = result && test_self_sub<DBSInt64> (op);
  result = result && test_self_sub<DBSRichReal> (op);

  result = result && test_self_mul<DBSInt64> (op);
  result = result && test_self_mul<DBSRichReal> (op);

  result = result && test_self_div<DBSInt64> (op);
  result = result && test_self_div<DBSRichReal> (op);

  result = result && (! test_self_mod<DBSInt64> (op));

  result = result && (! test_self_and<DBSBool> (op));
  result = result && (! test_self_and<DBSInt64> (op));

  result = result && ( ! test_self_xor<DBSBool> (op));
  result = result && ( ! test_self_xor<DBSInt64> (op));

  result = result && ( ! test_self_or<DBSBool> (op));
  result = result && ( ! test_self_or<DBSInt64> (op));

  try
  {
      op.GetField ();
      result = false;
  }
  catch (InterException& e)
  {
      if (e.Extra() != InterException::INVALID_OP_REQ)
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
  catch (InterException& e)
  {
      if (e.Extra() != InterException::INVALID_OP_REQ)
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
  catch (InterException& e)
  {
      if (e.Extra() != InterException::INVALID_OP_REQ)
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

  op.SetValue (DBS_T ());
  op.GetValue (value);

  if (value.IsNull () == false)
    result = false;

  return result;
}

template <typename DBS_T> bool
test_array_read_value (DBSArray array, const DBS_T testVal)
{
  bool  result = true;
  ArrayOperand arrayOp (array);
  NullOperand nullOp;
  StackValue sv (nullOp);
  I_Operand* pOp = NULL;

  for (uint_t index = 0;
       (index < array.ElementsCount ()) && result;
       ++index)
    {
      sv = arrayOp.GetValueAt (index);
      pOp = &sv.GetOperand ();


      DBS_T first, second;
      pOp->GetValue (first);
      array.GetElement (second, index);

      if ( (first != second) && ( ! test_op_invalid_conv (*pOp) ))
        result = false;
    }

  sv = arrayOp.GetValueAt (array.ElementsCount () - 1);
  pOp = &sv.GetOperand ();
  result &= test_op_invalid_conv (*pOp);
  pOp->SetValue (testVal);

  DBS_T val;

  arrayOp.GetValue (array);
  array.GetElement (val, array.ElementsCount () - 1);

  if ( val != testVal)
    result = false;

  return result;
}

template <typename DBS_T> bool
test_table_value (I_DBSTable& table, DBSArray& array, const DBS_T testVal)
{
  bool result = true;
  DBSArray fieldArray;
  DBS_T    simpleVal;

  table.GetEntry (0,
                  table.GetFieldIndex ("simple_type"),
                  simpleVal);
  table.GetEntry (0,
                  table.GetFieldIndex ("array_type"),
                  fieldArray);

  if (testVal != simpleVal)
    return false;

  if (array.ElementsCount () != fieldArray.ElementsCount())
    return false;

  for (uint64_t el = 0; el < array.ElementsCount (); ++el)
    {
      DBS_T firstVal, secondVal;

      fieldArray.GetElement (firstVal, el);
      array.GetElement (secondVal, el);

      if (firstVal != secondVal)
        result = false;
    }

  return result;
}

template <typename DBS_T> bool
test_array_tableread_value (I_DBSHandler& dbsHnd,
                            DBSArray      array,
                            const DBS_T   testVal)
{
  bool  result = true;
  const NullOperand nullOp;
  StackValue svArray (nullOp);
  StackValue svSimple (nullOp);
  StackValue svArrayEl (nullOp);

  DBSFieldDescriptor fd[2];

  fd[0].isArray      = true;
  fd[0].m_FieldType  = testVal.GetDBSType ();
  fd[0].m_pFieldName = "array_type";

  fd[1].isArray      = false;
  fd[1].m_FieldType  = testVal.GetDBSType ();
  fd[1].m_pFieldName = "simple_type";

  I_DBSTable& testTable = dbsHnd.CreateTempTable (2, fd);
  const ROW_INDEX row = testTable.AddReusedRow ();
  {
    TableOperand tableOp (dbsHnd, testTable);
    FieldOperand fieldArrayOp (tableOp, testTable.GetFieldIndex ("array_type"));
    FieldOperand fieldSimpleOp (tableOp,
                                testTable.GetFieldIndex ("simple_type"));

    svArray = fieldArrayOp.GetValueAt (row);
    svSimple = fieldSimpleOp.GetValueAt (row);
  }

  DBS_T firstArrayVal;
  array.GetElement (firstArrayVal, row);

  assert (testVal != firstArrayVal);

  result &= test_op_invalid_conv (svSimple.GetOperand ());

  svArray.GetOperand ().SetValue (array);
  svSimple.GetOperand ().SetValue (firstArrayVal);

  result &= test_table_value (testTable, array, firstArrayVal);

  array.SetElement (testVal, 0);
  svArrayEl = svArray.GetOperand ().GetValueAt (0);

  result &= test_op_invalid_conv (svArrayEl.GetOperand ());

  svArrayEl.GetOperand ().SetValue (testVal);
  svSimple.GetOperand ().SetValue (testVal);

  result &= test_table_value (testTable, array, testVal);

  result &= test_null_write (svSimple.GetOperand (), DBS_T ());

  result &= test_table_value (testTable, array, DBS_T ());

  return result;
}


int
main ()
{
  bool success = true;

  {
    DBSInit (DBSSettings ());
  }

  DBSCreateDatabase (admin);

  {
    I_DBSHandler& dbsHnd = DBSRetrieveDatabase (admin);
    RealOperand op (DBSReal (-10.123));
    success = success && test_op_invalid_conv (op);
    success = success && test_null_write (op, DBSReal (0.0));
    success = success && test_array_read_value (get_test_array (),
                                                DBSReal (0));
    success = success && test_array_tableread_value (dbsHnd,
                                                     get_test_array (),
                                                     DBSReal (0.0));

    DBSReleaseDatabase (dbsHnd);
  }

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
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
