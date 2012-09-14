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

WSocket::WSocket (const D_CHAR* const pServerName,
                  const D_CHAR* const pService)
  : m_Socket (),
    m_Owned (false)
{
  WH_SOCK_ERROR e = WH_SOCK_OK;

  if (! wh_socket_client (pServerName, pService, &m_Socket, &e))
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const D_CHAR* const pServerName,
                  const D_UINT16      port)
  : m_Socket (),
    m_Owned (false)
{
  D_CHAR service[16];
  WH_SOCK_ERROR e = WH_SOCK_OK;

  sprintf (service, "%u", port);
  if (! wh_socket_client (pServerName, service, &m_Socket, &e))
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const D_CHAR* const pLocalAddress,
                  const D_CHAR* const pService,
                  const D_UINT        backLog)
  : m_Socket (),
    m_Owned (false)
{
  WH_SOCK_ERROR e = WH_SOCK_OK;

  if (! wh_socket_server (pLocalAddress, pService, backLog, &m_Socket, &e))
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const D_CHAR* const pLocalAddress,
                  const D_UINT16      port,
                  const D_UINT        backLog)
  : m_Socket (),
    m_Owned (false)
{
  D_CHAR service[16];
  WH_SOCK_ERROR e = WH_SOCK_OK;

  sprintf (service, "%u", port);
  if (! wh_socket_server (pLocalAddress, service, backLog,  &m_Socket, &e))
    throw WSocketException (NULL, _EXTRA (e));

  m_Owned = true;
}

WSocket::WSocket (const WH_SOCKET sd)
  : m_Socket (sd),
    m_Owned (true)
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
    wh_socket_close (m_Socket);
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
  WH_SOCK_ERROR e = WH_SOCK_OK;
  WH_SOCKET     client;

  if (! wh_socket_accept (m_Socket, &client, &e))
    throw WSocketException (NULL, _EXTRA (e));

  return WSocket (client);
}

D_UINT
WSocket::Read (const D_UINT count, D_UINT8* const pBuffer)
{
  WH_SOCK_ERROR e = WH_SOCK_OK;
  D_UINT result = count;

  if (! wh_socket_read (m_Socket, pBuffer, &result, &e))
    throw WSocketException (NULL, _EXTRA (e));

  return result;
}

void
WSocket::Write (const D_UINT count, const D_UINT8* const pBuffer)
{
  WH_SOCK_ERROR e = WH_SOCK_OK;

  if (! wh_socket_write (m_Socket, pBuffer, count, &e))
    throw WSocketException (NULL, _EXTRA (e));
}

void
WSocket::Close ()
{
  assert (m_Owned);

  wh_socket_close (m_Socket);
  m_Owned = false;
}
