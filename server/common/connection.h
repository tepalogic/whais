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
#include <memory>

#include "whisper.h"


#include "utils/auto_array.h"
#include "utils/wthread.h"
#include "utils/wsocket.h"
#include "server/server_protocol.h"

#include "configuration.h"


using namespace whisper;


struct UserHandler
{
  UserHandler ()
    : mDesc (NULL),
      mLastReqTick (0),
      mThread (),
      mSocket (INVALID_SOCKET),
      mRoot (false),
      mEndConnetion (true)
  {
    mThread.IgnoreExceptions (true);
  }

  ~UserHandler ()
  {
    assert (mThread.IsEnded ());
  }

  const DBSDescriptors* mDesc;
  uint64_t              mLastReqTick;
  Thread                mThread;
  Socket                mSocket;
  bool                  mRoot;
  bool                  mEndConnetion;
};



class ConnectionException : public Exception
{
public:
  ConnectionException (const uint32_t  code,
                       const char*     file,
                       uint32_t        line,
                       const char*     fmtMsg = NULL,
                       ...);

  virtual Exception* Clone () const;

  virtual EXCEPTION_TYPE Type () const;

  virtual const char* Description () const;
};



class ClientConnection
{
public:
  ClientConnection (UserHandler&                 client,
                    std::vector<DBSDescriptors>& databases);

  uint_t MaxSize () const;

  uint_t DataSize () const;
  void   DataSize (const uint16_t size);

  uint8_t* Data ();

  uint32_t ReadCommand ();

  void SendCmdResponse (const uint16_t respType);

  const DBSDescriptors& Dbs ()
  {
    assert (mUserHandler.mDesc != NULL);

    return *mUserHandler.mDesc;
  }

  SessionStack& Stack ()
  {
    return mStack;
  }

  bool IsAdmin () const
  {
    return mUserHandler.mRoot;
  }

private:
  uint8_t* RawCmdData ();

  void ReciveRawClientFrame ();

  void SendRawClientFrame (const uint8_t type);

  UserHandler&              mUserHandler;
  SessionStack              mStack;
  uint32_t                  mWaitingFrameId;
  uint32_t                  mClientCookie;
  uint32_t                  mServerCookie;
  uint16_t                  mLastReceivedCmd;
  uint16_t                  mFrameSize;
  uint8_t                   mVersion;
  uint8_t                   mCipher;
  uint_t                    mDataSize;
  std::vector<uint8_t>		mData;
  std::string         		mKey;


  ClientConnection (const ClientConnection&);
  const ClientConnection& operator= (const ClientConnection&);
};

#endif /* CONNECTION_H_ */

