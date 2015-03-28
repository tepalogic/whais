/*
 * test_dbsmgr.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"
#include "utils/wrandom.h"

#include "../pastra/ps_table.h"
#include "../pastra/ps_serializer.h"

using namespace whais;
using namespace pastra;
using namespace std;

const char db_name[] = "t_baza_date_1";

const char tb_name_1[] = "t_test_tab_1";
const char tb_name_2[] = "t_test_tab_2";
const char tb_name_3[] = "t_test_tab_3";
const char tb_name_4[] = "t_test_tab_4";

static uint_t _rowsCount   = 1000;

static const char text1[] = "This is a text!";
static const char text2[] = "This is should be bigger. It must have more chars then the first one.";
static const char text3[] = "FirstName LastName";
static const char text4[] = "Total domination of the world is the goal, but why?\n"
                             "Someone will say because one can, but this is not the case.\n"
                             "I have no idea what I wrote here! But it will be nice if I will dominate the world some day.\n";

static const char text5[]  = "B";
static const char text6[]  = "BT";
static const char text7[]  = "\0xC8\0x9B345678901234";
static const char text8[]  = "\0xE8\0x9E\0x99456\0xF0\0x90\0x83\0x8412345";
static const char text9[]  = "\0xF0\0x90\0x83\0x84567890\0xC8\0x9B3456";
static const char text10[] = "\0xC8\0x9B34567890123\0xF0\0x90\0x83\0x84";
static const char text11[] = "1234567\0xE8\0x9E\0x9912345678901234567890\0xC8\0x9B"
                             "\0xC8\0x9B34567890123\0xC8\0x9B678901234567890123456789012345\0xE8\0x9E\0x99"
                             "\0xE8\0x9E\0x994567890\0xF0\0x90\0x83\0x84567890123456789012345678901234567\0xE8"
                             "\0x9E\0x9934567890123456789012345678901234567890123456\0xE8\0x9E"
                             "\0x9923456789012345678\0xE8\0x9E\0x9923456789012345678901234\0xF0\0x90\0x83\0x84"
                             "\0xF0\0x90\0x83\0x84567890123456789012\0xF0\0x90\0x83\0x84789012345678901234567\0xF0"
                             "\0x90\0x83\0x844567890123456789012345678901234567890123456\0xF0\0x90"
                             "\0x83\0x84345678901234567890123456789\0xE8\0x9E\0x993456789012345\0xF0\0x90\0x83"
                             "\0x8423456789012345678901234567\0xC8\0x9B0123456789012345678";




const char* text[] = {text1, text2, text3, text4, text5, text6, text7, text8, text9, text10, text11 };

struct DBSFieldDescriptor field_descs[] = {
    {"bool", T_BOOL, false},
    {"char", T_CHAR, false},
    {"uint8", T_UINT8, false},
    {"uint16", T_UINT16, false},
    {"uint32", T_UINT32, false},
    {"uint64", T_UINT64, false},
    {"int64", T_INT64, false},
    {"int32", T_INT32, false},
    {"int16", T_INT16, false},
    {"int8", T_INT8, false},
    {"date", T_DATE, false},
    {"datetime", T_DATETIME, false},
    {"hirestime", T_HIRESTIME, false},
    {"real", T_REAL, false},
    {"richreal", T_RICHREAL, false},


    {"text", T_TEXT, false},

    {"array_bool", T_BOOL, true},
    {"array_char", T_CHAR, true},
    {"array_uint8", T_UINT8, true},
    {"array_uint16", T_UINT16, true},
    {"array_uint32", T_UINT32, true},
    {"array_uint64", T_UINT64, true},
    {"array_int64", T_INT64, true},
    {"array_int32", T_INT32, true},
    {"array_int16", T_INT16, true},
    {"array_int8", T_INT8, true},
    {"array_date", T_DATE, true},
    {"array_datetime", T_DATETIME, true},
    {"array_hirestime", T_HIRESTIME, true},
    {"array_real", T_REAL, true},
    {"array_richreal", T_RICHREAL, true},

};

static DArray bool_rows;
static DArray char_rows;
static DArray date_rows;
static DArray datetime_rows;
static DArray hirestime_rows;
static DArray int8_rows;
static DArray int16_rows;
static DArray int32_rows;
static DArray int64_rows;
static DArray u_int8_rows;
static DArray u_int16_rows;
static DArray u_int32_rows;
static DArray u_int64_rows;
static DArray real_rows;
static DArray richreal_rows;

static vector<bool> bool_rows_n;
static vector<bool> char_rows_n;
static vector<bool> date_rows_n;
static vector<bool> datetime_rows_n;
static vector<bool> hirestime_rows_n;
static vector<bool> int8_rows_n;
static vector<bool> int16_rows_n;
static vector<bool> int32_rows_n;
static vector<bool> int64_rows_n;
static vector<bool> u_int8_rows_n;
static vector<bool> u_int16_rows_n;
static vector<bool> u_int32_rows_n;
static vector<bool> u_int64_rows_n;
static vector<bool> real_rows_n;
static vector<bool> richreal_rows_n;

static vector<uint8_t> a_bool_rows_n;
static vector<uint8_t> a_char_rows_n;
static vector<uint8_t> a_date_rows_n;
static vector<uint8_t> a_datetime_rows_n;
static vector<uint8_t> a_hirestime_rows_n;
static vector<uint8_t> a_int8_rows_n;
static vector<uint8_t> a_int16_rows_n;
static vector<uint8_t> a_int32_rows_n;
static vector<uint8_t> a_int64_rows_n;
static vector<uint8_t> a_u_int8_rows_n;
static vector<uint8_t> a_u_int16_rows_n;
static vector<uint8_t> a_u_int32_rows_n;
static vector<uint8_t> a_u_int64_rows_n;
static vector<uint8_t> a_real_rows_n;
static vector<uint8_t> a_richreal_rows_n;


static const uint_t FIXED_DESC_FIELDS_COUNT = 15;
static const uint_t VAR_DESC_FIELDS_COUNT   = 16;
static const uint_t TOTAL_DESC_FIELDS_COUNT = 31;

static const uint_t TABLE_BREAK_MARKER      = 0xAB;


template<typename T> bool
verify_int_marker (const T& value)
{
  for (uint_t i = 0; i < sizeof value; ++i)
    {
      if ((_RC (const uint8_t*, &value)[i] == TABLE_BREAK_MARKER)
          || (_RC (const uint8_t*, &value)[i] == 0xFF))
        {
          return true;
        }
    }

  return false;
}


template<typename T> bool
check_indexed_field (ITable&            table,
                     DArray&            reffArray,
                     vector<bool>&      reffNullsArray,
                     const FIELD_INDEX  field,
                     const ROW_INDEX    rowsCount,
                     const bool         broken)
{
  bool indexed = table.IsIndexed (field);

  DArray matchedRows = table.MatchRows (T::Min (),
                                        T::Max (),
                                        0,
                                        rowsCount,
                                        field);

  vector<bool> visitedRows (rowsCount, false);

  T prev, current;
  for (uint_t i = 0; i < matchedRows.Count(); ++i)
    {
      T           arrayValue;
      DROW_INDEX  row;

      matchedRows.Get (i, row);
      table.Get (row.mValue, field, current);
      reffArray.Get (row.mValue, arrayValue);

      if (visitedRows[row.mValue])
        return false;

      else
        visitedRows[row.mValue] = true;

      if (indexed && (current < prev))
        return false;

      else if (current != arrayValue)
        {
          if ( ! broken)
            return false;

          else if (! verify_int_marker (current))
            return false;
        }

      if (broken)
        reffNullsArray[row.mValue] = false;
    }

  matchedRows = table.MatchRows (T(), T(), 0, rowsCount, field);
  for (uint_t i = 0; i < reffNullsArray.size(); ++i)
    {
      if ( ! reffNullsArray[i])
        continue;

      if (visitedRows[i])
        return false;

      else
        visitedRows[i] = true;

      bool notFound = true;
      for (uint_t j = 0; (j < matchedRows.Count()) && notFound; ++j)
        {
          DROW_INDEX row;

          matchedRows.Get (j, row);
          if (row.mValue == i)
            notFound = false;
        }

      if ( notFound)
        return false;
    }

  for (ROW_INDEX i = 0; i < rowsCount; ++i)
    {
      if ( ! visitedRows[i])
        {
          bool notFound = true;
          for (uint_t j = 0;
               broken && (j < matchedRows.Count()) && notFound;
               ++j)
            {
              DROW_INDEX row;

              matchedRows.Get (j, row);
              if (row.mValue == i)
                notFound = false;
            }

          if (notFound)
            return false;
        }
    }

  return true;
}

static bool
add_bool_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("bool");

  DBool value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DBool ((wh_rnd () & 1) == 0);
      bool_rows.Add (value);
      bool_rows_n.push_back (false);
    }
  else
    {
      bool_rows_n.push_back (true);
      bool_rows.Add (DBool (false));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_bool_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'bool'.\n";
  if (bool_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("bool");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DBool rowValue, arrayValue;

      table.Get (row, field, rowValue);
      bool_rows.Get (row, arrayValue);

      if (bool_rows_n[row])
        {
           if (arrayValue != DBool (false))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }

      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DBool> (table,
                                     bool_rows,
                                     bool_rows_n,
                                     field,
                                     rowsCount,
                                     broken);
}


static bool
add_char_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("char");

  DChar value;

  if (wh_rnd () % (row + 1) != 0)
    {
      int x = 0xD800;

      while ((x >= 0xD800) && (x <= 0xDFFF))
        x = (wh_rnd () + 1) % 0x10FFFF;

      value = DChar (x);
      char_rows.Add (value);
      char_rows_n.push_back (false);
    }
  else
    {
      char_rows_n.push_back (true);
      char_rows.Add (DChar ('Z'));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_char_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'char'.\n";

  if (char_rows.Count() != rowsCount)
    return false;

  const FIELD_INDEX field = table.RetrieveField ("char");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DChar rowValue, arrayValue;

      table.Get (row, field, rowValue);
      char_rows.Get (row, arrayValue);

      if (char_rows_n[row])
        {
           if (arrayValue != DChar ('Z'))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }

      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DChar> (table,
                                     char_rows,
                                     char_rows_n,
                                     field,
                                     rowsCount,
                                     broken);
}


static bool
add_date_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("date");

  DDate value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DDate (-800 + wh_rnd () % 1600,
                     wh_rnd () % 11 + 1,
                     wh_rnd () % 25 + 1);

      date_rows.Add (value);
      date_rows_n.push_back (false);
    }
  else
    {
      date_rows_n.push_back (true);
      date_rows.Add (DDate (1, 1, 1));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_date_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'date'.\n";
  if (date_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("date");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DDate rowValue, arrayValue;

      table.Get (row, field, rowValue);
      date_rows.Get (row, arrayValue);

      if (date_rows_n[row])
        {
           if (arrayValue != DDate (1, 1, 1))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mYear))
                return false;
            }
        }
    }

  return check_indexed_field<DDate> (table,
                                     date_rows,
                                     date_rows_n,
                                     field,
                                     rowsCount,
                                     broken);
}


static bool
add_datetime_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("datetime");

  DDateTime value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DDateTime (-1600 + wh_rnd () % 3200,
                         wh_rnd () % 11 + 1,
                         wh_rnd () % 25 + 1,
                         wh_rnd () % 24,
                         wh_rnd () % 60,
                         wh_rnd () % 60);

      datetime_rows.Add (value);
      datetime_rows_n.push_back (false);
    }
  else
    {
      datetime_rows_n.push_back (true);
      datetime_rows.Add (DDateTime (1, 1, 1, 0, 1, 2));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_datetime_rows (ITable&           table,
                      const ROW_INDEX   rowsCount,
                      const bool        broken)
{
  cout << "Test field 'datetime'.\n";
  if (datetime_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("datetime");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DDateTime rowValue, arrayValue;

      table.Get (row, field, rowValue);
      datetime_rows.Get (row, arrayValue);

      if (datetime_rows_n[row])
        {
           if (arrayValue != DDateTime (1, 1, 1, 0, 1, 2))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mYear))
                return false;
            }
        }
    }

  return check_indexed_field<DDateTime> (table,
                                         datetime_rows,
                                         datetime_rows_n,
                                         field,
                                         rowsCount,
                                         broken);
}


static bool
add_hirestime_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("hirestime");

  DHiresTime value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DHiresTime (-1600 + wh_rnd () % 3200,
                          wh_rnd () % 11 + 1,
                          wh_rnd () % 25 + 1,
                          wh_rnd () % 24,
                          wh_rnd () % 60,
                          wh_rnd () % 60,
                          wh_rnd () % 1000000);

      hirestime_rows.Add (value);
      hirestime_rows_n.push_back (false);
    }
  else
    {
      hirestime_rows_n.push_back (true);
      hirestime_rows.Add (DHiresTime (1, 1, 1, 0, 1, 2, 10));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_hirestime_rows (ITable& table,
                      const ROW_INDEX rowsCount,
                      const bool broken)
{
  cout << "Test field 'hirestime'.\n";
  if (hirestime_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("hirestime");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DHiresTime rowValue, arrayValue;

      table.Get (row, field, rowValue);
      hirestime_rows.Get (row, arrayValue);

      if (hirestime_rows_n[row])
        {
           if (arrayValue != DHiresTime (1, 1, 1, 0, 1, 2, 10))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mYear))
                return false;
            }
        }
    }

  return check_indexed_field<DHiresTime> (table,
                                          hirestime_rows,
                                          hirestime_rows_n,
                                          field,
                                          rowsCount,
                                          broken);
}


static bool
add_int8_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("int8");

  DInt8 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DInt8 (wh_rnd ());

      int8_rows.Add (value);
      int8_rows_n.push_back (false);
    }
  else
    {
      int8_rows_n.push_back (true);
      int8_rows.Add (DInt8 (12));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_int8_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'int8'.\n";
  if (int8_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("int8");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DInt8 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      int8_rows.Get (row, arrayValue);

      if (int8_rows_n[row])
        {
           if (arrayValue != DInt8 (12))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DInt8> (table,
                                     int8_rows,
                                     int8_rows_n,
                                     field,
                                     rowsCount,
                                     broken);
}


static bool
add_int16_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("int16");

  DInt16 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DInt16 (wh_rnd ());

      int16_rows.Add (value);
      int16_rows_n.push_back (false);
    }
  else
    {
      int16_rows_n.push_back (true);
      int16_rows.Add (DInt16 (-13));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_int16_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'int16'.\n";
  if (int16_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("int16");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DInt16 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      int16_rows.Get (row, arrayValue);

      if (int16_rows_n[row])
        {
           if (arrayValue != DInt16 (-13))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DInt16> (table,
                                      int16_rows,
                                      int16_rows_n,
                                      field,
                                      rowsCount,
                                      broken);
}


static bool
add_int32_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("int32");

  DInt32 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DInt32 (wh_rnd ());

      int32_rows.Add (value);
      int32_rows_n.push_back (false);
    }
  else
    {
      int32_rows_n.push_back (true);
      int32_rows.Add (DInt32 (14));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_int32_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'int32'.\n";
  if (int32_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("int32");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DInt32 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      int32_rows.Get (row, arrayValue);

      if (int32_rows_n[row])
        {
           if (arrayValue != DInt32 (14))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DInt32> (table,
                                      int32_rows,
                                      int32_rows_n,
                                      field,
                                      rowsCount,
                                      broken);
}


static bool
add_int64_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("int64");

  DInt64 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DInt64 (wh_rnd ());

      int64_rows.Add (value);
      int64_rows_n.push_back (false);
    }
  else
    {
      int64_rows_n.push_back (true);
      int64_rows.Add (DInt64 (-15));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_int64_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'int64'.\n";
  if (int64_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("int64");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DInt64 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      int64_rows.Get (row, arrayValue);

      if (int64_rows_n[row])
        {
           if (arrayValue != DInt64 (-15))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DInt64> (table,
                                      int64_rows,
                                      int64_rows_n,
                                      field,
                                      rowsCount,
                                      broken);
}


static bool
add_u_int8_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("uint8");

  DUInt8 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DUInt8 (wh_rnd ());

      u_int8_rows.Add (value);
      u_int8_rows_n.push_back (false);
    }
  else
    {
      u_int8_rows_n.push_back (true);
      u_int8_rows.Add (DUInt8 (12));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_u_int8_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'uint8'.\n";
  if (u_int8_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("uint8");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DUInt8 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      u_int8_rows.Get (row, arrayValue);

      if (u_int8_rows_n[row])
        {
           if (arrayValue != DUInt8 (12))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DUInt8> (table,
                                      u_int8_rows,
                                      u_int8_rows_n,
                                      field,
                                      rowsCount,
                                      broken);
}


static bool
add_u_int16_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("uint16");

  DUInt16 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DUInt16 (wh_rnd ());

      u_int16_rows.Add (value);
      u_int16_rows_n.push_back (false);
    }
  else
    {
      u_int16_rows_n.push_back (true);
      u_int16_rows.Add (DUInt16 (13));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_u_int16_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'uint16'.\n";
  if (u_int16_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("uint16");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DUInt16 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      u_int16_rows.Get (row, arrayValue);

      if (u_int16_rows_n[row])
        {
           if (arrayValue != DUInt16 (13))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DUInt16> (table,
                                       u_int16_rows,
                                       u_int16_rows_n,
                                       field,
                                       rowsCount,
                                       broken);
}


static bool
add_u_int32_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("uint32");

  DUInt32 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DUInt32 (wh_rnd ());

      u_int32_rows.Add (value);
      u_int32_rows_n.push_back (false);
    }
  else
    {
      u_int32_rows_n.push_back (true);
      u_int32_rows.Add (DUInt32 (14));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_u_int32_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'uint32'.\n";
  if (u_int32_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("uint32");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DUInt32 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      u_int32_rows.Get (row, arrayValue);

      if (u_int32_rows_n[row])
        {
           if (arrayValue != DUInt32 (14))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DUInt32> (table,
                                       u_int32_rows,
                                       u_int32_rows_n,
                                       field,
                                       rowsCount,
                                       broken);
}


static bool
add_u_int64_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("uint64");

  DUInt64 value;

  if (wh_rnd () % (row + 1) != 0)
    {
      value = DUInt64 (wh_rnd ());

      u_int64_rows.Add (value);
      u_int64_rows_n.push_back (false);
    }
  else
    {
      u_int64_rows_n.push_back (true);
      u_int64_rows.Add (DUInt64 (15));
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_u_int64_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'uint64'.\n";
  if (u_int64_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("uint64");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DUInt64 rowValue, arrayValue;

      table.Get (row, field, rowValue);
      u_int64_rows.Get (row, arrayValue);

      if (u_int64_rows_n[row])
        {
           if (arrayValue != DUInt64 (15))
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue))
                return false;
            }
        }
    }

  return check_indexed_field<DUInt64> (table,
                                       u_int64_rows,
                                       u_int64_rows_n,
                                       field,
                                       rowsCount,
                                       broken);
}



static bool
add_real_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("real");

  DReal value;

  if (wh_rnd () % (row + 1) != 0)
    {
      int64_t temp = wh_rnd ();

      value = DReal (DBS_REAL_T ((double)temp / (100000000)));

      real_rows.Add (value);
      real_rows_n.push_back (false);
    }
  else
    {
      real_rows_n.push_back (true);
      real_rows.Add (DReal::Min ());
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_real_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'real'.\n";
  if (u_int64_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("real");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DReal rowValue, arrayValue;

      table.Get (row, field, rowValue);
      real_rows.Get (row, arrayValue);

      if (real_rows_n[row])
        {
           if (arrayValue != DReal::Min ())
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue.Integer ())
                  || ! verify_int_marker (rowValue.mValue.Fractional()))
                {
                  return false;
                }
            }
        }
    }

  return check_indexed_field<DReal> (table,
                                     real_rows,
                                     real_rows_n,
                                     field,
                                     rowsCount,
                                     broken );
}


static bool
add_richreal_row (ITable& table, const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("richreal");

  DRichReal value;

  if (wh_rnd () % (row + 1) != 0)
    {
      int64_t temp = wh_rnd ();

      value = DRichReal (DBS_RICHREAL_T ((double)temp / DBS_RICHREAL_PREC));

      richreal_rows.Add (value);
      richreal_rows_n.push_back (false);
    }
  else
    {
      richreal_rows_n.push_back (true);
      richreal_rows.Add (DRichReal::Max ());
    }

  table.Set (row, field, value);

  return true;
}


static bool
verify_richreal_rows (ITable& table, const ROW_INDEX rowsCount, const bool broken)
{
  cout << "Test field 'richreal'.\n";
  if (u_int64_rows.Count() != rowsCount)
    return false;

  FIELD_INDEX field = table.RetrieveField ("richreal");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DRichReal rowValue, arrayValue;

      table.Get (row, field, rowValue);
      richreal_rows.Get (row, arrayValue);

      if (richreal_rows_n[row])
        {
           if (arrayValue != DRichReal::Max ())
             return false;

           if ( ! broken && ! rowValue.IsNull())
             return false;
        }
      else if (rowValue != arrayValue)
        {
          if (! broken)
            return false;

          if ( ! rowValue.IsNull())
            {
              if ( ! verify_int_marker (rowValue.mValue.Integer ())
                  || ! verify_int_marker (rowValue.mValue.Fractional()))
                {
                  return false;
                }
            }
        }
    }

  return check_indexed_field<DRichReal> (table,
                                         richreal_rows,
                                         richreal_rows_n,
                                         field,
                                         rowsCount,
                                         broken);
}



static bool
add_text_row (ITable&         table,
              const ROW_INDEX row)
{
  FIELD_INDEX field = table.RetrieveField ("text");

  if (row % 5 == 0)
    table.Set (row, field, DText());

  else
    table.Set (row, field, DText (text [row % 11]));

  return true;
}


static bool
verify_text_rows (ITable& table,
                  const ROW_INDEX rowsCount,
                  const bool broken)
{

  cout << "Test field 'text'.\n";
  FIELD_INDEX field = table.RetrieveField ("text");

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DText fieldValue;
      table.Get (row, field, fieldValue);

      if (row % 5 == 0)
        {
          if ( ! fieldValue.IsNull())
            return false;
        }
      else if  ((fieldValue != DText (text[row % 11]))
                && ( broken && ! fieldValue.IsNull()))
        {
          return false;
        }
    }

  return true;
}



template<typename T> bool
add_array_row (ITable& table,
               const ROW_INDEX row,
               const char* const fieldName,
               const DArray& refArray,
               vector<uint8_t>& arraysSizes)
{
  FIELD_INDEX field = table.RetrieveField (fieldName);

  const uint8_t arraySize = (wh_rnd () % 13) % refArray.Count();

  DArray temp;
  for (uint8_t i = 0; i < arraySize; ++i)
    {
      T value;

      refArray.Get (i, value);
      temp.Add (value);
    }

  table.Set (row, field, temp);
  arraysSizes.push_back (arraySize);

  return true;
}


template<typename T> bool
verify_array_rows (ITable& table,
                   const ROW_INDEX rowsCount,
                   const char* const fieldName,
                   const DArray& refArray,
                   const vector<uint8_t>& arraysSizes,
                   const bool broken)
{
  cout << "Test field '" << fieldName << "'.\n";

  FIELD_INDEX field = table.RetrieveField (fieldName);

  for (ROW_INDEX row = 0; row < rowsCount; ++row)
    {
      DArray temp;
      table.Get (row, field, temp);

      if (temp.Count() != arraysSizes[row])
        {
          if (broken && temp.IsNull())
            continue ;

          return false;
        }

      for (uint_t i = 0; i < arraysSizes[row]; ++i)
        {
          T refValue, tabValue;

          temp.Get (i, tabValue);
          refArray.Get (i, refValue);

          if (tabValue != refValue)
            {
              if ( ! broken)
                return false;

              else if (! verify_int_marker (tabValue))
                return false;
            }
        }
    }

  return true;
}


static bool
add_table_value_tab1 (ITable& table, const ROW_INDEX rowsCount)
{
  bool result = true;

  cout << "Filling a table with " << rowsCount << "..." << endl;

  table.CreateIndex (table.RetrieveField ("bool"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("char"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("date"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("datetime"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("hirestime"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("int8"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("int16"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("int32"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("int64"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("uint8"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("uint16"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("uint32"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("uint64"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("real"), NULL, NULL);
  table.CreateIndex (table.RetrieveField ("richreal"), NULL, NULL);

  for (ROW_INDEX row = 0; result && (row < rowsCount); ++row)
    {
      if ((row * 100) % rowsCount == 0)
        {
          cout << '\r' << (row * 100) / rowsCount << "%";
          cout.flush();
        }

      result = result && add_bool_row (table, row);
      result = result && add_char_row (table, row);
      result = result && add_date_row (table, row);
      result = result && add_datetime_row (table, row);
      result = result && add_hirestime_row (table, row);
      result = result && add_int8_row (table, row);
      result = result && add_int16_row (table, row);
      result = result && add_int32_row (table, row);
      result = result && add_int64_row (table, row);

      result = result && add_u_int8_row (table, row);
      result = result && add_u_int16_row (table, row);
      result = result && add_u_int32_row (table, row);
      result = result && add_u_int64_row (table, row);

      result = result && add_real_row (table, row);
      result = result && add_richreal_row (table, row);

      result = result && add_array_row<DBool> (table, row, "array_bool", bool_rows, a_bool_rows_n);
      result = result && add_array_row<DChar> (table, row, "array_char", char_rows, a_char_rows_n);
      result = result && add_array_row<DDate> (table, row, "array_date", date_rows, a_date_rows_n);
      result = result && add_array_row<DDateTime> (table, row, "array_datetime", datetime_rows, a_datetime_rows_n);
      result = result && add_array_row<DHiresTime> (table, row, "array_hirestime", hirestime_rows, a_hirestime_rows_n);
      result = result && add_array_row<DInt8> (table, row, "array_int8", int8_rows, a_int8_rows_n);
      result = result && add_array_row<DInt16> (table, row, "array_int16", int16_rows, a_int16_rows_n);
      result = result && add_array_row<DInt32> (table, row, "array_int32", int32_rows, a_int32_rows_n);
      result = result && add_array_row<DInt64> (table, row, "array_int64", int64_rows, a_int64_rows_n);

      result = result && add_array_row<DUInt8> (table, row, "array_uint8", u_int8_rows, a_u_int8_rows_n);
      result = result && add_array_row<DUInt16> (table, row, "array_uint16", u_int16_rows, a_u_int16_rows_n);
      result = result && add_array_row<DUInt32> (table, row, "array_uint32", u_int32_rows, a_u_int32_rows_n);
      result = result && add_array_row<DUInt64> (table, row, "array_uint64", u_int64_rows, a_u_int64_rows_n);

      result = result && add_array_row<DReal> (table, row, "array_real", real_rows, a_real_rows_n);
      result = result && add_array_row<DRichReal> (table, row, "array_richreal", richreal_rows, a_richreal_rows_n);

      result = result && add_text_row (table, row);
    }

  cout << "\r100%" << endl;

  return result;
}


bool build_data_base (const ROW_INDEX rowsCount)
{
  bool result = true;

  DBSCreateDatabase (db_name);

  IDBSHandler& handler = DBSRetrieveDatabase (db_name);

  handler.AddTable (tb_name_1, TOTAL_DESC_FIELDS_COUNT, field_descs);
  ITable& table_1 = handler.RetrievePersistentTable (tb_name_1);

  result = result && add_table_value_tab1 (table_1, rowsCount);

  handler.ReleaseTable (table_1);

  DBSReleaseDatabase (handler);

  return result;
}


bool
break_data_base()
{
  //Break the database file
  const uint8_t mask          = 0xFD;
  const char   noTableName[]  = "This_is_love";

  File dataBaseFile ((string (db_name) + ".db").c_str (),
                     WH_FILEOPEN_EXISTING | WH_FILEWRITE);

  dataBaseFile.Seek (24, WH_SEEK_BEGIN);   //data base flags
  dataBaseFile.Write (&mask, sizeof mask);

  dataBaseFile.Seek (1, WH_SEEK_END);
  dataBaseFile.Write (_RC (const uint8_t*, noTableName), sizeof noTableName);

  //Break the database's table

  File tableFile (tb_name_1, WH_FILEOPEN_EXISTING | WH_FILEWRITE);

  tableFile.Seek (60, WH_SEEK_BEGIN);  //table flags
  tableFile.Write (&mask, sizeof mask);

  tableFile.Seek (56, WH_SEEK_BEGIN); //table row size
  tableFile.Write (&mask, sizeof mask);

  File vsTable ((string (tb_name_1) + "_v").c_str (),
                WH_FILEOPEN_EXISTING | WH_FILEWRITE);

  File fsTable ((string (tb_name_1) + "_f").c_str (),
                WH_FILEOPEN_EXISTING | WH_FILEWRITE);

  uint8_t vsCorruption[64];
  memset (vsCorruption, TABLE_BREAK_MARKER, sizeof vsCorruption);

  const uint_t last = wh_rnd () % 20;
  for (uint_t i = 0, index = 0; i < last; ++i)
    {
      cout << "Breaking # " << i << " index " << index << endl;

      vsTable.Seek (index * sizeof vsCorruption, WH_SEEK_BEGIN);
      vsTable.Write (vsCorruption, sizeof vsCorruption);

      fsTable.Seek (index * sizeof vsCorruption, WH_SEEK_BEGIN);
      fsTable.Write (vsCorruption, sizeof vsCorruption);

      index = wh_rnd () % (vsTable.Size() / sizeof vsCorruption);
    }

  vsTable.Seek (sizeof vsCorruption, WH_SEEK_END);
  vsTable.Write (vsCorruption, sizeof vsCorruption);

  fsTable.Seek (sizeof vsCorruption, WH_SEEK_END);
  fsTable.Write (vsCorruption, sizeof vsCorruption);

  return true;
}

static bool
repair_callback (const FIX_ERROR_CALLBACK_TYPE type,
                 const char* const             format,
                 ... )
{
  va_list vl;

  va_start (vl, format);
  vprintf (format, vl);
  va_end (vl);

  printf ("\n");

  return true;
}


static bool
repair_data_base (const bool broken)
{
  const bool valid = DBSValidateDatabase (db_name);

  if (broken && valid)
    return false;

  return DBSRepairDatabase (db_name, NULL, repair_callback);
}


static bool
check_repaired_database (const ROW_INDEX rowsCount,
                         const bool      broken)
{
  cout << "Checking the repair ... \n";

  IDBSHandler& handler = DBSRetrieveDatabase (db_name);
  ITable& table = handler.RetrievePersistentTable (tb_name_1);

  bool result = true;

  result = result && verify_bool_rows (table, rowsCount, broken);
  result = result && verify_char_rows (table, rowsCount, broken);
  result = result && verify_date_rows (table, rowsCount, broken);
  result = result && verify_datetime_rows (table, rowsCount, broken);
  result = result && verify_hirestime_rows (table, rowsCount, broken);
  result = result && verify_int8_rows (table, rowsCount, broken);
  result = result && verify_int16_rows (table, rowsCount, broken);
  result = result && verify_int32_rows (table, rowsCount, broken);
  result = result && verify_int64_rows (table, rowsCount, broken);

  result = result && verify_u_int8_rows (table, rowsCount, broken);
  result = result && verify_u_int16_rows (table, rowsCount, broken);
  result = result && verify_u_int32_rows (table, rowsCount, broken);
  result = result && verify_u_int64_rows (table, rowsCount, broken);

  result = result && verify_real_rows (table, rowsCount, broken);
  result = result && verify_richreal_rows (table, rowsCount, broken);


  result = result && verify_array_rows<DBool> (table, rowsCount, "array_bool", bool_rows, a_bool_rows_n, broken);
  result = result && verify_array_rows<DChar> (table, rowsCount, "array_char", char_rows, a_char_rows_n, broken);
  result = result && verify_array_rows<DDate> (table, rowsCount, "array_date", date_rows, a_date_rows_n, broken);
  result = result && verify_array_rows<DDateTime> (table, rowsCount, "array_datetime", datetime_rows, a_datetime_rows_n, broken);
  result = result && verify_array_rows<DHiresTime> (table, rowsCount, "array_hirestime", hirestime_rows, a_hirestime_rows_n, broken);
  result = result && verify_array_rows<DInt8> (table, rowsCount, "array_int8", int8_rows, a_int8_rows_n, broken);
  result = result && verify_array_rows<DInt16> (table, rowsCount, "array_int16", int16_rows, a_int16_rows_n, broken);
  result = result && verify_array_rows<DInt32> (table, rowsCount, "array_int32", int32_rows, a_int32_rows_n, broken);
  result = result && verify_array_rows<DInt64> (table, rowsCount, "array_int64", int64_rows, a_int64_rows_n, broken);

  result = result && verify_array_rows<DUInt8> (table, rowsCount, "array_uint8", u_int8_rows, a_u_int8_rows_n, broken);
  result = result && verify_array_rows<DUInt16> (table, rowsCount, "array_uint16", u_int16_rows, a_u_int16_rows_n, broken);
  result = result && verify_array_rows<DUInt32> (table, rowsCount, "array_uint32", u_int32_rows, a_u_int32_rows_n, broken);
  result = result && verify_array_rows<DUInt64> (table, rowsCount, "array_uint64", u_int64_rows, a_u_int64_rows_n, broken);

  result = result && verify_array_rows<DReal> (table, rowsCount, "array_real", real_rows, a_real_rows_n, broken);
  result = result && verify_array_rows<DRichReal> (table, rowsCount, "array_richreal", richreal_rows, a_richreal_rows_n, broken);

  result = result && verify_text_rows (table, rowsCount, broken);

  handler.ReleaseTable (table);

  DBSReleaseDatabase (handler);

  if (broken)
    DBSRemoveDatabase (db_name);

  cout << (result ? "OK" : "FAIL") << endl;

  return result;
}


bool
test_data_base (const ROW_INDEX rowsCount)
{
  bool result = true;

  result = result && build_data_base (rowsCount);
  result = result && repair_data_base (false);
  result = result && check_repaired_database (rowsCount, false);
  result = result && break_data_base();
  result = result && repair_data_base (true);
  result = result && check_repaired_database (rowsCount, true);

  return result;
}

int
main (int argc, char** argv)
{
  if (argc > 1)
    {
      _rowsCount = atol (argv[1]);
    }

  DBSInit (DBSSettings());

  bool success = true;

  success = success && test_data_base (_rowsCount);

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
