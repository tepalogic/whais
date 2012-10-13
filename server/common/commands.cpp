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
cmd_ping_sever (ClientConnection& conn, const bool receivedAll)
{
  if ((conn.Size () != 0) || (receivedAll == false))
    {
      throw ConnectionException (
                              "Command to ping the sever has invalid format.",
                              _EXTRA (0)
                                );
    }

  bool dummy;
  conn.Size (0);
  conn.SendCmdResponse (CMD_PING_SERVER_RSP, true, dummy);

  assert (dummy == false);
}

static void
cmd_list_glbs (ClientConnection& conn, const bool receivedAll)
{
  if ((conn.Size () != 0) || (receivedAll == false))
    {
      throw ConnectionException (
                              "Command used to retrieve context global "
                              " variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  const I_Session& session = *conn.Dbs ().m_Session;

  const D_UINT32 glbsCount  = session.GlobalValuesCount ();
  D_UINT8        namesCount = 0;
  bool           firstResp  = true;
  bool           sendNext;

  conn.Size (sizeof (D_UINT8) + sizeof (D_UINT32) + sizeof (D_UINT8));
  for (D_UINT32 glbIndex = 0; glbIndex < glbsCount; ++glbIndex)
    {
      const D_UINT8* pName    = session.GlobalValueName (glbIndex);
      const D_UINT   nameSize = strlen (_RC (const D_CHAR*, pName)) + 1;

      if (((nameSize + conn.Size ()) < conn.MaxSize())
          && (namesCount < 255))
        {
          D_UINT8* pData = conn.Data () + conn.Size ();
          memcpy (pData, pName, nameSize);
          conn.Size (conn.Size () + nameSize);
          ++namesCount;
          continue;
        }

      if (namesCount == 0)
        goto cmd_list_toobig_err;

      conn.Data ()[0] = CMD_STATUS_OK;
      if (firstResp)
        {
          store_le_int32 (glbsCount, conn.Data () + sizeof (D_UINT8));
          conn.Data ()[sizeof (D_UINT32) + sizeof (D_UINT8)] = namesCount;
        }
      else
        conn.Data ()[sizeof (D_UINT8)] = namesCount;

      conn.SendCmdResponse (CMD_LIST_GLOBALS_RSP, false, sendNext);
      if ( ! sendNext)
        goto cmd_list_cancel_op;

      assert (glbIndex > 0);
      --glbIndex;

      namesCount = 0;
      firstResp  = false;
      conn.Size (sizeof (D_UINT8) + sizeof (D_UINT8));
    }

  conn.Data ()[0] = CMD_STATUS_OK;
  if (firstResp)
    {
      store_le_int32 (glbsCount, conn.Data () + sizeof (D_UINT8));
      conn.Data ()[sizeof (D_UINT32) + sizeof (D_UINT8)] = namesCount;
    }
  else
    conn.Data ()[sizeof (D_UINT8)] = namesCount;

  conn.SendCmdResponse (CMD_LIST_GLOBALS_RSP, true, sendNext);
  assert (sendNext == false);
  return;

cmd_list_toobig_err:
  conn.Size (1);
  conn.Data ()[0] = CMD_STATUS_TOOBIG;
  conn.SendCmdResponse (CMD_LIST_GLOBALS_RSP, true, sendNext);

cmd_list_cancel_op:
  assert (sendNext == false);

  return;
}

static void
cmd_glb_desc (ClientConnection& conn, const bool receivedAll)
{
  bool sendNext = false;
  if (conn.Size () < (sizeof (D_UINT16) + 1))
    {
      throw ConnectionException (
                              "Command used to retrieve description of global "
                              "variables has invalid format.",
                              _EXTRA (0)
                                );
    }
  else if (receivedAll == false)
    {
      conn.AckCommandPart (false);
      throw ConnectionException (
                              "Unable to find the description of a global "
                              "variable because its name is too large.",
                              _EXTRA (0)
                                );
    }

  D_UINT8* const pData    = conn.Data ();
  const D_UINT16 nameSize = from_le_int16 (pData);

  assert ((nameSize + sizeof (D_UINT16)) == conn.Size());

  const I_Session& session = *conn.Dbs ().m_Session;
  const D_UINT8*   pGlbTI  = session.GlobalValueType (
                                          pData + sizeof (D_UINT16)
                                                     );
  if (pGlbTI == NULL)
    {
      conn.Size (sizeof (D_UINT8));
      pData[0] = CMD_STATUS_INVAL_ARGS;
      conn.SendCmdResponse (CMD_GLOBAL_DESC_RSP, true, sendNext);

      assert (sendNext == false);
      return;
    }

  const D_UINT dataSize =
      from_le_int16 (pGlbTI + sizeof (D_UINT16)) + 2 * sizeof (D_UINT16);
  assert (dataSize > sizeof (D_UINT16));

  D_UINT progress = 0;

  while (progress < dataSize)
    {
      D_UINT chunkSize = conn.MaxSize () -
                         (sizeof (D_UINT16) + sizeof (D_UINT8));
      if (chunkSize > (dataSize - progress))
        chunkSize = (dataSize - progress);

      conn.Size (sizeof (D_UINT8) + sizeof (D_UINT16) + chunkSize);
      pData[0] = CMD_STATUS_OK;
      store_le_int16 (chunkSize, &pData[sizeof(D_UINT8)]);
      memcpy (&pData[sizeof (D_UINT8) + sizeof (D_UINT16)],
              pGlbTI + progress,
              chunkSize);
      progress += chunkSize;

      assert (progress <= dataSize);

      conn.SendCmdResponse (CMD_GLOBAL_DESC_RSP,
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
        cmd_ping_sever                   // CMD_PING_SERVER
    };

COMMAND_HANDLER* gpAdminCommands = saAdminCmds;
COMMAND_HANDLER* gpUserCommands  = saUserCmds;
