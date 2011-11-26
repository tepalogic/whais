/******************************************************************************
WHISPER - An advanced database system
Copyright (C) 2008  Iulian Popa

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
#ifndef __cplusplus
#error "This header file should not be included from a C source file!"
#endif

#ifndef WEXCEPTION_H_
#define WEXCEPTION_H_

//Base class to handle all exceptions
class WException
{
public:
  WException (const D_CHAR *message,
      const D_CHAR *file,
      D_UINT32 line,
      D_UINT32 extra)
  : mErrorMessage (message),
    mFile (file),
    mLine (line),
    mExtra (extra)
  {
  }
  virtual ~WException () {};

  D_UINT32 GetExtra ()
  {
    return mExtra;
  };

  const D_CHAR* GetDescription ()
  {
    return mErrorMessage;
  }

  const D_CHAR* GetFile ()
  {
    return mFile;
  }

  D_UINT32 GetLine ()
  {
    return mLine;
  }

private:
  const D_CHAR* mErrorMessage;
  const D_CHAR* mFile;
  D_UINT32 mLine;
  D_UINT32 mExtra;
};

// Macro used to expand the CMD line
#define _EXTRA(x)  __FILE__, __LINE__, (x)

#endif /* WEXCEPTION_H_ */
