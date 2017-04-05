/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#include "whais.h"


namespace whais {


class EXCEP_SHL SocketException : public Exception
{
public:
  SocketException(const uint32_t code,
                  const char*    file,
                  uint32_t       line,
                  const char*    fmMsg = nullptr,
                  ...);

  virtual Exception* Clone() const override;
  virtual EXCEPTION_TYPE Type() const override;
  virtual const char* Description() const override;
};


class EXCEP_SHL Socket
{
public:
  //Client server constructors
  Socket(const char* const serverHost, const char* const service);
  Socket(const char* const serverHost, const uint16_t port);

  //Server sockets constructors
  Socket(const char* const address, const char* const service, const uint_t backLog);
  Socket(const char* const address, const uint16_t port, const uint_t backLog);

  //Utility constructors
  Socket(const WH_SOCKET sd);
  Socket(const Socket& src);

  Socket& operator= (const Socket& src);

  ~Socket();


  Socket  Accept();
  uint_t  Read(uint8_t* const buffer, const uint_t maxCount);
  void    Write(const uint8_t* const buffer, const uint_t count);
  void    Close();

private:
  WH_SOCKET   mSocket;
  bool        mOwned;

  struct EXCEP_SHL SocketInitialiser
  {
    SocketInitialiser()
    {
      if ( !whs_init())
        throw SocketException(_EXTRA(0), "The network subsystem could not be initialized.");
    }

    ~SocketInitialiser()
    {
      whs_clean();
    }
  };

  // Use the following static member to initialise the socket framework.
  // Note: in a program that uses the Socket wrapper class, one must not
  // call 'whs_init()' or 'whs_clean()', as this is handled automatically.
  static SocketInitialiser __initer;
};


} //namespace whais


#endif /* WSOCKET_H_ */
