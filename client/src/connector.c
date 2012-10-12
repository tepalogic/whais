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
#include "utils/include/random.h"

#include "connector.h"
#include "client_connection.h"


static D_UINT16
max_data_size (const struct INTERNAL_HANDLER* const pHnd)
{
  assert (pHnd->encType == FRAME_ENCTYPE_PLAIN);

  return sizeof (pHnd->data) - (FRAME_DATA_OFF + PLAIN_DATA_OFF);
}

static D_UINT16
data_size (const struct INTERNAL_HANDLER* const pHnd)
{
  const D_UINT16 frameSize = from_le_int16 (&pHnd->data[FRAME_SIZE_OFF]);

  assert (pHnd->encType == FRAME_ENCTYPE_PLAIN);
  assert (frameSize >= (FRAME_DATA_OFF + PLAIN_DATA_OFF));
  assert (frameSize <= sizeof (pHnd->data));

  return frameSize - (FRAME_DATA_OFF + PLAIN_DATA_OFF);
}

static void
set_data_size (struct INTERNAL_HANDLER* const pHnd, const D_UINT16 size)
{
  const D_UINT16 frameSize = size + (FRAME_DATA_OFF + PLAIN_DATA_OFF);

  assert (size <= max_data_size (pHnd));

  store_le_int16 (frameSize, &pHnd->data[FRAME_SIZE_OFF]);
}

static D_UINT8*
raw_data (struct INTERNAL_HANDLER* const pHnd)
{
  assert (pHnd->encType == FRAME_ENCTYPE_PLAIN);

  return &pHnd->data[FRAME_DATA_OFF];
}

static D_UINT8*
data (struct INTERNAL_HANDLER* const pHnd)
{
  assert (pHnd->encType == FRAME_ENCTYPE_PLAIN);

  return &pHnd->data[FRAME_DATA_OFF + PLAIN_DATA_OFF];
}

static enum CONNECTOR_STATUS
translate_server_resp_code (const D_UINT8 code)
{
  switch (code)
  {
  case CMD_STATUS_OK:
    return CS_OK;
  case CMD_STATUS_INVAL_ARGS:
    return CS_INVALID_ARGS;
  case CMD_STATUS_TOOBIG:
    return CS_LARGE_ARGS;
  }

  assert (0);
  return CS_UNKNOWN_ERR;
}

static enum CONNECTOR_STATUS
send_raw_frame (struct INTERNAL_HANDLER* const pHnd,
                const D_UINT8                  type)
{
  const D_UINT16 frameSize = from_le_int16 (&pHnd->data[FRAME_SIZE_OFF]);

  assert (pHnd->encType == FRAME_ENCTYPE_PLAIN);
  assert ((frameSize > 0) && (frameSize <= sizeof (pHnd->data)));

  pHnd->data[FRAME_ENCTYPE_OFF] = pHnd->encType;
  pHnd->data[FRAME_TYPE_OFF]    = type;
  store_le_int32 (pHnd->expectedFrameId++, &pHnd->data[FRAME_ID_OFF]);

  if (! wh_socket_write (pHnd->socket, pHnd->data, frameSize, NULL))
    return CS_OS_INTERNAL;

  return CS_OK;
}

static enum CONNECTOR_STATUS
receive_raw_frame (struct INTERNAL_HANDLER* const pHnd)
{
  D_UINT16 frameSize;
  D_UINT16 frameRead = 0;

  while (frameRead < FRAME_DATA_OFF)
    {
      D_UINT chunkSize = FRAME_DATA_OFF - frameRead;
      if ( ! wh_socket_read (pHnd->socket,
                             &pHnd->data [frameRead],
                             &chunkSize,
                             NULL))
        {
          return CS_OS_INTERNAL;
        }
      else if (chunkSize == 0)
        return CS_DROPPED;

      frameRead += chunkSize;
    }

  frameSize = from_le_int16 (&pHnd->data[FRAME_SIZE_OFF]);
  if ((frameSize < FRAME_DATA_OFF)
      || (frameSize > FRAME_MAX_SIZE)
      || (pHnd->data[FRAME_ENCTYPE_OFF] != pHnd->encType))
    {
      return CS_COMM_OUT_OF_SYNC;
    }

  if (pHnd->data[FRAME_TYPE_TIMEOUT])
    return CS_CONNECTION_TIMEOUT;

  while (frameRead < frameSize)
    {
      D_UINT chunkSize = frameSize - frameRead;
      if ( ! wh_socket_read (pHnd->socket,
                             &pHnd->data [frameRead],
                             &chunkSize,
                             NULL))
        {
          return CS_OS_INTERNAL;
        }
      else if (chunkSize == 0)
        return CS_DROPPED;

      frameRead += chunkSize;
    }

  if (from_le_int32 (&pHnd->data[FRAME_ID_OFF]) != pHnd->expectedFrameId)
    return CS_UNEXPECTED_FRAME;

  return CS_OK;
}

