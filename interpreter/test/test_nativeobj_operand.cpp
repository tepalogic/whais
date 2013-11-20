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

using namespace whisper;
using namespace prima;


static const char admin[] = "administrator";


class TestNativeObj : public INativeObject
{
public:
  TestNativeObj ()
    : mRegistered (0),
      mChunkMemory (new int[1024])
  {
  }

  virtual ~TestNativeObj ()
  {
    if ((mRegistered != 0)
        || (mChunkMemory != NULL))
      {
        throw "Objected is deleted though it was not properly 'released'.";
      }
  }

  virtual void RegisterUser ()
  {
    ++mRegistered;
    if (mChunkMemory == NULL)
      throw "Where is the memory!";
  }

  virtual void ReleaseUser ()
  {
    if (mRegistered <= 0)
      throw "mRegistred has not a proper value.";

    if (--mRegistered == 0)
      {
        delete [] mChunkMemory;

        mChunkMemory = NULL;
      }
  }

  virtual void* Memory () { return _SC (void*, mChunkMemory); }
  virtual int   RegisterCount () { return mRegistered; }

private:
  int     mRegistered;
  int*    mChunkMemory;

};


template <typename T> bool
test_basic_value_stored (IOperand&        op,
                         const T&         refValue,
                         const uint_t     expectedType)
{
  T value;

  op.SetValue (refValue);
  op.GetValue (value);

  if ((value != refValue)
      || (op.GetType () != expectedType))
    {
      return false;
    }

  op.SetValue (T ());
  op.GetValue (value);

  if ((value.IsNull () == false)
      || (op.GetType () != expectedType))
    {
      return false;
    }

  op.SetValue (refValue);
  value = T ();

  StackValue sv     = op.Duplicate ();
  IOperand& stackOp = sv.Operand ();

  stackOp.GetValue (value);
  if ((value != refValue)
      || (op.GetType () != expectedType))
    {
      return false;
    }

  return true;
}


bool
test_basic_value ()
{
  std::cout << "Test basic values ...\n";

  NativeObjectOperand op;

  bool success = true;

  success = success && test_basic_value_stored (op, DBool (true), T_BOOL);

  success = success && test_basic_value_stored (op, DChar ('z'), T_CHAR);

  success = success && test_basic_value_stored (op, DDate (1890, 1, 2), T_HIRESTIME);
  success = success && test_basic_value_stored (op, DDateTime (1876, 1, 23, 1, 3, 4), T_HIRESTIME);
  success = success && test_basic_value_stored (op, DHiresTime (2090, 7, 8, 1, 3, 4, 8), T_HIRESTIME);
  success = success && test_basic_value_stored (op, DHiresTime (2090, 7, 8, 1, 3, 4, 8), T_HIRESTIME);

  success = success && test_basic_value_stored (op, DInt8 (-1), T_INT64);
  success = success && test_basic_value_stored (op, DInt16 (0), T_INT64);
  success = success && test_basic_value_stored (op, DInt32 (1), T_INT64);
  success = success && test_basic_value_stored (op, DInt64::Min (), T_INT64);

  success = success && test_basic_value_stored (op, DUInt8 (0), T_UINT64);
  success = success && test_basic_value_stored (op, DUInt16 (1), T_UINT64);
  success = success && test_basic_value_stored (op, DUInt32 (2), T_UINT64);
  success = success && test_basic_value_stored (op, DUInt64::Max (), T_UINT64);

  success = success && test_basic_value_stored (op, DReal::Min (), T_RICHREAL);
  success = success && test_basic_value_stored (op, DReal::Max (), T_RICHREAL);

  success = success && test_basic_value_stored (op, DRichReal::Min (), T_RICHREAL);
  success = success && test_basic_value_stored (op, DRichReal::Max (), T_RICHREAL);

  return success;
}


