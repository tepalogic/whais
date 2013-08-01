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

namespace whisper
{

const std::string
NextToken (const std::string&     text,
           size_t&                inoutOff,
           const string&          delims)
{
  inoutOff = text.find_first_not_of (delims, inoutOff);

  if (inoutOff == string::npos)
    {
      inoutOff  = text.length ();
      return string ();
    }

  assert (inoutOff != string::npos);
  assert (inoutOff < text.length ());

  size_t lastPos = text.find_first_of (delims, inoutOff);

  if (lastPos == string::npos)
    lastPos = text.length () - 1;

  else
    --lastPos;

  assert (inoutOff <= lastPos);

  string result = text.substr (inoutOff, lastPos - inoutOff + 1);

  inoutOff = lastPos + 1;

  return result;
}

} //namespace whisper

