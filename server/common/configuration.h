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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include <fstream>

#include "whais.h"

#include "dbs/dbs_mgr.h"
#include "interpreter/interpreter.h"
#include "utils/logger.h"

#include "server_protocol.h"

#define UNSET_VALUE             0
#define DEFAULT_MAX_STACK_CNT   4096

struct ListenEntry
{
  std::string mInterface;
  std::string mService;
};



struct ServerSettings
{
  ServerSettings()
    : mMaxConnections (UNSET_VALUE),
      mMaxFrameSize (UNSET_VALUE),
      mTableCacheBlockSize (UNSET_VALUE),
      mTableCacheBlockCount (UNSET_VALUE),
      mVLBlockSize (UNSET_VALUE),
      mVLBlockCount (UNSET_VALUE),
      mTempValuesCache (UNSET_VALUE),
      mAuthTMO (UNSET_VALUE),
      mSyncWakeup (UNSET_VALUE),
      mSyncInterval (UNSET_VALUE),
      mWaitReqTmo (UNSET_VALUE),
      mWorkDirectory(),
      mTempDirectory(),
      mLogFile(),
      mListens(),
      mCipher (UNSET_VALUE),
      mShowDebugLog (false)
  {
  }

  uint_t                   mMaxConnections;
  uint_t                   mMaxFrameSize;
  uint_t                   mTableCacheBlockSize;
  uint_t                   mTableCacheBlockCount;
  uint_t                   mVLBlockSize;
  uint_t                   mVLBlockCount;
  uint_t                   mTempValuesCache;
  int                      mAuthTMO;
  int                      mSyncWakeup;
  int                      mSyncInterval;
  int                      mWaitReqTmo;
  std::string              mWorkDirectory;
  std::string              mTempDirectory;
  std::string              mLogFile;
  std::vector<ListenEntry> mListens;
  uint8_t                  mCipher;
  bool                     mShowDebugLog;

};




struct DBSDescriptors
{
  DBSDescriptors (const uint_t configLine)
    : mConfigLine (configLine),
      mSyncInterval (UNSET_VALUE),
      mWaitReqTmo (UNSET_VALUE),
      mStackCount (DEFAULT_MAX_STACK_CNT),
      mDbsName(),
      mDbsDirectory(),
      mObjectLibs(),
      mNativeLibs(),
      mDbs (NULL),
      mSession (NULL),
      mLogger (NULL),
      mLastFlushTick (0)
  {
  }

  uint_t                           mConfigLine;
  int                              mSyncInterval;
  int                              mWaitReqTmo;
  uint_t                           mStackCount;
  std::string                      mDbsName;
  std::string                      mDbsDirectory;
  std::string                      mDbsLogFile;
  std::string                      mRootPass;
  std::string                      mUserPasswd;
  std::vector<std::string>         mObjectLibs;
  std::vector<std::string>         mNativeLibs;
  whais::IDBSHandler*              mDbs;
  whais::ISession*                 mSession;
  whais::Logger*                   mLogger;
  uint64_t                         mLastFlushTick;
};

const std::string&
GlobalContextDatabase();

const ServerSettings&
GetAdminSettings();

bool
SeekAtConfigurationSection (std::ifstream& config, uint_t& outConfigLine);

bool
FindNextContextSection (std::ifstream& config, uint_t& inoutConfigLine);

bool
ParseConfigurationSection (std::ifstream& config, uint_t& inoutConfigLine);

bool
ParseContextSection (whais::Logger&        log,
                     std::ifstream&          config,
                     uint_t&                 inoutConfigLine,
                     DBSDescriptors&         output);

bool
PrepareConfigurationSection (whais::Logger& log);

bool
PrepareContextSection (whais::Logger& log, DBSDescriptors& inoutDesc);

#endif /* CONFIGURATION_H_ */

