/******************************************************************************
UTILS - Common routines used trough WHISPER project
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
#include <assert.h>

#include "tokenizer.h"

using namespace std;

const std::string
NextToken (const std::string& input,
          size_t&             ioPosition,
          const string&       delimiters)
{
  ioPosition = input.find_first_not_of (delimiters, ioPosition);

  if (ioPosition == string::npos)
    return string ();

  assert (ioPosition != string::npos);
  assert (ioPosition < input.length ());

  size_t lastPos = input.find_first_of (delimiters, ioPosition);

  if (lastPos == string::npos)
    lastPos = input.length () - 1;
  else
    --lastPos;

  assert (ioPosition <= lastPos);

  string result = input.substr (ioPosition, lastPos - ioPosition + 1);

  ioPosition = lastPos + 1;

  return result;
}
