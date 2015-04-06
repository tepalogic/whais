/******************************************************************************
WHAIS - An advanced database system
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

#include "whais.h"

namespace whais
{

class EXCEP_SHL FileException : public Exception
{
public:
  FileException (const uint32_t    code,
                 const char*       file,
                 uint32_t          line,
                 const char*       fmtMsg,
                 ...);

  virtual Exception* Clone () const;

  virtual EXCEPTION_TYPE Type () const;

  virtual const char* Description () const;
};



class EXCEP_SHL File
{
public:
  explicit File (const char* name, uint_t mode = 0);

  File (const File& src);
  File& operator= (const File&);

  ~File ();

  void     Read (uint8_t* buffer, uint_t size);

  void     Write (const uint8_t* buffer, uint_t size);

  void     Seek (const int64_t where, const int whence);

  uint64_t Tell ();

  void     Sync ();

  uint64_t Size () const;

  void     Size (const uint64_t size);

  void     Close ();


private:
  static const uint64_t UNKNOWN_SIZE = 0xFFFFFFFFFFFFFFFFull;
  WH_FILE     mHandle;
  uint64_t    mFileSize;
};

} //namespace whais

#endif /* WFILE_H_ */
