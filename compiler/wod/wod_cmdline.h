/******************************************************************************
WOD - Whisper Object Dumper.
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

#ifndef WOD_CMDLINE_H_
#define WOD_CMDLINE_H_

#include <iostream>

#include "whisper.h"



class WodCmdLineParser
{
public:
  WodCmdLineParser (int argc, char** argv);
  ~WodCmdLineParser ();

  const char* GetSourceFile () const
  {
    return m_SourceFile;
  }
  std::ostream& GetOutStream () const
  {
    return *m_OutStream;
  }

private:
  void Parse ();
  void DisplayUsage () const;
  void CheckArguments ();

private:
  int         m_ArgCount;
  char**      m_Args;
  const char* m_SourceFile;
  std::ostream* m_OutStream;
  bool          m_ShowHelp;

};

class WodCmdLineException : public WException
{
public:
  WodCmdLineException (const char* pMessage,
                       const char* pFile,
                       uint32_t      line,
                       uint32_t      extra)
    : WException (pMessage, pFile, line, extra)
  {
  }

  virtual ~WodCmdLineException ()
  {
  };

  virtual WException*     Clone () const
  {
    return new WodCmdLineException (*this);
  }

  virtual EXPCEPTION_TYPE Type () const { return DUMP_CMD_LINE_EXCEPTION; }
  virtual const char*   Description () const
  {
    return "Invalid command line.";
  }
};

#endif /* WOD_CMDLINE_H_ */
