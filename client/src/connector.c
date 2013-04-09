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
#include "utils/include/utf8.h"
#include "utils/include/enc_3k.h"

#include "connector.h"
#include "client_connection.h"

static const char WANONIM_FIELD[] = "";

static const uint_t INVALID_OFF    = ~0;

/* TODO: Handle the cases where the text entry is bigger than 0xFFFF */

static uint_t
max_data_size (const struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize;

  assert ((hnd->cipher == FRAME_ENCTYPE_PLAIN)
          || (hnd->cipher == FRAME_ENCTYPE_3K));

  assert (MIN_FRAME_SIZE <= hnd->dataSize);
  assert (hnd->dataSize <= MAX_FRAME_SIZE);

  switch (hnd->cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (FALSE);
  }

  return hnd->dataSize - metaDataSize;
}

static uint_t
data_size (const struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize;

  const uint16_t frameSize = from_le_int16 (&hnd->data[FRAME_SIZE_OFF]);

  switch (hnd->cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (FALSE);
  }

  assert ((frameSize >= metaDataSize) && (frameSize <= hnd->dataSize));

  return frameSize - metaDataSize;
}

static void
set_data_size (struct INTERNAL_HANDLER* const hnd, const uint_t size)
{
  uint_t metaDataSize;

  switch (hnd->cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (FALSE);
  }

  assert (size <= max_data_size (hnd));

  store_le_int16 (size + metaDataSize, &hnd->data[FRAME_SIZE_OFF]);
}

static uint8_t*
raw_data (struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize;

  switch (hnd->cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE;
    break;

  default:
    assert (FALSE);
  }

  return hnd->data + metaDataSize;
}

static uint8_t*
data (struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize;

  switch (hnd->cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (FALSE);
  }

  return hnd->data + metaDataSize;
}

static uint_t
send_raw_frame (struct INTERNAL_HANDLER* const hnd,
                const uint8_t                  type)
{
  uint32_t status = 0;

  uint_t frameSize = from_le_int16 (hnd->data + FRAME_SIZE_OFF);

  if (hnd->cipher == FRAME_ENCTYPE_3K)
    {
      uint32_t firstKing, secondKing;
      uint8_t  prev, i;

      const uint16_t plainSize = frameSize;

      while (frameSize % sizeof (uint32_t) != 0)
        hnd->data[frameSize++] = w_rnd () & 0xFF;

      firstKing  = w_rnd () & 0xFFFFFFFF;
      store_le_int32 (firstKing,
                      hnd->data + FRAME_HDR_SIZE + ENC_3K_FIRST_KING_OFF);

      secondKing = w_rnd () & 0xFFFFFFFF;
      store_le_int32 (secondKing,
                      hnd->data + FRAME_HDR_SIZE + ENC_3K_SECOND_KING_OFF);

      for (i = 0, prev = 0; i < ENC_3K_PLAIN_SIZE_OFF; ++i)
        {
          const uint8_t temp = hnd->data[FRAME_HDR_SIZE + i];
          hnd->data[FRAME_HDR_SIZE + i] ^=
                              hnd->encriptionKey[prev % hnd->encKeySize];
          prev = temp;
        }

      store_le_int16 (plainSize,
                      hnd->data + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF);

      store_le_int16 (w_rnd () & 0xFFFF,
                      hnd->data + FRAME_HDR_SIZE + ENC_3K_SPARE_OFF);

      encrypt_3k_buffer (firstKing,
                         secondKing,
                         hnd->encriptionKey,
                         hnd->encKeySize,
                         hnd->data + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF,
                         frameSize - (FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF));

      store_le_int16 (frameSize, hnd->data + FRAME_SIZE_OFF);
    }

  assert ((frameSize > 0) && (frameSize <= hnd->dataSize));

  hnd->data[FRAME_TYPE_OFF]    = type;
  hnd->data[FRAME_ENCTYPE_OFF] = hnd->cipher;
  store_le_int32 (hnd->expectedFrameId++, &hnd->data[FRAME_ID_OFF]);

  status = wh_socket_write (hnd->socket, hnd->data, frameSize);
  if (status != WOP_OK)
    return WENC_OS_ERROR (status);

  return WCS_OK;
}

static uint_t
receive_raw_frame (struct INTERNAL_HANDLER* const hnd)
{
  uint_t frameSize;
  uint_t frameRead = 0;

  while (frameRead < FRAME_HDR_SIZE)
    {
      uint_t chunkSize = FRAME_HDR_SIZE - frameRead;

      const uint32_t status  = wh_socket_read (hnd->socket,
                                               &hnd->data [frameRead],
                                               &chunkSize);
      if (status != WOP_OK)
        return WENC_OS_ERROR (status);

      else if (chunkSize == 0)
        return WCS_DROPPED;

      frameRead += chunkSize;
    }

  if (hnd->data[FRAME_TYPE_OFF] == FRAME_TYPE_TIMEOUT)
    return WCS_CONNECTION_TIMEOUT;

  else if (hnd->data[FRAME_TYPE_OFF] != FRAME_TYPE_NORMAL)
    return WCS_UNEXPECTED_FRAME;

  frameSize = from_le_int16 (&hnd->data[FRAME_SIZE_OFF]);
  if ((frameSize < FRAME_HDR_SIZE)
      || (frameSize > hnd->dataSize)
      || (hnd->data[FRAME_ENCTYPE_OFF] != hnd->cipher))
    {
      return WCS_UNEXPECTED_FRAME;
    }

  while (frameRead < frameSize)
    {
      uint_t chunkSize = frameSize - frameRead;
      const uint32_t status = wh_socket_read (hnd->socket,
                                              &hnd->data [frameRead],
                                              &chunkSize);
      if (status != WOP_OK)
        return WENC_OS_ERROR (status);

      else if (chunkSize == 0)
        return WCS_DROPPED;

      frameRead += chunkSize;
    }

  if (from_le_int32 (hnd->data + FRAME_ID_OFF) != hnd->expectedFrameId)
    return WCS_UNEXPECTED_FRAME;

  if (hnd->cipher == FRAME_ENCTYPE_3K)
    {
      uint_t    i, prev;
      uint32_t  firstKing, secondKing;
      uint16_t  plainSize;

      for (i = 0, prev = 0; i < ENC_3K_PLAIN_SIZE_OFF; ++i)
        {
          hnd->data[FRAME_HDR_SIZE + i] ^=
                              hnd->encriptionKey[prev % hnd->encKeySize];
          prev = hnd->data[FRAME_HDR_SIZE + i];
        }

      firstKing = from_le_int32 (hnd->data +
                                FRAME_HDR_SIZE +
                                ENC_3K_FIRST_KING_OFF);
      secondKing = from_le_int32 (hnd->data +
                                  FRAME_HDR_SIZE +
                                  ENC_3K_SECOND_KING_OFF);
      decrypt_3k_buffer (firstKing,
                         secondKing,
                         hnd->encriptionKey,
                         hnd->encKeySize,
                         hnd->data + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF,
                         frameSize - (FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF));

      plainSize = from_le_int16 (hnd->data+
                                 FRAME_HDR_SIZE +
                                 ENC_3K_PLAIN_SIZE_OFF);
      assert (plainSize <= frameSize);
      frameSize = plainSize;
      store_le_int16 (plainSize, hnd->data + FRAME_SIZE_OFF);
    }

  return WCS_OK;
}

static uint_t
send_command (struct INTERNAL_HANDLER* const hnd,
              const uint16_t                 commandId)
{
  uint8_t*     pData    = raw_data (hnd);
  uint_t       index    = 0;
  const uint_t dataSize = data_size (hnd);
  uint_t       cs       = WCS_OK;
  uint16_t     chkSum   = 0;

  hnd->clientCookie = w_rnd ();

  for (index = 0; index < dataSize; index++)
    chkSum += data (hnd)[index];

  store_le_int32 (hnd->clientCookie, &pData[PLAIN_CLNT_COOKIE_OFF]);
  store_le_int32 (hnd->serverCookie, &pData[PLAIN_SERV_COOKIE_OFF]);
  store_le_int16 (commandId, &pData[PLAIN_TYPE_OFF]);
  store_le_int16 (chkSum, &pData[PLAIN_CRC_OFF]);

  cs = send_raw_frame (hnd, FRAME_TYPE_NORMAL);
  return cs;
}

static uint_t
recieve_answer (struct INTERNAL_HANDLER* const hnd,
                uint16_t* const                pRespType)
{
  uint_t        cs       = receive_raw_frame (hnd);
  uint_t        index    = 0;
  const uint_t  dataSize = data_size (hnd);
  uint16_t      chkSum   = 0;

  if (cs != WCS_OK)
    return cs;

  for (index = 0; index < dataSize; index++)
    chkSum += data (hnd)[index];

  if (chkSum != from_le_int16 (&raw_data (hnd)[PLAIN_CRC_OFF]))
    {
      cs = WCS_INVALID_FRAME;
      goto recieve_failure;
    }
  else if (from_le_int32 (&raw_data (hnd)[PLAIN_CLNT_COOKIE_OFF]) !=
           hnd->clientCookie)
    {
      cs = WCS_UNEXPECTED_FRAME;
      goto recieve_failure;
    }

  hnd->serverCookie = from_le_int32 (&raw_data (hnd)[PLAIN_SERV_COOKIE_OFF]);
  *pRespType = from_le_int16 (&raw_data (hnd)[PLAIN_TYPE_OFF]);
  if ((*pRespType == CMD_INVALID_RSP) || ((*pRespType & 1) == 0))
    {
      cs = WCS_INVALID_FRAME;
      goto recieve_failure;
    }

  hnd->lastCmdRespReceived        = *pRespType;

  return WCS_OK;

recieve_failure:

  hnd->lastCmdRespReceived      = CMD_INVALID_RSP;

  wh_socket_close (hnd->socket);
  hnd->socket = INVALID_SOCKET;

  return cs;
}

