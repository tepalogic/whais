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

static inline bool
isStrEqual (const D_CHAR* str1, const D_CHAR* str2)
{
  return::strcmp (str1, str2) == 0;
}

WhcCmdLineParser::WhcCmdLineParser (int argc, char **argv) :
  m_ArgCount (argc),
  m_Args (argv),
  m_SourceFile (NULL),
  m_OutputFile (NULL),
  m_ShowHelp (false),
  m_OutputFileOwn (true)
{
  Parse ();
}

WhcCmdLineParser::~WhcCmdLineParser ()
{
  if (m_OutputFileOwn)
    delete [] m_OutputFile;
}

void
WhcCmdLineParser::Parse ()
{
  using namespace std;

  int index = 1;
  if (index >= m_ArgCount)
    throw WhcCmdLineException ("No arguments! Use --help first!", _EXTRA (0));

  while (index < m_ArgCount)
    {
      if (isStrEqual (m_Args[index], "-h") ||
          isStrEqual (m_Args[index], "--help"))
        {
          m_ShowHelp = true;
          ++index;
        }
      else if (isStrEqual (m_Args[index], "-o"))
        {
          if ((void *) m_OutputFile != NULL)
            throw WhcCmdLineException ("Parameter '-o' is given twice", _EXTRA (0));

          if ((++index >= m_ArgCount) || (m_Args[index][0] == '-'))
            throw WhcCmdLineException ("Missing file name argument for for parameter '-o'.",
                                       _EXTRA (0));
          else
            m_OutputFile = m_Args[index++];

          m_OutputFileOwn = false;
        }
      else if ((m_Args[index][0] != '-') && (m_Args[index][0] != '\\'))
        {
          if ((void *) m_SourceFile != NULL)
            throw WhcCmdLineException ("The object file was already specified!", _EXTRA (0));

          m_SourceFile = m_Args[index++];
        }
      else
        throw WhcCmdLineException ("Unknown arguments! Use --help first!", _EXTRA (0));
    }

  CheckArguments ();
}

void
WhcCmdLineParser::CheckArguments ()
{
  if (m_ShowHelp)
    {
      DisplayUsage ();
      exit (0);
    }
  else if (m_SourceFile == NULL)
    throw WhcCmdLineException ("No given input file!", _EXTRA(0));
  else if (m_OutputFile == NULL)
  {
    const D_CHAR  fileExt[]   = ".wo";
    const D_UINT  fileNameLen = strlen (m_SourceFile);
    D_CHAR* const tempBuffer  = new D_CHAR[fileNameLen + sizeof fileExt];

    m_OutputFile = tempBuffer;
    strcpy (tempBuffer, m_SourceFile);
    if ((m_SourceFile[fileNameLen - 1] == 'w') &&
        (m_SourceFile[fileNameLen - 2] == '.'))
      {
        strcat (tempBuffer, "o");
      }
    else
      strcat (tempBuffer, fileExt);
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
       << "Usage: whisperc  input_file [-o output_file] [--help | -h]" << endl;
}


