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

#ifndef WHISPER_CONNECTOR_H_
#define WHISPER_CONNECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* CONNECTOR_HND;

enum CONNECTOR_STATUS
{
  CS_OK,
  CS_INVALID_ARGS,
  CS_OP_NOTPERMITED,
  CS_DROPPED,
  CS_ENCTYPE_NOTSUPP,
  CS_UNEXPECTED_FRAME,
  CS_INVALID_FRAME,
  CS_COMM_OUT_OF_SYNC,
  CS_LARGE_ARGS,
  CS_CONNECTION_TIMEOUT,
  CS_SERVER_BUSY,
  CS_OS_INTERNAL,
  CS_UNKNOWN_ERR
};

#define CONNECTOR_ENC_ALL   (CONNECTOR_ENC_PLAIN | CONNECTOR_ENC_ISX)

enum CONNECTOR_STATUS
Connect (const char* const   pHost,
         const char* const   pPort,
         const char* const   pDatabaseName,
         const char* const   pPassword,
         const unsigned int  userId,
         CONNECTOR_HND*     pHnd);
void
Close (CONNECTOR_HND hnd);

enum CONNECTOR_STATUS
PingServer (const CONNECTOR_HND hnd);

enum CONNECTOR_STATUS
ListGlobals (const CONNECTOR_HND hnd, unsigned int* poGlbsCount);

enum CONNECTOR_STATUS
ListGlobalsFetch (const CONNECTOR_HND hnd, const char** poGlbName);

enum CONNECTOR_STATUS
ListGlobalsFetchCancel (const CONNECTOR_HND hnd);

enum CONNECTOR_STATUS
DescribeGlobal (const CONNECTOR_HND    hnd,
                const char*            pName,
                unsigned int*          poTypeDescSize);

enum CONNECTOR_STATUS
DescribeGlobalFetch (const CONNECTOR_HND    hnd,
                     const unsigned char**  poGlbTypeInfoChunk,
                     unsigned int*          poChunkSize);

enum CONNECTOR_STATUS
DescribeGlobalFetchCancel (const CONNECTOR_HND hnd);


#ifdef __cplusplus
}
#endif
#endif /* WHISPER_CONNECTOR_H_ */
