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
  WodCmdLineParser (D_INT argc, D_CHAR ** argv);
  ~WodCmdLineParser ();

  const D_CHAR *GetSourceFile () const
  {
    return mSourceFile;
  }
  std::ostream & GetOutStream () const
  {
    return *mOutStream;
  }

private:
  void Parse ();
  void DisplayUsage () const;
  void CheckArguments ();

private:
    D_INT mArgCount;
  D_CHAR **mArgs;
  const D_CHAR *mSourceFile;
    std::ostream * mOutStream;
  bool mDisplayHelp;

};

class WodCmdLineException:public WException
{
public:
  WodCmdLineException (const D_CHAR *message,
      const D_CHAR *file, D_UINT32 line, D_UINT32 extra)
  : WException (message, file, line, extra) {}
  virtual ~ WodCmdLineException () {};
};

#endif /* WOD_CMDLINE_H_ */
