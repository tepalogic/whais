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

#include "test/test_fmw.h"

#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"

#include "../pastra/ps_table.h"
#include "../pastra/ps_valintep.h"

const D_CHAR db_name[] = "t_baza_date_1";
const D_CHAR tb_name[] = "t_test_tab";

static const D_UINT array_values = 5;

static D_CHAR text1[] = "This is a text!";
static D_CHAR text2[] = "This is should be bigger. It mus have more chars then the first one";
static D_CHAR text3[] = "FirstName LastName";
static D_CHAR text4[] = "Total domination of the world is the goal, but why?\n"
                        "Some one will say because one can, but this is not the case.\b"
                         "I have no idea what I wrote here!\n";


#define INIT_VECTORS  std::vector<DBSBool> vectBool;\
  std::vector<DBSChar> vectChar;\
  std::vector<DBSDate> vectDate;\
  std::vector<DBSDateTime> vectDateTime;\
  std::vector<DBSHiresTime> vectHiResDate;\
  std::vector<DBSUInt8> vectUInt8;\
  std::vector<DBSUInt16> vectUInt16;\
  std::vector<DBSUInt32> vectUInt32;\
  std::vector<DBSUInt64> vectUInt64;\
  std::vector<DBSReal> vectReal;\
  std::vector<DBSRichReal> vectRichReal;\
  std::vector<DBSInt8> vectInt8;\
  std::vector<DBSInt16> vectInt16;\
  std::vector<DBSInt32> vectInt32;\
  std::vector<DBSInt64> vectInt64;\
  std::vector<DBSText> vectText;\
\
  vectBool.push_back (DBSBool(false, true));\
  vectBool.push_back (DBSBool(false, true));\
  vectBool.push_back (DBSBool(false, false));\
  vectBool.push_back (DBSBool(true));\
  vectBool.push_back (DBSBool(false, false));\
  \
  vectChar.push_back (DBSChar (true));\
  vectChar.push_back (DBSChar (false, 'B'));\
  vectChar.push_back (DBSChar (false, 0x2312));\
  vectChar.push_back (DBSChar (false, 'V'));\
  vectChar.push_back (DBSChar (false, '\n'));\
\
  vectDate.push_back (DBSDate (false, 1981, 11, 14));\
  vectDate.push_back (DBSDate (false, 2091, 10, 11));\
  vectDate.push_back (DBSDate (false, 92, 3, 30));\
  vectDate.push_back (DBSDate (true));\
  vectDate.push_back (DBSDate (false, -1, 3, 30));\
\
  vectDateTime.push_back (DBSDateTime (false, 781, 1, 1, 10, 11, 12));\
  vectDateTime.push_back (DBSDateTime (false, 1970, 1, 1, 0, 0, 0));\
  vectDateTime.push_back (DBSDateTime (false, 0, 1, 1, 0, 0, 0));\
  vectDateTime.push_back (DBSDateTime (false, 21, 12, 31, 23, 59, 59));\
  vectDateTime.push_back (DBSDateTime (true));\
\
  vectHiResDate.push_back (DBSHiresTime (false, 1, 1, 1, 0, 0, 0, 2312));\
  vectHiResDate.push_back (DBSHiresTime (true));\
  vectHiResDate.push_back (DBSHiresTime (false, 0, 1, 2, 1, 1, 1, 2132));\
  vectHiResDate.push_back (DBSHiresTime (false, -1324, 12, 31, 23, 59, 59, 9897));\
  vectHiResDate.push_back (DBSHiresTime (true));\
\
  vectUInt8.push_back (DBSUInt8 (true));\
  vectUInt8.push_back (DBSUInt8 (false, 2));\
  vectUInt8.push_back (DBSUInt8 (true));\
  vectUInt8.push_back (DBSUInt8 (false, 0x67));\
  vectUInt8.push_back (DBSUInt8 (false, 2));\
\
  vectUInt16.push_back (DBSUInt16 (false, 0xAABB));\
  vectUInt16.push_back (DBSUInt16 (false,  0xBBAA));\
  vectUInt16.push_back (DBSUInt16 (true));\
  vectUInt16.push_back (DBSUInt16 (false, 0x67));\
  vectUInt16.push_back (DBSUInt16 (false, 0xFF00));\
