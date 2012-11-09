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
max_data_size (const struct INTERNAL_HANDLER* const hnd)
{
  assert (hnd->encType == FRAME_ENCTYPE_PLAIN);

  return sizeof (hnd->data) - (FRAME_DATA_OFF + PLAIN_DATA_OFF);
}

static D_UINT16
data_size (const struct INTERNAL_HANDLER* const hnd)
{
  const D_UINT16 frameSize = from_le_int16 (&hnd->data[FRAME_SIZE_OFF]);

  assert (hnd->encType == FRAME_ENCTYPE_PLAIN);
  assert (frameSize >= (FRAME_DATA_OFF + PLAIN_DATA_OFF));
  assert (frameSize <= sizeof (hnd->data));

  return frameSize - (FRAME_DATA_OFF + PLAIN_DATA_OFF);
}

static void
set_data_size (struct INTERNAL_HANDLER* const hnd, const D_UINT16 size)
{
  const D_UINT16 frameSize = size + (FRAME_DATA_OFF + PLAIN_DATA_OFF);

  assert (size <= max_data_size (hnd));

  store_le_int16 (frameSize, &hnd->data[FRAME_SIZE_OFF]);
}

static D_UINT8*
raw_data (struct INTERNAL_HANDLER* const hnd)
{
  assert (hnd->encType == FRAME_ENCTYPE_PLAIN);

  return &hnd->data[FRAME_DATA_OFF];
}

static D_UINT8*
data (struct INTERNAL_HANDLER* const hnd)
{
  assert (hnd->encType == FRAME_ENCTYPE_PLAIN);

  return &hnd->data[FRAME_DATA_OFF + PLAIN_DATA_OFF];
}

static D_UINT
send_raw_frame (struct INTERNAL_HANDLER* const hnd,
                const D_UINT8                  type)
{
  D_UINT32 status = 0;

  const D_UINT16 frameSize = from_le_int16 (&hnd->data[FRAME_SIZE_OFF]);

  assert (hnd->encType == FRAME_ENCTYPE_PLAIN);
  assert ((frameSize > 0) && (frameSize <= sizeof (hnd->data)));

  hnd->data[FRAME_ENCTYPE_OFF] = hnd->encType;
  hnd->data[FRAME_TYPE_OFF]    = type;
  store_le_int32 (hnd->expectedFrameId++, &hnd->data[FRAME_ID_OFF]);

  status = wh_socket_write (hnd->socket, hnd->data, frameSize);
  if (status != WOP_OK)
    return CS_OS_ERR_BASE + status;

  return CS_OK;
}

static D_UINT
receive_raw_frame (struct INTERNAL_HANDLER* const hnd)
{
  D_UINT16 frameSize;
  D_UINT16 frameRead = 0;

  while (frameRead < FRAME_DATA_OFF)
    {
      D_UINT chunkSize = FRAME_DATA_OFF - frameRead;

      const D_UINT32 status  = wh_socket_read (hnd->socket,
                                               &hnd->data [frameRead],
                                               &chunkSize);
      if (status != WOP_OK)
        return CS_OS_ERR_BASE + status;
      else if (chunkSize == 0)
        return CS_DROPPED;

      frameRead += chunkSize;
    }

  frameSize = from_le_int16 (&hnd->data[FRAME_SIZE_OFF]);
  if ((frameSize < FRAME_DATA_OFF)
      || (frameSize > FRAME_MAX_SIZE)
      || (hnd->data[FRAME_ENCTYPE_OFF] != hnd->encType))
    {
      return CS_COMM_OUT_OF_SYNC;
    }

  if (hnd->data[FRAME_TYPE_TIMEOUT])
    return CS_CONNECTION_TIMEOUT;

  while (frameRead < frameSize)
    {
      D_UINT chunkSize = frameSize - frameRead;
      const D_UINT32 status = wh_socket_read (hnd->socket,
                                              &hnd->data [frameRead],
                                              &chunkSize);
      if (status != WOP_OK)
        return CS_OS_ERR_BASE + status;
      else if (chunkSize == 0)
        return CS_DROPPED;

      frameRead += chunkSize;
    }

  if (from_le_int32 (&hnd->data[FRAME_ID_OFF]) != hnd->expectedFrameId)
    return CS_UNEXPECTED_FRAME;

  return CS_OK;
}

static D_UINT
send_command (struct INTERNAL_HANDLER* const hnd,
              const D_UINT16                 commandId,
              const D_BOOL                   lastPart)
{
  D_UINT8*               pData    = raw_data (hnd);
  D_UINT32               chkSum   = 0;
  D_UINT16               index    = 0;
  const D_UINT16         dataSize = data_size (hnd);
  D_UINT                 cs       = CS_OK;

  assert (hnd->encType == FRAME_ENCTYPE_PLAIN);

  hnd->clientCookie = w_rnd ();

  for (index = 0; index < dataSize; index++)
    {
      chkSum = (chkSum >> 1) + ((chkSum & 1) << 15);
      chkSum += pData [PLAIN_DATA_OFF + index];
      chkSum &= 0xFFFFF;
    }

  store_le_int32 (hnd->clientCookie, &pData[PLAIN_CLNT_COOKIE_OFF]);
  store_le_int32 (hnd->serverCookie, &pData[PLAIN_SERV_COOKIE_OFF]);
  store_le_int16 (commandId, &pData[PLAIN_TYPE_OFF]);
  store_le_int16 (chkSum, &pData[PLAIN_CRC_OFF]);

  if (lastPart)
    {
      cs = send_raw_frame (hnd, FRAME_TYPE_NORMAL);
      return cs;
    }

  if ((cs = send_raw_frame (hnd, FRAME_TYPE_PARTIAL)) != CS_OK)
    goto send_failure;

  if ((cs = receive_raw_frame (hnd)) != CS_OK)
    goto send_failure;

  pData = raw_data (hnd);

  if ((hnd->data[FRAME_TYPE_OFF] != FRAME_TYPE_PARTIAL_ACK)
      || (from_le_int32 (&pData[PLAIN_CLNT_COOKIE_OFF]) != hnd->clientCookie)
      || (from_le_int16 (&pData[PLAIN_CRC_OFF]) != 0)
      || (from_le_int32 (&pData[PLAIN_TYPE_OFF]) != commandId)
      || (data_size (hnd) != 0))
    {
      cs = CS_UNEXPECTED_FRAME;
      goto send_failure;
    }

  hnd->serverCookie = from_le_int32 (&pData[PLAIN_SERV_COOKIE_OFF]);

  return CS_OK;

send_failure:
  wh_socket_close (hnd->socket);
  hnd->socket = INVALID_SOCKET;

  return cs;
}