static enum CONNECTOR_STATUS
send_command (struct INTERNAL_HANDLER* const pHnd,
              const D_UINT16                 commandId,
              const D_UINT16                 dataSize,
              const D_BOOL                   lastPart)
{
  D_UINT8*               pData  = raw_data (pHnd);
  D_UINT32               chkSum = 0;
  D_UINT16               index  = 0;
  enum  CONNECTOR_STATUS cs     = CS_OK;

  assert (pHnd->encType == FRAME_ENCTYPE_PLAIN);

  set_data_size (pHnd, dataSize);
  pHnd->clientCookie = w_rnd ();

  for (index = 0; index < dataSize; index++)
    {
      chkSum = (chkSum >> 1) + ((chkSum & 1) << 15);
      chkSum += pData [PLAIN_DATA_OFF + index];
      chkSum &= 0xFFFFF;
    }

  store_le_int32 (pHnd->clientCookie, &pData[PLAIN_CLNT_COOKIE_OFF]);
  store_le_int32 (pHnd->serverCookie, &pData[PLAIN_SERV_COOKIE_OFF]);
  store_le_int16 (commandId, &pData[PLAIN_TYPE_OFF]);
  store_le_int16 (chkSum, &pData[PLAIN_CRC_OFF]);

  if (lastPart)
    {
      cs = send_raw_frame (pHnd, FRAME_TYPE_NORMAL);
      return cs;
    }

  if ((cs = send_raw_frame (pHnd, FRAME_TYPE_PARTIAL)) != CS_OK)
    goto send_failure;

  if ((cs = receive_raw_frame (pHnd)) != CS_OK)
    goto send_failure;

  pData = raw_data (pHnd);

  if ((pHnd->data[FRAME_TYPE_OFF] != FRAME_TYPE_PARTIAL_ACK)
      || (from_le_int32 (&pData[PLAIN_CLNT_COOKIE_OFF]) != pHnd->clientCookie)
      || (from_le_int16 (&pData[PLAIN_CRC_OFF]) != 0)
      || (from_le_int32 (&pData[PLAIN_TYPE_OFF]) != commandId)
      || (data_size (pHnd) != 0))
    {
      cs = CS_UNEXPECTED_FRAME;
      goto send_failure;
    }

  pHnd->serverCookie = from_le_int32 (&pData[PLAIN_SERV_COOKIE_OFF]);

  return CS_OK;

send_failure:
  wh_socket_close (pHnd->socket);
  pHnd->socket = INVALID_SOCKET;

  return cs;
}

static enum CONNECTOR_STATUS
recieve_answer (struct INTERNAL_HANDLER* const pHnd,
                D_UINT16* const                respType,
                D_BOOL* const                  lastPart)
{
  enum CONNECTOR_STATUS cs        = receive_raw_frame (pHnd);
  D_UINT32              chkSum    = 0;
  const D_UINT16        dataSize  = data_size (pHnd);
  D_UINT16              index     = 0;

  for (index = 0; index < dataSize; index++)
    {
      chkSum = (chkSum >> 1) + ((chkSum & 1) << 15);
      chkSum += data (pHnd) [index];
      chkSum &= 0xFFFFF;
    }

  if (chkSum != from_le_int16 (&raw_data (pHnd)[PLAIN_CRC_OFF]))
    {
      cs = CS_INVALID_FRAME;
      goto recieve_failure;
    }
  else if (from_le_int32 (&raw_data (pHnd)[PLAIN_CLNT_COOKIE_OFF]) !=
           pHnd->clientCookie)
    {
      cs = CS_UNEXPECTED_FRAME;
      goto recieve_failure;
    }

  pHnd->serverCookie = from_le_int32 (&raw_data (pHnd)[PLAIN_SERV_COOKIE_OFF]);
  *respType = from_le_int16 (&raw_data (pHnd)[PLAIN_TYPE_OFF]);
  if ((*respType == CMD_INVALID_RSP) || ((*respType & 1) == 0))
    {
      cs = CS_INVALID_FRAME;
      goto recieve_failure;
    }

  switch (pHnd->data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
    *lastPart = TRUE;
    break;
  case FRAME_TYPE_PARTIAL:
    *lastPart = FALSE;
  default:
    cs = CS_UNEXPECTED_FRAME;
    goto recieve_failure;
  }

  return CS_OK;

recieve_failure:
  wh_socket_close (pHnd->socket);
  pHnd->socket = INVALID_SOCKET;

  return cs;
}

