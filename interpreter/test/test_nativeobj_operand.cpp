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

using namespace whais;
using namespace prima;


static const char admin[] = "administrator";


class TestNativeObj : public INativeObject
{
public:
  TestNativeObj()
    : mRegistered(0),
      mChunkMemory(new int[1024])
  {
  }

  virtual ~TestNativeObj()
  {
    if ((mRegistered != 0)
        || (mChunkMemory != nullptr))
      {
        assert(FALSE);
        *(volatile char*)0 = 0; //Hopefully will trigger a crash!
      }
  }

  virtual void RegisterUser()
  {
    ++mRegistered;
    if (mChunkMemory == nullptr)
      throw "Where is the memory!";
  }

  virtual void ReleaseUser()
  {
    if (mRegistered <= 0)
      throw "mRegistred has not a proper value.";

    if (--mRegistered == 0)
      {
        delete [] mChunkMemory;

        mChunkMemory = nullptr;
      }
  }

  virtual void* Memory() { return _SC(void*, mChunkMemory); }
  virtual int   RegisterCount() { return mRegistered; }

private:
  int     mRegistered;
  int*    mChunkMemory;
};

bool
test_native_obj()
{
  std::cout << "Testing native object handling ...\n";

  TestNativeObj      nativeObj, nativeObj2;
  UndefinedOperand   op1(nativeObj);

  if ((op1.GetType() != T_UNDETERMINED)
      && (op1.IsNull() == false))
    {
      return false;
    }

  TestNativeObj& t = _SC(TestNativeObj&, op1.NativeObject());
  if ((&t != &nativeObj)
      || (t.RegisterCount() != 1))
    {
      return false;
    }

  StackValue sv = op1.Duplicate();

  if ((&t != &op1.NativeObject())
      || (&t != &sv.Operand().NativeObject())
      || (&t != &nativeObj)
      || (t.RegisterCount() != 2)
      || (t.Memory() != _SC(TestNativeObj&, op1.NativeObject()).Memory())
      || (t.Memory() != nativeObj.Memory())
      || (t.Memory() !=
          _SC(TestNativeObj&, sv.Operand().NativeObject()).Memory()))
    {
      return false;
    }

  op1.NativeObject(nullptr);

  if ((t.RegisterCount() != 1)
      || ! op1.IsNull()
      || sv.Operand().IsNull())
    {
      return false;
    }

  op1.NativeObject(&nativeObj2);
  if ((&nativeObj2 != &op1.NativeObject())
       || (nativeObj2.RegisterCount() != 1))
    {
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

  {
    IDBSHandler& dbsHnd = DBSRetrieveDatabase(admin);
    success = success && test_native_obj();

    DBSReleaseDatabase(dbsHnd);
  }

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
uint32_t WMemoryTracker::smInitCount = 0x80000000;
const char* WMemoryTracker::smModule = "T";
#endif
