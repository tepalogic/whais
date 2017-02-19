/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#include "whais.h"

#include "utils/endianness.h"
#include "utils/wrandom.h"
#include "utils/enc_3k.h"
#include "utils/enc_des.h"
#include "server/server_protocol.h"

#include "connection.h"


using namespace std;



static const uint32_t FRAME_SERVER_VERS  = 0x00000001;



ConnectionException::ConnectionException(const uint32_t  code,
                                          const char*     file,
                                          uint32_t        line,
                                          const char*     fmtMsg,
                                          ...)
  : Exception(code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start(vl, fmtMsg);
      this->Message(fmtMsg, vl);
      va_end(vl);
    }
}


Exception*
ConnectionException::Clone() const
{
  return new ConnectionException(*this);
}


EXCEPTION_TYPE
ConnectionException::Type() const
{
  return CONNECTION_EXCEPTION;
}


const char*
ConnectionException::Description() const
{
  return NULL;
}


ClientConnection::ClientConnection(UserHandler&            client,
                                    vector<DBSDescriptors>& databases)
  : mUserHandler(client),
    mStack(),
    mDataSize(GetAdminSettings().mMaxFrameSize),
    mData(mDataSize, 0),
    mWaitingFrameId(0),
    mClientCookie(0),
    mServerCookie(0),
    mLastReceivedCmd(CMD_INVALID),
    mFrameSize(0),
    mVersion(1),
    mCipher(FRAME_ENCTYPE_PLAIN)
{
  assert((mDataSize >= MIN_FRAME_SIZE) && (mDataSize <= MAX_FRAME_SIZE));

  if (mCipher != FRAME_ENCTYPE_PLAIN)
    mDataSize -= mDataSize % sizeof(uint64_t);

  mUserHandler.mDesc = NULL;
  const uint16_t authFrameLen = FRAME_HDR_SIZE + FRAME_AUTH_SIZE;

  assert(authFrameLen <= MIN_FRAME_SIZE);

  store_le_int16(MIN_FRAME_SIZE, &mData[FRAME_SIZE_OFF]);
  mData[FRAME_TYPE_OFF]    = FRAME_TYPE_AUTH_CLNT;
  mData[FRAME_ENCTYPE_OFF] = FRAME_ENCTYPE_PLAIN;
  store_le_int32(0, &mData[FRAME_ID_OFF]);

  store_le_int32(mVersion, &mData[FRAME_HDR_SIZE + FRAME_AUTH_VER_OFF]);
  store_le_int16(mDataSize, &mData[FRAME_HDR_SIZE + FRAME_AUTH_SIZE_OFF]);
  mData[FRAME_HDR_SIZE + FRAME_AUTH_ENC_OFF] = GetAdminSettings().mCipher;

  const uint64_t challenge = wh_rnd();
  store_le_int64(challenge, &mData[FRAME_HDR_SIZE + FRAME_AUTH_CHALLENGE_OFF]);

  mUserHandler.mSocket.Write(&mData.front(), MIN_FRAME_SIZE);
  ReciveRawClientFrame();

  mCipher = GetAdminSettings().mCipher;
  const uint32_t protocolVer = load_le_int32(
                              &mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_VER_OFF]
                                             );
  if ((mFrameSize < authFrameLen)
      || (protocolVer != mVersion)
      || (mData[FRAME_TYPE_OFF] != FRAME_TYPE_AUTH_CLNT_RSP)
      || (mData[FRAME_ENCTYPE_OFF] != FRAME_ENCTYPE_PLAIN))
    {
      throw ConnectionException(_EXTRA(0),
                                 "Unexpected authentication frame received.");
    }

  if (mCipher != mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_ENC_OFF])
    {
      throw ConnectionException(_EXTRA(0),
                                 "The cipher was not echoed back by client.");
    }

  if (load_le_int16(&mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_SIZE_OFF]) <
      mDataSize)
    {
      mDataSize = load_le_int16(&mData[FRAME_HDR_SIZE
                                          + FRAME_AUTH_RSP_SIZE_OFF]);
      if (mDataSize < MIN_FRAME_SIZE)
        {
          throw ConnectionException(
              _EXTRA(0),
              "Cannot use the client's specified frame size of %u bytes.",
              mDataSize
                                    );
        }

      if (mCipher != FRAME_ENCTYPE_PLAIN)
        mDataSize -= mDataSize % sizeof(uint64_t);
    }
  else if (mDataSize <
            load_le_int16(&mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_SIZE_OFF]))
    {
      throw ConnectionException(
            _EXTRA(0),
            "Client requested a frame size of %u bytes. The server's "
              "maximum frame size is set at %u bytes.",
            load_le_int16(&mData[FRAME_HDR_SIZE + FRAME_AUTH_SIZE_OFF]),
            mDataSize
                                );
    }

  const char* const dbsName = _RC(const char*,
                                   &mData[FRAME_HDR_SIZE
                                          + FRAME_AUTH_RSP_FIXED_SIZE]);
  for (vector<DBSDescriptors>::iterator it = databases.begin();
      it != databases.end();
      ++it)
    {
      if (strcmp(it->mDbsName.c_str(), dbsName) == 0)
        {
          mUserHandler.mDesc = &it[0];
          break;
        }
    }

  if (mUserHandler.mDesc == NULL)
    {
      throw ConnectionException(_EXTRA(0),
                                 "Failed to retrieve database '%s'.",
                                 dbsName);
    }

  mUserHandler.mRoot = (mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_USR_OFF] == 0)
                         ? true
                         : false;


  const string& password = mUserHandler.mRoot
                             ? mUserHandler.mDesc->mRootPass
                             : mUserHandler.mDesc->mUserPasswd;
  if (mCipher == FRAME_ENCTYPE_3K)
    {
      strncpy(_RC(char*, mKey._3K), password.c_str(), sizeof mKey);
      mKey._3K[sizeof mKey - 1] = 0;
    }
  else
    {
      wh_prepare_des_keys(_RC(const uint8_t*, password.c_str()),
                           password.length(),
                           mCipher == FRAME_ENCTYPE_3DES,
                           mKey._DES);
    }

  uint8_t challengeRsp[sizeof challenge];
  memcpy(challengeRsp,
          &mData[FRAME_HDR_SIZE + FRAME_AUTH_RSP_CHALLENGE_OFF],
          sizeof challengeRsp);
  wh_buff_des_decode(_RC(const uint8_t*, password.c_str()),
                      challengeRsp,
                      sizeof challengeRsp);

  if (load_le_int64(challengeRsp) != challenge)
    {
      throw ConnectionException(
                _EXTRA(0),
                mUserHandler.mRoot
                     ? "Failed to authenticate database '%s' root user."
                     : "Failed to authenticate database '%s' user.",
                dbsName
                               );
    }

  assert((mCipher == FRAME_ENCTYPE_PLAIN)
          || (mCipher == FRAME_ENCTYPE_3K)
          || (mCipher == FRAME_ENCTYPE_DES)
          || (mCipher == FRAME_ENCTYPE_3DES));
}


