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

#include "utils/include/le_converter.h"

#include "commands.h"
#include "stack_cmds.h"

static void
cmd_invalid (ClientConnection&)
{
  throw ConnectionException ("Invalid command received.",
                             _EXTRA (0));
}

static void
cmd_value_desc (ClientConnection& rConn)
{
  uint32_t result = WCS_OK;

  uint8_t*      data_      = rConn.Data ();
  const char* glbName    = _RC (const char*, data_ + sizeof (uint32_t));
  uint16_t      fieldHint  = from_le_int16 (data_);
  uint16_t      dataOffset = sizeof (uint32_t) + strlen (glbName) + 1;
  I_Session&    session    = *rConn.Dbs ().m_Session;
  uint_t        rawType;

  if (rConn.DataSize () < dataOffset)
    {
      throw ConnectionException (
                              "Command used to retrieve description of global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }

  rConn.DataSize (rConn.MaxSize ());

  try
    {
      if (strlen (_RC (const char*, glbName)) > 0)
        {
          if (! rConn.IsAdmin ())
            {
              throw ConnectionException (
                                      "Only and admin may request for a "
                                      "global value description.",
                                      _EXTRA (0)
                                        );
            }
          rawType = session.GlobalValueRawType (glbName);
          store_le_int32 (WCS_OK, data_);

          store_le_int16 (rawType, data_ + dataOffset);
          dataOffset += sizeof (uint16_t);

          if (IS_TABLE (rawType))
            {
              const FIELD_INDEX fieldsCount = session.GlobalValueFieldsCount (
                                                                       glbName
                                                                             );
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
              store_le_int16 (fieldsCount, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);

              store_le_int16 (fieldHint, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);

              bool oneAtLeast = false;
              do
                {
                  const char* fieldName = session.GlobalValueFieldName (
                                                                      glbName,
                                                                      fieldHint
                                                                         );
                  const uint_t  fieldLen  = strlen (fieldName) + 1;
                  const uint_t  fieldType = session.GlobalValueFieldType (
                                                                      glbName,
                                                                      fieldHint
                                                                         );
                  if (dataOffset + fieldLen + sizeof (uint16_t) >
                      rConn.DataSize ())
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
              while (fieldHint < fieldsCount);

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
      else
        {
          SessionStack& stack = rConn.Stack ();
          I_Operand*    op;

          if (stack.Size() == 0)
            {
              result = WCS_INVALID_ARGS;
              goto cmd_glb_desc_err;
            }

          op = &stack[stack.Size () - 1].GetOperand ();
          rawType = op->GetType ();
          store_le_int32 (WCS_OK, data_);

          store_le_int16 (rawType, data_ + dataOffset);
          dataOffset += sizeof (uint16_t);

          if (IS_TABLE (rawType))
            {
              I_DBSTable&       table       = op->GetTable ();
              const FIELD_INDEX fieldsCount = table.GetFieldsCount ();

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
              store_le_int16 (fieldsCount, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);

              store_le_int16 (fieldHint, data_ + dataOffset);
              dataOffset += sizeof (uint16_t);

              bool oneAtLeast = false;
              do
                {
                  const DBSFieldDescriptor fd = table.GetFieldDescriptor (
                                                                      fieldHint
                                                                          );
                  const uint_t fieldLen  = strlen (fd.m_pFieldName) + 1;
                  uint_t       fieldType = fd.m_FieldType;

                  if (fd.isArray)
                    MARK_ARRAY (fieldType);

                  if (dataOffset + fieldLen + sizeof (uint16_t) >
                      rConn.DataSize ())
                    {
                      break;
                    }

                  strcpy (_RC (char*, data_ + dataOffset), fd.m_pFieldName);
                  dataOffset += fieldLen;

                  store_le_int16 (fieldType, data_ + dataOffset);
                  dataOffset += sizeof (uint16_t);

                  ++fieldHint;
                  oneAtLeast = true;
                }
              while (fieldHint < fieldsCount);

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
  rConn.DataSize (dataOffset);
  rConn.SendCmdResponse (CMD_GLOBAL_DESC_RSP);

  return ;

cmd_glb_desc_err:
  assert (result != WCS_OK);

  store_le_int32 (result, data_);
  rConn.DataSize (sizeof (uint32_t));
  rConn.SendCmdResponse (CMD_GLOBAL_DESC_RSP);
}

static void
cmd_read_stack (ClientConnection& rConn)
{
  uint8_t* const       data    = rConn.Data ();
  uint32_t             status  = WCS_OK;
  uint_t               dataOff = 0;

  const char* const fieldNameHint = _RC (const char*, data + dataOff);
  dataOff += strlen (fieldNameHint) + 1;

  const uint64_t rowHint = from_le_int64 (data + dataOff);
  dataOff += sizeof (uint64_t);

  const uint64_t arrayHint  = from_le_int64 (data + dataOff);
  dataOff += sizeof (uint64_t);

  const uint64_t textHint  = from_le_int64 (data + dataOff);
  dataOff += sizeof (uint64_t);

  if ((dataOff != rConn.DataSize ())
      || (rConn.Stack ().Size () == 0))
    {
      status = WCS_INVALID_ARGS;
      goto cmd_read_exit;
    }

  try
    {
      rConn.DataSize (rConn.MaxSize ());
      dataOff = sizeof (uint32_t) + sizeof (uint16_t);

      StackValue&    topValue = rConn.Stack ()[rConn.Stack ().Size () - 1];
      const uint16_t valType  = topValue.GetOperand ().GetType ();

      if (IS_TABLE (valType))
        {
          I_DBSTable& table     = topValue.GetOperand ().GetTable ();
          FIELD_INDEX fieldHint = (fieldNameHint[0] != 0) ?
                                  table.GetFieldIndex (fieldNameHint) : 0;

          status = cmd_read_table_stack_top (rConn,
                                             topValue,
                                             fieldHint,
                                             rowHint,
                                             arrayHint,
                                             textHint,
                                             &dataOff);
        }
      else if (IS_FIELD (valType))
        {
          status = cmd_read_field_stack_top (rConn,
                                             topValue,
                                             rowHint,
                                             arrayHint,
                                             textHint,
                                             &dataOff);
        }
      else if (IS_ARRAY (valType))
        {
          assert ((valType & 0xFF) != WFT_TEXT);
          status = cmd_read_array_stack_top (rConn,
                                             topValue,
                                             textHint,
                                             &dataOff);
        }
      else if (GET_BASIC_TYPE (valType) == T_TEXT)
        {
          status = cmd_read_text_stack_top (rConn,
                                            topValue,
                                            textHint,
                                            &dataOff);
        }
      else
        status = cmd_read_basic_stack_top (rConn, topValue, &dataOff);

      store_le_int16 (valType, data + sizeof (uint32_t));
    }
  catch (DBSException& e)
  {
      const uint_t extra = e.GetExtra ();
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

  rConn.DataSize (dataOff);

cmd_read_exit:

  if (status != WCS_OK)
    rConn.DataSize (sizeof (uint32_t));

  store_le_int32 (status, rConn.Data());

  rConn.SendCmdResponse (CMD_READ_STACK_RSP);
}

static void
cmd_update_stack (ClientConnection& rConn)
{
  const uint8_t* const data    = rConn.Data ();
  uint32_t             status  = WCS_OK;
  uint_t               dataOff = 0;

  if (rConn.DataSize () == 0)
    {
      status = WCS_INVALID_ARGS;
      goto cmd_update_exit;
    }

  while (dataOff < rConn.DataSize ())
    {
      const uint8_t subcmd = data[dataOff++];
      switch (subcmd)
      {
      case CMD_UPDATE_FUNC_PUSH:
          status = cmd_push_stack (rConn, &dataOff);
          break;

      case CMD_UPDATE_FUNC_POP:
          status = cmd_pop_stack (rConn, &dataOff);
          break;

      case CMD_UPDATE_FUNC_CHTOP:
          status = cmd_update_stack_top (rConn, &dataOff);
          break;

      default:
        throw ConnectionException (
                               "Encountered unexpected update sub function.",
                               _EXTRA (subcmd)
                                  );
      }
      if (status != WCS_OK)
        goto cmd_update_exit;
    }

  assert (status == WCS_OK);
  assert (dataOff == rConn.DataSize ());

cmd_update_exit:
  rConn.DataSize (sizeof (status));
  store_le_int32 (status, rConn.Data());

  rConn.SendCmdResponse (CMD_UPDATE_STACK_RSP);
}

static void
cmd_execute_procedure (ClientConnection& rConn)
{
  const char* procName   = _RC (const char*, rConn.Data ());
  I_Session&    session    = *rConn.Dbs ().m_Session;
  SessionStack& stack      = rConn.Stack ();

  session.ExecuteProcedure (procName, stack);

  rConn.DataSize (sizeof (uint32_t));
  store_le_int32 (WCS_OK, rConn.Data ());
  rConn.SendCmdResponse (CMD_EXEC_PROC_RSP);
}

static void
cmd_ping_sever (ClientConnection& rConn)
{
  if (rConn.DataSize () != 0)
    {
      throw ConnectionException (
                              "Command to ping the sever has invalid format.",
                              _EXTRA (0)
                                );
    }

  rConn.DataSize (sizeof (uint32_t));
  store_le_int32 (WCS_OK, rConn.Data ());
  rConn.SendCmdResponse (CMD_PING_SERVER_RSP);
}

static void
cmd_list_globals (ClientConnection& rConn)
{
  uint32_t result = WCS_OK;

  if (rConn.DataSize () != sizeof (uint32_t))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  const I_Session& session = *rConn.Dbs ().m_Session;

  const uint32_t glbsCount  = session.GlobalValuesCount ();
  uint32_t       firstHint  = from_le_int32 (rConn.Data ());
  uint16_t       dataOffset = 0;
  bool           oneAtLeast = false;

  rConn.DataSize (rConn.MaxSize ());
  store_le_int32 (WCS_OK, rConn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (glbsCount, rConn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (firstHint, rConn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  assert (rConn.DataSize () > 3 * sizeof (uint32_t));

  if (firstHint >= glbsCount)
    {
      result = WCS_INVALID_ARGS;
      goto cmd_list_globals_err;
    }

  for (; firstHint < glbsCount; ++firstHint)
    {
      const char* pName   = session.GlobalValueName (firstHint);
      const uint_t  nameLen = strlen (_RC (const char*, pName)) + 1;

      if (dataOffset + nameLen >= rConn.DataSize())
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

      memcpy (rConn.Data () + dataOffset, pName, nameLen);
      dataOffset += nameLen;
    }

  assert (result == WCS_OK);
  assert (dataOffset <= rConn.DataSize ());
  assert (oneAtLeast);

  rConn.DataSize (dataOffset);
  rConn.SendCmdResponse (CMD_LIST_GLOBALS_RSP);

  return;

cmd_list_globals_err:
  assert (result != WCS_OK);

  rConn.DataSize (sizeof (result));
  store_le_int32 (result, rConn.Data ());
  rConn.SendCmdResponse (CMD_LIST_GLOBALS_RSP);

  return;
}

static void
cmd_list_procedures (ClientConnection& rConn)
{
  uint32_t result = WCS_OK;

  if (rConn.DataSize () != sizeof (uint32_t))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  const I_Session& session = *rConn.Dbs ().m_Session;

  const uint32_t procsCount  = session.ProceduresCount ();
  uint32_t       firstHint   = from_le_int32 (rConn.Data ());
  uint16_t       dataOffset  = 0;
  bool           oneAtLeast  = false;

  rConn.DataSize (rConn.MaxSize ());
  store_le_int32 (WCS_OK, rConn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (procsCount, rConn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  store_le_int32 (firstHint, rConn.Data () + dataOffset);
  dataOffset += sizeof (uint32_t);

  assert (rConn.DataSize () > 3 * sizeof (uint32_t));

  if (firstHint >= procsCount)
    {
      result = WCS_INVALID_ARGS;
      goto cmd_list_procedures_err;
    }

  for (; firstHint < procsCount; ++firstHint)
    {
      const char* name    = session.ProcedureName (firstHint);
      const uint_t  nameLen = strlen (_RC (const char*, name)) + 1;

      if (dataOffset + nameLen >= rConn.DataSize())
        {
          if (! oneAtLeast)
            {
              //This global variable name is too large.
              result = WCS_LARGE_ARGS;
              goto cmd_list_procedures_err;
            }
          else
            break;
        }

      oneAtLeast = true;

      memcpy (rConn.Data () + dataOffset, name, nameLen);
      dataOffset += nameLen;
    }

  assert (result == WCS_OK);
  assert (dataOffset <= rConn.DataSize ());
  assert (oneAtLeast);

  rConn.DataSize (dataOffset);
  rConn.SendCmdResponse (CMD_LIST_PROCEDURE_RSP);

  return;

cmd_list_procedures_err:
  assert (result != WCS_OK);

  rConn.DataSize (sizeof (result));
  store_le_int32 (result, rConn.Data ());
  rConn.SendCmdResponse (CMD_LIST_PROCEDURE_RSP);

  return;
}

static void
cmd_procedure_param_desc (ClientConnection& rConn)
{
  uint32_t result = WCS_OK;

  if (rConn.DataSize () < sizeof (uint16_t) + 2 * sizeof (uint8_t))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  I_Session&          session     = *rConn.Dbs ().m_Session;
  uint8_t*            data_       = rConn.Data ();
  uint16_t            hint        = from_le_int16 (data_);
  const char* const procName    = _RC (const char*,
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

  rConn.DataSize (rConn.MaxSize ());
  if (2 * sizeof (uint32_t) + procNameLen > rConn.DataSize ())
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
      do
        {
          const uint_t paramType = session.ProcedurePameterRawType (procName,
                                                                    hint);
          if (IS_TABLE (paramType))
            {
              if (offset + sizeof (uint16_t) > rConn.DataSize ())
                break;

              store_le_int16 (WFT_TABLE_MASK, data_ + offset);
              offset += sizeof (uint16_t);

              const uint_t fieldsCount = session.ProcedurePameterFieldsCount (
                                                                      procName,
                                                                      hint
                                                                             );
              if ((fieldsCount > 0xFFFF)
                  || (offset + sizeof (uint16_t) >= rConn.DataSize ()))
                {
                  break;
                }

              store_le_int16 (fieldsCount, data_ + offset);
              offset += sizeof (uint16_t);

              for (uint_t field = 0; field < fieldsCount; field++)
                {
                  const char* fieldName = _RC (
                                          const char*,
                                          session.ProcedurePameterFieldName (
                                                                    procName,
                                                                    hint,
                                                                    field
                                                                           )
                                                );
                  const uint16_t fieldType =
                      session.ProcedurePameterFieldType (procName,
                                                         hint,
                                                         field);
                  const uint_t fieldLen = strlen (fieldName) + 1;
                  if (offset + fieldLen + sizeof (uint16_t) > rConn.DataSize ())
                    break;

                  strcpy (_RC (char*, data_ + offset), fieldName);
                  offset += fieldLen;

                  store_le_int16 (fieldType, data_ + offset);
                  offset += sizeof (uint16_t);
                }
            }
          else
            {
              if (offset + sizeof (uint16_t) > rConn.DataSize ())
                break;

              store_le_int16 (paramType, data_ + offset);
              offset += sizeof (uint16_t);
            }

          ++hint;
          oneAtLeast = true;
        }
      while (hint < paramsCount);

      if (! oneAtLeast)
        {
          result = WCS_LARGE_ARGS;
          goto cmd_procedure_param_desc_err;
        }
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
  rConn.DataSize (offset); //Send only what it's needed!
  rConn.SendCmdResponse (CMD_DESC_PROC_PARAM_RSP);

  return;

cmd_procedure_param_desc_err:
  assert (result != WCS_OK);

  rConn.DataSize (sizeof (result));
  store_le_int32 (result, rConn.Data ());
  rConn.SendCmdResponse (CMD_DESC_PROC_PARAM_RSP);

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

COMMAND_HANDLER* gpAdminCommands = saAdminCmds;
COMMAND_HANDLER* gpUserCommands  = saUserCmds;
