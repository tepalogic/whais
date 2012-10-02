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

#define FRAME_SIZE_OFF                  0x00
#define FRAME_TYPE_OFF                  0x02
#define FRAME_ENCTYPE_OFF               0x03
#define FRAME_ID_OFF                    0x04
#define FRAME_DATA_OFF                  0x08

#define FRAME_TYPE_NORMAL               0x00
#define FRAME_TYPE_AUTH_CLNT            0x01
#define FRAME_TYPE_AUTH_CLNT_RSP        0x02
#define FARME_TYPE_AUTH_SRV_DONE        0x04
#define FRAME_TYPE_COMM_NOSYNC          0xFD
#define FRAME_TYPE_TIMEOUT              0xFE
#define FRAME_TYPE_SERV_BUSY            0xFF

#define FRAME_ENCTYPE_PLAIN             0x01
#define FRAME_ENCTYPE_IXS               0x02

#define PLAIN_CLNT_COOKIE_OFF           0x00
#define PLAIN_SERV_COOKIE_OFF           0x04
#define PLAIN_TYPE_OFF                  0x08
#define PLAIN_RSERVED_OFF               0x09
#define PLAIN_CRC_OFF                   0x0A
#define PLAIN_OFF                       0x0C

/* Authenticate offsets */
#define FRAME_AUTH_CLNT_VER             0x00
#define FRAME_AUTH_CLNT_USR             0x04
#define FRAME_AUTH_CLNT_RESERVED        0x05
#define FRAME_AUTH_CLNT_DATA            0x08


#define FRAME_MAX_SIZE                  0x4000

#endif /* SERVER_PROTOCOL_H_ */
