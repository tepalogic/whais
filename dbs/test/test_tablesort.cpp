/*
 * test_tablesort.cpp
 *
 *  Created on: Dec 28, 2013
 *      Author: ipopa
 */


#include <assert.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "utils/wrandom.h"
#include "utils/wthread.h"
#include "utils/wsort.h"
#include "custom/include/test/test_fmw.h"

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "dbs/dbs_values.h"

using namespace whais;


struct TestContext
{
  IDBSHandler*      dbs;
  ITable*           originalTable;
  FIELD_INDEX       field;
  bool              reverseSort;
};


const char db_name[] = "t_baza_date_1";

struct DBSFieldDescriptor fieldsDescs[] =
{
    {"a", T_UINT32, false},
    {"c", T_CHAR, false},
    {"d", T_DATE, false},
    {"dt", T_DATETIME, false},
    {"ht", T_HIRESTIME, false},
    {"r", T_REAL, false},
    {"rr", T_RICHREAL, false},
    {"i8", T_INT8, false},
    {"i16", T_INT16, false},
    {"i32", T_INT32, false},
    {"i64", T_INT64, false},
    {"u8", T_UINT8, false},
    {"u16", T_UINT16, false},
    {"u32", T_UINT32, false},
    {"u64", T_UINT64, false},
    {"t", T_TEXT, false}
};

#define FIELDS_COUNT      (sizeof(fieldsDescs) / sizeof(fieldsDescs[0]))

uint_t _elemsCount = 10000;

FIELD_INDEX f_a;
FIELD_INDEX f_c;
FIELD_INDEX f_d;
FIELD_INDEX f_dt;
FIELD_INDEX f_ht;
FIELD_INDEX f_r;
FIELD_INDEX f_rr;
FIELD_INDEX f_i8;
FIELD_INDEX f_i16;
FIELD_INDEX f_i32;
FIELD_INDEX f_i64;
FIELD_INDEX f_u8;
FIELD_INDEX f_u16;
FIELD_INDEX f_u32;
FIELD_INDEX f_u64;
FIELD_INDEX f_t;



DDateTime
get_random_datetime()
{
  int16_t year  = wh_rnd() & 0xFFFF;
  uint8_t month = wh_rnd() % 12 + 1;
  uint8_t day   = wh_rnd() % 27 + 1;
  uint8_t hour  = wh_rnd() % 24;
  uint8_t mins  = wh_rnd() % 60;
  uint8_t secs  = wh_rnd() % 60;

  return DDateTime(year, month, day, hour, mins, secs);
}

DHiresTime
get_random_hirestime()
{
  int16_t year  = wh_rnd() & 0xFFFF;
  uint8_t month = wh_rnd() % 12 + 1;
  uint8_t day   = wh_rnd() % 27 + 1;
  uint8_t hour  = wh_rnd() % 24;
  uint8_t mins  = wh_rnd() % 60;
  uint8_t secs  = wh_rnd() % 60;
  uint32_t  mic = wh_rnd() % 1000000;

  return DHiresTime(year, month, day, hour, mins, secs, mic);
}

DDate
get_random_date()
{
  int16_t year  = wh_rnd() & 0xFFFF;
  uint8_t month = wh_rnd() % 12 + 1;
  uint8_t day   = wh_rnd() % 27 + 1;

  return DDate(year, month, day);
}

DText
get_random_text(uint_t count)
{
  DText result;

  while(count-- != 0)
    result.Append(DChar(wh_rnd() % 300 + 1));

  return result;
}


