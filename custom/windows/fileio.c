/******************************************************************************
WHISPER - An advanced database system
Copyright (C) 2008  Iulian Popa

Address: Str Olimp nr. 6
         Pantelimon Ilfov,
         Romania
Phone:   +40721939650
e-mail:  popaiulian@gmail.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <assert.h>

#include "whisper.h"
#include "whisper_fileio.h"

WH_FILE
whf_open (const char* const file, uint_t mode)
{
  DWORD       dwDesiredAccess = 0;
  DWORD       dwCreation      = 0;
  DWORD       dwFlagsAndAttr  = 0;
  WH_FILE     result;

  if (mode & WHC_FILECREATE_NEW)
    dwCreation |= CREATE_NEW;

  else if (mode & WHC_FILECREATE)
    dwCreation |= CREATE_ALWAYS;

  else
    dwCreation |= OPEN_EXISTING;

  dwFlagsAndAttr |= (mode & WHC_FILEDIRECT) ? FILE_FLAG_WRITE_THROUGH : 0;
  dwFlagsAndAttr |= (mode & WHC_FILESYNC) ? FILE_FLAG_NO_BUFFERING : 0;

  dwDesiredAccess |= (mode & WHC_FILEREAD) ? GENERIC_READ : 0;
  dwDesiredAccess |= (mode & WHC_FILEWRITE) ? GENERIC_WRITE : 0;

  result = CreateFile (file,
                       dwDesiredAccess,
                       0,
                       NULL,
                       dwCreation,
                       dwFlagsAndAttr,
                       NULL);
  result = (result == INVALID_HANDLE_VALUE) ? (WH_FILE)-1 : result;

  return result;
}

WH_FILE
whf_dup (WH_FILE hnd)
{
  WH_FILE result = INVALID_HANDLE_VALUE;

  DuplicateHandle(GetCurrentProcess(),
                  hnd,
                  GetCurrentProcess(),
                  &result,
                  0,
                  FALSE,
                  DUPLICATE_SAME_ACCESS);

  return (result == INVALID_HANDLE_VALUE) ? 0 : result;

}

bool_t
whf_seek (WH_FILE hnd, int64_t where, int whence)
{
  LARGE_INTEGER sliWhere;

  sliWhere.QuadPart = where;

  switch (whence)
    {
    case WHC_SEEK_BEGIN:
      whence = FILE_BEGIN;
      break;

    case WHC_SEEK_END:
      whence = FILE_END;
      break;

    case WHC_SEEK_CURR:
      whence = FILE_CURRENT;
      break;

    default:
      assert (FALSE);
    }

  if ( ! SetFilePointerEx (hnd, sliWhere, NULL, whence))
    return FALSE;

  return TRUE;
}

bool_t
whf_read (WH_FILE hnd, uint8_t* dstBuffer, uint_t size)
{
  bool_t result       = TRUE;
  uint_t actual_count = 0;

  while (actual_count < size)
    {
      DWORD count;
      if ( ! ReadFile (hnd,
                       dstBuffer + actual_count,
                       size - actual_count,
                       &count,
                       NULL))
        {
          result = FALSE;

          break;
        }
      actual_count += count;
    }

  assert ((result == TRUE) || (actual_count < size));
  assert ((result == FALSE) || (size == actual_count));

  return result;
}

bool_t
whf_write (WH_FILE hnd, const uint8_t* srcBuffer, uint_t size)
{

  bool_t result       = TRUE;
  uint_t actualCount = 0;

  while (actualCount < size)
    {
      DWORD count;
      if ( ! WriteFile (hnd,
                        srcBuffer + actualCount,
                        size - actualCount,
                        &count,
                        NULL))
        {
          result = FALSE;

          break;
        }
      actualCount += count;
    }

  assert ((result == TRUE) || (actualCount < size));
  assert ((result == FALSE) || (size == actualCount));

  return result;
}

bool_t
whf_tell (WH_FILE hnd, uint64_t* const outPosition)
{

  const LARGE_INTEGER sTemp = { 0, };

  if (!SetFilePointerEx (hnd,
                         sTemp,
                         (LARGE_INTEGER*)outPosition,
                         FILE_CURRENT))
    {
      return FALSE;
    }

  return TRUE;
}

bool_t
whf_sync (WH_FILE hnd)
{
  const HANDLE handler = (HANDLE) hnd;

  return FlushFileBuffers (handler);
}

bool_t
whf_tell_size (WH_FILE hnd, uint64_t* const outSize)
{
  return GetFileSizeEx (hnd, (LARGE_INTEGER*) outSize);
}

bool_t
whf_set_size (WH_FILE hnd, const uint64_t newSize)
{
  if (whf_seek (hnd, newSize, WHC_SEEK_BEGIN)
      && (SetEndOfFile (hnd) != 0))
    {
      return TRUE;
    }

  return FALSE;
}

bool_t
whf_close (WH_FILE hnd)
{
  return CloseHandle (hnd);
}

uint32_t
whf_last_error ()
{
  return GetLastError ();
}

bool_t
whf_err_to_str (uint64_t error_code, char* str, uint_t strSize)
{
  return FormatMessage (
              FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL,
              (DWORD) error_code,
              MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
              str, strSize, NULL
                       ) != 0;
}

bool_t
whf_remove (const char* const file)
{
  return DeleteFile (file);
}

const char*
whf_dir_delim ()
{
  return "\\";
}

const char*
whf_current_dir ()
{
  return ".\\";
}

bool_t
whf_is_absolute (const char* const path)
{
  assert (path != NULL);

  return (path[1] == ':');
}
