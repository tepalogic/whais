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
whs_init ()
{
  static bool_t _inited = FALSE;

  assert (_inited == FALSE);
  if (_inited)
    return FALSE;

  _inited = TRUE;
  return TRUE;
}


uint32_t
whs_create_client (const char* const       server,
                  const char* const        port,
                  WH_SOCKET* const         outSocket)
{
  struct addrinfo  hints    = {0, };
  struct addrinfo* pResults = NULL;
  struct addrinfo* pIt      = NULL;
  uint32_t         status   = ~0;
  int              sd       = -1;
  const int        on       = 1;

  assert (server != NULL);
  assert (port != NULL);

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_ADDRCONFIG;

  status = getaddrinfo (server, port, &hints, &pResults);
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
      *outSocket = sd;
      return WOP_OK;
    }

  return status;
}


uint32_t
whs_create_server (const char* const       localAddress,
                  const char* const        localPort,
                  const uint_t             listenBackLog,
                  WH_SOCKET* const         outSocket)
{
  struct addrinfo  hints    = {0, };
  struct addrinfo* pResults = NULL;
  struct addrinfo* pIt      = NULL;
  uint32_t         status   = ~0;
  int            sd       = -1;
  const int      on       = 1;

  assert (localPort != NULL);

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_ADDRCONFIG | AI_NUMERICHOST | AI_PASSIVE;

  status = getaddrinfo (localAddress, localPort, &hints, &pResults);
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

      *outSocket = sd;
      return WOP_OK;
    }

  return status;
}


uint32_t
whs_accept (const WH_SOCKET      sd,
            WH_SOCKET* const     outSocket)
{
  const int clientSD = accept (sd, NULL, NULL);
  if (clientSD < 0)
    return errno;

  *outSocket = clientSD;
  return WOP_OK;
}


uint32_t
whs_write (const WH_SOCKET      sd,
           const uint8_t*       srcBuffer,
           const uint_t         count)
{
  uint_t wrote = 0;

  assert (count > 0);

  while (wrote < count)
    {
      const ssize_t chunk = send (sd, srcBuffer + wrote, count - wrote, 0);
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
whs_read (const WH_SOCKET           sd,
                uint8_t*            dstBuffer,
                uint_t* const       inoutCount)
{
  if (*inoutCount == 0)
    return EINVAL;

  const ssize_t chunk = recv (sd, dstBuffer, *inoutCount, 0);
  if (chunk < 0)
    {
      if (errno != EAGAIN)
        return errno;
    }
  else
    {
      assert (chunk <= *inoutCount);

      *inoutCount = chunk;
    }

  return WOP_OK;
}


void
whs_close (const WH_SOCKET sd)
{
  shutdown (sd, SHUT_RDWR);
  close (sd);
}


void
whs_clean ()
{
}

