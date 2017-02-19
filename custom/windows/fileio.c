/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#include "whais.h"
#include "whais_fileio.h"

#include <shlwapi.h>

WH_FILE
whf_open(const char* const file, uint_t mode)
{
  DWORD       dwDesiredAccess = 0;
  DWORD       dwCreation      = 0;
  DWORD       dwFlagsAndAttr  = 0;
  WH_FILE     result;

  if (mode & WH_FILECREATE_NEW)
    dwCreation = CREATE_NEW;

  else if (mode & WH_FILECREATE)
    dwCreation = OPEN_ALWAYS;

  else
    dwCreation = OPEN_EXISTING;

  dwFlagsAndAttr |= (mode & WH_FILESYNC) ? FILE_FLAG_WRITE_THROUGH : 0;
  dwFlagsAndAttr |= FILE_FLAG_RANDOM_ACCESS;

  dwDesiredAccess |= (mode & WH_FILEREAD)  ? GENERIC_READ  : 0;
  dwDesiredAccess |= (mode & WH_FILEWRITE) ? GENERIC_WRITE : 0;

  result = CreateFile(file,
                       dwDesiredAccess,
                       0,
                       NULL,
                       dwCreation,
                       dwFlagsAndAttr,
                       NULL);

  if ((result == INVALID_FILE)
      && (GetLastError() == ERROR_SHARING_VIOLATION))
    {
      result = FILE_LOCKED;
    }

  if (mode & WH_FILETRUNC)
    {
      if (! whf_set_size(result, 0))
        {
          const DWORD lastError = GetLastError();
          CloseHandle(result);
          SetLastError(lastError);

          result = INVALID_FILE;
        }
    }

  return result;
}

WH_FILE
whf_dup(WH_FILE hnd)
{
  WH_FILE result = INVALID_HANDLE_VALUE;

  DuplicateHandle(GetCurrentProcess(),
                  hnd,
                  GetCurrentProcess(),
                  &result,
                  0,
                  FALSE,
                  DUPLICATE_SAME_ACCESS);

  return result;

}

bool_t
whf_seek(WH_FILE hnd, int64_t where, int whence)
{
  LARGE_INTEGER sliWhere;

  sliWhere.QuadPart = where;

  switch(whence)
    {
    case WH_SEEK_BEGIN:
      whence = FILE_BEGIN;
      break;

    case WH_SEEK_END:
      whence = FILE_END;
      break;

    case WH_SEEK_CURR:
      whence = FILE_CURRENT;
      break;

    default:
      assert(FALSE);
    }

  if ( ! SetFilePointerEx(hnd, sliWhere, NULL, whence))
    return FALSE;

  return TRUE;
}

bool_t
whf_read(WH_FILE hnd, uint8_t* dstBuffer, uint_t size)
{
  bool_t result       = TRUE;
  uint_t actual_count = 0;

  while (actual_count < size)
    {
      DWORD count;
      if ( ! ReadFile(hnd,
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

  assert((result == TRUE) || (actual_count < size));
  assert((result == FALSE) || (size == actual_count));

  return result;
}

bool_t
whf_write(WH_FILE hnd, const uint8_t* srcBuffer, uint_t size)
{

  bool_t result       = TRUE;
  uint_t actualCount = 0;

  while (actualCount < size)
    {
      DWORD count;
      if ( ! WriteFile(hnd,
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

  assert((result == TRUE) || (actualCount < size));
  assert((result == FALSE) || (size == actualCount));

  return result;
}

bool_t
whf_tell(WH_FILE hnd, uint64_t* const outPosition)
{

  const LARGE_INTEGER sTemp = { 0, };

  if (!SetFilePointerEx( hnd,
                         sTemp,
                         (LARGE_INTEGER*)outPosition,
                         FILE_CURRENT))
    {
      return FALSE;
    }

  return TRUE;
}

bool_t
whf_sync(WH_FILE hnd)
{
  const HANDLE handler = (HANDLE) hnd;

  return FlushFileBuffers(handler);
}

bool_t
whf_tell_size(WH_FILE hnd, uint64_t* const outSize)
{
  return GetFileSizeEx(hnd, (LARGE_INTEGER*) outSize);
}

bool_t
whf_set_size(WH_FILE hnd, const uint64_t newSize)
{
  if (whf_seek( hnd, newSize, WH_SEEK_BEGIN)
      && (SetEndOfFile( hnd) != 0))
    {
      return TRUE;
    }

  return FALSE;
}

bool_t
whf_close(WH_FILE hnd)
{
  return CloseHandle(hnd);
}

uint32_t
whf_last_error()
{
  return GetLastError();
}

bool_t
whf_err_to_str(uint64_t error_code, char* str, uint_t strSize)
{
  return FormatMessage(
              FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
              NULL,
              (DWORD) error_code,
              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
              str, strSize, NULL
                       ) != 0;
}


bool_t
whf_remove(const char* const file)
{
  return DeleteFile(file);
}


void
whf_move_file(const char* existingFile, const char* newFile )
{
  MoveFile(existingFile, newFile);
}


const char
whf_dir_delim()
{
  return '\\' ;
}

const char*
whf_current_dir()
{
  /* This is not thread save! */

  static char currentDir[MAX_PATH];
  int pathLen;

  GetCurrentDirectory(sizeof currentDir - 2, currentDir);

  pathLen = strlen(currentDir);
  if (currentDir[pathLen - 1] != '\\')
    {
      currentDir[pathLen] = '\\';
      currentDir[pathLen + 1] = 0;
    };

  return currentDir;
}

bool_t
whf_is_absolute(const char* const path)
{
  assert(path != NULL);

  return ! PathIsRelative(path);
}


bool_t
whf_file_exists(const char* const file)
{
  return PathFileExists(file);
}

