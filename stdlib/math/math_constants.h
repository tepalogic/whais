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
/******************************************************************************
 Template file containing a skeleton to write a a natve Whisper library.
 *****************************************************************************/

#ifndef MATH_CONSTANTS_H_
#define MATH_CONSTANTS_H_

#include "whisper.h"

#include "stdlib/interface.h"

#ifndef MATH_SKIP_EXTERNS_DECLS

extern const uint8_t* const gRichRealType;

extern whisper::WLIB_PROC_DESCRIPTION     gProcPI;
extern whisper::WLIB_PROC_DESCRIPTION     gProcEM;
extern whisper::WLIB_PROC_DESCRIPTION     gProcGR;
extern whisper::WLIB_PROC_DESCRIPTION     gProcSQRT2;
extern whisper::WLIB_PROC_DESCRIPTION     gProcNE;

#endif //MATH_SKIP_EXTERNS_DECLS


whisper::WLIB_STATUS
math_constants_init ();


#endif //MATH_CONSTANTS_H_