uint_t
WConnect (const char* const    host,
          const char* const    port,
          const char* const    databaseName,
          const char* const    password,
          const unsigned int   userId,
          W_CONNECTOR_HND* const pHnd)
{
  struct INTERNAL_HANDLER* result      = NULL;
  const uint_t             passwordLen = strlen (password);
  uint_t                   frameSize   = 0;
  uint32_t                 status      = WCS_OK;

  uint8_t                  tempBuffer[MIN_FRAME_SIZE];

  if ((host == NULL)
      || (port == NULL)
      || (databaseName == NULL)
      || (password == NULL)
      || (pHnd == NULL)
      || (strlen (host) == 0)
      || (strlen (port) == 0)
      || (strlen (databaseName) == 0))
    {
      status = WCS_INVALID_ARGS;
      goto fail_ret;
    }

  *pHnd = NULL;
  result = mem_alloc (sizeof (*result) + passwordLen);
  memset (result, 0, sizeof (*result));

  result->userId     = userId;
  result->encKeySize = passwordLen;
  result->socket     = INVALID_SOCKET;
  result->cipher     = FRAME_ENCTYPE_PLAIN;
  memcpy (result->encriptionKey, password, passwordLen);
  result->data       = tempBuffer;
  result->dataSize   = sizeof (tempBuffer);

  if ((status = wh_socket_client (host, port, &result->socket)) != WCS_OK)
    {
      status = WENC_OS_ERROR (status);
      goto fail_ret;
    }

  if ((status = read_raw_frame (result, &frameSize)) != WCS_OK)
    goto fail_ret;

  assert (frameSize <= result->dataSize);
  {
      uint_t serverFrameSize = 0;
      const uint32_t frameId = from_le_int32 (&result->data[FRAME_ID_OFF]);

      if ((frameId != 0)
          || (result->data[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT))
        {
          status = WCS_UNEXPECTED_FRAME;
          goto fail_ret;
        }

      assert (frameSize == FRAME_HDR_SIZE + FRAME_AUTH_SIZE);
      assert (result->data[FRAME_ENCTYPE_OFF] == FRAME_ENCTYPE_PLAIN);


      result->cipher = result->data[FRAME_HDR_SIZE + FRAME_AUTH_ENC_OFF];
      if ((result->cipher != FRAME_ENCTYPE_PLAIN)
          && (result->cipher != FRAME_ENCTYPE_3K))
        {
          status = WCS_ENCTYPE_NOTSUPP;
          goto fail_ret;
        }
      result->version = from_le_int32 (result->data +
                                      FRAME_HDR_SIZE +
                                      FRAME_AUTH_VER_OFF);
      if ((result->version & CLIENT_VERSION) == 0)
        {
          status = WCS_PROTOCOL_NOTSUPP;
          goto fail_ret;
        }
      else
        result->version = CLIENT_VERSION;

      /* Make sure we are able to handle server's published max frames size */
      serverFrameSize = from_le_int16 (result->data +
                                       FRAME_HDR_SIZE +
                                       FRAME_AUTH_SIZE_OFF);
      assert (MIN_FRAME_SIZE <= serverFrameSize);
      assert (serverFrameSize <= MAX_FRAME_SIZE);

      result->data     = mem_alloc (serverFrameSize);
      result->dataSize = serverFrameSize;
  }

  {
    const uint_t frameSize = FRAME_HDR_SIZE +
                              FRAME_AUTH_RSP_FIXED_SIZE +
                              strlen (databaseName) + 1 +
                              passwordLen + 1;
    char* const pAuthData =
        (char*)&result->data[FRAME_HDR_SIZE + FRAME_AUTH_RSP_FIXED_SIZE];

    if (frameSize > result->dataSize)
      {
        status = WCS_LARGE_ARGS;
        goto fail_ret;
      }

    store_le_int16 (frameSize, &result->data[FRAME_SIZE_OFF]);
    result->data[FRAME_ENCTYPE_OFF] = FRAME_ENCTYPE_PLAIN;
    result->data[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT_RSP;
    store_le_int32 (0, &result->data[FRAME_ID_OFF]);

    store_le_int32 (result->version,
                    &result->data[FRAME_HDR_SIZE + FRAME_AUTH_RSP_VER_OFF]);
    result->data[FRAME_HDR_SIZE + FRAME_AUTH_RSP_USR_OFF] = userId;
    result->data[FRAME_HDR_SIZE + FRAME_AUTH_RSP_ENC_OFF] = result->cipher;

    strcpy (pAuthData, databaseName);

    if (result->cipher == FRAME_ENCTYPE_PLAIN)
      strcpy (pAuthData + strlen (databaseName) + 1, password);

    if ((status = write_raw_frame (result, frameSize)) != WCS_OK)
      goto fail_ret;
  }

  assert (status == WCS_OK);

  *pHnd = result;
  return WCS_OK;

fail_ret:
  assert (status != WCS_OK);

  if (result != NULL)
    {
      if ((result->data != NULL) && (result->data != tempBuffer))
        mem_free (result->data);

      if (result->socket != INVALID_SOCKET)
        wh_socket_close (result->socket);

      mem_free (result);
    }

  return status;
}

void
WClose (W_CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if (hnd_ == NULL)
    return ;

  if (hnd_->socket != INVALID_SOCKET)
    {
      set_data_size (hnd_, 0);
      send_command (hnd_, CMD_CLOSE_CONN);
      wh_socket_close (hnd_->socket);
    }

  if (hnd_->data != NULL)
    mem_free (hnd_->data);

  mem_free (hnd_);
}

uint_t
WPingServer (const W_CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint16_t type;

  uint_t cs = WCS_OK;

  if (hnd == NULL)
    return WCS_INVALID_ARGS;
  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  set_data_size (hnd_, 0);
  if ((cs = send_command (hnd_, CMD_PING_SERVER)) != WCS_OK)
    goto exit_ping_server;

  if ((cs = recieve_answer (hnd_, &type)) != WCS_OK)
    goto exit_ping_server;

  if ((type != CMD_PING_SERVER_RSP)
      || (data_size (hnd_) != sizeof (uint32_t))
      || (from_le_int32 (data (hnd_)) != WCS_OK))

    {
      cs = WCS_INVALID_FRAME;
    }

exit_ping_server:
  return cs;
};

static uint_t
list_globals (struct INTERNAL_HANDLER* hnd,
              const unsigned int       hint,
              unsigned int* const      pGlbsCount)
{
  uint_t cs = WCS_OK;

  uint8_t* data_;
  uint_t   dataOffset;
  uint16_t type;

  set_data_size (hnd, sizeof (uint32_t));
  store_le_int32 (hint, data (hnd));

  if ((cs = send_command (hnd, CMD_LIST_GLOBALS)) != WCS_OK)
    goto list_globals_err;

  if ((cs = recieve_answer (hnd, &type)) != WCS_OK)
    goto list_globals_err;
  else if (type != CMD_LIST_GLOBALS_RSP)
    {
      cs = WCS_INVALID_FRAME;
      goto list_globals_err;
    }

  data_ = data (hnd);
  if ((cs = from_le_int32 (data_)) != WCS_OK)
    goto list_globals_err;
  else if (data_size (hnd) < 3 * sizeof (uint32_t))
    {
      cs = WCS_INVALID_FRAME;
      goto list_globals_err;
    }

  dataOffset  =  sizeof (uint32_t);
  *pGlbsCount =  from_le_int32 (data_ + dataOffset);
  dataOffset  += sizeof (uint32_t);

  hnd->cmdInternal[LIST_GLBINDEX] = from_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (uint32_t);

  hnd->cmdInternal[LIST_GLBOFF]    = dataOffset;
  hnd->cmdInternal[LIST_GLBSCOUNT] = *pGlbsCount;

  assert (hnd->lastCmdRespReceived == type);

  return WCS_OK;

list_globals_err:

  assert (cs != WCS_OK);

  hnd->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}

uint_t
WListGlobals (const W_CONNECTOR_HND hnd, unsigned int* pGlbsCount)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if ((hnd == NULL) || (pGlbsCount == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd_->userId != 0)
    return WCS_OP_NOTPERMITED;
  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  return list_globals (hnd_, 0, pGlbsCount);
}

uint_t
WListGlobalsFetch (const W_CONNECTOR_HND hnd, const char** ppGlbName)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t  cs = WCS_OK;
  uint_t  dataOffset;
  uint_t  glbIndex;
  uint_t  glbsCount;

  if ((hnd_ == NULL)
      || (ppGlbName == NULL)
      || (hnd_->lastCmdRespReceived != CMD_LIST_GLOBALS_RSP))
    {
      return WCS_INVALID_ARGS;
    }
  else if ((cs = from_le_int32 (data (hnd_))) != WCS_OK)
    goto list_global_fetch_err;

  glbsCount  = hnd_->cmdInternal[LIST_GLBSCOUNT];
  glbIndex   = hnd_->cmdInternal[LIST_GLBINDEX];
  dataOffset = hnd_->cmdInternal[LIST_GLBOFF];

  if (glbIndex >= glbsCount)
    {
      *ppGlbName = NULL;
      return WCS_OK;
    }

  if (dataOffset >= data_size (hnd_))
    {
      uint_t glbCount;

      if ((cs = list_globals (hnd_, glbIndex, &glbCount)) != WCS_OK)
        goto list_global_fetch_err;

      assert (glbIndex < glbCount);

      dataOffset = 3 * sizeof (uint32_t);

      assert (glbsCount  == hnd_->cmdInternal[LIST_GLBSCOUNT]);
      assert (glbIndex   == hnd_->cmdInternal[LIST_GLBINDEX]);
      assert (dataOffset == hnd_->cmdInternal[LIST_GLBOFF]);
    }


  *ppGlbName = (char*)data (hnd_) + dataOffset;
  dataOffset += strlen (*ppGlbName) + 1;

  assert (dataOffset <= data_size (hnd_));

  hnd_->cmdInternal[LIST_GLBSCOUNT] = glbsCount;
  hnd_->cmdInternal[LIST_GLBINDEX]  = glbIndex + 1;
  hnd_->cmdInternal[LIST_GLBOFF]    = dataOffset;

  assert (cs == WCS_OK);

  return WCS_OK;

list_global_fetch_err:

  assert (cs != WCS_OK);

  hnd_->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}

static uint_t
list_procedures (struct INTERNAL_HANDLER* hnd,
                 const unsigned int       hint,
                 unsigned int* const      pProcsCount)
{
  uint_t cs = WCS_OK;

  uint8_t* data_;
  uint_t   dataOffset;
  uint16_t type;

  set_data_size (hnd, sizeof (uint32_t));
  store_le_int32 (hint, data (hnd));

  if ((cs = send_command (hnd, CMD_LIST_PROCEDURE)) != WCS_OK)
    goto list_procedures_err;

  if ((cs = recieve_answer (hnd, &type)) != WCS_OK)
    goto list_procedures_err;
  else if (type != CMD_LIST_PROCEDURE_RSP)
    {
      cs = WCS_INVALID_FRAME;
      goto list_procedures_err;
    }

  data_ = data (hnd);
  if ((cs = from_le_int32 (data_)) != WCS_OK)
    goto list_procedures_err;
  else if (data_size (hnd) < 3 * sizeof (uint32_t))
    {
      cs = WCS_INVALID_FRAME;
      goto list_procedures_err;
    }

  dataOffset   =  sizeof (uint32_t);
  *pProcsCount =  from_le_int32 (data_ + dataOffset);
  dataOffset   += sizeof (uint32_t);

  hnd->cmdInternal[LIST_PROCSINDEX] = from_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (uint32_t);

  hnd->cmdInternal[LIST_PROCOFF]    = dataOffset;
  hnd->cmdInternal[LIST_PROCSCOUNT] = *pProcsCount;

  assert (hnd->lastCmdRespReceived == type);

  return WCS_OK;

list_procedures_err:

  assert (cs != WCS_OK);

  hnd->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}

uint_t
WListProcedures (const W_CONNECTOR_HND hnd, unsigned int* pProcsCount)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if ((hnd == NULL) || (pProcsCount == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd_->userId != 0)
    return WCS_OP_NOTPERMITED;
  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  return list_procedures (hnd_, 0, pProcsCount);
}

uint_t
WListProceduresFetch (const W_CONNECTOR_HND hnd,
                      const char**          ppProcName)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t  cs = WCS_OK;
  uint_t  procIndex;
  uint_t  dataOffset;
  uint_t  procsCount;

  if ((hnd_ == NULL)
      || (ppProcName == NULL)
      || (hnd_->lastCmdRespReceived != CMD_LIST_PROCEDURE_RSP))
    {
      return WCS_INVALID_ARGS;
    }
  else if ((cs = from_le_int32 (data (hnd_))) != WCS_OK)
    goto list_procedure_fetch_err;


  procsCount = hnd_->cmdInternal[LIST_PROCSCOUNT];
  dataOffset = hnd_->cmdInternal[LIST_PROCOFF];
  procIndex  = hnd_->cmdInternal[LIST_PROCSINDEX];

  if (procIndex >= procsCount)
    {
      *ppProcName = NULL;
      return WCS_OK;
    }


  if (dataOffset >= data_size (hnd_))
    {
      uint_t procsCount;

      if ((cs = list_procedures (hnd_, procIndex, &procsCount)) != WCS_OK)
        goto list_procedure_fetch_err;

      assert (procIndex < procsCount);

      dataOffset = 3 * sizeof (uint32_t);
      assert (procsCount == hnd_->cmdInternal[LIST_PROCSCOUNT]);
      assert (dataOffset == hnd_->cmdInternal[LIST_PROCOFF]);
      assert (procIndex  == hnd_->cmdInternal[LIST_PROCSINDEX]);
    }

  *ppProcName =  (char*)data (hnd_) + dataOffset;
  dataOffset  += strlen (*ppProcName) + 1;

  assert (dataOffset <= data_size (hnd_));

  hnd_->cmdInternal[LIST_PROCSCOUNT] = procsCount;
  hnd_->cmdInternal[LIST_PROCOFF]    = dataOffset;
  hnd_->cmdInternal[LIST_PROCSINDEX] = procIndex + 1;

  assert (cs == WCS_OK);

  return WCS_OK;

list_procedure_fetch_err:

  assert (cs != WCS_OK);

  hnd_->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}


