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

#ifndef WFILE_H_
#define WFILE_H_

#include "whisper.h"

class WFileException : public WException
{
public:
  explicit WFileException (const D_CHAR *message,
      const D_CHAR *file, D_UINT32 line, D_UINT32 extra)
  : WException (message, file, line, extra) {}
  virtual ~ WFileException () {};

  virtual WException*     Clone () { return new WFileException (*this); }
  virtual EXPCEPTION_TYPE GetType () { return FILE_EXCEPTION; }
};

class WFile
{
public:
  explicit WFile (const D_CHAR * fname, D_UINT mode = 0);
  WFile (const WFile &rSource);
   ~WFile ();

  void Read (D_UINT8 * buffer, D_UINT size);
  void Write (const D_UINT8 * buffer, D_UINT size);
  void Seek (D_INT64 where, D_INT whence);
  D_UINT64 Tell ();
  void Sync ();
  D_UINT64 GetSize () const;
  void SetSize (D_UINT64 size);
  void Close ();

  WFile & operator= (const WFile &);

private:
  WH_FILE_HND mHandle;
};

#endif /* WFILE_H_ */
