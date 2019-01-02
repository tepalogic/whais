/*
 * test_field_filter_description.cpp
 *
 *  Created on: Dec 28, 2018
 *      Author: ipopa
 */


#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <tuple>

#include "utils/wrandom.h"
#include "utils/wthread.h"
#include "custom/include/test/test_fmw.h"
#include "extutils/table_filter.h"
#include "extutils/arrays_ops.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"

using namespace whais;
using namespace std;

const char db_name[] = "t_baza_date_1";
const char tb_name[] = "t_test_tab";

const uint32_t tableRowsCount = 1024;

TableFieldValuesFilter  filter;


DArray uint64_null, uint64_2s, uint64_3s, uint64_4s;
DArray uint32_null, uint32_8s, uint32_9s, uint32_10s;
DArray dates_null, dates_00s, dates_01s, dates_02s;
DArray rich_null;

template<typename T>
struct TestValue
{
  T value;
  bool included;
};

vector<TestValue<ROW_INDEX>> g_RowsTests = {
    {0, true},
    {1, true},
    {2, false},
    {6, false},
    {7, true},
    {8, true},
    {10, true},
    {14, true},
    {19, true},
    {20, true},
    {21, false},
    {27, false},
    {29, false},
    {30, true},
    {31, true},
    {34, true},
    {35, true},
    {36, false},
    {39, false},
    {40, true},
    {41, true},
    {42, false},
    {47, false},
    {49, false},
    {50, true},
    {55, true},
    {56, true},
    {69, true},
    {71, true},
    {80, true},
    {81, true},
    {82, true},
    {83, true},
    {84, false},
    {107, false}
};

vector<TestValue<ROW_INDEX>> g_ExcludedRowsTests = {
    {0, false},
    {2, false},
    {3, true},
    {4, true},
    {9, true},
    {10, true},
    {15, true},
    {27, true},
    {39, true},
    {40, true},
    {41, true},
    {43, true},
    {50, true},
    {51, true},
    {52, false},
    {61, false},
    {80, false},
    {81, true},
    {82, true},
    {83, true},
    {84, false},
    {99, false},
    {100, true},
    {101, true},
    {102, true},
    {103, true},
    {104, true},
    {105, false},
    {110, false},
    {111, true},
    {117, true},
    {130, true},
    {131, true},
    {132, false},
    {133, false},
    {140, false}
};

template<typename T>
bool check_test_values(const vector<tuple<T, T>>& intervals, const vector<TestValue<T>>& values)
{
  for (const auto& testValue : values)
  {
    bool found = false;
    for (const auto& interval : intervals)
    {
      if ((get<0>(interval) <= testValue.value) && (testValue.value <= get<1>(interval)))
      {
        found = true;
        break;
      }
    }
    if (found != testValue.included)
      return false;
  }

  return true;
}


bool
operator== (const DArray& a1, const DArray& a2)
{
  const auto count1 = a1.Count();
  const auto count2 = a2.Count();

  if (count1 != count2)
    return false;

  DArray a1Sorted = a1;
  DArray a2Sorted = a2;

  a1Sorted.Sort();
  a2Sorted.Sort();
  for (uint64_t i = 0; i < count1; ++i)
  {
    DUInt32 row1, row2;
    a1Sorted.Get(i, row1);
    a2Sorted.Get(i, row2);

    if (row1 != row2)
      return false;
  }

  return true;
}

bool
operator!= (const DArray& a1, const DArray& a2)
{
  return ! (a1 == a2);
}

bool
test_prepare_filter_row()
{
  cout << "Prepare filter rows ..." ;

  if (filter.GetRowsIntervals(false).size() != 0)
  {
    cout << "FAIL\n";
    return false;
  }

  filter.AddRow(10, 20, false);
  filter.AddRow(30, 30, false);
  filter.AddRow(0, 1, false);
  filter.AddRow(40, 41, false);
  filter.AddRow(60, 63, false);
  filter.AddRow(50, 55, false);
  filter.AddRow(80, 83, false);

  filter.AddRow(7, 13);
  filter.AddRow(53, 81);
  filter.AddRow(31, 35);

  if (filter.GetRowsIntervals(true).size() != 0)
  {
    cout << "FAIL\n";
    return false;
  }

  filter.AddRow(3, 9, true);
  filter.AddRow(40, 51, true);
  filter.AddRow(82, 82, true);
  filter.AddRow(102, 104, true);
  filter.AddRow(111, 131, true);
  filter.AddRow(10, 39, true);
  filter.AddRow(83, 83, true);
  filter.AddRow(81, 81, true);
  filter.AddRow(100, 102, true);

  if ( ! check_test_values(filter.GetRowsIntervals(), g_RowsTests))
  {
    cout << "FAIL\n";
    return false;
  }

  if ( ! check_test_values(filter.GetRowsIntervals(true), g_ExcludedRowsTests))
  {
    cout << "FAIL\n";
    return false;
  }

  filter.AddRow(101, 102, true);
  if ( ! check_test_values(filter.GetRowsIntervals(true), g_ExcludedRowsTests))
  {
    cout << "FAIL\n";
    return false;
  }

  filter.AddRow(82, 82, true);
  if ( ! check_test_values(filter.GetRowsIntervals(true), g_ExcludedRowsTests))
  {
    cout << "FAIL\n";
    return false;
  }

  filter.AddRow(81, 83, true);
  if ( ! check_test_values(filter.GetRowsIntervals(true), g_ExcludedRowsTests))
  {
    cout << "FAIL\n";
    return false;
  }

  cout << "OK\n";
  return true;
}