static enum CONNECTOR_STATUS
ack_answer_part (struct INTERNAL_HANDLER* const pHnd,
                 const  D_BOOL                  getNextPart)
{
  D_UINT8 *pRawData = raw_data (pHnd);

  assert (pHnd->lastCmdRespFrameReceived == FALSE);

  set_data_size (pHnd, 0);

  pHnd->clientCookie = w_rnd ();
  store_le_int32 (pHnd->clientCookie, &pRawData[PLAIN_CLNT_COOKIE_OFF]);
  store_le_int32 (pHnd->serverCookie, &pRawData[PLAIN_SERV_COOKIE_OFF]);
  store_le_int16 (pHnd->lastCmdRespReceived, &pRawData[PLAIN_TYPE_OFF]);
  store_le_int16 (0, &pRawData[PLAIN_CRC_OFF]);

  if (getNextPart)
    {
      D_UINT16 type = CMD_INVALID_RSP;

      enum CONNECTOR_STATUS cs = send_raw_frame (pHnd, FRAME_TYPE_PARTIAL_ACK);
      if (cs != CS_OK)
        return cs;

       cs = recieve_answer (pHnd, &type, &pHnd->lastCmdRespFrameReceived);
       if ((cs == CS_OK) && (type != pHnd->lastCmdRespReceived))
         return CS_UNEXPECTED_FRAME;

       return cs;
    }

  pHnd->lastCmdRespFrameReceived = TRUE;
  return send_raw_frame (pHnd, FRAME_TYPE_PARTIAL_CANCEL);
};

enum CONNECTOR_STATUS
Connect (const char* const   pHost,
         const char* const   pPort,
         const char* const   pDatabaseName,
         const char* const   pPassword,
         const unsigned int  userId,
         CONNECTOR_HND*      pHnd)
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

  *pHnd = NULL;
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
Close (CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* pHnd = (struct INTERNAL_HANDLER*)hnd;

  if (pHnd == NULL)
    return ;

  if (pHnd->socket != INVALID_SOCKET)
    {
      send_command (pHnd, CMD_CLOSE_CONN, 0, TRUE);
      wh_socket_close (pHnd->socket);
    }
  mem_free (pHnd);
}

enum CONNECTOR_STATUS
PingServer (const CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* pHnd = (struct INTERNAL_HANDLER*)hnd;

  D_UINT16 type;
  D_BOOL   lastPart;

  enum CONNECTOR_STATUS cs = CS_OK;

  if (hnd == NULL)
    return CS_INVALID_ARGS;
  else if (pHnd->lastCmdRespFrameReceived == FALSE)
    {
      if ((cs = ack_answer_part (pHnd, FALSE)) != CS_OK)
        goto exit_ping_server;
    }

  if ((cs = send_command (pHnd, CMD_PING_SERVER, 0, TRUE)) != CS_OK)
    goto exit_ping_server;

  if ((cs = recieve_answer (pHnd, &type, &lastPart)) != CS_OK)
    goto exit_ping_server;

  if ((type != CMD_PING_SERVER_RSP)
      || (lastPart == FALSE)
      || (data_size (pHnd) != 0))
    {
      cs = CS_INVALID_FRAME;
    }

exit_ping_server:
  return cs;
};