static D_UINT
recieve_answer (struct INTERNAL_HANDLER* const hnd,
                D_UINT16* const                pRespType,
                D_BOOL* const                  pLastPart)
{
  D_UINT                cs        = receive_raw_frame (hnd);
  D_UINT32              chkSum    = 0;
  const D_UINT16        dataSize  = data_size (hnd);
  D_UINT16              index     = 0;

  for (index = 0; index < dataSize; index++)
    {
      chkSum = (chkSum >> 1) + ((chkSum & 1) << 15);
      chkSum += data (hnd) [index];
      chkSum &= 0xFFFFF;
    }

  if (chkSum != from_le_int16 (&raw_data (hnd)[PLAIN_CRC_OFF]))
    {
      cs = CS_INVALID_FRAME;
      goto recieve_failure;
    }
  else if (from_le_int32 (&raw_data (hnd)[PLAIN_CLNT_COOKIE_OFF]) !=
           hnd->clientCookie)
    {
      cs = CS_UNEXPECTED_FRAME;
      goto recieve_failure;
    }

  hnd->serverCookie = from_le_int32 (&raw_data (hnd)[PLAIN_SERV_COOKIE_OFF]);
  *pRespType = from_le_int16 (&raw_data (hnd)[PLAIN_TYPE_OFF]);
  if ((*pRespType == CMD_INVALID_RSP) || ((*pRespType & 1) == 0))
    {
      cs = CS_INVALID_FRAME;
      goto recieve_failure;
    }

  switch (hnd->data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
    *pLastPart = TRUE;
    break;
  case FRAME_TYPE_PARTIAL:
    *pLastPart = FALSE;
  default:
    cs = CS_UNEXPECTED_FRAME;
    goto recieve_failure;
  }

  hnd->lastCmdRespReceived        = *pRespType;
  hnd->lastCmdRespFrameReceived   = *pLastPart;

  return CS_OK;

recieve_failure:

  hnd->lastCmdRespReceived      = CMD_INVALID_RSP;
  hnd->lastCmdRespFrameReceived = FALSE;

  wh_socket_close (hnd->socket);
  hnd->socket = INVALID_SOCKET;

  return cs;
}

static D_UINT
ack_answer_part (struct INTERNAL_HANDLER* const hnd,
                 const  D_BOOL                  waitingNextPart)
{
  D_UINT8* rawData_ = raw_data (hnd);

  assert (hnd->lastCmdRespFrameReceived == FALSE);

  set_data_size (hnd, 0);

  hnd->clientCookie = w_rnd ();
  store_le_int32 (hnd->clientCookie, &rawData_[PLAIN_CLNT_COOKIE_OFF]);
  store_le_int32 (hnd->serverCookie, &rawData_[PLAIN_SERV_COOKIE_OFF]);
  store_le_int16 (hnd->lastCmdRespReceived, &rawData_[PLAIN_TYPE_OFF]);
  store_le_int16 (0, &rawData_[PLAIN_CRC_OFF]);

  if (waitingNextPart)
    {
      D_UINT16 type = CMD_INVALID_RSP;

      D_UINT cs = send_raw_frame (hnd, FRAME_TYPE_PARTIAL_ACK);
      if (cs != CS_OK)
        return cs;

       cs = recieve_answer (hnd, &type, &hnd->lastCmdRespFrameReceived);
       if ((cs == CS_OK) && (type != hnd->lastCmdRespReceived))
         return CS_UNEXPECTED_FRAME;

       return cs;
    }

  hnd->lastCmdRespFrameReceived = TRUE;
  return send_raw_frame (hnd, FRAME_TYPE_PARTIAL_CANCEL);
};

