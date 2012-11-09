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

#include <assert.h>
#include <memory.h>

#include "whisper.h"

#include "server/include/server_protocol.h"
#include "utils/include/le_converter.h"
#include "utils/include/random.h"

#include "connection.h"

static const D_UINT32 FRAME_SERVER_VERS  = 0x00000001;

using namespace std;

ConnectionException::ConnectionException (const D_CHAR* pMessage,
                                          const D_CHAR* pFile,
                                          D_UINT32      line,
                                          D_UINT32      extra)
  : WException (pMessage, pFile, line, extra)
{
}

WException*
ConnectionException::Clone () const
{
  ConnectionException* pResult = new ConnectionException (*this);

  return pResult;
}

EXPCEPTION_TYPE
ConnectionException::Type () const
{
  return CONNECTION_EXCEPTION;
}

const D_CHAR*
ConnectionException::Description () const
{
  return NULL;
}

ClientConnection::ClientConnection (UserHandler&            client,
                                    vector<DBSDescriptors>& databases)
  : m_UserHandler (client),
    m_Stack (),
    m_WaitingFrameId (0),
    m_ClientCookie (0),
    m_ServerCookie (0),
    m_LastReceivedCmd (CMD_INVALID),
    m_FrameSize (0),
    m_EncriptionType (FRAME_ENCTYPE_PLAIN),
    m_Version (1)
{
  m_UserHandler.m_pDesc = NULL;
  const D_UINT16 authFrameLen = FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA;

  memset (&m_Data[0], 0, authFrameLen);

  m_Data[FRAME_ENCTYPE_OFF] = m_EncriptionType;
  m_Data[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT;
  store_le_int32 (0, &m_Data[FRAME_ID_OFF]);
  store_le_int32 (m_Version, &m_Data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_VER]);
  store_le_int16 (authFrameLen, &m_Data[FRAME_SIZE_OFF]);

  m_UserHandler.m_Socket.Write (authFrameLen, &m_Data[0]);
  ReciveRawClientFrame ();

  const D_UINT32 protocolVer = from_le_int32 (
                              &m_Data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_VER]
                                             );
  if ((m_FrameSize < authFrameLen)
      || (protocolVer != m_Version)
      || (m_Data[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT_RSP)
      || (m_Data[FRAME_ENCTYPE_OFF] != FRAME_ENCTYPE_PLAIN))
    {
      throw ConnectionException ("Unexpected authentication frame received.",
                                 _EXTRA (0));
    }

  const string dbsName = _RC (const D_CHAR*,
                              &m_Data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA]);

  for (vector<DBSDescriptors>::iterator it = databases.begin ();
      it != databases.end ();
      ++it)
    {
      if (it->m_DbsName == dbsName)
        {
          m_UserHandler.m_pDesc = &it[0];
          break;
        }
    }

  if (m_UserHandler.m_pDesc == NULL)
    {
      throw ConnectionException ("Failed to retrieve database.",
                                 _EXTRA (0));
    }

  m_UserHandler.m_Root = (m_Data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_USR] == 0) ?
                         true :
                         false;

  const string passwd = _RC (
      const D_CHAR*,
      &m_Data[FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA + dbsName.size () + 1]
                            );

  if (m_UserHandler.m_Root)
    {
      if (m_UserHandler.m_pDesc->m_RootPass != passwd)
        {
          throw ConnectionException (
                            "Failed to authenticate database root user.",
                            _EXTRA (0)
                                    );

        }
    }
  else
    {
      if (m_UserHandler.m_pDesc->m_UserPasswd != passwd)
        {
          throw ConnectionException (
                                "Failed to authenticate database user.",
                                _EXTRA (0)
                                    );
        }
    }
}

ClientConnection::~ClientConnection ()
{
}

D_UINT16
ClientConnection::MaxSize () const
{
  assert (m_EncriptionType == FRAME_ENCTYPE_PLAIN);
  return sizeof (m_Data) - (FRAME_DATA_OFF + PLAIN_DATA_OFF);
}

