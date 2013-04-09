/*
 * test_datetimebtindex.cpp
 *
 *  Created on: Jan 17, 2012
 *      Author: ipopa
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <vector>

#include "utils/include/random.h"
#include "custom/include/test/test_fmw.h"

#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"

#include "../pastra/ps_table.h"

struct DBSFieldDescriptor field_desc[] = {
    {"test_field", T_DATETIME, false}
};

const char db_name[] = "t_baza_date_1";
const char tb_name[] = "t_test_tab";

uint_t _rowsCount   = 5000000;
uint_t _removedRows = _rowsCount / 10;

static DBSDateTime _max_date (0x7FFF, 12, 31, 23, 59, 59);

DBSDateTime
get_random_datetime ()
{
  int16_t year  = w_rnd () & 0xFFFF;
  uint8_t month = w_rnd () % 12 + 1;
  uint8_t day   = w_rnd () % 27 + 1;
  uint8_t hour  = w_rnd () % 24;
  uint8_t mins  = w_rnd () % 60;
  uint8_t secs  = w_rnd () % 60;

  return DBSDateTime (year, month, day, hour, mins, secs);
}


bool
fill_table_with_values (I_DBSTable& table,
                        const uint32_t rowCount,
                        uint64_t seed,
                        DBSArray& tableValues)
{
  bool     result = true;
  DBSDateTime prev;

  table.CreateFieldIndex (0, NULL, NULL);
  std::cout << "Filling table with values ... " << std::endl;

  w_rnd_set_seed (seed);
  for (uint_t index = 0; index < rowCount; ++index)
    {
      DBSDateTime value = get_random_datetime ();
      if (table.AddRow () != index)
        {
          result = false;
          break;
        }

      if (((index * 100) % rowCount) == 0)
        {
          std::cout << (index * 100) / rowCount << "%\r";
          std::cout.flush ();
        }

      table.SetEntry (index, 0, value);
      tableValues.AddElement (value);

    }

  std::cout << std::endl << "Check table with values ... " << std::endl;
  DBSArray values = table.GetMatchingRows (DBSDateTime (),
                                           _max_date,
                                           0,
                                           ~0,
                                           0,
                                           ~0,
                                           0);
  if ((values.ElementsCount() != tableValues.ElementsCount ()) ||
      (values.ElementsCount () != rowCount))
    {
      result = false;
    }

  for (uint_t checkIndex = 0; (checkIndex < rowCount) && result; ++checkIndex)
    {
      DBSDateTime  rowValue;
      DBSUInt64 rowIndex;

      values.GetElement (rowIndex, checkIndex);
      assert (rowIndex.IsNull() == false);

      table.GetEntry (rowIndex.m_Value, 0, rowValue);

      DBSDateTime generated;
      tableValues.GetElement (generated, rowIndex.m_Value);
      assert (generated.IsNull() == false);

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
          std::cout.flush ();
        }
    }

  std::cout << std::endl << (result ? "OK" : "FAIL") << std::endl;

  return result;
}

bool
fill_table_with_first_nulls (I_DBSTable& table, const uint32_t rowCount)
{
  bool result = true;
  std::cout << "Set NULL values for the first " << rowCount << " rows!" << std::endl;

  DBSDateTime nullValue;

  for (uint64_t index = 0; index < rowCount; ++index)
    {
      table.SetEntry (index, 0, nullValue);

      if (((index * 100) % rowCount) == 0)
        {
          std::cout << (index * 100) / rowCount << "%\r";
          std::cout.flush ();
        }
    }

  DBSArray values = table.GetMatchingRows (nullValue,
                                           nullValue,
                                           0,
                                           ~0,
                                           0,
                                           ~0,
                                           0);

  for (uint64_t index = 0; (index < rowCount) && result; ++index)
    {
      DBSUInt64 element;
      values.GetElement (element, index);

      if (element.IsNull() || (element.m_Value != index))
        result = false;

      DBSDateTime rowValue;
      table.GetEntry (index, 0, rowValue);

      if (rowValue.IsNull() == false)
        result = false;
    }

  if (values.ElementsCount() != rowCount)
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

  DBSDateTime nullValue;
  DBSArray values  = table.GetMatchingRows (nullValue,
                                            nullValue,
                                            0,
                                            ~0,
                                            0,
                                            ~0,
                                            0);
  for (uint64_t index = 0; (index < _removedRows) && result; ++index)
    {
      DBSUInt64 element;
      values.GetElement (element, index);

      if (element.IsNull() || (element.m_Value != index))
        result = false;

      DBSDateTime rowValue;
      table.GetEntry (index, 0, rowValue);

      if (rowValue.IsNull() == false)
        result = false;
    }

  values  = table.GetMatchingRows (nullValue,
                                   _max_date,
                                   0,
                                   ~0,
                                   _removedRows,
                                   ~0,
                                    0);

  for (uint64_t index = _removedRows; (index < _rowsCount) && result; ++index)
    {
      DBSUInt64 element;
      values.GetElement (element, index - _removedRows);

      DBSDateTime rowValue;
      table.GetEntry (element.m_Value, 0, rowValue);

      if (rowValue.IsNull() == true)
        result = false;

      DBSDateTime generatedValue;
      tableValues.GetElement (generatedValue, element.m_Value);
      if ((rowValue == generatedValue) == false)
        result = false;
    }

  dbsHnd.ReleaseTable (table);

  std::cout << (result ? "OK" : "FAIL") << std::endl;
  return result;
}

void
callback_index_create (CallBackIndexData* const pData)
{
  if (((pData->m_RowIndex * 100) % pData->m_RowsCount) == 0)
    {
      std::cout << (pData->m_RowIndex * 100) / pData->m_RowsCount << "%\r";
      std::cout.flush ();
    }
}

bool
test_index_creation (I_DBSHandler& dbsHnd, DBSArray& tableValues)
{
  CallBackIndexData data;
  bool result = true;
  std::cout << "Test index creation ... " << std::endl;

  I_DBSTable& table = dbsHnd.RetrievePersistentTable (tb_name);

  table.RemoveFieldIndex (0);

  for (uint64_t index = 0; index < _removedRows; ++index)
    {
      DBSDateTime rowValue;
      tableValues.GetElement (rowValue, index);

      table.SetEntry (index, 0, rowValue);
    }


  table.CreateFieldIndex (0, callback_index_create, &data);

  DBSArray values  = table.GetMatchingRows (DBSDateTime (),
                                            _max_date,
                                            0,
                                            ~0,
                                            0,
                                            ~0,
                                            0);

  if (values.ElementsCount() != _rowsCount)
    result = false;

  std::cout << (result ? "OK" : "FAIL") << std::endl;

  std::cout << "Check index values ... " << std::endl;

  for (uint64_t index = 0; (index < _rowsCount) && result; ++index)
    {
      DBSDateTime rowValue;
      table.GetEntry (index, 0, rowValue);

      if (rowValue.IsNull() == true)
        result = false;

      DBSDateTime generatedValue;
      tableValues.GetElement (generatedValue, index);
      if ((rowValue == generatedValue) == false)
        result = false;

      if (((index * 100) % _rowsCount) == 0)
        {
          std::cout << (index * 100) / _rowsCount << "%\r";
          std::cout.flush ();
        }
    }

  dbsHnd.ReleaseTable (table);

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
  handler.AddTable ("t_test_tab", sizeof field_desc / sizeof (field_desc[0]), field_desc);

  {
    DBSArray tableValues (_SC (DBSDateTime*, NULL));
    {
      I_DBSTable& table = handler.RetrievePersistentTable (tb_name);

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