\
  vectUInt32.push_back (DBSUInt32 (false, 0x0));\
  vectUInt32.push_back (DBSUInt32 (false,  0xCDDEBBAA));\
  vectUInt32.push_back (DBSUInt32 (false, 0x0));\
  vectUInt32.push_back (DBSUInt32 (false, 0xFF0000FF));\
  vectUInt32.push_back (DBSUInt32 (true));\
\
  vectUInt64.push_back (DBSUInt64 (false, ~0x0));\
  vectUInt64.push_back (DBSUInt64 (false,  0x0102030405060708));\
  vectUInt64.push_back (DBSUInt64 (false, 1));\
  vectUInt64.push_back (DBSUInt64 (false, 2));\
  vectUInt64.push_back (DBSUInt64 (true));\
\
  vectInt8.push_back (DBSInt8 (true));\
  vectInt8.push_back (DBSInt8 (false, -2));\
  vectInt8.push_back (DBSInt8 (true));\
  vectInt8.push_back (DBSInt8 (false, -67));\
  vectInt8.push_back (DBSInt8 (false, 2));\
\
  vectInt16.push_back (DBSInt16 (false, -21));\
  vectInt16.push_back (DBSInt16 (false,  21112));\
  vectInt16.push_back (DBSInt16 (true));\
  vectInt16.push_back (DBSInt16 (false, -2312));\
  vectInt16.push_back (DBSInt16 (false, -12));\
\
  vectInt32.push_back (DBSInt32 (false, 0x10));\
  vectInt32.push_back (DBSInt32 (false, 0x80808080));\
  vectInt32.push_back (DBSInt32 (false, -1));\
  vectInt32.push_back (DBSInt32 (false, -2));\
  vectInt32.push_back (DBSInt32 (true));\
\
  vectInt64.push_back (DBSInt64 (false, ~0x0));\
  vectInt64.push_back (DBSInt64 (false, 0x0102030405060708));\
  vectInt64.push_back (DBSInt64 (false, -1));\
  vectInt64.push_back (DBSInt64 (false, 2));\
  vectInt64.push_back (DBSInt64 (true));\
\
  vectReal.push_back (DBSReal (false, -199.21));\
  vectReal.push_back (DBSReal (false, 199.21));\
  vectReal.push_back (DBSReal (true));\
  vectReal.push_back (DBSReal (false, 0.98989));\
  vectReal.push_back (DBSReal (false, 98989.0));\
\
  vectRichReal.push_back (DBSRichReal (false, 1.0));\
  vectRichReal.push_back (DBSRichReal (false, 1234.91823412));\
  vectRichReal.push_back (DBSRichReal (false, 3.141592653589793));\
  vectRichReal.push_back (DBSRichReal (false, -1.6180339887498948482));\
  vectRichReal.push_back (DBSRichReal (true));\
\
  vectText.push_back (DBSText (_RC(D_UINT8*, text1)));\
  vectText.push_back (DBSText (NULL));\
  vectText.push_back (DBSText (_RC(D_UINT8*, text2)));\
  vectText.push_back (DBSText (_RC(D_UINT8*, text3)));\
  vectText.push_back (DBSText (_RC(D_UINT8*, text4)));


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
  vectInt16.clear ();\
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

static const D_UINT FIXED_DESC_FIELDS_COUNT = 15;
static const D_UINT VAR_DESC_FIELDS_COUNT = 16;
static const D_UINT TOTAL_DESC_FIELDS_COUNT = 31;

template <class T> D_UINT
add_fixed_values_vector_to_table (I_DBSTable &table, std::vector<T> &vectValues)
{
  D_UINT fieldIndex = 0;

  while (fieldIndex < table.GetFieldsCount())
    {
      DBSFieldDescriptor desc = table.GetFieldDescriptor (fieldIndex);
      if ((desc.m_FieldType == vectValues[0]) && (desc.isArray == false))
        break;

      ++fieldIndex;
    }

  if (fieldIndex >= table.GetFieldsCount())
    return ~0;


  for (D_UINT index = 0; index < vectValues.size(); ++index)
    {
      if (table.GetAllocatedRows() <= index)
        table.AddRow();

      table.SetEntry (vectValues[index], index, fieldIndex);
    }

  return fieldIndex;
}

