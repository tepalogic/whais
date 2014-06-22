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

#include "utils/endianness.h"
#include "utils/wrandom.h"
#include "utils/enc_3k.h"
#include "server/server_protocol.h"

#include "connection.h"


using namespace std;



static const uint32_t FRAME_SERVER_VERS  = 0x00000001;



ConnectionException::ConnectionException (const uint32_t  code,
                                          const char*     file,
                                          uint32_t        line,
                                          const char*     fmtMsg,
                                          ...)
  : Exception (code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start (vl, fmtMsg);
      this->Message (fmtMsg, vl);
      va_end (vl);
    }
}


Exception*
ConnectionException::Clone () const
{
  return new ConnectionException (*this);
}


EXCEPTION_TYPE
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
  : mUserHandler (client),
    mStack (),
    mWaitingFrameId (0),
    mClientCookie (0),
    mServerCookie (0),
    mLastReceivedCmd (CMD_INVALID),
    mFrameSize (0),
    mVersion (1),
    mCipher (FRAME_ENCTYPE_PLAIN),
    mDataSize (GetAdminSettings ().mMaxFrameSize),
    mData (new uint8_t[mDataSize])
{
  assert ((mDataSize >= MIN_FRAME_SIZE) && (mDataSize <= MAX_FRAME_SIZE));

  if (mCipher == FRAME_ENCTYPE_3K)
    mDataSize -= mDataSize % sizeof (uint32_t);

  mUserHandler.mDesc = NULL;
  const uint16_t authFrameLen = FRAME_HDR_SIZE + FRAME_AUTH_SIZE;

  memset (mData, 0, mDataSize);

  assert (authFrameLen <= MIN_FRAME_SIZE);

  store_le_int16 (MIN_FRAME_SIZE, &mData[FRAME_SIZE_OFF]);
  mData[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT;
  mData[FRAME_ENCTYPE_OFF] = FRAME_ENCTYPE_PLAIN;
  store_le_int32 (0, &mData[FRAME_ID_OFF]);

  store_le_int32 (mVersion, &mData[FRAME_HDR_SIZE + FRAME_AUTH_VER_OFF]);
  store_le_int16 (mDataSize, &mData[FRAME_HDR_SIZE + FRAME_AUTH_SIZE_OFF]);
  mData[FRAME_HDR_SIZE + FRAME_AUTH_ENC_OFF] = GetAdminSettings().mCipher;;

  mUserHandler.mSocket.Write (mData, MIN_FRAME_SIZE);
  ReciveRawClientFrame ();

  mCipher = GetAdminSettings().mCipher;
  const uint32_t protocolVer = load_le_int32 (
                              &mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_VER_OFF]
                                             );
  if ((mFrameSize < authFrameLen)
      || (protocolVer != mVersion)
      || (mData[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT_RSP)
      || (mData[FRAME_ENCTYPE_OFF] != FRAME_ENCTYPE_PLAIN))
    {
      throw ConnectionException (_EXTRA (0),
                                 "Unexpected authentication frame received.");
    }

  if (mCipher != mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_ENC_OFF])
    {
      throw ConnectionException (_EXTRA (0),
                                 "The cipher was not echoed back by client.");
    }

  const char* dbsName = _RC (const char*,
                             &mData[FRAME_HDR_SIZE +
                                    FRAME_AUTH_RSP_FIXED_SIZE]);

  for (vector<DBSDescriptors>::iterator it = databases.begin ();
      it != databases.end ();
      ++it)
    {
      if (strcmp (it->mDbsName.c_str (), dbsName) == 0)
        {
          mUserHandler.mDesc = &it[0];
          break;
        }
    }

  if (mUserHandler.mDesc == NULL)
    {
      throw ConnectionException (_EXTRA (0),
                                 "Failed to retrieve database '%s'.",
                                 dbsName);
    }

  mUserHandler.mRoot =
        (mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_USR_OFF] == 0) ? true : false;

  if (mCipher == FRAME_ENCTYPE_PLAIN)
    {
      const string passwd = _RC (const char*,
                                 mData +
                                   FRAME_HDR_SIZE +
                                   FRAME_AUTH_RSP_FIXED_SIZE +
                                   strlen (dbsName) + 1);
      if (mUserHandler.mRoot)
        {
          if (mUserHandler.mDesc->mRootPass != passwd)
            {
              throw ConnectionException (
                        _EXTRA (0),
                        "Failed to authenticate database ('%s') root user.",
                        dbsName
                                        );

            }
          else
            mKey = mUserHandler.mDesc->mRootPass;
        }
      else
        {
          if (mUserHandler.mDesc->mUserPasswd != passwd)
            {
              throw ConnectionException (
                            _EXTRA (0),
                            "Failed to authenticate database ('%s') user.",
                            dbsName
                                        );
            }
          else
            mKey = mUserHandler.mDesc->mUserPasswd;
        }
    }
  else if (mCipher == FRAME_ENCTYPE_3K)
    {
      if (mUserHandler.mRoot)
         mKey = mUserHandler.mDesc->mRootPass;

      else
        mKey = mUserHandler.mDesc->mUserPasswd;
    }
  else
    {
      assert (false);
    }

}


