/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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

#ifndef BASE_ARRAYS_H_
#define BASE_ARRAYS_H_


#include "stdlib/interface.h"

extern whais::WLIB_PROC_DESCRIPTION         gProcArrayCount;
extern whais::WLIB_PROC_DESCRIPTION         gProcArraySort;
extern whais::WLIB_PROC_DESCRIPTION         gProcArrayMin;
extern whais::WLIB_PROC_DESCRIPTION         gProcArrayMax;
extern whais::WLIB_PROC_DESCRIPTION         gProcArrayTruncate;
extern whais::WLIB_PROC_DESCRIPTION         gProcArrayHash;

whais::WLIB_STATUS
base_arrays_init();

#endif /* BASE_ARRAYS_H_ */
