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

//A global variable to used every one when you need one
NullLogger NULL_LOGGER;

I_Logger::~I_Logger ()
{

}

Logger::Logger (const D_CHAR* const pFile, const bool printStart)
  : I_Logger (),
    m_StartTick (wh_msec_ticks ()),
    m_Sync (),
    m_OutStream (pFile, ios::app | ios::out)
{
  if (! m_OutStream.good ())
    {
      throw ios_base::failure ("The file associated with the output stream "
                               "could not be opened");
    }

  if (printStart)
    {
      const WTime dayStart = wh_get_currtime ();

      m_OutStream << "\n* Start of the day: " << (int)dayStart.year;
      m_OutStream << '-' << (int)dayStart.month + 1;
      m_OutStream << "-" << (int)dayStart.day + 1;
      m_OutStream << ' ' << (int)dayStart.hour;
      m_OutStream << ':' << (int)dayStart.min;
      m_OutStream << ':' << (int)dayStart.sec << "\n\n";
    }
}

void
Logger::Log (const LOG_TYPE type, const D_CHAR* pStr)
{

  WSynchronizerRAII holder (m_Sync);

  const WTICKS ticks = wh_msec_ticks ();
  const D_INT markSize = PrintTimeMark (type, ticks);

  while (*pStr != 0)
    {
      if ((*pStr == '\n') && (*(pStr + 1) != '\n'))
        {
          m_OutStream << endl;
          for (D_INT i = 0; i < markSize; ++i)
            m_OutStream << ' ';
        }
      else
        m_OutStream << *pStr;
      ++pStr;
    }
  m_OutStream << endl;
  m_OutStream.flush ();
}

void
Logger::Log (const LOG_TYPE type, const string& str)
{
  Log (type, str.c_str ());
}

D_UINT
Logger::PrintTimeMark (LOG_TYPE type, WTICKS ticks)
{
  static D_CHAR logIds[] = { '!', 'C', 'E', 'W', 'I', 'D' };

  if (type > LOG_DEBUG)
    type = LOG_UNKNOW;

  ticks -= m_StartTick;

  const D_UINT days = ticks / (1000 * 3600 * 24);
  ticks %= (1000 * 3600 * 24);

  const D_UINT hours = ticks / (1000 * 3600);
  ticks %= (1000 * 3600);

  const D_UINT mins = ticks / (1000 * 60);
  ticks %= (1000 * 60);

  const D_UINT secs = ticks / 1000;
  ticks %= 1000;

  const char       fill  = m_OutStream.fill ();
  const streamsize width = m_OutStream.width ();

  m_OutStream << logIds [type];
  m_OutStream.width (4);
  m_OutStream.fill ('0');
  m_OutStream << days << 'd';
  m_OutStream.width (2);
  m_OutStream << hours << 'h';
  m_OutStream.width (2);
  m_OutStream << mins << 'm';
  m_OutStream.width (2);
  m_OutStream << secs << 's';
  m_OutStream.width (3);
  m_OutStream << ticks << ": ";
  m_OutStream.width (0);
  m_OutStream.fill (0);

  m_OutStream.fill (fill);
  m_OutStream.width (width);

  return 20; //The length of the mark!
}

void
NullLogger::Log (const LOG_TYPE, const D_CHAR*)
{
}

void
NullLogger::Log (const LOG_TYPE, const string&)
{
}