template <class T> bool
test_fixed_values_vector (I_DBSTable &table, const D_UINT fieldIndex, std::vector<T> &vectValues)
{
  T fieldValue (true);

  for (D_UINT rowIndex = 0; rowIndex < table.GetAllocatedRows(); ++rowIndex)
    {
      table.GetEntry(fieldValue, rowIndex, fieldIndex);
      if (! (fieldValue == vectValues[rowIndex]))
        return false;
    }

  return true;
}

template <class T> bool
test_fixed_values_vector_reverse (I_DBSTable &table,
                                  const D_UINT fieldIndex,
                                  std::vector<T> &vectValues)
{
  T fieldValue (true);
  D_UINT rowIndex = table.GetAllocatedRows();

  do
    {
      rowIndex--;
      table.GetEntry(fieldValue, rowIndex, fieldIndex);
      if (! (fieldValue == vectValues[rowIndex]))
        return false;
    }
  while (rowIndex > 0);

  return true;
}

template <class T> bool
test_fixed_value_field (I_DBSHandler &rDbs, std::vector<T> &valuesVect)
{

  D_UINT fieldIndex = ~0;
  bool   result = true;

  {
    I_DBSTable &table = rDbs.RetrievePersistentTable(tb_name);

    fieldIndex = add_fixed_values_vector_to_table (table, valuesVect);

    if (fieldIndex == _SC(D_UINT, ~0))
      result = false;


    if (result && test_fixed_values_vector (table, fieldIndex, valuesVect) == false)
      result = false;

    rDbs.ReleaseTable (table);
  }

  if (result)
  {
    I_DBSTable &table = rDbs.RetrievePersistentTable(tb_name);

    if (test_fixed_values_vector_reverse (table, fieldIndex, valuesVect) == false)
      result = false;

    rDbs.ReleaseTable (table);
  }

  return result;
}

