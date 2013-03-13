/******************************************************************************
 PASTRA - A light database one file system and more.
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

#ifndef UTF8_H_
#define UTF8_H_

#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

static const D_UINT8 UTF8_7BIT_MASK = 0x00;
static const D_UINT8 UTF8_11BIT_MASK = 0xC0;
static const D_UINT8 UTF8_16BIT_MASK = 0xE0;
static const D_UINT8 UTF8_21BIT_MASK = 0xF0;
static const D_UINT8 UTF8_26BIT_MASK = 0xF8;
static const D_UINT8 UTF8_31BIT_MASK = 0xFC;
static const D_UINT8 UTF8_37BIT_MASK = 0xFE;

static const D_UINT8 UTF8_EXTRA_BYTE_SIG = 0x80;
static const D_UINT8 UTF8_EXTRA_BYTE_MASK  = 0xC0;
static const D_UINT8 UTF8_MAX_BYTES_COUNT = 0x8;

D_UINT
get_utf8_char_size (D_UINT8 firstUtf8Byte);

D_UINT
decode_utf8_char (const D_UINT8 *pSource, D_UINT32* pCh);

D_UINT
encode_utf8_char (D_UINT32 ch, D_UINT8 *pDest);

D_UINT
utf8_encode_size (D_UINT32 codePoint);

D_INT
utf8_strlen (const D_UINT8* pSource);

#ifdef __cplusplus
}
#endif

#endif /* UTF8_H_ */