uint_t
ClientConnection::MaxSize() const
{
  assert((mCipher == FRAME_ENCTYPE_PLAIN)
          || (mCipher == FRAME_ENCTYPE_3K)
          || (mCipher == FRAME_ENCTYPE_DES)
          || (mCipher == FRAME_ENCTYPE_3DES));

  assert((MIN_FRAME_SIZE<= mDataSize) && (mDataSize <= MAX_FRAME_SIZE));

  uint_t metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
  if (mCipher != FRAME_ENCTYPE_PLAIN)
    metaDataSize += ENC_HDR_SIZE;

  return mDataSize - metaDataSize;
}


uint_t
ClientConnection::DataSize() const
{
  uint_t metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
  if (mCipher != FRAME_ENCTYPE_PLAIN)
    metaDataSize += ENC_HDR_SIZE;

  assert((mFrameSize == 0) || (mFrameSize >= metaDataSize));
  assert(mFrameSize <= mDataSize);

  return(mFrameSize == 0) ? 0 : (mFrameSize - metaDataSize);

}


uint8_t*
ClientConnection::Data()
{
  uint_t metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
  if (mCipher != FRAME_ENCTYPE_PLAIN)
    metaDataSize += ENC_HDR_SIZE;

  return &mData.front() + metaDataSize;
}


void
ClientConnection::DataSize(const uint16_t size)
{
  assert(size <= MaxSize());

  uint_t metaDataSize = FRAME_HDR_SIZE + PLAIN_HDR_SIZE;
  if (mCipher != FRAME_ENCTYPE_PLAIN)
    metaDataSize += ENC_HDR_SIZE;

  mFrameSize = size + metaDataSize;

  assert(mFrameSize <= mDataSize);
}


uint8_t*
ClientConnection::RawCmdData()
{
  uint_t metaDataSize = FRAME_HDR_SIZE;
  if (mCipher != FRAME_ENCTYPE_PLAIN)
    metaDataSize += ENC_HDR_SIZE;

  return &mData.front() + metaDataSize;
}


