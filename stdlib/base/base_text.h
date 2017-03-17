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

#ifndef BASE_TEXT_H_
#define BASE_TEXT_H_


#include "stdlib/interface.h"


extern whais::WLIB_PROC_DESCRIPTION       gIsUpper;
extern whais::WLIB_PROC_DESCRIPTION       gIsLower;
extern whais::WLIB_PROC_DESCRIPTION       gIsDigit;
extern whais::WLIB_PROC_DESCRIPTION       gIsAlpha;
extern whais::WLIB_PROC_DESCRIPTION       gIsSpace;
extern whais::WLIB_PROC_DESCRIPTION       gIsPunct;
extern whais::WLIB_PROC_DESCRIPTION       gIsPrint;
extern whais::WLIB_PROC_DESCRIPTION       gUnicodeCP;
extern whais::WLIB_PROC_DESCRIPTION       gUpperChar;
extern whais::WLIB_PROC_DESCRIPTION       gLowerChar;
extern whais::WLIB_PROC_DESCRIPTION       gUpperText;
extern whais::WLIB_PROC_DESCRIPTION       gLowerText;
extern whais::WLIB_PROC_DESCRIPTION       gTextToUtf8;
extern whais::WLIB_PROC_DESCRIPTION       gTextFromUtf8;
extern whais::WLIB_PROC_DESCRIPTION       gTextToUtf16;
extern whais::WLIB_PROC_DESCRIPTION       gTextFromUtf16;
extern whais::WLIB_PROC_DESCRIPTION       gTextToCharArray;
extern whais::WLIB_PROC_DESCRIPTION       gTextFromCharArray;
extern whais::WLIB_PROC_DESCRIPTION       gTextCharsCount;
extern whais::WLIB_PROC_DESCRIPTION       gTextHash;
extern whais::WLIB_PROC_DESCRIPTION       gCharFind;
extern whais::WLIB_PROC_DESCRIPTION       gTextFind;
extern whais::WLIB_PROC_DESCRIPTION       gTextReplace;
extern whais::WLIB_PROC_DESCRIPTION       gTextCompare;


whais::WLIB_STATUS
base_text_init();


#endif //BASE_TEXT_H_
