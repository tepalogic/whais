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
  WSocketException (const char* message,
                    const char* file,
                    uint32_t      line,
                    uint32_t      extra);
  virtual WException*     Clone () const;
  virtual EXPCEPTION_TYPE Type () const;
  virtual const char*   Description () const;
};

class EXCEP_SHL WSocket
{
public:
  WSocket (const char* const pServerName,
           const char* const pService);
  WSocket (const char* const pServerName,
           const uint16_t      port);
  WSocket (const char* const pLocalAddress,
           const char* const pService,
           const uint_t        backLog);
  WSocket (const char* const pLocalAddress,
           const uint16_t      port,
           const uint_t        backLog);
  WSocket (const WH_SOCKET sd);
  WSocket (const WSocket& source);
  ~WSocket ();

  WSocket& operator= (const WSocket& source);

  WSocket Accept ();
  uint_t  Read (const uint_t count, uint8_t* const pBuffer);
  void    Write (const uint_t count, const uint8_t* const pBuffer);
  void    Close ();

private:

  WH_SOCKET m_Socket;
  bool      m_Owned;

  struct EXCEP_SHL SocketInitialiser
  {
    SocketInitialiser ()
    {
      if ( ! wh_init_socks ())
        {
          throw WSocketException ("Network system could not be initialized.",
                                  _EXTRA (0));
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
