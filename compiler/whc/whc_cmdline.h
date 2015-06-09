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

#ifndef __WHC_CMDLINE_H
#define __WHC_CMDLINE_H


#include <vector>
#include <string>

#include "whais.h"


#include "whc_preprocess.h"



namespace whais {
namespace whc {



class CmdLineParser
{
public:
  CmdLineParser (int argc, char **argv);
  ~CmdLineParser ();

  const char* SourceFile () const
  {
    return mSourceFile;
  }

  const char* OutputFile () const
  {
    return mOutputFile;
  }


  const bool JustPreprocess () const
  {
    return mPreprocessOnly;
  }

  const bool BuildDependencies () const
  {
    return mBuildDependencies;
  }

  const std::vector<std::string>& InclusionPaths () const
  {
    return mInclusionPaths;
  }

  const std::vector<ReplacementTag>& ReplacementTags () const
  {
    return mReplacementTags;
  }

private:
  void Parse ();

  void DisplayUsage () const;

  void CheckArguments ();

  void AddInclusionPaths (const char* const paths);

private:
  int         mArgCount;
  char**      mArgs;
  const char* mSourceFile;
  const char* mOutputFile;
  bool        mShowHelp;
  bool        mOutputFileOwn;
  bool        mPreprocessOnly;
  bool        mBuildDependencies;
  bool        mShowLogo;
  bool        mShowLicense;

  std::vector<std::string>            mInclusionPaths;
  std::vector<ReplacementTag>         mReplacementTags;
};


class CmdLineException : public Exception
{
public:
  CmdLineException (const uint32_t      code,
                    const char*         file,
                    const uint32_t      line,
                    const char*         fmtMsg = NULL,
                    ... );

  virtual Exception* Clone () const;

  virtual EXCEPTION_TYPE Type () const;

  virtual const char* Description () const;
};

} //namespace whc
} //namespace whais

// Declaration of error codes
#define ECMD_LINE_NO_ARGS        1
#define ECMD_LINE_INVAL_ARGS     2

#endif // __WHC_CMDLINE_H

