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

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include "whisper.h"

#include "server/include/server_protocol.h"

struct INTERNAL_HANDLER
{
  D_UINT64  cmdInternal;
  WH_SOCKET socket;
  D_UINT32  expectedFrameId;
  D_UINT32  serverCookie;
  D_UINT32  clientCookie;
  D_UINT32  encKeySize;
  D_UINT16  lastCmdRespReceived;
  D_UINT16  buildingCmd;
  D_UINT8   userId;
  D_UINT8   encType;
  D_UINT8   data [FRAME_MAX_SIZE];
  D_UINT8   encriptionKey[1];
};

#endif /* CONNECTOR_H_ */
