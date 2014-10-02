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
#include <memory.h>
#include <sstream>

#include "utils/endianness.h"

#include "commands.h"
#include "stack_cmds.h"


static void
cmd_invalid (ClientConnection&)
{
  throw ConnectionException (_EXTRA (0), "Invalid command received.");
}


static void
cmd_value_desc (ClientConnection& conn)
{
  uint32_t result = WCS_OK;

  uint8_t*      data_      = conn.Data ();
  const char*   glbName    = _RC (const char*, data_ + sizeof (uint32_t));
  uint16_t      fieldHint  = load_le_int16 (data_);
  uint16_t      dataOffset = sizeof (uint32_t) + strlen (glbName) + 1;
  ISession&     session    = *conn.Dbs ().mSession;
  uint_t        rawType;

  if (conn.DataSize () < dataOffset)
    {
      throw ConnectionException (
                            _EXTRA (0),
                            "Command used to retrieve description of globals"
                              " variables has invalid format."
                                );
    }

  conn.DataSize (conn.MaxSize ());

  try
    {
      if (strlen (_RC (const char*, glbName)) > 0)
        {
          if (! conn.IsAdmin ())
            {
              throw ConnectionException (
                                    _EXTRA (0),
                                    "Only and admin may request for a "
                                      "global value description."
                                        );
            }

          rawType = session.GlobalValueRawType (glbName);
          store_le_int32 (WCS_OK, data_);

          store_le_int16 (rawType, data_ + dataOffset);
          dataOffset += sizeof (uint16_t);

          if (IS_TABLE (rawType))
            {
              const FIELD_INDEX fieldsCount =
                                    session.GlobalValueFieldsCount (glbName);

              store_le_int16 (fieldsCount, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);

              store_le_int16 (fieldHint, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);


              if ((fieldsCount > 0) && (fieldHint >= fieldsCount))
                {
                  result = WCS_INVALID_ARGS;

                  goto cmd_glb_desc_err;
                }
              else if (fieldsCount > 0xFFFF)
                {
                  result = WCS_LARGE_ARGS;

                  goto cmd_glb_desc_err;
                }

              bool oneAtLeast = false;
              while (fieldHint < fieldsCount)
                {
                  const char* fieldName =
                          session.GlobalValueFieldName (glbName, fieldHint);

                  const uint_t fieldLen  = strlen (fieldName) + 1;
                  const uint_t fieldType =
                          session.GlobalValueFieldType (glbName, fieldHint);

                  if (dataOffset + fieldLen + sizeof (uint16_t) >
                        conn.DataSize ())
                    {
                      break;
                    }

                  strcpy (_RC (char*, data_ + dataOffset), fieldName);
                  dataOffset += fieldLen;

                  store_le_int16 (fieldType, data_ + dataOffset);
                  dataOffset += sizeof (uint16_t);

                  ++fieldHint;
                  oneAtLeast = true;
                }

              if ((fieldsCount > 0) && ! oneAtLeast)
                {
                  result = WCS_LARGE_ARGS;

                  goto cmd_glb_desc_err;
                }
            }
          else if (fieldHint > 0)
            {
              result = WCS_INVALID_ARGS;

              goto cmd_glb_desc_err;
            }
        }
      else
        {
          SessionStack& stack = conn.Stack ();

          if (stack.Size() == 0)
            {
              result = WCS_INVALID_ARGS;

              goto cmd_glb_desc_err;
            }

          IOperand& op = stack[stack.Size () - 1].Operand ();
          rawType = op.GetType ();
          store_le_int32 (WCS_OK, data_);

          store_le_int16 (rawType, data_ + dataOffset);
          dataOffset += sizeof (uint16_t);

          if (IS_TABLE (rawType))
            {
              ITable&           table       = op.GetTable ();
              const FIELD_INDEX fieldsCount = table.FieldsCount ();

              assert (fieldsCount > 0);

              store_le_int16 (fieldsCount, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);

              store_le_int16 (fieldHint, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);

              if (fieldHint >= fieldsCount)
                {
                  result = WCS_INVALID_ARGS;

                  goto cmd_glb_desc_err;
                }
              else if (fieldsCount > 0xFFFF)
                {
                  result = WCS_LARGE_ARGS;

                  goto cmd_glb_desc_err;
                }

              bool oneAtLeast = false;
              while (fieldHint < fieldsCount)
                {
                  const DBSFieldDescriptor fd = table.DescribeField (fieldHint);

                  const uint_t fieldLen  = strlen (fd.name) + 1;
                  uint_t       fieldType = fd.type;

                  if (fd.isArray)
                    MARK_ARRAY (fieldType);

                  if (dataOffset + fieldLen + sizeof (uint16_t) >
                        conn.DataSize ())
                    {
                      break;
                    }

                  strcpy (_RC (char*, data_ + dataOffset), fd.name);
                  dataOffset += fieldLen;

                  store_le_int16 (fieldType, data_ + dataOffset);
                  dataOffset += sizeof (uint16_t);

                  ++fieldHint;
                  oneAtLeast = true;
                }

              if (! oneAtLeast)
                {
                  result = WCS_LARGE_ARGS;
                  goto cmd_glb_desc_err;
                }
            }
          else if (fieldHint > 0)
            {
              result = WCS_INVALID_ARGS;
              goto cmd_glb_desc_err;
            }
        }
    }
  catch (InterException&)
    {
      result = WCS_INVALID_ARGS;

      goto cmd_glb_desc_err;
    }
  catch (DBSException& )
    {
      result = WCS_INVALID_ARGS;

      goto cmd_glb_desc_err;
    }

  assert (result == WCS_OK);

  conn.DataSize (dataOffset);
  conn.SendCmdResponse (CMD_GLOBAL_DESC_RSP);

  return ;

cmd_glb_desc_err:

  assert (result != WCS_OK);

  store_le_int32 (result, data_);

  conn.DataSize (sizeof (uint32_t));
  conn.SendCmdResponse (CMD_GLOBAL_DESC_RSP);
}