template <typename T> bool
test_array_value_stored (const T&       val1,
                         const T&       val2,
                         const T&       val3,
                         uint_t         arrayType)
{

  NativeObjectOperand op;

  assert (val1 != val2);
  assert (val2 != val3);
  assert (val1 != val3);

  DArray  temp, value, value2;
  T       v1, v2;
  uint_t  type;

  type = T_UNDETERMINED;
  MARK_ARRAY  (type);

  op.SetValue (temp);
  if ((op.GetType () != type)
      || ! op.IsNull ())
    {
      return false;
    }

  type = arrayType;
  MARK_ARRAY (type);

  temp.Add (val1);
  temp.Add (val2);

  op.SetValue (temp);
  if ((op.GetType () != type) || op.IsNull ())
    return false;

  op.GetValue (value);

  if (temp.Count () != value.Count ())
    return false;

  temp.Get (0, v1);
  value.Get (0, v2);
  if ((v1 != v2) || (v1 != val1))
    return false;

  temp.Get (1, v1);
  value.Get (1, v2);
  if ((v1 != v2) || (v1 != val2))
    return false;

  StackValue sv = op.Duplicate ();

  temp.Set (0, val3);

  temp.Get (0, v1);
  value.Get (0, v2);
  if ( (v1 == v2) || (v1 != val3) || (v2 != val1))
    return false;

  op.SetValue (temp);
  op.GetValue (value2);

  value2.Get (0, v1);
  value.Get (0, v2);
  if ( (v1 == v2) || (v1 != val3) || (v2 != val1))
    return false;

  value2.Get (1, v1);
  value.Get (1, v2);
  if ((v1 != v2) || (v1 != val2))
    return false;

  temp = DArray (_SC (T*, NULL));
  op.SetValue (temp);

  if ((op.GetType () != type) || ! op.IsNull ())
    return false;

  sv.Operand ().GetValue (temp);

  temp.Get (0, v1);
  value.Get (0, v2);
  if ((v1 != v2) || (v1 != val1))
    return false;

  temp.Get (1, v1);
  value.Get (1, v2);
  if ((v1 != v2) || (v1 != val2))
    return false;

  return true;
}


bool
test_array_value ()
{
  std::cout << "Test native array values ...\n";

  bool success = true;

  //success = success && test_array_value_stored (DBool (true), DBool (true), DBool (false), T_BOOL);

  success = success && test_array_value_stored (DChar ('a'), DChar ('b'), DChar ('c'), T_CHAR);
  success = success && test_array_value_stored (DDate (1889, 11, 10), DDate (1900, 10, 9), DDate (1901, 9, 8), T_DATE);
  success = success && test_array_value_stored (DDateTime (1889, 11, 10, 0, 0, 0), DDateTime (1900, 10, 9, 1, 1, 1), DDateTime (1901, 9, 8, 2, 2, 2), T_DATETIME);
  success = success && test_array_value_stored (DHiresTime (1889, 11, 10, 0, 0, 0, 0), DHiresTime (1900, 10, 9, 1, 1, 1, 10), DHiresTime (1901, 9, 8, 2, 2, 2, 20), T_HIRESTIME);

  success = success && test_array_value_stored (DInt8 (-1), DInt8 (0), DInt8 (1), T_INT8);
  success = success && test_array_value_stored (DInt16 (-1), DInt16 (0), DInt16 (1), T_INT16);
  success = success && test_array_value_stored (DInt32 (-1), DInt32 (0), DInt32 (1), T_INT32);
  success = success && test_array_value_stored (DInt64 (-1), DInt64 (0), DInt64 (1), T_INT64);

  success = success && test_array_value_stored (DUInt8 (0), DUInt8 (1), DUInt8 (2), T_UINT8);
  success = success && test_array_value_stored (DUInt16 (0), DUInt16 (1), DUInt16 (2), T_UINT16);
  success = success && test_array_value_stored (DUInt32 (0), DUInt32 (1), DUInt32 (2), T_UINT32);
  success = success && test_array_value_stored (DUInt64 (0), DUInt64 (1), DUInt64 (2), T_UINT64);

  success = success && test_array_value_stored (DReal (DBS_REAL_T (0, 0, 10)), DReal (DBS_REAL_T (0, -100, 10000)), DReal (DBS_REAL_T (1, 100, 10000)), T_REAL);
  success = success && test_array_value_stored (DRichReal (DBS_RICHREAL_T (0, 0, 10)), DRichReal (DBS_RICHREAL_T (0, -100, 10000)), DRichReal (DBS_RICHREAL_T (1, 100, 10000)), T_RICHREAL);

  return success;
}


