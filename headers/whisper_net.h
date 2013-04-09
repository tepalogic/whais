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

#ifndef WHISPER_NET_H_
#define WHISPER_NET_H_

#ifdef __cplusplus
extern "C"
{
#endif

bool_t
wh_init_socks ();

uint32_t
wh_socket_client (const char* const       pServer,
                  const char* const       pPort,
                  WH_SOCKET* const          pOutSocket);

uint32_t
wh_socket_server (const char* const       pLocalAdress,
                  const char* const       pPort,
                  const uint_t              listenBackLog,
                  WH_SOCKET* const          pOutSocket);

uint32_t
wh_socket_accept (const WH_SOCKET           sd,
                  WH_SOCKET* const          pConnectSocket);

uint32_t
wh_socket_write (const WH_SOCKET           sd,
                 const uint8_t*            pBuffer,
                 const uint_t              count);

uint32_t
wh_socket_read (const WH_SOCKET           sd,
                uint8_t*                  pOutBuffer,
                uint_t* const             pIOCount);

void
wh_socket_close (const WH_SOCKET socket);

void
wh_clean_socks ();

#ifdef __cplusplus
}
#endif


#endif /* WHISPER_NET_H_ */
