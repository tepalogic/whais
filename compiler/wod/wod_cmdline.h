/******************************************************************************
WOD - Whais Object Dumper.
Copyright(C) 2009  Iulian Popa

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

#include "whais.h"

namespace whais {
namespace wod {


static const char sProgramName[] = "Whais Object Dumper";


class CmdLineParser
{
public:
  CmdLineParser(int argc, char** argv);
  ~CmdLineParser();

  auto SourceFile() const { return mSourceFile; }
  auto& OutStream() const { return *mOutStream; }

private:
  void Parse();
  void DisplayUsage() const;
  void CheckArguments();

private:
  int            mArgCount;
  char**         mArgs;
  const char    *mSourceFile;
  std::ostream  *mOutStream;
  bool           mShowHelp;
  bool           mShowLogo;
  bool           mShowLicense;
};


class CmdLineException : public Exception
{
public:
  CmdLineException(const uint32_t   code,
                   const char      *file,
                   uint32_t         line,
                   const char      *fmtMsg = nullptr,
                   ...);
  virtual Exception* Clone() const;
  virtual EXCEPTION_TYPE Type() const;
  virtual const char* Description() const;
};


} //namespace wod
} //namespace whais


#endif /* WOD_CMDLINE_H_ */
