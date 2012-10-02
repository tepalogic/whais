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

private:
  void ReadClientFrame (D_UINT16& outDataSize);

  UserHandler&        m_UserHandler;
  D_UINT32            m_WaitingFrameId;
  D_UINT32            m_ClientCookie;
  D_UINT32            m_ServerCookie;
  D_UINT8             m_IncomeType;
  D_UINT8             m_Version;
  auto_array<D_UINT8> m_Array;
};



#endif /* CONNECTION_H_ */
