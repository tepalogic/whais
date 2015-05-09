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

#include <termios.h>
#include <unistd.h>

#include "whais.h"

bool_t
wh_disable_echo ()
{
  struct termios t;

  if (tcgetattr( STDIN_FILENO, &t) < 0)
    return FALSE;

  t.c_lflag &= ~ECHO;
  if (tcsetattr( STDIN_FILENO, TCSANOW, &t) < 0)
    return FALSE;

  return TRUE;
}


bool_t
wh_enable_echo ()
{
  struct termios t;

  if (tcgetattr( STDIN_FILENO, &t) < 0)
    return FALSE;

  t.c_lflag |= ECHO;
  if (tcsetattr( STDIN_FILENO, TCSANOW, &t) < 0)
    return FALSE;

  return TRUE;
}
