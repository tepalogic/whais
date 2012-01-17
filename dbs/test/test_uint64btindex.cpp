/*
 * test_uint64btindex.cpp
 *
 *  Created on: Jan 11, 2012
 *      Author: ipopa
 */

#include <assert.h>
#include <iostream>
#include <string.h>
#include <vector>

#include "utils/include/random.h"
#include "test/test_fmw.h"

#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"

#include "../pastra/ps_table.h"

struct DBSFieldDescriptor field_desc[] = {
    {"test_field", T_UINT64, false}
};

const D_CHAR db_name[] = "t_baza_date_1";
const D_CHAR tb_name[] = "t_test_tab";

D_UINT _rowsCount   = 100000000;
D_UINT _removedRows = _rowsCount / 10;


bool
fill_table_with_values (I_DBSTable &table,
                        const D_UINT32 rowCount,
                        D_UINT64 seed,
                        DBSArray &tableValues)
{
  bool     result = true;

  table.CreateFieldIndex (0, NULL, NULL);
  std::cout << "Filling table with values ... " << std::endl;

  w_rnd_set_seed (seed);
  for (D_UINT index = 0; index < rowCount; ++index)
    {
      DBSUInt64 value (false, w_rnd ());
      if (table.AddRow () != index)
        {
          result = false;
          break;
        }

      std::cout << "\r" << index << "(" << rowCount << ")";
      table.SetEntry (value, index, 0);
      tableValues.AddElement (value);

    }

  std::cout << std::endl << "Check table with values ... " << std::endl;
  DBSArray values = table.GetMatchingRows (DBSUInt64 (true),
                                           DBSUInt64 (false, ~0),
                                           0,
                                           ~0,
                                           0,
                                           ~0,
                                           0);
  if ((values.GetElementsCount() != tableValues.GetElementsCount ()) ||
      (values.GetElementsCount () != rowCount))
    {
      result = false;
    }

  for (D_UINT checkIndex = 0; (checkIndex < rowCount) && result; ++checkIndex)
    {
      DBSUInt64  rowValue (true);
      DBSUInt64 rowIndex (true);

      values.GetElement (rowIndex, checkIndex);
      assert (rowIndex.IsNull() == false);

      table.GetEntry (rowValue, rowIndex.m_Value, 0);

      DBSUInt64 generated (true);
      tableValues.GetElement (generated, rowIndex.m_Value);
      assert (generated.IsNull() == false);

      if ((rowValue == generated) == false)
        {
          result = false;
          break;
        }

      std::cout << "\r" << checkIndex << "(" << rowCount << ")";
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;

  return result;
}

bool
fill_table_with_first_nulls (I_DBSTable &table, const D_UINT32 rowCount)
{
  bool result = true;
  std::cout << "Set NULL values for the first " << rowCount << " rows!" << std::endl;

  DBSUInt64 nullValue (true);

  for (D_UINT64 index = 0; index < rowCount; ++index)
    {
      table.SetEntry (nullValue, index, 0);

      std::cout << "\r" << index << "(" << rowCount << ")";
    }

  DBSArray values = table.GetMatchingRows (nullValue,
                                           nullValue,
                                           0,
                                           ~0,
                                           0,
                                           ~0,
                                           0);

  for (D_UINT64 index = 0; (index < rowCount) && result; ++index)
    {
      DBSUInt64 element (true);
      values.GetElement (element, index);

      if (element.IsNull() || (element.m_Value != index))
        result = false;

      DBSUInt64 rowValue(true, 31);
      table.GetEntry (rowValue, index, 0);

      if (rowValue.IsNull() == false)
        result = false;
    }

  if (values.GetElementsCount() != rowCount)
    result = false;

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;

  return result;
}

bool
test_table_index_survival (I_DBSHandler& dbsHnd, DBSArray& tableValues)
{
  bool result = true;
  std::cout << "Test index survival ... ";

  I_DBSTable& table = dbsHnd.RetrievePersistentTable (tb_name);

  DBSUInt64 nullValue (true);
  DBSArray values  = table.GetMatchingRows (nullValue,
                                            nullValue,
                                            0,
                                            ~0,
                                            0,
                                            ~0,
                                            0);
  for (D_UINT64 index = 0; (index < _removedRows) && result; ++index)
    {
      DBSUInt64 element (true);
      values.GetElement (element, index);

      if (element.IsNull() || (element.m_Value != index))
        result = false;

      DBSUInt64 rowValue(true, 31);
      table.GetEntry (rowValue, index, 0);

      if (rowValue.IsNull() == false)
        result = false;
    }

  values  = table.GetMatchingRows (nullValue,
                                   DBSUInt64 (false, ~0),
                                   0,
                                   ~0,
                                   _removedRows,
                                   ~0,
                                    0);

  for (D_UINT64 index = _removedRows; (index < _rowsCount) && result; ++index)
    {
      DBSUInt64 element (true);
      values.GetElement (element, index - _removedRows);

      DBSUInt64 rowValue(true, 31);
      table.GetEntry (rowValue, element.m_Value, 0);

      if (rowValue.IsNull() == true)
        result = false;

      DBSUInt64 generatedValue (true);
      tableValues.GetElement (generatedValue, element.m_Value);
      if ((rowValue == generatedValue) == false)
        result = false;
    }

  dbsHnd.ReleaseTable (table);

  std::cout << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

void
callback_index_create (CallBackIndexData *const pData)
{
  std::cout << '\r' << pData->m_RowIndex << '(' << pData->m_RowsCount << ')';
}

bool
test_index_creation (I_DBSHandler& dbsHnd, DBSArray& tableValues)
{
  CallBackIndexData data;
  bool result = true;
  std::cout << "Test index creation ... " << std::endl;

  I_DBSTable& table = dbsHnd.RetrievePersistentTable (tb_name);

  table.RemoveFieldIndex (0);

  for (D_UINT64 index = 0; index < _removedRows; ++index)
    {
      DBSUInt64 rowValue (true);
      tableValues.GetElement (rowValue, index);

      table.SetEntry (rowValue, index, 0);
    }


  table.CreateFieldIndex (0, callback_index_create, &data);

  DBSArray values  = table.GetMatchingRows (DBSUInt64 (true),
                                            DBSUInt64 (false, ~0),
                                            0,
                                            ~0,
                                            0,
                                            ~0,
                                            0);

  if (values.GetElementsCount() != _rowsCount)
    result = false;

  std::cout << (result ? "OK" : "FAIL") << std::endl;

  std::cout << "Check index values ... " << std::endl;

  for (D_UINT64 index = 0; (index < _rowsCount) && result; ++index)
    {
      DBSUInt64 rowValue(true, 31);
      table.GetEntry (rowValue, index, 0);

      if (rowValue.IsNull() == true)
        result = false;

      DBSUInt64 generatedValue (true);
      tableValues.GetElement (generatedValue, index);
      if ((rowValue == generatedValue) == false)
        result = false;

      std::cout << '\r' << index << '(' << _rowsCount << ')';
    }

  dbsHnd.ReleaseTable (table);

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialized
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

  {
    DBSArray tableValues (_SC (DBSUInt64*, NULL));
    {
      I_DBSTable &table = handler.RetrievePersistentTable (tb_name);

      success = success && fill_table_with_values (table, _rowsCount, 0, tableValues);
      success = success && fill_table_with_first_nulls (table, _removedRows);
      handler.ReleaseTable (table);
      success = success && test_table_index_survival (handler, tableValues);
    }
      success = success && test_index_creation (handler, tableValues);

  }
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