ITable&
build_test_table(IDBSHandler& dbs)
{
  DBSFieldDescriptor inoutFields[] ={
      {"fuint32", T_UINT32, false},
      {"frreal", T_RICHREAL, false},
      {"fhiresdate", T_HIRESTIME, false},
      {"fint64", T_INT64, false}
  };

  ITable& result = dbs.CreateTempTable(sizeof (inoutFields) / sizeof (inoutFields[0]), inoutFields);

  const FIELD_INDEX fuint32 = result.RetrieveField("fuint32");
  const FIELD_INDEX frreal = result.RetrieveField("frreal");
  const FIELD_INDEX fhiresdate = result.RetrieveField("fhiresdate");
  const FIELD_INDEX fint64 = result.RetrieveField("fint64");

  for (ROW_INDEX row = 0; row < tableRowsCount; ++row)
  {
    result.Set(row, fint64, row % 10 == 0 ? DInt64() : DInt64(row % 13));
    result.Set(row, frreal, row % 6 == 0 ? DRichReal() : DRichReal(1.0 + row / 10.0));
    result.Set(row, fuint32, row % 12 == 0 ? DUInt32() : DUInt32(row % 13));
    result.Set(row,
               fhiresdate,
               row % 12 == 0 ? DHiresTime() : DHiresTime(row % 21, 1, 2, 3, 4, 5, 900 + row));
  }

  for (ROW_INDEX row = 0; row < tableRowsCount; ++row)
  {
    DInt64  v64;
    DUInt32 v32;
    DRichReal  vreal;
    DHiresTime vtime;

    result.Get(row, fint64, v64);
    result.Get(row, fuint32, v32);
    result.Get(row, frreal, vreal);
    result.Get(row, fhiresdate, vtime);

    if (v64.IsNull())
      uint64_null.Add(DUInt32(row));

    else if (v64.mValue == 2)
      uint64_2s.Add(DUInt32(row));

    else if (v64.mValue == 3)
      uint64_3s.Add(DUInt32(row));

    else if (v64.mValue == 4)
      uint64_4s.Add(DUInt32(row));

    if (v32.IsNull())
      uint32_null.Add(DUInt32(row));

    else if (v32.mValue == 8)
      uint32_8s.Add(DUInt32(row));

    else if (v32.mValue == 9)
      uint32_9s.Add(DUInt32(row));

    else if (v32.mValue == 10)
      uint32_10s.Add(DUInt32(row));

    if (vtime.IsNull())
      dates_null.Add(DUInt32(row));

    else if (vtime.mYear == 0)
      dates_00s.Add(DUInt32(row));

    else if (vtime.mYear == 1)
      dates_01s.Add(DUInt32(row));

    else if (vtime.mYear == 2)
      dates_02s.Add(DUInt32(row));

    if (vreal.IsNull())
      rich_null.Add(DUInt32(row));
  }

  return result;
}

bool
test_rows_selection(ITable& table)
{
  cout << "Testing rows selection ..." ;

  TableFilterRunner filterRunner(table);
  filterRunner.AddFilterRules(filter);

  DArray rows = filterRunner.Run();
  vector<TestValue<ROW_INDEX>> rowsTests;
  for (ROW_INDEX i = 0; i < tableRowsCount; ++i)
  {
    bool included = false;
    for (uint64_t j = 0; j < rows.Count(); ++j)
    {
      DUInt32 row;
      rows.Get(j, row);
      if (row.mValue == i)
      {
        included = true;
        break;
      }
    }
    rowsTests.push_back({i, included});
  }

  for (const auto& test : rowsTests)
  {
    if (test.included)
    {
      bool foundInIncludeRows = false;
      for (const auto& interval : filter.GetRowsIntervals())
      {
        if (get<0>(interval) <= test.value && test.value <= get<1>(interval))
        {
          foundInIncludeRows = true;
          break;
        }
      }

      if (!foundInIncludeRows)
      {
        cout << "FAIL\n";
        return false;
      }
    }
    else
    {
      bool foundInIncludeRows = false;
      for (const auto& interval : filter.GetRowsIntervals())
      {
        if (get<0>(interval) <= test.value && test.value <= get<1>(interval))
        {
          foundInIncludeRows = true;
          break;
        }
      }

      if (! foundInIncludeRows)
        continue;

      bool foundInExcludedRows = false;
      for (const auto& interval : filter.GetRowsIntervals(true))
      {
        if (get<0>(interval) <= test.value && test.value <= get<1>(interval))
        {
          foundInExcludedRows = true;
          break;
        }
      }

      if (!foundInExcludedRows)
      {
        cout << "FAIL\n";
        return false;
      }
    }
  }

  cout << "OK\n";
  return true;
}

