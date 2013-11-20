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

#ifndef BASE_TEXT_H_
#define BASE_TEXT_H_

#include "whisper.h"

#include "stdlib/interface.h"

extern whisper::WLIB_PROC_DESCRIPTION       gIsUpper;
extern whisper::WLIB_PROC_DESCRIPTION       gIsLower;
extern whisper::WLIB_PROC_DESCRIPTION       gIsDigit;
extern whisper::WLIB_PROC_DESCRIPTION       gIsAlpha;
extern whisper::WLIB_PROC_DESCRIPTION       gIsSpace;
extern whisper::WLIB_PROC_DESCRIPTION       gIsPunct;
extern whisper::WLIB_PROC_DESCRIPTION       gIsPrint;
extern whisper::WLIB_PROC_DESCRIPTION       gUnicodeCP;
extern whisper::WLIB_PROC_DESCRIPTION       gUpperChar;
extern whisper::WLIB_PROC_DESCRIPTION       gLowerChar;
extern whisper::WLIB_PROC_DESCRIPTION       gUpperText;
extern whisper::WLIB_PROC_DESCRIPTION       gLowerText;
extern whisper::WLIB_PROC_DESCRIPTION       gTextToUtf8;
extern whisper::WLIB_PROC_DESCRIPTION       gTextFromUtf8;
extern whisper::WLIB_PROC_DESCRIPTION       gTextToUtf16;
extern whisper::WLIB_PROC_DESCRIPTION       gTextFromUtf16;
extern whisper::WLIB_PROC_DESCRIPTION       gTextToCharArray;
extern whisper::WLIB_PROC_DESCRIPTION       gTextFromCharArray;
extern whisper::WLIB_PROC_DESCRIPTION       gTextCharsCount;
extern whisper::WLIB_PROC_DESCRIPTION       gTextHash;
extern whisper::WLIB_PROC_DESCRIPTION       gCharFind;
extern whisper::WLIB_PROC_DESCRIPTION       gTextFind;
extern whisper::WLIB_PROC_DESCRIPTION       gTextCompare;

whisper::WLIB_STATUS
base_text_init ();

#endif //BASE_TEXT_H_

