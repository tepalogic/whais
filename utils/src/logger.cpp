/******************************************************************************
WHISPERC - A compiler for whisper programs
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

namespace whisper
{

//A global variable to used every one when you need one
NullLogger NULL_LOGGER;



Logger::~Logger ()
{
}



FileLogger::FileLogger (const char* const file, const bool printStart)
  : Logger (),
    mStartTick (wh_msec_ticks ()),
    mSync (),
    mOutStream (file, ios::app | ios::out)
{
  if (! mOutStream.good ())
    {
      throw ios_base::failure ("The file associated with the output stream "
                               "could not be opened");
    }

  if (printStart)
    {
      const WTime dayStart = wh_get_currtime ();

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

  LockRAII holder (mSync);

  const WTICKS ticks    = wh_msec_ticks ();
  const int    markSize = PrintTimeMark (type, ticks);

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
  mOutStream.flush ();
}


void
FileLogger::Log (const LOG_TYPE type, const string& str)
{
  Log (type, str.c_str ());
}


uint_t
FileLogger::PrintTimeMark (LOG_TYPE type, WTICKS ticks)
{
  static char logIds[] = { '!', 'C', 'E', 'W', 'I', 'D' };

  if (type > LOG_DEBUG)
    type = LOG_UNKNOW;

  ticks -= mStartTick;

  const uint_t days = ticks / (1000 * 3600 * 24);
  ticks %= (1000 * 3600 * 24);

  const uint_t hours = ticks / (1000 * 3600);
  ticks %= (1000 * 3600);

  const uint_t mins = ticks / (1000 * 60);
  ticks %= (1000 * 60);

  const uint_t secs = ticks / 1000;
  ticks %= 1000;

  const char       fill  = mOutStream.fill ();
  const streamsize width = mOutStream.width ();

  mOutStream << logIds [type];
  mOutStream.width (4);
  mOutStream.fill ('0');
  mOutStream << days << 'd';
  mOutStream.width (2);
  mOutStream << hours << 'h';
  mOutStream.width (2);
  mOutStream << mins << 'm';
  mOutStream.width (2);
  mOutStream << secs << 's';
  mOutStream.width (3);
  mOutStream << ticks << ": ";
  mOutStream.width (0);
  mOutStream.fill (0);

  mOutStream.fill (fill);
  mOutStream.width (width);

  return 20; //The length of the time mark!
}



void
NullLogger::Log (const LOG_TYPE, const char*)
{
}


void
NullLogger::Log (const LOG_TYPE, const string&)
{
}

} //namespace whisper

