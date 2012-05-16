/*
 * test_varstore.cpp
 *
 *  Created on: Oct 9, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <iostream>
#include <string.h>

#include "test/test_fmw.h"


#include "../include/dbs_mgr.h"
#include "../include/dbs_exception.h"
#include "../include/dbs_values.h"

#include "../pastra/ps_varstorage.h"
#include "../pastra/ps_textstrategy.h"

using namespace pastra;

static D_UINT8 pattern1[48];
static D_UINT8 pattern2[503];
static D_UINT8 pattern3[0x1001F7];

static D_UINT64 firstEntries[3];

#define TEST_UNIT_MAX_SIZE              105000

void init_pattern (D_UINT8 *pattern, D_UINT size, D_UINT8 seed)
{
  for (D_UINT index = 0; index < size; ++index, ++seed)
    pattern[index] = seed;
}

bool test_pattern (D_UINT8 *pattern, D_UINT size, D_UINT8 seed)
{
  for (D_UINT index = 0; index < size; ++index, ++seed)
    if (pattern[index] != seed)
      return false;

  return true;
}

bool test_record (VLVarsStore* storage,
                  D_UINT8*const pattern,
                  D_UINT seed,
                  D_UINT firstEntry,
                  D_UINT size)
{
  init_pattern (pattern, size, seed + 11);
  storage->GetRecord (firstEntry, 0, size, pattern);

  return test_pattern (pattern, size, seed);
}


bool test_record_create ()
{
  bool result = true;


  std::cout << "Testing record creation ... ";

  {
    std::string temp_file_base = DBSGetWorkingDir();
    temp_file_base += "t_ps_varstore";

    VLVarsStore storage;
    storage.Init(temp_file_base.c_str(), 0, TEST_UNIT_MAX_SIZE);

    init_pattern (pattern1, sizeof pattern1, 31);
    firstEntries[0] = storage.AddRecord (pattern1, sizeof pattern1);

    if (test_record (&storage, pattern1, 31, firstEntries[0], sizeof pattern1) == false)
      result = false;

    if (result)
      {
        init_pattern (pattern2, sizeof pattern2, 41);
        firstEntries[1] = storage.AddRecord (pattern2, sizeof pattern2);

        if (test_record (&storage, pattern1, 31, firstEntries[0], sizeof pattern1) == false)
          result = false;
        else if (test_record (&storage, pattern2, 41, firstEntries[1], sizeof pattern2) == false)
          result = false;
      }

    if (result)
      {
        init_pattern (pattern3, sizeof pattern3, 61);
        firstEntries[2] = storage.AddRecord (pattern3, sizeof pattern3);

        if (test_record (&storage, pattern1, 31, firstEntries[0], sizeof pattern1) == false)
          result = false;
        else if (test_record (&storage, pattern2, 41, firstEntries[1], sizeof pattern2) == false)
          result = false;
        else if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
          result = false;
      }

    storage.Flush ();
  }

  if (result)
    {
      std::string temp_file_base = DBSGetWorkingDir();
      temp_file_base += "t_ps_varstore";

      D_UINT storageSize = sizeof pattern1 + sizeof pattern2 + sizeof pattern3;

      storageSize = ((storageSize + 47 ) /48) * 64;

      VLVarsStore storage;
      storage.Init(temp_file_base.c_str(), storageSize, TEST_UNIT_MAX_SIZE);

      if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
        result = false;
      else if (test_record (&storage, pattern1, 31, firstEntries[0], sizeof pattern1) == false)
        result = false;
      else if (test_record (&storage, pattern2, 41, firstEntries[1], sizeof pattern2) == false)
        result = false;

      storage.Flush ();
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}


bool
test_record_removal ()
{
  bool result = true;

  std::cout << "Testing record removal ... ";

  {
    std::string temp_file_base = DBSGetWorkingDir();
    temp_file_base += "t_ps_varstore";

    D_UINT storageSize = sizeof pattern1 + sizeof pattern2 + sizeof pattern3;

    storageSize = ((storageSize + 47 ) /48) * 64;

    VLVarsStore storage;
    storage.Init(temp_file_base.c_str(), storageSize, TEST_UNIT_MAX_SIZE);

    if (result)
      {
        storage.DecrementRecordRef (firstEntries[1]);

        if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
          result = false;
        else if (test_record (&storage, pattern1, 31, firstEntries[0], sizeof pattern1) == false)
          result = false;
      }

    if (result)
      {
        storage.DecrementRecordRef (firstEntries[0]);

        if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
          result = false;
      }

    storage.Flush ();
  }

  if (result)
    {
      std::string temp_file_base = DBSGetWorkingDir();
      temp_file_base += "t_ps_varstore";

      D_UINT storageSize = sizeof pattern1 + sizeof pattern2 + sizeof pattern3;

      storageSize = ((storageSize + 47 ) /48) * 64;

      VLVarsStore storage;
      storage.Init(temp_file_base.c_str(), storageSize, TEST_UNIT_MAX_SIZE);

      if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
        result = false;

      storage.Flush ();
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool test_record_update ()
{
  bool result = true;

  std::cout << "Testing record update ... ";

  {
    std::string temp_file_base = DBSGetWorkingDir();
    temp_file_base += "t_ps_varstore";

    D_UINT storageSize = sizeof pattern1 + sizeof pattern2 + sizeof pattern3;

    storageSize = ((storageSize + 47 ) /48) * 64;

    VLVarsStore storage;
    storage.Init(temp_file_base.c_str(), storageSize, TEST_UNIT_MAX_SIZE);

    if (result)
      {

        firstEntries[0] = storage.AddRecord (NULL, 0);
        init_pattern (pattern1, sizeof pattern1, 58);
        storage.UpdateRecord (firstEntries[0], 0, sizeof pattern1, pattern1);

        if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
          result = false;
        else if (test_record (&storage, pattern1, 58, firstEntries[0], sizeof pattern1) == false)
          result = false;
      }

    if (result)
      {
        init_pattern (pattern2, sizeof pattern2, 11);
        storage.UpdateRecord (firstEntries[0], 0, sizeof pattern2, pattern2);

        if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
          result = false;
        else if (test_record (&storage, pattern2, 11, firstEntries[0], sizeof pattern2) == false)
          result = false;
      }

    storage.Flush ();
  }

  if (result)
    {
      std::string temp_file_base = DBSGetWorkingDir();
      temp_file_base += "t_ps_varstore";

      D_UINT storageSize = sizeof pattern1 + sizeof pattern2 + sizeof pattern3;

      storageSize = ((storageSize + 47 ) /48) * 64;

      VLVarsStore storage;
      storage.Init(temp_file_base.c_str(), storageSize, TEST_UNIT_MAX_SIZE);
      storage.MarkForRemoval ();

      init_pattern (pattern3, sizeof pattern3, 21);
      storage.UpdateRecord (firstEntries[0], 0, sizeof pattern3, pattern3);

      if (test_record (&storage, pattern3, 61, firstEntries[2], sizeof pattern3) == false)
        result = false;
      else if (test_record (&storage, pattern3, 21, firstEntries[0], sizeof pattern3) == false)
        result = false;

      storage.Flush ();
    }

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}



static D_UINT8 testBuffer[128] = "This is a test! Let us hope it will be big enough to keep a large"
                                 " text (e.g. at least the size of a 48 byte :)))";

bool
test_record_container_update ()
{

  bool result = true;
  std::cout << "Testing record with container update ... ";

  do
   {
    std::string temp_file_base = DBSGetWorkingDir();
    temp_file_base += "t_ps_varstore";

    TempContainer container (DBSGetTempDir(), 1024);
    container.Write (0, sizeof testBuffer, testBuffer);

    VLVarsStore storage;
    storage.Init(temp_file_base.c_str(), 0, TEST_UNIT_MAX_SIZE);
    storage.MarkForRemoval ();

    const D_UINT64 entry = storage.AddRecord (container,  0, 0);

    if (entry == 0)
      {
        result = false;
        break;
      }
    storage.UpdateRecord (entry, 0, container, 0, container.Size());
    D_UINT8 temp[128];
    assert (sizeof temp == sizeof testBuffer);

    memset (temp, 0xFF, sizeof temp);
    storage.GetRecord(entry, 0, container.Size(), temp);
    if ((memcmp(temp, testBuffer, sizeof temp) != 0) ||
        (sizeof temp  != container.Size()))
      {
        result = false;
        break;
      }

    storage.UpdateRecord (entry, sizeof testBuffer, container, 0, container.Size());
    memset (temp, 0xFF, sizeof temp);
    storage.GetRecord(entry, 0, container.Size(), temp);
    if ((memcmp(temp, testBuffer, sizeof temp) != 0) ||
        (sizeof temp  != container.Size()))
      {
        result = false;
        break;
      }

    memset (temp, 0xFF, sizeof temp);
    storage.GetRecord(entry, sizeof temp, sizeof temp, temp);
    if (memcmp(temp, testBuffer, sizeof temp) != 0)
      {
        result = false;
        break;
      }

    storage.Flush ();
   }
  while (0);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}

bool
test_record_record_update ()
{

  bool result = true;
  std::cout << "Testing record with other record update ... ";

  do
   {
    std::string temp_file_base = DBSGetWorkingDir();
    temp_file_base += "t_ps_varstore";

    TempContainer container (DBSGetTempDir(), 1024);
    container.Write (0, sizeof testBuffer, testBuffer);

    VLVarsStore storage;
    storage.Init(temp_file_base.c_str(), 0, TEST_UNIT_MAX_SIZE);
    storage.MarkForRemoval ();

    const D_UINT64 entry = storage.AddRecord (container,  0, container.Size());

    if (entry == 0)
      {
        result = false;
        break;
      }
    D_UINT8 temp[128];
    assert (sizeof temp == sizeof testBuffer);

    memset (temp, 0xFF, sizeof temp);
    storage.GetRecord(entry, 0, container.Size(), temp);
    if ((memcmp(temp, testBuffer, sizeof temp) != 0) ||
        (sizeof temp  != container.Size()))
      {
        result = false;
        break;
      }

    storage.UpdateRecord (entry, sizeof testBuffer, storage, entry, 0, container.Size());
    memset (temp, 0xFF, sizeof temp);
    storage.GetRecord(entry, 0, container.Size(), temp);
    if ((memcmp(temp, testBuffer, sizeof temp) != 0) ||
        (sizeof temp  != container.Size()))
      {
        result = false;
        break;
      }

    memset (temp, 0xFF, sizeof temp);
    storage.GetRecord(entry, sizeof temp, sizeof temp, temp);
    if (memcmp(temp, testBuffer, sizeof temp) != 0)
      {
        result = false;
        break;
      }

    storage.Flush ();
   }
  while (0);

  std::cout << ( result ? "OK" : "FALSE") << std::endl;
  return result;
}



int
main ()
{
  // VC++ allocates memory when the C++ runtime is initialized
  // We need not to test against it!
  D_UINT prealloc_mem = test_get_mem_used ();
  bool success = true;

  {
    std::string dir = ".";
    dir += whc_get_directory_delimiter ();

    DBSInit (dir.c_str (), dir.c_str ());
  }

  success = success && test_record_create ();
  success = success && test_record_removal ();
  success = success && test_record_update ();
  success = success && test_record_container_update ();
  success = success && test_record_record_update ();

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
  std::cout << "Current memory usage: " << mem_usage << " bytes." << std::endl;
  if (!success)
    {
      std::cout << "TEST RESULT: FAIL" << std::endl;
      return -1;
    }
  else
    std::cout << "TEST RESULT: PASS" << std::endl;

  return 0;
}

