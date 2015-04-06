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


bool
operator!= (const DBSFieldDescriptor& field_1,
            const DBSFieldDescriptor& field_2)
{
  return (field_1.type != field_2.type) ||
         (field_1.isArray != field_2.isArray) ||
         (strcmp (field_1.name, field_2.name) != 0);
}

bool
test_for_no_args (IDBSHandler& rDbs)
{
  bool result = false;

  std::cout << "Test for invalid argumetns ... ";

  try
  {
    rDbs.AddTable ("test_dummy", 10, NULL);
  }
  catch (DBSException& e)
  {
    if (e.Code () == DBSException::INVALID_PARAMETERS)
      result = true;
  }

  try
  {
    DBSFieldDescriptor temp;
    temp.name = "dummy";
    temp.type = T_BOOL;
    temp.isArray = false;

    if (result)
      rDbs.AddTable ("test_dummy", 0, &temp);
    result = false;
  }
  catch (DBSException& e)
  {
    if (e.Code () == DBSException::INVALID_PARAMETERS)
      result = true;
    else
      result = false;
  }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_for_invalid_fields (IDBSHandler& rDbs)
{
  bool result = false;
  DBSFieldDescriptor temp;

  std::cout << "Test for invalid fields ... ";

  temp.type = _SC (DBS_FIELD_TYPE, 78);
  temp.isArray = false;
  temp.name = "good_name";

  try
  {
    rDbs.AddTable ("test_dummy", 1, &temp);
  }
  catch (DBSException& e)
  {
    if (e.Code () == DBSException::FIELD_TYPE_INVALID)
      result = true;
  }

  temp.name = "1bad_name?";
  temp.type = T_TEXT;

  try
  {
    if (result )
      rDbs.AddTable ("test_dummy", 1, &temp);
    result = false;
  }
  catch (DBSException& e)
  {
    if (e.Code () == DBSException::FIELD_NAME_INVALID)
      result = true;
    else
      result = false;
  }

  try
  {
    DBSFieldDescriptor more_temps[3];

    temp.name = "field_1";
    more_temps[0] = temp;
    temp.name = "field_2";
    more_temps[1] = temp;
    temp.name = "field_1";
    more_temps[2] = temp;

    if (result )
      rDbs.AddTable ("test_dummy", 3, more_temps);
    result = false;
  }
  catch (DBSException& e)
  {
    if (e.Code () == DBSException::FIELD_NAME_INVALID)
      result = true;
    else
      result = false;
  }


  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_for_one_field (IDBSHandler& rDbs)
{
  bool result = true;

  std::cout << "Test with one field ... ";

  DBSFieldDescriptor temp;
  temp.name = "dummy";
  temp.type = T_INT16;
  temp.isArray = false;

  rDbs.AddTable ("t_test_tab", 1, &temp);
  ITable& table = rDbs.RetrievePersistentTable ("t_test_tab");

  uint_t rowSize = (_RC (pastra::PrototypeTable &, table)).RowSize ();

  //Check if we added the byte to keep the null bit.
  if (rowSize != (pastra::Serializer::Size (T_INT16, false) + 1))
    result = false;

  rDbs.ReleaseTable (table);
  rDbs.DeleteTable ("t_test_tab");

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

struct StorageInterval
  {
    StorageInterval (uint32_t begin_byte, uint32_t end_byte) :
        mBegin (begin_byte),
        mEnd (end_byte)
    {}

    uint32_t mBegin;
    uint32_t mEnd;

  };

bool
test_for_fields (IDBSHandler& rDbs,
                 DBSFieldDescriptor* pDesc,
                 const uint32_t fieldsCount)
{
  bool result = true;

  std::cout << "Test with fields with count " << fieldsCount << " ... ";

  rDbs.AddTable ("t_test_tab", fieldsCount, pDesc);
  ITable& table = rDbs.RetrievePersistentTable ("t_test_tab");

  std::vector<uint32_t > nullPositions;
  std::vector<StorageInterval> storage;

  if (table.FieldsCount () != fieldsCount)
    result = false;

  for (uint32_t fieldIndex = 0;
       result && (fieldIndex < fieldsCount);
      ++fieldIndex)
    {
      FieldDescriptor& descr = _SC(PrototypeTable&, table).GetFieldDescriptorInternal (fieldIndex);

      if (descr.NullBitIndex () != fieldIndex)
        {
          result = false;
          break;
        }

      nullPositions.push_back (descr.NullBitIndex ());

      if (! result)
        break;

      uint_t elem_start = descr.RowDataOff ();
      uint_t elem_end = elem_start +
          Serializer::Size (_SC (DBS_FIELD_TYPE, descr.Type () & PS_TABLE_FIELD_TYPE_MASK),
                            (descr.Type () & PS_TABLE_ARRAY_MASK) != 0);

      for (uint_t index = 0; index < storage.size (); ++index)
        {
          if ( ((storage[index].mBegin >= elem_start) &&
              (elem_start <= storage[index].mEnd))
              || ((storage[index].mBegin >= elem_end) &&
                  (elem_end <= storage[index].mEnd))
              || (elem_start >= elem_end)
              || (storage[index].mBegin <= descr.NullBitIndex () / 8))
            {
              result = false;
              break;
            }
        }
      storage.push_back (StorageInterval (elem_start, elem_end - 1));

      if (result)
        {
          DBSFieldDescriptor desc = table.DescribeField (fieldIndex);
          uint_t array_index = desc.name[0] - 'a';
          if (desc != pDesc[array_index])
            {
              result = false;
              break;
            }
        }

    }

  rDbs.ReleaseTable (table);
  rDbs.DeleteTable ("t_test_tab");

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


struct DBSFieldDescriptor int_descs[] = {
    {"a", T_UINT8, false},
    {"b", T_UINT16, false},
    {"c", T_UINT32, false},
    {"d", T_UINT64, false},
    {"e", T_INT64, false},
    {"f", T_INT32, false},
    {"g", T_INT16, false},
    {"h", T_INT8, false}
};

struct DBSFieldDescriptor non_int_descs[] = {
    {"a", T_DATE, false},
    {"b", T_DATETIME, false},
    {"c", T_HIRESTIME, false},
    {"d", T_REAL, false},
    {"e", T_RICHREAL, false},
    {"f", T_BOOL, false},
    {"g", T_TEXT, false},
};

struct DBSFieldDescriptor alt_descs[] = {
    {"a", T_UINT8, false},
    {"b", T_UINT16, false},
    {"c", T_UINT32, false},
    {"d", T_UINT64, false},
    {"e", T_INT64, true},
    {"f", T_INT32, false},
    {"g", T_INT16, true},
    {"h", T_INT8, true},
    {"i", T_DATE, false},
    {"j", T_DATETIME, true},
    {"k", T_HIRESTIME, false},
    {"l", T_REAL, false},
    {"m", T_RICHREAL, true},
    {"n", T_BOOL, false},
    {"o", T_TEXT, false},
    {"p", T_TEXT, false}
};

const char db_name[] = "t_baza_date_1";
int
main ()
{
  bool success = true;
  {
    DBSInit (DBSSettings ());
    DBSCreateDatabase (db_name);
  }

  IDBSHandler& handler = DBSRetrieveDatabase (db_name);

  success = test_for_no_args (handler);
  success = success && test_for_invalid_fields (handler);
  success = success && test_for_one_field (handler);
  success = success && test_for_fields (handler, int_descs,
                                        sizeof (int_descs) / sizeof (int_descs[0]));
  success = success && test_for_fields (handler,
                                        non_int_descs, sizeof (non_int_descs) / sizeof (non_int_descs[0]));
  success = success && test_for_fields (handler,
                                        alt_descs, sizeof (alt_descs) / sizeof (alt_descs[0]));

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
