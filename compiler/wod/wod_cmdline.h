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
  WodCmdLineParser (D_INT argc, D_CHAR** argv);
  ~WodCmdLineParser ();

  const D_CHAR* GetSourceFile () const
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
  D_INT         m_ArgCount;
  D_CHAR**      m_Args;
  const D_CHAR* m_SourceFile;
  std::ostream* m_OutStream;
  bool          m_ShowHelp;

};

class WodCmdLineException : public WException
{
public:
  WodCmdLineException (const D_CHAR* pMessage,
                       const D_CHAR* pFile,
                       D_UINT32      line,
                       D_UINT32      extra)
  : WException (pMessage, pFile, line, extra)
  {
  }

  virtual ~WodCmdLineException ()
  {
  };

  virtual WException*     Clone () { return new WodCmdLineException (*this); }
  virtual EXPCEPTION_TYPE GetType () { return DUMP_CMD_LINE_EXCEPTION; }
};

#endif /* WOD_CMDLINE_H_ */