static ITable&
build_reference_table(IDBSHandler& dbs, const uint_t rowsCount)
{
  ITable& result = dbs.CreateTempTable(FIELDS_COUNT, fieldsDescs);

  std::cout << "Building a " << rowsCount << " rows table ... ";

  f_a   = result.RetrieveField("a");
  f_c   = result.RetrieveField("c");
  f_d   = result.RetrieveField("d");
  f_dt  = result.RetrieveField("dt");
  f_ht  = result.RetrieveField("ht");
  f_r   = result.RetrieveField("r");
  f_rr  = result.RetrieveField("rr");
  f_i8  = result.RetrieveField("i8");
  f_i16 = result.RetrieveField("i16");
  f_i32 = result.RetrieveField("i32");
  f_i64 = result.RetrieveField("i64");
  f_u8  = result.RetrieveField("u8");
  f_u16 = result.RetrieveField("u16");
  f_u32 = result.RetrieveField("u32");
  f_u64 = result.RetrieveField("u64");
  f_t   = result.RetrieveField("t");

  for (uint_t row = 0; row < rowsCount; ++row)
    {
      const DChar c(wh_rnd() % 0x300 + 1);
      const DDate d(get_random_date());
      const DDateTime dt(get_random_datetime());
      const DHiresTime ht(get_random_hirestime());

      const DReal r(DBS_REAL_T(wh_rnd() % 100000,
                                 wh_rnd() % 100000,
                                 DBS_REAL_PREC));
      const DRichReal rr(DBS_RICHREAL_T(wh_rnd() % 10000000,
                                          wh_rnd() % 10000000,
                                          DBS_RICHREAL_PREC));
      const DInt8  i8(wh_rnd());
      const DInt16 i16(wh_rnd());
      const DInt32 i32(wh_rnd());
      const DInt64 i64(wh_rnd());

      const DUInt8  u8(wh_rnd());
      const DUInt16 u16(wh_rnd());
      const DUInt32 u32(wh_rnd());
      const DUInt64 u64(wh_rnd());

      const DText t(get_random_text(wh_rnd() % 32));

      result.Set(row, f_a, DUInt32(row));
      result.Set(row, f_c, (row % (wh_rnd() % 4 + 1)) ? DChar() : c);

      result.Set(row, f_d, (row % (wh_rnd() % 4 + 1)) ? DDate() : d);
      result.Set(row, f_dt, (row % (wh_rnd() % 4 + 1)) ? DDateTime() : dt);
      result.Set(row, f_ht, (row % (wh_rnd() % 4 + 1)) ? DHiresTime() : ht);

      result.Set(row, f_i8, (row % (wh_rnd() % 4 + 1)) ? DInt8() : i8);
      result.Set(row, f_i16, (row % (wh_rnd() % 4 + 1)) ? DInt16() : i16);
      result.Set(row, f_i32, (row % (wh_rnd() % 4 + 1)) ? DInt32() : i32);
      result.Set(row, f_i64, (row % (wh_rnd() % 4 + 1)) ? DInt64() : i64);

      result.Set(row, f_r, (row % (wh_rnd() % 4 + 1)) ? DReal() : r);
      result.Set(row, f_rr, (row % (wh_rnd() % 4 + 1)) ? DRichReal() : rr);

      result.Set(row, f_u8, (row % (wh_rnd() % 4 + 1)) ? DUInt8() : u8);
      result.Set(row, f_u16, (row % (wh_rnd() % 4 + 1)) ? DUInt16() : u16);
      result.Set(row, f_u32, (row % (wh_rnd() % 4 + 1)) ? DUInt32() : u32);
      result.Set(row, f_u64, (row % (wh_rnd() % 4 + 1)) ? DUInt64() : u64);

      result.Set(row, f_t, t);
    }

  std::cout << "DONE\n";

  return result;
}

