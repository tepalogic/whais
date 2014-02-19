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

#include <sstream>

#include "utils/wthread.h"
#include "server/server_protocol.h"

#include "server.h"
#include "connection.h"
#include "commands.h"


using namespace std;
using namespace whisper;


class Listener
{
public:
  Listener ()
    : mInterface (NULL),
      mPort (NULL),
      mListenThread (),
      mSocket (INVALID_SOCKET),
      mUsersPool (GetAdminSettings ().mMaxConnections)
  {
    mListenThread.IgnoreExceptions (true);
  }

  ~Listener ()
  {
    assert (mListenThread.IsEnded ());
  }

  UserHandler* SearchFreeUser ()
  {
    assert (mUsersPool.Size () > 0);

    for (uint_t index = 0; index < mUsersPool.Size (); ++index)
      {
        if (mUsersPool[index].mThread.IsEnded ())
          return &mUsersPool[index];
      }

    return NULL;
  }

  void Close ()
  {
    //If no exception has be thrown by now, there is
    //no point to do it now when we are about to close.
    mListenThread.IgnoreExceptions (true);
    mListenThread.DiscardException ();

    //Cancel any pending IO operations.
    mSocket.Close ();

    for (uint_t index = 0; index < mUsersPool.Size (); ++index)
      {
        mUsersPool[index].mThread.IgnoreExceptions (true);
        mUsersPool[index].mThread.DiscardException ();

        mUsersPool[index].mEndConnetion = true;
        mUsersPool[index].mSocket.Close ();
      }
  }

  const char*             mInterface;
  const char*             mPort;
  Thread                  mListenThread;
  Socket                  mSocket;
  auto_array<UserHandler> mUsersPool;

private:
  Listener (const Listener& );
  Listener& operator= (const Listener&);
};



static const uint_t SOCKET_BACK_LOG = 10;

static vector<DBSDescriptors>*     sDbsDescriptors;
static FileLogger*                 sMainLog;
static bool                        sAcceptUsersConnections;
static bool                        sServerStopped;
static auto_array<Listener>*       sListeners;
static Lock                        sClosingLock;


