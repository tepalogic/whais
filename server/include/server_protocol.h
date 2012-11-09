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

#ifndef SERVER_PROTOCOL_H_
#define SERVER_PROTOCOL_H_

#include "client/include/whisper_connector.h"

#define FRAME_SIZE_OFF                  0x00
#define FRAME_TYPE_OFF                  0x02
#define FRAME_ENCTYPE_OFF               0x03
#define FRAME_ID_OFF                    0x04
#define FRAME_DATA_OFF                  0x08

#define FRAME_TYPE_NORMAL               0x00
#define FRAME_TYPE_PARTIAL              0x01
#define FRAME_TYPE_PARTIAL_ACK          0x02
#define FRAME_TYPE_PARTIAL_CANCEL       0x03
#define FRAME_TYPE_AUTH_CLNT            0x04
#define FRAME_TYPE_AUTH_CLNT_RSP        0x05
#define FRAME_TYPE_COMM_NOSYNC          0xFD
#define FRAME_TYPE_TIMEOUT              0xFE
#define FRAME_TYPE_SERV_BUSY            0xFF

#define FRAME_ENCTYPE_PLAIN             0x01
#define FRAME_ENCTYPE_IXS               0x02

#define PLAIN_CLNT_COOKIE_OFF           0x00
#define PLAIN_SERV_COOKIE_OFF           0x04
#define PLAIN_TYPE_OFF                  0x08
#define PLAIN_CRC_OFF                   0x0A
#define PLAIN_DATA_OFF                  0x0C

/* Authenticate offsets */
/*
 * Auth
 * {
 *      version : 32bit map
 * }
 *
 * AuthRsp
 * {
 *      version  : 32bit map     // Chosen interface.
 *      userId   : uint8
 *      database : char[]
 *      password : char[]
 * }
 */
#define FRAME_AUTH_CLNT_VER             0x00
#define FRAME_AUTH_CLNT_USR             0x04
#define FRAME_AUTH_CLNT_RESERVED        0x05
#define FRAME_AUTH_CLNT_DATA            0x08

#define FRAME_MAX_SIZE                  0x1000

#define ADMIN_CMD_BASE                  0x0000
#define USER_CMD_BASE                   0x1000

/* List database context globals */
#define CMD_INVALID                     ADMIN_CMD_BASE
#define CMD_INVALID_RSP                 (CMD_INVALID + 1)

#define CMD_LIST_GLOBALS                (CMD_INVALID_RSP + 1)
#define CMD_LIST_GLOBALS_RSP            (CMD_LIST_GLOBALS + 1)
/*
 * CmdListResponseRsp
 * {
 *      status       : uint8
 *      globalsCount : uint32;
 *      frameVars    : uint8
 *      varNames     : char[]
 * }
 *
 * CmdListResponsePartialRsp
 * {
 *      status       : uint8
 *      frameVars    : uint8
 *      varNames     : char[]
 * }
 */

/* Global describe type */
#define CMD_GLOBAL_DESC                 (CMD_LIST_GLOBALS_RSP + 1)
#define CMD_GLOBAL_DESC_RSP             (CMD_GLOBAL_DESC + 1)

/*
 * CmdGlobalDesc
 * {
 *     nameSize : uint16
 *     name     : char[]
 * }
 *
 * CmdGlobalDescRsp, CmdGlobalDescRspPartial
 * {
 *      status       : uint8
 *      typeDescSend : uint16
 *      typeDesc     : uint8[]
 * }
 */

/* Connection close command */
#define CMD_CLOSE_CONN          USER_CMD_BASE
#define CMD_CLOSE_CONN_RSP      (CMD_CLOSE_CONN + 1)

#define CMD_READ_STACK          (CMD_CLOSE_CONN_RSP + 1)
#define CMD_READ_STACK_RSP      (CMD_READ_STACK + 1)

#define CMD_UPDATE_STACK        (CMD_READ_STACK_RSP + 1)
#define CMD_UPDATE_STACK_RSP    (CMD_UPDATE_STACK + 1)

#define CMD_UPDATE_FUNC_POP     1
#define CMD_UPDATE_FUNC_PUSH    2
#define CMD_UPDATE_FUNC_CHTOP   3

/*
 * CmdUpdate
 * {
 *      value_name : uint8[];
 *      field_name : uint8[];
 *      type       : uint16;
 *      flags      : uint16;
 *      row_index  : uint64;
 *      from_pos   : uint64;
 *      data       : uint8 [];
 * }
 *
 * CmdSubCmdPop
 * {
 *      count      : uint32;
 * }
 *
 * CmdSubCmdPush
 * {
 *      type      : uint16
 *      fieldName : uint8[] (type == FT_TABLE)
 *      fieldType : uint16 (type == FT_TABLE)
 * }
 *
 * CmdSubcmdUpdateTop
 * {
 *      flags     : uint16
 *      fieldName : uint8[]  (flags & FT_FIELD_MASK)
 *      rowIndex  : uint8[]  (flags & FT_FIELD_MASK)
 *      arrayIndex: uint8[]  (flags & FT_ARRAY_MASK)
 *      count     : uint8[]  (flags & FT_ARRAY_MASK)
 *      value1    : uint8[]  (count > 0)
 *      value2    : uint8[]  (count > 1)
 *      .
 *      .
 *      .
 *      valuen   : uint8[]   (count > n-1)
 * }
 *
 * CmdUpdatRsp
 * {
 *      status: uint16
 * }
 *
 */

/* Ping command */
#define CMD_PING_SERVER         (CMD_UPDATE_STACK_RSP + 1)
#define CMD_PING_SERVER_RSP     (CMD_PING_SERVER + 1)


#define ADMIN_CMDS_COUNT        ((CMD_GLOBAL_DESC / 2) + 1)
#define USER_CMDS_COUNT         ((CMD_PING_SERVER - USER_CMD_BASE) / 2 + 1)

#endif /* SERVER_PROTOCOL_H_ */
