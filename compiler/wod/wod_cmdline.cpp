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

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "wod_cmdline.h"
#include "utils/license.h"

using namespace std;



namespace whais {
namespace wod {


static inline bool
isStrEqual(const char* str1, const char* str2)
{
  return::strcmp(str1, str2) == 0;
}

CmdLineParser::CmdLineParser(int argc, char ** argv)
  : mArgCount(argc),
    mArgs(argv),
    mSourceFile(NULL),
    mOutStream(&cout),
    mShowHelp(false),
    mShowLogo(false),
    mShowLicense(false)
{
  Parse();
}


CmdLineParser::~CmdLineParser()
{
  if (mOutStream != &cout)
    delete mOutStream;
}


void
CmdLineParser::Parse()
{
  int index = 1;
  if (index >= mArgCount)
    {
      throw CmdLineException(
                  _EXTRA(0),
                  "No arguments provided. Use '--help' for information."
                             );
    }

  while(index < mArgCount)
    {
      if (isStrEqual(mArgs[index], "-h") ||
          isStrEqual(mArgs[index], "--help"))
        {
          mShowHelp = true;
          ++index;
        }
      else if (isStrEqual(mArgs[index], "-v")
               || isStrEqual(mArgs[index], "--version"))
        {
          mShowLogo = true;
          ++index;
        }
      else if (isStrEqual(mArgs[index], "-l")
               || isStrEqual(mArgs[index], "--license"))
        {
          mShowLicense = true;
          ++index;
        }
      else if (isStrEqual(mArgs[index], "-o"))
        {
          if (mOutStream != &cout)
            {
              throw CmdLineException(
                  _EXTRA(0),
                  "The output file '-o' is specified multiple times."
                                     );
            }

          if ((++index >= mArgCount) || (mArgs[index][0] == '-'))
            {
              throw CmdLineException(_EXTRA(0),
                                      "Missing parameter for argument '-o'.");
            }
          else
            mOutStream = new ofstream(mArgs[index++]);
        }
      else if ((mArgs[index][0] != '-') && (mArgs[index][0] != '\\'))
        {
          if ((void *) mSourceFile != NULL)
            {
              throw CmdLineException(_EXTRA(0),
                                      "The input file was already specified.");
            }
          mSourceFile = mArgs[index++];
        }
      else
        {
          throw CmdLineException(_EXTRA(0),
                                  "Cannot handle argument '%s' Try '--help'!",
                                  mArgs[index]);
        }
    }
  CheckArguments();
}


void
CmdLineParser::CheckArguments()
{
  if (mShowHelp)
    {
      DisplayUsage();
      exit(0);
    }
  else if (mShowLogo)
    {
      displayBanner(cout, sProgramName, WVER_MAJ, WVER_MIN);
      exit(0);
    }
  else if (mShowLicense)
    {
      displayLicenseInformation(cout, sProgramName, NULL);
      exit(0);
    }
  else if (mSourceFile == NULL)
    throw CmdLineException(_EXTRA(0), "The input file was not specified.");
}


void
CmdLineParser::DisplayUsage() const
{
  using namespace std;

  displayBanner(cout, sProgramName,  WVER_MAJ, WVER_MIN);
  cout <<
    "Usage: wod [options] input_file\n"
    "Options:\n"
    "-h, --help      Display this help.\n"
    "-o file         Use 'file' as the output file.\n"
    "-v, --version   Display the program's version.\n"
    "-l, --license   Display the license information.\n";
}


CmdLineException::CmdLineException(const uint32_t  code,
                                    const char*     file,
                                    uint32_t        line,
                                    const char*     fmtMsg,
                                    ...)
  : Exception(code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start(vl, fmtMsg);
      this->Message(fmtMsg, vl);
      va_end(vl);
    }
}


Exception*
CmdLineException::Clone() const
{
  return new CmdLineException(*this);
}



EXCEPTION_TYPE
CmdLineException::Type() const
{
  return DUMP_CMD_LINE_EXCEPTION;
}


const char*
CmdLineException::Description() const
{
  return "Invalid command line.";
}



} //namespace wod
} //namespace whais

