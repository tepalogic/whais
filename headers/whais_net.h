/******************************************************************************
WHAIS - An advanced database system
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

#ifndef WHAIS_NET_H_
#define WHAIS_NET_H_

#ifdef __cplusplus
extern "C"
{
#endif

bool_t
whs_init();

uint32_t
whs_create_client( const char* const       server,
                   const char* const       port,
                   WH_SOCKET* const        outSocket);

uint32_t
whs_create_server( const char* const       localAddress,
                   const char* const       localPort,
                   const uint_t            listenBackLog,
                   WH_SOCKET* const        outSocket);

uint32_t
whs_accept( const WH_SOCKET           sd,
            WH_SOCKET* const          outSocket);

uint32_t
whs_write( const WH_SOCKET           sd,
           const uint8_t*            srcBuffer,
           const uint_t              count);

uint32_t
whs_read( const WH_SOCKET           sd,
          uint8_t*                  dstBuffer,
          uint_t* const             inoutCount);

void
whs_close( const WH_SOCKET socket);

void
whs_clean();

#ifdef __cplusplus
}
#endif

#endif /* WHAIS_NET_H_ */