D_UINT
Connect (const char* const    host,
         const char* const    port,
         const char* const    databaseName,
         const char* const    password,
         const unsigned int   userId,
         CONNECTOR_HND* const pHnd)
{
  struct INTERNAL_HANDLER* result      = NULL;
  const D_UINT             passwordLen = strlen (password);
  D_UINT16                 frameSize   = 0;
  D_UINT32                 status      = CS_OK;

  if ((host == NULL)
      || (port == NULL)
      || (databaseName == NULL)
      || (password == NULL)
      || (pHnd == NULL)
      || (strlen (host) == 0)
      || (strlen (port) == 0)
      || (strlen (databaseName) == 0))
    {
      status = CS_INVALID_ARGS;
      goto fail_ret;
    }

  *pHnd = NULL;
  result = mem_alloc (sizeof (*result) + passwordLen);
  memset (result, 0, sizeof (*result));

  result->userId     = userId;
  result->encKeySize = passwordLen;
  result->socket     = INVALID_SOCKET;
  result->encType    = FRAME_ENCTYPE_PLAIN;
  memcpy (result->encriptionKey, password, passwordLen);

  status = wh_socket_client (host, port, &result->socket);
  if (status != WOP_OK)
    {
      status += CS_OS_ERR_BASE;
      goto fail_ret;
    }

  status = read_raw_frame (result, &frameSize);
  if (status != CS_OK)
    goto fail_ret;

  {
      D_UINT32 frameId   = from_le_int32 (&result->data[FRAME_ID_OFF]);

      if ((frameId != 0)
          || (frameSize != (FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA))
          || (result->data[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT))
        {
          status = CS_UNEXPECTED_FRAME;
          goto fail_ret;
        }

      if ((result->data[FRAME_ENCTYPE_OFF] & result->encType) == 0)
        {
          status = CS_ENCTYPE_NOTSUPP;
          goto fail_ret;
        }

      assert (result->data[FRAME_ENCTYPE_OFF] == result->encType);
  }

  assert (result->encType == FRAME_ENCTYPE_PLAIN);
  {
    D_UINT32 wss;

    const D_UINT16 frameSize = FRAME_DATA_OFF +
                               FRAME_AUTH_CLNT_DATA +
                               strlen (databaseName) + 1 +
                               passwordLen + 1;
    D_CHAR* const pAuthData =
        (D_CHAR*)&result->data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA];

    if (frameSize >= FRAME_MAX_SIZE)
      {
        status = CS_LARGE_ARGS;
        goto fail_ret;
      }

    result->data[FRAME_ENCTYPE_OFF] = FRAME_ENCTYPE_PLAIN;
    result->data[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT_RSP;
    store_le_int16 (frameSize, &result->data[FRAME_SIZE_OFF]);
    store_le_int32 (0, &result->data[FRAME_ID_OFF]);

    store_le_int32 (1, &result->data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_VER]);
    result->data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_USR] = userId;
    strcpy (pAuthData, databaseName);
    strcpy (pAuthData + strlen (databaseName) + 1, password);

    wss = wh_socket_write (result->socket, result->data, frameSize);
    if (wss != WOP_OK)
      goto fail_ret;
  }

  assert (status == CS_OK);

  *pHnd = result;
  return CS_OK;

fail_ret:
  assert (status != CS_OK);

  if (result != NULL)
    {
      if (result->socket != INVALID_SOCKET)
        wh_socket_close (result->socket);

      mem_free (result);
    }

  return status;
}

void
Close (CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if (hnd_ == NULL)
    return ;

  if (hnd_->socket != INVALID_SOCKET)
    {
      set_data_size (hnd_, 0);
      send_command (hnd_, CMD_CLOSE_CONN, TRUE);
      wh_socket_close (hnd_->socket);
    }
  mem_free (hnd_);
}

D_UINT
PingServer (const CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT16 type;
  D_BOOL   lastPart;

  D_UINT cs = CS_OK;

  if (hnd == NULL)
    return CS_INVALID_ARGS;
  else if (hnd_->lastCmdRespFrameReceived == FALSE)
    {
      if ((cs = ack_answer_part (hnd_, FALSE)) != CS_OK)
        goto exit_ping_server;
    }

  set_data_size (hnd_, 0);
  if ((cs = send_command (hnd_, CMD_PING_SERVER, TRUE)) != CS_OK)
    goto exit_ping_server;

  if ((cs = recieve_answer (hnd_, &type, &lastPart)) != CS_OK)
    goto exit_ping_server;

  if ((type != CMD_PING_SERVER_RSP)
      || (lastPart == FALSE)
      || (data_size (hnd_) != 0))
    {
      cs = CS_INVALID_FRAME;
    }

exit_ping_server:
  return cs;
};

D_UINT
ListGlobals (const CONNECTOR_HND hnd, unsigned int* pGlbsCount)
{
  struct INTERNAL_HANDLER* pHnd = (struct INTERNAL_HANDLER*)hnd;

  D_UINT8* pData = NULL;
  D_UINT16 type;
  D_BOOL   lastPart;

  D_UINT cs = CS_OK;

  if ((hnd == NULL) || (pGlbsCount == NULL))
    return CS_INVALID_ARGS;
  else if (pHnd->userId != 0)
    return CS_OP_NOTPERMITED;
  else if (pHnd->lastCmdRespFrameReceived == FALSE)
    {
      if ((cs = ack_answer_part (pHnd, FALSE)) != CS_OK)
        goto list_globals_err;
    }

  set_data_size (pHnd, 0);
  if ((cs = send_command (pHnd, CMD_LIST_GLOBALS, TRUE)) != CS_OK)
    goto list_globals_err;

  if ((cs = recieve_answer (pHnd, &type, &lastPart)) != CS_OK)
    goto list_globals_err;
  else if (type != CMD_LIST_GLOBALS_RSP)
    {
      cs = CS_INVALID_FRAME;
      goto list_globals_err;
    }

  pData = data (pHnd);
  if ((cs = pData[0]) != CS_OK)
    goto list_globals_err;

  *pGlbsCount = from_le_int32 (pData + sizeof (D_UINT8));

  if (((*pGlbsCount == 0) && (lastPart == FALSE))
      || (*pGlbsCount < pData[sizeof (D_UINT8) + sizeof (D_UINT32)]))
    {
      cs = CS_INVALID_FRAME;
      goto list_globals_err;
    }

  assert (pHnd->lastCmdRespReceived == type);
  assert (pHnd->lastCmdRespFrameReceived == lastPart);

  pHnd->cmdInternal = 0;
  pHnd->cmdInternal |= pData[sizeof (D_UINT8) + sizeof (D_UINT32)];
  pHnd->cmdInternal |= (0 << 8);
  pHnd->cmdInternal |=
      ((sizeof (D_UINT8) + sizeof (D_UINT32) + sizeof (D_UINT8)) << 16);

  return CS_OK;

list_globals_err:
  return cs;
}