bool
test_rows_filtering_no(ITable& table)
{
  cout << "Testing rows selection (no indexes) ..." ;

  TableFieldValuesFilter filterInUse = filter;

  filterInUse.AddValue("fuint32", T_UINT32, "", "", false);

  TableFilterRunner filterRunner(table);
  filterRunner.AddFilterRules(filterInUse);
  filterRunner.ResetRowsFilter();
  DArray result = filterRunner.Run();
  if (result != uint32_null)
  {
    cout << "FAIL\n";
    return false;
  }

  filterInUse = filter;
  filterInUse.AddValue("fint64", T_INT64, "2", "4", false);
  filterInUse.AddValue("fint64", T_INT64, "3", "3", true);
  filterInUse.AddValue("fint64", T_INT64, "", "", false);

  filterRunner.ResetFilterRules();
  filterRunner.AddFilterRules(filterInUse);
  filterRunner.ResetRowsFilter();

  result = filterRunner.Run();

  if (result !=
      (array_unite(uint64_null, array_unite(uint64_2s, uint64_4s))))
  {
    cout << "FAIL\n";
    return false;
  }

  filterInUse.AddValue("fuint32", T_UINT32, "", "", false);
  filterRunner.ResetFilterRules();
  filterRunner.AddFilterRules(filterInUse);
  filterRunner.ResetRowsFilter();
  result = filterRunner.Run();
  if (result !=
      array_intersect(uint32_null,(array_unite(uint64_null, array_unite(uint64_2s, uint64_4s)))))
  {
    cout << "FAIL\n";
    return false;
  }

  filterRunner.ResetFilterRules();
  filterRunner.ResetRowsFilter();
  filterRunner.AddFilterRules(filter);

  DArray filterRows = filterRunner.Run();

  filterInUse = filter;
  filterInUse.AddValue("fint64", T_INT64, "2", "4", false);
  filterInUse.AddValue("fint64", T_INT64, "3", "3", true);
  filterInUse.AddValue("fint64", T_INT64, "", "", false);

  filterRunner.ResetFilterRules();
  filterRunner.ResetRowsFilter();
  filterRunner.AddFilterRules(filterInUse);

  result = filterRunner.Run();
  if (result !=
      array_intersect(filterRows,  array_unite(uint64_null, array_unite(uint64_2s, uint64_4s))))
  {
    cout << "FAIL\n";
    return false;
  }

  cout << "OK\n";
  return true;
}


bool
test_rows_filtering_indexes(ITable& table)
{
  cout << "Testing rows selection (with indexes) ..." ;

  TableFieldValuesFilter filterInUse = filter;

  filterInUse.AddValue("fuint32", T_UINT32, "", "", false);
  filterInUse.AddValue("fint64", T_INT64, "2", "4", false);
  filterInUse.AddValue("fint64", T_INT64, "3", "3", true);
  filterInUse.AddValue("fint64", T_INT64, "", "", false);

  TableFilterRunner filterRunner(table);
  filterRunner.AddFilterRules(filterInUse);
  filterRunner.ResetRowsFilter();

  DArray reference = array_unite(uint64_2s, uint64_4s);
  reference = array_unite(uint64_null, reference);
  reference = array_intersect(uint32_null, reference);
  if (filterRunner.Run() != reference)
  {
    cout << "FAIL\n";
    return false;
  }

  table.CreateIndex(table.RetrieveField("fint64"), nullptr, nullptr);
  if (filterRunner.Run() != reference)
  {
    cout << "FAIL\n";
    return false;
  }

  table.CreateIndex(table.RetrieveField("fuint32"), nullptr, nullptr);
  if (filterRunner.Run() != reference)
  {
    cout << "FAIL\n";
    return false;
  }

  cout << "OK\n";
  return true;
}


int
main(int argc, char** argv)
{
  bool success = true;
  DBSInit(DBSSettings());
  DBSCreateDatabase(db_name);
  IDBSHandler& dbs = DBSRetrieveDatabase(db_name);
  ITable& testTable = build_test_table(dbs);

  success = success && test_prepare_filter_row();
  success = success && test_rows_selection(testTable);
  success = success && test_rows_filtering_no(testTable);
  success = success && test_rows_filtering_indexes(testTable);


  dbs.ReleaseTable(testTable);
  DBSReleaseDatabase(dbs);
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

