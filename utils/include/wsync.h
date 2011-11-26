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

#ifndef WSYNC_H_
#define WSYNC_H_

class WSynchronizer
{
public:
  WSynchronizer()
  {
    wh_init_sync(&mSync);
  }

  ~WSynchronizer()
  {
    wh_destroy_sync(&mSync);
  }

  void
  Enter()
  {
    wh_enter_sync(&mSync);
  }

  void
  Leave()
  {
    wh_leave_sync(&mSync);
  }

private:

  //Does not support any kind of copy or assignment!
  WSynchronizer(const WSynchronizer&);
  WSynchronizer& operator=(const WSynchronizer&);

  WH_SYNC mSync;
};

class WSynchronizerHolder
  {
public:
  explicit WSynchronizerHolder(WSynchronizer &rSync) :
    mSync(rSync),
    m_IsEntered (true)
  {
    mSync.Enter();
  }

  void Enter ()
  {
    m_IsEntered = true;
    mSync.Enter ();
  }

  void Leave ()
  {
    m_IsEntered = false;
    mSync.Leave();
  }

  ~WSynchronizerHolder()
  {
    if (m_IsEntered)
      mSync.Leave();
  }

private:
  WSynchronizer &mSync;
  bool           m_IsEntered;
};

#endif /* WSYNC_H_ */
