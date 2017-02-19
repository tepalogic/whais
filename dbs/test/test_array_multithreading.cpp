
#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "utils/wrandom.h"
#include "utils/wthread.h"
#include "custom/include/test/test_fmw.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"

using namespace std;
using namespace whais;



#define TEST_VALUES_COUNT 5

const char db_name[] = "t_baza_date_1";

uint32_t  _iterationsCount = 400000;




struct DBSFieldDescriptor fieldsDescs[] =
{
    {"a", T_INT64, true}
};

static ITable*    refTable;
static DArray*    testValues;
static bool       testEnd;
static bool       testResult;
static bool       resetStart;

static void
test_array_sort(void *)
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep(100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd() % TEST_VALUES_COUNT;

        testValues[j].Sort();

        while(resetStart)
          wh_sleep(1);

        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_array_remove(void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep(100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd() % TEST_VALUES_COUNT;

        testValues[j].Remove(0);

        while(resetStart)
          wh_sleep(1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
test_array_set(void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep(100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd() % TEST_VALUES_COUNT;

        testValues[j].Set(0, (wh_rnd() & 1) ? DInt64() : DInt64(wh_rnd()));

        while(resetStart)
          wh_sleep(1);


        if (wh_rnd() & 1)
          wh_yield();

      }
  } catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
test_array_add(void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep(100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd() % TEST_VALUES_COUNT;

        testValues[j].Add(DInt64(wh_rnd()));

        while(resetStart)
          wh_sleep(1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_array_get(void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep(100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        DInt64 v;
        const uint_t j = wh_rnd() % TEST_VALUES_COUNT;

        testValues[j].Get(0, v);

        while(resetStart)
          wh_sleep(1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_mirror(void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep(100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd() % TEST_VALUES_COUNT;
        const uint_t z = wh_rnd() % TEST_VALUES_COUNT;

        testValues[j].MakeMirror(testValues[z]);

        while(resetStart)
          wh_sleep(1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
reset_array_variables(void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
      while( ! testEnd)
        {
          resetStart = true;
          uint_t i;
          for (i = 0; i < TEST_VALUES_COUNT; ++i)
            refTable->Get(i, 0, testValues[i]);
          resetStart = false;
          wh_sleep(10);
        }

  }
  catch(Exception& e)
  {
      testResult = false;
      testEnd = true;
      cout << "Got an exception in " << __FUNCTION__ << ":\n\t"
           << e.Description() << '(' << e.Code() << '/' << e.Type() << ")\n"
           << e.Message() << endl;

      throw;
  }
  catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got an unknown exception in " << __FUNCTION__ << endl;
      throw;
  }

  cout << "Ended " << __FUNCTION__ << endl;
}

int
main(int argc, char** argv)
{
  if (argc > 1)
    _iterationsCount = atol(argv[1]);

  cout << "Iteration count set at " << _iterationsCount << endl;

  {
    DBSInit(DBSSettings());
    DBSCreateDatabase(db_name);
    IDBSHandler& handler = DBSRetrieveDatabase(db_name);

    refTable = &handler.CreateTempTable(1, fieldsDescs);

    DArray t;

    t.Add(DInt64(3));
    refTable->Set(0, 0, t);

    t = DArray();
    t.Add(DInt64(-10));
    t.Add(DInt64(-12));
    t.Add(DInt64(0));
    refTable->Set(1, 0, t);

    t = DArray();
    t.Add(DInt64(19));
    t.Add(DInt64(10));
    refTable->Set(2, 0, t);

    t = DArray();
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    t.Add(DInt64(-89));
    t.Add(DInt64(-111));
    t.Add(DInt64(0x98112));
    refTable->Set(3, 0, t);

    t = DArray();
    refTable->Set(4, 0, t);
    DArray values[TEST_VALUES_COUNT];

    testValues  = values;
    testEnd     = false;
    testResult  = true;

    Thread th[7];

    th[0].Run(reset_array_variables, NULL);
    th[1].Run(test_array_remove, NULL);
    th[2].Run(test_array_set, NULL);
    th[3].Run(test_array_add, NULL);
    th[4].Run(test_array_get, NULL);
    th[5].Run(test_array_sort, NULL);
    th[6].Run(test_mirror, NULL);

    th[1].WaitToEnd(true);
    th[2].WaitToEnd(true);
    th[3].WaitToEnd(true);
    th[4].WaitToEnd(true);
    th[5].WaitToEnd(true);
    th[6].WaitToEnd(true);


    testEnd = true;

    th[0].WaitToEnd(true);

    handler.ReleaseTable(*refTable);
    DBSReleaseDatabase(handler);
  }

  DBSRemoveDatabase(db_name);
  DBSShoutdown();

  if (!testResult)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 1;
const char* WMemoryTracker::smModule = "T";
#endif