D_UINT
ListGlobalsFetch (const CONNECTOR_HND hnd, const char** ppGlbName)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT8* data_ = NULL;
  D_UINT16 frameDataIndex;
  D_UINT8  frameGlbs;
  D_UINT8  fetchedFrameGlbs;

  D_UINT cs = CS_OK;

  if ((ppGlbName == NULL)
      || (hnd_ == NULL)
      || (hnd_->lastCmdRespReceived != CMD_LIST_GLOBALS_RSP))
    {
      cs = CS_INVALID_ARGS;
      goto fetch_global_exit;
    }

  data_            = data (hnd_);
  frameGlbs        = hnd_->cmdInternal & 0xFF;
  fetchedFrameGlbs = (hnd_->cmdInternal >> 8) & 0xFF;
  frameDataIndex   = (hnd_->cmdInternal >> 16) & 0xFFFF;

  assert (fetchedFrameGlbs <= frameGlbs);
  assert (frameDataIndex <= data_size (hnd_));

  if (fetchedFrameGlbs < frameGlbs)
    {
      *ppGlbName = (const char*)data_ + frameDataIndex;
      frameDataIndex += strlen ((D_CHAR*)*ppGlbName) + 1;
      ++fetchedFrameGlbs;
    }
  else
    {
      assert (fetchedFrameGlbs == frameGlbs);
      if (hnd_->lastCmdRespFrameReceived == FALSE)
        {
          cs = ack_answer_part (hnd_, TRUE);
          if (cs != CS_OK)
            goto fetch_global_exit;

          data_ = data (hnd_);
          if ((cs = data_[0]) != CS_OK)
            goto fetch_global_exit;

          frameGlbs = data_[sizeof(D_UINT8)];
          if (frameGlbs == 0)
            {
             cs = CS_INVALID_FRAME;
             goto fetch_global_exit;
            }

          fetchedFrameGlbs = 1;
          frameDataIndex = sizeof (D_UINT8) + sizeof (D_UINT8);
          *ppGlbName = (const char*)data_ + frameDataIndex;

          frameDataIndex += strlen ((D_CHAR*)*ppGlbName) + 1;

          assert (frameDataIndex <= data_size (hnd_));
        }
      else
        {
          *ppGlbName = NULL;
          cs = CS_OK;

          goto fetch_global_exit;
        }
    }

  hnd_->cmdInternal = 0;
  hnd_->cmdInternal |= frameGlbs;
  hnd_->cmdInternal |= (fetchedFrameGlbs << 8);
  hnd_->cmdInternal |= (frameDataIndex << 16);

  return CS_OK;

fetch_global_exit:
  return cs;
}

D_UINT
ListGlobalsFetchCancel (const CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if ((hnd_ == NULL)
      || (hnd_->lastCmdRespFrameReceived != FALSE)
      || (hnd_->lastCmdRespReceived != CMD_LIST_GLOBALS_RSP))
    {
      return CS_INVALID_ARGS;
    }

  return ack_answer_part (hnd_, FALSE);
}


D_UINT
DescribeValue (const CONNECTOR_HND    hnd,
               const char* const      globalName,
               unsigned int* const    pTypeDescSize)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT8* data_    = NULL;
  D_UINT16 type     = CMD_INVALID_RSP;
  D_UINT16 dataSize = strlen (globalName) + sizeof (D_UINT16) + 1;
  D_BOOL   lastPart;

  D_UINT cs = CS_OK;

  if ((hnd == NULL) || (pTypeDescSize == NULL))
    return CS_INVALID_ARGS;
  else if (hnd_->userId != 0)
    return CS_OP_NOTPERMITED;
  else if (dataSize > max_data_size (hnd_))
    return CS_LARGE_ARGS;
  else if (hnd_->lastCmdRespFrameReceived == FALSE)
    {
      if ((cs = ack_answer_part (hnd_, FALSE)) != CS_OK)
        goto global_desc_exit;
    }

  set_data_size (hnd_, dataSize);
  data_ = data (hnd_);
  store_le_int16 (dataSize - sizeof (D_UINT16), data_);
  strcpy ((D_CHAR*)&data_[sizeof (D_UINT16)], globalName);

  if ((cs = send_command (hnd_, CMD_GLOBAL_DESC, TRUE)) != CS_OK)
    goto global_desc_exit;

  if ((cs = recieve_answer (hnd_, &type, &lastPart)) != CS_OK)
    goto global_desc_exit;
  else if (type != CMD_GLOBAL_DESC_RSP)
    {
      cs = CS_INVALID_FRAME;
      goto global_desc_exit;
    }

  data_ = data (hnd_);
  if ((cs = data_[0]) != CS_OK)
    goto global_desc_exit;

  *pTypeDescSize = from_le_int16 (
                            data_ + sizeof (D_UINT8) + (2 * sizeof (D_UINT16))
                                  );
  *pTypeDescSize += 2 * sizeof (D_UINT16);

  dataSize = from_le_int16 (data_ + sizeof (D_UINT8));
  hnd_->cmdInternal = dataSize & 0xFFFF;

  return CS_OK;

global_desc_exit:
  return cs;
}