D_UINT16
ClientConnection::DataSize () const
{
  assert ((m_FrameSize == 0)
          || (m_FrameSize >= (FRAME_DATA_OFF + PLAIN_DATA_OFF)));

  assert (m_FrameSize <= sizeof (m_Data));

  return (m_FrameSize == 0) ?
         0 :
         (m_FrameSize - (FRAME_DATA_OFF + PLAIN_DATA_OFF));
}

D_UINT8*
ClientConnection::Data ()
{
  return &m_Data[FRAME_DATA_OFF + PLAIN_DATA_OFF];
}

void
ClientConnection::DataSize (const D_UINT16 size)
{
  assert (size <= MaxSize ());

  m_FrameSize = size + FRAME_DATA_OFF + PLAIN_DATA_OFF;

  assert (m_FrameSize <= sizeof (m_Data));
}

D_UINT8*
ClientConnection::RawCmdData ()
{
  return &m_Data[FRAME_DATA_OFF];
}

void
ClientConnection::ReciveRawClientFrame ()
{
  D_UINT16 frameRead   = 0;

  while (frameRead < FRAME_DATA_OFF)
    {
      D_UINT16 chunkSize;
      chunkSize = m_UserHandler.m_Socket.Read (FRAME_DATA_OFF - frameRead,
                                               &m_Data[frameRead]);
      if (chunkSize == 0)
        throw ConnectionException ("Connection reset by peer.", _EXTRA (0));

      frameRead += chunkSize;
    }

  m_FrameSize = from_le_int16 (&m_Data[0]);

  if ((m_FrameSize < frameRead)
      || (m_FrameSize > FRAME_MAX_SIZE))
    {
      throw ConnectionException ("Invalid frame received.", _EXTRA (0));
    }

  while (frameRead < m_FrameSize)
    {
      D_UINT16 chunkSize;
      chunkSize = m_UserHandler.m_Socket.Read (m_FrameSize - frameRead,
                                               &m_Data[frameRead]);
      if (chunkSize == 0)
        throw ConnectionException ("Connection reset by peer.", _EXTRA (0));

      frameRead += chunkSize;
    }

  assert (frameRead == m_FrameSize);

  const D_UINT32 frameId = from_le_int32 (&m_Data[FRAME_ID_OFF]);
  if (frameId != m_WaitingFrameId)
    {
      throw ConnectionException ("Connection with peer is out of sync",
                                 _EXTRA (0));
    }

  switch (m_Data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
  case FRAME_TYPE_PARTIAL:
  case FRAME_TYPE_PARTIAL_ACK:
  case FRAME_TYPE_PARTIAL_CANCEL:
  case FRAME_TYPE_AUTH_CLNT_RSP:
    //These are legitimate frame types that could be received by a server.
    break;
  case FRAME_TYPE_TIMEOUT:
    throw ConnectionException ("Client peer has signaled a timeout condition.",
                               _EXTRA (0));
    break;
  default:
    assert (false);
    throw ConnectionException ("Unexpected frame type received.", _EXTRA (0));
  }
}

void
ClientConnection::SendRawClientFrame (const D_UINT8 type)
{
  assert ((m_FrameSize >= FRAME_DATA_OFF) && (m_FrameSize <= FRAME_MAX_SIZE));

  store_le_int16 (m_FrameSize, &m_Data[FRAME_SIZE_OFF]);
  store_le_int32 (++m_WaitingFrameId, &m_Data[FRAME_ENCTYPE_OFF]);

  m_Data[FRAME_ENCTYPE_OFF] = m_EncriptionType;
  m_Data[FRAME_TYPE_OFF]    = type;

  m_UserHandler.m_Socket.Write (m_FrameSize, m_Data);

  m_FrameSize    = 0; //This frame content is not valid anymore.
  m_ClientCookie = ~0; //Make the client cookie is reread.
}

