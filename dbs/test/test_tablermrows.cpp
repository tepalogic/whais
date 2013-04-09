/*
 * test_tablermrows.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: ipopa
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"

#include "../pastra/ps_table.h"

struct DBSFieldDescriptor field_desc[] = {
    {"test_field", T_UINT32, false}
};

static uint_t gElemsCount = 5000000; /* Scale this down for debug purposes! */

bool
fill_table (I_DBSTable& table)
{
  std::cout << "Fill table with " << gElemsCount << " elements ... " << std::endl;

  bool result = true;
  for (uint32_t index = 0; index < gElemsCount; ++index)
    {
      DBSUInt32 fieldValue (index);
      uint_t rowIndex = table.AddRow ();

      if (rowIndex != index)
        {
          result = false;
          break;
        }

      table.SetEntry (rowIndex, 0, fieldValue);

      std::cout << index + 1 << " (" << gElemsCount << ")\r";
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

bool
remove_first_rows (I_DBSTable& table)
{
  std::cout << "Deleting " << gElemsCount / 2 << " rows ... " << std::endl;
  bool result = true;

  const uint32_t count = gElemsCount / 2;

  for (uint32_t rowIndex = count; rowIndex > 0; --rowIndex)
    {
      if ((rowIndex % 21) == 0)
        {
          table.MarkRowForReuse (rowIndex);
        }
      else
        {
          DBSUInt32 fieldValue;
          table.SetEntry (rowIndex, 0, fieldValue);
        }

      if (table.AddReusedRow() != rowIndex)
        {
          result = false;
          break;
        }

      std::cout << count - rowIndex + 1 << " (" << count << ")\r";
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

bool
restore_first_rows (I_DBSTable& table)
{
  std::cout << "Restore the first " << gElemsCount / 2 << " rows ... " << std::endl;
  bool result = true;

  const uint32_t count = gElemsCount / 2;

  for (uint32_t rowIndex = 1; rowIndex <= count; ++rowIndex)
    {
      DBSUInt32 fieldValue (rowIndex);
      table.SetEntry (rowIndex, 0, fieldValue);

      if ((rowIndex < count) && (table.AddReusedRow() != (rowIndex + 1)))
        {
          result = false;
          break;
        }
      else if ((rowIndex == count) && (table.AddReusedRow() != gElemsCount ))
        {
          result = false;
          break;
        }


      std::cout << rowIndex << " (" << count << ")\r";
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

bool
test_for_radius_rows (I_DBSTable& table)
{
  std::cout << "Delete rows symmetrically ... " << std::endl;
  bool result = true;

  const uint32_t count = gElemsCount / 10;

  for (uint_t rowIndex = 0; rowIndex < count; ++rowIndex)
    {
      DBSUInt32 fieldValue;
      table.SetEntry ((gElemsCount / 2) - rowIndex, 0, fieldValue);
      table.SetEntry ((gElemsCount / 2) + count - rowIndex, 0, fieldValue);

      if (table.AddReusedRow() != ((gElemsCount / 2) - rowIndex))
        {
          result = false;
          break;
        }

      std::cout << rowIndex << " (" << count << ")\r";
    }

  if (result)
    {
      std::cout << std::endl << "Restore symmetric rows ... " << std::endl;

      for (uint_t rowIndex = (gElemsCount / 2) - count + 1;
          rowIndex <= (gElemsCount / 2) + count;
          ++rowIndex)
        {
          if (table.AddReusedRow() != rowIndex)
            {
              result = false;
              break;
            }
          DBSUInt32 fieldValue (rowIndex);
          table.SetEntry (rowIndex, 0, fieldValue);

          std::cout << rowIndex - ((gElemsCount / 2) - count + 1) << " (" << count * 2 << ")\r";

        }
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

const char db_name[] = "t_baza_date_1";

int
main (int argc, char **argv)
{
  if (argc > 1)
    {
      gElemsCount = atol (argv[1]);
    }

  bool success = true;
  {
    DBSInit (DBSSettings ());
    DBSCreateDatabase (db_name);
  }

  I_DBSHandler& handler = DBSRetrieveDatabase (db_name);
  handler.AddTable ("t_test_tab", sizeof field_desc / sizeof (field_desc[0]), field_desc);
  I_DBSTable& table        = handler.RetrievePersistentTable ("t_test_tab");
  I_DBSTable& spawnedTable = table.Spawn ();

  success = success && fill_table (table);
  success = success && remove_first_rows (table);
  success = success && restore_first_rows (table);
  success = success && test_for_radius_rows (table);

  handler.ReleaseTable (table);

  success = success && fill_table (spawnedTable);
  success = success && remove_first_rows (spawnedTable);
  success = success && restore_first_rows (spawnedTable);
  success = success && test_for_radius_rows (spawnedTable);

  handler.ReleaseTable (spawnedTable);

  DBSReleaseDatabase (handler);
  DBSRemoveDatabase (db_name);
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