D_UINT
DescribeValueFetch (const CONNECTOR_HND         hnd,
                    const unsigned char** const ppGlbTypeDataChunk,
                    unsigned int* const         pChunkSize)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT8* data_ = NULL;
  D_UINT16 typeFrameSize;
  D_UINT16 typeIndex;

  D_UINT cs = CS_OK;

  if ((ppGlbTypeDataChunk == NULL)
      || (pChunkSize == NULL)
      || (hnd_ == NULL)
      || (hnd_->lastCmdRespReceived != CMD_GLOBAL_DESC_RSP))
    {
      cs = CS_INVALID_ARGS;
      goto global_desc_fetch_exit;
    }

  data_            = data (hnd_);
  typeFrameSize    = hnd_->cmdInternal & 0xFFFF;
  typeIndex        = (hnd_->cmdInternal >> 16) & 0xFFFF;

  assert (typeFrameSize > 0);

  if (typeIndex == typeFrameSize)
    {
      if (hnd_->lastCmdRespFrameReceived)
        {
          *ppGlbTypeDataChunk = NULL;
          *pChunkSize        = 0;
          return CS_OK;
        }
      else if ((cs = ack_answer_part (hnd_, TRUE)) != CS_OK)
        goto global_desc_fetch_exit;

      data_ = data (hnd);
      if ((cs = data_[0]) != CS_OK)
        goto global_desc_fetch_exit;

      typeFrameSize = from_le_int16 (data_ + sizeof (D_UINT8));
      typeIndex     = 0;
    }

  assert (typeIndex == 0);
  *ppGlbTypeDataChunk = data_ + sizeof (D_UINT8) + sizeof (D_UINT16);
  *pChunkSize        = typeFrameSize;
  typeIndex           = typeFrameSize;

  hnd_->cmdInternal =  typeFrameSize;
  hnd_->cmdInternal |= (typeIndex & 0xFFFF) << 16;

  return CS_OK;

global_desc_fetch_exit:
  return cs;
}

D_UINT
DescribeValueFetchCancel (const CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if ((hnd_ == NULL)
      || (hnd_->lastCmdRespFrameReceived != FALSE)
      || (hnd_->lastCmdRespReceived != CMD_GLOBAL_DESC_RSP))
    {
      return CS_INVALID_ARGS;
    }

  return ack_answer_part (hnd_, FALSE);
}

static D_UINT
compute_push_space_need (const D_UINT                        type,
                         unsigned int                        fieldsCount,
                         const struct FieldDescriptor* const fields)
{
  D_UINT result = 1;

  if (type != FT_TABLE_MASK)
    {
      const D_BOOL isArray = (type & FT_ARRAY_MASK) != 0;
      if ((((type & 0xFFFF) < FT_BOOL) || ((type & 0XFFFF) > FT_TEXT))
          || (isArray && ((type & 0xFFFF) == FT_TEXT)))
        {
          return 0;
        }
      else
        return sizeof (D_UINT16);
    }
  else if (type == FT_TABLE_MASK)
    {
      result += sizeof (D_UINT16);

      if ((fieldsCount == 0) || (fields == NULL))
        return 0;

      result += sizeof (D_UINT16);
      while (fieldsCount-- > 0)
        {
          const D_CHAR* const fieldName = fields[fieldsCount].m_FieldName;

          D_UINT16     fieldType = fields[fieldsCount].m_FieldType;
          const D_BOOL isArray   = (fieldType & FT_ARRAY_MASK) != 0;
          fieldType &= ~FT_ARRAY_MASK;

          if ((fieldName == NULL)
              || (fieldName[0] == 0)
              || (isArray && (fieldType == FT_TEXT))
              || ((fieldType < FT_BOOL) || (fieldType > FT_TEXT)))
            {
              return 0;
            }
          result += strlen (fieldName) + 1;
          result += sizeof (D_UINT16);
        }
    }
  /* else: Type is not looking good! Signal it as an error result! */

  return result;
}

static void
write_push_cmd (const D_UINT                        type,
                unsigned int                        fieldsCount,
                const struct FieldDescriptor* const fields,
                D_UINT8*                            dest)
{
  *dest++ = CMD_UPDATE_FUNC_PUSH;

  if (type != FT_TABLE_MASK)
      store_le_int16 (type, dest);
  else if (type == FT_TABLE_MASK)
    {
      store_le_int16 (type, dest);
      dest += sizeof (D_UINT16);

      assert  ((fieldsCount != 0) && (fields != NULL));

      store_le_int16 (fieldsCount, dest);
      dest += sizeof (D_UINT16);

      while (fieldsCount-- > 0)
        {
          const D_UINT16      fieldType = fields[fieldsCount].m_FieldType;
          const D_CHAR* const fieldName = fields[fieldsCount].m_FieldName;
          const D_UINT        nameLen   = strlen (fieldName) + 1;

          memcpy (dest, fieldName, nameLen);
          dest += nameLen;
          store_le_int16 (fieldType, dest);
          dest += sizeof (D_UINT16);
        }
    }
  else
    {
      assert (FALSE);
    }
}

D_UINT
PushStackValue (const CONNECTOR_HND                 hnd,
                const unsigned int                  type,
                const unsigned int                  fieldsCount,
                const struct FieldDescriptor* const fields)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT       cs       = CS_OK;
  D_UINT       dataSize = 0;
  const D_UINT spaceReq = compute_push_space_need (type, fieldsCount, fields);

  if (hnd_ == NULL)
    return CS_INVALID_ARGS;
  else if (spaceReq == 0)
    return CS_INVALID_ARGS;
  else if (spaceReq > max_data_size (hnd_))
    return CS_LARGE_ARGS;

  dataSize = data_size (hnd_);
  if (max_data_size (hnd_) - spaceReq < dataSize)
    {
      if ((cs == UpdateStackFlush (hnd)) != CS_OK)
        return cs;
    }

  assert (cs == CS_OK);

  write_push_cmd (type, fieldsCount, fields, data (hnd_) + dataSize);
  dataSize += spaceReq;
  set_data_size (hnd_, dataSize);

  hnd_->cmdInternal = 0;

  return cs;
}

