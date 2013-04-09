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

#ifndef __WHC_CMDLINE_H
#define __WHC_CMDLINE_H

#include "whisper.h"

namespace whisper
{

class WhcCmdLineParser
{
public:
  WhcCmdLineParser (int argc, char **argv);
  ~WhcCmdLineParser ();

  const char* GetSourceFile () const
  {
    return m_SourceFile;
  }
  const char* GetOutputFile () const
  {
    return m_OutputFile;
  }

private:
  void Parse ();
  void DisplayUsage () const;
  void CheckArguments ();

private:
  int         m_ArgCount;
  char**      m_Args;
  const char* m_SourceFile;
  const char* m_OutputFile;
  bool        m_ShowHelp;
  bool        m_OutputFileOwn;
};

class WhcCmdLineException : public Exception
{
public:
  WhcCmdLineException (const char* pMessage,
                       const char* pFile,
                       uint32_t      line,
                       uint32_t      extra)
    : Exception (pMessage, pFile, line, extra)
  {
  }

  virtual ~WhcCmdLineException ()
  {
  };

  virtual Exception*     Clone () const
  {
    return new WhcCmdLineException (*this);
  }
  virtual EXPCEPTION_TYPE Type () const { return COMPILER_CMD_LINE_EXCEPTION; }
  virtual const char*   Description () const
  {
    return "Invalid command line.";
  }
};

} //namespace whisper

// Declaration of error codes
#define ECMD_LINE_NO_ARGS        1
#define ECMD_LINE_INVAL_ARGS     2

#endif // __WHC_CMDLINE_H
