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

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include <fstream>

#include "whisper.h"

#include "dbs/dbs_mgr.h"
#include "interpreter/interpreter.h"
#include "utils/logger.h"

#include "server_protocol.h"

struct ListenEntry
{
  std::string mInterface;
  std::string mService;
};

struct ServerSettings
{
  ServerSettings ()
    : mMaxConnections (0),
      mMaxFrameSize (0),
      mTableCacheBlockSize (0),
      mTableCacheBlockCount (0),
      mVLBlockSize (0),
      mVLBlockCount (0),
      mTempValuesCache (0),
      mWorkDirectory (),
      mTempDirectory (),
      mLogFile (),
      mListens (),
      mCipher (0),
      mShowDebugLog (false)
  {
  }

  uint_t      mMaxConnections;
  uint_t      mMaxFrameSize;
  uint_t      mTableCacheBlockSize;
  uint_t      mTableCacheBlockCount;
  uint_t      mVLBlockSize;
  uint_t      mVLBlockCount;
  uint_t      mTempValuesCache;
  std::string mWorkDirectory;
  std::string mTempDirectory;
  std::string mLogFile;

  std::vector<ListenEntry> mListens;

  uint8_t     mCipher;
  bool        mShowDebugLog;

};

struct DBSDescriptors
{
  DBSDescriptors (const uint_t configLine)
    : mConfigLine (configLine),
      mDbsName (),
      mDbsDirectory (),
      mObjectLibs (),
      mNativeLibs (),
      mDbs (NULL),
      mSession (NULL),
      mpLogger (NULL)
  {
  }

  uint_t                   mConfigLine;
  std::string              mDbsName;
  std::string              mDbsDirectory;
  std::string              mDbsLogFile;
  std::string              mRootPass;
  std::string              mUserPasswd;
  std::vector<std::string> mObjectLibs;
  std::vector<std::string> mNativeLibs;

  whisper::IDBSHandler*            mDbs;
  whisper::ISession*               mSession;

  whisper::Logger*                mpLogger;
};

const std::string&
GlobalContextDatabase ();

const ServerSettings&
GetAdminSettings ();

bool
SeekAtConfigurationSection (std::ifstream& config, uint_t& oConfigLine);

bool
FindNextContextSection (std::ifstream& config, uint_t& ioConfigLine);

bool
ParseConfigurationSection (std::ifstream& config, uint_t& ioConfigLine);

bool
ParseContextSection (whisper::Logger&        log,
                     std::ifstream&   config,
                     uint_t&          ioConfigLine,
                     DBSDescriptors&  output);

bool
PrepareConfigurationSection (whisper::Logger& log);

bool
PrepareContextSection (whisper::Logger& log, DBSDescriptors& ioDesc);

#endif /* CONFIGURATION_H_ */
