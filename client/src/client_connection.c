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

#include "utils/include/le_converter.h"

#include "client_connection.h"

D_UINT
read_raw_frame (struct INTERNAL_HANDLER* const pHnd,
                D_UINT16* const              pOutSize)
{
  D_UINT32 frameId;
  D_UINT16 frameSize = 0;

  while (frameSize < FRAME_DATA_OFF)
    {
      D_UINT chunkSize = FRAME_DATA_OFF - frameSize;

      const D_UINT32 status = wh_socket_read (pHnd->socket,
                                              &pHnd->data[frameSize],
                                              &chunkSize);
      if (status != WOP_OK)
        return CS_OS_ERR_BASE + status;
      else if (chunkSize == 0)
        return CS_DROPPED;

      frameSize += chunkSize;
    }

  frameId = from_le_int32 (&pHnd->data[FRAME_ID_OFF]);
  if (frameId != pHnd->expectedFrameId)
    return CS_UNEXPECTED_FRAME;

  switch (pHnd->data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
  case FRAME_TYPE_AUTH_CLNT:
    {
      const D_UINT16 expected = from_le_int16 (&pHnd->data[FRAME_SIZE_OFF]);

      if ((expected < frameSize)
          || (expected >= FRAME_MAX_SIZE))
        {
          return CS_UNEXPECTED_FRAME;
        }

      while (frameSize < expected)
        {
          D_UINT chunkSize = expected - frameSize;

          const D_UINT32 status = wh_socket_read (pHnd->socket,
                                                  &pHnd->data[frameSize],
                                                  &chunkSize);
          if (status != WOP_OK)
            return CS_OS_ERR_BASE + status;
          else if (chunkSize == 0)
            return CS_DROPPED;

          frameSize += chunkSize;
        }
      *pOutSize = expected;
      return CS_OK;
    }
  case FRAME_TYPE_SERV_BUSY:
    return CS_SERVER_BUSY;
  case FRAME_TYPE_TIMEOUT:
    return CS_CONNECTION_TIMEOUT;
  case FRAME_TYPE_COMM_NOSYNC:
    return CS_COMM_OUT_OF_SYNC;
  default:
    return CS_UNEXPECTED_FRAME;
  }

  return CS_UNEXPECTED_FRAME;
}
