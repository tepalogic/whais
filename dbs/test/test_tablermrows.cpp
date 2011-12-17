/*
 * test_tablermrows.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: ipopa
 */

#include <assert.h>
#include <iostream>
#include <string.h>
#include <vector>

#include "test/test_fmw.h"

#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"

#include "../pastra/ps_table.h"

struct DBSFieldDescriptor field_desc[] = {
    {"test_field", T_UINT32, false}
};

static const D_UINT table_elemes_count = 100000000;

bool
fill_table (I_DBSTable &table)
{
  std::cout << "Fill table with " << table_elemes_count << " elements ... " << std::endl;

  bool result = true;
  for (D_UINT32 index = 0; index < table_elemes_count; ++index)
    {
      DBSUInt32 fieldValue (false, index);
      D_UINT rowIndex = table.AddRow();

      if (rowIndex != index + 1)
        {
          result = false;
          break;
        }

      table.SetEntry (fieldValue, rowIndex, 0);

      if ((index % 100000) == 0)
        std::cout << index / 100000 << "(" << table_elemes_count / 100000 << ")\r";
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

const D_CHAR db_name[] = "t_baza_date_1";
int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialised
  // We need not to test against it!
  std::cout << "Print a message to not confuse the memory tracker: " << (D_UINT) 0x3456 << "\n";
  D_UINT prealloc_mem = test_get_mem_used ();
  bool success = true;

  {
    std::string dir = ".";
    dir += whc_get_directory_delimiter ();

    DBSInit (dir.c_str (), dir.c_str ());
  }

  DBSCreateDatabase (db_name);
  I_DBSHandler & handler = DBSRetrieveDatabase (db_name);
  handler.AddTable ("t_test_tab", field_desc, sizeof field_desc / sizeof (field_desc[0]));
  I_DBSTable &table = handler.RetrievePersistentTable ("t_test_tab");

  success = success & fill_table (table);

  handler.ReleaseTable (table);
  DBSReleaseDatabase (handler);
  DBSRemoveDatabase (db_name);
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
  std::cout << "Current memory usage: " << mem_usage << " bytes." << std::
            endl;
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return -1;
    }
  else
    std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}


