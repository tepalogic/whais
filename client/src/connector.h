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

#define CLIENT_VERSION                    1

#define MAX_CMD_INTERNALS                 4

static const D_UINT LIST_GLBSCOUNT      = 0;
static const D_UINT LIST_GLBINDEX       = 1;
static const D_UINT LIST_GLBOFF         = 2;

static const D_UINT LIST_PROCSCOUNT     = 0;
static const D_UINT LIST_PROCSINDEX     = 1;
static const D_UINT LIST_PROCOFF        = 2;

static const D_UINT DESC_RAWTYPE        = 0;
static const D_UINT DESC_FIELD_COUNT    = 1;
static const D_UINT DESC_FIELD_HINT     = 2;
static const D_UINT DESC_FIELD_OFFSET   = 3;

static const D_UINT LAST_UPDATE_OFF     = 0;

struct INTERNAL_HANDLER
{
  D_UINT8*  data;
  D_UINT32  dataSize;
  D_UINT32  version;
  D_UINT32  cmdInternal[MAX_CMD_INTERNALS];
  WH_SOCKET socket;
  D_UINT32  expectedFrameId;
  D_UINT32  serverCookie;
  D_UINT32  clientCookie;
  D_UINT32  encKeySize;
  D_UINT16  lastCmdRespReceived;
  D_UINT16  buildingCmd;
  D_UINT8   userId;
  D_UINT8   cipher;
  D_UINT8   encriptionKey[1];
};

#endif /* CONNECTOR_H_ */
