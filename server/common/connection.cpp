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
#include "utils/include/enc_3k.h"

#include "connection.h"

static const uint32_t FRAME_SERVER_VERS  = 0x00000001;

using namespace std;

ConnectionException::ConnectionException (const char* pMessage,
                                          const char* pFile,
                                          uint32_t      line,
                                          uint32_t      extra)
  : Exception (pMessage, pFile, line, extra)
{
}

Exception*
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

const char*
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
    m_Version (1),
    m_Cipher (FRAME_ENCTYPE_PLAIN),
    m_DataSize (GetAdminSettings ().m_MaxFrameSize),
    m_Data (new uint8_t[m_DataSize])
{
  assert ((m_DataSize >= MIN_FRAME_SIZE) && (m_DataSize <= MAX_FRAME_SIZE));

  if (m_Cipher == FRAME_ENCTYPE_3K)
    m_DataSize -= m_DataSize % sizeof (uint32_t);

  m_UserHandler.m_pDesc = NULL;
  const uint16_t authFrameLen = FRAME_HDR_SIZE + FRAME_AUTH_SIZE;

  memset (&m_Data[0], 0, authFrameLen);

  assert (authFrameLen <= MIN_FRAME_SIZE);
  store_le_int16 (authFrameLen, &m_Data[FRAME_SIZE_OFF]);
  m_Data[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT;
  m_Data[FRAME_ENCTYPE_OFF] = FRAME_ENCTYPE_PLAIN;
  store_le_int32 (0, &m_Data[FRAME_ID_OFF]);

  store_le_int32 (m_Version, &m_Data[FRAME_HDR_SIZE + FRAME_AUTH_VER_OFF]);
  store_le_int16 (m_DataSize, &m_Data[FRAME_HDR_SIZE + FRAME_AUTH_SIZE_OFF]);
  m_Data[FRAME_HDR_SIZE + FRAME_AUTH_ENC_OFF] = GetAdminSettings().m_Cipher;;


  m_UserHandler.m_Socket.Write (authFrameLen, m_Data);
  ReciveRawClientFrame ();

  m_Cipher = GetAdminSettings().m_Cipher;
  const uint32_t protocolVer = from_le_int32 (
                              &m_Data[FRAME_HDR_SIZE + FRAME_AUTH_RSP_VER_OFF]
                                             );
  if ((m_FrameSize < authFrameLen)
      || (protocolVer != m_Version)
      || (m_Data[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT_RSP)
      || (m_Data[FRAME_ENCTYPE_OFF] != FRAME_ENCTYPE_PLAIN))
    {
      throw ConnectionException ("Unexpected authentication frame received.",
                                 _EXTRA (0));
    }

  if (m_Cipher != m_Data[FRAME_HDR_SIZE + FRAME_AUTH_RSP_ENC_OFF])
    {
      throw ConnectionException ("The cipher not echoed back by client.",
                                 _EXTRA (0));
    }

  const string dbsName = _RC (const char*,
                              &m_Data[FRAME_HDR_SIZE +
                                      FRAME_AUTH_RSP_FIXED_SIZE]);

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

  m_UserHandler.m_Root =
        (m_Data[FRAME_HDR_SIZE + FRAME_AUTH_RSP_USR_OFF] == 0) ?
          true :
          false;

  if (m_Cipher == FRAME_ENCTYPE_PLAIN)
    {
      const string passwd = _RC (const char*,
                                   m_Data +
                                   FRAME_HDR_SIZE +
                                   FRAME_AUTH_RSP_FIXED_SIZE +
                                   dbsName.size () + 1);
      if (m_UserHandler.m_Root)
        {
          if (m_UserHandler.m_pDesc->m_RootPass != passwd)
            {
              throw ConnectionException (
                                "Failed to authenticate database root user.",
                                _EXTRA (0)
                                        );

            }
          else
            m_Key = m_UserHandler.m_pDesc->m_RootPass;
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
          else
            m_Key = m_UserHandler.m_pDesc->m_UserPasswd;
        }
    }
  else if (m_Cipher == FRAME_ENCTYPE_3K)
    {
      if (m_UserHandler.m_Root)
         m_Key = m_UserHandler.m_pDesc->m_RootPass;

      else
        m_Key = m_UserHandler.m_pDesc->m_UserPasswd;
    }
  else
    {
      assert (false);
    }

}

ClientConnection::~ClientConnection ()
{
  delete [] m_Data;
}

uint_t
ClientConnection::MaxSize () const
{
  assert ((m_Cipher == FRAME_ENCTYPE_PLAIN) || (m_Cipher == FRAME_ENCTYPE_3K));
  assert ((MIN_FRAME_SIZE<= m_DataSize) && (m_DataSize <= MAX_FRAME_SIZE));

  uint_t metaDataSize;

  switch (m_Cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (false);
  }

  return m_DataSize - metaDataSize;
}

uint_t
ClientConnection::DataSize () const
{
  uint_t metaDataSize;

  switch (m_Cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (false);
  }

  assert ((m_FrameSize == 0) || (m_FrameSize >= metaDataSize));
  assert (m_FrameSize <= m_DataSize);

  return (m_FrameSize == 0) ? 0 : (m_FrameSize - metaDataSize);

}

uint8_t*
ClientConnection::Data ()
{
  uint_t metaDataSize;

  switch (m_Cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (false);
  }

  return m_Data + metaDataSize;
}

void
ClientConnection::DataSize (const uint16_t size)
{
  assert (size <= MaxSize ());

  uint_t metaDataSize;

  switch (m_Cipher)
  {
  case FRAME_ENCTYPE_PLAIN:
    metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  case FRAME_ENCTYPE_3K:
    metaDataSize = FRAME_HDR_SIZE + ENC_3K_HDR_SIZE + PLAIN_HDR_SIZE;
    break;

  default:
    assert (false);
  }

  m_FrameSize = size + metaDataSize;

  assert (m_FrameSize <= m_DataSize);
}

uint8_t*
ClientConnection::RawCmdData ()
{
  if (m_Cipher == FRAME_ENCTYPE_PLAIN)
    return m_Data + FRAME_HDR_SIZE;

  else if (m_Cipher == FRAME_ENCTYPE_3K)
    return m_Data + FRAME_HDR_SIZE + ENC_3K_HDR_SIZE;

  return NULL;
}

void
ClientConnection::ReciveRawClientFrame ()
{
  uint16_t frameRead = 0;

  while (frameRead < FRAME_HDR_SIZE)
    {
      uint16_t chunkSize;
      chunkSize = m_UserHandler.m_Socket.Read (FRAME_HDR_SIZE - frameRead,
                                               &m_Data[frameRead]);
      if (chunkSize == 0)
        throw ConnectionException ("Connection reset by peer.", _EXTRA (0));

      frameRead += chunkSize;
    }

  switch (m_Data[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
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

  m_FrameSize = from_le_int16 (m_Data + FRAME_SIZE_OFF);

  if ((m_FrameSize < frameRead)
      || (m_FrameSize > m_DataSize))
    {
      throw ConnectionException ("Invalid frame received.", _EXTRA (0));
    }

  while (frameRead < m_FrameSize)
    {
      uint16_t chunkSize;
      chunkSize = m_UserHandler.m_Socket.Read (m_FrameSize - frameRead,
                                               &m_Data[frameRead]);
      if (chunkSize == 0)
        throw ConnectionException ("Connection reset by peer.", _EXTRA (0));

      frameRead += chunkSize;
    }

  assert (frameRead == m_FrameSize);

  const uint32_t frameId = from_le_int32 (m_Data + FRAME_ID_OFF);
  if (frameId != m_WaitingFrameId)
    {
      throw ConnectionException ("Connection with peer is out of sync",
                                 _EXTRA (0));
    }

  const uint32_t encType = m_Data[FRAME_ENCTYPE_OFF];
  if (encType != m_Cipher)
    throw ConnectionException ("Peer has used a wrong cipher.", _EXTRA (0));

  if (encType == FRAME_ENCTYPE_3K)
    {
      uint8_t prev = 0;
      for (uint_t i = 0; i < ENC_3K_PLAIN_SIZE_OFF; ++i)
        {
          m_Data[FRAME_HDR_SIZE + i] ^= m_Key.at (prev % m_Key.length ());
          prev = m_Data[FRAME_HDR_SIZE + i];
        }

      const uint32_t firstKing = from_le_int32 (m_Data +
                                                FRAME_HDR_SIZE +
                                                ENC_3K_FIRST_KING_OFF);
      const uint32_t secondKing = from_le_int32 (m_Data +
                                                 FRAME_HDR_SIZE +
                                                 ENC_3K_SECOND_KING_OFF);
      decrypt_3k_buffer (
                      firstKing,
                      secondKing,
                      _RC (const uint8_t*, m_Key.c_str ()),
                      m_Key.length (),
                      m_Data + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF,
                      m_FrameSize - (FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF)
                         );

      const uint16_t plainSize = from_le_int16 (m_Data +
                                                FRAME_HDR_SIZE +
                                                ENC_3K_PLAIN_SIZE_OFF);
      m_FrameSize = plainSize;
      store_le_int16 (plainSize, m_Data + FRAME_SIZE_OFF);
    }
}

void
ClientConnection::SendRawClientFrame (const uint8_t type)
{
  assert ((m_FrameSize >= FRAME_HDR_SIZE) && (m_FrameSize <= m_DataSize));


  if (m_Cipher == FRAME_ENCTYPE_3K)
    {
      const uint16_t plainSize = m_FrameSize;

      while (m_FrameSize % sizeof (uint32_t) != 0)
        m_Data[m_FrameSize++] = w_rnd () & 0xFF;

      const uint32_t firstKing  = w_rnd () & 0xFFFFFFFF;
      store_le_int32 (firstKing,
                      m_Data + FRAME_HDR_SIZE + ENC_3K_FIRST_KING_OFF);

      const uint32_t secondKing = w_rnd () & 0xFFFFFFFF;
      store_le_int32 (secondKing,
                      m_Data + FRAME_HDR_SIZE + ENC_3K_SECOND_KING_OFF);

      uint8_t prev = 0;
      for (uint_t i = 0; i < ENC_3K_PLAIN_SIZE_OFF; ++i)
        {
          const uint8_t temp = m_Data[FRAME_HDR_SIZE + i];

          m_Data[FRAME_HDR_SIZE + i] ^= m_Key.at (prev % m_Key.length ());
          prev = temp;
        }

      store_le_int16 (plainSize,
                      m_Data + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF);
      store_le_int16 (w_rnd () & 0xFFFF,
                      m_Data + FRAME_HDR_SIZE + ENC_3K_SPARE_OFF);

      encrypt_3k_buffer (
                      firstKing,
                      secondKing,
                      _RC (const uint8_t*, m_Key.c_str ()),
                      m_Key.length (),
                      m_Data + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF,
                      m_FrameSize - (FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF)
                         );
    }

  store_le_int16 (m_FrameSize, m_Data + FRAME_SIZE_OFF);
  store_le_int32 (++m_WaitingFrameId, m_Data + FRAME_ID_OFF);

  m_Data[FRAME_TYPE_OFF]    = type;
  m_Data[FRAME_ENCTYPE_OFF] = m_Cipher;

  m_UserHandler.m_Socket.Write (m_FrameSize, m_Data);

  m_FrameSize    = 0;  //This frame content is not valid anymore.
  m_ClientCookie = ~0; //Make sure the client cookie is reread.
}

uint32_t
ClientConnection::ReadCommand ()
{
  ReciveRawClientFrame ();

  const uint32_t servCookie = from_le_int32 (
                                RawCmdData () + PLAIN_SERV_COOKIE_OFF
                                            );
  if (servCookie != m_ServerCookie)
    {
      throw ConnectionException ("Peer context cannot be recognized.",
                                 _EXTRA (0));
    }



  const uint16_t  respSize = DataSize ();
  uint16_t        chkSum   = 0;
  for (uint_t i = 0; i < respSize; i++)
    chkSum += Data ()[i];

  if (chkSum != from_le_int16 (RawCmdData () + PLAIN_CRC_OFF))
    throw ConnectionException ("Frame with invalid CRC received.", _EXTRA (0));

  m_ClientCookie = from_le_int32 (RawCmdData () + PLAIN_CLNT_COOKIE_OFF);

  m_LastReceivedCmd = from_le_int16 (RawCmdData () + PLAIN_TYPE_OFF);
  assert ((m_LastReceivedCmd & 1) == 0);

  return m_LastReceivedCmd;
}

void
ClientConnection::SendCmdResponse (const uint16_t respType)
{
  assert ((respType & 1) != 0);
  assert ((m_LastReceivedCmd + 1) == respType);

  const uint16_t respSize = DataSize ();
  m_ServerCookie = w_rnd ();

  store_le_int32 (m_ClientCookie, RawCmdData () + PLAIN_CLNT_COOKIE_OFF);
  store_le_int32 (m_ServerCookie, RawCmdData () + PLAIN_SERV_COOKIE_OFF);
  store_le_int16 (respType, RawCmdData () + PLAIN_TYPE_OFF);

  uint32_t chkSum = 0;
  for (uint_t i = 0; i < respSize; i++)
    chkSum += Data ()[i];

  store_le_int16 (chkSum, RawCmdData () + PLAIN_CRC_OFF);

  SendRawClientFrame (FRAME_TYPE_NORMAL);
}
