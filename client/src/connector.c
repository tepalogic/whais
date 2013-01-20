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
    return WCS_OS_ERR_BASE + status;

  return WCS_OK;
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
        return WCS_OS_ERR_BASE + status;
      else if (chunkSize == 0)
        return WCS_DROPPED;

      frameRead += chunkSize;
    }

  if (hnd->data[FRAME_TYPE_OFF] == FRAME_TYPE_TIMEOUT)
    return WCS_CONNECTION_TIMEOUT;
  else if (hnd->data[FRAME_TYPE_OFF] != FRAME_TYPE_NORMAL)
    return WCS_UNEXPECTED_FRAME;

  frameSize = from_le_int16 (&hnd->data[FRAME_SIZE_OFF]);
  if ((frameSize < FRAME_DATA_OFF)
      || (frameSize > FRAME_MAX_SIZE)
      || (hnd->data[FRAME_ENCTYPE_OFF] != hnd->encType))
    {
      return WCS_COMM_OUT_OF_SYNC;
    }


  while (frameRead < frameSize)
    {
      D_UINT chunkSize = frameSize - frameRead;
      const D_UINT32 status = wh_socket_read (hnd->socket,
                                              &hnd->data [frameRead],
                                              &chunkSize);
      if (status != WOP_OK)
        return WCS_OS_ERR_BASE + status;
      else if (chunkSize == 0)
        return WCS_DROPPED;

      frameRead += chunkSize;
    }

  if (from_le_int32 (&hnd->data[FRAME_ID_OFF]) != hnd->expectedFrameId)
    return WCS_UNEXPECTED_FRAME;

  return WCS_OK;
}

