/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whisper.
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
 *****************************************************************************/

#ifndef BASE_CONSTANTS_H_
#define BASE_CONSTANTS_H_

#include "whisper.h"

#include "stdlib/interface.h"

extern whisper::WLIB_PROC_DESCRIPTION     gProcPI;
extern whisper::WLIB_PROC_DESCRIPTION     gProcSQRT2;
extern whisper::WLIB_PROC_DESCRIPTION     gProcNE;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMinB;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinC;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMinD;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinDT;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinHT;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMinI8;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinI16;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinI32;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinI64;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMinU8;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinU16;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinU32;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinU64;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMinR;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMinRR;


extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxB;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxC;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxD;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxDT;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxHT;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxI8;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxI16;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxI32;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxI64;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxU8;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxU16;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxU32;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxU64;

extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxR;
extern whisper::WLIB_PROC_DESCRIPTION     gProcMaxRR;


whisper::WLIB_STATUS
base_constants_init ();


#endif //BASE_CONSTANTS_H_

