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

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <vector>

#include "whisper.h"

#include "server_protocol.h"

#include "utils/include/auto_array.h"
#include "utils/include/wthread.h"
#include "utils/include/wsocket.h"

#include "configuration.h"

struct UserHandler
{
  UserHandler ()
    : m_pDesc (NULL),
      m_Thread (),
      m_Socket (INVALID_SOCKET),
      m_EndConnetion (true)
  {
    m_Thread.IgnoreExceptions (true);
  }

  ~UserHandler ()
  {
    assert (m_Thread.IsEnded ());
  }

  const DBSDescriptors* m_pDesc;
  WThread               m_Thread;
  WSocket               m_Socket;
  bool                  m_Root;
  bool                  m_EndConnetion;
};

class ConnectionException : public WException
{
public:
  ConnectionException (const D_CHAR* pMessage,
                       const D_CHAR* pFile,
                       D_UINT32      line,
                       D_UINT32      extra);

  virtual WException*     Clone () const;
  virtual EXPCEPTION_TYPE Type () const;
  virtual const D_CHAR*   Description () const;
};

class ClientConnection
{
public:
  ClientConnection (UserHandler&                 client,
                    std::vector<DBSDescriptors>& databases);
  ~ClientConnection ();

  D_UINT16      MaxSize () const;
  D_UINT16      DataSize () const;
  void          DataSize (const D_UINT16 size);
  D_UINT8*      Data ();

  D_UINT32 ReadCommand ();
  void     SendCmdResponse (const D_UINT16 respType);

  const DBSDescriptors&  Dbs () { return *m_UserHandler.m_pDesc; }
  SessionStack&          Stack () { return m_Stack; }
  bool                   IsAdmin () const { return m_UserHandler.m_Root; }
private:
  D_UINT8* RawCmdData ();

  void ReciveRawClientFrame ();
  void SendRawClientFrame (const D_UINT8 type);

  UserHandler&        m_UserHandler;
  SessionStack        m_Stack;
  D_UINT32            m_WaitingFrameId;
  D_UINT32            m_ClientCookie;
  D_UINT32            m_ServerCookie;
  D_UINT16            m_LastReceivedCmd;
  D_UINT16            m_FrameSize;
  D_UINT8             m_EncriptionType;
  D_UINT8             m_Version;
  D_UINT8             m_Data[FRAME_MAX_SIZE];

  ClientConnection (const ClientConnection&);
  const ClientConnection& operator= (const ClientConnection&);
};



#endif /* CONNECTION_H_ */