static unsigned int
describe_value (struct INTERNAL_HANDLER* const  hnd,
                const char* const               globalName,
                const unsigned int              fieldHint,
                unsigned int* const             pRawType)
{
  uint_t    cs     = WCS_OK;
  uint_t    offset = 0;
  uint16_t  type;
  uint8_t*  data_;

  if (hnd->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;
  else if (fieldHint > 0xFFFF)
    return WCS_OP_NOTSUPP;

  set_data_size (hnd, max_data_size (hnd));
  data_ = data (hnd);

  store_le_int16 (fieldHint, data_);
  offset += sizeof (uint16_t);

  /* These 16 bits are reserved */
  store_le_int16 (0, data_);
  offset += sizeof (uint16_t);

  if (globalName == NULL)
    {
      strcpy ((char*)data_ + offset, "");
      offset += sizeof (uint8_t);
    }
  else
    {
      strcpy ((char*)data_ + offset, globalName);
      offset += strlen (globalName) + 1;
    }
  set_data_size (hnd, offset);

  if ((cs = send_command (hnd, CMD_GLOBAL_DESC)) != WCS_OK)
    goto describe_value_err;

  if ((cs = recieve_answer (hnd, &type)) != WCS_OK)
    goto describe_value_err;
  else if (type != CMD_GLOBAL_DESC_RSP)
    {
      cs = WCS_INVALID_FRAME;
      goto describe_value_err;
    }

  data_  = data (hnd);

  if ((cs = from_le_int32 (data_)) != WCS_OK)
    goto describe_value_err;

  assert (offset == strlen ((char*)data_ + sizeof (uint32_t)) + 1 +
                    sizeof (uint32_t));

  *pRawType = from_le_int16 (data_ + offset);
  hnd->cmdInternal[DESC_RAWTYPE] = *pRawType;
  offset += sizeof (uint16_t);

  hnd->cmdInternal[DESC_FIELD_COUNT] = from_le_int16 (data_ + offset);
  offset += sizeof (uint16_t);

  hnd->cmdInternal[DESC_FIELD_HINT] = from_le_int16 (data_ + offset);
  offset += sizeof (uint16_t);

  hnd->cmdInternal[DESC_FIELD_OFFSET] = offset;

  hnd->lastCmdRespReceived = CMD_GLOBAL_DESC_RSP;

  assert (cs == WCS_OK);
  return WCS_OK;

describe_value_err:
  assert (cs != WCS_OK);
  hnd->lastCmdRespReceived = CMD_INVALID_RSP;

  return cs;
}

unsigned int
WDescribeValue (const W_CONNECTOR_HND    hnd,
                const char* const        globalName,
                unsigned int* const      pRawType)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;


  if ((hnd == NULL)
      || (globalName == NULL)
      || (pRawType == NULL)
      || (strlen (globalName) == 0))
    {
      return WCS_INVALID_ARGS;
    }
  else if (hnd_->userId != 0)
    return WCS_OP_NOTPERMITED;

  return describe_value (hnd_, globalName, 0, pRawType);
}

unsigned int
WDescribeValueGetFieldsCount (const W_CONNECTOR_HND  hnd,
                              unsigned int* const    pFieldCount)
{
  struct INTERNAL_HANDLER* hnd_   = (struct INTERNAL_HANDLER*)hnd;
  uint_t                   cs     = WCS_OK;
  uint8_t*                 data_  = NULL;
  uint16_t                 type;

  if ((hnd_ == NULL) || (pFieldCount == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd_->lastCmdRespReceived != CMD_GLOBAL_DESC_RSP)
    return WCS_INCOMPLETE_CMD;

  data_ = data (hnd);
  if ((cs = from_le_int32 (data_)) != WCS_OK)
    goto describe_value_field_cnt_err;

  type = hnd_->cmdInternal[DESC_RAWTYPE];
  if ((type & WFT_TABLE_MASK) == 0)
    {
      *pFieldCount = 0;
      return WCS_OK;
    }

  *pFieldCount = hnd_->cmdInternal[DESC_FIELD_COUNT];

  assert (cs == WCS_OK);

  return WCS_OK;

describe_value_field_cnt_err:
  assert (cs != WCS_OK);
  hnd_->lastCmdRespReceived = CMD_INVALID_RSP;

  return cs;
}

unsigned int
WDescribeValueFetchField (const W_CONNECTOR_HND  hnd,
                          const char**           ppFieldName,
                          unsigned int* const    pFieldType)
{
  struct INTERNAL_HANDLER* hnd_   = (struct INTERNAL_HANDLER*)hnd;
  uint_t                   cs     = WCS_OK;
  uint8_t*                 data_  = NULL;
  uint16_t                 type;
  uint_t                   fieldHint;
  uint_t                   fieldCount;
  uint_t                   offset;

  if ((hnd_ == NULL) || (ppFieldName == NULL) || (pFieldType == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd_->lastCmdRespReceived != CMD_GLOBAL_DESC_RSP)
    return WCS_INCOMPLETE_CMD;

  data_ = data (hnd_);
  if ((cs = from_le_int32 (data_)) != WCS_OK)
    goto describe_value_fetch_field_err;

describe_value_fetch_field_again:

  type = hnd_->cmdInternal[DESC_RAWTYPE];
  if ((type & WFT_TABLE_MASK) == 0)
    return WCS_INVALID_ARGS;

  fieldCount = hnd_->cmdInternal[DESC_FIELD_COUNT];
  fieldHint  = hnd_->cmdInternal[DESC_FIELD_HINT];
  offset     = hnd_->cmdInternal[DESC_FIELD_OFFSET];

  assert (fieldCount <= 0xFFFF);
  assert (offset > 0);

  if (fieldHint > fieldCount)
    return WCS_INVALID_ARGS;
  else if (fieldHint == fieldCount)
    {
      *pFieldType  = WFT_NOTSET;
      *ppFieldName = NULL;

      hnd_->cmdInternal[DESC_FIELD_HINT] = ++fieldHint;

      return WCS_OK;
    }
  else if (offset < data_size (hnd_))
    {
      *ppFieldName =  (char*)data (hnd_) + offset;
      offset       += strlen (*ppFieldName) + 1;

      *pFieldType =  from_le_int16 (data(hnd_) + offset);
      offset      += sizeof (uint16_t);

      ++fieldHint;
      assert (offset <= 0xFFFF);
      assert (fieldHint <= 0xFFFF);

      hnd_->cmdInternal[DESC_FIELD_COUNT]  = fieldCount;
      hnd_->cmdInternal[DESC_FIELD_HINT]   = fieldHint;
      hnd_->cmdInternal[DESC_FIELD_OFFSET] = offset;
    }
  else
    {
      uint_t dummyType;
      cs = describe_value (hnd_,
                           (char*)data (hnd_) + sizeof (uint32_t),
                           fieldHint,
                           &dummyType);
      if (cs != WCS_OK)
        goto describe_value_fetch_field_err;

      assert (type == dummyType);
      goto describe_value_fetch_field_again;
    }

  assert (cs == WCS_OK);
  return cs;

describe_value_fetch_field_err:
  assert (cs != WCS_OK);
  hnd_->lastCmdRespReceived = CMD_INVALID_RSP;

  return cs;
}


unsigned int
WDescribeStackTop (const W_CONNECTOR_HND hnd,
                   unsigned int* const   pRawType)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;


  if ((hnd == NULL) || (pRawType == NULL))
    return WCS_INVALID_ARGS;

  return describe_value (hnd_, NULL, 0, pRawType);
}

static uint_t
compute_push_space_need (const uint_t                          type,
                         unsigned int                          fieldsCount,
                         const struct W_FieldDescriptor* const fields)
{
  if (type != WFT_TABLE_MASK)
    {
      if (type & WFT_FIELD_MASK)
        return 0;

      const uint_t baseType = type & ~(WFT_ARRAY_MASK);
      if ((baseType < WFT_BOOL) || (baseType > WFT_TEXT)
          || (fieldsCount > 0))
        {
          return 0;
        }

      return sizeof (uint16_t) + 1;
    }

  if ((fieldsCount == 0) || (fields == NULL))
    return 0;

  uint_t result = 2 * sizeof (uint16_t) + 1;
  while (fieldsCount-- > 0)
    {
      const char* const fieldName = fields[fieldsCount].m_FieldName;
      const uint_t        nameLen   = strlen (fieldName) + 1;

      uint16_t fieldType = fields[fieldsCount].m_FieldType;
      fieldType &= ~WFT_ARRAY_MASK;

      if ((fieldName == NULL)
          || (nameLen <= 1)
          || ((fieldType < WFT_BOOL) || (fieldType > WFT_TEXT)))
        {
          return 0;
        }
      result += nameLen;
      result += sizeof (uint16_t);
    }

  return result;
}

static void
write_push_cmd (const uint_t                          type,
                unsigned int                          fieldsCount,
                const struct W_FieldDescriptor* const fields,
                uint8_t*                              dest)
{
  *dest++ = CMD_UPDATE_FUNC_PUSH;

  store_le_int16 (type, dest);

  if (type == WFT_TABLE_MASK)
    {
      dest += sizeof (uint16_t);

      assert  ((fieldsCount != 0) && (fields != NULL));

      store_le_int16 (fieldsCount, dest);
      dest += sizeof (uint16_t);

      while (fieldsCount-- > 0)
        {
          const uint16_t      fieldType = fields[fieldsCount].m_FieldType;
          const char* const fieldName = fields[fieldsCount].m_FieldName;
          const uint_t        nameLen   = strlen (fieldName) + 1;

          memcpy (dest, fieldName, nameLen);
          dest += nameLen;
          store_le_int16 (fieldType, dest);
          dest += sizeof (uint16_t);
        }
    }
}

uint_t
WPushStackValue (const W_CONNECTOR_HND                 hnd,
                 const unsigned int                    type,
                 const unsigned int                    fieldsCount,
                 const struct W_FieldDescriptor* const fields)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t       cs       = WCS_OK;
  const uint_t spaceReq = compute_push_space_need (type, fieldsCount, fields);

  if (hnd_ == NULL)
    return WCS_INVALID_ARGS;
  else if (spaceReq == 0)
    return WCS_INVALID_ARGS;
  else if ((hnd_->buildingCmd != CMD_UPDATE_STACK)
           && (hnd_->buildingCmd != CMD_INVALID))
    {
      return WCS_INCOMPLETE_CMD;
    }
  else if (spaceReq > max_data_size (hnd_))
    return WCS_LARGE_ARGS;

  if (hnd_->buildingCmd == CMD_INVALID)
    set_data_size (hnd_, 0);

  if (max_data_size (hnd_) - spaceReq < data_size (hnd_))
    {
      assert (hnd_->buildingCmd == CMD_UPDATE_STACK);
      assert (data_size (hnd_) > 0);

      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;

      assert (hnd_->buildingCmd == CMD_INVALID);
    }

  {
    uint8_t* const data_    = data (hnd_);
    const uint_t   dataSize = data_size (hnd_);

    assert ((dataSize == 0) || (hnd_->buildingCmd == CMD_UPDATE_STACK));
    assert ((dataSize > 0) || (hnd_->buildingCmd == CMD_INVALID));

    write_push_cmd (type, fieldsCount, fields, data_ + dataSize);
    set_data_size (hnd_, dataSize + spaceReq);
  }

  assert (cs == WCS_OK);

  memset (hnd_->cmdInternal, 0, sizeof (hnd_->cmdInternal));
  hnd_->buildingCmd = CMD_UPDATE_STACK;

  return cs;
}