void
ClientConnection::ReciveRawClientFrame()
{
  uint16_t frameRead = 0;

  while(frameRead < FRAME_HDR_SIZE)
    {
      const uint16_t chunkSize =
                   mUserHandler.mSocket.Read(&mData[frameRead],
                                             FRAME_HDR_SIZE - frameRead);
      if (chunkSize == 0)
        throw ConnectionException(_EXTRA(0), "Connection reset by peer.");

      frameRead += chunkSize;
    }

  switch(mData[FRAME_TYPE_OFF])
  {
  case FRAME_TYPE_NORMAL:
  case FRAME_TYPE_AUTH_CLNT_RSP:
      mFrameSize = load_le_int16(&mData.front() + FRAME_SIZE_OFF);
      if ((mFrameSize < frameRead)
          || (mFrameSize > mDataSize))
        {
          throw ConnectionException(_EXTRA(0),
                                     "Invalid frame received.");
        }

      while(frameRead < mFrameSize)
        {
          const uint16_t chunkSize =
                       mUserHandler.mSocket.Read(&mData[frameRead],
                                                  mFrameSize - frameRead);
          if (chunkSize == 0)
            throw ConnectionException(_EXTRA(0),
                                       "Connection reset by peer.");

          frameRead += chunkSize;
        }

      assert(frameRead == mFrameSize);

      if (load_le_int32(&mData.front() + FRAME_ID_OFF) != mWaitingFrameId)
        {
          throw ConnectionException(_EXTRA(0),
                                     "Connection with peer is out of sync");

        }

      if (mCipher != mData[FRAME_ENCTYPE_OFF])
        throw ConnectionException(_EXTRA(0), "Peer has used a wrong cipher.");
    break;

  case FRAME_TYPE_TIMEOUT:
    throw ConnectionException(_EXTRA(0),
                               "Client peer has signaled a timeout condition.");
    break;

  default:
    assert(false);

    throw ConnectionException(_EXTRA(0),
                               "Unexpected frame type received.");
  }

  if (mData[FRAME_TYPE_OFF] == FRAME_TYPE_AUTH_CLNT_RSP)
    return ;

  if (mCipher == FRAME_ENCTYPE_3K)
    {
      const uint_t keyLen = strlen(_RC(const char*, mKey._3K));

      uint8_t prev = 0;
      for (uint_t i = 0; i < ENC_PLAIN_SIZE_OFF; ++i)
        {
          mData[FRAME_HDR_SIZE + i] ^= mKey._3K[prev % keyLen];
          prev = mData[FRAME_HDR_SIZE + i];
        }

      const uint32_t firstKing = load_le_int32(&mData.front()
                                                + FRAME_HDR_SIZE
                                                + ENC_3K_FIRST_KING_OFF);
      const uint32_t secondKing = load_le_int32(&mData.front()
                                                 + FRAME_HDR_SIZE
                                                 + ENC_3K_SECOND_KING_OFF);
      wh_buff_3k_decode(
                      firstKing,
                      secondKing,
                      mKey._3K,
                      keyLen,
                      &mData.front() + FRAME_HDR_SIZE + ENC_PLAIN_SIZE_OFF,
                      mFrameSize - (FRAME_HDR_SIZE + ENC_PLAIN_SIZE_OFF)
                         );

      const uint16_t plainSize = load_le_int16(&mData.front()
                                                  + FRAME_HDR_SIZE
                                                  + ENC_PLAIN_SIZE_OFF);
      mFrameSize = plainSize;
      store_le_int16(plainSize, &mData.front() + FRAME_SIZE_OFF);
    }
  else if (mCipher != FRAME_ENCTYPE_PLAIN)
    {
      if (mCipher == FRAME_ENCTYPE_DES)
        {
          wh_buff_des_decode_ex(mKey._DES,
                                 &mData.front() + FRAME_HDR_SIZE,
                                 mFrameSize - FRAME_HDR_SIZE);
        }
      else
        {
          assert(mCipher == FRAME_ENCTYPE_3DES);

          wh_buff_3des_decode_ex(mKey._DES,
                                  &mData.front() + FRAME_HDR_SIZE,
                                  mFrameSize - FRAME_HDR_SIZE);
        }

      const uint16_t plainSize = load_le_int16(&mData.front()
                                                  + FRAME_HDR_SIZE
                                                  + ENC_PLAIN_SIZE_OFF);
      mFrameSize = plainSize;
      store_le_int16(plainSize, &mData.front() + FRAME_SIZE_OFF);
    }
  else
    wh_buff_des_decode_ex(mKey._DES, RawCmdData(), sizeof(uint64_t));
}


