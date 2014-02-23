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

#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>
#include <assert.h>

#include "compiler/whisperc.h"

#include "whc_cmdline.h"

using namespace std;

namespace whisper {
namespace whc {

static inline bool
isStrEqual (const char* str1, const char* str2)
{
  return strcmp (str1, str2) == 0;
}

CmdLineParser::CmdLineParser (int argc, char** argv)
  : mArgCount (argc),
    mArgs (argv),
    mSourceFile (NULL),
    mOutputFile (NULL),
    mShowHelp (false),
    mOutputFileOwn (true)
{
  Parse ();
}


CmdLineParser::~CmdLineParser ()
{
  if (mOutputFileOwn)
    delete [] mOutputFile;
}


void
CmdLineParser::Parse ()
{
  int index = 1;

  if (index >= mArgCount)
    {
      throw CmdLineException (
                  _EXTRA (0),
                  "No arguments provided. Use '--help' for information."
                             );
    }

  while (index < mArgCount)
    {
      if (isStrEqual (mArgs[index], "-h")
          || isStrEqual (mArgs[index], "--help"))
        {
          mShowHelp = true;
          ++index;
        }
      else if (isStrEqual (mArgs[index], "-o"))
        {
          if (mOutputFile != NULL)
            {
              throw CmdLineException (
                                  _EXTRA (0),
                                  "Parameter '-o' is given multiple times."
                                      );
            }

          if ((++index >= mArgCount) || (mArgs[index][0] == '-'))
            {
              throw CmdLineException (_EXTRA (0),
                                      "Missing parameter for argument '-o'.");
            }

          else
            mOutputFile = mArgs[index++];

          mOutputFileOwn = false;
        }
      else if ((mArgs[index][0] != '-') && (mArgs[index][0] != '\\'))
        {
          if (mSourceFile != NULL)
            {
              throw CmdLineException (
                              _EXTRA (0),
                              "An input  file was already specified ('%s').",
                              mSourceFile
                                     );
            }

          mSourceFile = mArgs[index++];
        }
      else
        {
          throw CmdLineException (_EXTRA (0),
                                  "Cannot handle argument '%s.'",
                                  mArgs[index]);
        }
    }

  CheckArguments ();
}

void
CmdLineParser::CheckArguments ()
{
  if (mShowHelp)
    {
      DisplayUsage ();
      exit (0);
    }
  else if (mSourceFile == NULL)
    throw CmdLineException (_EXTRA(0), "The input file was not specified.");

  else if (mOutputFile == NULL)
  {
    const char    fileExt[]   = ".wo";
    const uint_t  fileNameLen = strlen (mSourceFile);
    char* const   tempBuffer  = new char[fileNameLen + sizeof fileExt];

    strcpy (tempBuffer, mSourceFile);

    if ((mSourceFile[fileNameLen - 1] == 'w') &&
        (mSourceFile[fileNameLen - 2] == '.'))
      {
        strcat (tempBuffer, "o");
      }
    else
      strcat (tempBuffer, fileExt);

    mOutputFile = tempBuffer;
  }
}

void
CmdLineParser::DisplayUsage () const
{
  using namespace std;

  unsigned int ver_maj, ver_min;
  wh_compiler_libver (&ver_maj, &ver_min);

  cout << "Whisper Compiler ver. " << ver_maj << '.' << ver_min;
  cout << " by Iulian POPA (popaiulian@gmail.com)" << endl
       << "Usage: whisperc  input_file [-o output_file] [--help | -h]" << endl;
}



CmdLineException::CmdLineException (const uint32_t      code,
                                    const char*         file,
                                    const uint32_t      line,
                                    const char*         fmtMsg,
                                    ... )
  : Exception (code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start (vl, fmtMsg);
      this->Message (fmtMsg, vl);
      va_end (vl);
    }
}


Exception*
CmdLineException::Clone () const
{
  return new CmdLineException (*this);
}


EXCEPTION_TYPE
CmdLineException::Type () const
{
  return COMPILER_CMD_LINE_EXCEPTION;
}


const char*
CmdLineException::Description () const
{
  return "Invalid command line.";
}

} //namespace whc
} //namespace whisper

