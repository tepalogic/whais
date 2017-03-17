/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whais.
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
 *****************************************************************************/

#ifndef BASE_GENERICS_H_
#define BASE_GENERICS_H_


#include "stdlib/interface.h"


extern whais::WLIB_PROC_DESCRIPTION     gProcPrevB;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevC;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevD;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevDT;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevHT;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevI8;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevI16;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevI32;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevI64;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevU8;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevU16;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevU32;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevU64;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevR;
extern whais::WLIB_PROC_DESCRIPTION     gProcPrevRR;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextB;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextC;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextD;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextDT;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextHT;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextI8;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextI16;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextI32;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextI64;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextU8;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextU16;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextU32;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextU64;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextR;
extern whais::WLIB_PROC_DESCRIPTION     gProcNextRR;
extern whais::WLIB_PROC_DESCRIPTION     gProcSleep;
extern whais::WLIB_PROC_DESCRIPTION     gProcLog;
extern whais::WLIB_PROC_DESCRIPTION     gProcCeil;
extern whais::WLIB_PROC_DESCRIPTION     gProcRound;
extern whais::WLIB_PROC_DESCRIPTION     gProcFloor;
extern whais::WLIB_PROC_DESCRIPTION     gProcAbs;
extern whais::WLIB_PROC_DESCRIPTION     gProcRnd;


whais::WLIB_STATUS
base_generics_init();


#endif //BASE_GENERICS_H_
