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

#include "dbs/include/dbs_mgr.h"
#include "interpreter/include/interpreter.h"
#include "utils/include/logger.h"

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
      m_ShowDebugLog (false)
  {
  }

  D_UINT      m_MaxConnections;
  D_UINT      m_MaxFrameSize;
  D_UINT      m_TableCacheBlockSize;
  D_UINT      m_TableCacheBlockCount;
  D_UINT      m_VLBlockSize;
  D_UINT      m_VLBlockCount;
  D_UINT      m_TempValuesCache;
  std::string m_WorkDirectory;
  std::string m_TempDirectory;
  std::string m_LogFile;

  std::vector<ListenEntry> m_Listens;

  bool        m_ShowDebugLog;

};

struct DBSDescriptors
{
  DBSDescriptors (const D_UINT configLine)
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

  D_UINT                   m_ConfigLine;
  std::string              m_DbsName;
  std::string              m_DbsDirectory;
  std::string              m_DbsLogFile;
  std::string              m_RootPass;
  std::string              m_UserPasswd;
  std::vector<std::string> m_ObjectLibs;
  std::vector<std::string> m_NativeLibs;

  I_DBSHandler*            m_Dbs;
  I_Session*               m_Session;

  I_Logger*                m_pLogger;
};

const std::string&
GlobalContextDatabase ();

const ServerSettings&
GetAdminSettings ();

bool
SeekAtConfigurationSection (std::ifstream& config, D_UINT& oConfigLine);

bool
FindNextContextSection (std::ifstream& config, D_UINT& ioConfigLine);

bool
ParseConfigurationSection (std::ifstream& config, D_UINT& ioConfigLine);

bool
ParseContextSection (I_Logger&        log,
                     std::ifstream&   config,
                     D_UINT&          ioConfigLine,
                     DBSDescriptors&  output);

bool
PrepareConfigurationSection (I_Logger& log);

bool
PrepareContextSection (I_Logger& log, DBSDescriptors& ioDesc);

#endif /* CONFIGURATION_H_ */
