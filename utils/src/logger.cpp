/******************************************************************************
WHAISC - A compiler for whais programs
Copyright (C) 2009  Iulian Popa

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



Logger::~Logger()
{
}


FileLogger::FileLogger( const char* const file, const bool printStart)
  : Logger(),
    mStartTick( wh_msec_ticks()),
    mSync(),
    mOutStream(),
    mLogFile( file),
    mTodayTime( wh_get_currtime())
{
  if (! mOutStream.good())
    {
      throw ios_base::failure( "The file associated with the output stream "
                               "could not be opened");
    }

  mLogFile.append( ".wlog.yyyymmdd");
  SwitchFile();

  if (printStart)
    {
      const WTime dayStart = wh_get_currtime();

      mOutStream << "\n* Start of the day: " << (int)dayStart.year;
      mOutStream << '-' << (int)dayStart.month;
      mOutStream << "-" << (int)dayStart.day;
      mOutStream << ' ' << (int)dayStart.hour;
      mOutStream << ':' << (int)dayStart.min;
      mOutStream << ':' << (int)dayStart.sec << "\n\n";
    }
}

void
FileLogger::Log (const LOG_TYPE type, const char* str)
{
  LockRAII holder( mSync);

  const int markSize = PrintTimeMark( type);

  /* Print white spaces where the time mark should have been for
     for string messages that have more than one line, to keep
     a mice indentation. */
  while( *str != 0)
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
FileLogger::Log (const LOG_TYPE type, const string& str)
{
  Log (type, str.c_str());
}


uint_t
FileLogger::PrintTimeMark( LOG_TYPE type)
{
  static char logIds[] = { '!', 'C', 'E', 'W', 'I', 'D' };

  if (type > LOG_DEBUG)
    type = LOG_UNKNOW;

  const WTime ctime = wh_get_currtime();

  if ((ctime.day != mTodayTime.day)
      || (ctime.month != mTodayTime.month)
      || (ctime.year != mTodayTime.year))
    {
      mTodayTime = ctime;
      SwitchFile();
    }

  const char       fill  = mOutStream.fill();
  const streamsize width = mOutStream.width();

  mOutStream << '(' << logIds [type] << ')';
  mOutStream.fill( '0');
  mOutStream.width( 2);
  mOutStream << (uint_t) ctime.hour << ':';
  mOutStream.width( 2);
  mOutStream << (uint_t) ctime.min << ':';
  mOutStream.width( 2);
  mOutStream << (uint_t) ctime.sec << '.';
  mOutStream.width( 6);
  mOutStream << (uint_t) ctime.usec << ": ";

  mOutStream.fill( fill);
  mOutStream.width( width);

  return 1 + 2 + 1 + 2 + 1 + 2 + 1 + 6 + 2;
}


void
FileLogger::SwitchFile()
{
  if (mOutStream.is_open())
    mOutStream.close();

  snprintf( _CC (char*, mLogFile.c_str() + mLogFile.size() - 14),
            15,
            ".wlog.%04u%02u%02u",
            mTodayTime.year,
            mTodayTime.month,
            mTodayTime.day);

  mOutStream.open( mLogFile.c_str(), ios::app | ios::out);
}


void
NullLogger::Log (const LOG_TYPE, const char*)
{
}


void
NullLogger::Log (const LOG_TYPE, const string&)
{
}

} //namespace whais