static D_UINT
send_command (struct INTERNAL_HANDLER* const hnd,
              const D_UINT16                 commandId)
{
  D_UINT8*               pData    = raw_data (hnd);
  D_UINT16               chkSum   = 0;
  D_UINT16               index    = 0;
  const D_UINT16         dataSize = data_size (hnd);
  D_UINT                 cs       = WCS_OK;

  assert (hnd->encType == FRAME_ENCTYPE_PLAIN);

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

static D_UINT
recieve_answer (struct INTERNAL_HANDLER* const hnd,
                D_UINT16* const                pRespType)
{
  D_UINT                cs        = receive_raw_frame (hnd);
  D_UINT16              chkSum    = 0;
  const D_UINT16        dataSize  = data_size (hnd);
  D_UINT16              index     = 0;

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

D_UINT
WConnect (const char* const    host,
          const char* const    port,
          const char* const    databaseName,
          const char* const    password,
          const unsigned int   userId,
          W_CONNECTOR_HND* const pHnd)
{
  struct INTERNAL_HANDLER* result      = NULL;
  const D_UINT             passwordLen = strlen (password);
  D_UINT16                 frameSize   = 0;
  D_UINT32                 status      = WCS_OK;

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
  result->encType    = FRAME_ENCTYPE_PLAIN;
  memcpy (result->encriptionKey, password, passwordLen);

  status = wh_socket_client (host, port, &result->socket);
  if (status != WOP_OK)
    {
      status += WCS_OS_ERR_BASE;
      goto fail_ret;
    }

  status = read_raw_frame (result, &frameSize);
  if (status != WCS_OK)
    goto fail_ret;

  {
      D_UINT32 frameId = from_le_int32 (&result->data[FRAME_ID_OFF]);

      if ((frameId != 0)
          || (frameSize != (FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA))
          || (result->data[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT))
        {
          status = WCS_UNEXPECTED_FRAME;
          goto fail_ret;
        }

      if ((result->data[FRAME_ENCTYPE_OFF] & result->encType) == 0)
        {
          status = WCS_ENCTYPE_NOTSUPP;
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
        status = WCS_LARGE_ARGS;
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

  assert (status == WCS_OK);

  *pHnd = result;
  return WCS_OK;

fail_ret:
  assert (status != WCS_OK);

  if (result != NULL)
    {
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
  mem_free (hnd_);
}

D_UINT
WPingServer (const W_CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT16 type;

  D_UINT cs = WCS_OK;

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
      || (data_size (hnd_) != 0))
    {
      cs = WCS_INVALID_FRAME;
    }

exit_ping_server:
  return cs;
};

static D_UINT
list_globals (struct INTERNAL_HANDLER* hnd,
              const unsigned int       hint,
              unsigned int* const      pGlbsCount)
{
  D_UINT cs = WCS_OK;

  D_UINT8* data_;
  D_UINT16 dataOffset;
  D_UINT16 type;

  set_data_size (hnd, sizeof (D_UINT32));
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
  else if (data_size (hnd) < 3 * sizeof (D_UINT32))
    {
      cs = WCS_INVALID_FRAME;
      goto list_globals_err;
    }

  dataOffset  =  sizeof (D_UINT32);
  *pGlbsCount =  from_le_int32 (data_ + dataOffset);
  dataOffset  += sizeof (D_UINT32);

  hnd->cmdInternal[LIST_GLBINDEX] = from_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (D_UINT32);

  hnd->cmdInternal[LIST_GLBOFF]    = dataOffset;
  hnd->cmdInternal[LIST_GLBSCOUNT] = *pGlbsCount;

  assert (hnd->lastCmdRespReceived == type);

  return WCS_OK;

list_globals_err:

  assert (cs != WCS_OK);

  hnd->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}

D_UINT
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

D_UINT
WListGlobalsFetch (const W_CONNECTOR_HND hnd, const char** ppGlbName)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT    cs = WCS_OK;
  D_UINT32  glbIndex;
  D_UINT16  dataOffset;
  D_UINT32  glbsCount;

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
      D_UINT glbCount;

      if ((cs = list_globals (hnd_, glbIndex, &glbCount)) != WCS_OK)
        goto list_global_fetch_err;

      assert (glbIndex < glbCount);

      dataOffset = 3 * sizeof (D_UINT32);

      assert (glbsCount  == hnd_->cmdInternal[LIST_GLBSCOUNT]);
      assert (glbIndex   == hnd_->cmdInternal[LIST_GLBINDEX]);
      assert (dataOffset == hnd_->cmdInternal[LIST_GLBOFF]);
    }


  *ppGlbName = (D_CHAR*)data (hnd_) + dataOffset;
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

static D_UINT
list_procedures (struct INTERNAL_HANDLER* hnd,
                 const unsigned int       hint,
                 unsigned int* const      pProcsCount)
{
  D_UINT cs = WCS_OK;

  D_UINT8* data_;
  D_UINT16 dataOffset;
  D_UINT16 type;

  set_data_size (hnd, sizeof (D_UINT32));
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
  else if (data_size (hnd) < 3 * sizeof (D_UINT32))
    {
      cs = WCS_INVALID_FRAME;
      goto list_procedures_err;
    }

  dataOffset   =  sizeof (D_UINT32);
  *pProcsCount =  from_le_int32 (data_ + dataOffset);
  dataOffset   += sizeof (D_UINT32);

  hnd->cmdInternal[LIST_PROCSINDEX] = from_le_int32 (data_ + dataOffset);
  dataOffset += sizeof (D_UINT32);

  hnd->cmdInternal[LIST_PROCOFF]    = dataOffset;
  hnd->cmdInternal[LIST_PROCSCOUNT] = *pProcsCount;

  assert (hnd->lastCmdRespReceived == type);

  return WCS_OK;

list_procedures_err:

  assert (cs != WCS_OK);

  hnd->lastCmdRespReceived = CMD_INVALID_RSP;
  return cs;
}

D_UINT
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

D_UINT
WListProceduresFetch (const W_CONNECTOR_HND hnd,
                      const char**          ppProcName)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT    cs = WCS_OK;
  D_UINT32  procIndex;
  D_UINT16  dataOffset;
  D_UINT32  procsCount;

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
      D_UINT procsCount;

      if ((cs = list_procedures (hnd_, procIndex, &procsCount)) != WCS_OK)
        goto list_procedure_fetch_err;

      assert (procIndex < procsCount);

      dataOffset = 3 * sizeof (D_UINT32);
      assert (procsCount == hnd_->cmdInternal[LIST_PROCSCOUNT]);
      assert (dataOffset == hnd_->cmdInternal[LIST_PROCOFF]);
      assert (procIndex  == hnd_->cmdInternal[LIST_PROCSINDEX]);
    }

  *ppProcName =  (D_CHAR*)data (hnd_) + dataOffset;
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
  D_UINT                   cs     = WCS_OK;
  D_UINT16                 offset = 0;
  D_UINT16                 type;
  D_UINT8*                 data_;

  if (hnd->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;
  else if (fieldHint > 0xFFFF)
    return WCS_LARGE_ARGS;

  set_data_size (hnd, max_data_size (hnd));
  data_ = data (hnd);

  store_le_int16 (0, data_);
  offset += 2 * sizeof (D_UINT16);

  if (globalName == NULL)
    {
      strcpy ((D_CHAR*)data_ + offset, "");
      offset += sizeof (D_UINT8);
    }
  else
    {
      strcpy ((D_CHAR*)data_ + offset, globalName);
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

  assert (offset == strlen ((D_CHAR*)data_ + sizeof (D_UINT32)) + 1 +
                    sizeof (D_UINT32));

  *pRawType = from_le_int16 (data_ + offset);
  hnd->cmdInternal[DESC_RAWTYPE] = *pRawType;
  offset += sizeof (D_UINT16);

  hnd->cmdInternal[DESC_FIELD_COUNT] = from_le_int16 (data_ + offset);
  offset += sizeof (D_UINT16);

  hnd->cmdInternal[DESC_FIELD_HINT] = from_le_int16 (data_ + offset);
  offset += sizeof (D_UINT16);

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
  D_UINT                   cs     = WCS_OK;
  D_UINT8*                 data_  = NULL;
  D_UINT16                 type;

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
  D_UINT                   cs     = WCS_OK;
  D_UINT8*                 data_  = NULL;
  D_UINT16                 type;
  D_UINT16                 fieldHint;
  D_UINT16                 fieldCount;
  D_UINT16                 offset;

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
      *ppFieldName =  (D_CHAR*)data (hnd_) + offset;
      offset       += strlen (*ppFieldName) + 1;

      *pFieldType =  from_le_int16 (data(hnd_) + offset);
      offset      += sizeof (D_UINT16);

      ++fieldHint;
      assert (offset <= 0xFFFF);
      assert (fieldHint <= 0xFFFF);

      hnd_->cmdInternal[DESC_FIELD_COUNT]  = fieldCount;
      hnd_->cmdInternal[DESC_FIELD_HINT]   = fieldHint;
      hnd_->cmdInternal[DESC_FIELD_OFFSET] = offset;
    }
  else
    {
      D_UINT dummyType;
      cs = describe_value (hnd_,
                           (D_CHAR*)data (hnd_) + sizeof (D_UINT32),
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

static D_UINT
compute_push_space_need (const D_UINT                          type,
                         unsigned int                          fieldsCount,
                         const struct W_FieldDescriptor* const fields)
{
  if (type != WFT_TABLE_MASK)
    {
      if (type & WFT_FIELD_MASK)
        return 0;

      const D_UINT baseType = type & ~(WFT_ARRAY_MASK);
      if ((baseType < WFT_BOOL) || (baseType > WFT_TEXT)
          || (fieldsCount > 0))
        {
          return 0;
        }

      return sizeof (D_UINT16) + 1;
    }

  if ((fieldsCount == 0) || (fields == NULL))
    return 0;

  D_UINT result = 2 * sizeof (D_UINT16) + 1;
  while (fieldsCount-- > 0)
    {
      const D_CHAR* const fieldName = fields[fieldsCount].m_FieldName;
      const D_UINT        nameLen   = strlen (fieldName) + 1;

      D_UINT16 fieldType = fields[fieldsCount].m_FieldType;
      fieldType &= ~WFT_ARRAY_MASK;

      if ((fieldName == NULL)
          || (nameLen <= 1)
          || ((fieldType < WFT_BOOL) || (fieldType > WFT_TEXT)))
        {
          return 0;
        }
      result += nameLen;
      result += sizeof (D_UINT16);
    }

  return result;
}

static void
write_push_cmd (const D_UINT                          type,
                unsigned int                          fieldsCount,
                const struct W_FieldDescriptor* const fields,
                D_UINT8*                              dest)
{
  *dest++ = CMD_UPDATE_FUNC_PUSH;

  store_le_int16 (type, dest);

  if (type == WFT_TABLE_MASK)
    {
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
}

D_UINT
WPushStackValue (const W_CONNECTOR_HND                 hnd,
                 const unsigned int                    type,
                 const unsigned int                    fieldsCount,
                 const struct W_FieldDescriptor* const fields)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT       cs       = WCS_OK;
  const D_UINT spaceReq = compute_push_space_need (type, fieldsCount, fields);

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

      if ((cs == WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;

      assert (hnd_->buildingCmd == CMD_INVALID);
    }

  {
    D_UINT8* const data_    = data (hnd_);
    const D_UINT   dataSize = data_size (hnd_);

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

D_UINT
WPopStackValues (const W_CONNECTOR_HND hnd,
                 unsigned int        count)
{
  static const D_UINT spaceReq = sizeof (D_UINT32) + 1;

  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

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
      assert (hnd_->buildingCmd == CMD_UPDATE_STACK);
      assert (data_size (hnd_) > 0);

      D_UINT32 cs;
      if ((cs = WUpdateStackFlush (hnd_)) != WCS_OK)
        return cs;

      assert (hnd_->buildingCmd == CMD_INVALID);
    }

  {
    D_UINT8* const data_    = data (hnd_);
    const D_UINT   dataSize = data_size (hnd_);

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

D_UINT
stack_top_basic_update (struct INTERNAL_HANDLER* hnd,
                        const D_UINT             type,
                        const char* const        value)
{
  D_UINT32       cs        = WCS_OK;
  const D_UINT16 spaceNeed = sizeof (D_UINT8) +
                             sizeof (D_UINT16) +
                             strlen (value) + 1;

  assert (hnd != NULL);
  assert (strlen (value) > 0);
  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
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

    hnd->cmdInternal[UPDATE_SIZE] = currSize;
  }

  assert (cs == WCS_OK);
  return cs;
}

D_UINT
stack_top_field_basic_update (struct INTERNAL_HANDLER* hnd,
                              const D_UINT             type,
                              const D_CHAR* const      fieldNane,
                              const W_TABLE_ROW_INDEX  row,
                              const D_CHAR* const      value)
{
  D_UINT32       cs        = WCS_OK;
  const D_UINT16 spaceNeed = sizeof (D_UINT8) +
                             sizeof (D_UINT16) +
                             strlen (fieldNane) + 1 +
                             sizeof (D_UINT64) +
                             strlen (value) + 1;

  assert (hnd != NULL);
  assert (strlen (fieldNane) > 0);
  assert (strlen (value) > 0);
  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));

  if (spaceNeed > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - spaceNeed < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  {
    D_UINT8*     data_    = data (hnd);
    const D_UINT currSize = data_size (hnd);

    *data_++ = CMD_UPDATE_FUNC_CHTOP;

    store_le_int16 (type | WFT_FIELD_MASK, data_);
    data_ += sizeof (D_UINT16);

    strcpy ((D_CHAR*)data_, fieldNane);
    data_ += strlen (fieldNane) + 1;

    store_le_int64 (row, data_);
    data_ += sizeof (D_UINT64);

    strcpy ((D_CHAR*)data_, value);

    assert ((currSize + spaceNeed) <= max_data_size (hnd));
    set_data_size (hnd, currSize + spaceNeed);

    hnd->cmdInternal[UPDATE_SIZE] = currSize;
  }

  assert (cs == WCS_OK);
  return cs;
}

static D_UINT
stack_top_text_update (struct INTERNAL_HANDLER* hnd,
                       const W_ELEMENT_INDEX      fromPos,
                       const D_CHAR* const      value)
{
  D_UINT32       cs         = WCS_OK;
  const D_UINT16 fixedSize  =  sizeof (D_UINT8) +
                               sizeof (D_UINT16) +
                               sizeof (D_UINT64);
  const D_UINT16 newEntSize = fixedSize + strlen (value) + 1;

  D_UINT8* data_;
  D_UINT   currSize;

  assert (hnd != NULL);
  assert (strlen (value) > 0);

  if (newEntSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal[UPDATE_SIZE] + fixedSize) < currSize)
      && (data_[hnd->cmdInternal[UPDATE_SIZE]] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if (prevType == WFT_TEXT)
        {
          data_ += sizeof (D_UINT16);

          const D_UINT64 prevPos = from_le_int64 (data_);
          data_ += sizeof (D_UINT64);

          const D_UINT64 prevValueLen = strlen ((const D_CHAR*)data_);
          data_ += prevValueLen;

          if ((prevPos + prevValueLen) == fromPos)
            {
              const D_UINT newDataSize = hnd->cmdInternal[UPDATE_SIZE] +
                                         fixedSize +
                                         prevValueLen +
                                         strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  currSize = data_size (hnd);
  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (WFT_TEXT, data_);
  data_ += sizeof (D_UINT16);

  store_le_int64 (fromPos, data_);
  data_ += sizeof (D_UINT64);

  strcpy ((D_CHAR*)data_, value);

  assert ((currSize + newEntSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + newEntSize);

  hnd->cmdInternal[UPDATE_SIZE] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

static D_UINT
stack_top_field_text_update (struct INTERNAL_HANDLER* hnd,
                             const D_CHAR* const      fieldName,
                             const W_TABLE_ROW_INDEX  row,
                             const W_ELEMENT_INDEX    fromPos,
                             const D_CHAR* const      value)
{
  D_UINT32       cs         = WCS_OK;
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
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal[UPDATE_SIZE] + fixedSize) < currSize)
      && (data_[hnd->cmdInternal[UPDATE_SIZE]] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if (prevType == (WFT_TEXT | WFT_FIELD_MASK))
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
              const D_UINT newDataSize = hnd->cmdInternal[UPDATE_SIZE] +
                                         fixedSize +
                                         prevValueLen +
                                         strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  currSize = data_size (hnd);
  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (WFT_TEXT | WFT_FIELD_MASK, data_),
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

  hnd->cmdInternal[UPDATE_SIZE] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

static D_UINT
stack_top_array_update (struct INTERNAL_HANDLER* hnd,
                        const D_UINT             type,
                        const W_ELEMENT_INDEX      fromPos,
                        const D_CHAR* const      value)
{
  D_UINT32       cs         = WCS_OK;
  D_UINT32       countOff   = sizeof (D_UINT8) +
                              sizeof (D_UINT16);
  const D_UINT16 fixedSize  = countOff +
                              sizeof (D_UINT16) +
                              sizeof (D_UINT64);
  const D_UINT16 newEntSize = fixedSize + strlen (value) + 1;

  D_UINT8* data_;
  D_UINT   currSize;

  assert (hnd != NULL);
  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));
  assert (strlen (value) > 0);

  if (newEntSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal[UPDATE_SIZE] + fixedSize) < currSize)
      && (data_[hnd->cmdInternal[UPDATE_SIZE]] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if ((type | WFT_ARRAY_MASK) == prevType)
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
              data_ = data (hnd) + hnd->cmdInternal[UPDATE_SIZE] + countOff;
              store_le_int16 (prevCount + 1, data_);

              const D_UINT newDataSize = data_size (hnd) + strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              data_ = data (hnd) + data_size (hnd);
              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  data_    = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (type | WFT_ARRAY_MASK, data_);
  data_ += sizeof (D_UINT16);

  store_le_int16 (1, data_);
  data_ += sizeof (D_UINT16);

  store_le_int64 (fromPos, data_);
  data_ += sizeof (D_UINT64);

  strcpy ((D_CHAR*)data_, value);

  assert ((currSize + newEntSize) <= max_data_size (hnd));
  set_data_size (hnd, currSize + newEntSize);

  hnd->cmdInternal[UPDATE_SIZE] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

static D_UINT
stack_top_field_array_update (struct INTERNAL_HANDLER* hnd,
                              const D_UINT             type,
                              const D_CHAR*            fieldName,
                              const W_TABLE_ROW_INDEX    row,
                              const W_ELEMENT_INDEX      fromPos,
                              const D_CHAR* const      value)
{
  D_UINT32       cs         = WCS_OK;
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
  assert ((WFT_BOOL <= type) && (type < WFT_TEXT));
  assert (strlen (fieldName) > 0);
  assert (strlen (value) > 0);

  if (newEntSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;
  else if (max_data_size (hnd) - newEntSize < data_size (hnd))
    {
      if ((cs = WUpdateStackFlush (hnd)) != WCS_OK)
        return cs;
    }

  data_    = data (hnd);
  currSize = data_size (hnd);

  if (((hnd->cmdInternal[UPDATE_SIZE] + fixedSize) < currSize)
      && (data_[hnd->cmdInternal[UPDATE_SIZE]] == CMD_UPDATE_FUNC_CHTOP))
  {
      const D_UINT16 prevType = from_le_int16 (++data_);
      if ((type | WFT_ARRAY_MASK | WFT_FIELD_MASK) == prevType)
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
              data_ = data (hnd) + hnd->cmdInternal[UPDATE_SIZE] + countOff;
              store_le_int16 (prevCount + 1, data_);

              const D_UINT newDataSize = data_size (hnd) + strlen (value) + 1;

              assert (newDataSize <= max_data_size (hnd));
              assert (newDataSize > currSize);

              data_ = data (hnd) + data_size (hnd);
              strcpy ((D_CHAR*)data_, value);
              set_data_size (hnd, newDataSize);

              return WCS_OK;
            }
        }
  }

  data_ = data (hnd) + currSize;

  *data_++ = CMD_UPDATE_FUNC_CHTOP;

  store_le_int16 (type | WFT_ARRAY_MASK | WFT_FIELD_MASK, data_);
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

  hnd->cmdInternal[UPDATE_SIZE] = currSize;

  assert (cs == WCS_OK);
  return cs;
}

D_UINT
WUpdateStackValue (const W_CONNECTOR_HND         hnd,
                   const unsigned int            valueType,
                   const char* const             fieldName,
                   const W_TABLE_ROW_INDEX       row,
                   const W_ELEMENT_INDEX         position,
                   const char* const             value)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT32 cs = WCS_OK;

  if ((hnd_ == NULL)
      || (value == NULL)
      || (value[0] == 0))
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

  if ((valueType & (WFT_FIELD_MASK | WFT_ARRAY_MASK)) == 0)
    {
      if ((fieldName != NULL) || (row != 0))
        return WCS_INVALID_ARGS;
      else if ((valueType & 0xFF) != WFT_TEXT)
        {
          if (position != 0)
            return WCS_INVALID_ARGS;

          cs = stack_top_basic_update (hnd_, valueType, value);
        }
      else
        cs = stack_top_text_update (hnd_, position, value);
    }
  else if ((valueType & (WFT_FIELD_MASK | WFT_ARRAY_MASK)) == WFT_ARRAY_MASK)
    {
      if ((fieldName != NULL) || (row != 0))
        return WCS_INVALID_ARGS;
      else if ((valueType & 0xFF) == WFT_TEXT)
        return WCS_OP_NOTSUPP;

      cs = stack_top_array_update (hnd_, valueType & 0xFF, position, value);
    }
  else if ((valueType & WFT_FIELD_MASK) != 0)
    {
      if ((fieldName == NULL) || (fieldName[0] == 0))
        return WCS_INVALID_ARGS;

      if ((valueType & WFT_ARRAY_MASK) == 0)
        {
          if ((valueType & 0xFF) == WFT_TEXT)
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
          if ((valueType & 0xFF) == WFT_TEXT)
            return WCS_OP_NOTSUPP;

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
      cs = WCS_GENERAL_ERR;
    }

  hnd_->buildingCmd = CMD_UPDATE_STACK;

  return cs;
}

D_UINT
WUpdateStackFlush (const W_CONNECTOR_HND hnd)
{
  struct INTERNAL_HANDLER* hnd_ = (struct INTERNAL_HANDLER*)hnd;

  D_UINT32  cs  = WCS_OK;

  D_UINT16  type;

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

static D_UINT
send_stack_read_req (struct INTERNAL_HANDLER* const hnd,
                     const D_CHAR*                  fieldName,
                     const D_UINT64                 rowIndex,
                     const D_UINT64                 elOffset)
{
  D_UINT   cs       = WCS_OK;
  D_UINT16 type     = 0;

  const D_UINT neededSize = ((fieldName != NULL) ?
                              strlen (fieldName) + 1 : 1) +
                            sizeof (D_UINT64) +
                            sizeof (D_UINT64);

  if (neededSize > max_data_size (hnd))
    return WCS_LARGE_ARGS;

  {
    D_UINT64       dataSize = 0;
    D_UINT8* const data_    = data (hnd);

    strcpy ((D_CHAR*)data_, fieldName);
    dataSize += strlen (fieldName) + 1;

    store_le_int64 (rowIndex, data_ + dataSize);
    dataSize += sizeof (D_UINT64);

    store_le_int64 (elOffset, data_ + dataSize);
    dataSize += sizeof (D_UINT64);

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

static D_UINT
table_next_field (struct INTERNAL_HANDLER* const hnd,
                  D_UINT16                       from)
{
  const D_UINT8* const data_     = data (hnd);
  const D_UINT16       field_len = strlen ((const D_CHAR*)data_ + from) + 1;
  D_UINT16             type      = from_le_int16 (data_ + from + field_len);

  assert ((type & WFT_FIELD_MASK) != 0);

  from += field_len + sizeof (D_UINT16);
  if (from >= data_size (hnd))
    return 0;

  if (type & WFT_ARRAY_MASK)
    {
      D_UINT64 max_count = from_le_int64 (data_ + from);
      from += sizeof (D_UINT64);

      if (max_count != 0)
        {
          max_count -= from_le_int64 (data_ + from);
          from += sizeof (D_UINT64);

          while ((from < data_size (hnd)) && (max_count-- > 0))
            from += strlen ((const D_CHAR*)data_) + 1;
        }
    }
  else
    {
      type &= ~(WFT_FIELD_MASK | WFT_ARRAY_MASK);
      if (type == WFT_TEXT)
        {
          const D_UINT64 chars_count = from_le_int64 (data_ + from);
          from += sizeof (D_UINT64);

          if (chars_count != 0)
            {
              from += sizeof (D_UINT64);
              from += strlen ((const D_CHAR*)data_ + from) + 1;
            }
        }
      else if ((type >= WFT_BOOL) && (type < WFT_TEXT))
        from += strlen ((const D_CHAR*)data_ + from) + 1;
      else
        return 0;
    }

  if (from >= data_size (hnd))
    return 0;

  return from;
}

static D_UINT
field_off_from_stack_cache (struct INTERNAL_HANDLER* const hnd,
                            const D_CHAR* const            field_name)
{
  const D_UINT8* const data_ = data (hnd);
  D_UINT16       type        = 0;

  assert (strlen (field_name) > 0);
  assert (hnd->lastCmdRespReceived == CMD_READ_STACK_RSP);
  assert (data_size (hnd) > (D_UINT32) + sizeof (D_UINT16));
  assert (from_le_int32 (data_) == WCS_OK);

  type = from_le_int16 (data_ + sizeof (D_UINT32));

  D_UINT16 fieldOff = sizeof (D_UINT32) + sizeof (D_UINT16);
  if ((type & WFT_TABLE_MASK) != 0)
    {
      /* Search to see if a field value is cached. */
      if (from_le_int64 (data_ + fieldOff) == 0)
        return 0;

      fieldOff += sizeof (D_UINT64) + sizeof (D_UINT32);
      do
        {
          const D_CHAR* cached_field = (const D_CHAR*)data_ + fieldOff;
          if (strcmp (cached_field, field_name) == 0)
            return fieldOff;

          fieldOff = table_next_field (hnd, fieldOff);
        }
      while ((0 < fieldOff) && (fieldOff < data_size (hnd)));

      return 0; /* We did not find anything */
    }
  else if ((type & WFT_FIELD_MASK) != 0)
    {
      const D_BOOL ffound = strcmp (
                                (const D_CHAR*)data_ + fieldOff,
                                field_name
                                   ) == 0;
      if (ffound)
        return fieldOff;
    }

  /* The cached stack value does not contains any field values. */
  return 0;
}

static D_UINT16
row_off_from_stack_cache (struct INTERNAL_HANDLER* const hnd,
                          const D_UINT64                 row,
                          const D_BOOL                   table_entry,
                          const D_UINT16                 fieldOff,
                          D_UINT16* const                pValueType)
{
  const D_UINT8* const data_ = data (hnd);

  D_UINT16 field_type = 0;
  D_UINT16 result     = fieldOff;

  D_UINT64 rowPos;

  assert (strlen ((const D_CHAR*)data_ + fieldOff) > 0);
  assert (hnd->lastCmdRespReceived == CMD_READ_STACK_RSP);
  assert (data_size (hnd) > (D_UINT32) + sizeof (D_UINT16));
  assert (from_le_int32 (data_) == WCS_OK);

  if (table_entry)
    {
      assert (from_le_int16 (data_ + sizeof (D_UINT32)) == WFT_TABLE_MASK);

      const D_UINT64 rowsCount =
          from_le_int64 (data_ + sizeof (D_UINT32) + sizeof (D_UINT16));
      if (rowsCount == 0)
        return 0;

      rowPos = from_le_int64 (
            data_ + sizeof (D_UINT32) + sizeof (D_UINT16) + sizeof (D_UINT64)
                             );
      if (rowPos != row)
        return 0;

      *pValueType  = from_le_int16 (data_ + result);
      *pValueType &= ~(WFT_FIELD_MASK);

      result += sizeof (D_UINT16);
      result += strlen ((const D_CHAR*)data_ + result) + 1;

      return result;
    }

  assert (sizeof (D_UINT32) == fieldOff);
  assert ((from_le_int16 (data_ + fieldOff) & WFT_FIELD_MASK) != 0);

  field_type  = from_le_int16 (data_ + fieldOff);
  field_type &= ~WFT_FIELD_MASK;

  *pValueType = field_type;

  result += sizeof (D_UINT16);
  result += strlen ((const D_CHAR*)data_ + result) + 1;

  rowPos = from_le_int64 (data_ + result);
  if (row < rowPos)
    return 0;

  result += sizeof (D_UINT64);

  assert (result < data_size (hnd));

  while (rowPos < row)
    {
      assert (result <= data_size (hnd));
      if (data_size (hnd) <= result)
        return 0;

      if ((field_type & WFT_ARRAY_MASK) == 0)
        {
          assert ((WFT_BOOL <= field_type) && (field_type <= WFT_TEXT));
          if (field_type != WFT_TEXT)
            result += strlen ((const D_CHAR*)data_ + result) + 1;
          else
            {
              D_UINT64 ch_count = from_le_int64 (data_ + result);
              result += sizeof (D_UINT64);

              if (ch_count != 0)
                {
                  result += sizeof (D_UINT64);
                  result += strlen ((const D_CHAR*)data_ + result) + 1;
                }
            }
        }
      else
        {
          field_type &= ~WFT_ARRAY_MASK;

          assert ((WFT_BOOL <= field_type) && (field_type < WFT_TEXT));
          D_UINT64 elemsCount = from_le_int64 (data_ + result);
          result += sizeof (D_UINT64);

          if (elemsCount > 0)
            {
              const D_UINT64 fromPos = from_le_int64 (data_ + result);
              result += sizeof (D_UINT64);

              assert (fromPos < elemsCount);
              elemsCount -= fromPos;
              while (elemsCount-- > 0)
                {
                  assert (result <= data_size (hnd));
                  if (result >= data_size (hnd))
                    return 0;

                  result += strlen ((const D_CHAR*)data_ + result) + 1;
                }
            }
        }

      rowPos++;
    }

  assert (result <= data_size (hnd));
  if (result >= data_size (hnd))
    return 0;

  assert (result > sizeof (D_UINT32));

  return result;
}

static D_UINT16
array_el_off_from_stack_cache (struct INTERNAL_HANDLER* const hnd,
                               const D_UINT64                 elOff,
                               D_UINT16                       offset,
                               D_UINT64* const                pCount)
{
  const D_UINT8* data_   = data (hnd);
  const D_UINT64 elCount = from_le_int64 (data_ + offset);
  D_UINT64       fromPos;

  assert (sizeof (D_UINT32) <= offset);
  assert (offset <= data_size (hnd));

  *pCount = elCount;
  if (elCount == 0)
    return 0;

  offset += sizeof (D_UINT64);
  if (data_size (hnd) <= offset)
    return 0;

  fromPos = from_le_int64 (data_ + offset);
  if ((elOff < fromPos) || (elCount <= elOff))
    return 0;

  while (fromPos < elOff)
    {
      if (data_size (hnd) <= offset)
        return 0;

      offset += strlen ((const D_CHAR*)data_ + offset) + 1;
      fromPos++;
    }

  if (data_size (hnd) <= offset)
    return 0;

  return offset;

}

static D_UINT16
text_el_off_from_stack_cache (struct INTERNAL_HANDLER* const hnd,
                              const D_UINT64                 elOff,
                              D_UINT16                       offset,
                              D_UINT64* const                pCount)
{
  const D_UINT8* data_   = data (hnd);
  const D_UINT64 elCount = from_le_int64 (data_ + offset);
  D_UINT64       fromPos;

  assert (sizeof (D_UINT32) <= offset);
  assert (offset <= data_size (hnd));

  *pCount = elCount;
  if (elCount == 0)
    return 0;

  offset += sizeof (D_UINT64);
  if (data_size (hnd) <= offset)
    return 0;

  fromPos = from_le_int64 (data_ + offset);
  if ((elOff < fromPos) || (elCount <= elOff))
    return 0;

  while (fromPos < elOff)
    {
      D_UINT chSize = 0;

      if (data_size (hnd) <= offset)
        return 0;

      chSize = get_utf8_char_size (data_[offset]);
      if (chSize == 0)
        return 0;

      offset += chSize;
      fromPos++;
    }

  if (data_size (hnd) <= offset)
    return 0;

  return offset;

}


unsigned int
WStackValueRawType (const W_CONNECTOR_HND hnd,
                    unsigned int*         pType)
{
  struct INTERNAL_HANDLER* hnd_    = (struct INTERNAL_HANDLER*)hnd;
  const  D_UINT8*          data_   = NULL;
  D_UINT                   cs      = WCS_OK;

  if ((hnd == NULL) || (pType == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd_->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  if ((hnd_->lastCmdRespReceived != CMD_READ_STACK_RSP)
      || (from_le_int32 (data (hnd)) != WCS_OK))
    {
      cs = send_stack_read_req (hnd_,
                                WIGNORE_FIELD,
                                WIGNORE_ROW,
                                WIGNORE_OFFSET);
      if (cs != WCS_OK)
        return cs;
    }

  data_  = data (hnd_);
  *pType = from_le_int16 (data_ + sizeof (D_UINT32));

  assert (from_le_int32 (data_) == WCS_OK);

  return cs;
}

unsigned int
WGetStackValueRowsCount (const W_CONNECTOR_HND       hnd,
                         unsigned long long* const   pCount)
{
  struct INTERNAL_HANDLER* hnd_    = (struct INTERNAL_HANDLER*)hnd;
  const  D_UINT8*          data_   = NULL;
  D_UINT                   cs      = WCS_OK;
  D_UINT16                 valType = 0;

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
                                WIGNORE_OFFSET);
      if (cs != WCS_OK)
        return cs;
    }

  data_   = data (hnd_);
  valType = from_le_int16 (data_ + sizeof (D_UINT32));

  assert (from_le_int32 (data_) == WCS_OK);

  if (valType & WFT_TABLE_MASK)
    *pCount = from_le_int64 (data_ + sizeof (D_UINT32) + sizeof (D_UINT16));
  else if (valType & WFT_FIELD_MASK)
    {
      D_UINT16 offset = sizeof (D_UINT32) + sizeof (D_UINT16);
      offset += strlen ((const D_CHAR*)data_ + offset) + 1;

      assert (offset < data_size (hnd_));

      *pCount = from_le_int64 (data_ + offset);
    }
  else
    cs = WCS_INVALID_ARGS;

  return cs;
}

static int
get_stack_value (struct INTERNAL_HANDLER* hnd,
                 const char* const        field,
                 W_TABLE_ROW_INDEX        row,
                 const W_ELEMENT_INDEX    fromPos,
                 D_UINT64* const          pElementsCount,
                 D_UINT16* const          pValueType,
                 const char** const       pValue)
{
  const  D_UINT8*          data_    = NULL;
  D_UINT                   cs       = WCS_OK;
  D_BOOL                   tryAgain = TRUE;

  D_UINT64 elOff;
  D_UINT64 rowOff;
  D_BOOL   tableVal;
  D_BOOL   fieldVal;

  if ((hnd == NULL) || (pValue == NULL))
    return WCS_INVALID_ARGS;
  else if (hnd->buildingCmd != CMD_INVALID)
    return WCS_INCOMPLETE_CMD;

  if ((hnd->lastCmdRespReceived != CMD_READ_STACK_RSP)
      || (from_le_int32 (data (hnd)) != WCS_OK))
    {
      cs = send_stack_read_req (hnd, field, row, fromPos);

      tryAgain = FALSE;
      if (cs != WCS_OK)
        goto exit_get_stack_entry;
    }

resend_req__get_stack_entry:

  data_    = data (hnd);
  elOff    = 0;
  rowOff   = 0;
  tableVal = 0;
  fieldVal = 0;

  *pValueType = from_le_int16 (data_ + sizeof (D_UINT32));

  if (*pValueType == WFT_TABLE_MASK)
    tableVal = TRUE;
  else if ((*pValueType & WFT_FIELD_MASK) != 0)
    fieldVal = TRUE;
  else
    {
      if ((field != WIGNORE_FIELD) || (row != WIGNORE_ROW))
        {
          tryAgain = FALSE;
          cs      = WCS_INVALID_ARGS;
          goto exit_get_stack_entry;
        }
    }

  if (tableVal || fieldVal)
    {
      D_UINT64 fieldOff;

      if (field == WIGNORE_FIELD)
        {
          tryAgain = FALSE;
          cs       = WCS_INVALID_ARGS;

          goto exit_get_stack_entry;
        }

      fieldOff = field_off_from_stack_cache (hnd, field);
      if (fieldOff == 0)
        {
          cs = WCS_INVALID_ARGS;
          goto exit_get_stack_entry;
        }

      rowOff = row_off_from_stack_cache (hnd,
                                         row,
                                         tableVal,
                                         fieldOff,
                                         pValueType);
      if (rowOff == 0)
        {
          cs = WCS_INVALID_ARGS;
          goto exit_get_stack_entry;
        }
    }
    else
      rowOff = sizeof (D_UINT32) + sizeof (D_UINT16);

    assert (rowOff > 0);
    assert (WIGNORE_OFFSET == 0);

    *pValueType &= ~(WFT_TABLE_MASK | WFT_FIELD_MASK);
    if (*pValueType & WFT_ARRAY_MASK)
      {
        elOff = array_el_off_from_stack_cache (hnd,
                                               fromPos,
                                               rowOff,
                                               pElementsCount);
        if (elOff == 0)
          {
            cs = WCS_INVALID_ARGS;
            goto exit_get_stack_entry;
          }

        *pValue = (const D_CHAR*)data_ + elOff;
      }
    else if (*pValueType == WFT_TEXT)
      {
        elOff = text_el_off_from_stack_cache (hnd,
                                              fromPos,
                                              rowOff,
                                              pElementsCount);
        if (elOff == 0)
          {
            cs = WCS_INVALID_ARGS;
            goto exit_get_stack_entry;
          }
        *pValue = (const D_CHAR*)data_ + elOff;
      }
    else if (fromPos != WIGNORE_OFFSET)
      {
        cs = WCS_INVALID_ARGS;
        goto exit_get_stack_entry;
      }
    else
      {
        elOff = sizeof (D_UINT32) + sizeof (D_UINT16);

        *pValue         = (const D_CHAR*)data_ + elOff;
        *pElementsCount = 1;
      }

    assert (cs = WCS_OK);
    assert (WFT_BOOL <= (*pValueType & ~WFT_ARRAY_MASK));
    assert (((*pValueType & ~WFT_ARRAY_MASK) < WFT_TEXT)
            || (*pValueType == WFT_TEXT));

exit_get_stack_entry:

  if (tryAgain && (cs != WCS_OK))
    {
      tryAgain = FALSE;

      cs = send_stack_read_req (hnd, field, row, fromPos);
      if (cs == WCS_OK)
        goto resend_req__get_stack_entry;
    }

  return cs;
}

unsigned int
WGetStackArrayElementsCount (const W_CONNECTOR_HND   hnd,
                             const char*             field,
                             const W_TABLE_ROW_INDEX row,
                             unsigned long long*     pCount)
{
  D_UINT64      count = 0;
  D_UINT16      type  = 0;
  const D_CHAR* value = NULL;

  D_UINT cs = get_stack_value (hnd, field, row, 0, &count, &type, &value);

  if (cs != WCS_OK)
    return cs;
  else if ((type & WFT_ARRAY_MASK) == 0)
    return WCS_INVALID_ARGS;

  assert (value != NULL);

  *pCount = count;
  return WCS_OK;
}

unsigned int
WGetStackTextLengthCount (const W_CONNECTOR_HND   hnd,
                          const char*             field,
                          const W_TABLE_ROW_INDEX row,
                          unsigned long long*     pCount)
{
  D_UINT64      count = 0;
  D_UINT16      type  = 0;
  const D_CHAR* value = NULL;

  D_UINT cs = get_stack_value (hnd, field, row, 0, &count, &type, &value);

  if (cs != WCS_OK)
    return cs;
  else if (type != WFT_TEXT)
    return WCS_INVALID_ARGS;

  assert (value != NULL);

  *pCount = count;
  return WCS_OK;
}

unsigned int
WGetStackValueEntry (const W_CONNECTOR_HND   hnd,
                     const char* const       field,
                     W_TABLE_ROW_INDEX       row,
                     const W_ELEMENT_INDEX   index,
                     const char** const      pValue)
{
  D_UINT64      count = 0;
  D_UINT16      type  = 0;

  D_UINT cs = get_stack_value (hnd, field, row, 0, &count, &type, pValue);

  if (cs != WCS_OK)
    return cs;

  assert (*pValue != NULL);

  return WCS_OK;
}

static D_UINT
describe_proc_parameter (struct INTERNAL_HANDLER* hnd,
                         const D_CHAR* const      procName,
                         const D_UINT             parameter)
{
  const D_UINT nameLen = strlen (procName) + 1;

  D_UINT   cs = WCS_OK;
  D_UINT8* data_;
  D_UINT16 type;

  if ((nameLen + sizeof (D_UINT16) > max_data_size (hnd))
      || (parameter > 0xFFFF))
    {
      cs = WCS_LARGE_ARGS;
      goto describe_proc_parameter_err;
    }

  set_data_size (hnd, nameLen + sizeof (D_UINT32));
  data_ = data (hnd);
  store_le_int16 (parameter, data_);
  strcpy ((D_CHAR*)data_ + 2 * sizeof (D_UINT16), procName);

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
  else if ((strcmp ((D_CHAR*)data_ + sizeof (D_UINT32), procName) != 0)
          || (sizeof (D_UINT32) + nameLen + 3 * sizeof (D_UINT16) >
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
  const D_UINT8*           data_   = NULL;
  D_UINT                   cs      = WCS_OK;

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

      if (strcmp (procName, (const D_CHAR*)data_ + sizeof (D_UINT32)) != 0)
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

  *pCount = from_le_int16 (data_ + sizeof (D_UINT32) + strlen (procName) + 1);

  return WCS_OK;
}

static D_UINT
get_paratmeter_offset (struct INTERNAL_HANDLER* const       hnd,
                       const D_UINT                         param,
                       D_UINT16* const                      pOff)
{
  const D_UINT8* data_   = data (hnd);
  const D_UINT   nameLen = strlen ((D_CHAR*)data_ + sizeof (D_UINT32)) + 1;

  D_UINT16 count = 0;
  D_UINT16 hint  = 0;
  D_UINT   it;

  assert (data_size (hnd) > sizeof (D_UINT32) + sizeof (D_UINT16));
  assert (hnd->lastCmdRespReceived == CMD_DESC_PROC_PARAM_RSP);
  assert (from_le_int32 (data (hnd)) == WCS_OK);

  *pOff =  sizeof (D_UINT32);
  *pOff += nameLen;

  count = from_le_int16 (data_ + *pOff); *pOff += sizeof (D_UINT16);
  hint  = from_le_int16 (data_ + *pOff); *pOff += sizeof (D_UINT16);

  if ((param < hint) || (count < param))
    return WCS_INVALID_ARGS;

  for (it = hint; (it < param) && (*pOff < data_size (hnd)); ++it)
    {
      const D_UINT16 type = from_le_int16 (data_ + *pOff);
      *pOff += sizeof (D_UINT16);
      if (type & WFT_TABLE_MASK)
        {
          D_UINT field;

          const D_UINT16 fieldsCount = from_le_int16 (data_ + *pOff);

          *pOff += sizeof (D_UINT16);

          for (field = 0;
               (field < fieldsCount) && (*pOff < data_size (hnd));
               ++field)
            {
              *pOff += strlen ((D_CHAR*)data_ + *pOff) + 1;
              *pOff += sizeof (D_UINT16);
            }
        }
      else
        *pOff += sizeof (D_UINT16);
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
  const D_UINT8*           data_    = NULL;
  D_UINT                   cs       = WCS_OK;
  D_BOOL                   tryAgain = TRUE;

  D_UINT16 offset;

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

      if (strcmp (procName, (const D_CHAR*)data_ + sizeof (D_UINT32)) != 0)
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

  assert (offset <= data_size (hnd_) - sizeof (D_UINT16));
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
  const D_UINT8*           data_    = NULL;
  D_UINT                   cs       = WCS_OK;
  D_BOOL                   tryAgain = TRUE;

  D_UINT16 offset;

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

      if (strcmp (procName, (const D_CHAR*)data_ + sizeof (D_UINT32)) != 0)
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

  assert (offset <= data_size (hnd_) - sizeof (D_UINT16));

  offset += sizeof (D_UINT16);
  *pCount = from_le_int16 (data_ + offset);
  assert (*pCount != 0);

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
  const D_UINT8*           data_    = NULL;
  D_UINT                   cs       = WCS_OK;
  D_BOOL                   tryAgain = TRUE;

  D_UINT16  iterator;
  D_UINT16  fieldsCount;
  D_UINT16  offset;

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

      if (strcmp (procName, (const D_CHAR*)data_ + sizeof (D_UINT32)) != 0)
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

  assert (offset <= data_size (hnd_) - sizeof (D_UINT16));

  offset += sizeof (D_UINT16);
  fieldsCount = from_le_int16 (data_ + offset);

  if (fieldsCount <= field)
    {
      cs = WCS_INVALID_ARGS;
      goto proc_parameter_field_err;
    }

  offset += sizeof (D_UINT16);
  for (iterator = 0; iterator < field; ++iterator)
    {
      offset += strlen ((D_CHAR*)data_ + offset) + 1;
      offset += sizeof (D_UINT16);
    }

  *pFieldName = (D_CHAR*)data_ + offset;
  offset += strlen (*pFieldName) + 1;
  *pFieldType = from_le_int16 (data_ + offset);

  assert (offset <= data_size (hnd_) - sizeof (D_UINT16));
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
