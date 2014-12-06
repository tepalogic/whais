/******************************************************************************
WHAISC - A compiler for whais programs
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
#include <cstdio>
#include <string>
#include <iostream>
#include <assert.h>

#include "compiler/whaisc.h"
#include "whc_cmdline.h"



using namespace std;



namespace whais {
namespace whc {



static inline bool
isStrEqual( const char* str1, const char* str2)
{
  return strcmp( str1, str2) == 0;
}




CmdLineParser::CmdLineParser( int argc, char** argv)
  : mArgCount( argc),
    mArgs( argv),
    mSourceFile( NULL),
    mOutputFile( NULL),
    mShowHelp( false),
    mOutputFileOwn( true),
    mPreprocessOnly( false),
    mBuildDependencies( false),
    mInclusionPaths(),
    mReplacementTags()
{
  Parse();
}


CmdLineParser::~CmdLineParser()
{
  if (mOutputFileOwn)
    delete [] mOutputFile;
}


void
CmdLineParser::Parse()
{
  int index = 1;

  if (index >= mArgCount)
    {
      throw CmdLineException( 
                  _EXTRA( 0),
                  "No arguments provided. Use '--help' for information."
                             );
    }

  while( index < mArgCount)
    {
      if (isStrEqual( mArgs[index], "-h")
          || isStrEqual( mArgs[index], "--help"))
        {
          mShowHelp = true;
          ++index;
        }
      else if (isStrEqual( mArgs[index], "-P"))
        {
          mPreprocessOnly = true;
          ++index;
        }
      else if (isStrEqual( mArgs[index], "-I"))
        {
          if ((++index >= mArgCount) || (mArgs[index][0] == '-'))
            {
              throw CmdLineException( _EXTRA( 0),
                                      "Missing value for parameter '-I'.");
            }
          AddInclusionPaths( mArgs[index++]);
        }
      else if (isStrEqual( mArgs[index], "-D"))
        {
          if ((++index >= mArgCount) || (mArgs[index][0] == '-'))
            {
              throw CmdLineException( _EXTRA( 0),
                                      "Missing values for parameter '-D'.");
            }
          else if (mArgs[index][0] == '=')
            {
              throw CmdLineException( _EXTRA( 0),
                                      "Missing tag value for parameter '-D'.");
            }

          const string param( mArgs[index]);
          const size_t separatorPos = param.find( '=');

          if (separatorPos == string::npos)
            {
              mReplacementTags.push_back( 
                  ReplacementTag( param.substr( 0, separatorPos),
                                  string())
                                         );
            }
          else
            {
              mReplacementTags.push_back( 
                  ReplacementTag( param.substr( 0, separatorPos),
                                  param.substr( separatorPos + 1))
                                         );
            }
          ++index;
        }
      else if (isStrEqual( mArgs[index], "--make_deps"))
        {
          mBuildDependencies = true;
          ++index;
        }
      else if (isStrEqual( mArgs[index], "-o"))
        {
          if (mOutputFile != NULL)
            {
              throw CmdLineException( 
                                  _EXTRA( 0),
                                  "Parameter '-o' is given multiple times."
                                      );
            }

          if ((++index >= mArgCount) || (mArgs[index][0] == '-'))
            {
              throw CmdLineException( _EXTRA( 0),
                                      "Missing value for parameter '-o'.");
            }

          else
            mOutputFile = mArgs[index++];

          mOutputFileOwn = false;
        }
      else if ((mArgs[index][0] != '-') && (mArgs[index][0] != '\\'))
        {
          if (mSourceFile != NULL)
            {
              throw CmdLineException( 
                              _EXTRA( 0),
                              "An input  file was already specified( '%s').",
                              mSourceFile
                                     );
            }

          mSourceFile = mArgs[index++];
        }
      else
        {
          throw CmdLineException( _EXTRA( 0),
                                  "Cannot handle argument '%s.'",
                                  mArgs[index]);
        }
    }

  CheckArguments();
}


static string&
normalize_path( string& path)
{
  const char* const dirDelim = whf_dir_delim();

  for (uint_t i = 0; i < path.size(); ++i)
    {
      if ((path[i] == '/') || (path[i] =='\\'))
        path[i] = dirDelim[0];
    }

  if (path[path.size() - 1] != dirDelim[0])
    path.append( dirDelim);

  return path;
}


void
CmdLineParser::AddInclusionPaths( const char* const paths)
{
  assert( paths != NULL);

  const char* currentPath = paths;
  const char* nextPath    = currentPath;

  while( nextPath && (*nextPath != 0))
    {
      nextPath = strpbrk( currentPath, ";");
      if (nextPath == NULL)
        {
          string path( currentPath);

          mInclusionPaths.push_back( normalize_path( path));
        }
      else
        {
          const uint_t pathSize = nextPath - currentPath;
          if (pathSize > 0)
            {
              string path( currentPath, pathSize);

              mInclusionPaths.push_back( normalize_path( path));
            }
          currentPath = ++nextPath;
        }
    }
}


void
CmdLineParser::CheckArguments()
{
  if (mShowHelp)
    {
      DisplayUsage();
      exit( 0);
    }
  else if (mSourceFile == NULL)
    throw CmdLineException( _EXTRA( 0), "The input file was not specified.");

  else if (mOutputFile == NULL)
  {
    const char    fileExt[]   = ".wo";
    const uint_t  fileNameLen = strlen( mSourceFile);
    char* const   tempBuffer  = new char[fileNameLen + sizeof fileExt];

    strcpy( tempBuffer, mSourceFile);

    if ((mSourceFile[fileNameLen - 1] == 'w') &&
        (mSourceFile[fileNameLen - 2] == '.'))
      {
        strcat( tempBuffer, "o");
      }
    else
      strcat( tempBuffer, fileExt);

    mOutputFile = tempBuffer;
  }

  const char* const defaultIncDirs = getenv( "WHAIS_INC");
  if (defaultIncDirs != NULL)
    AddInclusionPaths( defaultIncDirs);

  char  temp[64];
  const WTime t  = wh_get_currtime();

  snprintf( temp, sizeof temp, "%d", t.year);
  mReplacementTags.push_back( ReplacementTag( "YEAR", temp));

  snprintf( temp, sizeof temp, "%02u", t.month);
  mReplacementTags.push_back( ReplacementTag( "MONTH", temp));

  snprintf( temp, sizeof temp, "%02u", t.day);
  mReplacementTags.push_back( ReplacementTag( "DAY", temp));

  snprintf( temp, sizeof temp, "%02u", t.hour);
  mReplacementTags.push_back( ReplacementTag( "HOUR", temp));

  snprintf( temp, sizeof temp, "%02u", t.min);
  mReplacementTags.push_back( ReplacementTag( "MIN", temp));

  snprintf( temp, sizeof temp, "%02u", t.sec);
  mReplacementTags.push_back( ReplacementTag( "SEC", temp));

  snprintf( temp, sizeof temp, "%06u", t.usec);
  mReplacementTags.push_back( ReplacementTag( "USEC", temp));

  snprintf( temp,
            sizeof temp,
            "%d/%02u/%02u %02u:%02u:%02u.%06u",
            t.year,
            t.month,
            t.day,
            t.hour,
            t.min,
            t.sec,
            t.usec);

  mReplacementTags.push_back( ReplacementTag( "TIME_STAMP", temp));

  for (size_t i = 0; i < mReplacementTags.size(); ++i)
    {
      mReplacementTags[i].mTagName.insert( 0, "%");
      mReplacementTags[i].mTagName.push_back( '%');
    }
}


void
CmdLineParser::DisplayUsage() const
{
  using namespace std;

  unsigned int ver_maj, ver_min;
  wh_compiler_libver( &ver_maj, &ver_min);

  cout << "Whais Compiler v" << ver_maj << '.';

  cout.width( 2); cout.fill( '0');
  cout << ver_min;
  cout.width( 0);

  cout <<
    " by Iulian POPA( popaiulian@gmail.com)\n"
    "Usage: whc [options] input_file\n"
    "Options:\n"
    "-D 'tag=text'   Define a replace tag (e.g -D 'user_name=The Coder).\n"
    "-h, --help      Display this help.\n"
    "-I 'dir1;di2'   Add a list of directories paths to the list of\n"
    "                directories used to search for included files.\n"
    "                (e.g -I '/usr/local/whais/inc;C:\\whais\\inc').\n"
    "--make_deps     Generate the dependencies list of this file( in a 'make'\n"
    "                recognized way) on the standard output.\n"
    "-o file         Use 'file' as the compilation output file.\n"
    "-P              Preprocess only. Display the result on standard output.\n";
}


CmdLineException::CmdLineException( const uint32_t      code,
                                    const char*         file,
                                    const uint32_t      line,
                                    const char*         fmtMsg,
                                    ... )
  : Exception( code, file, line)
{
  if (fmtMsg != NULL)
    {
      va_list vl;

      va_start( vl, fmtMsg);
      this->Message( fmtMsg, vl);
      va_end( vl);
    }
}


Exception*
CmdLineException::Clone() const
{
  return new CmdLineException( *this);
}


EXCEPTION_TYPE
CmdLineException::Type() const
{
  return COMPILER_CMD_LINE_EXCEPTION;
}


const char*
CmdLineException::Description() const
{
  return "Invalid command line.";
}

} //namespace whc
} //namespace whais

