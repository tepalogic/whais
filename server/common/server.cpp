/******************************************************************************
WHAIS - An advanced database system
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
using namespace whais;


static const uint_t SOCKET_BACK_LOG       = 10;
static const uint_t SLEEP_TICK_RESOLUTION = 10;
static const uint_t REQ_TICK_RESOLUTION   = 100;

static vector<DBSDescriptors>*     sDbsDescriptors;
static FileLogger*                 sMainLog;
static bool                        sAcceptUsersConnections;
static bool                        sServerStopped;
static Lock                        sClosingLock;


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

  bool SearchFreeUser(void (*task)( void* args), Socket& socket)
  {
    assert (mUsersPool.Size () > 0);

    for (uint_t index = 0; index < mUsersPool.Size (); ++index)
      {
        while (mUsersPool[index].mClientSocket != NULL)
          wh_yield ();

        mUsersPool[index].mClientSocket = &socket;
        if (mUsersPool[index].mThread.Run (task, &mUsersPool[index]))
          {
            while (mUsersPool[index].mClientSocket != NULL)
              wh_yield ();

            return true;
          }

        mUsersPool[index].mClientSocket = NULL;
      }

    return false;
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

        mUsersPool[index].mEndConnection = true;
        mUsersPool[index].mSocket.Close ();
        mUsersPool[index].mThread.WaitToEnd (false);
      }
  }

  void ReqTmoCloseTick ()
  {
    for (uint_t c = 0; c < mUsersPool.Size (); ++c)
      {
        if ((mUsersPool[c].mEndConnection) || (mUsersPool[c].mLastReqTick == 0))
          continue;

        if (mUsersPool[c].mDesc == NULL)
          {
            if ((wh_msec_ticks () - mUsersPool[c].mLastReqTick) <
                _SC (uint_t, GetAdminSettings ().mAuthTMO))
              {
                continue ;
              }

            mUsersPool[c].mSocket.Close ();
            sMainLog->Log (LT_WARNING,
                           "Terminated authentication as it took too long...");
            continue ;
          }
        else if ((wh_msec_ticks () - mUsersPool[c].mLastReqTick) <
                 _SC (uint_t, mUsersPool[c].mDesc->mWaitReqTmo))
          {
            continue;
          }

        mUsersPool[c].mSocket.Close ();
        mUsersPool[c].mDesc->mLogger->Log (
                    LT_WARNING,
                    "Terminated connection due to a long wait for a request..."
                                           );
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

  static const uint_t MAX_AUTH_TMO_MS = 200;
};

static auto_array<Listener>* volatile  sListeners;


void
client_handler_routine (void* args)
{
  UserHandler* const client = _RC (UserHandler*, args);

  client->mLastReqTick   = 0;
  client->mSocket        = *client->mClientSocket;
  client->mClientSocket  = NULL;
  client->mEndConnection = false;

  assert (sDbsDescriptors != NULL);

  try
  {
      client->mLastReqTick = wh_msec_ticks (); //Start authentication timer.
      ClientConnection connection (*client, *sDbsDescriptors);

      while (true)
        {
          const COMMAND_HANDLER* cmds;

          client->mLastReqTick = wh_msec_ticks (); //Start request timer!
          uint16_t cmdType = connection.ReadCommand ();
          client->mLastReqTick = 0;                //Stop request timer!

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
                  throw ConnectionException(
                                  _EXTRA (cmdType),
                                  "Invalid administrator command received."
                                            );
                }
              else if (! connection.IsAdmin ())
                {
                  throw ConnectionException(
                      _EXTRA (cmdType),
                      "Regular user wants to execute an administrator command."
                                            );
                }
              cmds = gpAdminCommands;
            }
          cmds[cmdType] (connection);
        }
  }
  catch (SocketException& e)
  {
      assert (e.Description () != NULL);

      ostringstream logEntry;

      logEntry << e.Description () << endl;

      if ( ! e.Message ().empty ())
        logEntry << "Message:\n\t" << e.Message () << endl;

      logEntry <<"Extra: " << e.Code () << " (";
      logEntry << e.File () << ':' << e.Line () << ").\n";

      sMainLog->Log (LT_ERROR, logEntry.str ());
  }
  catch (ConnectionException& e)
  {
      if (client->mDesc != NULL)
        client->mDesc->mLogger->Log (LT_ERROR, e.Message ());

      else
        sMainLog->Log (LT_ERROR, e.Message ());
  }
  catch (FileException& e)
  {
      ostringstream logEntry;

      logEntry << "Client session exception: Unable to deal "
                  "with error condition.\n";
      if (e.Description ())
        logEntry << "Description:\n\t" << e.Description () << endl;

      if ( ! e.Message ().empty ())
        logEntry << "Message:\n\t" << e.Message () << endl;

      logEntry <<"Extra: " << e.Code () << " (";
      logEntry << e.File () << ':' << e.Line () << ").\n";

      sMainLog->Log (LT_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (Exception& e)
  {
      ostringstream logEntry;

      logEntry << "Error condition encountered: \n";
      if (e.Description ())
        logEntry << "Description:\n\t" << e.Description () << endl;

      if ( ! e.Message ().empty ())
        logEntry << "Message:\n\t" << e.Message () << endl;

      logEntry <<"Extra: " << e.Code () << " (";
      logEntry << e.File () << ':' << e.Line () << ").\n";

      client->mDesc->mLogger->Log (LT_ERROR, logEntry.str ());
  }
  catch (std::bad_alloc&)
  {
      sMainLog->Log (LT_CRITICAL, "OUT OF MEMORY!!!");

      StopServer ();
  }
  catch (std::exception& e)
  {
      ostringstream logEntry;

      logEntry << "General system failure: " << e.what () << endl;

      sMainLog->Log (LT_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (...)
  {
      sMainLog->Log (LT_CRITICAL, "Unknown exception!");
      StopServer ();
  }

  client->mSocket.Close ();

  client->mLastReqTick  = 0;
  client->mEndConnection = true;
}


static void
ticks_routine ()
{
  const uint_t syncWakeup = GetAdminSettings ().mSyncWakeup;

  uint_t syncElapsedTicks = 0, reqCheckElapsedTics = 0;
  do
    {
      if (sServerStopped)
        return ;

      wh_sleep (SLEEP_TICK_RESOLUTION);
      syncElapsedTicks    += SLEEP_TICK_RESOLUTION;
      reqCheckElapsedTics += SLEEP_TICK_RESOLUTION;
      if ((syncElapsedTicks < syncWakeup)
          && (reqCheckElapsedTics < REQ_TICK_RESOLUTION))
        {
          continue;
        }

      for (size_t i = 0;
           (syncElapsedTicks >= syncWakeup) && (i < sDbsDescriptors->size ());
           ++i)
        {
          if (sServerStopped)
            return ;

          const uint64_t dbsLstFlush = (*sDbsDescriptors)[i].mLastFlushTick;
          const int      dbsSyncTmo  = (*sDbsDescriptors)[i].mSyncInterval;

          if (_SC (int64_t, wh_msec_ticks () - dbsLstFlush) < dbsSyncTmo)
            continue;

          IDBSHandler&      hnd         = *((*sDbsDescriptors)[i].mDbs);
          const TABLE_INDEX tablesCount = hnd.PersistentTablesCount ();
          for (TABLE_INDEX t = 0; t < tablesCount; ++t)
            {
              if (sServerStopped)
                return ;

              hnd.SyncTableContent (t);
            }
          (*sDbsDescriptors)[i].mLastFlushTick = wh_msec_ticks ();
        }

        if (syncElapsedTicks >= syncWakeup)
          syncElapsedTicks = 0;

        if (sServerStopped)
          return ;

        for (uint_t i = 0;
            (reqCheckElapsedTics >= REQ_TICK_RESOLUTION)
                && (i < sListeners->Size ());
            ++i)
          {
            (*sListeners)[i].ReqTmoCloseTick ();
          }

        if (reqCheckElapsedTics >= REQ_TICK_RESOLUTION)
          reqCheckElapsedTics = 0;
    }
  while (true);
}

void
listener_routine (void* args)
{
  Listener* const listener = _RC (Listener*, args);

  assert (listener->mUsersPool.Size () > 0);
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

        sMainLog->Log (LT_INFO, logEntry.str ());
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

          if ( ! listener->SearchFreeUser (client_handler_routine, client))
            {
              static const uint8_t busyResp[] = { 0x04, 0x00, 0xFF, 0xFF };
              client.Write (busyResp, sizeof busyResp);

              sMainLog->Log (
                    LT_INFO,
                    "Connection refused because of unavailable slots."
                            );
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

                logEntry <<"Extra: " << e.Code () << " (";
                logEntry << e.File () << ':' << e.Line () << ").\n";

                sMainLog->Log (LT_ERROR, logEntry.str ());
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

      logEntry <<"Extra: " << e.Code () << " (";
      logEntry << e.File () << ':' << e.Line () << ").\n";

      sMainLog->Log (LT_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (std::bad_alloc&)
  {
      sMainLog->Log (LT_CRITICAL, "OUT OF MEMORY!!!");

      StopServer ();
  }
  catch (std::exception& e)
  {
      ostringstream logEntry;

      logEntry << "General system failure: " << e.what () << endl;

      sMainLog->Log (LT_CRITICAL, logEntry.str ());

      StopServer ();
  }
  catch (...)
  {
      sMainLog->Log (LT_CRITICAL, "Listener received unexpected exception!");
      StopServer ();
  }
}


void
StartServer (FileLogger& log, vector<DBSDescriptors>& databases)
{
  sDbsDescriptors = &databases;
  sMainLog        = &log;

  log.Log (LT_DEBUG, "Server started!");

  assert (databases.size () > 0);

  const ServerSettings& server = GetAdminSettings ();

  auto_array<Listener> listeners (server.mListens.size ());

  sListeners = &listeners;

  sAcceptUsersConnections = true;
  sServerStopped          = false;

  for (uint_t index = 0; index < listeners.Size (); ++index)
    {
      Listener* const listener = &listeners[index];

      listener->mInterface = (server.mListens[index].mInterface.size () == 0)
                              ? NULL
                              : server.mListens[index].mInterface.c_str ();
      listener->mPort = server.mListens[index].mService.c_str ();
      if ( ! listener->mListenThread.Run (listener_routine, listener))
        {
          ostringstream logBuffer;
          logBuffer << "Failed to start listener for '"
                    << server.mListens[index].mInterface << '\'';
          log.Log (LT_ERROR, logBuffer.str ());
        }
    }

  ticks_routine ();
  log.Log (LT_DEBUG, "Ticks routine has stopped.");

  for (uint_t index = 0; index < listeners.Size (); ++index)
    listeners[index].mListenThread.WaitToEnd (false);

  LockRAII<Lock> holder (sClosingLock);
  sListeners = NULL;

  log.Log (LT_DEBUG, "Server stopped!");
}


void
StopServer ()
{
  LockRAII<Lock> holder (sClosingLock);

  if ((sListeners == NULL)  || (sMainLog == NULL))
    return; //Ignore! The server probably did not even start.

  sMainLog->Log (LT_INFO, "Server asked to shutdown.");
  sAcceptUsersConnections = false;
  sServerStopped          = true;

  for (size_t i = 0; i < sDbsDescriptors->size (); i++)
    (*sDbsDescriptors)[i].mSession->NotifyEvent (ISession::SERVER_STOPED, NULL);

  for (uint_t i = 0; i < sListeners->Size (); ++i)
    (*sListeners)[i].Close ();

  sClosingLock.Release ();
  sClosingLock.Acquire (); //For Windows! Wait until is closed!
}

#ifdef ENABLE_MEMORY_TRACE
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "WHAIS";
#endif

