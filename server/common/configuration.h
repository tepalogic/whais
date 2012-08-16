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

#ifndef CONFIGURATION_H__
#define CONFIGURATION_H_

#include <string>
#include <fstream>

#include "whisper.h"

#include "utils/include/logger.h"

struct AdminSettings
{
  AdminSettings ()
    : m_ListenPort (0),
      m_TableCacheBlockSize (0),
      m_TableCacheBlockCount (0),
      m_VLBlockSize (0),
      m_VLBlockCount (0),
      m_TempValuesCache (0),
      m_MaxTableCacheBlockSize (0),
      m_MaxTableCacheBlockCount (0),
      m_MaxVLBlockSize (0),
      m_MaxVLBlockCount (0),
      m_MaxTempValuesCache (0),
      m_TempDirectory (),
      m_LogFile (),
      m_ShowDebugLog (false)
  {
  }

  D_UINT      m_ListenPort;
  D_UINT      m_TableCacheBlockSize;
  D_UINT      m_TableCacheBlockCount;
  D_UINT      m_VLBlockSize;
  D_UINT      m_VLBlockCount;
  D_UINT      m_TempValuesCache;
  D_UINT      m_MaxTableCacheBlockSize;
  D_UINT      m_MaxTableCacheBlockCount;
  D_UINT      m_MaxVLBlockSize;
  D_UINT      m_MaxVLBlockCount;
  D_UINT      m_MaxTempValuesCache;
  std::string m_TempDirectory;
  std::string m_LogFile;
  bool        m_ShowDebugLog;
};

struct SessionSettings
{
  SessionSettings ()
    : m_TableCacheBlockSize (0),
      m_TableCacheBlockCount (0),
      m_VLBlockSize (0),
      m_VLBlockCount (0),
      m_DBSDirectory (),
      m_TempDirectory (),
      m_ShowDebugLog (false)
  {
  }

  D_UINT      m_TableCacheBlockSize;
  D_UINT      m_TableCacheBlockCount;
  D_UINT      m_VLBlockSize;
  D_UINT      m_VLBlockCount;
  D_UINT      m_TempValuesCache;
  std::string m_Name;
  std::string m_DBSDirectory;
  std::string m_TempDirectory;
  bool        m_ShowDebugLog;
};

const AdminSettings&
GetAdminSettings ();

bool
SeekAtGlobalSection (std::ifstream& config, D_UINT& oSectionLine);

bool
FindNextSessionSection (std::ifstream& config, D_UINT& ioSectionLine);

bool
ParseMainSection (std::ifstream& config, D_UINT& ioSectionLine);

bool
ParseSessionSection (Logger&          log,
                     std::ifstream&   config,
                     D_UINT&          ioConfigLine,
                     SessionSettings& output);

bool
FixMainSection (Logger& log);

bool
FixSessionSection (Logger& log, SessionSettings& ioSession);

#endif /* CONFIGURATION_H_ */
