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

#ifndef BASE_CONSTANTS_H_
#define BASE_CONSTANTS_H_


#include "stdlib/interface.h"


extern whais::WLIB_PROC_DESCRIPTION     gProcPHI;
extern whais::WLIB_PROC_DESCRIPTION     gProcPI;
extern whais::WLIB_PROC_DESCRIPTION     gProcSQRT2;
extern whais::WLIB_PROC_DESCRIPTION     gProcE;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinB;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinC;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinD;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinDT;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinHT;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinS8;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinS16;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinS32;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinS64;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinU8;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinU16;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinU32;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinU64;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinR;
extern whais::WLIB_PROC_DESCRIPTION     gProcMinRR;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxB;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxC;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxD;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxDT;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxHT;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxS8;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxS16;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxS32;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxS64;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxU8;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxU16;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxU32;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxU64;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxR;
extern whais::WLIB_PROC_DESCRIPTION     gProcMaxRR;


whais::WLIB_STATUS
base_constants_init();

#endif //BASE_CONSTANTS_H_