D_UINT
PopStackValues (const CONNECTOR_HND hnd,
                unsigned int        count)
{
  static const D_UINT neededSize = sizeof (D_UINT32) + 1;

  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;
  D_UINT32 cs;

  if ((hnd_ == NULL) || (count == 0))
    return CS_INVALID_ARGS;
  else if (neededSize + data_size (hnd_) > max_data_size (hnd_))
    {
      if ((cs = UpdateStackFlush (hnd_)) != CS_OK)
        return cs;
    }

  {
    D_UINT8* const data_    = data (hnd_);
    const D_UINT   dataSize = data_size (hnd_);

    data_[0] = CMD_UPDATE_FUNC_POP;
    store_le_int32 (count, data_ + 1);

    set_data_size (hnd_, dataSize + neededSize);

    hnd_->cmdInternal = 0;
  }

  return CS_OK;
}

D_UINT
stack_top_basic_update (struct INTERNAL_HANDLER* hnd,
                        const D_UINT             type,
                        const char* const        value)
{
  D_UINT32       cs        = CS_OK;
  const D_UINT16 spaceNeed = sizeof (D_UINT8) +
                             sizeof (D_UINT16) +
                             strlen (value) + 1;

  assert (hnd != NULL);
  assert (strlen (value) > 0);
  assert ((FT_BOOL <= type) && (type < FT_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return CS_LARGE_ARGS;
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = UpdateStackFlush (hnd)) != CS_OK)
        return cs;
    }

  {
    D_UINT8*     data_    = data (hnd);
    const D_UINT currSize = data_size (hnd);

    *data_++ = CMD_UPDATE_FUNC_CHTOP;

    store_le_int16 (type, data_);
    data_ += sizeof (D_UINT16);

    strcpy ((D_CHAR*)data_, value);

    assert ((currSize + spaceNeed) <= max_data_size (hnd));
    set_data_size (hnd, currSize + spaceNeed);

    hnd->cmdInternal = currSize;
  }

  assert (cs == CS_OK);
  return cs;
}

D_UINT
stack_top_field_basic_update (struct INTERNAL_HANDLER* hnd,
                              const D_UINT             type,
                              const D_CHAR* const      fieldNane,
                              const TABLE_ROW_INDEX    row,
                              const D_CHAR* const      value)
{
  D_UINT32       cs        = CS_OK;
  const D_UINT16 spaceNeed = sizeof (D_UINT8) +
                             sizeof (D_UINT16) +
                             strlen (fieldNane) + 1 +
                             sizeof (D_UINT64) +
                             strlen (value) + 1;

  assert (hnd != NULL);
  assert (strlen (fieldNane) > 0);
  assert (strlen (value) > 0);
  assert ((FT_BOOL <= type) && (type < FT_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return CS_LARGE_ARGS;
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = UpdateStackFlush (hnd)) != CS_OK)
        return cs;
    }

  {
    D_UINT8*     data_    = data (hnd);
    const D_UINT currSize = data_size (hnd);

    *data_++ = CMD_UPDATE_FUNC_CHTOP;

    store_le_int16 (type | FT_FIELD_MASK, data_);
    data_ += sizeof (D_UINT16);

    strcpy ((D_CHAR*)data_, fieldNane);
    data_ += strlen (fieldNane) + 1;

    store_le_int64 (row, data_);
    data_ += sizeof (D_UINT64);

    strcpy ((D_CHAR*)data_, value);

    assert ((currSize + spaceNeed) <= max_data_size (hnd));
    set_data_size (hnd, currSize + spaceNeed);

    hnd->cmdInternal = currSize;
  }

  assert (cs == CS_OK);
  return cs;
}

static D_UINT
stack_top_text_update (struct INTERNAL_HANDLER* hnd,
                       const ELEMENT_INDEX      fromPos,
                       const D_CHAR* const      value)
{
  D_UINT32       cs         = CS_OK;
  const D_UINT16 fixedSize  =  sizeof (D_UINT8) +
                               sizeof (D_UINT16) +
                               sizeof (D_UINT64);
  const D_UINT16 newEntSize = fixedSize + strlen (value) + 1;

  D_UINT8* data_;
  D_UINT   currSize;

  assert (hnd != NULL);
  assert (strlen (value) > 0);

  if (newEntSize > max_data_size (hnd))
    return CS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = UpdateStackFlush (hnd)) != CS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal + fixedSize) < currSize)
      && (data_[hnd->cmdInternal] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if (prevType == FT_TEXT)
        {
          data_ += sizeof (D_UINT16);

          const D_UINT64 prevPos = from_le_int64 (data_);
          data_ += sizeof (D_UINT64);

          const D_UINT64 prevValueLen = strlen ((const D_CHAR*)data_);
          data_ += prevValueLen;

          if ((prevPos + prevValueLen) == fromPos)
            {
              const D_UINT newDataSize = hnd->cmdInternal +
                                         fixedSize +
                                         prevValueLen +
                                         strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return CS_OK;
            }
        }
  }

  currSize = data_size (hnd);
  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (FT_TEXT, data_);
  data_ += sizeof (D_UINT16);

  store_le_int64 (fromPos, data_);
  data_ += sizeof (D_UINT64);

  strcpy ((D_CHAR*)data_, value);

  assert ((currSize + newEntSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + newEntSize);

  hnd->cmdInternal = currSize;

  assert (cs == CS_OK);
  return cs;
}

