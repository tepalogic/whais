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

#define MAX_CMint_tERNALS                 4

static const uint_t LIST_GLBSCOUNT      = 0;
static const uint_t LIST_GLBINDEX       = 1;
static const uint_t LIST_GLBOFF         = 2;

static const uint_t LIST_PROCSCOUNT     = 0;
static const uint_t LIST_PROCSINDEX     = 1;
static const uint_t LIST_PROCOFF        = 2;

static const uint_t DESC_RAWTYPE        = 0;
static const uint_t DESC_FIELD_COUNT    = 1;
static const uint_t DESC_FIELD_HINT     = 2;
static const uint_t DESC_FIELD_OFFSET   = 3;

static const uint_t LAST_UPDATE_OFF     = 0;

struct INTERNAL_HANDLER
{
  uint8_t*  data;
  uint32_t  dataSize;
  uint32_t  version;
  uint32_t  cmdInternal[MAX_CMint_tERNALS];
  WH_SOCKET socket;
  uint32_t  expectedFrameId;
  uint32_t  serverCookie;
  uint32_t  clientCookie;
  uint32_t  encKeySize;
  uint16_t  lastCmdRespReceived;
  uint16_t  buildingCmd;
  uint8_t   userId;
  uint8_t   cipher;
  uint8_t   encriptionKey[1];
};

#endif /* CONNECTOR_H_ */
