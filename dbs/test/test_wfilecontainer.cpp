/*
 * test_wfilecontainer.cpp
 *
 *  Created on: May 22, 2011
 *      Author: iupo
 */

#include <assert.h>
#include <memory.h>
#include <iostream>

#include "dbs_mgr.h"

#include "test/test_fmw.h"
#include "../pastra/ps_container.h"

using namespace pastra;

D_UINT8 buffer[1468];
const D_CHAR fileName[] = "./test_file.tm";


static bool
create_container (D_UINT max_file_size, const D_UINT container_size)
{
  FileContainer container (fileName, max_file_size, 0);
  D_UINT8 marker = 0;
  D_UINT64 current_pos = 0;
  D_UINT left_to_write = container_size;

  while (left_to_write > 0)
    {
      D_UINT write_size = MIN (sizeof (buffer), left_to_write);

      memset (buffer, marker, sizeof buffer);
      container.Write (current_pos, write_size, buffer);
      marker = (marker + 1) & 0xFF;
      left_to_write -= write_size;
      current_pos += write_size;
    }
  return container.Size () == container_size;
}

static bool
check_container (D_UINT max_file_size,
                 const D_UINT container_size,
                 const D_UINT8 marker_start, const D_UINT8 marker_increment)
{
  FileContainer container (fileName, max_file_size,
                           (container_size / max_file_size) + 1);
  D_UINT64 current_pos = 0;
  D_UINT8 marker = marker_start;
  D_UINT left_to_read = container_size;

  if (container.Size () != container_size)
    return false;

  while (left_to_read > 0)
    {
      D_UINT read_size = MIN (sizeof (buffer), left_to_read);
      container.Read (current_pos, read_size, buffer);

      for (D_UINT index = 0; index < read_size; ++index)
        if (buffer[index] != marker)
          return false;
      marker = (marker + marker_increment) & 0xFF;
      left_to_read -= read_size;
      current_pos += read_size;
    }
  return true;
}

static D_UINT
colapse_container (D_UINT max_file_size, const D_UINT container_size)
{
  D_UINT64 current_pos = 0;
  FileContainer container (fileName, max_file_size,
                           (container_size / max_file_size) + 1);
  D_UINT64 new_container_size = container.Size ();

  if (new_container_size != container_size)
    return false;

  while (current_pos < container.Size ())
    {
      D_UINT64 to_delete = MIN (sizeof buffer,
                                container.Size () - current_pos);
      container.Colapse (current_pos, current_pos + to_delete);
      current_pos += to_delete;
      new_container_size -= to_delete;
    }
  return new_container_size;
}

static bool
check_temp_container (D_UINT uTestContainerSize)
{
  const D_UINT uContainerSize = 761;
  const D_UINT uStepSize = 100;
  TempContainer container ("./", uContainerSize);

  D_UINT8 marker = 0;
  D_UINT64 current_pos = 0;
  D_UINT left_to_write = uTestContainerSize;

  while (left_to_write > 0)
    {
      D_UINT write_size = MIN (uStepSize, left_to_write);

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

  D_UINT left_to_read = container.Size ();
  marker = 1;
  current_pos = 0;

  while (left_to_read > 0)
    {
      D_UINT read_size = MIN (uStepSize, left_to_read);
      container.Read (current_pos, read_size, buffer);

      for (D_UINT index = 0; index < read_size; ++index)
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
  // VC++ allocates memory when the C++ runtime is initialised
  // We need not to test against it!
  D_UINT mem_usage = test_get_mem_used ();

  bool success = true;

  {
    DBSInit (DBSSettings ());
  }

  const D_UINT max_file_size = 512 * 1024;
  const D_UINT container_size = max_file_size * 4 + 345;

  if (!create_container (max_file_size, container_size))
    success = false;

  if (!check_container (max_file_size, container_size, 0, 1))
    success = false;

  D_UINT64 new_container_size = colapse_container (max_file_size,
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

  DBSShoutdown ();

  D_UINT mem_peak = test_get_mem_peak ();
  mem_usage = test_get_mem_used () - mem_usage;

  if (mem_usage != 0)
    success = false;

  std::cout << "Memory peak: " << mem_peak << " bytes." << std::endl;
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