static D_UINT
stack_top_field_text_update (struct INTERNAL_HANDLER* hnd,
                             const D_CHAR* const      fieldName,
                             const TABLE_ROW_INDEX    row,
                             const ELEMENT_INDEX      fromPos,
                             const D_CHAR* const      value)
{
  D_UINT32       cs         = CS_OK;
  const D_UINT16 fixedSize  = sizeof (D_UINT8) +
                              sizeof (D_UINT16) +
                              strlen (fieldName) + 1 +
                              sizeof (D_UINT64) +
                              sizeof (D_UINT64);
  const D_UINT16 newEntSize = fixedSize + strlen (value) + 1;

  D_UINT8* data_;
  D_UINT   currSize;

  assert (hnd != NULL);
  assert (strlen (value) > 0);
  assert (strlen (fieldName) > 0);

  if (newEntSize > max_data_size (hnd))
    return CS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = UpdateStackFlush (hnd)) != CS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal + fixedSize) < currSize)
      && (data_[hnd->cmdInternal] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if (prevType == (FT_TEXT | FT_FIELD_MASK))
        {
          data_ += sizeof (D_UINT16);

          const D_CHAR* const prevFielName = (const D_CHAR*)data_;
          data_ += strlen (prevFielName) + 1;

          const D_UINT64 prevRow = from_le_int64 (data_);
          data_ += sizeof (D_UINT64);

          const D_UINT64 prevPos = from_le_int64 (data_);
          data_ += sizeof (D_UINT64);

          const D_UINT64 prevValueLen = strlen ((const D_CHAR*)data_);
          data_ += prevValueLen;

          if ((strcmp (prevFielName, fieldName) == 0)
              && (prevRow == row)
              && ((prevPos + prevValueLen) == fromPos))
            {
              const D_UINT newDataSize = hnd->cmdInternal +
                                         fixedSize +
                                         prevValueLen +
                                         strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return CS_OK;
            }
        }
  }

  currSize = data_size (hnd);
  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (FT_TEXT | FT_FIELD_MASK, data_),
  data_ += sizeof (D_UINT16);

  strcpy ((D_CHAR*)data_, fieldName);
  data_ += strlen (fieldName) + 1;

  store_le_int64 (row, data_);
  data_ += sizeof (D_UINT64);

  store_le_int64 (fromPos, data_);
  data_ += sizeof (D_UINT64);

  strcpy ((D_CHAR*)data_, value);

  assert ((currSize + newEntSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + newEntSize);

  hnd->cmdInternal = currSize;

  assert (cs == CS_OK);
  return cs;
}

static D_UINT
stack_top_array_update (struct INTERNAL_HANDLER* hnd,
                        const D_UINT             type,
                        const ELEMENT_INDEX      fromPos,
                        const D_CHAR* const      value)
{
  D_UINT32       cs         = CS_OK;
  D_UINT32       countOff   = sizeof (D_UINT8) +
                              sizeof (D_UINT16);
  const D_UINT16 fixedSize  = countOff +
                              sizeof (D_UINT16) +
                              sizeof (D_UINT64);
  const D_UINT16 newEntSize = fixedSize + strlen (value) + 1;

  D_UINT8* data_;
  D_UINT   currSize;

  assert (hnd != NULL);
  assert ((FT_BOOL <= type) && (type < FT_TEXT));
  assert (strlen (value) > 0);

  if (newEntSize > max_data_size (hnd))
    return CS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = UpdateStackFlush (hnd)) != CS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal + fixedSize) < currSize)
      && (data_[hnd->cmdInternal] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if ((type | FT_ARRAY_MASK) == prevType)
        {
          data_ += sizeof (D_UINT16);

          const D_UINT16 prevCount = from_le_int16 (data_);
          assert (prevCount > 0);
          data_ += sizeof (D_UINT16);

          const D_UINT64 prevPos = from_le_int64 (data_);
          data_ += sizeof (D_UINT64);

          if (((prevPos + prevCount) == fromPos)
              && (prevCount < (D_UINT16)~0))
            {
              data_ = data (hnd) + hnd->cmdInternal + countOff;
              store_le_int16 (prevCount + 1, data_);

              const D_UINT newDataSize = data_size (hnd) + strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              data_ = data (hnd) + data_size (hnd);
              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return CS_OK;
            }
        }
  }

  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (type | FT_ARRAY_MASK, data_);
  data_ += sizeof (D_UINT16);

  store_le_int16 (1, data_);
  data_ += sizeof (D_UINT16);

  store_le_int64 (fromPos, data_);
  data_ += sizeof (D_UINT64);

  strcpy ((D_CHAR*)data_, value);

  assert ((currSize + newEntSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + newEntSize);

  hnd->cmdInternal = currSize;

  assert (cs == CS_OK);
  return cs;
}

