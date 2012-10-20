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

#include "server/include/server_protocol.h"

#include "server.h"
#include "connection.h"
#include "commands.h"

using namespace std;

class Listener
{
public:
  Listener ()
    : m_pInterface (NULL),
      m_pPort (NULL),
      m_ListenThread (),
      m_Socket (INVALID_SOCKET),
      m_UsersPool (GetAdminSettings ().m_MaxConnections)
  {
  }

  ~Listener ()
  {
    assert (m_ListenThread.IsEnded ());

    for (D_UINT index = 0; index < m_UsersPool.Size (); ++index)
      m_UsersPool[index].m_Thread.Join ();
  }

  UserHandler* SearchFreeUser ()
  {
    assert (m_UsersPool.Size () > 0);

    for (D_UINT index = 0; index < m_UsersPool.Size (); ++index)
      {
        if (m_UsersPool[index].m_Thread.IsEnded ())
          return &m_UsersPool[index];
      }

    return NULL;
  }

  void Close ()
  {
    m_ListenThread.IgnoreExceptions (true);
    m_ListenThread.DiscardException ();
    m_Socket.Close ();

    for (D_UINT index = 0; index < m_UsersPool.Size (); ++index)
      {
        m_UsersPool[index].m_EndConnetion = true;
        m_UsersPool[index].m_Socket.Close ();
      }
  }

  const D_CHAR*           m_pInterface;
  const D_CHAR*           m_pPort;
  WThread                 m_ListenThread;
  WSocket                 m_Socket;
  auto_array<UserHandler> m_UsersPool;

private:
  Listener (const Listener& );
  Listener& operator= (const Listener&);
};

static const D_UINT SOCKET_BACK_LOG = 10;

static vector<DBSDescriptors>* spDatabases;
static Logger*                 spLogger;
static bool                    sAcceptUsersConnections;
static bool                    sServerStopped;
static auto_array<Listener>*   spaListeners;

void
client_handler_routine (void* args)
{
  UserHandler* const pClient = _RC (UserHandler*, args);

  assert (pClient != NULL);
  assert (spDatabases != NULL);

  try
  {
      ClientConnection connection (*pClient, *spDatabases);

      while (true)
        {
          const COMMAND_HANDLER* pCmds;
          bool                   lastPart;

          D_UINT16 cmdType = connection.ReadCommand (lastPart);

          if (cmdType == CMD_CLOSE_CONN)
            break;

          if ((cmdType & 1) != 0)
            {
              throw ConnectionException ("Invalid command received.",
                                         _EXTRA (0));
            }

          if (cmdType >= USER_CMD_BASE)
            {
              cmdType -= USER_CMD_BASE;
              cmdType /= 2;
              if (cmdType >= USER_CMDS_COUNT)
                {
                  throw ConnectionException ("Invalid user command recieved",
                                             _EXTRA (0));
                }
              pCmds = gpUserCommands;
            }
          else
            {
              cmdType /= 2;
              if (cmdType >= ADMIN_CMDS_COUNT)
                {
                  throw ConnectionException ("Invalid admin command recieved",
                                             _EXTRA (0));
                }
              pCmds = gpAdminCommands;
            }
          pCmds[cmdType] (connection, lastPart);
        }
  }
  catch (ConnectionException& e)
  {
      if (pClient->m_pDesc != NULL)
        pClient->m_pDesc->m_pLogger->Log (LOG_ERROR, e.Message ());
      else
        spLogger->Log (LOG_ERROR, e.Message ());
  }

  pClient->m_Socket.Close ();
  pClient->m_EndConnetion = true;
}

void
listener_routine (void* args)
{
  Listener* const pListener = _RC (Listener*, args);

  assert (pListener->m_UsersPool.Size () > 0);
  assert (pListener->m_ListenThread.IsEnded () == false);
  assert (pListener->m_ListenThread.HasExceptionPending () == false);
  assert (pListener->m_pPort != NULL);

  pListener->m_Socket = WSocket (pListener->m_pInterface,
                                 pListener->m_pPort,
                                 SOCKET_BACK_LOG);

  bool acceptUserConnections = sAcceptUsersConnections;
  while (acceptUserConnections)
    {
      WSocket client = pListener->m_Socket.Accept ();
      UserHandler* pUsrHnd = pListener->SearchFreeUser ();

      if (pUsrHnd != NULL)
        {
          pUsrHnd->m_Socket = client;
          pUsrHnd->m_EndConnetion = false;
          pUsrHnd->m_Thread.Run (client_handler_routine, pUsrHnd);
        }
      else
        {
          static const D_UINT8 busyResp[] = { 0x04, 0x00, 0xFF, 0xFF };

          client.Write (sizeof busyResp, busyResp);
          client.Close ();
        }

      acceptUserConnections = sAcceptUsersConnections;
    }
}

void
StartServer (Logger& log, vector<DBSDescriptors>& databases)
{
  spDatabases = &databases;
  spLogger    = &log;

  log.Log (LOG_DEBUG, "Server started!");

  assert (databases.size () > 0);

  const ServerSettings& server = GetAdminSettings ();
  auto_array<Listener> listeners (server.m_Listens.size ());

  spaListeners = &listeners;

  sAcceptUsersConnections = true;
  sServerStopped          = false;

  for (D_UINT index = 0; index < listeners.Size (); ++index)
    {
      Listener* const pEnt = &listeners[index];

      assert (pEnt->m_ListenThread.IsEnded ());

      pEnt->m_pInterface = (server.m_Listens[index].m_Interface.size () == 0) ?
                           NULL :
                           server.m_Listens[index].m_Interface.c_str ();
      pEnt->m_pPort = server.m_Listens[index].m_Service.c_str ();
      pEnt->m_ListenThread.Run (listener_routine, pEnt);
    }

  for (D_UINT index = 0; index < listeners.Size (); ++index)
    listeners[index].m_ListenThread.Join ();

  spaListeners = NULL;

  log.Log (LOG_DEBUG, "Server stopped!");
}

void
StopServer ()
{
  if ((spaListeners == NULL)  || (spLogger == NULL))
    return; //Ignore! The server probably did not even start.

  spLogger->Log (LOG_INFO, "Server asked to shutdown.");

  sAcceptUsersConnections = false;
  sServerStopped          = true;

  for (D_UINT index = 0; index < spaListeners->Size (); ++index)
    (*spaListeners)[index].Close ();
}


#ifdef ENABLE_MEMORY_TRACE
D_UINT32 WMemoryTracker::sm_InitCount = 0;
const D_CHAR* WMemoryTracker::sm_Module = "WHISPER";
#endif

