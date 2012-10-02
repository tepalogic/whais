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
#include <string.h>

#include "whisper_connector.h"

#include "whisper.h"

#include "server/include/server_protocol.h"
#include "utils/include/le_converter.h"

#include "connector.h"
#include "client_connection.h"

enum CONNECTOR_STATUS
Connect (const char* const   pHost,
         const char* const   pPort,
         const char* const   pDatabaseName,
         const char* const   pPassword,
         const unsigned int  userId,
         CONNECTOR_HDL*      pHnd)
{
  struct INTERNAL_HANDLER* pResult     = NULL;
  const D_UINT             passwordLen = strlen (pPassword);
  D_UINT16                 frameSize   = 0;
  enum CONNECTOR_STATUS    status      = CS_OK;

  if ((pHost == NULL)
      || (pPort == NULL)
      || (pDatabaseName == NULL)
      || (pPassword == NULL)
      || (pHnd == NULL)
      || (strlen (pHost) == 0)
      || (strlen (pPort) == 0)
      || (strlen (pDatabaseName) == 0))
    {
      status = CS_INVALID_ARGS;
      goto fail_ret;
    }

  pResult = mem_alloc (sizeof (*pResult) + passwordLen);
  memset (pResult, 0, sizeof (*pResult));

  pResult->userId     = userId;
  pResult->encKeySize = passwordLen;
  pResult->socket     = INVALID_SOCKET;
  pResult->encType    = FRAME_ENCTYPE_PLAIN;
  memcpy (pResult->encriptionKey, pPassword, passwordLen);

  if ( ! wh_socket_client (pHost, pPort, &pResult->socket, NULL))
    {
      status = CS_OS_INTERNAL;
      goto fail_ret;
    }

  status = read_raw_frame (pResult, &frameSize);
  if (status != CS_OK)
    goto fail_ret;

  {
      D_UINT32 frameId   = from_le_int32 (&pResult->data[FRAME_ID_OFF]);

      if ((frameId != 0)
          || (frameSize != (FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA))
          || (pResult->data[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT))
        {
          status = CS_UNEXPECTED_FRAME;
          goto fail_ret;
        }

      if ((pResult->data[FRAME_ENCTYPE_OFF] & pResult->encType) == 0)
        {
          status = CS_ENCTYPE_NOTSUPP;
          goto fail_ret;
        }

      assert (pResult->data[FRAME_ENCTYPE_OFF] == pResult->encType);
  }

  assert (pResult->encType == FRAME_ENCTYPE_PLAIN);
  {
    const D_UINT16 frameSize = FRAME_DATA_OFF +
                               FRAME_AUTH_CLNT_DATA +
                               strlen (pDatabaseName) + 1 +
                               passwordLen + 1;
    D_CHAR* const pAuthData =
        (D_CHAR*)&pResult->data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA];

    enum WH_SOCK_ERROR wss = WH_SOCK_OK;

    if (frameSize >= FRAME_MAX_SIZE)
      {
        status = CS_LARGE_ARGS;
        goto fail_ret;
      }

    pResult->data[FRAME_ENCTYPE_OFF] = FRAME_ENCTYPE_PLAIN;
    pResult->data[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT_RSP;
    store_le_int16 (frameSize, &pResult->data[FRAME_SIZE_OFF]);
    store_le_int32 (0, &pResult->data[FRAME_ID_OFF]);

    store_le_int32 (1, &pResult->data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_VER]);
    pResult->data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_USR] = userId;
    strcpy (pAuthData, pDatabaseName);
    strcpy (pAuthData + strlen (pDatabaseName) + 1, pPassword);

    if (! wh_socket_write (pResult->socket, pResult->data, frameSize, &wss))
      {
        assert (wss != WH_SOCK_OK);

        status = ((wss == WH_SOCK_EPIPE) || (wss == WH_SOCK_ETIMEOUT)) ?
                 CS_DROPPED :
                 CS_OS_INTERNAL;
        goto fail_ret;
      }
  }

  assert (status == CS_OK);

  *pHnd = pResult;
  return CS_OK;

fail_ret:
  assert (status != CS_OK);

  if (pResult != NULL)
    {
      if (pResult->socket != INVALID_SOCKET)
        wh_socket_close (pResult->socket);

      mem_free (pResult);
    }

  return status;
}

void
Close (CONNECTOR_HDL handler)
{
  struct INTERNAL_HANDLER* pHnd = (struct INTERNAL_HANDLER*)handler;

  if (pHnd->socket != INVALID_SOCKET)
    wh_socket_close (pHnd->socket);

  mem_free (pHnd);
}
