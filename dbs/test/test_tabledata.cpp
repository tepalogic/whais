/*
 * test_dbsmgr.cpp
 *
 *  Created on: Jul 11, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>
#include <vector>

#include "dbs/dbs_mgr.h"
#include "dbs/dbs_exception.h"

#include "../pastra/ps_table.h"
#include "../pastra/ps_serializer.h"

using namespace whais;
using namespace pastra;

const char db_name[] = "t_baza_date_1";
const char tb_name[] = "t_test_tab";

static const uint_t array_values = 5;

static char text1[] = "This is a text!";
static char text2[] = "This is should be bigger. It must have more chars then the first one";
static char text3[] = "FirstName LastName";
static char text4[] = "Total domination of the world is the goal, but why?\n"
                        "Some one will say because one can, but this is not the case.\b"
                        "I have no idea what I wrote here!\n";


#define INIT_VECTORS  std::vector<DBool> vectBool;\
  std::vector<DChar> vectChar;\
  std::vector<DDate> vectDate;\
  std::vector<DDateTime> vectDateTime;\
  std::vector<DHiresTime> vectHiResDate;\
  std::vector<DUInt8> vectUInt8;\
  std::vector<DUInt16> vectUInt16;\
  std::vector<DUInt32> vectUInt32;\
  std::vector<DUInt64> vectUInt64;\
  std::vector<DReal> vectReal;\
  std::vector<DRichReal> vectRichReal;\
  std::vector<DInt8> vectInt8;\
  std::vector<DInt16> vectInt16;\
  std::vector<DInt32> vectInt32;\
  std::vector<DInt64> vectInt64;\
  std::vector<DText> vectText;\
\
  vectBool.push_back(DBool(true));\
  vectBool.push_back(DBool(true));\
  vectBool.push_back(DBool(false));\
  vectBool.push_back(DBool());\
  vectBool.push_back(DBool(false));\
  \
  vectChar.push_back(DChar());\
  vectChar.push_back(DChar('B'));\
  vectChar.push_back(DChar(0x2312));\
  vectChar.push_back(DChar('V'));\
  vectChar.push_back(DChar('\n'));\
\
  vectDate.push_back(DDate(1981, 11, 14));\
  vectDate.push_back(DDate(2091, 10, 11));\
  vectDate.push_back(DDate(92, 3, 30));\
  vectDate.push_back(DDate());\
  vectDate.push_back(DDate(-1, 3, 30));\
\
  vectDateTime.push_back(DDateTime(781, 1, 1, 10, 11, 12));\
  vectDateTime.push_back(DDateTime(1970, 1, 1, 0, 0, 0));\
  vectDateTime.push_back(DDateTime(0, 1, 1, 0, 0, 0));\
  vectDateTime.push_back(DDateTime(21, 12, 31, 23, 59, 59));\
  vectDateTime.push_back(DDateTime());\
\
  vectHiResDate.push_back(DHiresTime(1, 1, 1, 0, 0, 0, 2312));\
  vectHiResDate.push_back(DHiresTime());\
  vectHiResDate.push_back(DHiresTime(0, 1, 2, 1, 1, 1, 2132));\
  vectHiResDate.push_back(DHiresTime(-1324, 12, 31, 23, 59, 59, 9897));\
  vectHiResDate.push_back(DHiresTime());\
\
  vectUInt8.push_back(DUInt8());\
  vectUInt8.push_back(DUInt8(2));\
  vectUInt8.push_back(DUInt8());\
  vectUInt8.push_back(DUInt8(0x67));\
  vectUInt8.push_back(DUInt8(2));\
\
  vectUInt16.push_back(DUInt16(0xAABB));\
  vectUInt16.push_back(DUInt16( 0xBBAA));\
  vectUInt16.push_back(DUInt16());\
  vectUInt16.push_back(DUInt16(0x67));\
  vectUInt16.push_back(DUInt16(0xFF00));\
\
  vectUInt32.push_back(DUInt32(0x0));\
  vectUInt32.push_back(DUInt32( 0xCDDEBBAA));\
  vectUInt32.push_back(DUInt32(0x0));\
  vectUInt32.push_back(DUInt32(0xFF0000FF));\
  vectUInt32.push_back(DUInt32());\
\
  vectUInt64.push_back(DUInt64(~0x0));\
  vectUInt64.push_back(DUInt64( 0x0102030405060708));\
  vectUInt64.push_back(DUInt64(1));\
  vectUInt64.push_back(DUInt64(2));\
  vectUInt64.push_back(DUInt64());\
\
  vectInt8.push_back(DInt8());\
  vectInt8.push_back(DInt8(-2));\
  vectInt8.push_back(DInt8());\
  vectInt8.push_back(DInt8(-67));\
  vectInt8.push_back(DInt8(2));\
\
  vectInt16.push_back(DInt16(-21));\
  vectInt16.push_back(DInt16( 21112));\
  vectInt16.push_back(DInt16());\
  vectInt16.push_back(DInt16(-2312));\
  vectInt16.push_back(DInt16(-12));\
\
  vectInt32.push_back(DInt32(0x10));\
  vectInt32.push_back(DInt32(0x80808080));\
  vectInt32.push_back(DInt32(-1));\
  vectInt32.push_back(DInt32(-2));\
  vectInt32.push_back(DInt32());\
\
  vectInt64.push_back(DInt64(~0x0));\
  vectInt64.push_back(DInt64(0x0102030405060708));\
  vectInt64.push_back(DInt64(-1));\
  vectInt64.push_back(DInt64(2));\
  vectInt64.push_back(DInt64());\
\
  vectReal.push_back(DReal(-199.21));\
  vectReal.push_back(DReal(199.21));\
  vectReal.push_back(DReal());\
  vectReal.push_back(DReal(0.98989));\
  vectReal.push_back(DReal(98989.0));\
\
  vectRichReal.push_back(DRichReal(1.0));\
  vectRichReal.push_back(DRichReal(1234.91823412));\
  vectRichReal.push_back(DRichReal(3.141592653589793));\
  vectRichReal.push_back(DRichReal(-1.6180339887498948482));\
  vectRichReal.push_back(DRichReal());\
\
  vectText.push_back(DText(_RC(uint8_t*, text1)));\
  vectText.push_back(DText());\
  vectText.push_back(DText(_RC(uint8_t*, text2)));\
  vectText.push_back(DText(_RC(uint8_t*, text3)));\
  vectText.push_back(DText(_RC(uint8_t*, text4)));


#define CLEAR_VECTORS vectBool.clear();\
  vectChar.clear();\
  vectDate.clear();\
  vectDateTime.clear();\
  vectHiResDate.clear();\
  vectUInt8.clear();\
  vectUInt16.clear();\
  vectUInt32.clear();\
  vectUInt64.clear();\
  vectInt8.clear();\
  vectInt16.clear();\
  vectInt32.clear();\
  vectInt64.clear();\
  vectReal.clear();\
  vectRichReal.clear();\
  vectText.clear();


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
    {"time", T_DATETIME, false},
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
    {"array_time", T_DATETIME, true},
    {"array_hirestime", T_HIRESTIME, true},
    {"array_real", T_REAL, true},
    {"array_richreal", T_RICHREAL, true},

};

static const uint_t FIXED_DESC_FIELDS_COUNT = 15;
static const uint_t VAR_DESC_FIELDS_COUNT = 16;
static const uint_t TOTAL_DESC_FIELDS_COUNT = 31;

static const char *
type_to_text(uint_t type)
{
  if (type == T_BOOL)
    {
      return "BOOL";
    }
  else if (type == T_CHAR)
    {
      return "CHAR";
    }
  else if (type == T_DATE)
    {
      return "DATE";
    }
  else if (type == T_DATETIME)
    {
      return "DATETIME";
    }
  else if (type == T_HIRESTIME)
    {
      return "HIRESTIME";
    }
  else if (type == T_INT8)
    {
      return "INT8";
    }
  else if (type == T_INT16)
    {
      return "INT16";
    }
  else if (type == T_INT32)
    {
      return "INT32";
    }
  else if (type == T_INT64)
    {
      return "INT64";
    }
  else if (type == T_REAL)
    {
      return "REAL";
    }
  else if (type == T_RICHREAL)
    {
      return "RICHREAL";
    }
  else if (type == T_TEXT)
    {
      return "TEXT";
    }
  else if (type == T_UINT8)
    {
      return "UINT8";
    }
  else if (type == T_UINT16)
    {
      return "UINT16";
    }
  else if (type == T_UINT32)
    {
      return "UINT32";
    }
  else if (type == T_UINT64)
    {
      return "UINT64";
    }
  assert(0);
  return nullptr;
}


static void
print_table_fields(IDBSHandler& rDbs, const char* tb_name)
{
  uint_t      fieldIndex = 0;
  ITable& table      = rDbs.RetrievePersistentTable(tb_name);

  std::cout << "Field list:" << std::endl;
  while (fieldIndex < table.FieldsCount())
    {
      DBSFieldDescriptor currField = table.DescribeField(fieldIndex);

      std::cout << '\t' << currField.name << "\t: ";
      if (currField.isArray)
        std::cout << "ARRAY ";
      std::cout << type_to_text(currField.type) << std::endl;

      ++fieldIndex;
    }
  rDbs.ReleaseTable(table);
}

template <class T> uint_t
add_fixed_values_vector_to_table(ITable& table, std::vector<T> &vectValues)
{
  uint_t fieldIndex = 0;

  while (fieldIndex < table.FieldsCount())
    {
      DBSFieldDescriptor desc = table.DescribeField(fieldIndex);
      if ((desc.type == vectValues[0].DBSType()) &&
          (desc.isArray == false))
        {
          break;
        }

      ++fieldIndex;
    }

  if (fieldIndex >= table.FieldsCount())
    return ~0;


  for (uint_t index = 0; index < vectValues.size(); ++index)
    {
      if (table.AllocatedRows() <= index)
        table.AddRow();

      table.Set(index, fieldIndex, vectValues[index]);
    }

  return fieldIndex;
}

template <class T> bool
test_fixed_values_vector(ITable& table, const uint_t fieldIndex, std::vector<T> &vectValues)
{
  T fieldValue;

  for (uint_t rowIndex = 0; rowIndex < table.AllocatedRows(); ++rowIndex)
    {
      table.Get(rowIndex, fieldIndex, fieldValue);
      if (! (fieldValue == vectValues[rowIndex]))
        return false;
    }

  return true;
}

template <class T> bool
test_fixed_values_vector_reverse(ITable& table,
                                  const uint_t fieldIndex,
                                  std::vector<T> &vectValues)
{
  T fieldValue;
  uint_t rowIndex = table.AllocatedRows();

  do
    {
      rowIndex--;
      table.Get(rowIndex, fieldIndex, fieldValue);
      if (! (fieldValue == vectValues[rowIndex]))
        return false;
    }
  while (rowIndex > 0);

  return true;
}

template <class T> bool
test_fixed_value_field(IDBSHandler& rDbs, std::vector<T> &valuesVect)
{

  uint_t fieldIndex = ~0;
  bool   result = true;

  {
    ITable& table = rDbs.RetrievePersistentTable(tb_name);

    fieldIndex = add_fixed_values_vector_to_table(table, valuesVect);

    if (fieldIndex == _SC(uint_t, ~0))
      result = false;


    if (result && test_fixed_values_vector(table, fieldIndex, valuesVect) == false)
      result = false;

    rDbs.ReleaseTable(table);
  }

  if (result)
  {
    ITable& table = rDbs.RetrievePersistentTable(tb_name);

    if (test_fixed_values_vector_reverse(table, fieldIndex, valuesVect) == false)
      result = false;

    rDbs.ReleaseTable(table);
  }

  return result;
}

template <class T> bool
test_fixed_value_field(ITable& table, std::vector<T> &valuesVect)
{

  uint_t fieldIndex = ~0;
  bool   result = true;

  {
    fieldIndex = add_fixed_values_vector_to_table(table, valuesVect);

    if (fieldIndex == _SC(uint_t, ~0))
      result = false;


    if (result && test_fixed_values_vector(table, fieldIndex, valuesVect) == false)
      result = false;
  }

  if (result)
  {
    if (test_fixed_values_vector_reverse(table, fieldIndex, valuesVect) == false)
      result = false;
  }

  return result;
}

bool
test_fixed_values_table(IDBSHandler& rDbs)
{
  std::cout << "Testing fixed values ... ";
  bool result = true;

  INIT_VECTORS;

  rDbs.AddTable(tb_name, FIXED_DESC_FIELDS_COUNT, field_descs);
  print_table_fields(rDbs, tb_name);

  result = result && test_fixed_value_field(rDbs, vectBool);
  result = result && test_fixed_value_field(rDbs, vectChar);
  result = result && test_fixed_value_field(rDbs, vectDate);
  result = result && test_fixed_value_field(rDbs, vectDateTime);
  result = result && test_fixed_value_field(rDbs, vectHiResDate);
  result = result && test_fixed_value_field(rDbs, vectUInt8);
  result = result && test_fixed_value_field(rDbs, vectUInt16);
  result = result && test_fixed_value_field(rDbs, vectUInt32);
  result = result && test_fixed_value_field(rDbs, vectUInt64);

  result = result && test_fixed_value_field(rDbs, vectReal);
  result = result && test_fixed_value_field(rDbs, vectRichReal);

  result = result && test_fixed_value_field(rDbs, vectInt8);
  result = result && test_fixed_value_field(rDbs, vectInt16);
  result = result && test_fixed_value_field(rDbs, vectInt32);
  result = result && test_fixed_value_field(rDbs, vectInt64);

  rDbs.DeleteTable(tb_name);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


template <class T> bool
check_value_array(std::vector<T> &source, DArray& array, const uint_t seed)
{
  if (array.Type() != source[0].DBSType())
    return false;

  uint_t arrayIndex = 0;

  for (uint_t elIndex = 0; elIndex < source.size(); ++elIndex)
    {
      T testElement;

      uint64_t testIndex = (elIndex + seed) % source.size();
      if (source[testIndex] == T())
        continue;

      array.Get(arrayIndex, testElement);

      if (testElement == source [testIndex])
        ++arrayIndex;
      else
        return false;
    }

  return true;
}


template <class T> bool
fill_value_array(std::vector<T> &source, DArray& array, const uint_t seed)
{
  if (array.Type() != source[0].DBSType())
    return false;

  for (uint_t elIndex = 0; elIndex < source.size(); ++elIndex)
    {
      uint64_t testIndex = (elIndex + seed) % source.size();
      if (source[testIndex] == T())
        continue;

      array.Add(source[testIndex]);
    }
  return check_value_array(source, array, seed);
}


template <class T> uint_t
add_vectors_values_to_table(ITable& table, std::vector<T> &testVect)
{
  uint_t fieldIndex = 0;

  for (; fieldIndex < table.FieldsCount(); ++fieldIndex)
    {
      struct DBSFieldDescriptor fieldDesc = table.DescribeField(fieldIndex);
      if (fieldDesc.isArray &&
          (fieldDesc.type == testVect[0].DBSType()))
        {
          break;
        }
    }

  if (fieldIndex >= table.FieldsCount())
    return ~0;


  for (uint_t rowIndex = 0; rowIndex < testVect.size(); ++rowIndex)
    {
      DArray valueArray(_SC(T*, nullptr));

      if (rowIndex == table.AllocatedRows())
        table.AddRow();

      if (fill_value_array(testVect, valueArray, rowIndex) == false)
        return ~0;

      table.Set(rowIndex, fieldIndex, valueArray);
    }

  return fieldIndex;
}

template <class T> bool
test_vector_values_table(ITable& table, uint_t fieldIndex, std::vector<T> &testVect)
{
  for (uint_t rowIndex = 0; rowIndex < table.AllocatedRows(); ++rowIndex)
    {

      DArray testArray(_SC(T*, nullptr));
      table.Get(rowIndex, fieldIndex, testArray);

      if (testVect[rowIndex].IsNull())
        {
          if (testArray.IsNull() != false)
            return false;
        }
      else if (check_value_array(testVect, testArray, rowIndex) == false)
        return false;
    }

  return true;
}

template <class T> bool
test_variable_field_array(IDBSHandler& rDbs, std::vector<T> &testVect)
{

  uint_t fieldIndex = ~0;
  bool   result = true;

  {
    ITable& table = rDbs.RetrievePersistentTable(tb_name);

    fieldIndex = add_vectors_values_to_table(table, testVect);

    if (fieldIndex == _SC(uint_t, ~0))
      result = false;


    if (result && test_vector_values_table(table, fieldIndex, testVect) == false)
      result = false;

    rDbs.ReleaseTable(table);
  }

  if (result)
  {
    ITable& table = rDbs.RetrievePersistentTable(tb_name);

    if (test_vector_values_table(table, fieldIndex, testVect) == false)
      result = false;

    rDbs.ReleaseTable(table);
  }

  return result;
}

template <class T> bool
test_variable_field_array(ITable& table, std::vector<T> &testVect)
{

  uint_t fieldIndex = ~0;
  bool   result = true;

  {
    fieldIndex = add_vectors_values_to_table(table, testVect);

    if (fieldIndex == _SC(uint_t, ~0))
      result = false;


    if (result && test_vector_values_table(table, fieldIndex, testVect) == false)
      result = false;
  }

  if (result)
  {
    if (test_vector_values_table(table, fieldIndex, testVect) == false)
      result = false;
  }

  return result;
}

bool
test_text_value_table(IDBSHandler& rDbs, std::vector<DText> &vectText)
{
  uint_t fieldIndex = 0;
  bool result = true;

  {
    ITable& table = rDbs.RetrievePersistentTable(tb_name);
    for (; fieldIndex < table.FieldsCount(); ++ fieldIndex)
      {
        const DBSFieldDescriptor fieldDesc = table.DescribeField(fieldIndex);
        if (fieldDesc.type == T_TEXT)
          {
            assert(fieldDesc.isArray == false);
            break;
          }
      }

    if (fieldIndex >= table.FieldsCount())
      result = false;

    rDbs.ReleaseTable(table);
  }

  if (result)
    {      ITable& table = rDbs.RetrievePersistentTable(tb_name);

      for (uint_t rowIndex = 0; rowIndex < vectText.size(); ++rowIndex)
        {
          if (rowIndex >= table.AllocatedRows())
            table.AddRow();
          table.Set(rowIndex, fieldIndex, vectText[rowIndex]);
        }

      for (uint_t rowIndex = 0; rowIndex < vectText.size(); ++rowIndex)
        {
          DText testValue;
          table.Get(rowIndex, fieldIndex, testValue);
          if (! (testValue == vectText[rowIndex]))
            {
              result = false;
              break;
            }
        }

      rDbs.ReleaseTable(table);
    }

  if (result)
    {
      ITable& table = rDbs.RetrievePersistentTable(tb_name);

      for (uint_t rowIndex = table.AllocatedRows(); rowIndex > 0; )
        {
          --rowIndex;
          DText testValue;
          table.Get(rowIndex, fieldIndex, testValue);
          if (! (testValue == vectText[rowIndex]))
            {
              result = false;
              break;
            }
        }
      rDbs.ReleaseTable(table);
    }

  return result;
}

bool
test_text_value_table(ITable& table, std::vector<DText> &vectText)
{
  uint_t fieldIndex = 0;
  bool result = true;

  {
    for (; fieldIndex < table.FieldsCount(); ++ fieldIndex)
      {
        const DBSFieldDescriptor fieldDesc = table.DescribeField(fieldIndex);
        if (fieldDesc.type == T_TEXT)
          {
            assert(fieldDesc.isArray == false);
            break;
          }
      }

    if (fieldIndex >= table.FieldsCount())
      result = false;
  }

  if (result)
    {
      for (uint_t rowIndex = 0; rowIndex < vectText.size(); ++rowIndex)
        {
          if (rowIndex >= table.AllocatedRows())
            table.AddRow();
          table.Set(rowIndex, fieldIndex, vectText[rowIndex]);
        }

      for (uint_t rowIndex = 0; rowIndex < vectText.size(); ++rowIndex)
        {
          DText testValue;
          table.Get(rowIndex, fieldIndex, testValue);
          if (! (testValue == vectText[rowIndex]))
            {
              result = false;
              break;
            }
        }
    }

  if (result)
    {
      for (uint_t rowIndex = table.AllocatedRows(); rowIndex > 0; )
        {
          --rowIndex;
          DText testValue;
          table.Get(rowIndex, fieldIndex, testValue);
          if (! (testValue == vectText[rowIndex]))
            {
              result = false;
              break;
            }
        }
    }

  return result;
}

bool
test_variable_values_table(IDBSHandler& rDbs)
{
  std::cout << "Testing variable values ... ";
  bool result = true;

  INIT_VECTORS;

  rDbs.AddTable(tb_name, VAR_DESC_FIELDS_COUNT, field_descs + FIXED_DESC_FIELDS_COUNT);
  print_table_fields(rDbs, tb_name);

  result = result && test_variable_field_array(rDbs, vectBool);
  result = result && test_variable_field_array(rDbs, vectChar);
  result = result && test_variable_field_array(rDbs, vectDate);
  result = result && test_variable_field_array(rDbs, vectDateTime);
  result = result && test_variable_field_array(rDbs, vectHiResDate);
  result = result && test_variable_field_array(rDbs, vectUInt8);
  result = result && test_variable_field_array(rDbs, vectUInt16);
  result = result && test_variable_field_array(rDbs, vectUInt32);
  result = result && test_variable_field_array(rDbs, vectUInt64);
  result = result && test_variable_field_array(rDbs, vectInt8);
  result = result && test_variable_field_array(rDbs, vectInt16);
  result = result && test_variable_field_array(rDbs, vectInt32);
  result = result && test_variable_field_array(rDbs, vectInt64);
  result = result && test_variable_field_array(rDbs, vectReal);
  result = result && test_variable_field_array(rDbs, vectRichReal);

  //Special case for DText!
  result = result && test_text_value_table(rDbs, vectText);

  rDbs.DeleteTable(tb_name);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_full_value_table(IDBSHandler& rDbs)
{
  std::cout << "Testing all values ... ";
  bool result = true;

  INIT_VECTORS;

  rDbs.AddTable(tb_name, TOTAL_DESC_FIELDS_COUNT, field_descs);
  print_table_fields(rDbs, tb_name);

  result = result && test_fixed_value_field(rDbs, vectBool);
  result = result && test_fixed_value_field(rDbs, vectChar);
  result = result && test_fixed_value_field(rDbs, vectDate);
  result = result && test_fixed_value_field(rDbs, vectDateTime);
  result = result && test_fixed_value_field(rDbs, vectHiResDate);
  result = result && test_fixed_value_field(rDbs, vectUInt8);
  result = result && test_fixed_value_field(rDbs, vectUInt16);
  result = result && test_fixed_value_field(rDbs, vectUInt32);
  result = result && test_fixed_value_field(rDbs, vectUInt64);

  result = result && test_variable_field_array(rDbs, vectBool);
  result = result && test_variable_field_array(rDbs, vectChar);
  result = result && test_variable_field_array(rDbs, vectDate);


  result = result && test_fixed_value_field(rDbs, vectReal);
  result = result && test_fixed_value_field(rDbs, vectRichReal);

  result = result && test_variable_field_array(rDbs, vectDateTime);
  result = result && test_variable_field_array(rDbs, vectHiResDate);
  result = result && test_variable_field_array(rDbs, vectUInt8);
  result = result && test_variable_field_array(rDbs, vectUInt16);
  result = result && test_variable_field_array(rDbs, vectUInt32);
  result = result && test_variable_field_array(rDbs, vectUInt64);
  result = result && test_variable_field_array(rDbs, vectInt8);
  result = result && test_variable_field_array(rDbs, vectInt16);

  result = result && test_fixed_value_field(rDbs, vectInt8);
  result = result && test_fixed_value_field(rDbs, vectInt16);
  result = result && test_fixed_value_field(rDbs, vectInt32);
  result = result && test_fixed_value_field(rDbs, vectInt64);


  result = result && test_variable_field_array(rDbs, vectInt32);
  result = result && test_variable_field_array(rDbs, vectInt64);
  result = result && test_variable_field_array(rDbs, vectReal);
  result = result && test_variable_field_array(rDbs, vectRichReal);

  //Special case for DText!
  result = result && test_text_value_table(rDbs, vectText);

  rDbs.DeleteTable(tb_name);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_full_value_temp_table(IDBSHandler& rDbs)
{
  std::cout << "Testing all values on temporal table... ";
  bool result = true;

  INIT_VECTORS;

  ITable& table = rDbs.CreateTempTable(TOTAL_DESC_FIELDS_COUNT, field_descs);

  result = result && test_fixed_value_field(table, vectBool);
  result = result && test_fixed_value_field(table, vectChar);
  result = result && test_fixed_value_field(table, vectDate);
  result = result && test_fixed_value_field(table, vectDateTime);
  result = result && test_fixed_value_field(table, vectHiResDate);
  result = result && test_fixed_value_field(table, vectUInt8);
  result = result && test_fixed_value_field(table, vectUInt16);
  result = result && test_fixed_value_field(table, vectUInt32);
  result = result && test_fixed_value_field(table, vectUInt64);

  result = result && test_variable_field_array(table, vectBool);
  result = result && test_variable_field_array(table, vectChar);
  result = result && test_variable_field_array(table, vectDate);


  result = result && test_fixed_value_field(table, vectReal);
  result = result && test_fixed_value_field(table, vectRichReal);

  result = result && test_variable_field_array(table, vectDateTime);
  result = result && test_variable_field_array(table, vectHiResDate);
  result = result && test_variable_field_array(table, vectUInt8);
  result = result && test_variable_field_array(table, vectUInt16);
  result = result && test_variable_field_array(table, vectUInt32);
  result = result && test_variable_field_array(table, vectUInt64);
  result = result && test_variable_field_array(table, vectInt8);
  result = result && test_variable_field_array(table, vectInt16);

  result = result && test_fixed_value_field(table, vectInt8);
  result = result && test_fixed_value_field(table, vectInt16);
  result = result && test_fixed_value_field(table, vectInt32);
  result = result && test_fixed_value_field(table, vectInt64);


  result = result && test_variable_field_array(table, vectInt32);
  result = result && test_variable_field_array(table, vectInt64);
  result = result && test_variable_field_array(table, vectReal);
  result = result && test_variable_field_array(table, vectRichReal);

  //Special case for DText!
  result = result && test_text_value_table(table, vectText);

  rDbs.ReleaseTable(table);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

int
main()
{
  bool success = true;
  {
    DBSInit(DBSSettings());
    DBSCreateDatabase(db_name);
  }

  IDBSHandler& handler = DBSRetrieveDatabase(db_name);

  success = success && test_fixed_values_table(handler);
  success = success && test_variable_values_table(handler);
  success = success && test_full_value_table(handler);
  success = success && test_full_value_temp_table(handler);

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
