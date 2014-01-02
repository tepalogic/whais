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

#ifndef BASE_FIELDS_H_
#define BASE_FIELDS_H_


#include "stdlib/interface.h"


extern whisper::WLIB_PROC_DESCRIPTION         gProcFieldTable;
extern whisper::WLIB_PROC_DESCRIPTION         gProcIsFielsIndexed;
extern whisper::WLIB_PROC_DESCRIPTION         gProcFieldName;
extern whisper::WLIB_PROC_DESCRIPTION         gProcFieldIndex;
extern whisper::WLIB_PROC_DESCRIPTION         gProcFindValueRange;
extern whisper::WLIB_PROC_DESCRIPTION         gProcFieldMinimum;
extern whisper::WLIB_PROC_DESCRIPTION         gProcFieldMaximum;
extern whisper::WLIB_PROC_DESCRIPTION         gProcFieldAverage;
extern whisper::WLIB_PROC_DESCRIPTION         gProcFieldSortTable;


whisper::WLIB_STATUS
base_fields_init ();

#endif /* BASE_FIELDS_H_ */

