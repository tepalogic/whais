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
cmd_invalid (ClientConnection&, const bool)
{
  throw ConnectionException ("Invalid command received.",
                             _EXTRA (0));
}

static void
cmd_unspp (ClientConnection&, const bool)
{
  throw ConnectionException ("Unsupported supported command.", _EXTRA (0));
}

static void
cmd_read_stack (ClientConnection& conn, const bool receivedAll)
{
  cmd_unspp (conn, receivedAll);
}

static void
cmd_update_stack (ClientConnection& rConn, const bool receivedAll)
{
  const D_UINT8* const data    = rConn.Data ();
  D_UINT32             status  = CS_OK;
  D_UINT16             dataOff = 0;

  if (rConn.DataSize () == 0)
    {
      status = CS_INVALID_ARGS;
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
      if (status != CS_OK)
        goto cmd_update_exit;
    }

  assert (status == CS_OK);
  assert (dataOff == rConn.DataSize ());

cmd_update_exit:
  rConn.DataSize (sizeof (status));
  store_le_int32 (status, rConn.Data());

  bool dummy;
  rConn.SendCmdResponse (CMD_UPDATE_STACK_RSP, true, dummy);
}

static void
cmd_ping_sever (ClientConnection& rConn, const bool receivedAll)
{
  if ((rConn.DataSize () != 0) || (receivedAll == false))
    {
      throw ConnectionException (
                              "Command to ping the sever has invalid format.",
                              _EXTRA (0)
                                );
    }

  bool dummy;
  rConn.DataSize (0);
  rConn.SendCmdResponse (CMD_PING_SERVER_RSP, true, dummy);

  assert (dummy == false);
}

static void
cmd_list_glbs (ClientConnection& rConn, const bool receivedAll)
{
  if ((rConn.DataSize () != 0) || (receivedAll == false))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              " variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  const I_Session& session = *rConn.Dbs ().m_Session;

  const D_UINT32 glbsCount  = session.GlobalValuesCount ();
  D_UINT8        namesCount = 0;
  bool           firstResp  = true;
  bool           sendNext;

  rConn.DataSize (sizeof (D_UINT8) + sizeof (D_UINT32) + sizeof (D_UINT8));
  for (D_UINT32 glbIndex = 0; glbIndex < glbsCount; ++glbIndex)
    {
      const D_UINT8* pName    = session.GlobalValueName (glbIndex);
      const D_UINT   nameSize = strlen (_RC (const D_CHAR*, pName)) + 1;

      if (((nameSize + rConn.DataSize ()) < rConn.MaxSize())
          && (namesCount < 255))
        {
          D_UINT8* destination = rConn.Data () + rConn.DataSize ();
          memcpy (destination, pName, nameSize);
          rConn.DataSize (rConn.DataSize () + nameSize);
          ++namesCount;
          continue;
        }

      if (namesCount == 0)
        goto cmd_list_toobig_err;

      rConn.Data ()[0] = CS_OK;
      if (firstResp)
        {
          store_le_int32 (glbsCount, rConn.Data () + sizeof (D_UINT8));
          rConn.Data ()[sizeof (D_UINT32) + sizeof (D_UINT8)] = namesCount;
        }
      else
        rConn.Data ()[sizeof (D_UINT8)] = namesCount;

      rConn.SendCmdResponse (CMD_LIST_GLOBALS_RSP, false, sendNext);
      if ( ! sendNext)
        goto cmd_list_cancel_op;

      assert (glbIndex > 0);
      --glbIndex;

      namesCount = 0;
      firstResp  = false;
      rConn.DataSize (sizeof (D_UINT8) + sizeof (D_UINT8));
    }

  rConn.Data ()[0] = CS_OK;
  if (firstResp)
    {
      store_le_int32 (glbsCount, rConn.Data () + sizeof (D_UINT8));
      rConn.Data ()[sizeof (D_UINT32) + sizeof (D_UINT8)] = namesCount;
    }
  else
    rConn.Data ()[sizeof (D_UINT8)] = namesCount;

  rConn.SendCmdResponse (CMD_LIST_GLOBALS_RSP, true, sendNext);
  assert (sendNext == false);
  return;

cmd_list_toobig_err:
  rConn.DataSize (1);
  rConn.Data ()[0] = CS_LARGE_ARGS;
  rConn.SendCmdResponse (CMD_LIST_GLOBALS_RSP, true, sendNext);

cmd_list_cancel_op:
  assert (sendNext == false);

  return;
}

static void
cmd_glb_desc (ClientConnection& rConn, const bool receivedAll)
{
  bool sendNext = false;
  if (rConn.DataSize () < (sizeof (D_UINT16) + 1))
    {
      throw ConnectionException (
                              "Command used to retrieve description of global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  else if (receivedAll == false)
    {
      rConn.AckCommandPart (false);
      throw ConnectionException (
                              "Unable to find the description of a global "
                              "variable because its name is too large.",
                              _EXTRA (0)
                                );
    }

  D_UINT8* const data_    = rConn.Data ();
  const D_UINT16 nameSize = from_le_int16 (data_);

  assert ((nameSize + sizeof (D_UINT16)) == rConn.DataSize());

  const I_Session& rSession = *rConn.Dbs ().m_Session;
  const D_UINT8*   pGlbTI   = rSession.GlobalValueType (
                                                    data_ + sizeof (D_UINT16)
                                                       );
  if (pGlbTI == NULL)
    {
      rConn.DataSize (sizeof (D_UINT8));
      data_[0] = CS_INVALID_ARGS;
      rConn.SendCmdResponse (CMD_GLOBAL_DESC_RSP, true, sendNext);

      assert (sendNext == false);
      return;
    }

  const D_UINT dataSize =
      from_le_int16 (pGlbTI + sizeof (D_UINT16)) + 2 * sizeof (D_UINT16);
  assert (dataSize > sizeof (D_UINT16));

  D_UINT progress = 0;

  while (progress < dataSize)
    {
      D_UINT chunkSize = rConn.MaxSize () -
                         (sizeof (D_UINT16) + sizeof (D_UINT8));
      if (chunkSize > (dataSize - progress))
        chunkSize = (dataSize - progress);

      rConn.DataSize (sizeof (D_UINT8) + sizeof (D_UINT16) + chunkSize);
      data_[0] = CS_OK;
      store_le_int16 (chunkSize, &data_[sizeof(D_UINT8)]);
      memcpy (&data_[sizeof (D_UINT8) + sizeof (D_UINT16)],
              pGlbTI + progress,
              chunkSize);
      progress += chunkSize;

      assert (progress <= dataSize);

      rConn.SendCmdResponse (CMD_GLOBAL_DESC_RSP,
                            (progress >= dataSize),
                            sendNext);
      if (! sendNext)
        break;
    }
}

static COMMAND_HANDLER saAdminCmds[] =
    {
        cmd_invalid,                     // CMD_INVALID
        cmd_list_glbs,                   // CMD_LIST_GLOBALS
        cmd_glb_desc                     // CMD_GLOBAL_DESC
    };

static COMMAND_HANDLER saUserCmds[] =
    {
        cmd_invalid,                     // CMD_CLOSE_CONN
        cmd_read_stack,                        // CMD_READ_STACK
        cmd_update_stack,                      // CMD_UPDATE_STACK
        cmd_ping_sever                   // CMD_PING_SERVER
    };

COMMAND_HANDLER* gpAdminCommands = saAdminCmds;
COMMAND_HANDLER* gpUserCommands  = saUserCmds;