static void
cmd_read_stack (ClientConnection& conn)
{
  uint8_t* const       data    = conn.Data ();
  uint32_t             status  = WCS_OK;
  uint_t               dataOff = 0;

  const char* const fieldNameHint = _RC (const char*, data + dataOff);
  dataOff += strlen (fieldNameHint) + 1;

  const uint64_t rowHint = load_le_int64 (data + dataOff);
  dataOff += sizeof (uint64_t);

  const uint64_t arrayHint  = load_le_int64 (data + dataOff);
  dataOff += sizeof (uint64_t);

  const uint64_t textHint  = load_le_int64 (data + dataOff);
  dataOff += sizeof (uint64_t);

  if ((dataOff != conn.DataSize ())
      || (conn.Stack ().Size () == 0))
    {
      status = WCS_INVALID_ARGS;
      goto cmd_read_exit;
    }

  try
    {
      conn.DataSize (conn.MaxSize ());
      dataOff = sizeof (uint32_t) + sizeof (uint16_t);

      StackValue&    topValue = conn.Stack ()[conn.Stack ().Size () - 1];
      const uint16_t valType  = topValue.Operand ().GetType ();

      if (IS_TABLE (valType))
        {
          ITable&     table     = topValue.Operand ().GetTable ();
          FIELD_INDEX fieldHint = (fieldNameHint[0] != 0) ?
                                    table.RetrieveField (fieldNameHint) :
                                    0;

          status = cmd_read_table_stack_top (conn,
                                             topValue,
                                             fieldHint,
                                             rowHint,
                                             arrayHint,
                                             textHint,
                                             &dataOff);
        }
      else if (IS_FIELD (valType))
        {
          status = cmd_read_field_stack_top (conn,
                                             topValue,
                                             rowHint,
                                             arrayHint,
                                             textHint,
                                             &dataOff);
        }
      else if (IS_ARRAY (valType))
        {
          assert ((valType & 0xFF) != WHC_TYPE_TEXT);

          status = cmd_read_array_stack_top (conn,
                                             topValue,
                                             textHint,
                                             &dataOff);
        }
      else if (GET_BASIC_TYPE (valType) == T_TEXT)
        status = cmd_read_text_stack_top (conn, topValue, textHint, &dataOff);

      else
        status = cmd_read_basic_stack_top (conn, topValue, &dataOff);

      store_le_int16 (valType, data + sizeof (uint32_t));
    }
  catch (DBSException& e)
  {
      const uint_t extra = e.Code ();

      if (extra == DBSException::FIELD_NOT_FOUND)
        status = WCS_INVALID_FIELD;

      else if (extra == DBSException::ARRAY_INDEX_TOO_BIG)
        status = WCS_INVALID_ARRAY_OFF;

      else if (extra == DBSException::STRING_INDEX_TOO_BIG)
        status = WCS_INVALID_TEXT_OFF;

      else if (extra == DBSException::ROW_NOT_ALLOCATED)
        status = WCS_INVALID_ROW;

      else
        throw ;
  }

  assert (dataOff >= sizeof (uint32_t));

  conn.DataSize (dataOff);

cmd_read_exit:

  if (status != WCS_OK)
    conn.DataSize (sizeof (uint32_t));

  store_le_int32 (status, conn.Data());

  conn.SendCmdResponse (CMD_READ_STACK_RSP);
}