void
ClientConnection::SendRawClientFrame(const uint8_t type)
{
  assert((mFrameSize >= FRAME_HDR_SIZE) && (mFrameSize <= mDataSize));

  if (mCipher == FRAME_ENCTYPE_3K)
    {
      const uint_t keyLen = strlen(_RC(const char*, mKey._3K));

      const uint16_t plainSize = mFrameSize;

      while(mFrameSize % sizeof(uint32_t) != 0)
        mData[mFrameSize++] = wh_rnd() & 0xFF;

      const uint32_t firstKing  = wh_rnd() & 0xFFFFFFFF;
      store_le_int32(firstKing, &mData.front()
                                 + FRAME_HDR_SIZE
                                 + ENC_3K_FIRST_KING_OFF);

      const uint32_t secondKing = wh_rnd() & 0xFFFFFFFF;
      store_le_int32(secondKing, &mData.front()
                                  + FRAME_HDR_SIZE
                                  + ENC_3K_SECOND_KING_OFF);

      uint8_t prev = 0;
      for (uint_t i = 0; i < ENC_PLAIN_SIZE_OFF; ++i)
        {
          const uint8_t temp = mData[FRAME_HDR_SIZE + i];

          mData[FRAME_HDR_SIZE + i] ^= mKey._3K[prev % keyLen];
          prev = temp;
        }

      store_le_int16(plainSize,
                      &mData.front() + FRAME_HDR_SIZE + ENC_PLAIN_SIZE_OFF);
      store_le_int16(wh_rnd() & 0xFFFF,
                      &mData.front() + FRAME_HDR_SIZE + ENC_SPARE_OFF);

      wh_buff_3k_encode(
                      firstKing,
                      secondKing,
                      mKey._3K,
                      keyLen,
                      &mData.front() + FRAME_HDR_SIZE + ENC_PLAIN_SIZE_OFF,
                      mFrameSize - (FRAME_HDR_SIZE + ENC_PLAIN_SIZE_OFF)
                         );
    }
  else if (mCipher != FRAME_ENCTYPE_PLAIN)
    {
      const uint16_t plainSize = mFrameSize;

      while(mFrameSize % sizeof(uint64_t) != 0)
        mData[mFrameSize++] = wh_rnd() & 0xFF;

      store_le_int16(plainSize,
                      &mData.front() + FRAME_HDR_SIZE + ENC_PLAIN_SIZE_OFF);

      if (mCipher == FRAME_ENCTYPE_DES)
        {
          wh_buff_des_encode_ex(mKey._DES,
                                 &mData.front() + FRAME_HDR_SIZE,
                                 mFrameSize - FRAME_HDR_SIZE);
        }
      else
        {
          assert(mCipher == FRAME_ENCTYPE_3DES);

          wh_buff_3des_encode_ex(mKey._DES,
                                  &mData.front() + FRAME_HDR_SIZE,
                                  mFrameSize - FRAME_HDR_SIZE);
        }
    }
  else
    wh_buff_des_encode_ex(mKey._DES, RawCmdData(), sizeof(uint64_t));

  store_le_int16(mFrameSize, &mData.front() + FRAME_SIZE_OFF);
  store_le_int32(++mWaitingFrameId, &mData.front() + FRAME_ID_OFF);

  mData[FRAME_TYPE_OFF]    = type;
  mData[FRAME_ENCTYPE_OFF] = mCipher;

  mUserHandler.mSocket.Write(&mData.front(), mFrameSize);

  mFrameSize    = 0;  //This frame content is not valid anymore.
  mClientCookie = ~0; //Make sure the client cookie is reread.
}


uint32_t
ClientConnection::ReadCommand()
{
  ReciveRawClientFrame();

  const uint32_t servCookie = load_le_int32(
                                RawCmdData() + PLAIN_SERV_COOKIE_OFF
                                            );
  if (servCookie != mServerCookie)
    {
      throw ConnectionException(_EXTRA(0),
                                 "Peer context cannot be recognized.");
    }

  uint16_t        chkSum   = 0;
  const uint16_t  respSize = DataSize();
  for (uint_t i = 0; i < respSize; i++)
    chkSum += Data()[i];

  if (chkSum != load_le_int16(RawCmdData() + PLAIN_CRC_OFF))
    throw ConnectionException(_EXTRA(0),
                               "Frame with invalid CRC received.");

  mClientCookie    = load_le_int32(RawCmdData() + PLAIN_CLNT_COOKIE_OFF);
  mLastReceivedCmd = load_le_int16(RawCmdData() + PLAIN_TYPE_OFF);

  assert((mLastReceivedCmd & 1) == 0);

  return mLastReceivedCmd;
}

void
ClientConnection::SendCmdResponse(const uint16_t respType)
{
  assert((respType & 1) != 0);
  assert((mLastReceivedCmd + 1) == respType);

  const uint16_t respSize = DataSize();
  mServerCookie = wh_rnd();

  store_le_int32(mClientCookie, RawCmdData() + PLAIN_CLNT_COOKIE_OFF);
  store_le_int32(mServerCookie, RawCmdData() + PLAIN_SERV_COOKIE_OFF);
  store_le_int16(respType, RawCmdData() + PLAIN_TYPE_OFF);

  uint32_t chkSum = 0;
  for (uint_t i = 0; i < respSize; i++)
    chkSum += Data()[i];

  store_le_int16(chkSum, RawCmdData() + PLAIN_CRC_OFF);

  SendRawClientFrame(FRAME_TYPE_NORMAL);
}

