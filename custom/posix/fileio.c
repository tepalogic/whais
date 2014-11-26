/******************************************************************************
WHAIS - An advanced database system
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

#ifndef _GNU_SOURCE
/* Not exactly POSIX, but we can leave with it. */
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#include "whais.h"
#include "whais_fileio.h"

#define POSIX_FAIL_RET (~0)


WH_FILE
whf_open( const char* const file, uint_t mode)
{
  int         openMode = O_LARGEFILE;
  const int   accMode  = (S_IRUSR | S_IWUSR | S_IRGRP);
  WH_FILE     result   = -1;

  if (mode & WH_FILECREATE_NEW)
    openMode |= O_CREAT | O_EXCL;

  else if (mode & WH_FILECREATE)
    openMode |= O_CREAT;

  else
    openMode |= 0; /* WHC_FILEOPEN_EXISTING */

  openMode |= (mode & WH_FILEDIRECT) ? O_DIRECT : 0;
  openMode |= (mode & WH_FILESYNC) ? O_SYNC : 0;

  if ((mode & WH_FILERDWR) == WH_FILERDWR)
    openMode |= O_RDWR;

  else
    {
      openMode |= (mode & WH_FILEREAD) ? O_RDONLY : 0;
      openMode |= (mode & WH_FILEWRITE) ? O_WRONLY : 0;
    }

  result = open( file, openMode, accMode);
  return( (result < 0) ? INVALID_FILE : result);
}


WH_FILE
whf_dup( WH_FILE hnd)
{
  WH_FILE result = dup (hnd);

  return( result < 0) ? INVALID_FILE: result;
}


bool_t
whf_seek( WH_FILE hnd, int64_t where, int whence)
{
  switch( whence)
    {
    case WH_SEEK_BEGIN:
      whence = SEEK_SET;
      break;

    case WH_SEEK_END:
      whence = SEEK_END;
      break;

    case WH_SEEK_CURR:
      whence = SEEK_CUR;
      break;

    default:
      assert( FALSE);
    }

  return( lseek64 (hnd, where, whence) != POSIX_FAIL_RET);
}


bool_t
whf_read( WH_FILE hnd, uint8_t* dstBuffer, uint_t size)
{
  bool_t result      = TRUE;
  uint_t actualCount = 0;

  while( actualCount < size)
    {
      uint_t count = read( hnd,
                           dstBuffer + actualCount,
                           size - actualCount);
      if (count == POSIX_FAIL_RET)
        {
          /* the errno is already set for this */
          result = FALSE;

          break;
        }
      else if (count == 0)
        {
          /* Reading past the end of file is not considered an
           * error in POSIX, but we do! */
          errno  = ENODATA;
          result = FALSE;

          break;
        }
      actualCount += count;
    }

  assert( (result == TRUE) || (actualCount < size));
  assert( (result == FALSE) || (size == actualCount));

  return result;
}


bool_t
whf_write( WH_FILE hnd, const uint8_t* srcBuffer, uint_t size)
{
  bool_t result       = TRUE;
  uint_t actual_count = 0;

  while( actual_count < size)
    {
      uint_t count = write( hnd,
                            srcBuffer + actual_count,
                            size - actual_count);
      if (count == POSIX_FAIL_RET)
        {
          /* the errno is already set for this */
          result = FALSE;
          break;
        }
      actual_count += count;
    }

  assert( (result == TRUE) || (actual_count < size));
  assert( (result == FALSE) || (size == actual_count));

  return result;
}


bool_t
whf_tell( WH_FILE hnd, uint64_t* const outPosition)
{
  *outPosition = lseek64 (hnd, 0, SEEK_CUR);

  return( *outPosition != POSIX_FAIL_RET);
}


bool_t
whf_sync( WH_FILE hnd)
{
  return( fsync( hnd) != POSIX_FAIL_RET);
}


bool_t
whf_tell_size( WH_FILE hnd, uint64_t* const outSize)
{
  struct stat64 buf;

  if (fstat64 (hnd, &buf) == POSIX_FAIL_RET)
    return FALSE;

  *outSize = buf.st_size;
  return TRUE;
}


bool_t
whf_set_size( WH_FILE hnd, const uint64_t newSize)
{
  if (ftruncate( hnd, newSize) != 0)
    return FALSE;

  return TRUE;
}


bool_t
whf_close( WH_FILE hnd)
{
  return( close( hnd) != POSIX_FAIL_RET);
}


uint32_t
whf_last_error()
{
  /* POSIX.1c enforce this to be thread safe */
  return errno;
}


bool_t
whf_err_to_str( uint64_t errorCode, char* str, uint_t strSize)
{
  const char* const pMessage = strerror_r( (int) errorCode, str, strSize);

  assert( strSize > 0);
  assert( str != NULL);

  str [0] = 0;

  if (pMessage != NULL)
    {
      strncpy( str, pMessage, strSize);
      return TRUE;
    }

  return FALSE;
}


bool_t
whf_remove( const char* const file)
{
  if (unlink( file) == 0)
    return TRUE;

  return FALSE;
}


const char*
whf_dir_delim()
{
  return "/";
}


const char*
whf_current_dir()
{
  return "./";
}


bool_t
whf_is_absolute( const char* const path)
{
  assert( path != NULL);

  return path[0] == '/';
}


bool_t
whf_file_exists( const char* const file)
{
  struct stat statBuffer;

  return stat( file, &statBuffer) == 0;
}