uint_t
WPopStackValues (const W_CONNECTOR_HND hnd,
                 unsigned int          count)
{
  struct INTERNAL_HANDLER* hnd_     = (struct INTERNAL_HANDLER*)hnd;
  static const uint_t      spaceReq = sizeof (uint32_t) + 1;

  if ((hnd_ == NULL) || (count == 0))
    return WCS_INVALID_ARGS;
  else if ((hnd_->buildingCmd != CMD_UPDATE_STACK)
           && (hnd_->buildingCmd != CMD_INVALID))
    {
      return WCS_INCOMPLETE_CMD;
    }

  if (hnd_->buildingCmd == CMD_INVALID)
    set_data_size (hnd_, 0);

  if (max_data_size(hnd_) - spaceReq  <  data_size (hnd_))
    {
      uint_t cs;

      assert (hnd_->buildingCmd == CMD_UPDATE_STACK);
      assert (data_size (hnd_) > 0);

      if ((cs = WUpdateStackFlush (hnd_)) != WCS_OK)
        return cs;

      assert (hnd_->buildingCmd == CMD_INVALID);
    }

  {
    uint8_t* const data_    = data (hnd_);
    const uint_t   dataSize = data_size (hnd_);

    assert ((dataSize == 0) || (hnd_->buildingCmd == CMD_UPDATE_STACK));
    assert ((dataSize > 0) || (hnd_->buildingCmd == CMD_INVALID));

    data_[dataSize] = CMD_UPDATE_FUNC_POP;
    store_le_int32 (count, data_ + dataSize + 1);

    set_data_size (hnd_, dataSize + spaceReq);
  }

  memset (hnd_->cmdInternal, 0, sizeof (hnd_->cmdInternal));
  hnd_->buildingCmd = CMD_UPDATE_STACK;

  return WCS_OK;
}

static uint_t
stack_top_basic_update (struct INTERNAL_HANDLER* hnd,
                        const uint_t             type,
                        const char* const        value)
{
  uint_t       cs        = WCS_OK;
  const uint_t spaceNeed = sizeof (uint8_t) +
                           sizeof (uint16_t) +
                           strlen (value) + 1;

  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  {
    uint8_t*     data_    = data (hnd);
    const uint_t currSize = data_size (hnd);

    data_    += currSize;
    *data_++  = CMD_UPDATE_FUNC_CHTOP;

    store_le_int16 (type, data_);
    data_ += sizeof (uint16_t);

    strcpy ((char*)data_, value);

    assert ((currSize + spaceNeed) <= max_data_size (hnd));
    set_data_size (hnd, currSize + spaceNeed);

    hnd->cmdInternal[LAST_UPDATE_OFF] = currSize;
  }

  assert (cs == WCS_OK);
  return cs;
}

uint_t
stack_top_field_basic_update (struct INTERNAL_HANDLER* hnd,
                              const uint_t             type,
                              const char* const      fieldNane,
                              const W_TABLE_ROW_INDEX  row,
                              const char* const      value)
{
  uint_t       cs        = WCS_OK;
  const uint_t spaceNeed = sizeof (uint8_t) +
                           sizeof (uint16_t) +
                           strlen (fieldNane) + 1 +
                           sizeof (uint64_t) +
                           strlen (value) + 1;

  assert (hnd != NULL);
  assert (strlen (fieldNane) > 0);
  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  {
    uint8_t*     data_    = data (hnd);
    const uint_t currSize = data_size (hnd);

    data_    += currSize;
    *data_++  = CMD_UPDATE_FUNC_CHTOP;

    store_le_int16 (type | WFT_FIELD_MASK, data_);
    data_ += sizeof (uint16_t);

    strcpy ((char*)data_, fieldNane);
    data_ += strlen (fieldNane) + 1;

    store_le_int64 (row, data_);
    data_ += sizeof (uint64_t);

    strcpy ((char*)data_, value);

    assert ((currSize + spaceNeed) <= max_data_size (hnd));
    set_data_size (hnd, currSize + spaceNeed);

    hnd->cmdInternal[LAST_UPDATE_OFF] = currSize;
  }

  assert (cs == WCS_OK);
  return cs;
}

