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

#include "logger.h"

using namespace std;

namespace whais
{

//A global variable to used every one when you need one
NullLogger NULL_LOGGER;



FileLogger::FileLogger(const char* const file, const bool printStart)
  : mStartTick(wh_msec_ticks()),
    mLogFile(file),
    mTodayTime(wh_get_currtime())
{
  mLogFile.append(".wlog");
  SwitchFile();

  if (printStart)
  {
    const WTime dayStart = wh_get_currtime();

    mOutStream << "\n* Server start on: " << (int)dayStart.year
               << '-' << (int)dayStart.month
               << "-" << (int)dayStart.day
               << ' ' << (int)dayStart.hour
               << ':' << (int)dayStart.min
               << ':' << (int)dayStart.sec << "\n\n";
  }

  if (! mOutStream.good())
    throw ios_base::failure("The file associated with the output stream could not be opened.");
}

void
FileLogger::Log(const LOG_TYPE type, const char* str)
{
  LockGuard<Lock> holder(mSync);

  const int markSize = PrintTimeMark(type);

  /* Print white spaces where the time mark should have been for
     for string messages that have more than one line, to keep
     a mice indentation. */
  while (*str != 0)
  {
    if ((*str == '\n') && (*(str + 1) != '\n'))
      {
        mOutStream << endl;
        for (int i = 0; i < markSize; ++i)
          mOutStream << ' ';
      }
    else
      mOutStream << *str;

    ++str;
  }

  mOutStream << endl;
  mOutStream.flush();
}


void
FileLogger::Log(const LOG_TYPE type, const string& str)
{
  Log(type, str.c_str());
}


uint_t
FileLogger::PrintTimeMark(LOG_TYPE type)
{
  static char logIds[] = { '!', 'C', 'E', 'W', 'I', 'D' };

  if (type > LT_DEBUG)
    type = LT_UNKNOW;

  const WTime ctime = wh_get_currtime();

  if ((ctime.day != mTodayTime.day)
      || (ctime.month != mTodayTime.month)
      || (ctime.year != mTodayTime.year))
  {
    SwitchFile();
    mTodayTime = ctime;

    mOutStream << "\n* Hello on: " << (int)mTodayTime.year
               << '-' << (int)mTodayTime.month
               << "-" << (int)mTodayTime.day
               << ' ' << (int)mTodayTime.hour
               << ':' << (int)mTodayTime.min
               << ':' << (int)mTodayTime.sec << "\n\n";
  }

  const char       fill  = mOutStream.fill();
  const streamsize width = mOutStream.width();

  mOutStream << '(' << logIds [type] << ')';
  mOutStream.fill('0');
  mOutStream.width(2);
  mOutStream << (uint_t) ctime.hour << ':';
  mOutStream.width(2);
  mOutStream << (uint_t) ctime.min << ':';
  mOutStream.width(2);
  mOutStream << (uint_t) ctime.sec << '.';
  mOutStream.width(6);
  mOutStream << (uint_t) ctime.usec << ": ";

  mOutStream.fill(fill);
  mOutStream.width(width);

  return 3 + 2 + 1 + 2 + 1 + 2 + 1 + 6 + 2;
}


void
FileLogger::SwitchFile()
{
  if (mOutStream.is_open())
  {
    mOutStream.close();
    string oldFile;

    oldFile.resize(mLogFile.length() + 16);

    sprintf(_CC(char*, oldFile.c_str()),
            "%s.%04u%02u%02u",
            mLogFile.c_str(),
            mTodayTime.year,
            mTodayTime.month,
            mTodayTime.day);

    whf_move_file(mLogFile.c_str(), oldFile.c_str());
  }

  mOutStream.open(mLogFile.c_str(), ios::app | ios::out);
}


void
NullLogger::Log(const LOG_TYPE, const char*)
{
}


void
NullLogger::Log(const LOG_TYPE, const string&)
{
}


} //namespace whais
