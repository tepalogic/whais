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

#include <string>

#include "whisper.h"

#include "server_protocol.h"

#include "utils/auto_array.h"
#include "utils/wthread.h"
#include "utils/wsocket.h"

#include "configuration.h"

using namespace whisper;

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
  Thread                m_Thread;
  Socket                m_Socket;
  bool                  m_Root;
  bool                  m_EndConnetion;
};

class ConnectionException : public Exception
{
public:
  ConnectionException (const char* pMessage,
                       const char* pFile,
                       uint32_t      line,
                       uint32_t      extra);

  virtual Exception*     Clone () const;
  virtual EXPCEPTION_TYPE Type () const;
  virtual const char*   Description () const;
};

class ClientConnection
{
public:
  ClientConnection (UserHandler&                 client,
                    std::vector<DBSDescriptors>& databases);
  ~ClientConnection ();

  uint_t      MaxSize () const;
  uint_t      DataSize () const;
  void        DataSize (const uint16_t size);
  uint8_t*    Data ();

  uint32_t ReadCommand ();
  void     SendCmdResponse (const uint16_t respType);

  const DBSDescriptors&  Dbs () { return *m_UserHandler.m_pDesc; }
  SessionStack&          Stack () { return m_Stack; }
  bool                   IsAdmin () const { return m_UserHandler.m_Root; }
private:
  uint8_t* RawCmdData ();

  void ReciveRawClientFrame ();
  void SendRawClientFrame (const uint8_t type);

  UserHandler&        m_UserHandler;
  SessionStack        m_Stack;
  uint32_t            m_WaitingFrameId;
  uint32_t            m_ClientCookie;
  uint32_t            m_ServerCookie;
  uint16_t            m_LastReceivedCmd;
  uint16_t            m_FrameSize;
  uint8_t             m_Version;
  uint8_t             m_Cipher;
  uint_t              m_DataSize;
  uint8_t*            m_Data;
  std::string         m_Key;


  ClientConnection (const ClientConnection&);
  const ClientConnection& operator= (const ClientConnection&);
};

#endif /* CONNECTION_H_ */

