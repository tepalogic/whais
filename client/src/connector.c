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

#include "utils/endianness.h"
#include "utils/wrandom.h"
#include "utils/wutf.h"
#include "utils/enc_3k.h"
#include "server/server_protocol.h"

#include "connector.h"
#include "client_connection.h"

static const char WANONIM_FIELD[] = "";

static const uint_t INVALID_OFF    = ~0;

/* Calculate how much data can fit in one communication frame. It depends on
 * advertised size, type of encryption, etc. */
static uint_t
max_data_size (const struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize = 0;

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

/* Get the size of the command's associated data. */
static uint_t
data_size (const struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize = 0;

  const uint16_t frameSize = load_le_int16 (&hnd->data[FRAME_SIZE_OFF]);

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

/* Set the size of the command's associated data. */
static void
set_data_size (struct INTERNAL_HANDLER* const hnd, const uint_t size)
{
  uint_t metaDataSize = 0;

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

/* Returns a pointer to the command associated data. */
static uint8_t*
data (struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize = 0;

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

/* Returns a pointer where the plain command header resides for further
 * processing before/after encryption step. */
static uint8_t*
raw_data (struct INTERNAL_HANDLER* const hnd)
{
  uint_t metaDataSize = 0;

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

/* Process the command frame before sending it to the server, according
 * to the connections specifics (e.g. used cipher, frame validations, etc).
 */
static uint_t
send_raw_frame (struct INTERNAL_HANDLER* const hnd,
                const uint8_t                  type)
{
  uint32_t status    = 0;
  uint_t   frameSize = load_le_int16 (hnd->data + FRAME_SIZE_OFF);

  if (hnd->cipher == FRAME_ENCTYPE_3K)
    {
      const uint16_t plainSize = frameSize;

      uint32_t firstKing, secondKing;
      uint8_t  prev, i;

      while (frameSize % sizeof (uint32_t) != 0)
        hnd->data[frameSize++] = wh_rnd () & 0xFF;

      firstKing  = wh_rnd () & 0xFFFFFFFF;
      store_le_int32 (firstKing,
                      hnd->data + FRAME_HDR_SIZE + ENC_3K_FIRST_KING_OFF);

      secondKing = wh_rnd () & 0xFFFFFFFF;
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

      store_le_int16 (wh_rnd () & 0xFFFF,
                      hnd->data + FRAME_HDR_SIZE + ENC_3K_SPARE_OFF);

      wh_buff_3k_encode (firstKing,
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

  status = whs_write (hnd->socket, hnd->data, frameSize);
  if (status != WOP_OK)
    return WENC_OS_ERROR (status);

  return WCS_OK;
}

/* Waith for a communication frame from server and do connection specfic
 * processing before gets forwarded to upper layers.
 */
static uint_t
receive_raw_frame (struct INTERNAL_HANDLER* const hnd)
{
  uint_t frameSize;
  uint_t frameRead = 0;

  /* Any frame will have at least FRAME_HDR_SIZE bytes.
   * Extract from this header the real size of the frame. */
  while (frameRead < FRAME_HDR_SIZE)
    {
      uint_t chunkSize = FRAME_HDR_SIZE - frameRead;

      const uint32_t status = whs_read (hnd->socket,
                                        hnd->data + frameRead,
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

  /* The real frame size. */
  frameSize = load_le_int16 (&hnd->data[FRAME_SIZE_OFF]);
  if ((frameSize < FRAME_HDR_SIZE)
      || (frameSize > hnd->dataSize)
      || (hnd->data[FRAME_ENCTYPE_OFF] != hnd->cipher))
    {
      return WCS_UNEXPECTED_FRAME;
    }

  while (frameRead < frameSize)
    {
      uint_t chunkSize = frameSize - frameRead;

      const uint32_t status = whs_read (hnd->socket,
                                        hnd->data + frameRead,
                                        &chunkSize);
      if (status != WOP_OK)
        return WENC_OS_ERROR (status);

      else if (chunkSize == 0)
        return WCS_DROPPED;

      frameRead += chunkSize;
    }

  if (load_le_int32 (hnd->data + FRAME_ID_OFF) != hnd->expectedFrameId)
    return WCS_UNEXPECTED_FRAME;

  if (hnd->cipher == FRAME_ENCTYPE_3K)
    {
      uint32_t  firstKing, secondKing;
      uint16_t  plainSize;
      uint8_t   i, prev;

      for (i = 0, prev = 0; i < ENC_3K_PLAIN_SIZE_OFF; ++i)
        {
          hnd->data[FRAME_HDR_SIZE + i] ^=
                              hnd->encriptionKey[prev % hnd->encKeySize];
          prev = hnd->data[FRAME_HDR_SIZE + i];
        }

      firstKing = load_le_int32 (hnd->data +
                                FRAME_HDR_SIZE +
                                ENC_3K_FIRST_KING_OFF);
      secondKing = load_le_int32 (hnd->data +
                                  FRAME_HDR_SIZE +
                                  ENC_3K_SECOND_KING_OFF);

      wh_buff_3k_decode (firstKing,
                         secondKing,
                         hnd->encriptionKey,
                         hnd->encKeySize,
                         hnd->data + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF,
                         frameSize - (FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF));

      plainSize = load_le_int16 (hnd->data+
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
              const uint16_t                 cmd)
{
  uint8_t* const rawData  = raw_data (hnd);
  uint_t         index    = 0;
  const uint_t   dataSize = data_size (hnd);
  uint_t         cs       = WCS_OK;
  uint16_t       chkSum   = 0;

  hnd->clientCookie = wh_rnd ();

  for (index = 0; index < dataSize; index++)
    chkSum += data (hnd)[index];

  store_le_int32 (hnd->clientCookie, rawData + PLAIN_CLNT_COOKIE_OFF);
  store_le_int32 (hnd->serverCookie, rawData + PLAIN_SERV_COOKIE_OFF);
  store_le_int16 (cmd, rawData + PLAIN_TYPE_OFF);
  store_le_int16 (chkSum, rawData + PLAIN_CRC_OFF);

  cs = send_raw_frame (hnd, FRAME_TYPE_NORMAL);
  return cs;
}

static uint_t
recieve_answer (struct INTERNAL_HANDLER* const hnd,
                uint16_t* const                outRsp)
{
  uint8_t* const rawData  = raw_data (hnd);
  uint_t         cs       = receive_raw_frame (hnd);
  uint_t         index    = 0;
  const uint_t   dataSize = data_size (hnd);
  uint16_t       chkSum   = 0;

  if (cs != WCS_OK)
    return cs;

  for (index = 0; index < dataSize; index++)
    chkSum += data (hnd)[index];

  if (chkSum != load_le_int16 (rawData + PLAIN_CRC_OFF))
    {
      cs = WCS_INVALID_FRAME;
      goto recieve_failure;
    }
  else if (load_le_int32 (rawData + PLAIN_CLNT_COOKIE_OFF) !=
           hnd->clientCookie)
    {
      cs = WCS_UNEXPECTED_FRAME;
      goto recieve_failure;
    }

  hnd->serverCookie = load_le_int32 (rawData + PLAIN_SERV_COOKIE_OFF);

  *outRsp = load_le_int16 (rawData + PLAIN_TYPE_OFF);
  if ((*outRsp == CMD_INVALID_RSP) || ((*outRsp & 1) == 0))
    {
      cs = WCS_INVALID_FRAME;
      goto recieve_failure;
    }
  hnd->lastCmdRespReceived = *outRsp;

  return WCS_OK;

recieve_failure:

  hnd->lastCmdRespReceived = CMD_INVALID_RSP;

  whs_close (hnd->socket);
  hnd->socket = INVALID_SOCKET;

  return cs;
}

uint_t
WConnect (const char* const    host,
          const char* const    port,
          const char* const    database,
          const char* const    password,
          const uint_t         userId,
          WH_CONNECTION* const pHnd)
{
  struct INTERNAL_HANDLER* result      = NULL;
  const uint_t             passwordLen = strlen (password);
  uint_t                   frameSize   = 0;
  uint32_t                 status      = WCS_OK;

  uint8_t                  tempBuffer[MIN_FRAME_SIZE];

  if ((host == NULL)
      || (port == NULL)
      || (database == NULL)
      || (password == NULL)
      || (pHnd == NULL)
      || (strlen (host) == 0)
      || (strlen (port) == 0)
      || (strlen (database) == 0))
    {
      status = WCS_INVALID_ARGS;
      goto fail_ret;
    }

  result = mem_alloc (sizeof (*result) + passwordLen);
  memset (result, 0, sizeof (*result));
  memcpy (result->encriptionKey, password, passwordLen);

  result->data       = tempBuffer;
  result->dataSize   = sizeof (tempBuffer);
  result->userId     = userId;
  result->socket     = INVALID_SOCKET;
  result->encKeySize = passwordLen;
  result->cipher     = FRAME_ENCTYPE_PLAIN;

  if ((status = whs_create_client (host, port, &result->socket)) != WCS_OK)
    {
      status = WENC_OS_ERROR (status);
      goto fail_ret;
    }

  /* The server is the one who starts the communication. The first frame
   * is an authenticate request and its size is fixed at minimum. */
  if ((status = read_raw_frame (result, &frameSize)) != WCS_OK)
    goto fail_ret;

  assert (frameSize == result->dataSize);

  {
      /* The authentication frame will always be send in plain, and it publish
       * some communication settings set at the server size (e.g. frame size,
       * cipher to be used, version, etc.). */
      uint_t serverFrameSize = 0;

      const uint32_t frameId = load_le_int32 (&result->data[FRAME_ID_OFF]);

      if ((frameId != 0)
          || (result->data[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT))
        {
          status = WCS_UNEXPECTED_FRAME;
          goto fail_ret;
        }

      assert (result->data[FRAME_ENCTYPE_OFF] == FRAME_ENCTYPE_PLAIN);

      result->cipher = result->data[FRAME_HDR_SIZE + FRAME_AUTH_ENC_OFF];
      if ((result->cipher != FRAME_ENCTYPE_PLAIN)
          && (result->cipher != FRAME_ENCTYPE_3K))
        {
          status = WCS_ENCTYPE_NOTSUPP;
          goto fail_ret;
        }
      result->version = load_le_int32 (result->data +
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
      serverFrameSize = load_le_int16 (result->data +
                                         FRAME_HDR_SIZE +
                                         FRAME_AUTH_SIZE_OFF);
      assert (MIN_FRAME_SIZE <= serverFrameSize);
      assert (serverFrameSize <= MAX_FRAME_SIZE);

      result->data     = mem_alloc (serverFrameSize);
      result->dataSize = serverFrameSize;
  }

  {
    /* Prepare the authentication response, according to the server's
     * published settings. */
    const uint_t frameSize = FRAME_HDR_SIZE +
                               FRAME_AUTH_RSP_FIXED_SIZE +
                               strlen (database) + 1 +
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

    strcpy (pAuthData, database);

    if (result->cipher == FRAME_ENCTYPE_PLAIN)
      strcpy (pAuthData + strlen (database) + 1, password);

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
        whs_close (result->socket);

      mem_free (result);
    }

  return status;
}

void
WClose (WH_CONNECTION hnd)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if (hnd_ == NULL)
    return ;

  if (hnd_->socket != INVALID_SOCKET)
    {
      set_data_size (hnd_, 0);
      send_command (hnd_, CMD_CLOSE_CONN);
      whs_close (hnd_->socket);
    }

  if (hnd_->data != NULL)
    mem_free (hnd_->data);

  mem_free (hnd_);
}

uint_t
WPingServer (const WH_CONNECTION hnd)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t   cs   = WCS_OK;
  uint16_t type = CMD_INVALID_RSP;

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
      || (load_le_int32 (data (hnd_)) != WCS_OK))

    {
      cs = WCS_INVALID_FRAME;
    }

exit_ping_server:
  return cs;
};

static uint_t
list_globals (struct INTERNAL_HANDLER* const hnd,
              const uint_t                   hint,
              uint_t* const                  outCount)
{
  uint_t   cs         = WCS_OK;
  uint8_t* data_      = NULL;
  uint_t   dataOffset = 0;
  uint16_t type       = CMD_INVALID_RSP;

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
  if ((cs = load_le_int32 (data_)) != WCS_OK)
    goto list_globals_err;

  dataOffset  = sizeof (uint32_t);
  *outCount   = load_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (uint32_t);

  hnd->cmdInternal[LIST_GLBINDEX] = load_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (uint32_t);

  hnd->cmdInternal[LIST_GLBOFF]    = dataOffset;
  hnd->cmdInternal[LIST_GLBSCOUNT] = *outCount;

  assert (hnd->lastCmdRespReceived == type);

  return WCS_OK;

list_globals_err:

  assert (cs != WCS_OK);

  hnd->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}

uint_t
WListGlobals (const WH_CONNECTION hnd,
              uint_t* const       outCount)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if ((hnd == NULL) || (outCount == NULL))
    return WCS_INVALID_ARGS;

  else if (hnd_->userId != 0)
    return WCS_OP_NOTPERMITED;

  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  return list_globals (hnd_, 0, outCount);
}

uint_t
WListGlobalsFetch (const WH_CONNECTION hnd,
                   const char** const  outpName)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t  cs = WCS_OK;

  uint_t  dataOffset;
  uint_t  glbIndex, glbsCount;

  if ((hnd_ == NULL)
      || (outpName == NULL)
      || (hnd_->lastCmdRespReceived != CMD_LIST_GLOBALS_RSP))
    {
      return WCS_INVALID_ARGS;
    }
  else if ((cs = load_le_int32 (data (hnd_))) != WCS_OK)
    goto list_global_fetch_err;

  glbsCount  = hnd_->cmdInternal[LIST_GLBSCOUNT];
  glbIndex   = hnd_->cmdInternal[LIST_GLBINDEX];
  dataOffset = hnd_->cmdInternal[LIST_GLBOFF];

  if (glbIndex >= glbsCount)
    {
      *outpName = NULL;
      return WCS_OK;
    }

  if (dataOffset >= data_size (hnd_))
    {
      /* Couldn't find the next name in this frame, send a new request
       * to the server with a new hint. */

      if ((cs = list_globals (hnd_, glbIndex, &glbsCount)) != WCS_OK)
        goto list_global_fetch_err;

      assert (glbIndex < glbsCount);

      dataOffset = 3 * sizeof (uint32_t);

      assert (glbsCount  == hnd_->cmdInternal[LIST_GLBSCOUNT]);
      assert (glbIndex   == hnd_->cmdInternal[LIST_GLBINDEX]);
      assert (dataOffset == hnd_->cmdInternal[LIST_GLBOFF]);
    }


  *outpName   = (char*)data (hnd_) + dataOffset;
  dataOffset += strlen (*outpName) + 1;

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
list_procedures (struct INTERNAL_HANDLER* const hnd,
                 const uint_t             hint,
                 uint_t* const            outCount)
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
  if ((cs = load_le_int32 (data_)) != WCS_OK)
    goto list_procedures_err;

  else if (data_size (hnd) < 3 * sizeof (uint32_t))
    {
      cs = WCS_INVALID_FRAME;
      goto list_procedures_err;
    }

  dataOffset  = sizeof (uint32_t);
  *outCount   = load_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (uint32_t);

  hnd->cmdInternal[LIST_PROCSINDEX] = load_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (uint32_t);

  hnd->cmdInternal[LIST_PROCOFF]    = dataOffset;
  hnd->cmdInternal[LIST_PROCSCOUNT] = *outCount;

  assert (hnd->lastCmdRespReceived == type);

  return WCS_OK;

list_procedures_err:

  assert (cs != WCS_OK);

  hnd->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}

uint_t
WListProcedures (const WH_CONNECTION hnd, uint_t* const outpCount)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if ((hnd == NULL) || (outpCount == NULL))
    return WCS_INVALID_ARGS;

  else if (hnd_->userId != 0)
    return WCS_OP_NOTPERMITED;

  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  return list_procedures (hnd_, 0, outpCount);
}


uint_t
WListProceduresFetch (const WH_CONNECTION  hnd,
                      const char** const   outpName)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t  cs = WCS_OK;

  uint_t  dataOffset;
  uint_t  procIndex, procsCount;

  if ((hnd_ == NULL)
      || (outpName == NULL)
      || (hnd_->lastCmdRespReceived != CMD_LIST_PROCEDURE_RSP))
    {
      return WCS_INVALID_ARGS;
    }

  else if ((cs = load_le_int32 (data (hnd_))) != WCS_OK)
    goto list_procedure_fetch_err;


  procsCount = hnd_->cmdInternal[LIST_PROCSCOUNT];
  dataOffset = hnd_->cmdInternal[LIST_PROCOFF];
  procIndex  = hnd_->cmdInternal[LIST_PROCSINDEX];

  if (procIndex >= procsCount)
    {
      *outpName = NULL;
      return WCS_OK;
    }

  if (dataOffset >= data_size (hnd_))
    {
      /* Couldn't find the next name in this frame, send a new request
       * to the server with a new hint. */

      if ((cs = list_procedures (hnd_, procIndex, &procsCount)) != WCS_OK)
        goto list_procedure_fetch_err;

      assert (procIndex < procsCount);

      dataOffset = 3 * sizeof (uint32_t);

      assert (procsCount == hnd_->cmdInternal[LIST_PROCSCOUNT]);
      assert (dataOffset == hnd_->cmdInternal[LIST_PROCOFF]);
      assert (procIndex  == hnd_->cmdInternal[LIST_PROCSINDEX]);
    }

  *outpName   = (char*)data (hnd_) + dataOffset;
  dataOffset += strlen (*outpName) + 1;

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


/* Internal function used to retrieve type information about a value.
 * If the global name is NULL then it refers to the value from the connection
 * stack top. For a table value, the hint field specifies what fields should
 * cached from in this frame for next type processing. */
static uint_t
describe_value (struct INTERNAL_HANDLER* const  hnd,
                const char* const               globalName,
                const uint_t                    fieldHint,
                uint_t* const                   outRawType)
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

  data_ = data (hnd);

  if ((cs = load_le_int32 (data_)) != WCS_OK)
    goto describe_value_err;

  assert (offset == strlen ((char*)data_ + sizeof (uint32_t)) + 1 +
                    sizeof (uint32_t));

  *outRawType = load_le_int16 (data_ + offset);
  hnd->cmdInternal[DESC_RAWTYPE] = *outRawType;
  offset += sizeof (uint16_t);

  hnd->cmdInternal[DESC_FIELD_COUNT] = load_le_int16 (data_ + offset);
  offset += sizeof (uint16_t);

  hnd->cmdInternal[DESC_FIELD_HINT] = load_le_int16 (data_ + offset);
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

uint_t
WGlobalType (const WH_CONNECTION    hnd,
             const char* const      name,
             uint_t* const          outRawType)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  if ((hnd == NULL)
      || (name == NULL)
      || (strlen (name) == 0)
      || (outRawType == NULL))
    {
      return WCS_INVALID_ARGS;
    }

  else if (hnd_->userId != 0)
    return WCS_OP_NOTPERMITED;

  return describe_value (hnd_, name, 0, outRawType);
}

uint_t
WFieldsCount (const WH_CONNECTION  hnd,
              uint_t* const        outCount)
{
  struct INTERNAL_HANDLER* const hnd_   = (struct INTERNAL_HANDLER*)hnd;
  uint_t                         cs     = WCS_OK;
  uint8_t*                       data_  = NULL;
  uint16_t                       type;

  if ((hnd_ == NULL) || (outCount == NULL))
    return WCS_INVALID_ARGS;

  else if (hnd_->lastCmdRespReceived != CMD_GLOBAL_DESC_RSP)
    return WCS_INCOMPLETE_CMD;

  data_ = data (hnd);
  if ((cs = load_le_int32 (data_)) != WCS_OK)
    goto describe_value_field_cnt_err;

  type = hnd_->cmdInternal[DESC_RAWTYPE];
  if ((type & WHC_TYPE_TABLE_MASK) == 0)
    {
      *outCount = 0;
      return WCS_OK;
    }

  *outCount = hnd_->cmdInternal[DESC_FIELD_COUNT];

  assert (cs == WCS_OK);

  return WCS_OK;

describe_value_field_cnt_err:
  assert (cs != WCS_OK);

  hnd_->lastCmdRespReceived = CMD_INVALID_RSP;

  return cs;
}

uint_t
WFetchField (const WH_CONNECTION    hnd,
             const char**           outpFieldName,
             uint_t* const          outFieldType)
{
  struct INTERNAL_HANDLER* const hnd_   = (struct INTERNAL_HANDLER*)hnd;

  uint_t    cs     = WCS_OK;
  uint8_t*  data_  = NULL;
  uint_t    offset;
  uint_t    fieldHint, fieldCount;
  uint16_t  type;

  if ((hnd_ == NULL) || (outpFieldName == NULL) || (outFieldType == NULL))
    return WCS_INVALID_ARGS;

  else if (hnd_->lastCmdRespReceived != CMD_GLOBAL_DESC_RSP)
    return WCS_INCOMPLETE_CMD;

  data_ = data (hnd_);
  if ((cs = load_le_int32 (data_)) != WCS_OK)
    goto describe_value_fetch_field_err;

describe_value_fetch_field_again:

  type = hnd_->cmdInternal[DESC_RAWTYPE];
  if ((type & WHC_TYPE_TABLE_MASK) == 0)
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
      *outFieldType  = WHC_TYPE_NOTSET;
      *outpFieldName = NULL;

      hnd_->cmdInternal[DESC_FIELD_HINT] = ++fieldHint;

      return WCS_OK;
    }
  else if (offset < data_size (hnd_))
    {
      *outpFieldName  = (char*)data (hnd_) + offset;
      offset         += strlen (*outpFieldName) + 1;

      *outFieldType  = load_le_int16 (data(hnd_) + offset);
      offset        += sizeof (uint16_t);

      ++fieldHint;

      assert (offset <= 0xFFFF);
      assert (fieldHint <= 0xFFFF);

      hnd_->cmdInternal[DESC_FIELD_COUNT]  = fieldCount;
      hnd_->cmdInternal[DESC_FIELD_HINT]   = fieldHint;
      hnd_->cmdInternal[DESC_FIELD_OFFSET] = offset;
    }
  else
    {
      /* The next field isn't contained in this frame, send a new request
       * with a new field hint. */

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


uint_t
WStackValueType (const WH_CONNECTION hnd,
                 uint_t* const       outRawType)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;


  if ((hnd == NULL) || (outRawType == NULL))
    return WCS_INVALID_ARGS;

  return describe_value (hnd_, NULL, 0, outRawType);
}

static uint_t
compute_push_space_need (const uint_t               type,
                         uint_t                     fieldsCount,
                         const struct WField* const fields)
{
  uint_t result = 0;

  if (type != WHC_TYPE_TABLE_MASK)
    {
      const uint_t baseType = type & ~(WHC_TYPE_ARRAY_MASK);

      if (type & WHC_TYPE_FIELD_MASK)
        return 0;

      if ((baseType < WHC_TYPE_BOOL) || (baseType > WHC_TYPE_TEXT)
          || (fieldsCount > 0))
        {
          return 0;
        }

      return sizeof (uint16_t) + 1;
    }

  if ((fieldsCount == 0) || (fields == NULL))
    return 0;

  result = 2 * sizeof (uint16_t) + 1;
  while (fieldsCount-- > 0)
    {
      const char* const fieldName = fields[fieldsCount].name;
      const uint_t      nameLen   = strlen (fieldName) + 1;

      uint16_t fieldType = fields[fieldsCount].type;
      fieldType &= ~WHC_TYPE_ARRAY_MASK;

      if ((fieldName == NULL)
          || (nameLen <= 1)
          || ((fieldType < WHC_TYPE_BOOL) || (fieldType > WHC_TYPE_TEXT)))
        {
          return 0;
        }
      result += nameLen;
      result += sizeof (uint16_t);
    }

  return result;
}

static void
write_push_cmd (const uint_t               type,
                uint_t                     fieldsCount,
                const struct WField* const fields,
                uint8_t*                   dest)
{
  *dest++ = CMD_UPDATE_FUNC_PUSH;

  store_le_int16 (type, dest);

  if (type == WHC_TYPE_TABLE_MASK)
    {
      dest += sizeof (uint16_t);

      assert ((fieldsCount != 0) && (fields != NULL));

      store_le_int16 (fieldsCount, dest);
      dest += sizeof (uint16_t);

      while (fieldsCount-- > 0)
        {
          const uint16_t    fieldType = fields[fieldsCount].type;
          const char* const fieldName = fields[fieldsCount].name;
          const uint_t      nameLen   = strlen (fieldName) + 1;

          memcpy (dest, fieldName, nameLen);
          dest += nameLen;
          store_le_int16 (fieldType, dest);
          dest += sizeof (uint16_t);
        }
    }
}

uint_t
WPushValue (const WH_CONNECTION        hnd,
            const uint_t               type,
            const uint_t               fieldsCount,
            const struct WField* const fields)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  const uint_t spaceReq = compute_push_space_need (type, fieldsCount, fields);

  uint_t cs = WCS_OK;

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

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  if (max_data_size (hnd_) - spaceReq < data_size (hnd_))
    {
      assert (hnd_->buildingCmd == CMD_UPDATE_STACK);
      assert (data_size (hnd_) > 0);

      if ((cs = WFlush (hnd)) != WCS_OK)
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
WPopValues (const WH_CONNECTION hnd,
            const uint_t        count)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  static const uint_t  spaceReq  = sizeof (uint32_t) + 1;

  if ((hnd_ == NULL) || (count == 0))
    return WCS_INVALID_ARGS;

  else if ((hnd_->buildingCmd != CMD_UPDATE_STACK)
           && (hnd_->buildingCmd != CMD_INVALID))
    {
      return WCS_INCOMPLETE_CMD;
    }

  if (hnd_->buildingCmd == CMD_INVALID)
    set_data_size (hnd_, 0);

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  if (max_data_size(hnd_) - spaceReq  <  data_size (hnd_))
    {
      uint_t cs;

      assert (hnd_->buildingCmd == CMD_UPDATE_STACK);
      assert (data_size (hnd_) > 0);

      if ((cs = WFlush (hnd_)) != WCS_OK)
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
stack_top_basic_update (struct INTERNAL_HANDLER* const hnd,
                        const uint_t                   type,
                        const char* const              value)
{
  const uint_t spaceNeed = sizeof (uint8_t) +
                             sizeof (uint16_t) +
                             strlen (value) + 1;
  uint_t cs = WCS_OK;

  assert ((WHC_TYPE_BOOL <= type) && (type < WHC_TYPE_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = WFlush (hnd)) != WCS_OK)
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
stack_top_field_basic_update (struct INTERNAL_HANDLER* const hnd,
                              const uint_t                   type,
                              const char* const              fieldNane,
                              const WHT_ROW_INDEX            row,
                              const char* const              value)
{
  const uint_t spaceNeed = sizeof (uint8_t) +
                             sizeof (uint16_t) +
                             strlen (fieldNane) + 1 +
                             sizeof (uint64_t) +
                             strlen (value) + 1;
  uint_t cs = WCS_OK;

  assert (hnd != NULL);
  assert (strlen (fieldNane) > 0);
  assert ((WHC_TYPE_BOOL <= type) && (type < WHC_TYPE_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = WFlush (hnd)) != WCS_OK)
        return cs;
    }

  {
    uint8_t*     data_    = data (hnd);
    const uint_t currSize = data_size (hnd);

    data_    += currSize;
    *data_++  = CMD_UPDATE_FUNC_CHTOP;

    store_le_int16 (type | WHC_TYPE_FIELD_MASK, data_);
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
                       const WHT_INDEX          textOff,
                       const char* const        value)
{
  const uint_t fixedSize = sizeof (uint8_t) +
                             sizeof (uint16_t) +
                             sizeof (uint64_t);
  const uint_t cmdSize   = fixedSize + strlen (value) + 1;

  uint8_t* data_;
  uint_t   currSize;
  uint_t   cs = WCS_OK;

  assert (hnd != NULL);
  assert (strlen (value) > 0);

  if (wh_utf8_strlen ((const uint8_t*)value) < 0)
    return WCS_INVALID_ARGS;

  else if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
  {
      const uint16_t prevType = load_le_int16 (++data_);
      if (prevType == WHC_TYPE_TEXT)
        {
          uint64_t      prevOffset;
          uint_t        prevRawlen;
          int           prevLen;


          data_ += sizeof (uint16_t);
          prevOffset = load_le_int64 (data_);
          data_ += sizeof (uint64_t);

          prevLen = wh_utf8_strlen (data_);

          assert (prevLen >= 0);

          prevRawlen = strlen ((const char*)data_);
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

  store_le_int16 (WHC_TYPE_TEXT, data_);
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
stack_top_field_text_update (struct INTERNAL_HANDLER* const hnd,
                             const char* const              fieldName,
                             const WHT_ROW_INDEX            row,
                             const WHT_INDEX                textOff,
                             const char* const              value)
{
  const uint_t fixedSize  = sizeof (uint8_t) +
                              sizeof (uint16_t) +
                              strlen (fieldName) + 1 +
                              sizeof (uint64_t) +
                              sizeof (uint64_t);
  const uint_t cmdSize    = fixedSize + strlen (value) + 1;

  uint8_t* data_;
  uint_t   currSize;
  uint_t   cs         = WCS_OK;

  assert (hnd != NULL);
  assert (strlen (value) > 0);
  assert (strlen (fieldName) > 0);

  if (wh_utf8_strlen ((const uint8_t*)value) < 0)
    return WCS_INVALID_ARGS;

  else if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
    {
      const uint16_t prevType = load_le_int16 (++data_);
      if (prevType == (WHC_TYPE_TEXT | WHC_TYPE_FIELD_MASK))
        {
          const char* prevFieldName;
          uint64_t    prevRow, prevOffset;
          uint_t      prevRawlen;
          int         prevLen;

          data_ += sizeof (uint16_t);

          prevFieldName = (const char*)data_;
          data_ += strlen (prevFieldName) + 1;

          prevRow = load_le_int64 (data_);
          data_ += sizeof (uint64_t);

          prevOffset = load_le_int64 (data_);
          data_ += sizeof (uint64_t);

          prevLen = wh_utf8_strlen (data_);

          assert (prevLen >= 0);

          prevRawlen = strlen ((const char*)data_);
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

  store_le_int16 (WHC_TYPE_TEXT | WHC_TYPE_FIELD_MASK, data_),
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
stack_top_array_basic_update (struct INTERNAL_HANDLER* const hnd,
                              const uint_t                   type,
                              const WHT_INDEX                arrayOff,
                              const char* const              value)
{
  uint_t   currSize;
  uint8_t* data_;

  uint_t cs       = WCS_OK;
  uint_t countOff = sizeof (uint8_t) + sizeof (uint16_t);

  const uint_t fixedSize  = countOff + sizeof (uint16_t) + sizeof (uint64_t);
  const uint_t cmdSize    = fixedSize + strlen (value) + 1;

  assert (hnd != NULL);
  assert ((WHC_TYPE_BOOL <= type) && (type < WHC_TYPE_TEXT));
  assert (strlen (value) > 0);

  if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_ = data (hnd);
  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
  {
      uint16_t prevType;
      data_ += hnd->cmdInternal[LAST_UPDATE_OFF];

      prevType = load_le_int16 (++data_);
      if ((type | WHC_TYPE_ARRAY_MASK) == prevType)
        {
          uint64_t prevOffset;
          uint16_t prevCount;

          data_ += sizeof (uint16_t);

          prevCount = load_le_int16 (data_);

          assert (prevCount > 0);

          data_ += sizeof (uint16_t);

          prevOffset = load_le_int64 (data_);
          data_ += sizeof (uint64_t);

          if ((prevCount < 0xFFFF) &&
              (prevOffset + prevCount) == arrayOff)
            {
              uint_t newDataSize;

              data_ = data (hnd) +
                        hnd->cmdInternal[LAST_UPDATE_OFF] +
                        countOff;
              store_le_int16 (prevCount + 1, data_);

              newDataSize = data_size (hnd) + strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));

              data_ = data (hnd) + data_size (hnd);
              strcpy ((char*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  currSize = data_size (hnd);

  data_    = data (hnd) + currSize;
  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (type | WHC_TYPE_ARRAY_MASK, data_);
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
stack_top_field_array_basic_update (struct INTERNAL_HANDLER* const hnd,
                                    const uint_t                   type,
                                    const char*                    fieldName,
                                    const WHT_ROW_INDEX            row,
                                    const WHT_INDEX                arrayOff,
                                    const char* const              value)
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
  assert ((WHC_TYPE_BOOL <= type) && (type < WHC_TYPE_TEXT));
  assert (strlen (fieldName) > 0);
  assert (strlen (value) > 0);

  if (cmdSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  /* Try to cache this operation if it's enough space in frame, other way
   * flush what's there. */
  else if (max_data_size (hnd) - cmdSize < data_size (hnd))
    {
      if ((cs = WFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (data_[hnd->cmdInternal[LAST_UPDATE_OFF]] == CMD_UPDATE_FUNC_CHTOP)
  {
      const uint16_t prevType = load_le_int16 (++data_);
      if ((type | WHC_TYPE_ARRAY_MASK | WHC_TYPE_FIELD_MASK) == prevType)
        {
          const char*    prevFielName;
          uint64_t       prevRow, prevOffset;
          uint16_t       prevCount;

          data_ += sizeof (uint16_t);

          prevFielName = (const char*)data_;
          data_ += strlen (prevFielName) + 1;

          prevRow = load_le_int64 (data_);
          data_ += sizeof (uint64_t);

          prevCount = load_le_int16 (data_);

          assert (prevCount > 0);

          data_ += sizeof (uint16_t);

          prevOffset = load_le_int64 (data_);
          data_ += sizeof (uint64_t);

          if (((prevOffset + prevCount) == arrayOff)
              && (prevRow == row)
              && (strcmp (prevFielName, fieldName) == 0))
            {
              uint_t newDataSize = data_size (hnd) + strlen (value) + 1;

              data_ = data (hnd) +
                        hnd->cmdInternal[LAST_UPDATE_OFF] +
                        countOff;
              store_le_int16 (prevCount + 1, data_);

              newDataSize = data_size (hnd) + strlen (value) + 1;

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

  store_le_int16 (type | WHC_TYPE_ARRAY_MASK | WHC_TYPE_FIELD_MASK, data_);
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
WUpdateValue (const WH_CONNECTION    hnd,
              const uint_t           type,
              const char* const      fieldName,
              const WHT_ROW_INDEX    row,
              const WHT_INDEX        arrayOff,
              const WHT_INDEX        textOff,
              const char* const      value)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint32_t cs = WCS_OK;

  if ((hnd_ == NULL)
      || (value == NULL)
      || (value[0] == 0)
      || (type < WHC_TYPE_BOOL) || (type > WHC_TYPE_TEXT))
    {
      return WCS_INVALID_ARGS;
    }

  /* Check if the current frame keep is caching data for other type of command.
   * That one has to be flushed prior this to proceed. */
  else if ((hnd_->buildingCmd != CMD_INVALID)
           && (hnd_->buildingCmd != CMD_UPDATE_STACK))
    {
      return WCS_INCOMPLETE_CMD;
    }

  if (hnd_->buildingCmd == CMD_INVALID)
    set_data_size (hnd_, 0);

  /* Based on the values of the supplied parameter, try to determine the if
   * the stack value's type is a table, field, array, etc. and based on these
   * findings call the proper help function. */
  if ((row == WIGNORE_ROW) && (arrayOff == WIGNORE_OFF))
    {
      if (fieldName != WIGNORE_FIELD)
        return WCS_TYPE_MISMATCH;

      else if (type != WHC_TYPE_TEXT)
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

      else if (type == WHC_TYPE_TEXT)
        return WCS_OP_NOTSUPP;

      else
        {
          if (textOff != WIGNORE_OFF)
            return WCS_INVALID_TEXT_OFF;

          cs = stack_top_array_basic_update (hnd_,type, arrayOff, value);
        }
    }
  else if (row != WIGNORE_ROW)
    {
      if (fieldName == WIGNORE_FIELD)
        return WCS_INVALID_FIELD;

      if (arrayOff == WIGNORE_OFF)
        {
          if (type == WHC_TYPE_TEXT)
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
          if (type == WHC_TYPE_TEXT)
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
WFlush (const WH_CONNECTION hnd)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint32_t  cs  = WCS_OK;
  uint16_t  type;

  if (hnd_ == NULL)
    return WCS_INVALID_ARGS;

  /* Currently WFlush() knows that only stack update commands are cached.
   * If the current frame holds a different command type, return the proper
   * error status. */
  else if (hnd_->buildingCmd != CMD_UPDATE_STACK)
    {
      if (hnd_->buildingCmd == CMD_INVALID)
        {
          /* It's nothing here, just ignore this call. */
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

  cs = load_le_int32 (data (hnd_));

  memset (hnd_->cmdInternal, 0, sizeof (hnd_->cmdInternal));
  set_data_size (hnd_, 0);

  return cs;
}

static uint_t
send_stack_read_req (struct INTERNAL_HANDLER* const hnd,
                     const char*                    field,
                     const uint64_t                 row,
                     const uint64_t                 arrayOff,
                     const uint64_t                 textOff)
{


  uint_t   cs         = WCS_OK;
  uint_t   neededSize = 0;
  uint16_t type       = 0;

  if (field == WIGNORE_FIELD)
    field = WANONIM_FIELD;

  neededSize = strlen (field) + 1 +
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

  cs = load_le_int32 (data (hnd));

  return cs;
}

/* Helper function to get frame offest of the next cached field value.
 * It calculates the bytes to be skipped based on the current field type. */
static uint_t
next_table_field_off (struct INTERNAL_HANDLER* const hnd,
                      uint_t                         position)
{
  const uint8_t* const data_    = data (hnd);
  const uint_t         dataSize = data_size (hnd);

  /* Position points at the beginning of the current field,
   * so retrieve the its type. */
  const uint_t nameLen = strlen ((const char*)data_ + position) + 1;
  uint16_t     type    = load_le_int16 (data_ + position + nameLen);

  position += nameLen + sizeof (uint16_t);

  assert (position < dataSize);

  /* Do the offset calculations based on the type of the current field. */
  if (type & WHC_TYPE_ARRAY_MASK)
    {
      const uint_t arraySize = load_le_int64 (data_ + position);
      position += sizeof (uint64_t);

      if (arraySize != 0)
        {
          uint64_t arrayOff = load_le_int64 (data_ + position);

          assert (arrayOff < arraySize);

          position += sizeof (uint64_t);

          while (arrayOff < arraySize)
            {
              if ((type & 0xFF) != WHC_TYPE_TEXT)
                {
                  assert (strlen ((const char*)data_ + position) > 0);

                  position += strlen ((const char*)data_ + position) + 1;
                }
              else
                {
                  assert (load_le_int32 (data_ + position) > 0);

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
      if (type == WHC_TYPE_TEXT)
        {
          const uint64_t charsCount = load_le_int64 (data_ + position);
          position += sizeof (uint64_t);

          if (charsCount > 0)
            {
              assert (load_le_int64 (data_ + position) < charsCount);

              position += sizeof (uint64_t);
              position += strlen ((const char*)data_ + position) + 1;
            }
        }
      else
        {
          assert ((type >= WHC_TYPE_BOOL) && (type < WHC_TYPE_TEXT));

          position += strlen ((const char*)data_ + position) + 1;
        }
    }

  /* If we got at the end of the frame, then we just miss hit the cache,
   * return 0 for the caller to reissue a server request. */
  if (position >= dataSize)
    {
      assert (position == dataSize);

      return 0;
    }

  return position;
}

/* Help function to search a specific field. */
static uint_t
get_table_field_off (struct INTERNAL_HANDLER* const hnd,
                     const char* const              field,
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

  return 0; /* The requested field is not cached in this frame. */
}

/* For cached table or field values, this should return the frame offset,
 * where the specified row values begins. It does that  based on the type
 * of the currently cached values. */
static uint_t
get_row_offset (struct INTERNAL_HANDLER* const hnd,
                const uint64_t                 row)
{
  const uint8_t* const data_    = data (hnd);
  const uint_t         dataSize = data_size (hnd);

  uint16_t type       = load_le_int16 (data_ + sizeof (uint32_t));
  uint_t   rowOff     = sizeof (uint32_t) + sizeof (uint16_t);
  uint64_t currentRow = 0;

  assert (hnd->lastCmdRespReceived == CMD_READ_STACK_RSP);
  assert (dataSize > (uint32_t) + sizeof (uint16_t));
  assert (load_le_int32 (data_) == WCS_OK);

  if (row >= load_le_int64 (data_ + rowOff))
    return 0;

  rowOff     += sizeof (uint64_t);
  currentRow  = load_le_int64 (data_ + rowOff);
  rowOff     += sizeof (uint64_t);

  if (currentRow > row)
    return 0; /* The requested row is not cached in this frame. */

  if (type == WHC_TYPE_TABLE_MASK)
    {
      const uint_t fieldsCount = load_le_int16 (data_ + rowOff);
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

  /* The cached values is of field type. So determine the type values it
   * holds, and try to find the row offset. */
  assert (type & WHC_TYPE_FIELD_MASK);

  type &= ~WHC_TYPE_FIELD_MASK;
  if (type & WHC_TYPE_ARRAY_MASK)
    {
      type &= ~WHC_TYPE_ARRAY_MASK;

      assert ((WHC_TYPE_BOOL <= type) && (type <= WHC_TYPE_TEXT));

      while ((currentRow < row)
             && (rowOff < dataSize))
        {
          const uint_t arrayCount = load_le_int64 (data_ + rowOff);
          rowOff += sizeof (uint64_t);

          assert (rowOff <= dataSize);

          if (arrayCount > 0)
            {
              uint_t arrayOff = load_le_int64 (data_ + rowOff);
              rowOff += sizeof (uint64_t);

              while ((arrayOff < arrayCount)
                     && (rowOff < dataSize))
                {
                  if (type == WHC_TYPE_TEXT)
                    {
                      assert (load_le_int64 (data_ + rowOff) > 0);

                      rowOff += sizeof (uint64_t);

                      assert (load_le_int64 (data_ + rowOff) > 0);

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

          return 0; /*  No row values are cached in here. */
        }

      return rowOff;
    }

  assert ((WHC_TYPE_BOOL <= type) && (type <= WHC_TYPE_TEXT));

  while ((currentRow < row)
         && (rowOff < dataSize))
    {
      if (type == WHC_TYPE_TEXT)
        {
          const uint_t charsCount = load_le_int64 (data_ + sizeof (uint64_t));
          rowOff += sizeof (uint64_t);

          assert (rowOff <= dataSize);

          if (charsCount > 0)
            {
              const uint_t textOff = load_le_int64 (data_ + sizeof (uint64_t));
              rowOff += sizeof (uint64_t);

              (void)textOff;
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

/* Find the offset of where the specified array element resides, given the
 * start of the array. */
static uint_t
get_array_el_off (struct INTERNAL_HANDLER* const hnd,
                  const uint_t                   type,
                  const uint64_t                 arrayOff,
                  uint_t                         fromPos,
                  uint64_t* const                pArraySize)
{
  const uint8_t* data_    = data (hnd);
  const uint_t   dataSize = data_size (hnd);
  const uint64_t elCount  = load_le_int64 (data_ + fromPos);
  uint64_t       currOff;

  assert (sizeof (uint32_t) <= fromPos);

  fromPos += sizeof (uint64_t);

  assert (fromPos <= dataSize);

  *pArraySize = elCount;
  if ((elCount == 0) || (*pArraySize <= arrayOff))
    return INVALID_OFF;

  assert ((WHC_TYPE_BOOL <= type) && (type <= WHC_TYPE_TEXT));
  assert (fromPos < dataSize);

  currOff = load_le_int64 (data_ + fromPos);
  if (arrayOff < currOff)
    return 0;

  fromPos += sizeof (uint64_t);
  while ((currOff < arrayOff)
         && (fromPos < dataSize))
    {
      assert (fromPos < dataSize);

      if (type == WHC_TYPE_TEXT)
        {
          assert (load_le_int64 (data_ + fromPos) > 0);

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
  const uint64_t elCount  = load_le_int64 (data_ + fromPos);
  uint64_t       currOff;

  *pCharsCount = elCount;
  if ((elCount == 0) || (elCount <= textOff))
    return INVALID_OFF;

  fromPos += sizeof (uint64_t);

  assert (fromPos < dataSize);

  currOff = load_le_int64 (data_ + fromPos);
  if (textOff < currOff)
    return 0;

  fromPos += sizeof (uint64_t);

  assert (fromPos < dataSize);

  while ((currOff < textOff)
         && (fromPos < dataSize))
    {
      uint32_t ch     = 0;
      uint_t   chSize = wh_load_utf8_cp (data_ +fromPos, &ch);

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

uint_t
WValueRowsCount (const WH_CONNECTION        hnd,
                 ullong_t* const            outCount)
{
  struct INTERNAL_HANDLER* const hnd_  = (struct INTERNAL_HANDLER*)hnd;
  uint_t                         cs    = WCS_OK;
  uint_t                         type  = 0;
  const  uint8_t*                data_;

  if ((hnd == NULL) || (outCount == NULL))
    return WCS_INVALID_ARGS;

  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  if ((hnd_->lastCmdRespReceived != CMD_READ_STACK_RSP)
      || (load_le_int32 (data (hnd)) != WCS_OK))
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
  assert (load_le_int32 (data_) == WCS_OK);

  type  = load_le_int16 (data_ + sizeof (uint32_t));

  assert (load_le_int32 (data_) == WCS_OK);

  if (type & WHC_TYPE_TABLE_MASK)
    {
      *outCount = load_le_int64 (data_ +
                                   sizeof (uint32_t) +
                                   sizeof (uint16_t));
    }
  else if (type & WHC_TYPE_FIELD_MASK)
    {
      uint_t offset = sizeof (uint32_t) + sizeof (uint16_t);
      offset += strlen ((const char*)data_ + offset) + 1;

      assert (offset < data_size (hnd_));

      *outCount = load_le_int64 (data_ + offset);
    }
  else
    cs = WCS_TYPE_MISMATCH;

  return cs;
}

static int
get_stack_value (struct INTERNAL_HANDLER* const hnd,
                 const char* const              field,
                 const WHT_ROW_INDEX            row,
                 const WHT_INDEX                arrayOff,
                 const WHT_INDEX                textOff,
                 uint64_t* const                outCount,
                 uint16_t* const                outType,
                 const char** const             outpValue)
{
  uint_t          cs       = WCS_OK;
  bool_t          tryAgain = TRUE;
  const  uint8_t* data_;
  uint_t          dataSize, dataOffset;

  assert (outType  != NULL);
  assert (outCount != NULL);

  if ((hnd == NULL) || (outpValue == NULL))
    return WCS_INVALID_ARGS;

  else if (hnd->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  if ((hnd->lastCmdRespReceived != CMD_READ_STACK_RSP)
      || (load_le_int32 (data (hnd)) != WCS_OK))
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

  *outType    = load_le_int16 (data_ + dataOffset);
  dataOffset += sizeof (uint16_t);

  if (*outType == WHC_TYPE_TABLE_MASK)
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

      *outType    = load_le_int16 (data_ + dataOffset);
      dataOffset += sizeof (uint16_t);

      assert (dataOffset < dataSize);
    }
  else if (*outType & WHC_TYPE_FIELD_MASK)
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

      *outType &= ~WHC_TYPE_FIELD_MASK;
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

  if (*outType & WHC_TYPE_ARRAY_MASK)
    {
      dataOffset = get_array_el_off (hnd,
                                     *outType & 0xFF,
                                     arrayOff,
                                     dataOffset,
                                     outCount);

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
      *outType &= ~WHC_TYPE_ARRAY_MASK;
    }
  else
    {
      if (arrayOff != WIGNORE_OFF)
        {
          cs = WCS_TYPE_MISMATCH;
          goto exit_get_stack_entry;
        }
    }

  assert ((WHC_TYPE_BOOL <= *outType) && (*outType <= WHC_TYPE_TEXT));

  if (*outType == WHC_TYPE_TEXT)
    {
      dataOffset = text_el_off (hnd, textOff, dataOffset, outCount);

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

      *outType  &= ~WHC_TYPE_ARRAY_MASK;
      *outpValue = (const char*)data_ + dataOffset;
    }
  else
    {
      if (textOff != WIGNORE_OFF)
        {
          cs = WCS_TYPE_MISMATCH;
          goto exit_get_stack_entry;
        }

      assert (dataOffset < dataSize);

      *outpValue = (const char*)data_ + dataOffset;
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

uint_t
WValueArraySize (const WH_CONNECTION  hnd,
                 const char*          field,
                 const WHT_ROW_INDEX  row,
                 ullong_t* const      outCount)
{
  uint64_t      count = 0;
  uint16_t      type  = 0;
  const char*   value = NULL;
  uint_t        cs;

  if (outCount == NULL)
    return WCS_INVALID_ARGS;

  cs = get_stack_value (hnd,
                        field,
                        row,
                        0,
                        WIGNORE_OFF,
                        &count,
                        &type,
                        &value);

  if ((cs != WCS_OK) && (cs != WCS_INVALID_ARRAY_OFF))
    return cs;


  *outCount = count;
  return WCS_OK;
}

uint_t
WValueTextLength (const WH_CONNECTION           hnd,
                          const char*           field,
                          const WHT_ROW_INDEX   row,
                          const WHT_ROW_INDEX   arrayOff,
                          ullong_t* const       outCount)
{
  uint_t        cs;
  uint64_t      count = 0;
  uint16_t      type  = 0;
  const char*   value = NULL;


  if (outCount == NULL)
    return WCS_INVALID_ARGS;

  cs = get_stack_value (hnd,
                        field,
                        row,
                        arrayOff,
                        0,
                        &count,
                        &type,
                        &value);

  if ((cs != WCS_OK) && (cs != WCS_INVALID_TEXT_OFF))
    return cs;

  *outCount = count;
  return WCS_OK;
}

uint_t
WValueEntry (const WH_CONNECTION        hnd,
             const char* const          field,
             WHT_ROW_INDEX              row,
             const WHT_INDEX            arrayOff,
             const WHT_INDEX            textOff,
             const char** const         outpValue)
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
                               outpValue);
  if (cs != WCS_OK)
    return cs;

  assert (*outpValue != NULL);

  return WCS_OK;
}

static uint_t
describe_proc_parameter (struct INTERNAL_HANDLER* const hnd,
                         const char* const              procedure,
                         const uint_t                   param)
{
  const uint_t nameLen = strlen (procedure) + 1;

  uint8_t* data_ = NULL;
  uint_t   cs    = WCS_OK;
  uint16_t type  = 0;

  if ((nameLen + sizeof (uint16_t) > max_data_size (hnd))
      || (param > 0xFFFF))
    {
      cs = WCS_LARGE_ARGS;
      goto describe_proc_parameter_err;
    }

  set_data_size (hnd, nameLen + sizeof (uint32_t));
  data_ = data (hnd);
  store_le_int16 (param, data_);
  strcpy ((char*)data_ + 2 * sizeof (uint16_t), procedure);

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
  if ((cs = load_le_int32 (data_)) != WCS_OK)
    goto describe_proc_parameter_err;

  else if ((strcmp ((char*)data_ + sizeof (uint32_t), procedure) != 0)
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

uint_t
WProcParamsCount (const WH_CONNECTION   hnd,
                  const char* const     procedure,
                  uint_t* const         outCount)
{
  struct INTERNAL_HANDLER* hnd_    = (struct INTERNAL_HANDLER*)hnd;
  const uint8_t*           data_   = NULL;
  uint_t                   cs      = WCS_OK;

  if ((hnd == NULL)
      || (procedure == NULL)
      || (strlen (procedure) == 0)
      || (outCount == NULL))
    {
      return WCS_INVALID_ARGS;
    }
  else if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);

      assert (load_le_int32 (data_) == WCS_OK);

      if (strcmp (procedure, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          if ((cs = describe_proc_parameter (hnd, procedure, 0)) != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procedure, 0)) != WCS_OK)
    return cs;

  data_ = data (hnd_);

  assert (cs == WCS_OK);
  assert (load_le_int32 (data_) == WCS_OK);

  *outCount = load_le_int16 (data_ +
                               sizeof (uint32_t) +
                               strlen (procedure) + 1);

  return WCS_OK;
}

/* Calculate the offset where a procedure parameter's description begins.
 * Returns an error if is not cached in this frame. */
static uint_t
get_paratmeter_offset (struct INTERNAL_HANDLER* const       hnd,
                       const uint_t                         param,
                       uint16_t* const                      outOff)
{
  const uint8_t* data_   = data (hnd);
  const uint_t   nameLen = strlen ((char*)data_ + sizeof (uint32_t)) + 1;

  uint16_t count = 0;
  uint16_t hint  = 0;
  uint_t   it;

  assert (data_size (hnd) > sizeof (uint32_t) + sizeof (uint16_t));
  assert (hnd->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP);
  assert (load_le_int32 (data (hnd)) == WCS_OK);

  *outOff  = sizeof (uint32_t);
  *outOff += nameLen;

  count = load_le_int16 (data_ + *outOff); *outOff += sizeof (uint16_t);
  hint  = load_le_int16 (data_ + *outOff); *outOff += sizeof (uint16_t);

  if ((param < hint) || (count < param))
    return WCS_INVALID_ARGS;

  for (it = hint; (it < param) && (*outOff < data_size (hnd)); ++it)
    {
      const uint16_t type = load_le_int16 (data_ + *outOff);
      *outOff += sizeof (uint16_t);
      if (type & WHC_TYPE_TABLE_MASK)
        {
          uint_t field;

          const uint16_t fieldsCount = load_le_int16 (data_ + *outOff);

          *outOff += sizeof (uint16_t);

          for (field = 0;
               (field < fieldsCount) && (*outOff < data_size (hnd));
               ++field)
            {
              *outOff += strlen ((char*)data_ + *outOff) + 1;
              *outOff += sizeof (uint16_t);
            }
        }
      else
        *outOff += sizeof (uint16_t);
    }

  if ((*outOff >= data_size (hnd)) || (hint != param))
    return WCS_INVALID_ARGS;

  return WCS_OK;
}

uint_t
WProcParamType (const WH_CONNECTION     hnd,
                const char* const       procedure,
                const uint_t            param,
                uint_t* const           outRawType)
{
  struct INTERNAL_HANDLER* const hnd_    = (struct INTERNAL_HANDLER*)hnd;

  const uint8_t*  data_    = NULL;
  uint_t          cs       = WCS_OK;
  bool_t          tryAgain = TRUE;
  uint16_t        offset   = 0;

  if ((hnd == NULL)
      || (procedure == NULL)
      || (strlen (procedure) == 0)
      || (outRawType == NULL))
    {
      return WCS_INVALID_ARGS;
    }
  else if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);

      assert (load_le_int32 (data_) == WCS_OK);

      if (strcmp (procedure, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          tryAgain = FALSE;
          cs = describe_proc_parameter (hnd, procedure, param);
          if (cs != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procedure, param)) != WCS_OK)
    {
      tryAgain = FALSE;
      return cs;
    }

proc_parameter_try_again:

  data_ = data (hnd_);

  assert (cs == WCS_OK);
  assert (load_le_int32 (data_) == WCS_OK);

  if ((cs = get_paratmeter_offset (hnd_, param, &offset)) != WCS_OK)
    goto proc_parameter_err;

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));

  *outRawType = load_le_int16 (data_ + offset);

  assert (cs == WCS_OK);
  return WCS_OK;

proc_parameter_err:

  assert (cs != WCS_OK);

  if (tryAgain)
    {
      /* Maybe the parameter asked for is not cached. As the server,
       * this with the right hints. */
      tryAgain = FALSE;

      cs = describe_proc_parameter (hnd, procedure, param);
      if (cs == WCS_OK)
        goto proc_parameter_try_again;
    }

  return cs;
}

uint_t
WProcParamFieldCount (const WH_CONNECTION       hnd,
                      const char* const         procedure,
                      const uint_t              param,
                      uint_t* const             outCount)
{
  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  const uint8_t* data_    = NULL;
  uint_t         cs       = WCS_OK;
  uint16_t       offset   = 0;
  bool_t         tryAgain = TRUE;

  if ((hnd == NULL)
      || (procedure == NULL)
      || (strlen (procedure) == 0)
      || (outCount == NULL))
    {
      return WCS_INVALID_ARGS;
    }

  if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);

      assert (load_le_int32 (data_) == WCS_OK);

      if (strcmp (procedure, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          cs = describe_proc_parameter (hnd, procedure, param);
          if (cs != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procedure, param))
             != WCS_OK)
    {
      return cs;
    }

proc_parameter_field_count_try_again:

  data_ = data (hnd_);

  assert (cs == WCS_OK);
  assert (load_le_int32 (data_) == WCS_OK);

  if ((cs = get_paratmeter_offset (hnd_, param, &offset)) != WCS_OK)
    goto proc_parameter_field_count_err;

  tryAgain = FALSE;

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));

  offset += sizeof (uint16_t);
  *outCount = load_le_int16 (data_ + offset);

  assert (cs == WCS_OK);

  return WCS_OK;

proc_parameter_field_count_err:

  assert (cs != WCS_OK);

  if (tryAgain)
    {
      /* Maybe the parameter info wasn't store in this cache frame.
       * So let's ask the server for it. */
      tryAgain = FALSE;

      cs = describe_proc_parameter (hnd, procedure, param);
      if (cs == WCS_OK)
        goto proc_parameter_field_count_try_again;
    }

  return cs;
}

uint_t
WProcParamField (const WH_CONNECTION   hnd,
                 const char* const     procedure,
                 const uint_t          param,
                 const uint_t          field,
                 const char**          outpFieldName,
                 uint_t* const         outFieldType)
{

  struct INTERNAL_HANDLER* const hnd_ = (struct INTERNAL_HANDLER*)hnd;

  const uint8_t* data_    = NULL;
  uint_t         cs       = WCS_OK;
  bool_t         tryAgain = TRUE;

  uint16_t  iterator;
  uint16_t  fieldsCount;
  uint16_t  offset;

  if ((hnd == NULL)
      || (procedure == NULL)
      || (outpFieldName == NULL)
      || (outFieldType == NULL)
      || (strlen (procedure) == 0))
    {
      return WCS_INVALID_ARGS;
    }
  else if (hnd_->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP)
    {
      data_ = data (hnd_);
      assert (load_le_int32 (data_) == WCS_OK);

      if (strcmp (procedure, (const char*)data_ + sizeof (uint32_t)) != 0)
        {
          cs = describe_proc_parameter (hnd, procedure, param);
          if (cs != WCS_OK)
            return cs;
        }
    }
  else if ((cs = describe_proc_parameter (hnd, procedure, param)) != WCS_OK)
    return cs;

proc_parameter_field_try_again:

  data_ = data (hnd_);

  assert (cs == WCS_OK);
  assert (load_le_int32 (data_) == WCS_OK);

  if ((cs = get_paratmeter_offset (hnd_, param, &offset)) != WCS_OK)
    goto proc_parameter_field_err;

  tryAgain = FALSE;

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));

  offset      += sizeof (uint16_t);
  fieldsCount  = load_le_int16 (data_ + offset);

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

  *outpFieldName  = (char*)data_ + offset;
  offset         += strlen (*outpFieldName) + 1;
  *outFieldType   = load_le_int16 (data_ + offset);

  assert (offset <= data_size (hnd_) - sizeof (uint16_t));
  assert (cs == WCS_OK);

  return WCS_OK;

proc_parameter_field_err:

  assert (cs != WCS_OK);

  if (tryAgain)
    {
      /* Might be the procedure parameter isn't cached in this frame.
       * Make a new request, */
      tryAgain = FALSE;

      cs = describe_proc_parameter (hnd, procedure, param);
      if (cs == WCS_OK)
        goto proc_parameter_field_try_again;
    }

  return cs;
}

uint_t
WExecuteProcedure (const WH_CONNECTION     hnd,
                   const char* const       procedure)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  uint_t   cs    = WCS_OK;
  uint16_t type  = 0;

  if ((hnd_ == NULL)
      || (procedure == NULL)
      || (strlen (procedure) == 0)
      || (hnd_->buildingCmd != CMD_INVALID))
    {
      return WCS_INCOMPLETE_CMD;
    }

  set_data_size (hnd, strlen (procedure) + 1);
  strcpy ((char*)data (hnd), procedure);

  if ((cs = send_command (hnd, CMD_EXEC_PROC)) != WCS_OK)
    goto execute_proc_err;

  if ((cs = recieve_answer (hnd, &type)) != WCS_OK)
    goto execute_proc_err;

  else if (type != CMD_EXEC_PROC_RSP)
    {
      cs = WCS_INVALID_FRAME;
      goto execute_proc_err;
    }

  else
    cs = load_le_int32 (data (hnd_));

execute_proc_err:
  return cs;
}