void
client_handler_routine (void* args)
{
  UserHandler* const client = _RC (UserHandler*, args);

  assert (client != NULL);
  assert (sDbsDescriptors != NULL);

  try
  {
      ClientConnection connection (*client, *sDbsDescriptors);

      while (true)
        {
          const COMMAND_HANDLER* cmds;

          uint16_t cmdType = connection.ReadCommand ();

          if (cmdType == CMD_CLOSE_CONN)
            break;

          if ((cmdType & 1) != 0)
            {
              throw ConnectionException (_EXTRA (cmdType),
                                         "Invalid command requested.");

            }

          if (cmdType >= USER_CMD_BASE)
            {
              cmdType -= USER_CMD_BASE;
              cmdType /= 2;
              if (cmdType >= USER_CMDS_COUNT)
                {
                  throw ConnectionException (_EXTRA (cmdType),
                                             "Invalid user command received.");
                }
              cmds = gpUserCommands;
            }
          else
            {
              cmdType /= 2;
              if (cmdType >= ADMIN_CMDS_COUNT)
                {
                  throw ConnectionException (
                                  _EXTRA (cmdType),
                                  "Invalid administrator command received."
                                            );
                }
              else if (! connection.IsAdmin ())
                {
                  throw ConnectionException (_EXTRA (cmdType),
                                             "Regular user wants to execute "
                                               "an administrator command.");
                }
              cmds = gpAdminCommands;
            }

          cmds[cmdType] (connection);
        }
  }
  catch (SocketException& e)
  {
      assert (e.Description() != NULL);

      ostringstream logEntry;

      logEntry << "Unable to deal with error condition.\n";
      logEntry << "Description:\n\t" << e.Description () << endl;

      if ( ! e.Message ().empty ())
        logEntry << "Message:\n\t" << e.Message () << endl;

      logEntry <<"Extra: " << e.Extra () << " (";
      logEntry << e.File () << ':' << e.Line() << ").\n";

      sMainLog->Log (LOG_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (ConnectionException& e)
  {
      if (client->mDesc != NULL)
        client->mDesc->mLogger->Log (LOG_ERROR, e.Message ());

      else
        sMainLog->Log (LOG_ERROR, e.Message ());
  }
  catch (Exception& e)
  {
      ostringstream logEntry;

      logEntry << "Unable to deal with error condition.\n";
      if (e.Description ())
        logEntry << "Description:\n\t" << e.Description () << endl;

      if ( ! e.Message ().empty ())
        logEntry << "Message:\n\t" << e.Message () << endl;

      logEntry <<"Extra: " << e.Extra () << " (";
      logEntry << e.File () << ':' << e.Line() << ").\n";

      sMainLog->Log (LOG_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (std::bad_alloc&)
  {
      sMainLog->Log (LOG_CRITICAL, "OUT OF MEMORY!!!");

      StopServer ();
  }
  catch (std::exception& e)
  {
      ostringstream logEntry;

      logEntry << "General system failure: " << e.what() << endl;

      sMainLog->Log (LOG_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (...)
  {
      sMainLog->Log (LOG_CRITICAL, "Unknown exception!");
      StopServer ();
  }

  client->mSocket.Close ();
  client->mEndConnetion = true;
}


void
listener_routine (void* args)
{
  Listener* const listener = _RC (Listener*, args);

  assert (listener->mUsersPool.Size () > 0);
  assert (listener->mListenThread.IsEnded () == false);
  assert (listener->mListenThread.HasExceptionPending () == false);
  assert (listener->mPort != NULL);

  try
  {
      {
        ostringstream logEntry;

        logEntry << "Listening ";
        logEntry << ((listener->mInterface == NULL) ?
                       "*" :
                       listener->mInterface);
        logEntry << '@' << listener->mPort << ".\n";

        sMainLog->Log (LOG_INFO, logEntry.str ());
      }

    listener->mSocket = Socket (listener->mInterface,
                                listener->mPort,
                                SOCKET_BACK_LOG);

    bool acceptUserConnections = sAcceptUsersConnections;

    while (acceptUserConnections)
      {
        try
        {
          Socket client = listener->mSocket.Accept ();

          UserHandler* const hnd = listener->SearchFreeUser ();

          if (hnd != NULL)
            {
              hnd->mSocket       = client;
              hnd->mEndConnetion = false;

              hnd->mThread.Run (client_handler_routine, hnd);
            }
          else
            {
              static const uint8_t busyResp[] = { 0x04, 0x00, 0xFF, 0xFF };

              client.Write (busyResp, sizeof busyResp);
              client.Close ();
            }
        }
        catch (SocketException& e)
        {
            if (sAcceptUsersConnections)
              {
                assert (e.Description () != NULL);
                ostringstream logEntry;

                logEntry << "Description:\n\t" << e.Description () << endl;

                if ( ! e.Message ().empty ())
                  logEntry << "Message:\n\t" << e.Message () << endl;

                logEntry <<"Extra: " << e.Extra () << " (";
                logEntry << e.File () << ':' << e.Line() << ").\n";

                sMainLog->Log (LOG_ERROR, logEntry.str ());
              }
        }
        acceptUserConnections = sAcceptUsersConnections;
      }
  }
  catch (Exception& e)
  {
      assert (e.Description () != NULL);

      ostringstream logEntry;

      logEntry << "Unable to deal with this error condition at this point.\n";
      logEntry << "Description:\n\t" << e.Description () << endl;

      if ( ! e.Message ().empty ())
        logEntry << "Message:\n\t" << e.Message () << endl;

      logEntry <<"Extra: " << e.Extra () << " (";
      logEntry << e.File () << ':' << e.Line() << ").\n";

      sMainLog->Log (LOG_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (std::bad_alloc&)
  {
      sMainLog->Log (LOG_CRITICAL, "OUT OF MEMORY!!!");

      StopServer ();
  }
  catch (std::exception& e)
  {
      ostringstream logEntry;

      logEntry << "General system failure: " << e.what() << endl;

      sMainLog->Log (LOG_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (...)
  {
      sMainLog->Log (LOG_CRITICAL, "Listener received unexpected exception!");
      StopServer ();
  }
}


void
StartServer (FileLogger& log, vector<DBSDescriptors>& databases)
{
  sDbsDescriptors = &databases;
  sMainLog        = &log;

  log.Log (LOG_DEBUG, "Server started!");

  assert (databases.size () > 0);

  const ServerSettings& server = GetAdminSettings ();

  auto_array<Listener> listeners (server.mListens.size ());

  sListeners = &listeners;

  sAcceptUsersConnections = true;
  sServerStopped          = false;

  for (uint_t index = 0; index < listeners.Size (); ++index)
    {
      Listener* const listener = &listeners[index];

      assert (listener->mListenThread.IsEnded ());

      listener->mInterface = (server.mListens[index].mInterface.size () == 0) ?
                               NULL :
                               server.mListens[index].mInterface.c_str ();
      listener->mPort = server.mListens[index].mService.c_str ();
      listener->mListenThread.Run (listener_routine, listener);
    }

  for (uint_t index = 0; index < listeners.Size (); ++index)
    listeners[index].mListenThread.WaitToEnd (false);

  LockRAII holder(sClosingLock);
  sListeners = NULL;

  log.Log (LOG_DEBUG, "Server stopped!");
}


void
StopServer ()
{
  LockRAII holder(sClosingLock);

  if ((sListeners == NULL)  || (sMainLog == NULL))
    return; //Ignore! The server probably did not even start.

  sMainLog->Log (LOG_INFO, "Server asked to shutdown.");

  sAcceptUsersConnections = false;
  sServerStopped          = true;

  for (uint_t index = 0; index < sListeners->Size (); ++index)
    (*sListeners)[index].Close ();
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "WHISPER";
#endif

