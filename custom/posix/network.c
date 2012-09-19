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

static void
handle_sock_error (int error,  enum WH_SOCK_ERROR* const  pOutError)
{
  if (pOutError == NULL)
    return;
  else if (error == EAI_SYSTEM)
    error = errno;

  switch (error)
  {
  case EAI_MEMORY:
  case ENOMEM:
    *pOutError = WH_SOCK_NOMEM;
    break;
  case EAI_FAIL:
  case EAI_NONAME:
    *pOutError = WH_SOCK_NONAME;
    break;
  case EAI_SERVICE:
    *pOutError = WH_SOCK_NOSERV;
    break;
  case EPIPE:
    *pOutError = WH_SOCK_EPIPE;
    break;
  case EINVAL:
    *pOutError = WH_SOCK_EINVAL;
    break;
  case ENOBUFS:
    *pOutError = WH_SOCK_ENOBUF;
    break;
  case EACCES:
  case EPERM:
    *pOutError = WH_SOCK_NOPERM;
    break;
  case ECONNREFUSED:
  case ENETUNREACH:
    *pOutError = WH_SOCK_NOTRICH;
    break;
  case ETIMEDOUT:
    *pOutError = WH_SOCK_ETIMEOUT;
    break;
  default:
    *pOutError = WH_SOCK_UNK;
  }
}

D_BOOL
wh_init_socks ()
{
  static D_BOOL _inited = FALSE;

  assert (_inited == FALSE);
  _inited = TRUE;

  return TRUE;
}

D_BOOL
wh_socket_client (const D_CHAR* const        pServer,
                  const D_CHAR* const        pPort,
                  WH_SOCKET* const           pOutSocket,
                  enum WH_SOCK_ERROR* const  pOutError)
{
  struct addrinfo  hints    = {0, };
  struct addrinfo* pResults = NULL;
  struct addrinfo* pIt      = NULL;
  int              status   = -1;
  int              sd      = -1;
  const int        on        = 1;

  assert (pServer != NULL);
  assert (pPort != NULL);

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_ADDRCONFIG;

  status = getaddrinfo (pServer, pPort, &hints, &pResults);
  if (status != 0)
    {
      handle_sock_error (status, pOutError);
      return FALSE;
    }

  for (pIt = pResults; pIt != NULL; pIt = pResults->ai_next)
    {
      sd = socket (pIt->ai_family, pIt->ai_socktype, pIt->ai_protocol);
      if (sd < 0)
        continue ;

      if (setsockopt (sd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof on) < 0)
        {
          handle_sock_error (errno, pOutError);
          close (sd);
        }
      else if (connect (sd, pIt->ai_addr, pIt->ai_addrlen) == 0)
          break;
      else
        {
          handle_sock_error (errno, pOutError);
          close (sd);
        }
    }

  freeaddrinfo (pResults);

  if (pIt != NULL)
    {
      /* We have a connected socket */
      *pOutSocket = sd;
      return TRUE;
    }

  return FALSE;
}

D_BOOL
wh_socket_server (const D_CHAR* const       pLocalAdress,
                  const D_CHAR* const       pPort,
                  const D_UINT              listenBackLog,
                  WH_SOCKET* const          pOutSocket,
                  enum WH_SOCK_ERROR* const pOutError)
{
  struct addrinfo  hints    = {0, };
  struct addrinfo* pResults = NULL;
  struct addrinfo* pIt      = NULL;
  int              status   = -1;
  int              sd      = -1;
  const int        on       = 1;

  assert (pPort != NULL);

  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_ADDRCONFIG | AI_NUMERICHOST | AI_PASSIVE;

  status = getaddrinfo (NULL, pPort, &hints, &pResults);
  if (status != 0)
    {
      handle_sock_error (status, pOutError);
      return FALSE;
    }

  for (pIt = pResults; pIt != NULL; pIt = pResults->ai_next)
    {
      sd = socket (pIt->ai_family, pIt->ai_socktype, pIt->ai_protocol);
      if (sd < 0)
        continue ;

      if ((setsockopt (sd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof on) < 0)
          || (setsockopt (sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on) < 0))
        {
          handle_sock_error (errno, pOutError);
          close (sd);
        }
      else if (bind (sd, pIt->ai_addr, pIt->ai_addrlen) == 0)
        break;
      else
        {
          handle_sock_error (errno, pOutError);
          close (sd);
        }
    }

  freeaddrinfo (pResults);

  if (pIt != NULL)
    {
      /* We have a valid socket */
      if (listen (sd, listenBackLog) < 0)
        {
          handle_sock_error (errno, pOutError);
          close (sd);

          return FALSE;
        }

      *pOutSocket = sd;
      return TRUE;
    }

  return FALSE;
}

D_BOOL
wh_socket_accept (const WH_SOCKET      sd,
                  WH_SOCKET* const     pConnectSocket,
                  enum WH_SOCK_ERROR* const pOutError)
{
  int csd = accept (sd, NULL, NULL);
  if (csd < 0)
    {
      handle_sock_error (errno, pOutError);
      return FALSE;
    }

  *pConnectSocket = csd;
  return TRUE;
}

D_BOOL
wh_socket_write (const WH_SOCKET      sd,
                 const D_UINT8*       pBuffer,
                 const D_UINT         count,
                 enum WH_SOCK_ERROR* const pOutError)
{
  D_UINT wrote = 0;

  assert (count > 0);

  while (wrote < count)
    {
      const ssize_t chunk = send (sd, pBuffer + wrote, count - wrote, 0);
      if (chunk < 0)
        {
          if (errno != EAGAIN)
            {
              handle_sock_error (errno, pOutError);
              return FALSE;
            }
        }
      else
        {
          assert (chunk > 0);
          wrote += chunk;
        }
    }

  assert (wrote == count);

  return TRUE;
}

D_BOOL
wh_socket_read (const WH_SOCKET           sd,
                D_UINT8*                  pOutBuffer,
                D_UINT* const             pIOCount,
                enum WH_SOCK_ERROR* const pOutError)
{
  while (TRUE)
    {
      const ssize_t chunk = recv (sd, pOutBuffer, *pIOCount, 0);
      if (chunk < 0)
        {
          if (errno != EAGAIN)
            {
              handle_sock_error (errno, pOutError);
              return FALSE;
            }
        }
      else if (*pIOCount == 0)
        {
          handle_sock_error (EPIPE, pOutError);
          return FALSE;
        }
      else
        {
          assert (chunk <= *pIOCount);
          *pIOCount = chunk;
          break;
        }
    }

  return TRUE;
}

void
wh_socket_close (const WH_SOCKET sd)
{
  shutdown (sd, SHUT_RDWR);
}

void
wh_clean_socks ()
{
}

