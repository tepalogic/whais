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

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

#include "whais.h"

WH_SHLIB
wh_shl_load(const char* const library)
{
  int i = 0;
  char name[512];

  WH_SHLIB result = dlopen(library, RTLD_NOW | RTLD_GLOBAL);
  if (result != INVALID_SHL)
    return result;

  i = strlen(library) + 1;

  if (sizeof name < (i + 3 + 3)) /* "lib" + * + ".so" */
    return INVALID_SHL;

  strcpy(name, library);
  while(--i >= 0)
    {
      if (name[i] == '/')
        break;
    }

  ++i;
  sprintf(name + i, "lib%s.so", library + i);

  result = dlopen(name, RTLD_NOW | RTLD_GLOBAL);
  return result;
}


void
wh_shl_release(WH_SHLIB shl)
{
  dlclose(shl);
}


void*
wh_shl_symbol(WH_SHLIB shl, const char* const symbol)
{
  void* const result = dlsym(shl, symbol);

  return result;
}

