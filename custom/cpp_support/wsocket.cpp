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

#include "utils/include/wsocket.h"

WSocket::WSocket (const char* const pServerName,
                  const char* const pService)
  : m_Socket (INVALID_SOCKET),
    m_Owned (false)
{
  const uint32_t e = wh_socket_client (pServerName, pService, &m_Socket);

  if (e != WOP_OK)
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const char* const pServerName,
                  const uint16_t      port)
  : m_Socket (INVALID_SOCKET),
    m_Owned (false)
{
  uint32_t e;
  char   service[16];

  sprintf (service, "%u", port);
  e = wh_socket_client (pServerName, service, &m_Socket);

  if (e != WOP_OK)
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const char* const pLocalAddress,
                  const char* const pService,
                  const uint_t        backLog)
  : m_Socket (INVALID_SOCKET),
    m_Owned (false)
{
  const uint32_t e = wh_socket_server (pLocalAddress,
                                       pService,
                                       backLog,
                                       &m_Socket);

  if (e != WOP_OK)
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const char* const pLocalAddress,
                  const uint16_t      port,
                  const uint_t        backLog)
  : m_Socket (INVALID_SOCKET),
    m_Owned (false)
{
  uint32_t e;
  char   service[16];

  sprintf (service, "%u", port);
  e = wh_socket_server (pLocalAddress, service, backLog, &m_Socket);

  if (e != WOP_OK)
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const WH_SOCKET sd)
  : m_Socket (sd),
    m_Owned ((sd != INVALID_SOCKET) ? true : false)
{
}

WSocket::WSocket (const WSocket& source)
  : m_Socket (source.m_Socket),
    m_Owned (source.m_Owned)
{
  _CC (bool&, source.m_Owned) = false;
}

WSocket::~WSocket ()
{
  if (m_Owned)
    {
      assert (m_Socket != INVALID_SOCKET);
      wh_socket_close (m_Socket);
    }
}

WSocket&
WSocket::operator= (const WSocket& source)
{
  if (this != &source)
    {
      if (m_Owned)
        wh_socket_close (m_Socket);

      m_Socket = source.m_Socket;
      m_Owned  = source.m_Owned;

      _CC (bool&, source.m_Owned) = false;
    }
  return *this;
}

WSocket
WSocket::Accept ()
{
  WH_SOCKET      client = INVALID_SOCKET;
  const uint32_t e      = wh_socket_accept (m_Socket, &client);

  if (e != WOP_OK )
    throw WSocketException (NULL, _EXTRA (e));

  assert (client != INVALID_SOCKET);

  return WSocket (client);
}

uint_t
WSocket::Read (const uint_t count, uint8_t* const pBuffer)
{
  uint_t         result = count;
  const uint32_t e      = wh_socket_read (m_Socket, pBuffer, &result);

  if (e != WOP_OK)
    throw WSocketException (NULL, _EXTRA (e));

  return result;
}

void
WSocket::Write (const uint_t count, const uint8_t* const pBuffer)
{
  const uint32_t e = wh_socket_write (m_Socket, pBuffer, count);

  if (e != WOP_OK)
    throw WSocketException (NULL, _EXTRA (e));
}

void
WSocket::Close ()
{
  if (! m_Owned)
    return;

  assert (m_Socket != INVALID_SOCKET);

  wh_socket_close (m_Socket);

  m_Socket = INVALID_SOCKET;
  m_Owned  = false;
}
