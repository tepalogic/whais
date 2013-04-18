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
  std::string m_Interface;
  std::string m_Service;
};

struct ServerSettings
{
  ServerSettings ()
    : m_MaxConnections (0),
      m_MaxFrameSize (0),
      m_TableCacheBlockSize (0),
      m_TableCacheBlockCount (0),
      m_VLBlockSize (0),
      m_VLBlockCount (0),
      m_TempValuesCache (0),
      m_WorkDirectory (),
      m_TempDirectory (),
      m_LogFile (),
      m_Listens (),
      m_Cipher (0),
      m_ShowDebugLog (false)
  {
  }

  uint_t      m_MaxConnections;
  uint_t      m_MaxFrameSize;
  uint_t      m_TableCacheBlockSize;
  uint_t      m_TableCacheBlockCount;
  uint_t      m_VLBlockSize;
  uint_t      m_VLBlockCount;
  uint_t      m_TempValuesCache;
  std::string m_WorkDirectory;
  std::string m_TempDirectory;
  std::string m_LogFile;

  std::vector<ListenEntry> m_Listens;

  uint8_t     m_Cipher;
  bool        m_ShowDebugLog;

};

struct DBSDescriptors
{
  DBSDescriptors (const uint_t configLine)
    : m_ConfigLine (configLine),
      m_DbsName (),
      m_DbsDirectory (),
      m_ObjectLibs (),
      m_NativeLibs (),
      m_Dbs (NULL),
      m_Session (NULL),
      m_pLogger (NULL)
  {
  }

  uint_t                   m_ConfigLine;
  std::string              m_DbsName;
  std::string              m_DbsDirectory;
  std::string              m_DbsLogFile;
  std::string              m_RootPass;
  std::string              m_UserPasswd;
  std::vector<std::string> m_ObjectLibs;
  std::vector<std::string> m_NativeLibs;

  whisper::I_DBSHandler*            m_Dbs;
  whisper::I_Session*               m_Session;

  whisper::Logger*                m_pLogger;
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
