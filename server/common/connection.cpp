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

static const D_UINT   FRAME_SIZE_DEFAULT = 256;
static const D_UINT   FRAME_SIZE_MAX     = 4096;
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
    m_WaitingFrameId (0),
    m_ClientCookie (0),
    m_ServerCookie (w_rnd ()),
    m_IncomeType (FRAME_ENCTYPE_PLAIN),
    m_Version (1),
    m_Array (FRAME_SIZE_DEFAULT)
{
  m_UserHandler.m_pDesc = NULL;
  const D_UINT16 authFrameLen = FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA;

  memset (&m_Array[0], 0, authFrameLen);

  m_Array[FRAME_ENCTYPE_OFF] = m_IncomeType;
  m_Array[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT;
  store_le_int32 (0, &m_Array[FRAME_ID_OFF]);
  store_le_int32 (m_Version, &m_Array[FRAME_DATA_OFF + FRAME_AUTH_CLNT_VER]);
  store_le_int16 (authFrameLen, &m_Array[FRAME_SIZE_OFF]);

  m_UserHandler.m_Socket.Write (authFrameLen, &m_Array[0]);

  D_UINT16 respFrameSize = 0;
  ReadClientFrame (respFrameSize);

  const D_UINT32 protocolVer = from_le_int32 (
                              &m_Array[FRAME_DATA_OFF + FRAME_AUTH_CLNT_VER]
                                             );
  if ((respFrameSize < authFrameLen)
      || (protocolVer != m_Version)
      || (m_Array[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT_RSP)
      || (m_Array[FRAME_ENCTYPE_OFF] != FRAME_ENCTYPE_PLAIN))
    {
      throw ConnectionException ("Unexpected authentication frame received.",
                                 _EXTRA (0));
    }

  m_IncomeType = m_Array[FRAME_ENCTYPE_OFF];

  const string dbsName = _RC (const D_CHAR*,
                              &m_Array[FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA]);

  for (vector<DBSDescriptors>::iterator it = databases.begin ();
      it != databases.end ();
      ++it)
    {
      if (it->m_DbsName == dbsName)
        {
          m_UserHandler.m_pDesc = it.base ();
          break;
        }
    }

  if (m_UserHandler.m_pDesc == NULL)
    {
      throw ConnectionException ("Failed to retrieve database.",
                                 _EXTRA (0));
    }

  m_UserHandler.m_Root = (m_Array[FRAME_DATA_OFF + FRAME_AUTH_CLNT_USR] == 0) ?
                         true :
                         false;

  const string passwd = _RC (
      const D_CHAR*,
      &m_Array[FRAME_DATA_OFF + FRAME_AUTH_CLNT_DATA + dbsName.size () + 1]
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

void
ClientConnection::ReadClientFrame (D_UINT16& outDataSize)
{
  assert (m_Array.Size () > FRAME_DATA_OFF);

  D_UINT16 frameRead = 0;

  while (frameRead < FRAME_DATA_OFF)
    {
      D_UINT16 chunkSize;
      chunkSize = m_UserHandler.m_Socket.Read (FRAME_DATA_OFF - frameRead,
                                               &m_Array[frameRead]);
      if (chunkSize == 0)
        throw ConnectionException ("Connection reset by peer.", _EXTRA (0));

      frameRead += chunkSize;
    }

  outDataSize = from_le_int16 (&m_Array[0]);

  if ((outDataSize <= frameRead)
      || (outDataSize >= FRAME_MAX_SIZE))
    {
      throw ConnectionException ("Invalid frame received.", _EXTRA (0));
    }

  if (outDataSize > m_Array.Size ())
    m_Array.Size (outDataSize);

  while (frameRead < outDataSize)
    {
      D_UINT16 chunkSize;
      chunkSize = m_UserHandler.m_Socket.Read (outDataSize - frameRead,
                                               &m_Array[frameRead]);
      if (chunkSize == 0)
        throw ConnectionException ("Connection reset by peer.", _EXTRA (0));

      frameRead += chunkSize;
    }

  assert (frameRead == outDataSize);
}