static uint_t
stack_top_text_update (struct INTERNAL_HANDLER* hnd,
                       const W_ELEMENT_OFFSET   textOff,
                       const char* const      value)
{
  uint_t       cs        = WCS_OK;
  const uint_t fixedSize =  sizeof (uint8_t) +
                            sizeof (uint16_t) +
                            sizeof (uint64_t);
  const uint_t cmdSize   = fixedSize + strlen (value) + 1;

  uint8_t* data_;
  uint_t   currSize;

  assert (hnd != NULL);
  assert (strlen (value) > 0);

  if (utf8_strlen ((const uint8_t*)value) < 0)
    return WCS_INVALID_ARGS;
  else if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
  {
      const uint16_t prevType = from_le_int16 (++data_);
      if (prevType == WFT_TEXT)
        {
          data_ += sizeof (uint16_t);

          const uint64_t prevOffset = from_le_int64 (data_);
          data_ += sizeof (uint64_t);

          const int prevLen = utf8_strlen (data_);
          assert (prevLen >= 0);

          const uint_t prevRawlen = strlen ((const char*)data_);
          data_ += prevRawlen;
          if ((prevOffset + prevLen) == textOff)
            {
              const uint_t newDataSize = hnd->cmdInternal[LAST_UPDATE_OFF] +
                                         fixedSize +
                                         prevRawlen +
                                         strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              strcpy ((char*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  currSize = data_size (hnd);
  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (WFT_TEXT, data_);
  data_ += sizeof (uint16_t);

  store_le_int64 (textOff, data_);
  data_ += sizeof (uint64_t);

  strcpy ((char*)data_, value);

  assert ((currSize + cmdSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + cmdSize);

  hnd->cmdInternal[LAST_UPDATE_OFF] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

static uint_t
stack_top_field_text_update (struct INTERNAL_HANDLER* hnd,
                             const char* const      fieldName,
                             const W_TABLE_ROW_INDEX  row,
                             const W_ELEMENT_OFFSET   textOff,
                             const char* const      value)
{
  uint_t       cs         = WCS_OK;
  const uint_t fixedSize  = sizeof (uint8_t) +
                              sizeof (uint16_t) +
                              strlen (fieldName) + 1 +
                              sizeof (uint64_t) +
                              sizeof (uint64_t);
  const uint_t cmdSize    = fixedSize + strlen (value) + 1;

  uint8_t* data_;
  uint_t   currSize;

  assert (hnd != NULL);
  assert (strlen (value) > 0);
  assert (strlen (fieldName) > 0);

  if (utf8_strlen ((const uint8_t*)value) < 0)
    return WCS_INVALID_ARGS;
  else if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
  {
      const uint16_t prevType = from_le_int16 (++data_);
      if (prevType == (WFT_TEXT | WFT_FIELD_MASK))
        {
          data_ += sizeof (uint16_t);

          const char* const prevFieldName = (const char*)data_;
          data_ += strlen (prevFieldName) + 1;

          const uint64_t prevRow = from_le_int64 (data_);
          data_ += sizeof (uint64_t);

          const uint64_t prevOffset = from_le_int64 (data_);
          data_ += sizeof (uint64_t);

          const int prevLen = utf8_strlen (data_);
          assert (prevLen >= 0);

          const uint_t prevRawlen = strlen ((const char*)data_);
          data_ += prevRawlen;

          if ((strcmp (prevFieldName, fieldName) == 0)
              && (prevRow == row)
              && ((prevOffset + prevLen) == textOff))
            {
              const uint_t newDataSize = hnd->cmdInternal[LAST_UPDATE_OFF] +
                                         fixedSize +
                                         prevRawlen +
                                         strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              strcpy ((char*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  currSize = data_size (hnd);
  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (WFT_TEXT | WFT_FIELD_MASK, data_),
  data_ += sizeof (uint16_t);

  strcpy ((char*)data_, fieldName);
  data_ += strlen (fieldName) + 1;

  store_le_int64 (row, data_);
  data_ += sizeof (uint64_t);

  store_le_int64 (textOff, data_);
  data_ += sizeof (uint64_t);

  strcpy ((char*)data_, value);

  assert ((currSize + cmdSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + cmdSize);

  hnd->cmdInternal[LAST_UPDATE_OFF] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

static uint_t
stack_top_array_basic_update (struct INTERNAL_HANDLER* hnd,
                              const uint_t             type,
                              const W_ELEMENT_OFFSET   arrayOff,
                              const char* const      value)
{
  uint_t       cs         = WCS_OK;
  uint_t       countOff   = sizeof (uint8_t) + sizeof (uint16_t);
  const uint_t fixedSize  = countOff + sizeof (uint16_t) + sizeof (uint64_t);
  const uint_t cmdSize    = fixedSize + strlen (value) + 1;

  assert (hnd != NULL);
  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));
  assert (strlen (value) > 0);

  if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  uint8_t* data_ = data (hnd);
  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
  {
      data_ += hnd->cmdInternal[LAST_UPDATE_OFF];

      const uint16_t prevType = from_le_int16 (++data_);
      if ((type | WFT_ARRAY_MASK) == prevType)
        {
          data_ += sizeof (uint16_t);

          const uint16_t prevCount = from_le_int16 (data_);
          assert (prevCount > 0);
          data_ += sizeof (uint16_t);

          const uint64_t prevOffset = from_le_int64 (data_);
          data_ += sizeof (uint64_t);

          if ((prevCount < 0xFFFF) &&
              (prevOffset + prevCount) == arrayOff)
            {
              data_ = data (hnd) +
                      hnd->cmdInternal[LAST_UPDATE_OFF] +
                      countOff;
              store_le_int16 (prevCount + 1, data_);

              const uint_t newDataSize = data_size (hnd) + strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));

              data_ = data (hnd) + data_size (hnd);
              strcpy ((char*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  const uint_t currSize = data_size (hnd);

  data_    = data (hnd) + currSize;
  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (type | WFT_ARRAY_MASK, data_);
  data_ += sizeof (uint16_t);

  store_le_int16 (1, data_);
  data_ += sizeof (uint16_t);

  store_le_int64 (arrayOff, data_);
  data_ += sizeof (uint64_t);

  strcpy ((char*)data_, value);

  assert ((currSize + cmdSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + cmdSize);

  hnd->cmdInternal[LAST_UPDATE_OFF] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

static uint_t
stack_top_field_array_basic_update (struct INTERNAL_HANDLER* hnd,
                                    const uint_t             type,
                                    const char*            fieldName,
                                    const W_TABLE_ROW_INDEX  row,
                                    const W_ELEMENT_OFFSET   arrayOff,
                                    const char* const      value)
{
  uint_t       cs         = WCS_OK;
  uint_t       countOff   = sizeof (uint8_t) +
                            sizeof (uint16_t) +
                            strlen (fieldName) + 1 +
                            sizeof (uint64_t);
  const uint_t fixedSize  = countOff + sizeof (uint16_t) + sizeof (uint64_t);
  const uint_t cmdSize    = fixedSize + strlen (value) + 1;

  uint8_t* data_;
  uint_t   currSize;

  assert (hnd != NULL);
  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));
  assert (strlen (fieldName) > 0);
  assert (strlen (value) > 0);

  if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
  {
      const uint16_t prevType = from_le_int16 (++data_);
      if ((type | WFT_ARRAY_MASK | WFT_FIELD_MASK) == prevType)
        {
          data_ += sizeof (uint16_t);

          const char* prevFielName = (const char*)data_;
          data_ += strlen (prevFielName) + 1;

          const uint64_t prevRow = from_le_int64 (data_);
          data_ += sizeof (uint64_t);

          const uint16_t prevCount = from_le_int16 (data_);
          assert (prevCount > 0);
          data_ += sizeof (uint16_t);

          const uint64_t prevOffset = from_le_int64 (data_);
          data_ += sizeof (uint64_t);

          if (((prevOffset + prevCount) == arrayOff)
              && (prevRow == row)
              && (strcmp (prevFielName, fieldName) == 0))
            {
              data_ = data (hnd) + hnd->cmdInternal[LAST_UPDATE_OFF] + countOff;
              store_le_int16 (prevCount + 1, data_);

              const uint_t newDataSize = data_size (hnd) + strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              data_ = data (hnd) + data_size (hnd);
              strcpy ((char*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  data_    = data (hnd) + currSize;
  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (type | WFT_ARRAY_MASK | WFT_FIELD_MASK, data_);
  data_ += sizeof (uint16_t);

  strcpy ((char*)data_, fieldName);
  data_ += strlen (fieldName) + 1;

  store_le_int64 (row, data_);
  data_ += sizeof (uint64_t);

  store_le_int16 (1, data_);
  data_ += sizeof (uint16_t);

  store_le_int64 (arrayOff, data_);
  data_ += sizeof (uint64_t);

  strcpy ((char*)data_, value);

  assert ((currSize + cmdSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + cmdSize);

  hnd->cmdInternal[LAST_UPDATE_OFF] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

uint_t
WUpdateStackValue (const W_CONNECTOR_HND         hnd,
                   const unsigned int            type,
                   const char* const             fieldName,
                   const W_TABLE_ROW_INDEX       row,
                   const W_ELEMENT_OFFSET        arrayOff,
                   const W_ELEMENT_OFFSET        textOff,
                   const char* const             value)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint32_t cs = WCS_OK;

  if ((hnd_ == NULL)
      || (value == NULL)
      || (value[0] == 0)
      || (type < WFT_BOOL) || (type > WFT_TEXT))
    {
      return WCS_INVALID_ARGS;
    }
  else if ((hnd_->buildingCmd != CMD_INVALID)
           && (hnd_->buildingCmd != CMD_UPDATE_STACK))
    {
      return WCS_INCOMPLETE_CMD;
    }

  if (hnd_->buildingCmd == CMD_INVALID)
    set_data_size (hnd_, 0);

  if ((row == WIGNORE_ROW) && (arrayOff == WIGNORE_OFF))
    {
      if (fieldName != WIGNORE_FIELD)
        return WCS_TYPE_MISMATCH;
      else if (type != WFT_TEXT)
        {
          if (textOff != WIGNORE_OFF)
            return WCS_INVALID_TEXT_OFF;

          cs = stack_top_basic_update (hnd_, type, value);
        }
      else
        {
          if (textOff == WIGNORE_OFF)
            return WCS_INVALID_TEXT_OFF;

          cs = stack_top_text_update (hnd_, textOff, value);
        }
    }
  else if ((row == WIGNORE_ROW) && (arrayOff != WIGNORE_OFF))
    {
      if (fieldName != WIGNORE_FIELD)
        return WCS_TYPE_MISMATCH;
      else if (type == WFT_TEXT)
        return WCS_OP_NOTSUPP;
      else
        {
          if (textOff != WIGNORE_OFF)
            return WCS_INVALID_TEXT_OFF;

          cs = stack_top_array_basic_update (hnd_,
                                             type,
                                             arrayOff,
                                             value);
        }
    }
  else if (row != WIGNORE_ROW)
    {
      if (fieldName == WIGNORE_FIELD)
        return WCS_INVALID_FIELD;

      if (arrayOff == WIGNORE_OFF)
        {
          if (type == WFT_TEXT)
            {
              if (textOff == WIGNORE_OFF)
                return WCS_INVALID_TEXT_OFF;

              cs = stack_top_field_text_update (hnd_,
                                                fieldName,
                                                row,
                                                textOff,
                                                value);
            }
          else
            {
              if (textOff != WIGNORE_OFF)
                return WCS_INVALID_TEXT_OFF;

              cs = stack_top_field_basic_update (hnd_,
                                                 type,
                                                 fieldName,
                                                 row,
                                                 value);
            }
        }
      else
        {
          if (type == WFT_TEXT)
            return WCS_OP_NOTSUPP;
          else
            {
              if (textOff != WIGNORE_OFF)
                return WCS_INVALID_TEXT_OFF;

              cs = stack_top_field_array_basic_update (hnd_,
                                                       type,
                                                       fieldName,
                                                       row,
                                                       arrayOff,
                                                       value);
            }
        }
    }
  else
    {
      assert (FALSE);
      cs = WCS_GENERAL_ERR;
    }

  hnd_->buildingCmd = CMD_UPDATE_STACK;

  return cs;
}

uint_t
WUpdateStackFlush (const W_CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint32_t  cs  = WCS_OK;

  uint16_t  type;

  if (hnd_ == NULL)
    return WCS_INVALID_ARGS;
  else if (hnd_->buildingCmd != CMD_UPDATE_STACK)
    {
      if (hnd_->buildingCmd == CMD_INVALID)
        {
          if (data_size (hnd_) == 0)
            return WCS_OK;
          else
            return WCS_INCOMPLETE_CMD;
        }
    }

  hnd_->buildingCmd = CMD_INVALID;
  if (data_size (hnd_) == 0)
    return WCS_OK;

  if ((cs = send_command (hnd_, CMD_UPDATE_STACK)) != WCS_OK)
    return cs;

  if ((cs = recieve_answer (hnd_, &type)) != WCS_OK)
    return cs;
  else if (type != CMD_UPDATE_STACK_RSP)
    return WCS_UNEXPECTED_FRAME;

  cs = from_le_int32 (data (hnd_));

  memset (hnd_->cmdInternal, 0, sizeof (hnd_->cmdInternal));
  set_data_size (hnd_, 0);

  return cs;
}

static uint_t
send_stack_read_req (struct INTERNAL_HANDLER* const hnd,
                     const char*                  field,
                     const uint64_t                 row,
                     const uint64_t                 arrayOff,
                     const uint64_t                 textOff)
{
  uint_t   cs       = WCS_OK;
  uint16_t type     = 0;

  if (field == WIGNORE_FIELD)
    field = WANONIM_FIELD;

  const uint_t neededSize = strlen (field) + 1 +
                            sizeof (uint64_t) +
                            sizeof (uint64_t) +
                            sizeof (uint64_t);

  if (neededSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  {
    uint64_t       dataSize = 0;
    uint8_t* const data_    = data (hnd);

    strcpy ((char*)data_, field);
    dataSize += strlen (field) + 1;

    store_le_int64 (row, data_ + dataSize);
    dataSize += sizeof (uint64_t);

    store_le_int64 (arrayOff, data_ + dataSize);
    dataSize += sizeof (uint64_t);

    store_le_int64 (textOff, data_ + dataSize);
    dataSize += sizeof (uint64_t);

    set_data_size (hnd, dataSize);

    memset (hnd->cmdInternal, 0, sizeof (hnd->cmdInternal));
  }

  if ((cs = send_command (hnd, CMD_READ_STACK)) != WCS_OK)
    return cs;

  if ((cs = recieve_answer (hnd, &type)) != WCS_OK)
    return cs;
  else if (type != CMD_READ_STACK_RSP)
    return WCS_UNEXPECTED_FRAME;

  cs = from_le_int32 (data (hnd));

  return cs;
}

static uint_t
next_table_field_off (struct INTERNAL_HANDLER* const hnd,
                      uint_t                         position)
{
  const uint8_t* const data_    = data (hnd);
  const uint_t         dataSize = data_size (hnd);

  const uint_t nameLen = strlen ((const char*)data_ + position) + 1;
  uint16_t     type    = from_le_int16 (data_ + position + nameLen);

  position += nameLen + sizeof (uint16_t);
  assert (position < dataSize);

  if (type & WFT_ARRAY_MASK)
    {
      const uint_t arraySize = from_le_int64 (data_ + position);
      position += sizeof (uint64_t);

      if (arraySize != 0)
        {
          uint64_t arrayOff = from_le_int64 (data_ + position);
          assert (arrayOff < arraySize);
          position += sizeof (uint64_t);

          while (arrayOff < arraySize)
            {
              if ((type & 0xFF) != WFT_TEXT)
                {
                  assert (strlen ((const char*)data_ + position) > 0);
                  position += strlen ((const char*)data_ + position) + 1;
                }
              else
                {
                  assert (from_le_int32 (data_ + position) > 0);
                  position += 2 * sizeof (uint64_t);

                  assert (position < dataSize);
                  assert (strlen ((const char*)data_ + position) > 0);
                  position += strlen ((const char*)data_ + position) + 1;
                }

              if (position >= dataSize)
                {
                  assert (position == dataSize);
                  return 0;
                }

              ++arrayOff;
            }
        }
    }
  else
    {
      if (type == WFT_TEXT)
        {
          const uint64_t charsCount = from_le_int64 (data_ + position);
          position += sizeof (uint64_t);

          if (charsCount > 0)
            {
              assert (from_le_int64 (data_ + position) < charsCount);
              position += sizeof (uint64_t);
              position += strlen ((const char*)data_ + position) + 1;
            }
        }
      else
        {
          assert ((type >= WFT_BOOL) && (type < WFT_TEXT));

          position += strlen ((const char*)data_ + position) + 1;
        }
    }

  if (position >= dataSize)
    {
      assert (position == dataSize);
      return 0;
    }

  return position;
}

static uint_t
get_table_field_off (struct INTERNAL_HANDLER* const hnd,
                     const char* const            field,
                     uint_t                         fromPos)
{
  const uint8_t* const data_    = data (hnd);
  const uint_t         dataSize = data_size (hnd);

  assert (hnd->lastCmdRespReceived == CMD_READ_STACK_RSP);
  assert (fromPos < data_size(hnd));

  assert (strlen (field) > 0);
  do
    {
      const char* cachedField = (const char*)data_ + fromPos;
      if (strcmp (cachedField, field) == 0)
        return fromPos;

      fromPos = next_table_field_off (hnd, fromPos);
    }
  while ((0 < fromPos) && (fromPos < dataSize));

  return 0; /* We did not find anything */
}

static uint_t
get_row_offset (struct INTERNAL_HANDLER* const hnd,
                const uint64_t                 row)
{
  const uint8_t* const data_    = data (hnd);
  const uint_t         dataSize = data_size (hnd);

  uint16_t type       = from_le_int16 (data_ + sizeof (uint32_t));
  uint_t   rowOff     = sizeof (uint32_t) + sizeof (uint16_t);
  uint64_t currentRow = 0;

  assert (hnd->lastCmdRespReceived == CMD_READ_STACK_RSP);
  assert (dataSize > (uint32_t) + sizeof (uint16_t));
  assert (from_le_int32 (data_) == WCS_OK);

  if (row >= from_le_int64 (data_ + rowOff))
    return 0;

  rowOff     += sizeof (uint64_t);
  currentRow  = from_le_int64 (data_ + rowOff);
  rowOff     += sizeof (uint64_t);

  if (currentRow > row)
    return 0;

  if (type == WFT_TABLE_MASK)
    {
      const uint_t fieldsCount = from_le_int16 (data_ + rowOff);
      rowOff += sizeof (uint16_t);

      while (currentRow < row)
        {
          uint_t i;
          for (i = 0; (i < fieldsCount) && (rowOff < dataSize); ++i)
            {
              const uint_t fieldOff = next_table_field_off (hnd, rowOff);
              if (fieldOff == 0)
                return 0;

              rowOff = fieldOff;
            }

          if (rowOff >= dataSize)
            {
              assert (rowOff == dataSize);
              return 0;
            }

          ++currentRow;
        }
      return rowOff;
    }

  assert (type & WFT_FIELD_MASK);

  type &= ~WFT_FIELD_MASK;
  if (type & WFT_ARRAY_MASK)
    {
      type &= ~WFT_ARRAY_MASK;
      assert ((WFT_BOOL <= type) && (type <= WFT_TEXT));

      while ((currentRow < row)
             && (rowOff < dataSize))
        {
          const uint_t arrayCount = from_le_int64 (data_ + rowOff);
          rowOff += sizeof (uint64_t);
          assert (rowOff <= dataSize);

          if (arrayCount > 0)
            {
              uint_t arrayOff = from_le_int64 (data_ + rowOff);
              rowOff += sizeof (uint64_t);

              while ((arrayOff < arrayCount)
                     && (rowOff < dataSize))
                {
                  if (type == WFT_TEXT)
                    {
                      assert (from_le_int64 (data_ + rowOff) > 0);
                      rowOff += sizeof (uint64_t);

                      assert (from_le_int64 (data_ + rowOff) > 0);
                      rowOff += sizeof (uint64_t);

                      assert (rowOff < dataSize);
                      assert (strlen ((const char*)data_ + rowOff) > 0);
                      rowOff += strlen ((const char*)data_ + rowOff) + 1;
                    }
                  else
                    {
                      assert (rowOff < dataSize);
                      assert (strlen ((const char*)data_ + rowOff) > 0);
                      rowOff += strlen ((const char*)data_ + rowOff) + 1;
                    }
                  arrayOff++;
                }
            }

          ++currentRow;
        }

      if (rowOff >= dataSize)
        {
          assert (rowOff == dataSize);
          return 0;
        }

      return rowOff;
    }

  assert ((WFT_BOOL <= type) && (type <= WFT_TEXT));
  while ((currentRow < row)
         && (rowOff < dataSize))
    {
      if (type == WFT_TEXT)
        {
          const uint_t charsCount = from_le_int64 (data_ + sizeof (uint64_t));
          rowOff += sizeof (uint64_t);
          assert (rowOff <= dataSize);

          if (charsCount > 0)
            {
              const uint_t textOff = from_le_int64 (data_ + sizeof (uint64_t));
              rowOff += sizeof (uint64_t);
              assert (rowOff <= dataSize);

              assert (textOff < charsCount);
              assert (strlen ((const char*)data_ + rowOff) > 0);
              rowOff += strlen ((const char*)data_ + rowOff) + 1;
            }
        }
      else
        {
            assert (strlen ((const char*)data_ + rowOff) > 0);
            rowOff += strlen ((const char*)data_ + rowOff) + 1;
        }

      currentRow++;
    }

  if (rowOff >= dataSize)
    {
      assert (rowOff == dataSize);
      return 0;
    }

  return rowOff;
}

static uint_t
get_array_el_off (struct INTERNAL_HANDLER* const hnd,
                  const uint_t                   type,
                  const uint64_t                 arrayOff,
                  uint_t                         fromPos,
                  uint64_t* const                pArraySize)
{
  const uint8_t* data_    = data (hnd);
  const uint_t   dataSize = data_size (hnd);
  const uint64_t elCount  = from_le_int64 (data_ + fromPos);
  uint64_t       currOff;

  assert (sizeof (uint32_t) <= fromPos);
  fromPos += sizeof (uint64_t);
  assert (fromPos <= dataSize);
  assert ((WFT_BOOL <= type) && (type <= WFT_TEXT));

  *pArraySize = elCount;
  if ((elCount == 0) || (*pArraySize <= arrayOff))
    return INVALID_OFF;


  assert (fromPos < dataSize);
  currOff = from_le_int64 (data_ + fromPos);
  if (arrayOff < currOff)
    return 0;

  fromPos += sizeof (uint64_t);
  while ((currOff < arrayOff)
         && (fromPos < dataSize))
    {
      assert (fromPos < dataSize);
      if (type == WFT_TEXT)
        {
          assert (from_le_int64 (data_ + fromPos) > 0);
          fromPos += sizeof (uint64_t);

          fromPos += sizeof (uint64_t);
          assert (fromPos < dataSize);

          assert (strlen ((const char*)data_ + fromPos) > 0);
          fromPos += strlen ((const char*)data_ + fromPos) + 1;
        }
      else
        {
          assert (strlen ((const char*)data_ + fromPos) > 0);
          fromPos += strlen ((const char*)data_ + fromPos) + 1;
        }

      ++currOff;
    }

  assert (currOff == arrayOff);
  if (fromPos >= dataSize)
    {
      assert (fromPos == dataSize);
      return 0;
    }

  return fromPos;
}

static uint_t
text_el_off (struct INTERNAL_HANDLER* const hnd,
             const uint64_t                 textOff,
             uint16_t                       fromPos,
             uint64_t* const                pCharsCount)
{
  const uint8_t* data_    = data (hnd);
  const uint_t   dataSize = data_size (hnd);
  const uint64_t elCount  = from_le_int64 (data_ + fromPos);
  uint64_t       currOff;

  *pCharsCount = elCount;
  if ((elCount == 0) || (elCount <= textOff))
    return INVALID_OFF;

  fromPos += sizeof (uint64_t);
  assert (fromPos < dataSize);

  currOff = from_le_int64 (data_ + fromPos);
  if (textOff < currOff)
    return 0;

  fromPos += sizeof (uint64_t);
  assert (fromPos < dataSize);

  while ((currOff < textOff)
         && (fromPos < dataSize))
    {
      uint32_t ch     = 0;
      uint_t   chSize = decode_utf8_char (data_ +fromPos, &ch);

      assert (chSize > 0);
      assert (fromPos < dataSize);

      ++currOff, fromPos += chSize;
    }

  if ((fromPos >= dataSize)
      || (data_[fromPos] == 0))
    {
      assert (fromPos <= dataSize);
      return 0;
    }

  assert (currOff == textOff);
  return fromPos;
}

unsigned int
WGetStackValueRowsCount (const W_CONNECTOR_HND       hnd,
                         unsigned long long* const   pCount)
{
  struct INTERNAL_HANDLER* hnd_  = (struct INTERNAL_HANDLER*)hnd;
  uint_t                   cs    = WCS_OK;
  uint_t                   type  = 0;
  const  uint8_t*          data_;

  if ((hnd == NULL) || (pCount == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  if ((hnd_->lastCmdRespReceived != CMD_READ_STACK_RSP)
      || (from_le_int32 (data (hnd)) != WCS_OK))
    {
      cs = send_stack_read_req (hnd_,
                                WIGNORE_FIELD,
                                WIGNORE_ROW,
                                WIGNORE_OFF,
                                WIGNORE_OFF);
      if (cs != WCS_OK)
        return cs;
    }

  data_ = data (hnd_);

  assert (data_size (hnd_) > sizeof (uint32_t) + sizeof (uint16_t));
  assert (from_le_int32 (data_) == WCS_OK);

  type  = from_le_int16 (data_ + sizeof (uint32_t));

  assert (from_le_int32 (data_) == WCS_OK);

  if (type & WFT_TABLE_MASK)
    *pCount = from_le_int64 (data_ + sizeof (uint32_t) + sizeof (uint16_t));
  else if (type & WFT_FIELD_MASK)
    {
      uint_t offset = sizeof (uint32_t) + sizeof (uint16_t);
      offset += strlen ((const char*)data_ + offset) + 1;

      assert (offset < data_size (hnd_));

      *pCount = from_le_int64 (data_ + offset);
    }
  else
    cs = WCS_TYPE_MISMATCH;

  return cs;
}

static int
get_stack_value (struct INTERNAL_HANDLER* const hnd,
                 const char* const              field,
                 const W_TABLE_ROW_INDEX        row,
                 const W_ELEMENT_OFFSET         arrayOff,
                 const W_ELEMENT_OFFSET         textOff,
                 uint64_t* const                pCount,
                 uint16_t* const                pType,
                 const char** const           pValue)
{
  uint_t                   cs       = WCS_OK;
  bool_t                   tryAgain = TRUE;
  const  uint8_t*          data_;
  uint_t                   dataSize;
  uint_t                   dataOffset;

  assert (pType != NULL);
  assert (pCount != NULL);

  if ((hnd == NULL) || (pValue == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  if ((hnd->lastCmdRespReceived != CMD_READ_STACK_RSP)
      || (from_le_int32 (data (hnd)) != WCS_OK))
    {
      cs = send_stack_read_req (hnd, field, row, arrayOff, textOff);

      tryAgain = FALSE;
      if (cs != WCS_OK)
        goto exit_get_stack_entry;
    }

resend_req_get_stack_entry:

  data_      = data (hnd);
  dataSize   = data_size (hnd);
  dataOffset = sizeof (uint32_t);

  *pType      = from_le_int16 (data_ + dataOffset);
  dataOffset += sizeof (uint16_t);

  if (*pType == WFT_TABLE_MASK)
    {
      if ((field == WIGNORE_FIELD) || (strlen (field) == 0))
        {
          cs = WCS_INVALID_FIELD;
          goto exit_get_stack_entry;
        }

      dataOffset = get_row_offset (hnd, row);
      assert (dataOffset <= dataSize);
      if ((dataOffset == 0) || (dataOffset == dataSize))
        {
          cs = WCS_INVALID_ROW;
          goto exit_get_stack_entry;
        }

      dataOffset = get_table_field_off (hnd, field, dataOffset);
      assert (dataOffset <= dataSize);
      if ((dataOffset == 0) || (dataOffset == dataSize))
        {
          cs = WCS_INVALID_FIELD;
          goto exit_get_stack_entry;
        }

      assert (strcmp ((const char*)data_ + dataOffset, field) == 0);
      dataOffset += strlen (field) + 1;
      assert (dataOffset < dataSize);

      *pType = from_le_int16 (data_ + dataOffset);
      dataOffset += sizeof (uint16_t);
      assert (dataOffset < dataSize);
    }
  else if (*pType & WFT_FIELD_MASK)
    {
      if (field != WIGNORE_FIELD)
        {
          cs = WCS_INVALID_FIELD;
          goto exit_get_stack_entry;
        }

      dataOffset = get_row_offset (hnd, row);
      assert (dataOffset < dataSize);
      if ((dataOffset == 0) || (dataOffset == dataSize))
        {
          cs = WCS_INVALID_ROW;
          goto exit_get_stack_entry;
        }

      *pType &= ~WFT_FIELD_MASK;
    }
  else
    {
      if (field != WIGNORE_FIELD)
        {
          cs = WCS_INVALID_FIELD;
          goto exit_get_stack_entry;
        }
      else if (row != WIGNORE_OFF)
        {
          cs = WCS_INVALID_ROW;
          goto exit_get_stack_entry;
        }
    }

  if (*pType & WFT_ARRAY_MASK)
    {
      dataOffset = get_array_el_off (hnd,
                                     *pType & 0xFF,
                                     arrayOff,
                                     dataOffset,
                                     pCount);
      assert ((dataOffset < dataSize) || (dataOffset == INVALID_OFF));
      if (dataOffset == INVALID_OFF)
        {
          cs = WCS_INVALID_ARRAY_OFF;
          goto exit_get_stack_entry;
        }
      else if (dataOffset == 0)
        {
          cs = WCS_INVALID_ARGS;
          goto exit_get_stack_entry;
        }
      *pType &= ~WFT_ARRAY_MASK;
    }
  else
    {
      if (arrayOff != WIGNORE_OFF)
        {
          cs = WCS_TYPE_MISMATCH;
          goto exit_get_stack_entry;
        }
    }

  assert ((WFT_BOOL <= *pType) && (*pType <= WFT_TEXT));
  if (*pType == WFT_TEXT)
    {
      dataOffset = text_el_off (hnd, textOff, dataOffset, pCount);

      assert ((dataOffset < dataSize) || (dataOffset == INVALID_OFF));
      if (dataOffset == INVALID_OFF)
        {
          cs = WCS_INVALID_TEXT_OFF;
          goto exit_get_stack_entry;
        }
      else if (dataOffset == 0)
        {
          cs = WCS_INVALID_ARGS;
          goto exit_get_stack_entry;
        }

      *pType &= ~WFT_ARRAY_MASK;
      *pValue = (const char*)data_ + dataOffset;
    }
  else
    {
      if (textOff != WIGNORE_OFF)
        {
          cs = WCS_TYPE_MISMATCH;
          goto exit_get_stack_entry;
        }

      assert (dataOffset < dataSize);
      *pValue = (const char*)data_ + dataOffset;
    }

exit_get_stack_entry:

  if (tryAgain && (cs != WCS_OK))
    {
      tryAgain = FALSE;

      cs = send_stack_read_req (hnd, field, row, arrayOff, textOff);
      if (cs == WCS_OK)
        goto resend_req_get_stack_entry;
    }

  return cs;
}

unsigned int
WGetStackArrayElementsCount (const W_CONNECTOR_HND   hnd,
                             const char*             field,
                             const W_TABLE_ROW_INDEX row,
                             unsigned long long*     pCount)
{
  uint64_t      count = 0;
  uint16_t      type  = 0;
  const char* value = NULL;

  if (pCount == NULL)
    return WCS_INVALID_ARGS;

  uint_t cs = get_stack_value (hnd,
                               field,
                               row,
                               0,
                               WIGNORE_OFF,
                               &count,
                               &type,
                               &value);

  if ((cs != WCS_OK) && (cs != WCS_INVALID_ARRAY_OFF))
    return cs;


  *pCount = count;
  return WCS_OK;
}

unsigned int
WGetStackTextLengthCount (const W_CONNECTOR_HND   hnd,
                          const char*             field,
                          const W_TABLE_ROW_INDEX row,
                          const W_TABLE_ROW_INDEX arrayOff,
                          unsigned long long*     pCount)
{
  uint64_t      count = 0;
  uint16_t      type  = 0;
  const char* value = NULL;

  if (pCount == NULL)
    return WCS_INVALID_ARGS;

  uint_t cs = get_stack_value (hnd,
                               field,
                               row,
                               arrayOff,
                               0,
                               &count,
                               &type,
                               &value);

  if ((cs != WCS_OK) && (cs != WCS_INVALID_TEXT_OFF))
    return cs;

  *pCount = count;
  return WCS_OK;
}

unsigned int
WGetStackValueEntry (const W_CONNECTOR_HND   hnd,
                     const char* const       field,
                     W_TABLE_ROW_INDEX       row,
                     const W_ELEMENT_OFFSET  arrayOff,
                     const W_ELEMENT_OFFSET  textOff,
                     const char** const      pValue)
{
  uint64_t  count = 0;
  uint16_t  type  = 0;

  uint_t cs = get_stack_value (hnd,
                               field,
                               row,
                               arrayOff,
                               textOff,
                               &count,
                               &type,
                               pValue);

  if (cs != WCS_OK)
    return cs;

  assert (*pValue != NULL);

  return WCS_OK;
}

static uint_t
describe_proc_parameter (struct INTERNAL_HANDLER* hnd,
                         const char* const      procName,
                         const uint_t             parameter)
{
  const uint_t nameLen = strlen (procName) + 1;

  uint_t   cs = WCS_OK;
  uint8_t* data_;
  uint16_t type;

  if ((nameLen + sizeof (uint16_t) > max_data_size (hnd))
      || (parameter > 0xFFFF))
    {
      cs = WCS_LARGE_ARGS;
      goto describe_proc_parameter_err;
    }

  set_data_size (hnd, nameLen + sizeof (uint32_t));
  data_ = data (hnd);
  store_le_int16 (parameter, data_);
  strcpy ((char*)data_ + 2 * sizeof (uint16_t), procName);

  if ((cs = send_command (hnd, CMD_DESC_PROC_PARAM)) != WCS_OK)
    goto describe_proc_parameter_err;

  if ((cs = recieve_answer (hnd, &type)) != WCS_OK)
    goto describe_proc_parameter_err;
  else if (type != CMD_DESC_PROC_PARAM_RSP)
    {
      cs = WCS_INVALID_FRAME;
      goto describe_proc_parameter_err;
    }

  data_ = data (hnd);
  if ((cs = from_le_int32 (data_)) != WCS_OK)
    goto describe_proc_parameter_err;
  else if ((strcmp ((char*)data_ + sizeof (uint32_t), procName) != 0)
          || (sizeof (uint32_t) + nameLen + 3 * sizeof (uint16_t) >
              data_size (hnd)))
    {
      cs = WCS_INVALID_FRAME;
      goto describe_proc_parameter_err;
    }

  assert (cs == WCS_OK);
  return cs;

describe_proc_parameter_err:

  assert (cs != WCS_OK);
  hnd->lastCmdRespReceived = CMD_INVALID_RSP;

  return cs;
}

unsigned int
WProcedureParametersCount (const W_CONNECTOR_HND hnd,
                           const char* const     procName,
                           unsigned int* const   pCount)
{
  struct INTERNAL_HANDLER* hnd_    = (struct INTERNAL_HANDLER*)hnd;
  const uint8_t*           data_   = NULL;
  uint_t                   cs      = WCS_OK;

  if ((hnd == NULL)
      || (procName == NULL)
      || (pCount == NULL)
      || (strlen (procName) == 0))
    {
      return WCS_INVALID_ARGS;
    }
  else if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);
      assert (from_le_int32 (data_) == WCS_OK);

      if (strcmp (procName, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          if ((cs = describe_proc_parameter (hnd, procName, 0)) != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procName, 0)) != WCS_OK)
    return cs;

  data_ = data (hnd_);
  assert (cs == WCS_OK);
  assert (from_le_int32 (data_) == WCS_OK);

  *pCount = from_le_int16 (data_ + sizeof (uint32_t) + strlen (procName) + 1);

  return WCS_OK;
}

static uint_t
get_paratmeter_offset (struct INTERNAL_HANDLER* const       hnd,
                       const uint_t                         param,
                       uint16_t* const                      pOff)
{
  const uint8_t* data_   = data (hnd);
  const uint_t   nameLen = strlen ((char*)data_ + sizeof (uint32_t)) + 1;

  uint16_t count = 0;
  uint16_t hint  = 0;
  uint_t   it;

  assert (data_size (hnd) > sizeof (uint32_t) + sizeof (uint16_t));
  assert (hnd->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP);
  assert (from_le_int32 (data (hnd)) == WCS_OK);

  *pOff =  sizeof (uint32_t);
  *pOff += nameLen;

  count = from_le_int16 (data_ + *pOff); *pOff += sizeof (uint16_t);
  hint  = from_le_int16 (data_ + *pOff); *pOff += sizeof (uint16_t);

  if ((param < hint) || (count < param))
    return WCS_INVALID_ARGS;

  for (it = hint; (it < param) && (*pOff < data_size (hnd)); ++it)
    {
      const uint16_t type = from_le_int16 (data_ + *pOff);
      *pOff += sizeof (uint16_t);
      if (type & WFT_TABLE_MASK)
        {
          uint_t field;

          const uint16_t fieldsCount = from_le_int16 (data_ + *pOff);

          *pOff += sizeof (uint16_t);

          for (field = 0;
               (field < fieldsCount) && (*pOff < data_size (hnd));
               ++field)
            {
              *pOff += strlen ((char*)data_ + *pOff) + 1;
              *pOff += sizeof (uint16_t);
            }
        }
      else
        *pOff += sizeof (uint16_t);
    }

  if ((*pOff >= data_size (hnd)) || (hint != param))
    return WCS_INVALID_ARGS;

  return WCS_OK;
}

unsigned int
WProcedureParameter (const W_CONNECTOR_HND hnd,
                     const char* const     procName,
                     const unsigned int    parameter,
                     unsigned int* const   pRawType)
{
  struct INTERNAL_HANDLER* hnd_     = (struct INTERNAL_HANDLER*)hnd;
  const uint8_t*           data_    = NULL;
  uint_t                   cs       = WCS_OK;
  bool_t                   tryAgain = TRUE;

  uint16_t offset;

  if ((hnd == NULL)
      || (procName == NULL)
      || (pRawType == NULL)
      || (strlen (procName) == 0))
    {
      return WCS_INVALID_ARGS;
    }
  else if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);
      assert (from_le_int32 (data_) == WCS_OK);

      if (strcmp (procName, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          tryAgain = FALSE;
          cs = describe_proc_parameter (hnd, procName, parameter);
          if (cs != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procName, parameter)) != WCS_OK)
    {
      tryAgain = FALSE;
      return cs;
    }

proc_parameter_try_again:

  data_ = data (hnd_);
  assert (cs == WCS_OK);
  assert (from_le_int32 (data_) == WCS_OK);

  if ((cs = get_paratmeter_offset (hnd_, parameter, &offset)) != WCS_OK)
    goto proc_parameter_err;

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));
  *pRawType = from_le_int16 (data_ + offset);

  assert (cs == WCS_OK);
  return WCS_OK;

proc_parameter_err:

  assert (cs != WCS_OK);

  if (tryAgain)
    {
      tryAgain = FALSE;

      cs = describe_proc_parameter (hnd, procName, parameter);
      if (cs == WCS_OK)
        goto proc_parameter_try_again;
    }

  return cs;
}

unsigned int
WProcedureParameterFieldCount (const W_CONNECTOR_HND hnd,
                               const char* const     procName,
                               const unsigned int    parameter,
                               unsigned int* const   pCount)
{
  struct INTERNAL_HANDLER* hnd_     = (struct INTERNAL_HANDLER*)hnd;
  const uint8_t*           data_    = NULL;
  uint_t                   cs       = WCS_OK;
  bool_t                   tryAgain = TRUE;

  uint16_t offset;

  if ((hnd == NULL)
      || (procName == NULL)
      || (pCount == NULL)
      || (strlen (procName) == 0))
    {
      return WCS_INVALID_ARGS;
    }

  if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);
      assert (from_le_int32 (data_) == WCS_OK);

      if (strcmp (procName, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          cs = describe_proc_parameter (hnd, procName, parameter);
          if (cs != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procName, parameter)) != WCS_OK)
    return cs;

proc_parameter_field_count_try_again:

  data_ = data (hnd_);
  assert (cs == WCS_OK);
  assert (from_le_int32 (data_) == WCS_OK);

  if ((cs = get_paratmeter_offset (hnd_, parameter, &offset)) != WCS_OK)
    goto proc_parameter_field_count_err;

  tryAgain = FALSE;

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));

  offset += sizeof (uint16_t);
  *pCount = from_le_int16 (data_ + offset);

  assert (cs == WCS_OK);
  return WCS_OK;

proc_parameter_field_count_err:

  assert (cs != WCS_OK);

  if (tryAgain)
    {
      tryAgain = FALSE;

      cs = describe_proc_parameter (hnd, procName, parameter);
      if (cs == WCS_OK)
        goto proc_parameter_field_count_try_again;
    }

  return cs;
}

unsigned int
WProcedureParameterField (const W_CONNECTOR_HND hnd,
                          const char* const     procName,
                          const unsigned int    parameter,
                          const unsigned int    field,
                          const char**          pFieldName,
                          unsigned int* const   pFieldType)
{

  struct INTERNAL_HANDLER* hnd_     = (struct INTERNAL_HANDLER*)hnd;
  const uint8_t*           data_    = NULL;
  uint_t                   cs       = WCS_OK;
  bool_t                   tryAgain = TRUE;

  uint16_t  iterator;
  uint16_t  fieldsCount;
  uint16_t  offset;

  if ((hnd == NULL)
      || (procName == NULL)
      || (pFieldName == NULL)
      || (pFieldType == NULL)
      || (strlen (procName) == 0))
    {
      return WCS_INVALID_ARGS;
    }
  else if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);
      assert (from_le_int32 (data_) == WCS_OK);

      if (strcmp (procName, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          cs = describe_proc_parameter (hnd, procName, parameter);
          if (cs != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procName, parameter)) != WCS_OK)
    return cs;

proc_parameter_field_try_again:

  data_ = data (hnd_);
  assert (cs == WCS_OK);
  assert (from_le_int32 (data_) == WCS_OK);

  if ((cs = get_paratmeter_offset (hnd_, parameter, &offset)) != WCS_OK)
    goto proc_parameter_field_err;

  tryAgain = FALSE;

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));

  offset += sizeof (uint16_t);
  fieldsCount = from_le_int16 (data_ + offset);

  if (fieldsCount <= field)
    {
      cs = WCS_INVALID_ARGS;
      goto proc_parameter_field_err;
    }

  offset += sizeof (uint16_t);
  for (iterator = 0; iterator < field; ++iterator)
    {
      offset += strlen ((char*)data_ + offset) + 1;
      offset += sizeof (uint16_t);
    }

  *pFieldName = (char*)data_ + offset;
  offset += strlen (*pFieldName) + 1;
  *pFieldType = from_le_int16 (data_ + offset);

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));
  assert (cs == WCS_OK);
  return WCS_OK;

proc_parameter_field_err:

  assert (cs != WCS_OK);

  if (tryAgain)
    {
      tryAgain = FALSE;

      cs = describe_proc_parameter (hnd, procName, parameter);
      if (cs == WCS_OK)
        goto proc_parameter_field_try_again;
    }

  return cs;
}

unsigned int
WExecuteProcedure (const W_CONNECTOR_HND     hnd,
                   const char* const         procedure)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t   cs    = WCS_OK;
  uint8_t* data_ = NULL;
  uint16_t type  = 0;

  if ((hnd_ == NULL)
      || (procedure == NULL)
      || (procedure[0] == 0)
      || (hnd_->buildingCmd != CMD_INVALID))
    {
      return WCS_INCOMPLETE_CMD;
    }

  set_data_size (hnd, strlen (procedure) + 1);
  data_ = data (hnd);
  strcpy ((char*)data_, procedure);

  if ((cs = send_command (hnd, CMD_EXEC_PROC)) != WCS_OK)
    goto execute_proc_err;

  if ((cs = recieve_answer (hnd, &type)) != WCS_OK)
    goto execute_proc_err;
  else if (type != CMD_EXEC_PROC_RSP)
    {
      cs = WCS_INVALID_FRAME;
      goto execute_proc_err;
    }

execute_proc_err:
  return cs;
}
