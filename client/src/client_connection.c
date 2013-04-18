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

#include "utils/le_converter.h"

#include "client_connection.h"

uint_t
read_raw_frame (struct INTERNAL_HANDLER* const pHnd,
                uint_t* const                  outFrameSize)
{
  uint32_t frameId;
  uint16_t frameSize = 0;

  while (frameSize < FRAME_HDR_SIZE)
    {
      uint_t chunkSize = FRAME_HDR_SIZE - frameSize;

      const uint32_t status = whs_read (pHnd->socket,
                                        &pHnd->data[frameSize],
                                        &chunkSize);
      if (status != WOP_OK)
        return WENC_OS_ERROR (status);
      else if (chunkSize == 0)
        return WCS_DROPPED;

      frameSize += chunkSize;
    }

  frameId = load_le_int32 (&pHnd->data[FRAME_ID_OFF]);
  if (frameId != pHnd->expectedFrameId)
    return WCS_UNEXPECTED_FRAME;

  switch (pHnd->data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
  case FRAME_TYPE_AUTH_CLNT:
    {
      const uint16_t expected = load_le_int16 (&pHnd->data[FRAME_SIZE_OFF]);

      if ((expected < frameSize)
          || (expected > pHnd->dataSize))
        {
          return WCS_UNEXPECTED_FRAME;
        }

      while (frameSize < expected)
        {
          uint_t chunkSize = expected - frameSize;

          const uint32_t status = whs_read (pHnd->socket,
                                                  &pHnd->data[frameSize],
                                                  &chunkSize);
          if (status != WOP_OK)
            return WENC_OS_ERROR (status);
          else if (chunkSize == 0)
            return WCS_DROPPED;

          frameSize += chunkSize;
        }
      *outFrameSize = expected;

      return WCS_OK;
    }
  case FRAME_TYPE_SERV_BUSY:
    return WCS_SERVER_BUSY;
  case FRAME_TYPE_TIMEOUT:
    return WCS_CONNECTION_TIMEOUT;
  case FRAME_TYPE_COMM_NOSYNC:
    return WCS_COMM_OUT_OF_SYNC;
  default:
    return WCS_UNEXPECTED_FRAME;
  }

  return WCS_UNEXPECTED_FRAME;
}


uint_t
write_raw_frame (struct INTERNAL_HANDLER* const pHnd,
                 const  uint_t                  frameSize)
{
  uint_t result = whs_write (pHnd->socket, pHnd->data, frameSize);
  if (result != WOP_OK)
    result = WENC_OS_ERROR (result);

  return result;
}