static void
cmd_update_stack (ClientConnection& conn)
{
  const uint8_t* const data    = conn.Data ();
  uint32_t             status  = WCS_OK;
  uint_t               dataOff = 0;

  if (conn.DataSize () == 0)
    {
      status = WCS_INVALID_ARGS;

      goto cmd_update_exit;
    }

  while (dataOff < conn.DataSize ())
    {
      const uint8_t subcmd = data[dataOff++];
      switch (subcmd)
      {
      case CMD_UPDATE_FUNC_PUSH:
          status = cmd_push_stack (conn, &dataOff);
          break;

      case CMD_UPDATE_FUNC_POP:
          status = cmd_pop_stack (conn, &dataOff);
          break;

      case CMD_UPDATE_FUNC_CHTOP:
          status = cmd_update_stack_top (conn, &dataOff);
          break;

      case CMD_UPDATE_FUNC_TBL_ROWS:
          status = cmd_update_stack_table_add_rows (conn, &dataOff);
          break;

      default:
        throw ConnectionException (
                               _EXTRA (subcmd),
                               "Encountered an unexpected update command."
                                  );
      }

      if (status != WCS_OK)
        goto cmd_update_exit;
    }

  assert (status == WCS_OK);
  assert (dataOff == conn.DataSize ());

cmd_update_exit:

  store_le_int32 (status, conn.Data());
  conn.DataSize (sizeof (status));

  conn.SendCmdResponse (CMD_UPDATE_STACK_RSP);
}

static void
cmd_execute_procedure (ClientConnection& conn)
{
  const char*   procName = _RC (const char*, conn.Data ());
  ISession&     session  = *conn.Dbs ().mSession;
  SessionStack& stack    = conn.Stack ();
  uint32_t      result   = WCS_GENERAL_ERR;

  try
  {
    session.ExecuteProcedure (procName, stack);
    result = WCS_OK;
  }
  catch (InterException& e)
  {
    if (e.Code () == InterException::INVALID_PROC_REQ)
      {
        result = WCS_PROC_NOTFOUND;

        std::ostringstream logEntry;

        logEntry << "Failed to find procedure '" << procName << "'.";
        session.GetLogger ().Log (LOG_ERROR, logEntry.str ().c_str ());
      }
    else
      throw;
  }

  store_le_int32 (result, conn.Data ());
  conn.DataSize (sizeof (uint32_t));

  conn.SendCmdResponse (CMD_EXEC_PROC_RSP);
}

static void
cmd_ping_sever (ClientConnection& conn)
{
  if (conn.DataSize () != 0)
    {
      throw ConnectionException (_EXTRA (0),
                                 "Ping command has invalid format.");
    }

  store_le_int32 (WCS_OK, conn.Data ());
  conn.DataSize (sizeof (uint32_t));

  conn.SendCmdResponse (CMD_PING_SERVER_RSP);
}

