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

typedef void*               CONNECTOR_HND;
typedef unsigned long long  TABLE_ROW_INDEX;
typedef unsigned long long  ELEMENT_INDEX;
typedef unsigned int        VALUE_TYPE;

static const unsigned int FT_BOOL       = 0x0001;
static const unsigned int FT_CHAR       = 0x0002;
static const unsigned int FT_DATE       = 0x0003;
static const unsigned int FT_DATETIME   = 0x0004;
static const unsigned int FT_HIRESTIME  = 0x0005;
static const unsigned int FT_INT8       = 0x0006;
static const unsigned int FT_INT16      = 0x0007;
static const unsigned int FT_INT32      = 0x0008;
static const unsigned int FT_INT64      = 0x0009;
static const unsigned int FT_UINT8      = 0x000A;
static const unsigned int FT_UINT16     = 0x000B;
static const unsigned int FT_UINT32     = 0x000C;
static const unsigned int FT_UINT64     = 0x000D;
static const unsigned int FT_REAL       = 0x000E;
static const unsigned int FT_RICHREAL   = 0x000F;
static const unsigned int FT_TEXT       = 0x0010;
static const unsigned int FT_ARRAY_MASK = 0x0100;
static const unsigned int FT_FIELD_MASK = 0x0200;
static const unsigned int FT_TABLE_MASK = 0x0400;

static const unsigned int CS_OK                 = 0;
static const unsigned int CS_INVALID_ARGS       = 1;
static const unsigned int CS_OP_NOTSUPP         = 2;
static const unsigned int CS_OP_NOTPERMITED     = 3;
static const unsigned int CS_DROPPED            = 4;
static const unsigned int CS_ENCTYPE_NOTSUPP    = 5;
static const unsigned int CS_UNEXPECTED_FRAME   = 6;
static const unsigned int CS_INVALID_FRAME      = 7;
static const unsigned int CS_COMM_OUT_OF_SYNC   = 8;
static const unsigned int CS_LARGE_ARGS         = 9;
static const unsigned int CS_CONNECTION_TIMEOUT = 10;
static const unsigned int CS_SERVER_BUSY        = 11;
static const unsigned int CS_GENERAL_ERR        = 0x0FFF;
static const unsigned int CS_OS_ERR_BASE        = 0x1000;

#define CONNECTOR_ENC_ALL   (CONNECTOR_ENC_PLAIN | CONNECTOR_ENC_ISX)
#define ENCODE_OS_ERROR(x)  ((x) + CS_OS_ERR_BASE)
#define DECODE_OS_ERROR(x)  ((x) - CS_OS_ERR_BASE)

struct FieldDescriptor
{
  const    char*          m_FieldName;
  unsigned int            m_FieldType;
};

unsigned int
Connect (const char* const    host,
         const char* const    port,
         const char* const    databaseName,
         const char* const    password,
         const unsigned int   userid,
         CONNECTOR_HND* const pHnd);
void
Close (CONNECTOR_HND hnd);

unsigned int
PingServer (const CONNECTOR_HND hnd);

unsigned int
ListGlobals (const CONNECTOR_HND hnd, unsigned int* const pGlbsCount);

unsigned int
ListGlobalsFetch (const CONNECTOR_HND hnd, const char** const ppGlobalName);

unsigned int
ListGlobalsFetchCancel (const CONNECTOR_HND hnd);

unsigned int
DescribeValue (const CONNECTOR_HND    hnd,
               const char* const      globalName,
               unsigned int* const    pTypeDescSize);

unsigned int
DescribeValueFetch (const CONNECTOR_HND         hnd,
                    const unsigned char** const ppGlbTypeDataChunk,
                    unsigned int* const         pChunkSize);

unsigned int
DescribeValueFetchCancel (const CONNECTOR_HND hnd);


unsigned int
PushStackValue (const CONNECTOR_HND                 hnd,
                const unsigned int                  type,
                const unsigned int                  fieldsCount,
                const struct FieldDescriptor* const fields);
unsigned int
PopStackValues (const CONNECTOR_HND hnd,
                unsigned int        count);

unsigned int
UpdateStackValue (const CONNECTOR_HND           hnd,
                  const unsigned int            valueType,
                  const char* const             fieldName,
                  const TABLE_ROW_INDEX         row,
                  const ELEMENT_INDEX           position,
                  const char* const             value);
unsigned int
UpdateStackFlush (const CONNECTOR_HND hnd);

#ifdef __cplusplus
}
#endif
#endif /* WHISPER_CONNECTOR_H_ */
