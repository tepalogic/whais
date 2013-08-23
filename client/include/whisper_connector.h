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

#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Custom types defined to ease the usage of the Whisper's connector API. */
typedef void*     WH_CONNECTION;
typedef ullong_t  WHT_ROW_INDEX;
typedef ullong_t  WHT_INDEX;

/* Constants describing the type of values or table fields. */
static const uint_t WHC_TYPE_BOOL       = 0x0001;
static const uint_t WHC_TYPE_CHAR       = 0x0002;
static const uint_t WHC_TYPE_DATE       = 0x0003;
static const uint_t WHC_TYPE_DATETIME   = 0x0004;
static const uint_t WHC_TYPE_HIRESTIME  = 0x0005;
static const uint_t WHC_TYPE_INT8       = 0x0006;
static const uint_t WHC_TYPE_INT16      = 0x0007;
static const uint_t WHC_TYPE_INT32      = 0x0008;
static const uint_t WHC_TYPE_INT64      = 0x0009;
static const uint_t WHC_TYPE_UINT8      = 0x000A;
static const uint_t WHC_TYPE_UINT16     = 0x000B;
static const uint_t WHC_TYPE_UINT32     = 0x000C;
static const uint_t WHC_TYPE_UINT64     = 0x000D;
static const uint_t WHC_TYPE_REAL       = 0x000E;
static const uint_t WHC_TYPE_RICHREAL   = 0x000F;
static const uint_t WHC_TYPE_TEXT       = 0x0010;
static const uint_t WHC_TYPE_NOTSET     = 0x0011;

/* Type modifiers to identify Whisper's composite types. */
static const uint_t WHC_TYPE_ARRAY_MASK = 0x0100;
static const uint_t WHC_TYPE_FIELD_MASK = 0x0200;
static const uint_t WHC_TYPE_TABLE_MASK = 0x0400;

/* Constants returned by connector's API to communicate their results. */
static const uint_t WCS_OK                 = 0;
static const uint_t WCS_INVALID_ARGS       = 1;
static const uint_t WCS_OP_NOTSUPP         = 2;
static const uint_t WCS_OP_NOTPERMITED     = 3;
static const uint_t WCS_DROPPED            = 4;
static const uint_t WCS_PROTOCOL_NOTSUPP   = 5;
static const uint_t WCS_ENCTYPE_NOTSUPP    = 6;
static const uint_t WCS_UNEXPECTED_FRAME   = 7;
static const uint_t WCS_INVALID_FRAME      = 8;
static const uint_t WCS_COMM_OUT_OF_SYNC   = 9;
static const uint_t WCS_LARGE_ARGS         = 10;
static const uint_t WCS_LARGE_RESPONSE     = 11;
static const uint_t WCS_CONNECTION_TIMEOUT = 12;
static const uint_t WCS_SERVER_BUSY        = 13;
static const uint_t WCS_INCOMPLETE_CMD     = 14;
static const uint_t WCS_INVALID_ARRAY_OFF  = 15;
static const uint_t WCS_INVALID_TEXT_OFF   = 16;
static const uint_t WCS_INVALID_ROW        = 17;
static const uint_t WCS_INVALID_FIELD      = 18;
static const uint_t WCS_TYPE_MISMATCH      = 19;
static const uint_t WCS_PROC_NOTFOUND      = 20;
static const uint_t WCS_GENERAL_ERR        = 0x0FFF;
static const uint_t WCS_OS_ERR_BASE        = 0x1000;

/* If a failure at the OS layer occurs, it will be signaled with the help of
 * 'WCS_OS_ERR_BASE'. Following macros encodes/decodes the error as it was
 * returned by the OS layer. That value is OS dependent. */
#define WENC_OS_ERROR(x)     ((x) + WCS_OS_ERR_BASE)
#define WDEC_OS_ERROR(x)     ((x) - WCS_OS_ERR_BASE)

/* Standard default parameters for connector's APIs. */
#define WIGNORE_FIELD        NULL
#define WIGNORE_ROW          (~0ull)
#define WIGNORE_OFF          (~0ull)
#define WPOP_ALL             (~0)