static void
cmd_list_globals (ClientConnection& conn)
{
  uint32_t result = WCS_OK;

  if (conn.DataSize () != sizeof (uint32_t))
    {
      throw ConnectionException (
                              _EXTRA (0),
                              "Command used to retrieve context globals"
                                " variables has invalid format."
                                );
    }

  const ISession& session    = *conn.Dbs ().mSession;
  const uint32_t  glbsCount  = session.GlobalValuesCount ();
  uint32_t        firstHint  = load_le_int32 (conn.Data ());
  uint16_t        dataOffset = 0;
  bool            oneAtLeast = false;

  conn.DataSize (conn.MaxSize ());
  store_le_int32 (WCS_OK, conn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (glbsCount, conn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (firstHint, conn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  assert (conn.DataSize () > 3 * sizeof (uint32_t));

  if ((glbsCount > 0) && (firstHint >= glbsCount))
    {
      result = WCS_INVALID_ARGS;

      goto cmd_list_globals_err;
    }

  for (; firstHint < glbsCount; ++firstHint)
    {
      const char* const name    = session.GlobalValueName (firstHint);
      const uint_t      nameLen = strlen (_RC (const char*, name)) + 1;

      if (dataOffset + nameLen >= conn.DataSize())
        {
          if (! oneAtLeast)
            {
              //This global variable name is too large.
              result = WCS_LARGE_ARGS;
              goto cmd_list_globals_err;

            }
          else
            break;
        }

      oneAtLeast = true;

      memcpy (conn.Data () + dataOffset, name, nameLen);
      dataOffset += nameLen;
    }

  assert (result == WCS_OK);
  assert (dataOffset <= conn.DataSize ());
  assert ((glbsCount == 0) || oneAtLeast);

  conn.DataSize (dataOffset);
  conn.SendCmdResponse (CMD_LIST_GLOBALS_RSP);

  return;

cmd_list_globals_err:

  assert (result != WCS_OK);

  store_le_int32 (result, conn.Data ());
  conn.DataSize (sizeof (result));

  conn.SendCmdResponse (CMD_LIST_GLOBALS_RSP);
}

static void
cmd_list_procedures (ClientConnection& conn)
{
  uint32_t result = WCS_OK;

  if (conn.DataSize () != sizeof (uint32_t))
    {
      throw ConnectionException (
                              _EXTRA (0),
                              "Command used to retrieve context globals"
                                " variables has invalid format."
                                );
    }

  const ISession& session = *conn.Dbs ().mSession;

  const uint32_t procsCount  = session.ProceduresCount ();
  uint32_t       firstHint   = load_le_int32 (conn.Data ());
  uint16_t       dataOffset  = 0;
  bool           oneAtLeast  = false;

  conn.DataSize (conn.MaxSize ());
  store_le_int32 (WCS_OK, conn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (procsCount, conn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (firstHint, conn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  assert (conn.DataSize () > 3 * sizeof (uint32_t));

  if ((procsCount > 0) && firstHint >= procsCount)
    {
      result = WCS_INVALID_ARGS;
      goto cmd_list_procedures_err;
    }

  for (; firstHint < procsCount; ++firstHint)
    {
      const char* const  name    = session.ProcedureName (firstHint);
      const uint_t       nameLen = strlen (_RC (const char*, name)) + 1;

      if (dataOffset + nameLen >= conn.DataSize())
        {
          if (! oneAtLeast)
            {
              result = WCS_LARGE_ARGS;

              goto cmd_list_procedures_err;
            }
          else
            break;
        }

      oneAtLeast = true;

      memcpy (conn.Data () + dataOffset, name, nameLen);

      dataOffset += nameLen;
    }

  assert (result == WCS_OK);
  assert (dataOffset <= conn.DataSize ());
  assert ((procsCount == 0) ^ oneAtLeast);

  conn.DataSize (dataOffset);
  conn.SendCmdResponse (CMD_LIST_PROCEDURE_RSP);

  return;

cmd_list_procedures_err:
  assert (result != WCS_OK);

  store_le_int32 (result, conn.Data ());
  conn.DataSize (sizeof (result));

  conn.SendCmdResponse (CMD_LIST_PROCEDURE_RSP);

  return;
}

static void
cmd_procedure_param_desc (ClientConnection& conn)
{
  uint32_t result = WCS_OK;

  if (conn.DataSize () < sizeof (uint16_t) + 2 * sizeof (uint8_t))
    {
      throw ConnectionException (
                              _EXTRA (0),
                              "Command used to retrieve context globals"
                                " variables has invalid format."
                                );
    }
  ISession&           session     = *conn.Dbs ().mSession;
  uint8_t*            data_       = conn.Data ();
  uint16_t            hint        = load_le_int16 (data_);
  const char* const   procName    = _RC (const char*,
                                         data_ + 2 * sizeof (uint16_t));
  const uint_t        procNameLen = strlen (procName) + 1;
  uint16_t            offset      = 0;
  bool                oneAtLeast  = false;
  uint_t              paramsCount = 0;

  try
  {
      paramsCount = session.ProcedureParametersCount (procName);
  }
  catch (InterException&)
  {
      result = WCS_INVALID_ARGS;
      goto cmd_procedure_param_desc_err;
  }

  if (paramsCount >= 0xFFFF)
    {
      result = WCS_LARGE_ARGS;

      goto cmd_procedure_param_desc_err;
    }
  else if (hint >= paramsCount)
    {
      result = WCS_INVALID_ARGS;

      goto cmd_procedure_param_desc_err;
    }

  conn.DataSize (conn.MaxSize ());
  if (2 * sizeof (uint32_t) + procNameLen > conn.DataSize ())
    {
      result = WCS_LARGE_ARGS;

      goto cmd_procedure_param_desc_err;
    }

  store_le_int32 (WCS_OK, data_);
  offset += sizeof (uint32_t);

  //Leave the name part unchanged!
  offset += strlen (_RC (const char*, procName)) + 1;

  store_le_int16 (paramsCount, data_ + offset);
  offset += sizeof (uint16_t);

  store_le_int16 (hint, data_ + offset);
  offset += sizeof (uint16_t);

  try
    {
      uint16_t lastValidOffset = offset;
      do
       {
          const uint_t paramType = session.ProcedurePameterRawType (procName,
                                                                    hint);
          if (IS_TABLE (paramType))
            {
              if (offset + sizeof (uint16_t) > conn.DataSize ())
                break;

              store_le_int16 (WHC_TYPE_TABLE_MASK, data_ + offset);
              offset += sizeof (uint16_t);

              const uint_t fieldsCount =
                session.ProcedurePameterFieldsCount (procName, hint);

              if ((fieldsCount > 0xFFFF)
                  || (offset + sizeof (uint16_t) >= conn.DataSize ()))
                {
                  break;
                }

              store_le_int16 (fieldsCount, data_ + offset);
              offset += sizeof (uint16_t);

              uint_t field = 0;
              for (; field < fieldsCount; field++)
                {
                  const char* fieldName = _RC (
                                const char*,
                                session.ProcedurePameterFieldName (procName,
                                                                   hint,
                                                                   field)
                                                );
                  const uint16_t fieldType =
                      session.ProcedurePameterFieldType (procName,
                                                         hint,
                                                         field);

                  const uint_t fieldLen = strlen (fieldName) + 1;
                  if (offset + fieldLen + sizeof (uint16_t) > conn.DataSize ())
                    break;

                  strcpy (_RC (char*, data_ + offset), fieldName);
                  offset += fieldLen;

                  store_le_int16 (fieldType, data_ + offset);
                  offset += sizeof (uint16_t);
                }

              if (field < fieldsCount)
                {
                  offset = lastValidOffset;
                  break;
                }
            }
          else
            {
              if (offset + sizeof (uint16_t) > conn.DataSize ())
                break;

              store_le_int16 (paramType, data_ + offset);
              offset += sizeof (uint16_t);
            }

          ++hint;
          oneAtLeast      = true;
          lastValidOffset = offset;
        }
      while (hint < paramsCount);

      if (! oneAtLeast)
        {
          result = WCS_LARGE_ARGS;
          goto cmd_procedure_param_desc_err;
        }

      offset = lastValidOffset;
    }
  catch (InterException&)
    {
      result = WCS_INVALID_ARGS;

      goto cmd_procedure_param_desc_err;
    }
  catch (DBSException& )
    {
      result = WCS_INVALID_ARGS;

      goto cmd_procedure_param_desc_err;
    }

  assert (result == WCS_OK);

  conn.DataSize (offset);
  conn.SendCmdResponse (CMD_DESC_PROC_PARAM_RSP);

  return;

cmd_procedure_param_desc_err:
  assert (result != WCS_OK);

  conn.DataSize (sizeof (result));
  store_le_int32 (result, conn.Data ());
  conn.SendCmdResponse (CMD_DESC_PROC_PARAM_RSP);

  return;
}



static COMMAND_HANDLER saAdminCmds[] =
    {
        cmd_invalid,                     // CMD_INVALID
        cmd_list_globals,                // CMD_LIST_GLOBALS
        cmd_list_procedures,             // CMD_LIST_PROC
        cmd_procedure_param_desc         // CMD_DESC_PROC_PARAM
    };


static COMMAND_HANDLER saUserCmds[] =
    {
        cmd_invalid,                     // CMD_CLOSE_CONN
        cmd_value_desc,                  // CMD_GLOBAL_DESC
        cmd_read_stack,                  // CMD_READ_STACK
        cmd_update_stack,                // CMD_UPDATE_STACK
        cmd_execute_procedure,           // CMD_EXEC_PROC
        cmd_ping_sever                   // CMD_PING_SERVER
    };


/* The commands registers external definitions. */
COMMAND_HANDLER* gpAdminCommands = saAdminCmds;
COMMAND_HANDLER* gpUserCommands  = saUserCmds;

