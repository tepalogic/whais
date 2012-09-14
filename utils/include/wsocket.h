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

#ifndef WSOCKET_H_
#define WSOCKET_H_

#include "whisper.h"

class EXCEP_SHL WSocketException : public WException
{
public:
  WSocketException (const D_CHAR* message,
                    const D_CHAR* file,
                    D_UINT32      line,
                    D_UINT32      extra);
  virtual WException*     Clone () const;
  virtual EXPCEPTION_TYPE Type () const;
  virtual const D_CHAR*   Description () const;
};

class EXCEP_SHL WSocket
{
public:
  WSocket (const D_CHAR* const pServerName,
           const D_CHAR* const pService);
  WSocket (const D_CHAR* const pServerName,
           const D_UINT16      port);
  WSocket (const D_CHAR* const pLocalAddress,
           const D_CHAR* const pService,
           const D_UINT        backLog);
  WSocket (const D_CHAR* const pLocalAddress,
           const D_UINT16      port,
           const D_UINT        backLog);
  WSocket (const WH_SOCKET sd);
  WSocket (const WSocket& source);
  ~WSocket ();

  WSocket& operator= (const WSocket& source);

  WSocket Accept ();
  D_UINT  Read (const D_UINT count, D_UINT8* const pBuffer);
  void    Write (const D_UINT count, const D_UINT8* const pBuffer);
  void    Close ();

private:

  WH_SOCKET m_Socket;
  bool      m_Owned;

  struct SocketInitialiser
  {
    SocketInitialiser ()
    {
      if ( ! wh_init_socks ())
        {
          throw WSocketException ("Network system could not be initialized.",
                                  _EXTRA (WH_SOCK_NOTINIT));
        }
    }

    ~SocketInitialiser ()
    {
      wh_clean_socks ();
    }
  };

  static SocketInitialiser __initer;
};



#endif /* WSOCKET_H_ */
