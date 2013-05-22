/*
 * test_temptableindex.cpp
 *
 *  Created on: Jan 11, 2012
 *      Author: ipopa
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

#include "utils/wrandom.h"
#include "custom/include/test/test_fmw.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"

#include "../pastra/ps_table.h"

struct DBSFieldDescriptor field_desc[] = {
    {"test_field", T_UINT64, false}
};

using namespace whisper;
using namespace pastra;

const char db_name[] = "t_baza_date_1";

static uint_t _rowsCount   = 5000000;
static uint_t _removedRows = _rowsCount / 10;


bool
fill_table_with_values (ITable& table,
                        const uint32_t rowCount,
                        uint64_t seed,
                        DArray& tableValues)
{
  bool     result = true;

  table.CreateIndex (0, NULL, NULL);
  std::cout << "Filling table with values ... " << std::endl;

  wh_rnd_set_seed (seed);
  for (uint_t index = 0; index < rowCount; ++index)
    {
      DUInt64 value (wh_rnd ());
      if (table.AddRow () != index)
        {
          result = false;
          break;
        }

      std::cout << "\r" << index << "(" << rowCount << ")";
      table.Set (index, 0, value);
      tableValues.Add (value);

    }

  std::cout << std::endl << "Check table with values ... " << std::endl;
  DArray values = table.MatchRows (DUInt64 (),
                                           DUInt64 (~0),
                                           0,
                                           ~0,
                                           0,
                                           ~0,
                                           0);
  if ((values.Count() != tableValues.Count ()) ||
      (values.Count () != rowCount))
    {
      result = false;
    }

  for (uint_t checkIndex = 0; (checkIndex < rowCount) && result; ++checkIndex)
    {
      DUInt64  rowValue;
      DUInt64 rowIndex;

      values.Get (checkIndex, rowIndex);
      assert (rowIndex.IsNull() == false);

      table.Get (rowIndex.mValue, 0, rowValue);

      DUInt64 generated;
      tableValues.Get (rowIndex.mValue, generated);
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
fill_table_with_first_nulls (ITable& table, const uint32_t rowCount)
{
  bool result = true;
  std::cout << "Set NULL values for the first " << rowCount << " rows!" << std::endl;

  DUInt64 nullValue;

  for (uint64_t index = 0; index < rowCount; ++index)
    {
      table.Set (index, 0, nullValue);

      std::cout << "\r" << index << "(" << rowCount << ")";
    }

  DArray values = table.MatchRows (nullValue,
                                           nullValue,
                                           0,
                                           ~0,
                                           0,
                                           ~0,
                                           0);

  for (uint64_t index = 0; (index < rowCount) && result; ++index)
    {
      DUInt64 element;
      values.Get (index, element);

      if (element.IsNull() || (element.mValue != index))
        result = false;

      DUInt64 rowValue;
      table.Get (index, 0, rowValue);

      if (rowValue.IsNull() == false)
        result = false;
    }

  if (values.Count() != rowCount)
    result = false;

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;

  return result;
}

void
callback_index_create (CreateIndexCallbackContext* const pData)
{
  std::cout << '\r' << pData->mRowIndex << '(' << pData->mRowsCount << ')';
}

bool
test_index_creation (ITable& table, I_DBSHandler& dbsHnd, DArray& tableValues)
{
  CreateIndexCallbackContext data;
  bool result = true;
  std::cout << "Test index creation ... " << std::endl;

  table.RemoveIndex (0);

  for (uint64_t index = 0; index < _removedRows; ++index)
    {
      DUInt64 rowValue;
      tableValues.Get (index, rowValue);

      table.Set (index, 0, rowValue);
    }


  table.CreateIndex (0, callback_index_create, &data);

  DArray values  = table.MatchRows (DUInt64 (),
                                            DUInt64 (~0),
                                            0,
                                            ~0,
                                            0,
                                            ~0,
                                            0);

  if (values.Count() != _rowsCount)
    result = false;

  std::cout << (result ? "OK" : "FAIL") << std::endl;

  std::cout << "Check index values ... " << std::endl;

  for (uint64_t index = 0; (index < _rowsCount) && result; ++index)
    {
      DUInt64 rowValue;
      table.Get (index, 0, rowValue);

      if (rowValue.IsNull() == true)
        result = false;

      DUInt64 generatedValue;
      tableValues.Get (index, generatedValue);
      if ((rowValue == generatedValue) == false)
        result = false;

      std::cout << '\r' << index << '(' << _rowsCount << ')';
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

int
main (int argc, char **argv)
{
  if (argc > 1)
    {
      _rowsCount = atol (argv[1]);
    }
  _removedRows = _rowsCount / 10;

  bool success = true;
  {
    DBSInit (DBSSettings ());
    DBSCreateDatabase (db_name);
  }

  I_DBSHandler& handler = DBSRetrieveDatabase (db_name);

  {
    DArray    tableValues (_SC (DUInt64*, NULL));
    ITable& table = handler.CreateTempTable (sizeof field_desc / sizeof (field_desc[0]),
                                                 field_desc);

    success = success && fill_table_with_values (table, _rowsCount, 0, tableValues);
    success = success && fill_table_with_first_nulls (table, _removedRows);
    success = success && test_index_creation (table, handler, tableValues);

    handler.ReleaseTable (table);
  }

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