/* Describes the field of a table. */
struct WField
{
  const    char*    name;    /* Field name. */
  uint_t            type;    /* Field type. */
};


/* Connects to a remote sever.
 *
 * Called prior any other connector's API, it's used to setup the connection to
 * a remote Whisper server. The port is an UTF-8 string so one might use
 * service names too.
 */
uint_t
WConnect (const char* const      host,
          const char* const      port,
          const char* const      database,
          const char* const      password,
          const uint_t           userid,
          WH_CONNECTION* const   outHnd);

/* Close a connection.
 *
 * Instructs the remote server to discard and data related to this connection.
 */
void
WClose (WH_CONNECTION hnd);

/* Send a dummy command to server.
 *
 * This has several usages from keeping the connection alive or test the
 * connection healthiness.
 */
uint_t
WPingServer (const WH_CONNECTION hnd);

/* Get the list of the global values.
 *
 * Used to initialise the fetching of the global values defined by the context
 * of the specified database. The caller has to be connected as root (e.g. the
 * used user id should be set to 0) for this to proceed.
 */
uint_t
WListGlobals (const WH_CONNECTION hnd, uint_t* const outCount);

/* Fetch the name of the next global value. */
uint_t
WListGlobalsFetch (const WH_CONNECTION hnd,
                   const char** const  outpName);

/* Get the list of the procedures.
 *
 * Initialise the fetching of the procedure defined by the context of the
 * specified database. The caller has to be connected as root for this to
 * proceed.
 */
uint_t
WListProcedures (const WH_CONNECTION hnd, uint_t* const outCount);

/* Fetch the name of the next procedure. */
uint_t
WListProceduresFetch (const WH_CONNECTION   hnd,
                      const char** const    outpName);

/* Get the parameters count of a procedure. */
uint_t
WProcParamsCount (const WH_CONNECTION hnd,
                  const char* const   name,
                  uint_t* const       outCount);

/* Get procedures' parameter type.
 *
 * Retrieves the raw type of a procedures' parameters. In case it is a
 * composite type (e.g. table), other APIs have to be called too to get the
 * full description.
 */
uint_t
WProcParamType (const WH_CONNECTION   hnd,
                const char* const     procedure,
                const uint_t          param,
                uint_t* const         outRawType);

/* Get the fields count of a table parameter.
 *
 * For a procedure parameter describe as a table, this is used to get the
 * number of fields, to get the full description of it.
 */
uint_t
WProcParamFieldCount (const WH_CONNECTION   hnd,
                      const char* const     procedure,
                      const uint_t          param,
                      uint_t* const         outCount);

/* Describe a field of a procedure parameter.
 *
 * Retrieves the name and type of a procedure parameter field.
 */
uint_t
WProcParamField (const WH_CONNECTION   hnd,
                 const char* const     procedure,
                 const uint_t          param,
                 const uint_t          field,
                 const char**          outpFieldName,
                 uint_t* const         outFieldType);

/* Get the type of a global value.
 *
 * Retrieve the raw type of a global values. In case of a composite type
 * further actions will be required to get the full descriptions.
 */
uint_t
WGlobalType (const WH_CONNECTION    hnd,
             const char* const      name,
             uint_t* const          outRawType);

/* Get the type of the stack top.
 *
 * Retrieves that type of the value from the connection stack's top. It is a
 * raw value, so extra effort has to be made in order to get the full value's
 * type description.
 */
uint_t
WStackValueType (const WH_CONNECTION   hnd,
                 uint_t* const         outRawType);

/* Get the fields count of the stack value.
 *
 * In case the global or stack value's raw type points to a composite type
 * (e.g. a table), this would be used to get the fields count.
 */
uint_t
WFieldsCount (const WH_CONNECTION  hnd,
              uint_t* const        outCount);

/* Fetch the field type of the stack top value.
 *
 * Use repeatedly to get value field name and type. A prior call to
 * 'WStackValueType () or WGlobalType ()' is needed prior calling this.
 */
