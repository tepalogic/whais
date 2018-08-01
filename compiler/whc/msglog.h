/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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

#ifndef MSGLOG_H_
#define MSGLOG_H_


#include <cstdarg>
#include <vector>
#include <string>

#include "whais.h"
#include "compiler/whaisc.h"


struct SourceCodeMark
{
  SourceCodeMark(uint32_t      bufferOffset,
                 uint32_t      bufferLine,
                 std::string   bufferSource,
                 uint_t        inclusionLevel)
    : mBufferOffset(bufferOffset),
      mBufferLine(bufferLine),
      mLevel(inclusionLevel),
      mBufferSource(bufferSource)
  {
  }

  uint32_t      mBufferOffset;
  uint32_t      mBufferLine;
  uint_t        mLevel;
  std::string   mBufferSource;
};

struct WHC_MESSAGE_CTX
{
  WHC_MESSAGE_CTX(const std::vector<SourceCodeMark>&  codeMarks,
                  const char* const                   sourceCode)
    : mCodeMarks(codeMarks),
      mCode(sourceCode)
  {
  }

  const std::vector<SourceCodeMark>&   mCodeMarks;
  const char* const                    mCode;
};



void
whc_messenger(WH_MESSENGER_CTXT data,
              uint_t            buffOff,
              uint_t            msgId,
              uint_t            msgType,
              const char*       msgFormat,
              va_list           args);

void
whc_messenger(WH_MESSENGER_CTXT   data,
              uint_t              buffOff,
              uint_t              msgId,
              uint_t              msgType,
              const char         *msgFormat,
              ...);

#endif /* MSGLOG_H_ */