ClientConnection::~ClientConnection ()
{
  delete [] mData;
}


uint_t
ClientConnection::MaxSize () const
{
  assert ((mCipher == FRAME_ENCTYPE_PLAIN) || (mCipher == FRAME_ENCTYPE_3K));
  assert ((MIN_FRAME_SIZE<= mDataSize) && (mDataSize <= MAX_FRAME_SIZE));

  uint_t metaDataSize = 0;

  switch (mCipher)
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

  return mDataSize - metaDataSize;
}


uint_t
ClientConnection::DataSize () const
{
  uint_t metaDataSize = 0;

  switch (mCipher)
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

  assert ((mFrameSize == 0) || (mFrameSize >= metaDataSize));
  assert (mFrameSize <= mDataSize);

  return (mFrameSize == 0) ? 0 : (mFrameSize - metaDataSize);

}


uint8_t*
ClientConnection::Data ()
{
  uint_t metaDataSize = 0;

  switch (mCipher)
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

  return mData + metaDataSize;
}


void
ClientConnection::DataSize (const uint16_t size)
{
  assert (size <= MaxSize ());

  uint_t metaDataSize = 0;

  switch (mCipher)
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

  mFrameSize = size + metaDataSize;

  assert (mFrameSize <= mDataSize);
}


uint8_t*
ClientConnection::RawCmdData ()
{
  if (mCipher == FRAME_ENCTYPE_PLAIN)
    return mData + FRAME_HDR_SIZE;

  else if (mCipher == FRAME_ENCTYPE_3K)
    return mData + FRAME_HDR_SIZE + ENC_3K_HDR_SIZE;

  return NULL;
}