static ITable&
copy_table(IDBSHandler& dbs, ITable& originalTable)
{
  const uint_t rowsCount = originalTable.AllocatedRows();

  ITable& result = dbs.CreateTempTable(FIELDS_COUNT, fieldsDescs);

  for (uint_t row = 0; row < rowsCount; ++row)
    {
      DChar      c;
      DDate      d;
      DDateTime  dt;
      DHiresTime ht;

      DReal      r;
      DRichReal  rr;

      DInt8      i8;
      DInt16     i16;
      DInt32     i32;
      DInt64     i64;

      DUInt8     u8;
      DUInt16    u16;
      DUInt32    u32;
      DUInt64    u64;

      DText      t;

      result.Set(row, f_a, DUInt32(row));

      originalTable.Get(row, f_c, c);
      result.Set(row, f_c, c);

      originalTable.Get(row, f_d, d);
      result.Set(row, f_d, d);

      originalTable.Get(row, f_dt, dt);
      result.Set(row, f_dt, dt);

      originalTable.Get(row, f_ht, ht);
      result.Set(row, f_ht, ht);

      originalTable.Get(row, f_i8,  i8);
      result.Set(row, f_i8,  i8);

      originalTable.Get(row, f_i16, i16);
      result.Set(row, f_i16, i16);

      originalTable.Get(row, f_i32, i32);
      result.Set(row, f_i32, i32);

      originalTable.Get(row, f_i64, i64);
      result.Set(row, f_i64, i64);

      originalTable.Get(row, f_r, r);
      result.Set(row, f_r, r);

      originalTable.Get(row, f_rr, rr);
      result.Set(row, f_rr, rr);

      originalTable.Get(row, f_u8, u8);
      result.Set(row, f_u8, u8);

      originalTable.Get(row, f_u16, u16);
      result.Set(row, f_u16, u16);

      originalTable.Get(row, f_u32, u32);
      result.Set(row, f_u32, u32);

      originalTable.Get(row, f_u64, u64);
      result.Set(row, f_u64, u64);

      originalTable.Get(row, f_t, t);
      result.Set(row, f_t, t);
    }

  return result;
}

template<typename T> bool
test_table_sort(ITable&           tableSorted,
                 ITable&           tableOriginal,
                 const FIELD_INDEX field,
                 const bool        reverseSort)
{
  const uint64_t rowsCount = tableSorted.AllocatedRows();

  if (rowsCount != tableOriginal.AllocatedRows())
    return false;

  T lastEtry;
  if (reverseSort)
    {
      for (int64_t row = rowsCount - 1; row >= 0; --row)
        {
          DUInt32 rowValue;
          T value, orgValue;

          tableSorted.Get(row, f_a, rowValue);
          tableSorted.Get(row, field, value);

          if (value < lastEtry)
            return false;

          else
            lastEtry = value;

          if (rowValue.IsNull())
            return false;

          tableOriginal.Get(rowValue.mValue, field, orgValue);
          if (value != orgValue)
            return false;
        }
    }
  else
    {
      for (uint64_t row = 0; row < rowsCount; ++row)
        {
          DUInt32 rowValue;
          T value, orgValue;

          tableSorted.Get(row, f_a, rowValue);
          tableSorted.Get(row, field, value);

          if (value < lastEtry)
            return false;

          else
            lastEtry = value;

          if (rowValue.IsNull())
            return false;

          tableOriginal.Get(rowValue.mValue, field, orgValue);
          if (value != orgValue)
            return false;
        }
    }

  return true;
}


static bool
check_field_sort(IDBSHandler&      dbs,
                  ITable&           originalTable,
                  const FIELD_INDEX field,
                  const bool        reverseSort)
{
  const DBSFieldDescriptor fd = originalTable.DescribeField(field);

  bool result = true;

  if (fd.isArray)
    return false;

  ITable& table = copy_table(dbs, originalTable);
  if (reverseSort)
    std::cout << "Starting the sort of field '" << fd.name << "' (reverse).\n";

  else
    std::cout << "Starting the sort of field '" << fd.name << "'.\n";

  table.Sort(field, 0, table.AllocatedRows() - 1, reverseSort);

  if (reverseSort)
    std::cout << "Ending the sort of field '" << fd.name << "' (reverse).\n";

  else
    std::cout << "Ending the sort of field '" << fd.name << "'.\n";


  switch(fd.type)
    {
    case T_CHAR:
      result = test_table_sort<DChar> (table,
                                       originalTable,
                                       field,
                                       reverseSort);
      break;

    case T_DATE:
      result = test_table_sort<DDate> (table,
                                       originalTable,
                                       field,
                                       reverseSort);
      break;

    case T_DATETIME:
      result = test_table_sort<DDateTime> (table,
                                           originalTable,
                                           field,
                                           reverseSort);
      break;

    case T_HIRESTIME:
      result = test_table_sort<DHiresTime> (table,
                                            originalTable,
                                            field,
                                            reverseSort);
      break;

    case T_INT8:
      result = test_table_sort<DInt8> (table,
                                       originalTable,
                                       field,
                                       reverseSort);
      break;

    case T_INT16:
      result = test_table_sort<DInt16> (table,
                                        originalTable,
                                        field,
                                        reverseSort);
      break;

    case T_INT32:
      result = test_table_sort<DInt32> (table,
                                        originalTable,
                                        field,
                                        reverseSort);
      break;

    case T_INT64:
      result = test_table_sort<DInt64> (table,
                                        originalTable,
                                        field,
                                        reverseSort);
      break;

    case T_REAL:
      result = test_table_sort<DReal> (table,
                                       originalTable,
                                       field,
                                       reverseSort);
      break;

    case T_RICHREAL:
      result = test_table_sort<DRichReal> (table,
                                           originalTable,
                                           field,
                                           reverseSort);
      break;

    case T_UINT8:
      result = test_table_sort<DUInt8> (table,
                                        originalTable,
                                        field,
                                        reverseSort);
      break;

    case T_UINT16:
      result = test_table_sort<DUInt16> (table,
                                         originalTable,
                                         field,
                                         reverseSort);
      break;

    case T_UINT32:
      result = test_table_sort<DUInt32> (table,
                                         originalTable,
                                         field,
                                         reverseSort);
      break;

    case T_UINT64:
      result = test_table_sort<DUInt64> (table,
                                         originalTable,
                                         field,
                                         reverseSort);
      break;

    case T_TEXT:
      result = test_table_sort<DText> (table,
                                       originalTable,
                                       field,
                                       reverseSort);
      break;

    default:
      result = false;
    }

  dbs.ReleaseTable(table);

  return result;
}


