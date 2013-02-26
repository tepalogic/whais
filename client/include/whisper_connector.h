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

typedef void*               W_CONNECTOR_HND;
typedef unsigned long long  W_TABLE_ROW_INDEX;
typedef unsigned long long  W_ELEMENT_OFFSET;

static const unsigned int WFT_BOOL       = 0x0001;
static const unsigned int WFT_CHAR       = 0x0002;
static const unsigned int WFT_DATE       = 0x0003;
static const unsigned int WFT_DATETIME   = 0x0004;
static const unsigned int WFT_HIRESTIME  = 0x0005;
static const unsigned int WFT_INT8       = 0x0006;
static const unsigned int WFT_INT16      = 0x0007;
static const unsigned int WFT_INT32      = 0x0008;
static const unsigned int WFT_INT64      = 0x0009;
static const unsigned int WFT_UINT8      = 0x000A;
static const unsigned int WFT_UINT16     = 0x000B;
static const unsigned int WFT_UINT32     = 0x000C;
static const unsigned int WFT_UINT64     = 0x000D;
static const unsigned int WFT_REAL       = 0x000E;
static const unsigned int WFT_RICHREAL   = 0x000F;
static const unsigned int WFT_TEXT       = 0x0010;
static const unsigned int WFT_NOTSET     = 0x0011;
static const unsigned int WFT_ARRAY_MASK = 0x0100;
static const unsigned int WFT_FIELD_MASK = 0x0200;
static const unsigned int WFT_TABLE_MASK = 0x0400;

static const unsigned int WCS_OK                 = 0;
static const unsigned int WCS_INVALID_ARGS       = 1;
static const unsigned int WCS_OP_NOTSUPP         = 2;
static const unsigned int WCS_OP_NOTPERMITED     = 3;
static const unsigned int WCS_DROPPED            = 4;
static const unsigned int WCS_ENCTYPE_NOTSUPP    = 5;
static const unsigned int WCS_UNEXPECTED_FRAME   = 6;
static const unsigned int WCS_INVALID_FRAME      = 7;
static const unsigned int WCS_COMM_OUT_OF_SYNC   = 8;
static const unsigned int WCS_LARGE_ARGS         = 9;
static const unsigned int WCS_LARGE_RESPONSE     = 10;
static const unsigned int WCS_CONNECTION_TIMEOUT = 11;
static const unsigned int WCS_SERVER_BUSY        = 12;
static const unsigned int WCS_INCOMPLETE_CMD     = 13;
static const unsigned int WCS_INVALID_ARRAY_OFF  = 14;
static const unsigned int WCS_INVALID_TEXT_OFF   = 15;
static const unsigned int WCS_INVALID_ROW        = 16;
static const unsigned int WCS_INVALID_FIELD      = 17;
static const unsigned int WCS_GENERAL_ERR        = 0x0FFF;
static const unsigned int WCS_OS_ERR_BASE        = 0x1000;

#define WCONNECTOR_ENC_ALL   (CONNECTOR_ENC_PLAIN | CONNECTOR_ENC_ISX)
#define WENC_OS_ERROR(x)     ((x) + WCS_OS_ERR_BASE)
#define WDEC_OS_ERROR(x)     ((x) - WCS_OS_ERR_BASE)

#define WIGNORE_FIELD                            NULL
#define WIGNORE_ROW                              (~0ull)
#define WIGNORE_OFF                              (~0ull)
#define WPOP_ALL                                 (~0)

struct W_FieldDescriptor
{
  const    char*          m_FieldName;
  unsigned int            m_FieldType;
};

unsigned int
WConnect (const char* const      host,
          const char* const      port,
          const char* const      databaseName,
          const char* const      password,
          const unsigned int     userid,
          W_CONNECTOR_HND* const pHnd);
void
WClose (W_CONNECTOR_HND hnd);

unsigned int
WPingServer (const W_CONNECTOR_HND hnd);

unsigned int
WListGlobals (const W_CONNECTOR_HND hnd, unsigned int* const pGlbsCount);

unsigned int
WListGlobalsFetch (const W_CONNECTOR_HND hnd, const char** const ppGlobalName);

unsigned int
WListProcedures (const W_CONNECTOR_HND hnd, unsigned int* const pProcsCount);

unsigned int
WListProceduresFetch (const W_CONNECTOR_HND hnd,
                      const char** const    ppProcName);

unsigned int
WProcedureParametersCount (const W_CONNECTOR_HND hnd,
                           const char* const     procName,
                           unsigned int* const   pCount);

unsigned int
WProcedureParameter (const W_CONNECTOR_HND hnd,
                     const char* const     procName,
                     const unsigned int    parameter,
                     unsigned int* const   pRawType);

unsigned int
WProcedureParameterFieldCount (const W_CONNECTOR_HND hnd,
                               const char* const     procName,
                               const unsigned int    parameter,
                               unsigned int* const   pCount);

unsigned int
WProcedureParameterField (const W_CONNECTOR_HND hnd,
                          const char* const     procName,
                          const unsigned int    parameter,
                          const unsigned int    field,
                          const char**          pFieldName,
                          unsigned int* const   pFieldType);

unsigned int
WDescribeValue (const W_CONNECTOR_HND    hnd,
                const char* const        globalName,
                unsigned int* const      pRawType);

unsigned int
WDescribeStackTop (const W_CONNECTOR_HND hnd,
                   unsigned int* const   pRawType);

unsigned int
WDescribeValueGetFieldsCount (const W_CONNECTOR_HND  hnd,
                              unsigned int* const    pFieldCount);

unsigned int
WDescribeValueFetchField (const W_CONNECTOR_HND  hnd,
                          const char**           ppFieldName,
                          unsigned int* const    pFieldType);

unsigned int
WPushStackValue (const W_CONNECTOR_HND                 hnd,
                 const unsigned int                    type,
                 const unsigned int                    fieldsCount,
                 const struct W_FieldDescriptor* const fields);
unsigned int
WPopStackValues (const W_CONNECTOR_HND hnd,
                 unsigned int          count);


unsigned int
WUpdateStackValue (const W_CONNECTOR_HND         hnd,
                   const unsigned int            type,
                   const char* const             fieldName,
                   const W_TABLE_ROW_INDEX       row,
                   const W_ELEMENT_OFFSET        arrayOff,
                   const W_ELEMENT_OFFSET        textOff,
                   const char* const             value);

unsigned int
WUpdateStackFlush (const W_CONNECTOR_HND hnd);

unsigned int
WGetStackValueRowsCount (const W_CONNECTOR_HND       hnd,
                         unsigned long long* const   pCount);


unsigned int
WGetStackArrayElementsCount (const W_CONNECTOR_HND   hnd,
                             const char*             field,
                             const W_TABLE_ROW_INDEX row,
                             unsigned long long*     pCount);

unsigned int
WGetStackTextLengthCount (const W_CONNECTOR_HND   hnd,
                          const char*             field,
                          const W_TABLE_ROW_INDEX row,
                          const W_ELEMENT_OFFSET  arrayOff,
                          unsigned long long*     pCount);

unsigned int
WGetStackValueEntry (const W_CONNECTOR_HND   hnd,
                     const char* const       field,
                     W_TABLE_ROW_INDEX       row,
                     const W_ELEMENT_OFFSET  arrayOff,
                     const W_ELEMENT_OFFSET  textOff,
                     const char** const      pValue);

unsigned int
WExecuteProcedure (const W_CONNECTOR_HND     hnd,
                   const char* const         procedure);

#ifdef __cplusplus
}
#endif
#endif /* WHISPER_CONNECTOR_H_ */
