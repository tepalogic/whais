

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



#define TEST_VALUES_COUNT 10

const char db_name[] = "t_baza_date_1";

uint32_t  _iterationsCount = 1000;



struct DBSFieldDescriptor fieldsDescs[] =
{
    {"t", T_TEXT, false}
};

static ITable*    refTable;
static DText*     testValues;
static bool       testEnd;
static bool       testResult;
static bool       resetStart;


static void
test_append_char (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  try
  {
    uint_t i;
    for (i = 0; ((i < _iterationsCount) && !testEnd) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;
        const uint_t z = wh_rnd () % TEST_VALUES_COUNT;

        testValues[j].Append (testValues[z].CharAt (0));

        while (resetStart)
          wh_sleep (1);

        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
test_append_text (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  uint_t i;
  try
  {
    for (i = 0; ((i < _iterationsCount) && !testEnd) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;
        const uint_t z = wh_rnd () % TEST_VALUES_COUNT;

        testValues[j].Append (testValues[z]);

        while (resetStart)
          wh_sleep (1);

        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }

  try
  {
    for (i = 0; i < TEST_VALUES_COUNT; ++i)
     testValues[i].Append (testValues[i]);

  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << " nr. 2\n";
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;

}

static void
test_update_char (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  try
  {
    uint_t i;
    for (i = 0; ((i < _iterationsCount) && !testEnd) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;
        const uint_t c = wh_rnd () % 100 + 1;

        testValues[j].CharAt(0, DChar (c));

        while (resetStart)
          wh_sleep (1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
test_find_in_text (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;
        const uint_t z = wh_rnd () % TEST_VALUES_COUNT;

        testValues[j].FindInText (testValues[z],
                                  (wh_rnd () & 1) ? true : false);

        while (resetStart)
          wh_sleep (1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_find_subtext (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;
        const uint_t z = wh_rnd () % TEST_VALUES_COUNT;

        testValues[j].FindSubstring (testValues[z],
                                    (wh_rnd () & 1) ? true : false);

        while (resetStart)
          wh_sleep (1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
test_to_lower (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;

        testValues[j].LowerCase ();

        while (resetStart)
          wh_sleep (1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
test_to_upper (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;

        testValues[j].UpperCase ();

        while (resetStart)
          wh_sleep (1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}


static void
test_mirror (void* )
{
  cout << "Started " << __FUNCTION__ << endl;
  wh_sleep (100);

  try
  {
    uint_t i;
    for (i = 0; (i < _iterationsCount) && !testEnd; ++i)
      {
        const uint_t j = wh_rnd () % TEST_VALUES_COUNT;
        const uint_t z = wh_rnd () % TEST_VALUES_COUNT;

        testValues[j].MakeMirror (testValues[z]);

        while (resetStart)
          wh_sleep (1);


        if (wh_rnd() & 1)
          wh_yield();
      }
  } catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }
  cout << "Ended " << __FUNCTION__ << endl;
}

static void
reset_text_variables (void *)
{
  cout << "Started " << __FUNCTION__ << endl;

  try
  {
      while ( ! testEnd)
        {
          resetStart = true;
          if (wh_rnd () & 1)
            {
              uint_t i;
              for (i = 0; i < TEST_VALUES_COUNT; ++i)
                refTable->Get (i, 0, testValues[i]);
            }
          else
            {
              testValues[0] = DText ("The first smart text... I don't know!");
              testValues[1] = DText ("My name is James! James Bond 1.1");
              testValues[2] = DText ("Eating is all the joy some people want.");
              testValues[3] = DText ("Not eating too much carbohydrates is the joy of other people want.");
              testValues[4] = DText ("What's life without the joy of coding?!? A Java's programmer life... :))))))))))))))))))))))))))");
              testValues[5] = DText ("Exception are not the answer, but sometimes they are the solution :)))))))))))))))))))))))))) I fell funny today!");
              testValues[6] = DText ("You need to go after were finishing here. Really!");
              testValues[7] = DText ("Small text!");
              testValues[8] = DText ("My name is Junes. Nice to meet you.");
            }
          resetStart = false;
          wh_sleep (10);
        }

  }
  catch (...)
  {
      testResult = false;
      testEnd = true;
      cout << "Got exception in " << __FUNCTION__ << endl;
      throw;
  }

  cout << "Ended " << __FUNCTION__ << endl;
}

int
main( int argc, char** argv)
{
  if (argc > 1)
    _iterationsCount = atol( argv[1]);

  cout << "Iteration count set at " << _iterationsCount << endl;

  {
    DBSInit( DBSSettings());
    DBSCreateDatabase( db_name);
    IDBSHandler& handler = DBSRetrieveDatabase( db_name);

    refTable = &handler.CreateTempTable (1, fieldsDescs);

    refTable->Set (0, 0, DText ("The first smart text... I don't know!"));
    refTable->Set (1, 0, DText ("My name is James! James Bond 1.1"));
    refTable->Set (2, 0, DText ("Eating is all the joy some people want."));
    refTable->Set (3, 0, DText ("Not eating too much carbohydrates is the joy of other people want."));
    refTable->Set (4, 0, DText ("What's life without the joy of coding?!? A Java's programmer life... :))))))))))))))))))))))))))"));
    refTable->Set (5, 0, DText ("Exception are not the answer, but sometimes they are the solution :)))))))))))))))))))))))))) I fell funny today!"));
    refTable->Set (6, 0, DText ("You need to go after were finishing here. Really!"));
    refTable->Set (7, 0, DText ("Small text!"));
    refTable->Set (8, 0, DText ("My name is Junes. Nice to meet you."));
    refTable->Set (9, 0, DText ());
    DText values[TEST_VALUES_COUNT];

    testValues  = values;
    testEnd     = false;
    testResult = true;

    Thread th[9];

    th[0].Run (reset_text_variables, NULL);
    th[1].Run (test_append_char, NULL);
    th[2].Run (test_append_text, NULL);
    th[3].Run (test_mirror, NULL);
    th[4].Run (test_to_lower, NULL);
    th[5].Run (test_to_upper, NULL);
    th[6].Run (test_update_char, NULL);
    th[7].Run (test_find_in_text, NULL);
    th[8].Run (test_find_subtext, NULL);

    th[1].WaitToEnd (true);
    th[2].WaitToEnd (true);
    th[3].WaitToEnd (true);
    th[4].WaitToEnd (true);
    th[5].WaitToEnd (true);
    th[6].WaitToEnd (true);
    th[7].WaitToEnd (true);
    th[8].WaitToEnd (true);

    testEnd = true;

    th[0].WaitToEnd (true);

    handler.ReleaseTable (*refTable);
    DBSReleaseDatabase( handler);
  }

  DBSRemoveDatabase( db_name);
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

