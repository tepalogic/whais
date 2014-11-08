/******************************************************************************
  WCMD - An utility to manage whais database files.
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
#include <stdlib.h>
#include <string>

#include "wcmd_optglbs.h"



using namespace std;
using namespace whais;



static const uint64_t   MINIMUM_FILE_SIZE = 0x100000;    //1 MB
static const char       DEFAULT_PORT[]    = "1761";
static const uint_t     DEFAULT_USER      = 1;


static string        sWorkingDirectory( whf_current_dir( ));
static string        sDBName;
static VERBOSE_LEVEL sVerbLevel       = VL_ERROR;
static uint64_t      sMaxFileSize     = 0x80000000; //default: 2GB
static IDBSHandler*  sDBSHnd          = NULL;
static string        sRemoteHost;
static string        sConnectPort;
static string        sPassword;
static uint_t        sUserId          = DEFAULT_USER;



const string&
GetRemoteHostName( )
{
  return sRemoteHost;
}


void
SetRemoteHostName( const char* const host)
{
  sRemoteHost = host;
}


const string&
GetConnectionPort( )
{
  assert( IsOnlineDatabase( ));

  if (sConnectPort.size( ) == 0)
    sConnectPort = DEFAULT_PORT;

  return sConnectPort;
}


void
SetConnectionPort( const char* const port)
{
  sConnectPort = port;
}


uint_t
GetUserId( )
{
  assert( IsOnlineDatabase( ));

  return sUserId;
}


void
SetUserId( const uint_t userId)
{
  sUserId = userId;
}


const string&
GetUserPassword( )
{
  assert( IsOnlineDatabase( ));

  return sPassword;
}


void
SetUserPassword( const char* const password)
{
  sPassword = password;
}


const string&
GetWorkingDirectory( )
{
  assert( ! IsOnlineDatabase( ));

  return sWorkingDirectory;
}


void
SetWorkingDirectory( const char* const directory)
{
  sWorkingDirectory = directory;
}


const string&
GetWorkingDB( )
{
  return sDBName;
}


void
SetWorkingDB( const char* const dbName)
{
  sDBName = dbName;
}


VERBOSE_LEVEL
GetVerbosityLevel( )
{
  return sVerbLevel;
}


void
SetVerbosityLevel( const uint_t level)
{
  sVerbLevel = _SC (VERBOSE_LEVEL, MIN (level, VL_MAX));
}


bool
SetMaximumFileSize( string size)
{
  static const string digits = "0123456789";

  if (size.length( ) == 0)
    return false;

  uint64_t multiplier = 1;
  size_t   lastPos    = size.find_first_not_of( digits);

  if (lastPos != size.npos)
    {
      switch( size[lastPos])
        {
        case 'k':
        case 'K':
          multiplier     = 1024;
          size [lastPos] = 0;
          break;

        case 'm':
        case 'M':
          multiplier     = 1024 * 1024;
          size [lastPos] = 0;
          break;

        case 'g':
        case 'G':
          multiplier     = 1024 * 1024 * 1024;
          size [lastPos] = 0;
          break;

        default:
          return false;
        }
    }

  sMaxFileSize = atoi( size.c_str( )) * multiplier;

  if (sMaxFileSize < MINIMUM_FILE_SIZE)
    return false;

  return true;
}

uint64_t
GetMaximumFileSize( )
{
  assert( ! IsOnlineDatabase( ));

  return sMaxFileSize;
}


void
SetDbsHandler( IDBSHandler& dbs)
{
  assert( ! IsOnlineDatabase( ));

  sDBSHnd = &dbs;
}


IDBSHandler&
GetDBSHandler( )
{
  assert( sDBSHnd != NULL);
  assert( ! IsOnlineDatabase( ));

  return *sDBSHnd;
}


bool
IsOnlineDatabase( )
{
  if (GetRemoteHostName( ).size( ) > 0)
    return true;

  return false;
}

