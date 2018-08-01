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

#include <assert.h>

#include "whais.h"

#include "tokenizer.h"


using namespace std;

namespace whais
{


const std::string
NextToken(const std::string& text, size_t& inoutOff, const string& delims)
{
  inoutOff = text.find_first_not_of(delims, inoutOff);

  if (inoutOff == string::npos)
  {
    inoutOff  = text.length();
    return string();
  }

  assert(inoutOff != string::npos);
  assert(inoutOff < text.length());

  size_t lastPos = inoutOff;
  if (text[lastPos] == '"')
  {
    bool ignoreEnd = false;

    while ((text[++lastPos] != '"') || ignoreEnd)
    {
      ignoreEnd = (text[lastPos] == '\\') ? !ignoreEnd : false;
      if (text[lastPos] == 0)
      {
        //Most likely an error, but not our jub to handle it!
        lastPos = string::npos;
        break;
      }
    }

    if (lastPos != string::npos)
    {
      assert(text[lastPos] == '"');
      ++lastPos;
    }
  }
  else if (text[lastPos] == '\'')
  {
    bool ignoreEnd = false;
    while ((text[++lastPos] != '\'') || ignoreEnd)
    {
      ignoreEnd = (text[lastPos] == '\\') ? !ignoreEnd : false;
      if (text[lastPos] == 0)
      {
        //Most likely an error, but not our jub to handle it!
        lastPos = string::npos;
        break;
      }
    }

    if (lastPos != string::npos)
    {
      assert(text[lastPos] == '\'');
      ++lastPos;
    }
  }
  else
    lastPos = text.find_first_of(delims, inoutOff);

  if (lastPos == string::npos)
    lastPos = text.length() - 1;

  else
    --lastPos;

  assert(inoutOff <= lastPos);

  const string result = text.substr(inoutOff, lastPos - inoutOff + 1);
  inoutOff = lastPos + 1;

  return result;
}


std::string&
NormalizeFilePath(std::string& path, const bool isDirectory)
{
  const char directoryDelimiter = whf_dir_delim();

  for (auto& c : path)
  {
    if ((c == '\\') || (c == '/'))
      c = directoryDelimiter;
  }

  if (isDirectory && (path[path.length() - 1] != directoryDelimiter))
    path.append(1, directoryDelimiter);

  return path;
}


} //namespace whais