static D_UINT
stack_top_field_array_update (struct INTERNAL_HANDLER* hnd,
                              const D_UINT             type,
                              const D_CHAR*            fieldName,
                              const TABLE_ROW_INDEX    row,
                              const ELEMENT_INDEX      fromPos,
                              const D_CHAR* const      value)
{
  D_UINT32       cs         = CS_OK;
  D_UINT32       countOff   = sizeof (D_UINT8) +
                              sizeof (D_UINT16) +
                              strlen (fieldName) + 1 +
                              sizeof (D_UINT64);
  const D_UINT16 fixedSize  = countOff +
                              sizeof (D_UINT16) +
                              sizeof (D_UINT64);
  const D_UINT16 newEntSize = fixedSize + strlen (value) + 1;

  D_UINT8* data_;
  D_UINT   currSize;

  assert (hnd != NULL);
  assert ((FT_BOOL <= type) && (type < FT_TEXT));
  assert (strlen (fieldName) > 0);
  assert (strlen (value) > 0);

  if (newEntSize > max_data_size (hnd))
    return CS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = UpdateStackFlush (hnd)) != CS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal + fixedSize) < currSize)
      && (data_[hnd->cmdInternal] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if ((type | FT_ARRAY_MASK | FT_FIELD_MASK) == prevType)
        {
          data_ += sizeof (D_UINT16);

          const D_CHAR* prevFielName = (const D_CHAR*)data_;
          data_ += strlen (prevFielName) + 1;

          const D_CHAR prevRow = from_le_int64 (data_);
          data_ += sizeof (D_UINT64);

          const D_UINT16 prevCount = from_le_int16 (data_);
          assert (prevCount > 0);
          data_ += sizeof (D_UINT16);

          const D_UINT64 prevPos = from_le_int64 (data_);
          data_ += sizeof (D_UINT64);

          if (((prevPos + prevCount) == fromPos)
              && (prevCount < (D_UINT16)~0)
              && (prevRow == row)
              && (strcmp (prevFielName, fieldName) == 0))
            {
              data_ = data (hnd) + hnd->cmdInternal + countOff;
              store_le_int16 (prevCount + 1, data_);

              const D_UINT newDataSize = data_size (hnd) + strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              data_ = data (hnd) + data_size (hnd);
              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return CS_OK;
            }
        }
  }

  data_ = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (type | FT_ARRAY_MASK | FT_FIELD_MASK, data_);
  data_ += sizeof (D_UINT16);

  strcpy ((D_CHAR*)data_, fieldName);
  data_ += strlen (fieldName) + 1;

  store_le_int64 (row, data_);
  data_ += sizeof (D_UINT64);

  store_le_int16 (1, data_);
  data_ += sizeof (D_UINT16);

  store_le_int64 (fromPos, data_);
  data_ += sizeof (D_UINT64);

  strcpy ((D_CHAR*)data_, value);

  assert ((currSize + newEntSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + newEntSize);

  hnd->cmdInternal = currSize;

  assert (cs == CS_OK);
  return cs;
}

D_UINT
UpdateStackValue (const CONNECTOR_HND           hnd,
                  const unsigned int            valueType,
                  const char* const             fieldName,
                  const TABLE_ROW_INDEX         row,
                  const ELEMENT_INDEX           position,
                  const char* const             value)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT32 cs = CS_OK;

  if ((hnd_ == NULL)
      || (value == NULL)
      || (value[0] == 0))
    {
      return CS_INVALID_ARGS;
    }

  if ((valueType & (FT_FIELD_MASK | FT_ARRAY_MASK)) == 0)
    {
      if ((fieldName != NULL) || (row != 0))
        return CS_INVALID_ARGS;
      else if ((valueType & 0xFF) != FT_TEXT)
        {
          if (position != 0)
            return CS_INVALID_ARGS;

          cs = stack_top_basic_update (hnd_, valueType, value);
        }
      else
        cs = stack_top_text_update (hnd_, position, value);
    }
  else if ((valueType & (FT_FIELD_MASK | FT_ARRAY_MASK)) == FT_ARRAY_MASK)
    {
      if ((fieldName != NULL) || (row != 0))
        return CS_INVALID_ARGS;
      else if ((valueType & 0xFF) == FT_TEXT)
        return CS_OP_NOTSUPP;

      cs = stack_top_array_update (hnd_, valueType & 0xFF, position, value);
    }
  else if ((valueType & FT_FIELD_MASK) != 0)
    {
      if ((fieldName == NULL) || (fieldName[0] == 0))
        return CS_INVALID_ARGS;

      if ((valueType & FT_ARRAY_MASK) == 0)
        {
          if ((valueType & 0xFF) == FT_TEXT)
            {
              cs = stack_top_field_text_update (hnd_,
                                                fieldName,
                                                row,
                                                position,
                                                value);
            }
          else
            {
              cs = stack_top_field_basic_update (hnd_,
                                                 valueType & 0xFF,
                                                 fieldName,
                                                 row,
                                                 value);
            }
        }
      else
        {
          if ((valueType & 0xFF) == FT_TEXT)
            return CS_OP_NOTSUPP;

          cs = stack_top_field_array_update (hnd_,
                                             valueType & 0xFF,
                                             fieldName,
                                             row,
                                             position,
                                             value);
        }
    }
  else
    {
      assert (FALSE);
      cs = CS_GENERAL_ERR;
    }

  return cs;
}

D_UINT
UpdateStackFlush (const CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT32  cs  = CS_OK;

  D_UINT16  type;
  D_BOOL    lastPart;

  if (hnd_ == NULL)
    return CS_INVALID_ARGS;
  else if (data_size (hnd_) == 0)
    return CS_OK;
  else if (hnd_->lastCmdRespFrameReceived == FALSE)
    {
      if ((cs = ack_answer_part (hnd_, FALSE)) != CS_OK)
        return cs;
    }

  if ((cs = send_command (hnd_, CMD_UPDATE_STACK, TRUE)) != CS_OK)
    return cs;

  if ((cs = recieve_answer (hnd_, &type, &lastPart)) != CS_OK)
    return cs;
  else if ((type != CMD_UPDATE_STACK_RSP) || (lastPart == FALSE))
    return CS_UNEXPECTED_FRAME;

  cs = from_le_int32 (data (hnd_));

  hnd_->cmdInternal = 0;
  set_data_size (hnd_, 0);

  return cs;
}