void
test_field_sort(void* ctxt)
{
  const TestContext* testCtxt = _SC(TestContext*, ctxt);

  if (! check_field_sort(*testCtxt->dbs,
                          *testCtxt->originalTable,
                          testCtxt->field,
                          testCtxt->reverseSort))
    {
      throw 1; //Just an exception to signal the error.
    }
}


int
main(int argc, char** argv)
{
  bool success = true;

  test_set_mem_max(1024 * 1024 * 1024);

  if (argc > 1)
    _elemsCount = atol(argv[1]);

  {
    DBSInit(DBSSettings());
    DBSCreateDatabase(db_name);
  }

  IDBSHandler& handler = DBSRetrieveDatabase(db_name);

  {
    ITable& originalTable = build_reference_table(handler, _elemsCount);

    TestContext tc[30] = { {&handler, &originalTable, f_c, false}, };

#if 0
    tc[0] = tc[0], tc[0].field = f_i8, tc[0].reverseSort = false;
    test_field_sort(tc + 0);
#endif

    Thread th[30];

    tc[0] = tc[0], tc[0].field = f_c, tc[0].reverseSort = false;
    th[0].Run(test_field_sort, tc + 0);

    tc[1] = tc[0], tc[1].field = f_c, tc[1].reverseSort = true;
    th[1].Run(test_field_sort, tc + 1);

    tc[2] = tc[0], tc[2].field = f_d, tc[2].reverseSort = false;
    th[2].Run(test_field_sort, tc + 2);

    tc[3] = tc[0], tc[3].field = f_d, tc[3].reverseSort = true;
    th[3].Run(test_field_sort, tc + 3);

    tc[4] = tc[0], tc[4].field = f_dt, tc[4].reverseSort = false;
    th[4].Run(test_field_sort, tc + 4);

    tc[5] = tc[0], tc[5].field = f_dt, tc[5].reverseSort = true;
    th[5].Run(test_field_sort, tc + 5);

    tc[6] = tc[0], tc[6].field = f_ht, tc[6].reverseSort = false;
    th[6].Run(test_field_sort, tc + 6);

    tc[7] = tc[0], tc[7].field = f_ht, tc[7].reverseSort = true;
    th[7].Run(test_field_sort, tc + 7);

    tc[8] = tc[0], tc[8].field = f_i8, tc[8].reverseSort = false;
    th[8].Run(test_field_sort, tc + 8);

    tc[9] = tc[0], tc[9].field = f_i8, tc[9].reverseSort = true;
    th[9].Run(test_field_sort, tc + 9);

    tc[10] = tc[0], tc[10].field = f_i16, tc[10].reverseSort = false;
    th[10].Run(test_field_sort, tc + 10);

    tc[11] = tc[0], tc[11].field = f_i16, tc[11].reverseSort = true;
    th[11].Run(test_field_sort, tc + 11);

    tc[12] = tc[0], tc[12].field = f_i32, tc[12].reverseSort = false;
    th[12].Run(test_field_sort, tc + 12);

    tc[13] = tc[0], tc[13].field = f_i32, tc[13].reverseSort = true;
    th[13].Run(test_field_sort, tc + 13);

    tc[14] = tc[0], tc[14].field = f_i64, tc[14].reverseSort = false;
    th[14].Run(test_field_sort, tc + 14);

    tc[15] = tc[0], tc[15].field = f_i64, tc[15].reverseSort = true;
    th[15].Run(test_field_sort, tc + 15);

    tc[16] = tc[0], tc[16].field = f_r, tc[16].reverseSort = false;
    th[16].Run(test_field_sort, tc + 16);

    tc[17] = tc[0], tc[17].field = f_r, tc[17].reverseSort = true;
    th[17].Run(test_field_sort, tc + 17);

    tc[18] = tc[0], tc[18].field = f_rr, tc[18].reverseSort = false;
    th[18].Run(test_field_sort, tc + 18);

    tc[19] = tc[0], tc[19].field = f_rr, tc[19].reverseSort = true;
    th[19].Run(test_field_sort, tc + 19);

    tc[20] = tc[0], tc[20].field = f_u8, tc[20].reverseSort = false;
    th[20].Run(test_field_sort, tc + 20);

    tc[21] = tc[0], tc[21].field = f_u8, tc[21].reverseSort = true;
    th[21].Run(test_field_sort, tc + 21);

    tc[22] = tc[0], tc[22].field = f_u16, tc[22].reverseSort = false;
    th[22].Run(test_field_sort, tc + 22);

    tc[23] = tc[0], tc[23].field = f_u16, tc[23].reverseSort = true;
    th[23].Run(test_field_sort, tc + 23);

    tc[24] = tc[0], tc[24].field = f_u32, tc[24].reverseSort = false;
    th[24].Run(test_field_sort, tc + 24);

    tc[25] = tc[0], tc[25].field = f_u32, tc[25].reverseSort = true;
    th[25].Run(test_field_sort, tc + 25);

    tc[26] = tc[0], tc[26].field = f_u64, tc[26].reverseSort = false;
    th[26].Run(test_field_sort, tc + 26);

    tc[27] = tc[0], tc[27].field = f_u64, tc[27].reverseSort = true;
    th[27].Run(test_field_sort, tc + 27);

    tc[28] = tc[0], tc[28].field = f_u64, tc[28].reverseSort = false;
    th[28].Run(test_field_sort, tc + 28);

    tc[29] = tc[0], tc[29].field = f_u64, tc[29].reverseSort = true;
    th[29].Run(test_field_sort, tc + 29);


    th[0].WaitToEnd(true);
    th[1].WaitToEnd(true);
    th[2].WaitToEnd(true);
    th[3].WaitToEnd(true);
    th[4].WaitToEnd(true);
    th[5].WaitToEnd(true);
    th[6].WaitToEnd(true);
    th[7].WaitToEnd(true);
    th[8].WaitToEnd(true);
    th[9].WaitToEnd(true);
    th[10].WaitToEnd(true);
    th[11].WaitToEnd(true);
    th[12].WaitToEnd(true);
    th[13].WaitToEnd(true);
    th[14].WaitToEnd(true);
    th[15].WaitToEnd(true);
    th[16].WaitToEnd(true);
    th[17].WaitToEnd(true);
    th[18].WaitToEnd(true);
    th[19].WaitToEnd(true);
    th[20].WaitToEnd(true);
    th[21].WaitToEnd(true);
    th[22].WaitToEnd(true);
    th[23].WaitToEnd(true);
    th[24].WaitToEnd(true);
    th[25].WaitToEnd(true);
    th[26].WaitToEnd(true);
    th[27].WaitToEnd(true);
    th[28].WaitToEnd(true);
    th[29].WaitToEnd(true);

    handler.ReleaseTable(originalTable);
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
uint32_t WMemoryTracker::smInitCount = 1;
const char* WMemoryTracker::smModule = "T";
#endif

