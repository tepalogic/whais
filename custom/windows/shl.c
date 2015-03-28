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

#include "whais.h"

WH_SHLIB
wh_shl_load (const char* const library)
{
  HMODULE hnd = LoadLibrary (library);

  return hnd;
}


void
wh_shl_release (WH_SHLIB shl)
{
  FreeLibrary (shl);
}


void*
wh_shl_symbol (WH_SHLIB shl, const char* const symbol)
{
  void* const result = (void*)GetProcAddress( shl, symbol);

  return result;
}