void
ClientConnection::ReciveRawClientFrame ()
{
  uint16_t frameRead = 0;

  while (frameRead < FRAME_HDR_SIZE)
    {
      const uint16_t chunkSize =
                   mUserHandler.mSocket.Read (&mData[frameRead],
                                             FRAME_HDR_SIZE - frameRead);
      if (chunkSize == 0)
        throw ConnectionException (_EXTRA (0), "Connection reset by peer.");

      frameRead += chunkSize;
    }

  switch (mData[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
  case FRAME_TYPE_AUTH_CLNT_RSP:
    //These are legitimate frame types that could be received by a server.
    break;

  case FRAME_TYPE_TIMEOUT:
    throw ConnectionException (_EXTRA (0),
                               "Client peer has signaled a timeout condition.");
    break;

  default:
    assert (false);

    throw ConnectionException (_EXTRA (0),
                               "Unexpected frame type received.");
  }

  mFrameSize = load_le_int16 (mData + FRAME_SIZE_OFF);

  if ((mFrameSize < frameRead)
      || (mFrameSize > mDataSize))
    {
      throw ConnectionException (_EXTRA (0),
                                 "Invalid frame received.");
    }

  while (frameRead < mFrameSize)
    {
      const uint16_t chunkSize =
                   mUserHandler.mSocket.Read (&mData[frameRead],
                                              mFrameSize - frameRead);
      if (chunkSize == 0)
        throw ConnectionException (_EXTRA (0),
                                   "Connection reset by peer.");

      frameRead += chunkSize;
    }

  assert (frameRead == mFrameSize);

  const uint32_t frameId = load_le_int32 (mData + FRAME_ID_OFF);
  if (frameId != mWaitingFrameId)
    {
      throw ConnectionException (_EXTRA (0),
                                 "Connection with peer is out of sync");

    }

  const uint32_t encType = mData[FRAME_ENCTYPE_OFF];
  if (encType != mCipher)
    throw ConnectionException (_EXTRA (0), "Peer has used a wrong cipher.");

  if (encType == FRAME_ENCTYPE_3K)
    {
      uint8_t prev = 0;
      for (uint_t i = 0; i < ENC_3K_PLAIN_SIZE_OFF; ++i)
        {
          mData[FRAME_HDR_SIZE + i] ^= mKey.at (prev % mKey.length ());
          prev = mData[FRAME_HDR_SIZE + i];
        }

      const uint32_t firstKing = load_le_int32 (mData +
                                                  FRAME_HDR_SIZE +
                                                  ENC_3K_FIRST_KING_OFF);
      const uint32_t secondKing = load_le_int32 (mData +
                                                   FRAME_HDR_SIZE +
                                                   ENC_3K_SECOND_KING_OFF);
      wh_buff_3k_decode (
                      firstKing,
                      secondKing,
                      _RC (const uint8_t*, mKey.c_str ()),
                      mKey.length (),
                      mData + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF,
                      mFrameSize - (FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF)
                         );

      const uint16_t plainSize = load_le_int16 (mData +
                                                  FRAME_HDR_SIZE +
                                                  ENC_3K_PLAIN_SIZE_OFF);
      mFrameSize = plainSize;
      store_le_int16 (plainSize, mData + FRAME_SIZE_OFF);
    }
}


void
ClientConnection::SendRawClientFrame (const uint8_t type)
{
  assert ((mFrameSize >= FRAME_HDR_SIZE) && (mFrameSize <= mDataSize));


  if (mCipher == FRAME_ENCTYPE_3K)
    {
      const uint16_t plainSize = mFrameSize;

      while (mFrameSize % sizeof (uint32_t) != 0)
        mData[mFrameSize++] = wh_rnd () & 0xFF;

      const uint32_t firstKing  = wh_rnd () & 0xFFFFFFFF;
      store_le_int32 (firstKing,
                      mData + FRAME_HDR_SIZE + ENC_3K_FIRST_KING_OFF);

      const uint32_t secondKing = wh_rnd () & 0xFFFFFFFF;
      store_le_int32 (secondKing,
                      mData + FRAME_HDR_SIZE + ENC_3K_SECOND_KING_OFF);

      uint8_t prev = 0;
      for (uint_t i = 0; i < ENC_3K_PLAIN_SIZE_OFF; ++i)
        {
          const uint8_t temp = mData[FRAME_HDR_SIZE + i];

          mData[FRAME_HDR_SIZE + i] ^= mKey.at (prev % mKey.length ());
          prev = temp;
        }

      store_le_int16 (plainSize,
                      mData + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF);
      store_le_int16 (wh_rnd () & 0xFFFF,
                      mData + FRAME_HDR_SIZE + ENC_3K_SPARE_OFF);

      wh_buff_3k_encode (
                      firstKing,
                      secondKing,
                      _RC (const uint8_t*, mKey.c_str ()),
                      mKey.length (),
                      mData + FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF,
                      mFrameSize - (FRAME_HDR_SIZE + ENC_3K_PLAIN_SIZE_OFF)
                         );
    }

  store_le_int16 (mFrameSize, mData + FRAME_SIZE_OFF);
  store_le_int32 (++mWaitingFrameId, mData + FRAME_ID_OFF);

  mData[FRAME_TYPE_OFF]    = type;
  mData[FRAME_ENCTYPE_OFF] = mCipher;

  mUserHandler.mSocket.Write (mData, mFrameSize);

  mFrameSize    = 0;  //This frame content is not valid anymore.
  mClientCookie = ~0; //Make sure the client cookie is reread.
}


uint32_t
ClientConnection::ReadCommand ()
{
  ReciveRawClientFrame ();

  const uint32_t servCookie = load_le_int32 (
                                RawCmdData () + PLAIN_SERV_COOKIE_OFF
                                            );
  if (servCookie != mServerCookie)
    {
      throw ConnectionException (_EXTRA (0),
                                 "Peer context cannot be recognized.");
    }

  uint16_t        chkSum   = 0;
  const uint16_t  respSize = DataSize ();
  for (uint_t i = 0; i < respSize; i++)
    chkSum += Data ()[i];

  if (chkSum != load_le_int16 (RawCmdData () + PLAIN_CRC_OFF))
    throw ConnectionException (_EXTRA (0),
                               "Frame with invalid CRC received.");

  mClientCookie    = load_le_int32 (RawCmdData () + PLAIN_CLNT_COOKIE_OFF);
  mLastReceivedCmd = load_le_int16 (RawCmdData () + PLAIN_TYPE_OFF);

  assert ((mLastReceivedCmd & 1) == 0);

  return mLastReceivedCmd;
}

void
ClientConnection::SendCmdResponse (const uint16_t respType)
{
  assert ((respType & 1) != 0);
  assert ((mLastReceivedCmd + 1) == respType);

  const uint16_t respSize = DataSize ();
  mServerCookie = wh_rnd ();

  store_le_int32 (mClientCookie, RawCmdData () + PLAIN_CLNT_COOKIE_OFF);
  store_le_int32 (mServerCookie, RawCmdData () + PLAIN_SERV_COOKIE_OFF);
  store_le_int16 (respType, RawCmdData () + PLAIN_TYPE_OFF);

  uint32_t chkSum = 0;
  for (uint_t i = 0; i < respSize; i++)
    chkSum += Data ()[i];

  store_le_int16 (chkSum, RawCmdData () + PLAIN_CRC_OFF);

  SendRawClientFrame (FRAME_TYPE_NORMAL);
}

