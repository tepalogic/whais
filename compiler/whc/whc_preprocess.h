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

#ifndef __WHC_PREPROCESS_H
#define __WHC_PREPROCESS_H

#include <vector>
#include <string>

#include "whais.h"
#include "utils/auto_array.h"
#include "msglog.h"



struct ReplacementTag
{
  ReplacementTag (const std::string& tagName,
                  const std::string& tagValue,
                  int                offset = BUILDIN_OFF)
    : mTagName (tagName),
      mTagValue (tagValue),
      mDefinitionOffset (offset)
  {
  }

  std::string mTagName;
  std::string mTagValue;
  int         mDefinitionOffset;

  static const int CMDLINE_OFF = -1;
  static const int BUILDIN_OFF = -2;
};


bool
preprocess_source (const std::string&                  sourceFile,
                   const std::vector<std::string>&     inclusionPaths,
                   std::vector<ReplacementTag>&        tagPairs,
                   std::ostringstream&                 sourceCode,
                   std::vector<SourceCodeMark>&        codeMarks,
                   std::vector<std::string>&           usedFiles);


#endif /* __WHC_PREPROCESS_H */
