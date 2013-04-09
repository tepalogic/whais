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

static const uint8_t UTF8_7BIT_MASK  = 0x00;
static const uint8_t UTF8_11BIT_MASK = 0xC0;
static const uint8_t UTF8_16BIT_MASK = 0xE0;
static const uint8_t UTF8_21BIT_MASK = 0xF0;
static const uint8_t UTF8_26BIT_MASK = 0xF8;
static const uint8_t UTF8_31BIT_MASK = 0xFC;
static const uint8_t UTF8_37BIT_MASK = 0xFE;

static const uint8_t UTF8_EXTRA_BYTE_SIG   = 0x80;
static const uint8_t UTF8_EXTRA_BYTE_MASK  = 0xC0;
static const uint8_t UTF8_MAX_BYTES_COUNT  = 0x08;

uint_t
get_utf8_char_size (uint8_t firstUtf8Byte);

uint_t
decode_utf8_char (const uint8_t *pSource, uint32_t* pCh);

uint_t
encode_utf8_char (uint32_t ch, uint8_t *pDest);

uint_t
utf8_encode_size (uint32_t codePoint);

int
utf8_strlen (const uint8_t* pSource);

#ifdef __cplusplus
}
#endif

#endif /* UTF8_H_ */
