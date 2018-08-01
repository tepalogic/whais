/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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

#ifndef LOGGER_H_
#define LOGGER_H_


#include <fstream>

#include "wthread.h"


namespace whais
{


enum LOG_TYPE
{
  LT_UNKNOW,
  LT_CRITICAL,
  LT_ERROR,
  LT_WARNING,
  LT_INFO,
  LT_DEBUG
};

class Logger
{
public:
  virtual ~Logger() = default;

  virtual void Log(const LOG_TYPE type, const char* str) = 0;
  virtual void Log(const LOG_TYPE type, const std::string& str) = 0;
};


class FileLogger : public Logger
{
public:
  FileLogger(const char* const file, const bool printStart = true);

  void Log(const LOG_TYPE type, const char* str);
  void Log(const LOG_TYPE type, const std::string& str);

private:
  FileLogger(const Logger&);
  FileLogger& operator= (const Logger&);

  uint_t PrintTimeMark(LOG_TYPE type);
  void   SwitchFile();

  WTICKS        mStartTick;
  Lock          mSync;
  std::ofstream mOutStream;
  std::string   mLogFile;
  WTime         mTodayTime;
};


class NullLogger : public Logger
{
public:
  void Log(const LOG_TYPE type, const char* str);
  void Log(const LOG_TYPE type, const std::string& str);
};

extern NullLogger NULL_LOGGER;


} //namespace whais


#endif /* LOGGER_H_ */
