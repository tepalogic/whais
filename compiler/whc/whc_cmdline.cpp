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

#include "./compiler/include/whisperc/whisperc.h"

#include "whc_cmdline.h"

static inline bool
isStrEqual (const D_CHAR * str1, const D_CHAR * str2)
{
  return::strcmp (str1, str2) == 0;
}

WhcCmdLineParser::WhcCmdLineParser (int argc, char **argv):
mArgCount (argc),
mArgs (argv),
mSourceFile (NULL),
mOutputFile (NULL), mDisplayHelp (false), mAllocatedOutputFileName (true)
{
  Parse ();
}

WhcCmdLineParser::~WhcCmdLineParser ()
{
  if (mAllocatedOutputFileName)
    delete[]mOutputFile;
}

void
WhcCmdLineParser::Parse ()
{
  using namespace std;
  int index = 1;
  if (index >= mArgCount)
    throw WhcCmdLineException ("No arguments! Use --help first!", _EXTRA(0));

  while (index < mArgCount)
    {
      if (isStrEqual (mArgs[index], "-h")
	  || isStrEqual (mArgs[index], "--help"))
	{
	  mDisplayHelp = true;
	  ++index;
	}
      else if (isStrEqual (mArgs[index], "-o"))
	{
	  if ((void *) mOutputFile != NULL)
	    throw WhcCmdLineException ("Parameter '-o' is given twice",
	        _EXTRA(0));

	  if ((++index >= mArgCount) || (mArgs[index][0] == '-'))
	    throw WhcCmdLineException (
	        "Missing file name argument for for parameter '-o'.",
	        _EXTRA(0));
	  else
	  mOutputFile = mArgs[index++];

	  mAllocatedOutputFileName = false;
	}
      else if ((mArgs[index][0] != '-') && (mArgs[index][0] != '\\'))
	{
	  if ((void *) mSourceFile != NULL)
	    throw
	      WhcCmdLineException ("The object file was already specified!",
	          _EXTRA(0));

	  mSourceFile = mArgs[index++];
	}
      else
	throw WhcCmdLineException ("Unknown arguments! Use --help first!",
	    _EXTRA(0));
    }

  CheckArguments ();
}

void
WhcCmdLineParser::CheckArguments ()
{
  if (mDisplayHelp)
    {
      DisplayUsage ();
      exit (0);
    }
  else if (mSourceFile == NULL)
    throw WhcCmdLineException ("No given input file!", _EXTRA(0));
  else
if (mOutputFile == NULL)
  {
    const D_CHAR file_ext[] = ".wo";
    const D_UINT file_name_len = strlen (mSourceFile);
    D_CHAR *const temp = new D_CHAR[file_name_len + sizeof file_ext];

    mOutputFile = temp;
    strcpy (temp, mSourceFile);
    if ((mSourceFile[file_name_len - 1] == 'c')
	&& (mSourceFile[file_name_len - 2] == 'w')
	&& (mSourceFile[file_name_len - 3]) == '.')
      temp[file_name_len - 1] = 'o';
    else
      strcat (temp, file_ext);
  }
}

void
WhcCmdLineParser::DisplayUsage () const
{
  using namespace std;
  unsigned int ver_maj, ver_min;
  whc_get_libver (&ver_maj, &ver_min);
  cout << "Whisper Compiler ver. " << ver_maj << '.' << ver_min;
  cout << " by Iulian POPA (popaiulian@gmail.com)" << endl
    << "Usage: "
    << "whisperc  input_file [-o output_file] [--help | -h]" << endl;
}


