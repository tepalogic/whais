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
#include <stdio.h>

#include "utils/wsocket.h"



namespace whisper {



Socket::Socket (const char* const   serverHost,
                const char* const   service)
  : mSocket (INVALID_SOCKET),
    mOwned (false)
{
  const uint32_t e = whs_create_client (serverHost, service, &mSocket);
  if (e != WOP_OK)
    throw SocketException (NULL, _EXTRA (e));

  mOwned = true;
}


Socket::Socket (const char* const   serverHost,
                const uint16_t      port)
  : mSocket (INVALID_SOCKET),
    mOwned (false)
{
  uint32_t e;
  char     service[16];

  sprintf (service, "%u", port);

  e = whs_create_client (serverHost, service, &mSocket);
  if (e != WOP_OK)
    throw SocketException (NULL, _EXTRA (e));

  mOwned = true;
}


Socket::Socket (const char* const     interface,
                const char* const     service,
                const uint_t          backLog)
  : mSocket (INVALID_SOCKET),
    mOwned (false)
{
  const uint32_t e = whs_create_server (interface,
                                        service,
                                        backLog,
                                        &mSocket);
  if (e != WOP_OK)
    throw SocketException (NULL, _EXTRA (e));

  mOwned = true;
}


Socket::Socket (const char* const   interface,
                const uint16_t      port,
                const uint_t        backLog)
  : mSocket (INVALID_SOCKET),
    mOwned (false)
{
  uint32_t e;
  char   service[16];

  sprintf (service, "%u", port);

  e = whs_create_server (interface, service, backLog, &mSocket);
  if (e != WOP_OK)
    throw SocketException (NULL, _EXTRA (e));

  mOwned = true;
}


Socket::Socket (const WH_SOCKET sd)
  : mSocket (sd),
    mOwned ((sd != INVALID_SOCKET) ? true : false)
{
}


Socket::Socket (const Socket& src)
  : mSocket (src.mSocket),
    mOwned (src.mOwned)
{
  _CC (bool&, src.mOwned) = false;
}


Socket::~Socket ()
{
  if (mOwned)
    {
      assert (mSocket != INVALID_SOCKET);
      whs_close (mSocket);
    }
}


Socket&
Socket::operator= (const Socket& src)
{
  if (this != &src)
    {
      if (mOwned)
        whs_close (mSocket);

      mSocket = src.mSocket;
      mOwned  = src.mOwned;

      _CC (bool&, src.mOwned) = false;
    }
  return *this;
}


Socket
Socket::Accept ()
{
  WH_SOCKET      client = INVALID_SOCKET;
  const uint32_t e      = whs_accept (mSocket, &client);

  if (e != WOP_OK )
    throw SocketException (NULL, _EXTRA (e));

  assert (client != INVALID_SOCKET);

  return Socket (client);
}


uint_t
Socket::Read (uint8_t* const buffer, const uint_t maxCount)
{
  uint_t         result = maxCount;
  const uint32_t e      = whs_read (mSocket, buffer, &result);

  if (e != WOP_OK)
    throw SocketException (NULL, _EXTRA (e));

  return result;
}


void
Socket::Write (const uint8_t* const buffer, const uint_t count)
{
  const uint32_t e = whs_write (mSocket, buffer, count);

  if (e != WOP_OK)
    throw SocketException (NULL, _EXTRA (e));
}


void
Socket::Close ()
{
  if (! mOwned)
    return;

  assert (mSocket != INVALID_SOCKET);

  whs_close (mSocket);

  mSocket = INVALID_SOCKET;
  mOwned  = false;
}

} //namespace whisper

