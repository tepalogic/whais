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

#ifndef _GNU_SOURCE
/* Not exactly POSIX, but we can leave with it. */
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <errno.h>

#include "whisper.h"

bool_t
wh_init_socks ()
{
  static bool_t _inited = FALSE;

  assert (_inited == FALSE);
  if (_inited)
    return FALSE;

  _inited = TRUE;
  return TRUE;
}

uint32_t
wh_socket_client (const char* const        pServer,
                  const char* const        pPort,
                  WH_SOCKET* const           pOutSocket)
{
  struct addrinfo  hints    = {0, };
  struct addrinfo* pResults = NULL;
  struct addrinfo* pIt      = NULL;
  uint32_t         status   = ~0;
  int            sd       = -1;
  const int      on       = 1;

  assert (pServer != NULL);
  assert (pPort != NULL);

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_ADDRCONFIG;

  status = getaddrinfo (pServer, pPort, &hints, &pResults);
  if (status != 0)
    return status;

  for (pIt = pResults; pIt != NULL; pIt = pResults->ai_next)
    {
      sd = socket (pIt->ai_family, pIt->ai_socktype, pIt->ai_protocol);
      if (sd < 0)
        continue ;

      if (setsockopt (sd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof on) < 0)
        {
          status = errno;
          close (sd);

          return status;
        }
      else if (connect (sd, pIt->ai_addr, pIt->ai_addrlen) == 0)
          break;
      else
        {
          status = errno;
          close (sd);

          return status;
        }
    }

  status = errno;
  freeaddrinfo (pResults);

  if (pIt != NULL)
    {
      /* We have a connected socket */
      *pOutSocket = sd;
      return WOP_OK;
    }

  return status;
}

uint32_t
wh_socket_server (const char* const       pLocalAdress,
                  const char* const       pPort,
                  const uint_t              listenBackLog,
                  WH_SOCKET* const          pOutSocket)
{
  struct addrinfo  hints    = {0, };
  struct addrinfo* pResults = NULL;
  struct addrinfo* pIt      = NULL;
  uint32_t         status   = ~0;
  int            sd       = -1;
  const int      on       = 1;

  assert (pPort != NULL);

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_ADDRCONFIG | AI_NUMERICHOST | AI_PASSIVE;

  status = getaddrinfo (pLocalAdress, pPort, &hints, &pResults);
  if (status != 0)
    return status;

  for (pIt = pResults; pIt != NULL; pIt = pResults->ai_next)
    {
      sd = socket (pIt->ai_family, pIt->ai_socktype, pIt->ai_protocol);
      if (sd < 0)
        continue ;

      if ((setsockopt (sd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof on) < 0)
          || (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) < 0))
        {
          status = errno;
          close (sd);

          return status;
        }
      else if (bind (sd, pIt->ai_addr, pIt->ai_addrlen) == 0)
        break;
      else
        {
          status = errno;
          close (sd);

          return status;
        }
    }
  status = errno;
  freeaddrinfo (pResults);

  if (pIt != NULL)
    {
      /* We have a valid socket */
      if (listen (sd, listenBackLog) < 0)
        {
          status = errno;
          close (sd);

          return status;
        }

      *pOutSocket = sd;
      return WOP_OK;
    }

  return status;
}

uint32_t
wh_socket_accept (const WH_SOCKET      sd,
                  WH_SOCKET* const     pConnectSocket)
{
  int csd = accept (sd, NULL, NULL);
  if (csd < 0)
    return errno;

  *pConnectSocket = csd;
  return WOP_OK;
}

uint32_t
wh_socket_write (const WH_SOCKET      sd,
                 const uint8_t*       pBuffer,
                 const uint_t         count)
{
  uint_t wrote = 0;

  assert (count > 0);

  while (wrote < count)
    {
      const ssize_t chunk = send (sd, pBuffer + wrote, count - wrote, 0);
      if (chunk < 0)
        {
          if (errno != EAGAIN)
            return errno;
        }
      else
        {
          assert (chunk > 0);
          wrote += chunk;
        }
    }

  assert (wrote == count);

  return WOP_OK;
}

uint32_t
wh_socket_read (const WH_SOCKET           sd,
                uint8_t*                  pOutBuffer,
                uint_t* const             pIOCount)
{
  if (*pIOCount == 0)
    return EINVAL;

  while (TRUE)
    {
      const ssize_t chunk = recv (sd, pOutBuffer, *pIOCount, 0);
      if (chunk < 0)
        {
          if (errno != EAGAIN)
            return errno;
        }
      else
        {
          assert (chunk <= *pIOCount);

          *pIOCount = chunk;
          break;
        }
    }

  return WOP_OK;
}

void
wh_socket_close (const WH_SOCKET sd)
{
  shutdown (sd, SHUT_RDWR);
  close (sd);
}

void
wh_clean_socks ()
{
}

