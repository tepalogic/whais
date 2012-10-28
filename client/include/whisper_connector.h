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

static const unsigned int  CS_OK                 = 0;
static const unsigned int  CS_INVALID_ARGS       = 1;
static const unsigned int  CS_OP_NOTPERMITED     = 2;
static const unsigned int  CS_DROPPED            = 3;
static const unsigned int  CS_ENCTYPE_NOTSUPP    = 4;
static const unsigned int  CS_UNEXPECTED_FRAME   = 5;
static const unsigned int  CS_INVALID_FRAME      = 6;
static const unsigned int  CS_COMM_OUT_OF_SYNC   = 7;
static const unsigned int  CS_LARGE_ARGS         = 8;
static const unsigned int  CS_CONNECTION_TIMEOUT = 9;
static const unsigned int  CS_SERVER_BUSY        = 10;
static const unsigned int  CS_GENERAL_ERR        = 11;
static const unsigned int  CS_OS_ERR_BASE        = 12;

#define CONNECTOR_ENC_ALL   (CONNECTOR_ENC_PLAIN | CONNECTOR_ENC_ISX)

unsigned int
Connect (const char* const   pHost,
         const char* const   pPort,
         const char* const   pDatabaseName,
         const char* const   pPassword,
         const unsigned int  userId,
         CONNECTOR_HND*     pHnd);
void
Close (CONNECTOR_HND hnd);

unsigned int
PingServer (const CONNECTOR_HND hnd);

unsigned int
ListGlobals (const CONNECTOR_HND hnd, unsigned int* poGlbsCount);

unsigned int
ListGlobalsFetch (const CONNECTOR_HND hnd, const char** poGlbName);

unsigned int
ListGlobalsFetchCancel (const CONNECTOR_HND hnd);

unsigned int
DescribeGlobal (const CONNECTOR_HND    hnd,
                const char*            pName,
                unsigned int*          poTypeDescSize);

unsigned int
DescribeGlobalFetch (const CONNECTOR_HND    hnd,
                     const unsigned char**  poGlbTypeInfoChunk,
                     unsigned int*          poChunkSize);

unsigned int
DescribeGlobalFetchCancel (const CONNECTOR_HND hnd);


#ifdef __cplusplus
}
#endif
#endif /* WHISPER_CONNECTOR_H_ */