bool
test_fixed_values_table (I_DBSHandler &rDbs)
{
  std::cout << "Testing fixed values ... ";
  bool result = true;

  INIT_VECTORS;

  rDbs.AddTable (tb_name, field_descs, FIXED_DESC_FIELDS_COUNT);

  result = result && test_fixed_value_field (rDbs, vectBool);
  result = result && test_fixed_value_field (rDbs, vectChar);
  result = result && test_fixed_value_field (rDbs, vectDate);
  result = result && test_fixed_value_field (rDbs, vectDateTime);
  result = result && test_fixed_value_field (rDbs, vectHiResDate);
  result = result && test_fixed_value_field (rDbs, vectUInt8);
  result = result && test_fixed_value_field (rDbs, vectUInt16);
  result = result && test_fixed_value_field (rDbs, vectUInt32);
  result = result && test_fixed_value_field (rDbs, vectUInt64);

  result = result && test_fixed_value_field (rDbs, vectReal);
  result = result && test_fixed_value_field (rDbs, vectRichReal);

  result = result && test_fixed_value_field (rDbs, vectInt8);
  result = result && test_fixed_value_field (rDbs, vectInt16);
  result = result && test_fixed_value_field (rDbs, vectInt32);
  result = result && test_fixed_value_field (rDbs, vectInt64);

  rDbs.DeleteTable (tb_name);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

template <class T> bool
check_value_array (std::vector<T> &source, DBSArray &array, const D_UINT seed)
{
  if (array.GetElementsType() != source[0])
    return false;

  D_UINT arrayIndex = 0;

  for (D_UINT elIndex = 0; elIndex < source.size (); ++elIndex)
    {
      T testElement (true);

      D_UINT64 testIndex = (elIndex + seed) % source.size ();
      if (source[testIndex] == T(true))
        continue;

      array.GetElement (testElement, arrayIndex);

      if (testElement == source [testIndex])
        ++arrayIndex;
      else
        return false;
    }

  return true;
}

template <class T> bool
fill_value_array (std::vector<T> &source, DBSArray &array, const D_UINT seed)
{
  if (array.GetElementsType() != source[0])
    return false;

  for (D_UINT elIndex = 0; elIndex < source.size (); ++elIndex)
    {
      D_UINT64 testIndex = (elIndex + seed) % source.size ();
      if (source[testIndex] == T(true))
        continue;

      array.AddElement (source[testIndex]);
    }
  return check_value_array (source, array, seed);
}

template <class T> D_UINT
add_vectors_values_to_table (I_DBSTable &table, std::vector<T> &testVect)
{
  D_UINT fieldIndex = 0;

  for (; fieldIndex < table.GetFieldsCount(); ++fieldIndex)
    {
      struct DBSFieldDescriptor fieldDesc = table.GetFieldDescriptor (fieldIndex);
      if (fieldDesc.isArray && (fieldDesc.m_FieldType == testVect[0]))
        break;
    }

  if (fieldIndex >= table.GetFieldsCount())
    return ~0;


  for (D_UINT rowIndex = 0; rowIndex < testVect.size (); ++rowIndex)
    {
      DBSArray valueArray (_SC(T*, NULL));

      if (rowIndex == table.GetAllocatedRows())
        table.AddRow();

      if (fill_value_array (testVect, valueArray, rowIndex) == false)
        return ~0;

      table.SetEntry (valueArray, rowIndex, fieldIndex);
    }

  return fieldIndex;
}

template <class T> bool
test_vector_values_table (I_DBSTable &table, D_UINT fieldIndex, std::vector<T> &testVect)
{
  for (D_UINT rowIndex = 0; rowIndex < table.GetAllocatedRows(); ++rowIndex)
    {

      DBSArray testArray (_SC(T*, NULL));
      table.GetEntry (testArray, rowIndex, fieldIndex);

      if (testVect[rowIndex].IsNull ())
        {
          if (testArray.IsNull() != false)
            return false;
        }
      else if (check_value_array (testVect, testArray, rowIndex) == false)
        return false;
    }

  return true;
}

template <class T> bool
test_variable_field_array (I_DBSHandler &rDbs, std::vector<T> &testVect)
{

  D_UINT fieldIndex = ~0;
  bool   result = true;

  {
    I_DBSTable &table = rDbs.RetrievePersistentTable(tb_name);

    fieldIndex = add_vectors_values_to_table (table, testVect);

    if (fieldIndex == _SC(D_UINT, ~0))
      result = false;


    if (result && test_vector_values_table (table, fieldIndex, testVect) == false)
      result = false;

    rDbs.ReleaseTable (table);
  }

  if (result)
  {
    I_DBSTable &table = rDbs.RetrievePersistentTable(tb_name);

    if (test_vector_values_table (table, fieldIndex, testVect) == false)
      result = false;

    rDbs.ReleaseTable (table);
  }

  return result;
}

bool
test_text_value_table (I_DBSHandler &rDbs, std::vector<DBSText> &vectText)
{
  D_UINT fieldIndex = 0;
  bool result = true;

  {
    I_DBSTable &table = rDbs.RetrievePersistentTable(tb_name);
    for (; fieldIndex < table.GetFieldsCount (); ++ fieldIndex)
      {
        const DBSFieldDescriptor fieldDesc = table.GetFieldDescriptor(fieldIndex);
        if (fieldDesc.m_FieldType == T_TEXT)
          {
            assert (fieldDesc.isArray == false);
            break;
          }
      }

    if (fieldIndex >= table.GetFieldsCount())
      result = false;

    rDbs.ReleaseTable (table);
  }

  if (result)
    {      I_DBSTable &table = rDbs.RetrievePersistentTable(tb_name);

      for (D_UINT rowIndex = 0; rowIndex < vectText.size(); ++rowIndex)
        {
          if (rowIndex >= table.GetAllocatedRows())
            table.AddRow();
          table.SetEntry (vectText[rowIndex], rowIndex, fieldIndex);
        }

      for (D_UINT rowIndex = 0; rowIndex < vectText.size(); ++rowIndex)
        {
          DBSText testValue(NULL);
          table.GetEntry (testValue, rowIndex, fieldIndex);
          if (! (testValue == vectText[rowIndex]))
            {
              result = false;
              break;
            }
        }

      rDbs.ReleaseTable (table);
    }

  if (result)
    {
      I_DBSTable &table = rDbs.RetrievePersistentTable(tb_name);

      for (D_UINT rowIndex = table.GetAllocatedRows(); rowIndex > 0; )
        {
          --rowIndex;
          DBSText testValue(NULL);
          table.GetEntry (testValue, rowIndex, fieldIndex);
          if (! (testValue == vectText[rowIndex]))
            {
              result = false;
              break;
            }
        }
      rDbs.ReleaseTable (table);
    }

  return result;
}

bool
test_variable_values_table (I_DBSHandler &rDbs)
{
  std::cout << "Testing variable values ... ";
  bool result = true;

  INIT_VECTORS;

  rDbs.AddTable (tb_name, field_descs + FIXED_DESC_FIELDS_COUNT, VAR_DESC_FIELDS_COUNT);

  result = result && test_variable_field_array (rDbs, vectBool);
  result = result && test_variable_field_array (rDbs, vectChar);
  result = result && test_variable_field_array (rDbs, vectDate);
  result = result && test_variable_field_array (rDbs, vectDateTime);
  result = result && test_variable_field_array (rDbs, vectHiResDate);
  result = result && test_variable_field_array (rDbs, vectUInt8);
  result = result && test_variable_field_array (rDbs, vectUInt16);
  result = result && test_variable_field_array (rDbs, vectUInt32);
  result = result && test_variable_field_array (rDbs, vectUInt64);
  result = result && test_variable_field_array (rDbs, vectInt8);
  result = result && test_variable_field_array (rDbs, vectInt16);
  result = result && test_variable_field_array (rDbs, vectInt32);
  result = result && test_variable_field_array (rDbs, vectInt64);
  result = result && test_variable_field_array (rDbs, vectReal);
  result = result && test_variable_field_array (rDbs, vectRichReal);

  //Special case for DBSText!
  result = result && test_text_value_table (rDbs, vectText);


  rDbs.DeleteTable (tb_name);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_full_value_table (I_DBSHandler &rDbs)
{
  std::cout << "Testing all values ... ";
  bool result = true;

  INIT_VECTORS;

  rDbs.AddTable (tb_name, field_descs, TOTAL_DESC_FIELDS_COUNT);

  result = result && test_fixed_value_field (rDbs, vectBool);
  result = result && test_fixed_value_field (rDbs, vectChar);
  result = result && test_fixed_value_field (rDbs, vectDate);
  result = result && test_fixed_value_field (rDbs, vectDateTime);
  result = result && test_fixed_value_field (rDbs, vectHiResDate);
  result = result && test_fixed_value_field (rDbs, vectUInt8);
  result = result && test_fixed_value_field (rDbs, vectUInt16);
  result = result && test_fixed_value_field (rDbs, vectUInt32);
  result = result && test_fixed_value_field (rDbs, vectUInt64);

  result = result && test_variable_field_array (rDbs, vectBool);
  result = result && test_variable_field_array (rDbs, vectChar);
  result = result && test_variable_field_array (rDbs, vectDate);


  result = result && test_fixed_value_field (rDbs, vectReal);
  result = result && test_fixed_value_field (rDbs, vectRichReal);

  result = result && test_variable_field_array (rDbs, vectDateTime);
  result = result && test_variable_field_array (rDbs, vectHiResDate);
  result = result && test_variable_field_array (rDbs, vectUInt8);
  result = result && test_variable_field_array (rDbs, vectUInt16);
  result = result && test_variable_field_array (rDbs, vectUInt32);
  result = result && test_variable_field_array (rDbs, vectUInt64);
  result = result && test_variable_field_array (rDbs, vectInt8);
  result = result && test_variable_field_array (rDbs, vectInt16);

  result = result && test_fixed_value_field (rDbs, vectInt8);
  result = result && test_fixed_value_field (rDbs, vectInt16);
  result = result && test_fixed_value_field (rDbs, vectInt32);
  result = result && test_fixed_value_field (rDbs, vectInt64);


  result = result && test_variable_field_array (rDbs, vectInt32);
  result = result && test_variable_field_array (rDbs, vectInt64);
  result = result && test_variable_field_array (rDbs, vectReal);
  result = result && test_variable_field_array (rDbs, vectRichReal);

  //Special case for DBSText!
  result = result && test_text_value_table (rDbs, vectText);


  rDbs.DeleteTable (tb_name);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialised
  // We need not to test against it!

  D_UINT prealloc_mem = test_get_mem_used ();
  bool success = true;

  {
    std::string dir = ".";
    dir += whc_get_directory_delimiter ();

    DBSInit (dir.c_str (), dir.c_str ());
  }


  DBSCreateDatabase (db_name);
  I_DBSHandler & handler = DBSRetrieveDatabase (db_name);

  success = success && test_fixed_values_table (handler);
  success = success && test_variable_values_table (handler);
  success = success && test_full_value_table (handler);

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