uint_t
WFetchField (const WH_CONNECTION  hnd,
             const char**         outpFieldName,
             uint_t* const        outFieldType);


/* Add a value on the stack top.
 *
 * Push a value on top of the connections stack. The newly added value will
 * have a null value bu default, but can be update with 'WUpdateValue ()'.
 * The actual update would be cached internally (see 'WFlush ()' for
 * more information).
 */

uint_t
WPushValue (const WH_CONNECTION             hnd,
            const uint_t                    type,
            const uint_t                    fieldsCount,
            const struct WField* const      fields);

/* Remove values from the stack top.
 *
 * Clears a certain number of values from the connection stack. The actual
 * update would be cached internally (see 'WFlush ()' for more
 * information).
 */
uint_t
WPopValues (const WH_CONNECTION hnd, const uint_t count);


/* Set stack's top value.
 *
 * Change the content of the top stack value. It can be used repeatedly in case
 * of table setups. The updates should made sequentially due to avoid failures
 * of internal limit. The rows of a table should be updated starting
 * from lower to bigger, array elements should update again from lower to
 * bigger indexes, etc.
 *
 * Note: 1. There is no way to to have a table with a NULL row.
 *       2. The result of this could be a fake success, as all it could be
 *          cached internally (see 'WFlush ()' for more details).
 */
uint_t
WUpdateValue (const WH_CONNECTION     hnd,
              const uint_t            type,
              const char* const       field,
              const WHT_ROW_INDEX     row,
              const WHT_INDEX         arrayOff,
              const WHT_INDEX         textOff,
              const char* const       value);

/* Flush all stack commits.
 *
 * To maximise network bandwidth, all stack update operations (push, pop and
 * set) might be cached internally. This forces their processing by server.
 *
 * Note: 1. If of the operations fails while is processed on the server side,
 *          the rest of the operation are canceled.
 *       2. If ones to know exactly what operation fails, this has to be called
 *          after all stack update functions.
 */
uint_t
WFlush (const WH_CONNECTION hnd);

/* Get the number of rows of the stack top values.
 *
 * It fails if the stack top does not hold a value with valid
 * type (table or field).
 */
uint_t
WValueRowsCount (const WH_CONNECTION  hnd,
                 ullong_t* const      outCount);

/* Get the top stack values array elements count. */
uint_t
WValueArraySize (const WH_CONNECTION   hnd,
                 const char*           field,
                 const WHT_ROW_INDEX   row,
                 ullong_t* const       outCount);

/* Get the stack top text length.
 *
 * The text values are encoded using UTF-8, and the length represents the
 * number of characters and not the number of bytes used to represent the
 * text. Also the length does not count the null terminator character.
 */
uint_t
WValueTextLength (const WH_CONNECTION     hnd,
                  const char*             field,
                  const WHT_ROW_INDEX     row,
                  const WHT_INDEX         arrayOff,
                  ullong_t* const         outCount);

/* Retrieve the stack top value.
 *
 * Get a representation of the actual value. The value is represented by an
 * null terminated UTF-8 encoded string.
 *
 * Note: 1. To improve caching usage the rows values are faster retrieved
 *          sequentially, from lower rows to bigger ones, and using the same
 *          fields order as the one returned during fetching.
 *       2. For big text values, not all chars will be returned by this call.
 *          Using a combination of 'WValueTextLength ()' and a text
 *          offset the all text should be accessible.
 *       3. Any text offsets are in terms of Unicode code points rather than
 *          bytes.
 */
uint_t
WValueEntry (const WH_CONNECTION hnd,
             const char* const   field,
             WHT_ROW_INDEX       row,
             const WHT_INDEX     arrayOff,
             const WHT_INDEX     textOff,
             const char** const  outpValue);

/* Execute a procedure remotely.
 *
 * The arguments of the specified procedure shall already been passed on the
 * stack, using the stack update functions.
 */
uint_t
WExecuteProcedure (const WH_CONNECTION     hnd,
                   const char* const       procedure);

#ifdef __cplusplus
}
#endif
#endif /* WHISPER_CONNECTOR_H_ */

