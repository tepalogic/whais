/*
 * test_dbsmgr.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"

using namespace whisper;

static DBSFieldDescriptor field_descs[] =
{
  {"Field_1", T_BOOL, false},
  {"AnotherField", T_INT32, true},
  {"dumy2field_", T_UINT64, false},
  {"dumy21field_", T_UINT64, true},
  {"dumy22field_", T_TEXT, false},
  {"f_data", T_DATE, false },
  {"f_int16", T_UINT16, false},
  {"f_hirestime", T_HIRESTIME, false},
  {"f_int8", T_UINT8, false}
};

static const uint_t descCount = sizeof (field_descs) / sizeof field_descs[0];

bool
operator!= (const DBSFieldDescriptor& field_1,
            const DBSFieldDescriptor& field_2)
{
  return (field_1.m_FieldType != field_2.m_FieldType) ||
         (field_1.isArray != field_2.isArray) ||
         (strcmp (field_1.m_pFieldName, field_2.m_pFieldName) != 0);
}

static bool
test_fields (I_DBSTable& table)
{

  DBSFieldDescriptor field_d;

  std::cout << "Real fields' order: " << std::endl;
  for (uint_t index = 0; index < descCount; ++index)
    {
      field_d = table.GetFieldDescriptor (index);
      std::cout << "\t" << field_d.m_pFieldName << std::endl;
    }

  if (table.GetFieldsCount () != descCount)
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("Field_1"));
  if (field_d != field_descs[0])
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("AnotherField"));
  if (field_d != field_descs[1])
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("dumy2field_"));
  if (field_d != field_descs[2])
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("dumy21field_"));
  if (field_d != field_descs[3])
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("dumy22field_"));
  if (field_d != field_descs[4])
    return false;


  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("f_data"));
  if (field_d != field_descs[5])
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("f_int16"));
  if (field_d != field_descs[6])
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("f_hirestime"));
  if (field_d != field_descs[7])
    return false;

  field_d = table.GetFieldDescriptor (table.GetFieldIndex ("f_int8"));
  if (field_d != field_descs[8])
    return false;


  return true;
}


int
main ()
{
  bool success = true;
  {
    DBSInit (DBSSettings ());
    DBSCreateDatabase ("baza_date_1");
  }

  I_DBSHandler& handler = DBSRetrieveDatabase ("baza_date_1");
  handler.AddTable ("table_1", descCount, field_descs);
  I_DBSTable& table = handler.RetrievePersistentTable ("table_1");
  handler.ReleaseTable (table);
  DBSReleaseDatabase (handler);
  DBSShoutdown ();

    {
      DBSInit (DBSSettings ());

      I_DBSHandler& handler = DBSRetrieveDatabase ("baza_date_1");
      I_DBSTable& table = handler.RetrievePersistentTable ("table_1");

      if (table.GetFieldsCount () != descCount)
        success = false;
      else
        {
          success = test_fields (table);

          if (success)
            {
              I_DBSTable& spawnedTable = table.Spawn ();
              success = test_fields (spawnedTable);
              handler.ReleaseTable (spawnedTable);
            }

          handler.ReleaseTable (table);

          DBSReleaseDatabase (handler);
          DBSRemoveDatabase ("baza_date_1");
          DBSShoutdown ();
        }
    }

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