template <typename T1, typename T2> bool
test_table_value (IDBSHandler&            dbsHnd,
                  const T1                val1,
                  const DBS_FIELD_TYPE    val1Type,
                  const T2                val2,
                  const DBS_FIELD_TYPE    val2Type)
{

  ITable*               refTable = NULL;
  NativeObjectOperand   nativeOp;

    {
      DBSFieldDescriptor fd[2];

      fd[0].isArray   = false;
      fd[0].type      = val1Type;
      fd[0].name      = "field1";

      fd[1].isArray   = false;
      fd[1].type      = val2Type;
      fd[1].name      = "field2";

      refTable = &dbsHnd.CreateTempTable (2, fd);

      if (refTable == NULL)
        return false;

      //This op should take the table ownership!
      TableOperand tableOp (dbsHnd, *refTable);

      //The table owner ship should be shared with this.
      nativeOp.CopyNativeObjectOperand (
                        NativeObjectOperand (tableOp.GetTableReference ())
                                       );
    }

  if ((refTable != &nativeOp.GetTable ())
      || ! nativeOp.IsNull ()
      || (IS_TABLE (nativeOp.GetType ()) == false))
    {
      return false;
    }

  refTable->Set (0, 0, val1);

    {
      StackValue secondVal = nativeOp.Duplicate ();

      IOperand& op = secondVal.Operand ();

      if ((&op.GetTable () != refTable)
          || (&op.GetTable () != &nativeOp.GetTable ())
          || (op.GetType () != nativeOp.GetType ())
          || op.IsNull ())
        {
          return false;
        }

      T1 v1, v2;

      refTable->Get (0, 0, v1);
      op.GetTable ().Get (0, 0, v2);

      if ((v1 != v2) || (v1 != val1))
        return NULL;

      op.GetTable ().Set (1, 1, val2);
    }

  if ((refTable != &nativeOp.GetTable ())
      || nativeOp.IsNull ()
      || (IS_TABLE (nativeOp.GetType ()) == false))
    {
      return false;
    }

  {
      T1 v1, v2;

      refTable->Get (0, 0, v1);
      nativeOp.GetTable ().Get (0, 0, v2);

      if ((v1 != v2) || (v1 != val1))
        return false;

      refTable->Get (1, 0, v1);
      nativeOp.GetTable ().Get (1, 0, v2);

      if ((v1 != v2) || (v1 != T1 ()))
        return false;
  }

  {
      T2 v1, v2;

      refTable->Get (0, 1, v1);
      nativeOp.GetTable ().Get (0, 1, v2);

      if ((v1 != v2) || (v1 != T2 ()))
        return false;

      refTable->Get (1, 1, v1);
      nativeOp.GetTable ().Get (1, 1, v2);

      if ((v1 != v2) || (v1 != val2))
        return false;
  }

  return true;
}


bool
test_table_alias (IDBSHandler& dbsHnd)
{
  std::cout << "Test native object table alias ...\n";

  bool success = true;


  success = success & test_table_value (dbsHnd, DChar ('a'), T_CHAR, DInt64 (1), T_INT64);
  success = success & test_table_value (dbsHnd, DBool (false), T_BOOL, DText ("Iulian este un programator bun!"), T_TEXT);
  success = success & test_table_value (dbsHnd, DDate (1900,10,11), T_DATE, DText ("Vasile este si el un proframator bun!"), T_TEXT);
  success = success & test_table_value (dbsHnd, DDateTime (1900,10,11, 0, 1, 1), T_DATETIME, DUInt8 (10), T_UINT8);
  success = success & test_table_value (dbsHnd, DHiresTime (1900,10,11, 0, 1, 1, 0), T_HIRESTIME, DUInt16 (32), T_UINT16);
  success = success & test_table_value (dbsHnd, DRichReal (RICHREAL_T (10, 10, 100)), T_RICHREAL, DInt32 (-1), T_INT32);


  return success;
}


