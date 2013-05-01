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

#define WIN32
#define _WINDOWS
#include <sdkddkver.h>
#include <windows.h>
#include <assert.h>

#include "whisper.h"
#include "whisper_fileio.h"

WH_FILE
whf_open (const char* pFileName, uint_t mode)
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

  result = CreateFile (pFileName,
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
whf_dup (WH_FILE f_hnd)
{
  WH_FILE result = INVALID_HANDLE_VALUE;

  DuplicateHandle(GetCurrentProcess(),
                  f_hnd,
                  GetCurrentProcess(),
                  &result,
                  0,
                  FALSE,
                  DUPLICATE_SAME_ACCESS);
  return (result == INVALID_HANDLE_VALUE) ? 0 : result;

}

bool_t
whf_seek (WH_FILE f_hnd, int64_t where, int whence)
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
      assert (0);
    }

  if (!SetFilePointerEx (f_hnd, sliWhere, NULL, whence))
    return FALSE;

  return TRUE;
}

bool_t
whf_read (WH_FILE f_hnd, uint8_t* pBuffer, uint_t size)
{
  bool_t result = TRUE;
  uint_t actual_count = 0;

  while (actual_count < size)
    {
      DWORD count;
      if (!ReadFile (f_hnd, pBuffer + actual_count,
                     size - actual_count, &count, NULL))
        {
          /* the errno is already set for this */
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
whf_write (WH_FILE f_hnd, const uint8_t* pBuffer, uint_t size)
{

  bool_t result     = TRUE;
  uint_t actualCount = 0;

  while (actualCount < size)
    {
      DWORD count;
      if (!WriteFile (f_hnd,
                      pBuffer + actualCount,
                      size - actualCount,
                      &count,
                      NULL))
        {
          /* the errno is already set for this */
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
whf_tell (WH_FILE f_hnd, uint64_t* pOutSize)
{

  const LARGE_INTEGER sTemp = { 0, };

  if (!SetFilePointerEx (f_hnd,
                         sTemp,
                         (LARGE_INTEGER*)pOutSize,
                         FILE_CURRENT))
    return FALSE;

  return TRUE;
}

bool_t
whf_sync (WH_FILE f_hnd)
{
  const HANDLE handler = (HANDLE) f_hnd;
  return FlushFileBuffers (handler);
}

bool_t
whf_tell_size (WH_FILE f_hnd, uint64_t* pOutSize)
{
  return GetFileSizeEx (f_hnd, (LARGE_INTEGER*) pOutSize);
}

bool_t
whf_set_size (WH_FILE f_hnd, uint64_t newSize)
{
  if (whf_seek (f_hnd, newSize, WHC_SEEK_BEGIN)
      && (SetEndOfFile (f_hnd) != 0))
    return TRUE;
  return FALSE;
}

bool_t
whf_close (WH_FILE f_hnd)
{
  return CloseHandle (f_hnd);
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
              str, strSize, NULL) != 0;
}

bool_t
whf_remove (const char *pFileName)
{
  return DeleteFile (pFileName);
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
whf_is_absolute (const char* pPath)
{
  assert (pPath != NULL);
  return (pPath[1] == ':');
}
