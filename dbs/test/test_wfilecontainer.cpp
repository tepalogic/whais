/*
 * test_wfilecontainer.cpp
 *
 *  Created on: May 22, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <memory.h>
#include <iostream>

#include "dbs/dbs_mgr.h"
#include "utils/wrandom.h"

#include "custom/include/test/test_fmw.h"
#include "../pastra/ps_container.h"

using namespace whisper;
using namespace pastra;

uint8_t buffer[1468];
const char fileName[] = "./test_file.tm";


static bool
create_container (uint_t max_file_size, const uint_t container_size)
{
  FileContainer container (fileName, max_file_size, 0);
  uint8_t marker = 0;
  uint64_t current_pos = 0;
  uint_t left_to_write = container_size;

  while (left_to_write > 0)
    {
      uint_t write_size = MIN (sizeof (buffer), left_to_write);

      memset (buffer, marker, sizeof buffer);
      container.Write (current_pos, write_size, buffer);
      marker = (marker + 1) & 0xFF;
      left_to_write -= write_size;
      current_pos += write_size;
    }
  return container.Size () == container_size;
}

static bool
check_container (uint_t max_file_size,
                 const uint_t container_size,
                 const uint8_t marker_start, const uint8_t marker_increment)
{
  FileContainer container (fileName, max_file_size,
                           (container_size / max_file_size) + 1);
  uint64_t current_pos = 0;
  uint8_t marker = marker_start;
  uint_t left_to_read = container_size;

  if (container.Size () != container_size)
    return false;

  while (left_to_read > 0)
    {
      uint_t read_size = MIN (sizeof (buffer), left_to_read);
      container.Read (current_pos, read_size, buffer);

      for (uint_t index = 0; index < read_size; ++index)
        if (buffer[index] != marker)
          return false;
      marker = (marker + marker_increment) & 0xFF;
      left_to_read -= read_size;
      current_pos += read_size;
    }
  return true;
}

static uint_t
colapse_container (uint_t max_file_size, const uint_t container_size)
{
  uint64_t current_pos = 0;
  FileContainer container (fileName, max_file_size,
                           (container_size / max_file_size) + 1);
  uint64_t new_container_size = container.Size ();

  if (new_container_size != container_size)
    return false;

  while (current_pos < container.Size ())
    {
      uint64_t to_delete = MIN (sizeof buffer,
                                container.Size () - current_pos);
      container.Colapse (current_pos, current_pos + to_delete);
      current_pos += to_delete;
      new_container_size -= to_delete;
    }
  return new_container_size;
}

static bool
check_temp_container (uint_t uTestContainerSize)
{
  const uint_t storeSize = wh_rnd () % 761 + 761;
  const uint_t uStepSize = wh_rnd () % 100 + 1;

  TemporalContainer container (storeSize);

  uint8_t marker = 0;
  uint64_t current_pos = 0;
  uint_t left_to_write = uTestContainerSize;

  while (left_to_write > 0)
    {
      uint_t write_size = MIN (uStepSize, left_to_write);

      memset (buffer, marker, sizeof buffer);
      container.Write (current_pos, write_size, buffer);
      marker = (marker + 1) & 0xFF;
      left_to_write -= write_size;
      current_pos += write_size;
    }

  if (container.Size () != uTestContainerSize)
    return false;

  container.Colapse (uTestContainerSize - uStepSize,
                            uTestContainerSize);

  if (container.Size () != (uTestContainerSize - uStepSize))
    return false;

  container.Colapse (0, uStepSize);

  if (container.Size () != (uTestContainerSize - 2 * uStepSize))
    return false;

  uint_t left_to_read = container.Size ();
  marker = 1;
  current_pos = 0;

  while (left_to_read > 0)
    {
      uint_t read_size = MIN (uStepSize, left_to_read);
      container.Read (current_pos, read_size, buffer);

      for (uint_t index = 0; index < read_size; ++index)
        if (buffer[index] != marker)
          return false;
      marker = (marker + 1) & 0xFF;
      left_to_read -= read_size;
      current_pos += read_size;
    }

  return true;
}



int
main ()
{
  bool success = true;

  {
    DBSInit (DBSSettings ());
  }

  const uint_t max_file_size = 512 * 1024;
  const uint_t container_size = max_file_size * 4 + 345;

  if (!create_container (max_file_size, container_size))
    success = false;

  if (!check_container (max_file_size, container_size, 0, 1))
    success = false;

  uint64_t new_container_size = colapse_container (max_file_size,
                                container_size);
  if ((new_container_size <= 0) || (new_container_size % sizeof buffer) != 0)
    success = false;

  if (!check_container (max_file_size, new_container_size, 1, 2))
    success = false;

  //delete container
  while (new_container_size > 0)
    new_container_size = colapse_container (max_file_size,
                                            new_container_size);

  if (!check_temp_container (760))
    success = false;

  if (!check_temp_container (678109))
    success = false;

  if (!check_temp_container (3412))
    success = false;

  if (!check_temp_container (23400))
    success = false;

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
