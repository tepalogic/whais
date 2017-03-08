#include <assert.h>
#include <iostream>
#include <cstring>
#include <cstdlib>


#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "utils/wfile.h"

using namespace std;
using namespace whais;


static const char testDbName_1[]       = "t_baza_date_1";
static const char testDbName_2[]       = "t_baza_date_2";
static const char testTableName[]      = "t_table_1";

struct DBSFieldDescriptor field_descs[] = {
    {"a", T_UINT8, false},
    {"b", T_UINT16, false},
    {"c", T_UINT32, false},
    {"d", T_UINT64, false},
    {"e", T_INT64, false},
    {"f", T_INT32, false},
    {"g", T_INT16, false},
    {"h", T_INT8, false}
};


bool
test_dbs_creation()
{
  cout << "Testing database creation ... ";

  bool result = false;

  DBSCreateDatabase(testDbName_2, nullptr);
  try
  {
      DBSCreateDatabase(testDbName_2, nullptr);

      cout << "Should not be possible to create a database with same name.";

      result = false;
  }
  catch(FileException&)
  {
      result = true;
  }

  DBSRemoveDatabase(testDbName_2, nullptr);

  cout << (result  ? " OK\n" : " (FAIL)\n");

  return result;
}


bool
test_table_addition(IDBSHandler& hnd)
{
  bool result = false;

  cout << "Test table addition ... ";

  hnd.AddTable(testTableName,
                sizeof field_descs /sizeof(field_descs[0]),
                field_descs);
  try
  {
      hnd.AddTable(testTableName,
                    sizeof field_descs /sizeof(field_descs[0]),
                    field_descs);

      cout << "Should not be possible to add a table with the same name.";
  }
  catch(DBSException& e)
  {
      if (e.Code() != DBSException::TABLE_EXISTS)
        {
          cout << "Test received unexpected exception code.";

          result &= false;
        }
      else
        result = true;
  }

  cout << (result  ? " OK\n" : " (FAIL)\n");

  return result;
}

 bool
 test_table_management()
 {
   bool result = true;

   cout << "Test table management ... ";

   IDBSHandler& hnd = DBSRetrieveDatabase(testDbName_1, nullptr);
   ITable& tbl      = hnd.RetrievePersistentTable(testTableName);
   ITable& t_tbl    = hnd.CreateTempTable(sizeof field_descs / sizeof field_descs[0],
                                           field_descs);
   try
   {
       ITable& tbl2 = hnd.RetrievePersistentTable(testTableName);

       cout << "Should be possible to retrieve a table twice in a row without"
            << " been released.";

       result &= false;

       hnd.ReleaseTable(tbl2);
   }
   catch(DBSException& e)
   {
       if (e.Code() != DBSException::TABLE_IN_USE)
         result &= false;
   }

   try
   {
     DBSReleaseDatabase(hnd);

     cout << "Released a database with unreleased tables.";
     result &= false;
   }
   catch(DBSException& e)
   {
       if (e.Code() != DBSException::DATABASE_IN_USE)
         result &= false;
   }

   hnd.ReleaseTable(tbl);

   try
   {
     DBSReleaseDatabase(hnd);

     cout << "Released a database with unreleased tables.";
     result &= false;
   }
   catch(DBSException& e)
   {
       if (e.Code() != DBSException::DATABASE_IN_USE)
         result &= false;
   }

   hnd.ReleaseTable(t_tbl);

   DBSReleaseDatabase(hnd);

   cout << (result ?  " OK\n"  : "(FAIL)\n");

   return result;
 }

int
main(int argc, char** argv)
{
  bool success = true;

  DBSInit(DBSSettings());
  DBSCreateDatabase(testDbName_1);

  IDBSHandler& hnd = DBSRetrieveDatabase(testDbName_1, nullptr);

  success &= test_dbs_creation();
  success &= test_table_addition(hnd);

  DBSReleaseDatabase(hnd);

  success &= test_table_management();

  DBSRemoveDatabase(testDbName_1);
  DBSShoutdown();

  if ( ! success)
    {
      cout << "TEST RESULT: FAIL" << std::endl;
      return 1;
    }

  cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}


#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