template <typename T1, typename T2> bool
test_field_value (IDBSHandler&            dbsHnd,
                  const T1                val1,
                  const DBS_FIELD_TYPE    val1Type,
                  const T2                val2,
                  const DBS_FIELD_TYPE    val2Type)
{
  FieldOperand          op1, op2;
  NativeObjectOperand   nativeOp;

  ITable* refTable = NULL;

    {
      DBSFieldDescriptor fd[2];

      fd[0].isArray   = false;
      fd[0].type      = val1Type;
      fd[0].name      = "field1";

      fd[1].isArray   = false;
      fd[1].type      = val2Type;
      fd[1].name      = "field2";

      refTable = &dbsHnd.CreateTempTable (2, fd);

      if (refTable == NULL)
        return false;

      //This op should take the table ownership!
      TableOperand tableOp (dbsHnd, *refTable);

      op1 = FieldOperand (tableOp.GetTableReference (), 0);
      op2 = FieldOperand (tableOp.GetTableReference (), 1);

      //The table owner ship should be shared with this.
      nativeOp.CopyNativeObjectOperand (
                        NativeObjectOperand (tableOp.GetTableReference ())
                                       );
    }

  if ((&op1.GetTableReference () != &nativeOp.GetTableReference ())
      || (&op2.GetTableReference () != &op1.GetTableReference ())
      || op1.IsNull ()
      || op2.IsNull ())
    {
      return false;
    }

    {
      op1.GetValueAt (0).Operand ().SetValue (val1);
      op2.GetValueAt (1).Operand ().SetValue (val2);
    }

    {
      NativeObjectOperand nField1 (&op1.GetTableReference (),
                                   op1.GetField (),
                                   op1.GetType ());

      if ((nField1.GetType () != op1.GetType ())
          || (nField1.IsNull () != op1.IsNull ())
          || nField1.IsNull ()
          || (nField1.GetField () != op1.GetField ())
          || (nField1.GetField () != 0)
          || (&nField1.GetTable () != &op1.GetTable ())
          || (&nField1.GetTable () != refTable))
        {
          return false;
        }

      T1 v1, v2;

      StackValue second = nField1.Duplicate ();
      IOperand&  op     = second.Operand ();

      refTable->Get (0, 0, v1);
      op.GetTable ().Get (0, op.GetField (), v2);

      if ((v1 != v2) || (v1 != val1))
        return false;

      refTable->Get (1, 0, v1);
      op.GetTable ().Get (1, op.GetField (), v2);

      if ((v1 != v2) || (v1 != T1 ()))
        return false;
    }

    {
      NativeObjectOperand nField2 (&op2.GetTableReference (),
                                   op2.GetField (),
                                   op2.GetType ());

      if ((nField2.GetType () != op2.GetType ())
          || (nField2.IsNull () != op2.IsNull ())
          || nField2.IsNull ()
          || (nField2.GetField () != op2.GetField ())
          || (nField2.GetField () != 1)
          || (&nField2.GetTable () != &op2.GetTable ())
          || (&nField2.GetTable () != refTable))
        {
          return false;
        }

      T2 v1, v2;

      StackValue second = nField2.Duplicate ();
      IOperand&  op     = second.Operand ();

      refTable->Get (0, 1, v1);
      op.GetTable ().Get (0, op.GetField (), v2);

      if ((v1 != v2) || (v1 != T2 ()))
        return false;

      refTable->Get (1, 1, v1);
      op.GetTable ().Get (1, op.GetField (), v2);

      if ((v1 != v2) || (v1 != val2))
        return false;
    }

    {
      T1 v1, v2;

      StackValue second = nativeOp.Duplicate ();
      IOperand&  op     = second.Operand ();

      refTable->Get (0, 0, v1);
      op.GetTable ().Get (0, 0, v2);

      if ((v1 != v2) || (v1 != val1))
        return false;

      refTable->Get (1, 0, v1);
      op.GetTable ().Get (1, 0, v2);

      if ((v1 != v2) || (v1 != T1 ()))
        return false;
    }

    {
      T2 v1, v2;

      StackValue second = nativeOp.Duplicate ();
      IOperand&  op     = second.Operand ();

      refTable->Get (0, 1, v1);
      op.GetTable ().Get (0, 1, v2);

      if ((v1 != v2) || (v1 != T2 ()))
        return false;

      refTable->Get (1, 1, v1);
      op.GetTable ().Get (1, 1, v2);

      if ((v1 != v2) || (v1 != val2))
        return false;
    }

  return true;
}


