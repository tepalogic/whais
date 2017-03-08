
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

uint32_t  _iterationsCount = 5000;



struct DBSFieldDescriptor fieldsDescs[] =
{
    {"f_int", T_INT64, false},
    {"f_array", T_INT64, true},
    {"f_array_2", T_INT64, true},
    {"f_text", T_TEXT, false},
    {"f_text_2", T_TEXT, false}
};

static ITable*    refTable;
static ITable*    refTable2;
static bool       testEnd;
static bool       testResult;


static void
test_table_sort(void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
    for (uint_t i = 0; !testEnd; ++i)
      {
        const FIELD_INDEX f = refTable->RetrieveField(((wh_rnd() & 1) == 0)
                                                       ? "f_int"
                                                       : "f_text");

        refTable->Sort(f, 0, refTable->AllocatedRows(), (i & 1) == 0);

        wh_sleep(3);
      }
  }
  catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_table_index(void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
    for (uint_t i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const FIELD_INDEX f = refTable->RetrieveField("f_int");

        if (refTable->IsIndexed(f))
          {
            if ((wh_rnd() & 1) == 0)
              refTable->RemoveIndex(f);
          }
        else if ((wh_rnd() & 1) == 0)
          refTable->CreateIndex(f, nullptr, nullptr);

        if (wh_rnd() & 1)
          wh_yield();
      }
  }
  catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_table_test_rows_basic(void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
    for (uint_t i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const FIELD_INDEX f = refTable->RetrieveField("f_int");

        switch(wh_rnd()  % 3)
        {
          case 0:
            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f, DInt64(wh_rnd()));
            refTable2->Set(wh_rnd() % refTable2->AllocatedRows(), f, DInt64(wh_rnd()));
            break;

          case 1:
            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f, DInt64());
            refTable2->Set(wh_rnd() % refTable2->AllocatedRows(), f, DInt64(wh_rnd()));
            break;

          default:
            {
              DInt64 value;
              ROW_INDEX row = wh_rnd() % refTable->AllocatedRows();

              refTable->Get(row, f, value);
              refTable->Set(row, f, value);
            }
        }
      }
  }
  catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_table_test_rows_array(void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
    for (uint_t i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const ROW_INDEX row = wh_rnd() % refTable->AllocatedRows();

        DArray value;
        const FIELD_INDEX f1 = refTable->RetrieveField("f_array");
        const FIELD_INDEX f2 = refTable->RetrieveField("f_array_2");

        switch(wh_rnd()  % 4)
        {
          case 0:

            for (int el = 0, p = wh_rnd() % 100; el < p; ++el)
              value.Add(DInt64(wh_rnd()));

            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f2, value);

            if ((wh_rnd() & 1) == 0)
              value.Add(DInt64(wh_rnd()));

            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f1, value);
            refTable2->Set(wh_rnd() % refTable2->AllocatedRows(), f1, value);
            break;

          case 1:
            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f1, value);
            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f2, value);

            break;

          case 2:
            refTable->Get(row, f1, value);
            refTable->Set(row, f1, value);

            refTable->Get(row, f2, value);
            refTable->Set(row, f2, value);

            refTable2->Set(row, f2, value);
            break;

          default:
            refTable->Get(row, f1, value);
            refTable->Set(row, f2, value);
            refTable2->Set(row, f2, value);
        }
      }
  }
  catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_table_test_rows_text(void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
    for (uint_t i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const ROW_INDEX row = wh_rnd() % refTable->AllocatedRows();

        DText value;
        const FIELD_INDEX f1 = refTable->RetrieveField("f_text");
        const FIELD_INDEX f2 = refTable->RetrieveField("f_text_2");

        switch(wh_rnd()  % 4)
        {
          case 0:

            for (int el = 0, p = wh_rnd() % 300; el < p; ++el)
              value.Append(DChar(wh_rnd() % 0xD800));

            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f2, value);

            if ((wh_rnd() & 1) == 0)
              value.Append(DChar(wh_rnd() % 0xD800));

            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f1, value);
            refTable2->Set(wh_rnd() % refTable2->AllocatedRows(), f1, value);
            break;

          case 1:
            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f1, value);
            refTable->Set(wh_rnd() % refTable->AllocatedRows(), f2, value);
            break;

          case 2:
            refTable->Get(row, f1, value);
            refTable->Set(row, f1, value);

            refTable->Get(row, f2, value);
            refTable->Set(row, f2, value);
            refTable2->Set(row, f2, value);
            break;

          default:
            refTable->Get(row, f1, value);
            refTable->Set(row, f2, value);
            refTable2->Set(row, f1, value);
        }
      }
  }
  catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}



static void
test_table_rows(void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
    for (uint_t i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        if (refTable->ReusableRowsCount() < refTable->AllocatedRows() / 3)
          {
            refTable->AddRow();
            refTable2->AddRow();
          }
        else
          {
            refTable->MarkRowForReuse(wh_rnd() % refTable->AllocatedRows());
            refTable->GetReusableRow(true);

            refTable2->MarkRowForReuse(wh_rnd() % refTable2->AllocatedRows());
            refTable2->GetReusableRow(true);
          }

        if (wh_rnd() & 1)
          wh_yield();
      }
  }
  catch(...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
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

    handler.AddTable("t_test_table", 5, fieldsDescs);
    refTable  = &handler.RetrievePersistentTable("t_test_table");
    refTable2 = &handler.CreateTempTable(5, fieldsDescs);

    refTable->AddRow();
    refTable2->AddRow();

    testEnd     = false;
    testResult  = true;

    Thread th[6];

    th[0].Run(test_table_rows, nullptr);
    th[1].Run(test_table_index, nullptr);
    th[2].Run(test_table_test_rows_array, nullptr);
    th[3].Run(test_table_test_rows_text, nullptr);
    th[4].Run(test_table_test_rows_basic, nullptr);
    th[5].Run(test_table_sort, nullptr);

    th[0].WaitToEnd(true);
    th[1].WaitToEnd(true);
    th[2].WaitToEnd(true);
    th[3].WaitToEnd(true);
    th[4].WaitToEnd(true);

    wh_sleep(100);
    testEnd = true;

    th[5].WaitToEnd(true);

    handler.ReleaseTable(*refTable);
    if (testResult)
      {
        cout << "Starting the second round.\n";

        refTable = &handler.RetrievePersistentTable("t_test_table");
        testEnd  = false;

        th[0].Run(test_table_rows, nullptr);
        th[1].Run(test_table_index, nullptr);
        th[2].Run(test_table_test_rows_array, nullptr);
        th[3].Run(test_table_test_rows_text, nullptr);
        th[4].Run(test_table_test_rows_basic, nullptr);
        th[5].Run(test_table_sort, nullptr);

        th[0].WaitToEnd(true);
        th[1].WaitToEnd(true);
        th[2].WaitToEnd(true);
        th[3].WaitToEnd(true);
        th[4].WaitToEnd(true);

        wh_sleep(100);
        testEnd = true;

        th[5].WaitToEnd(true);

        handler.ReleaseTable(*refTable);
      }

    handler.ReleaseTable(*refTable2);
    handler.DeleteTable("t_test_table");
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