D_UINT32
ClientConnection::ReadCommand (bool* pLastPart)
{
  ReciveRawClientFrame ();

  const D_UINT32 servCookie = from_le_int32 (
                                RawCmdData () + PLAIN_SERV_COOKIE_OFF
                                            );
  if (servCookie != m_ServerCookie)
    {
      throw ConnectionException ("Peer context cannot be recognized.",
                                 _EXTRA (0));
    }

  m_ClientCookie = from_le_int32 (RawCmdData () + PLAIN_CLNT_COOKIE_OFF);

  switch (m_Data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
    *pLastPart = true;
    break;
  case FRAME_TYPE_PARTIAL:
    *pLastPart = false;
    break;
  default:
    throw ConnectionException ("Connection with peer is out of sync.",
                               _EXTRA (0));
  }

  m_LastReceivedCmd = from_le_int16 (RawCmdData () + PLAIN_TYPE_OFF);
  assert ((m_LastReceivedCmd & 1) == 0);

  return m_LastReceivedCmd;
}

void
ClientConnection::AckCommandPart (const bool waitingNext)
{
  DataSize (0);

  m_ServerCookie = w_rnd ();

  assert (m_LastReceivedCmd != CMD_INVALID);

  store_le_int32 (m_ClientCookie, RawCmdData () + PLAIN_CLNT_COOKIE_OFF);
  store_le_int32 (m_ServerCookie, RawCmdData () + PLAIN_SERV_COOKIE_OFF);
  store_le_int16 (m_LastReceivedCmd, RawCmdData () + PLAIN_TYPE_OFF);
  store_le_int16 (0, RawCmdData () + PLAIN_CRC_OFF);

  const D_UINT8 type = waitingNext ?
                       FRAME_TYPE_PARTIAL_ACK :
                       FRAME_TYPE_PARTIAL_CANCEL;

  SendRawClientFrame (type);
  m_LastReceivedCmd = CMD_INVALID;
}

void
ClientConnection::SendCmdResponse (const D_UINT16 respType,
                                   const bool     lastPart,
                                   bool&          oSendNext)
{
  assert ((respType & 1) != 0);
  assert ((m_LastReceivedCmd + 1) == respType);

  const D_UINT16 respSize = DataSize ();
  m_ServerCookie = w_rnd ();

  store_le_int32 (m_ClientCookie, RawCmdData () + PLAIN_CLNT_COOKIE_OFF);
  store_le_int32 (m_ServerCookie, RawCmdData () + PLAIN_SERV_COOKIE_OFF);
  store_le_int16 (respType, RawCmdData () + PLAIN_TYPE_OFF);

  D_UINT32 chkSum = 0;
  for (int i = 0; i < respSize; i++)
    {
      chkSum = ((chkSum >> 1) & 0x7FFF) + ((chkSum & 1) << 15);
      chkSum += Data ()[i];
      chkSum &= 0xFFFF;
    }
  store_le_int16 (chkSum, RawCmdData () + PLAIN_CRC_OFF);

  const D_UINT8 type = lastPart ? FRAME_TYPE_NORMAL : FRAME_TYPE_PARTIAL;

  SendRawClientFrame (type);

  if (lastPart)
    {
      oSendNext = false;
      return ;
    }

  /* else lastPart == false*/

  ReciveRawClientFrame ();
  switch (m_Data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_PARTIAL_ACK:
    oSendNext = true;
    break;
  case FRAME_TYPE_PARTIAL_CANCEL:
    oSendNext = false;
    break;
  default:
    throw ConnectionException ("Connection with peer is out of sync.",
                               _EXTRA (0));
  }

  if ((DataSize () != 0) || (RawCmdData ()[PLAIN_CRC_OFF] != 0))
    {
      throw ConnectionException ("Peer has send invalid data.",
                                 _EXTRA (0));
    }

  if (from_le_int16 (RawCmdData () + PLAIN_TYPE_OFF) != respType)
    {
      throw ConnectionException ("Connection with peer is out of sync.",
                                 _EXTRA (0));
    }
}
