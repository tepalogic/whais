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
  D_UINT32 result = WCS_OK;

  D_UINT8*       data_     = rConn.Data ();
  const D_CHAR*  glbName   = _RC (const D_CHAR*, data_ + sizeof (D_UINT32));
  D_UINT32       fieldHint = from_le_int16 (data_);
  I_Operand*     op        = NULL;
  D_UINT16       dataOffset = sizeof (D_UINT32) + strlen (glbName) + 1;

  D_UINT         rawType;

  if (rConn.DataSize () <  + sizeof (D_UINT16) + strlen (glbName) + 1)
    {
      throw ConnectionException (
                              "Command used to retrieve description of global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }

  rConn.DataSize (rConn.MaxSize ());

  if (strlen (glbName) > 0)
    {
      if (! rConn.IsAdmin ())
        {
          throw ConnectionException (
                                  "Only and admin may request for a global "
                                  "value description.",
                                  _EXTRA (0)
                                    );
        }
      try
      {
          op = &rConn.Dbs ().m_Session->GlobalValueOp (
                                                _RC (const D_UINT8*, glbName)
                                                      );
      }
      catch (InterException& e)
      {
          if (e.GetExtra () != InterException::INVALID_GLOBAL_REQ)
            throw;

          result = WCS_INVALID_ARGS;
          goto cmd_glb_desc_err;
      }
    }
  else
    {
      SessionStack& stack = rConn.Stack ();

      if (stack.Size() == 0)
        {
          result = WCS_INVALID_ARGS;
          goto cmd_glb_desc_err;
        }

      op = &stack[stack.Size () - 1].GetOperand ();
    }

  assert (op != NULL);

  store_le_int32 (WCS_OK, data_);

  rawType = op->GetType ();
  store_le_int16 (rawType, data_ + dataOffset);
  dataOffset += sizeof (D_UINT16);

  if (IS_TABLE (rawType))
    {
      I_DBSTable& table = op->GetTable();
      const FIELD_INDEX fieldCount = table.GetFieldsCount ();
      if (fieldHint >= fieldCount)
        {
          result = WCS_INVALID_ARGS;
          goto cmd_glb_desc_err;
        }
      else if (fieldCount > 0xFFFF)
        {
          result = WCS_LARGE_ARGS;
          goto cmd_glb_desc_err;
        }
      store_le_int16 (fieldCount, data_ + dataOffset);
      dataOffset += sizeof (D_UINT16);

      store_le_int16 (fieldHint, data_ + dataOffset);
      dataOffset += sizeof (D_UINT16);

      bool oneAtLeast = false;
      do
        {
          DBSFieldDescriptor fd        = table.GetFieldDescriptor (fieldHint);
          const D_UINT       fieldLen  = strlen (fd.m_pFieldName) + 1;
          D_UINT16           fieldType = fd.m_FieldType;

          if (dataOffset + fieldLen + sizeof (D_UINT16) < rConn.DataSize ())
            break;

          strcpy (_RC (D_CHAR*, data_ + dataOffset), fd.m_pFieldName);
          dataOffset += fieldLen;

          if (fd.isArray)
            MARK_ARRAY (fieldType);

          store_le_int16 (fieldType, data_ + dataOffset);
          dataOffset += sizeof (D_UINT16);

          oneAtLeast = true;
        }
      while (fieldHint < fieldCount);

      if (! oneAtLeast)
        {
          result = WCS_INVALID_ARGS;
          goto cmd_glb_desc_err;
        }
    }
  else if (fieldHint > 0)
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
  rConn.DataSize (sizeof (D_UINT32));
  rConn.SendCmdResponse (CMD_GLOBAL_DESC_RSP);
}

static void
cmd_read_stack (ClientConnection& rConn)
{
  const D_UINT8* const data    = rConn.Data ();
  D_UINT32             status  = WCS_OK;
  D_UINT16             dataOff = 0;

  const D_UINT64 rowHint = from_le_int64 (data + dataOff);
  dataOff += sizeof (D_UINT64);

  const D_UINT64 elHint  = from_le_int64 (data + dataOff);
  dataOff += sizeof (D_UINT64);

  const D_CHAR* const fieldNameHint = _RC (const D_CHAR*, data + dataOff);
  dataOff += strlen (fieldNameHint) + 1;

  if ((dataOff > rConn.DataSize ())
      || (rConn.Stack ().Size () == 0))
    {
      status = WCS_INVALID_ARGS;
      goto cmd_read_exit;
    }

  try
    {
      dataOff = sizeof (D_UINT32);

      StackValue&    topValue = rConn.Stack ()[rConn.Stack ().Size () - 1];
      const D_UINT16 valType = topValue.GetOperand ().GetType ();

      if (IS_TABLE (valType))
        {
          I_DBSTable& table     = topValue.GetOperand ().GetTable ();
          FIELD_INDEX fieldHint = (fieldNameHint[0] != 0) ?
                                  table.GetFieldIndex (fieldNameHint) : 0;

          status = cmd_read_table_stack_top (rConn,
                                             topValue,
                                             fieldHint,
                                             rowHint,
                                             elHint,
                                             &dataOff);
        }
      else if (IS_FIELD (valType))
        {
          status = cmd_read_field_stack_top (rConn,
                                             topValue,
                                             rowHint,
                                             elHint,
                                             &dataOff);
        }
      else if (IS_ARRAY (valType))
        status = cmd_read_array_stack_top (rConn, topValue, elHint, &dataOff);
      else if (GET_BASIC_TYPE (valType) == T_TEXT)
        status = cmd_read_text_stack_top (rConn, topValue, elHint, &dataOff);
      else
        status = cmd_read_basic_stack_top (rConn, topValue, &dataOff);
    }
  catch (DBSException& e)
  {
      if ((e.GetExtra () == DBSException::FIELD_NOT_FOUND)
          || (e.GetExtra () == DBSException::FIELD_NOT_INDEXED)
          || (e.GetExtra () == DBSException::ARRAY_INDEX_TOO_BIG)
          || (e.GetExtra () == DBSException::ROW_NOT_ALLOCATED)
          || (e.GetExtra () == DBSException::STRING_INDEX_TOO_BIG))
        {
          status = WCS_INVALID_ARGS;
        }
      else
        throw ;
  }

  assert (dataOff >= sizeof (D_UINT32));
  rConn.DataSize (dataOff);

cmd_read_exit:

  if (status != WCS_OK)
    rConn.DataSize (sizeof (D_UINT32));

  store_le_int32 (status, rConn.Data());

  rConn.SendCmdResponse (CMD_READ_STACK_RSP);
}

static void
cmd_update_stack (ClientConnection& rConn)
{
  const D_UINT8* const data    = rConn.Data ();
  D_UINT32             status  = WCS_OK;
  D_UINT16             dataOff = 0;

  if (rConn.DataSize () == 0)
    {
      status = WCS_INVALID_ARGS;
      goto cmd_update_exit;
    }

  while (dataOff < rConn.DataSize ())
    {
      const D_UINT8 subcmd = data[dataOff++];
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

  rConn.DataSize (sizeof (D_UINT32));
  store_le_int32 (WCS_OK, rConn.Data ());
  rConn.SendCmdResponse (CMD_PING_SERVER_RSP);
}

static void
cmd_list_globals (ClientConnection& rConn)
{
  D_UINT32 result = WCS_OK;

  if (rConn.DataSize () != sizeof (D_UINT32))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  const I_Session& session = *rConn.Dbs ().m_Session;

  const D_UINT32 glbsCount  = session.GlobalValuesCount ();
  D_UINT32       firstHint  = from_le_int32 (rConn.Data ());
  D_UINT16       dataOffset = 0;
  bool           oneAtLeast = false;

  rConn.DataSize (rConn.MaxSize ());
  store_le_int32 (WCS_OK, rConn.Data () + dataOffset);
  dataOffset += sizeof (D_UINT32);

  store_le_int32 (glbsCount, rConn.Data () + dataOffset);
  dataOffset += sizeof (D_UINT32);

  store_le_int32 (firstHint, rConn.Data () + dataOffset);
  dataOffset += sizeof (D_UINT32);

  assert (rConn.DataSize () > 3 * sizeof (D_UINT32));

  if (firstHint >= glbsCount)
    {
      result = WCS_INVALID_ARGS;
      goto cmd_list_globals_err;
    }

  for (; firstHint < glbsCount; ++firstHint)
    {
      const D_UINT8* pName   = session.GlobalValueName (firstHint);
      const D_UINT   nameLen = strlen (_RC (const D_CHAR*, pName)) + 1;

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
  D_UINT32 result = WCS_OK;

  if (rConn.DataSize () != sizeof (D_UINT32))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  const I_Session& session = *rConn.Dbs ().m_Session;

  const D_UINT32 procsCount  = session.ProceduresCount ();
  D_UINT32       firstHint   = from_le_int32 (rConn.Data ());
  D_UINT16       dataOffset  = 0;
  bool           oneAtLeast  = false;

  rConn.DataSize (rConn.MaxSize ());
  store_le_int32 (WCS_OK, rConn.Data () + dataOffset);
  dataOffset += sizeof (D_UINT32);

  store_le_int32 (procsCount, rConn.Data () + dataOffset);
  dataOffset += sizeof (D_UINT32);

  store_le_int32 (firstHint, rConn.Data () + dataOffset);
  dataOffset += sizeof (D_UINT32);

  assert (rConn.DataSize () > 3 * sizeof (D_UINT32));

  if (firstHint >= procsCount)
    {
      result = WCS_INVALID_ARGS;
      goto cmd_list_procedures_err;
    }

  for (; firstHint < procsCount; ++firstHint)
    {
      const D_UINT8* pName   = session.ProcedureName (firstHint);
      const D_UINT   nameLen = strlen (_RC (const D_CHAR*, pName)) + 1;

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

      memcpy (rConn.Data () + dataOffset, pName, nameLen);
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
  D_UINT32 result = WCS_OK;

  if (rConn.DataSize () < sizeof (D_UINT16) + 2 * sizeof (D_UINT8))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  const I_Session&     session     = *rConn.Dbs ().m_Session;
  D_UINT8*             data_       = rConn.Data ();
  D_UINT16             hint        = from_le_int16 (data_);
  const D_UINT8* const procName    = data_ + sizeof (D_UINT16);
  D_UINT16             offset      = 0;
  bool                 oneAtLeast  = false;
  const D_UINT         paramsCount = session.ProcedureParametersCount (procName);

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
  store_le_int32 (WCS_OK, data_);
  offset += sizeof (D_UINT32);

  //Leave the name part unchanged!
  offset += strlen (_RC (const D_CHAR*, procName)) + 1;

  store_le_int16 (paramsCount, data_ + offset);
  offset += sizeof (D_UINT16);

  store_le_int16 (hint, data_ + offset);
  offset += sizeof (D_UINT16);

  do
    {
      I_Operand& op = session.ProcedureParameterOp (procName, hint);
      if (IS_TABLE (op.GetType ()))
        {
          I_DBSTable&  table       = op.GetTable ();
          const D_UINT fieldsCount = table.GetFieldsCount();

          if ((fieldsCount > 0xFFFF)
              || (offset + sizeof (D_UINT16) >= rConn.DataSize ()))
            {
              break;
            }

          store_le_int16 (fieldsCount, data_ + offset);
          offset += sizeof (D_UINT16);

          for (D_UINT field = 0; field < table.GetFieldsCount(); field++)
            {
              const DBSFieldDescriptor fd = table.GetFieldDescriptor (field);

              const D_UINT fieldLen  = strlen (fd.m_pFieldName) + 1;
              D_UINT16     fieldType = fd.m_FieldType;

              if (offset + fieldLen + sizeof (D_UINT16) > rConn.DataSize ())
                break;

              strcpy (_RC (D_CHAR*, data_ + offset), fd.m_pFieldName);
              offset += fieldLen;

              if (fd.isArray)
                MARK_ARRAY (fieldType);

              store_le_int16 (fieldType, data_ + offset);
              offset += sizeof (D_UINT16);
            }
        }
      else
        {
          if (offset + sizeof (D_UINT32) > rConn.DataSize ())
            break;

          store_le_int16 (op.GetType (), data_ + offset);
          offset += sizeof (D_UINT16);
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

  assert (result != WCS_OK);
  rConn.SendCmdResponse (CMD_LIST_PROCEDURE_RSP);

  return;

cmd_procedure_param_desc_err:
  assert (result != WCS_OK);

  rConn.DataSize (sizeof (result));
  store_le_int32 (result, rConn.Data ());
  rConn.SendCmdResponse (CMD_LIST_PROCEDURE_RSP);

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
        cmd_value_desc,                     // CMD_GLOBAL_DESC
        cmd_read_stack,                  // CMD_READ_STACK
        cmd_update_stack,                // CMD_UPDATE_STACK
        cmd_execute_procedure,           // CMD_EXEC_PROC
        cmd_ping_sever                   // CMD_PING_SERVER
    };

COMMAND_HANDLER* gpAdminCommands = saAdminCmds;
COMMAND_HANDLER* gpUserCommands  = saUserCmds;