enum CONNECTOR_STATUS
ListGlobals (const CONNECTOR_HND hnd, unsigned int* pGlbsCount)
{
  struct INTERNAL_HANDLER* pHnd = (struct INTERNAL_HANDLER*)hnd;

  D_UINT8* pData = NULL;
  D_UINT16 type;
  D_BOOL   lastPart;

  enum CONNECTOR_STATUS cs = CS_OK;

  if (hnd == NULL)
    return CS_INVALID_ARGS;
  else if (pHnd->userId != 0)
    return CS_OP_NOTPERMITED;
  else if (pHnd->lastCmdRespFrameReceived == FALSE)
    {
      if ((cs = ack_answer_part (pHnd, FALSE)) != CS_OK)
        goto list_globals_err;
    }

  if ((cs = send_command (pHnd, CMD_LIST_GLOBALS, 0, TRUE)) != CS_OK)
    goto list_globals_err;

  if ((cs = recieve_answer (pHnd, &type, &lastPart)) != CS_OK)
    goto list_globals_err;
  else if (type != CMD_LIST_GLOBALS_RSP)
    {
      cs = CS_INVALID_FRAME;
      goto list_globals_err;
    }

  pData = data (pHnd);
  if ((cs = translate_server_resp_code (pData[0])) != CS_OK)
    goto list_globals_err;

  *pGlbsCount = from_le_int32 (pData + sizeof (D_UINT8));

  if (((*pGlbsCount == 0) && (lastPart == FALSE))
      || (*pGlbsCount < pData[sizeof (D_UINT8) + sizeof (D_UINT32)]))
    {
      cs = CS_INVALID_FRAME;
      goto list_globals_err;
    }

  pHnd->lastCmdRespReceived      = type;
  pHnd->lastCmdRespFrameReceived = lastPart;

  pHnd->cmdInternal = 0;
  pHnd->cmdInternal |= pData[sizeof (D_UINT8) + sizeof (D_UINT32)];
  pHnd->cmdInternal |= (0 << 8);
  pHnd->cmdInternal |=
      ((sizeof (D_UINT8) + sizeof (D_UINT32) + sizeof (D_UINT8)) << 16);

  return CS_OK;

list_globals_err:
  return cs;
}

enum CONNECTOR_STATUS
GlobalFetch (const CONNECTOR_HND hnd, const unsigned char** pGlbName)
{
  struct INTERNAL_HANDLER* pHnd = (struct INTERNAL_HANDLER*)hnd;

  D_UINT8* pData = NULL;
  D_UINT16 frameDataIndex;
  D_UINT8  frameGlbs;
  D_UINT8  fetchedFrameGlbs;

  enum CONNECTOR_STATUS cs = CS_OK;

  if ((pGlbName == NULL)
      || (pHnd == NULL)
      || (pHnd->lastCmdRespReceived != CMD_LIST_GLOBALS_RSP))
    {
      cs = CS_INVALID_ARGS;
      goto fetch_global_exit;
    }

  pData            = data (pHnd);
  frameGlbs        = pHnd->cmdInternal & 0xFF;
  fetchedFrameGlbs = (pHnd->cmdInternal >> 8) & 0xFF;
  frameDataIndex   = (pHnd->cmdInternal >> 16) & 0xFFFF;

  assert (fetchedFrameGlbs <= frameGlbs);
  assert (frameDataIndex <= data_size (pHnd));

  if (fetchedFrameGlbs < frameGlbs)
    {
      *pGlbName = pData + frameDataIndex;
      frameDataIndex += strlen ((D_CHAR*)*pGlbName) + 1;
      ++fetchedFrameGlbs;
    }
  else
    {
      assert (fetchedFrameGlbs == frameGlbs);
      if (pHnd->lastCmdRespFrameReceived == FALSE)
        {
          cs = ack_answer_part (pHnd, TRUE);
          if (cs != CS_OK)
            goto fetch_global_exit;

          pData = data (pHnd);
          if ((cs = translate_server_resp_code (pData[0])) != CS_OK)
            goto fetch_global_exit;

          frameGlbs = pData[sizeof(D_UINT8)];
          if (frameGlbs == 0)
            {
             cs = CS_INVALID_FRAME;
             goto fetch_global_exit;
            }

          fetchedFrameGlbs = 1;
          frameDataIndex = sizeof (D_UINT8) + sizeof (D_UINT8);
          *pGlbName = pData + frameDataIndex;

          frameDataIndex += strlen ((D_CHAR*)*pGlbName) + 1;

          assert (frameDataIndex <= data_size (pHnd));
        }
      else
        {
          *pGlbName = NULL;
          cs = CS_OK;

          goto fetch_global_exit;
        }
    }

  pHnd->cmdInternal = 0;
  pHnd->cmdInternal |= frameGlbs;
  pHnd->cmdInternal |= (fetchedFrameGlbs << 8);
  pHnd->cmdInternal |= (frameDataIndex << 16);

  return CS_OK;

fetch_global_exit:
  return cs;
}

enum CONNECTOR_STATUS
GlobalFetchCancel (const CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* pHnd = (struct INTERNAL_HANDLER*)hnd;

  if ((pHnd == NULL)
      || (pHnd->lastCmdRespFrameReceived != FALSE)
      || (pHnd->lastCmdRespReceived != CMD_LIST_GLOBALS_RSP))
    {
      return CS_INVALID_ARGS;
    }

  return ack_answer_part (pHnd, FALSE);
}