bool
test_field_alias (IDBSHandler& dbsHnd)
{
  std::cout << "Test native object field alias ...\n";

  bool success = true;


  success = success & test_field_value (dbsHnd, DChar ('B'), T_CHAR, DInt64 (-2), T_INT64);
  success = success & test_field_value (dbsHnd, DBool (false), T_BOOL, DText ("Alt Iulian este un programator bun!"), T_TEXT);
  success = success & test_field_value (dbsHnd, DDate (1900,10,11), T_DATE, DText ("Alt Vasile este si el un proframator bun!"), T_TEXT);
  success = success & test_field_value (dbsHnd, DDateTime (1700,8,1, 0, 1, 1), T_DATETIME, DUInt8 (12), T_UINT8);
  success = success & test_field_value (dbsHnd, DHiresTime (1190, 1, 1, 0, 1, 1, 0), T_HIRESTIME, DUInt16 (89), T_UINT16);
  success = success & test_field_value (dbsHnd, DRichReal (RICHREAL_T (-10, -10, 100)), T_RICHREAL, DInt32 (-1), T_INT32);

  return success;
}
bool
test_native_obj ()
{
  std::cout << "Testing native object handling ...\n";

  TestNativeObj         nativeObj, nativeObj2;
  NativeObjectOperand   op1 (nativeObj);
  NativeObjectOperand   op2;

  if ((op2.GetType () != op1.GetType ())
      && (op1.GetType () != T_UNDETERMINED)
      && ! op2.IsNull ()
      && (op1.IsNull () == false))
    {
      return false;
    }

  TestNativeObj& t = _SC (TestNativeObj&, op1.NativeObject ());
  if ((&t != &nativeObj)
      || (t.RegisterCount () != 1))
    {
      return false;
    }

  op2.CopyNativeObjectOperand (op1);

  if ((&t != &op2.NativeObject ())
      || (&t != & op1.NativeObject ())
      || (t.RegisterCount () != 2))
    {
      return false;
    }

  StackValue sv = op2.Duplicate ();

  if ((&t != &op2.NativeObject ())
      || (&t != &op1.NativeObject ())
      || (&t != &sv.Operand ().NativeObject ())
      || (&t != &nativeObj)
      || (t.RegisterCount () != 3)
      || (t.Memory () != _SC(TestNativeObj&, op1.NativeObject ()).Memory ())
      || (t.Memory () != _SC(TestNativeObj&, op2.NativeObject ()).Memory ())
      || (t.Memory () != nativeObj.Memory ())
      || (t.Memory () !=
          _SC (TestNativeObj&, sv.Operand ().NativeObject ()).Memory ()))
    {
      return false;
    }

  op2.NativeObject (NULL);

  if ((t.RegisterCount () != 2)
      || ! op2.IsNull ())
    {
      return false;
    }

  op1.NativeObject (&nativeObj2);
  if ((&nativeObj2 != &op1.NativeObject ())
       || (nativeObj2.RegisterCount () != 1))
    {
      return false;
    }

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

  {
    IDBSHandler& dbsHnd = DBSRetrieveDatabase (admin);

    success = success && test_basic_value ();
    success = success && test_array_value ();
    success = success && test_table_alias (dbsHnd);
    success = success && test_field_alias (dbsHnd);
    success = success && test_native_obj ();

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
uint32_t WMemoryTracker::smInitCount = 0x80000000;
const char* WMemoryTracker::smModule = "T";
#endif
