/*
 * test_richrealbtindex.cpp
 *
 *  Created on: Jan 17, 2012
 *      Author: ipopa
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <limits>


#include "utils/wrandom.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"

#include "../pastra/ps_table.h"

using namespace whais;
using namespace pastra;

struct DBSFieldDescriptor field_desc[] = {
    {"test_field", T_RICHREAL, false}
};

const char db_name[] = "t_baza_date_1";
const char tb_name[] = "t_test_tab";

uint_t _rowsCount   = 5000000;
uint_t _removedRows = _rowsCount / 10;

DRichReal
MaxValue()
{
  static const int64_t intPart  = ~0ull >> 1;
  static const int64_t fracPart = DBS_RICHREAL_PREC - 1;

  static DRichReal _sentinel(DBS_RICHREAL_T(intPart, fracPart, DBS_RICHREAL_PREC));

  return _sentinel;
}

DRichReal
get_random_real()
{
  int64_t intPart  = wh_rnd();
  int64_t fracPart = _SC(int32_t, wh_rnd() & 0xFFFFFFFF);

  if (((intPart < 0) && (fracPart > 0))
      || ((intPart > 0) && (fracPart < 0)))
    {
      fracPart = -fracPart;
    }

  return DRichReal(DBS_RICHREAL_T(intPart, fracPart, DBS_RICHREAL_PREC));
}



bool
fill_table_with_values(ITable& table,
                        const uint32_t rowCount,
                        uint64_t seed,
                        DArray& tableValues)
{
  bool result = true;
  DRichReal prev;

  table.CreateIndex(0, NULL, NULL);
  std::cout << "Filling table with " << rowCount << " rows.\n";

  wh_rnd_set_seed(seed);
  for (uint_t index = 0; index < rowCount; ++index)
    {
      DRichReal value = get_random_real();
      if (table.AddRow() != index)
        {
          result = false;
          break;
        }

      if (((index * 100) % rowCount) == 0)
        {
          std::cout << (index * 100) / rowCount << "%\r";
          std::cout.flush();
        }

      table.Set(index, 0, value);
      tableValues.Add(value);
    }

  std::cout << std::endl << "Check table values ... " << std::endl;
  DArray values = table.MatchRows(DRichReal(),
                                   MaxValue(),
                                   0,
                                   ~0,
                                   0);
  if ((values.Count() != tableValues.Count()) ||
      (values.Count() != rowCount))
    {
      result = false;
    }


  for (uint_t checkIndex = 0; (checkIndex < rowCount) && result; ++checkIndex)
    {
      DRichReal   rowValue;
      DROW_INDEX  rowIndex;

      values.Get(checkIndex, rowIndex);
      assert(rowIndex.IsNull() == false);

      table.Get(rowIndex.mValue, 0, rowValue);

      DRichReal generated;
      tableValues.Get(rowIndex.mValue, generated);
      assert(generated.IsNull() == false);

      if (((rowValue == generated) == false) ||
          (rowValue < prev))
        {
          result = false;
          break;
        }
      else
        prev = rowValue;

      if (((checkIndex * 100) % rowCount) == 0)
        {
          std::cout << (checkIndex * 100) / rowCount << "%\r";
          std::cout.flush();
        }
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;

  return result;
}

bool
fill_table_with_first_nulls(ITable& table, const uint32_t rowCount)
{
  bool result = true;
  std::cout << "Set NULL values for the first " << rowCount << " rows!" << std::endl;

  DRichReal nullValue;

  for (uint64_t index = 0; index < rowCount; ++index)
    {
      table.Set(index, 0, nullValue);

      if (((index * 100) % rowCount) == 0)
        {
          std::cout << (index * 100) / rowCount << "%\r";
          std::cout.flush();
        }
    }

  DArray values = table.MatchRows(nullValue,
                                   nullValue,
                                   0,
                                   ~0,
                                   0);

  for (uint64_t index = 0; (index < rowCount) && result; ++index)
    {
      DROW_INDEX element;
      values.Get(index, element);

      if (element.IsNull() || (element.mValue != index))
        result = false;

      DRichReal rowValue;
      table.Get(index, 0, rowValue);

      if (rowValue.IsNull() == false)
        result = false;
    }

  if (values.Count() != rowCount)
    result = false;

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;

  return result;
}

bool
test_table_index_survival(IDBSHandler& dbsHnd, DArray& tableValues)
{
  bool result = true;
  std::cout << "Test index survival ... ";

  ITable& table = dbsHnd.RetrievePersistentTable(tb_name);

  DRichReal  nullValue;
  DArray values  = table.MatchRows(nullValue,
                                    nullValue,
                                    0,
                                    ~0,
                                    0);
  for (uint64_t index = 0; (index < _removedRows) && result; ++index)
    {
      DROW_INDEX element;
      values.Get(index, element);

      if (element.IsNull() || (element.mValue != index))
        result = false;

      DRichReal rowValue;
      table.Get(index, 0, rowValue);

      if (rowValue.IsNull() == false)
        result = false;
    }

  values  = table.MatchRows(nullValue,
                             MaxValue(),
                             _removedRows,
                             ~0,
                              0);

  for (uint64_t index = _removedRows; (index < _rowsCount) && result; ++index)
    {
      DROW_INDEX element;
      values.Get(index - _removedRows, element);

      DRichReal rowValue;
      table.Get(element.mValue, 0, rowValue);

      if (rowValue.IsNull() == true)
        result = false;

      DRichReal generatedValue;
      tableValues.Get(element.mValue, generatedValue);
      if ((rowValue == generatedValue) == false)
        result = false;
    }

  dbsHnd.ReleaseTable(table);

  std::cout << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

void
callback_index_create(CreateIndexCallbackContext* const pData)
{
  if (((pData->mRowIndex * 100) % pData->mRowsCount) == 0)
    {
      std::cout << (pData->mRowIndex * 100) / pData->mRowsCount << "%\r";
      std::cout.flush();
    }
}

bool
test_index_creation(IDBSHandler& dbsHnd, DArray& tableValues)
{
  CreateIndexCallbackContext data;
  bool result = true;
  std::cout << "Test index creation ... " << std::endl;

  ITable& table = dbsHnd.RetrievePersistentTable(tb_name);

  table.RemoveIndex(0);

  for (uint64_t index = 0; index < _removedRows; ++index)
    {
      DRichReal rowValue;
      tableValues.Get(index, rowValue);

      table.Set(index, 0, rowValue);
    }


  table.CreateIndex(0, callback_index_create, &data);

  DArray values  = table.MatchRows(DRichReal(),
                                    MaxValue(),
                                    0,
                                    ~0,
                                    0);

  if (values.Count() != _rowsCount)
    result = false;

  std::cout << (result ? "OK" : "FAIL") << std::endl;

  std::cout << "Check index values ... " << std::endl;

  for (uint64_t index = 0; (index < _rowsCount) && result; ++index)
    {
      DRichReal rowValue;
      table.Get(index, 0, rowValue);

      if (rowValue.IsNull() == true)
        result = false;

      DRichReal generatedValue;
      tableValues.Get(index, generatedValue);
      if ((rowValue == generatedValue) == false)
        result = false;

      if (((index * 100) % _rowsCount) == 0)
        {
          std::cout << (index * 100) / _rowsCount << "%\r";
          std::cout.flush();
        }
    }

  dbsHnd.ReleaseTable(table);

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

int
main(int argc, char **argv)
{
  if (argc > 1)
    {
      _rowsCount = atol(argv[1]);
    }
  _removedRows = _rowsCount / 10;

  bool success = true;
  {
    DBSInit(DBSSettings());
    DBSCreateDatabase(db_name);
  }

  IDBSHandler& handler = DBSRetrieveDatabase(db_name);
  handler.AddTable("t_test_tab", sizeof field_desc / sizeof(field_desc[0]), field_desc);

  {
    DArray tableValues(_SC(DRichReal*, NULL));
    {
      ITable& table = handler.RetrievePersistentTable(tb_name);

      success = success && fill_table_with_values(table, _rowsCount, 0, tableValues);
      success = success && fill_table_with_first_nulls(table, _removedRows);
      handler.ReleaseTable(table);
      success = success && test_table_index_survival(handler, tableValues);
    }
      success = success && test_index_creation(handler, tableValues);

  }
  DBSReleaseDatabase(handler);
  DBSRemoveDatabase(db_name);
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
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "T";
#endif
