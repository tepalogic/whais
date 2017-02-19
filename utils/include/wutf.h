/******************************************************************************
 PASTRA - A light database one file system and more.
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

#ifndef WUTF_H_
#define WUTF_H_

#include "whais.h"

#ifdef __cplusplus
extern "C" {
#endif

static const uint_t UTF_LAST_CODEPOINT          = 0x10FFFF;

static const uint_t UTF8_7BIT_MASK              = 0x00;
static const uint_t UTF8_11BIT_MASK             = 0xC0;
static const uint_t UTF8_16BIT_MASK             = 0xE0;
static const uint_t UTF8_21BIT_MASK             = 0xF0;
static const uint_t UTF8_26BIT_MASK             = 0xF8;
static const uint_t UTF8_31BIT_MASK             = 0xFC;
static const uint_t UTF8_37BIT_MASK             = 0xFE;

static const uint_t UTF8_EXTRA_BYTE_SIG         = 0x80;
static const uint_t UTF8_EXTRA_BYTE_MASK        = 0xC0;
static const uint_t UTF8_MAX_BYTES_COUNT        = 0x08;

static const uint_t UTF16_EXTRA_BYTE_MIN        = 0xD800;
static const uint_t UTF16_EXTRA_BYTE_MAX        = 0xDFFF;
static const uint_t UTF16_EXTRA_10BIT_MASK      = 0x03FF;
static const uint_t UTF16_EXTRA_CODE_UNIT_MARK  = 0x10000;


/* Get the code units count of an UTF-8 encoded char using the
 * first code unit. */
uint_t
wh_utf8_cu_count(const uint8_t codeUnit);


/* Get the code units count of an UTF-16 encoded char using the first code
   unit. */
uint_t
wh_utf16_cu_count(const uint16_t codeUnit);


/* Get the Unicode code point of the first UTF-8 encoded char. */
uint_t
wh_load_utf8_cp(const uint8_t* const utf8Str, uint32_t* const outCodePoint);


/* Get the Unicode code point of the first UTF-16 encoded char. */
uint_t
wh_load_utf16_cp(const uint16_t* const utf16Str, uint32_t* const outCodePoint);


/* Store a Unicode code point using the UTF-8 encoding. */
uint_t
wh_store_utf8_cp(const uint32_t codePoint, uint8_t* const dest);


/* Store a Unicode code point using the UTF-16 encoding. */
uint_t
wh_store_utf16_cp(const uint32_t codePoint, uint16_t* const dest);


/* Get the required code units count to store this Unicode code point using
 * the UTF-8 encoding. */
uint_t
wh_utf8_store_size(const uint32_t codePoint);


/* Get the required code units count to store this Unicode code point using
 * the UTF-16 encoding. */
uint_t
wh_utf16_store_size(const uint32_t codePoint);


/*  Get the Unicode code points count from an UTF-8 encoded
 *  string(null terminated). */
int
wh_utf8_strlen(const uint8_t* utf8Str);


/*  Get the Unicode code points count from an UTF-16 encoded
 *  string(null terminated). */
int
wh_utf16_strlen(const uint16_t* utf16Str);



#ifdef __cplusplus
} //extern "C"

class UTF8_CU_COUNTER
{
public:
  static uint8_t COUNTS[256];

  static uint8_t Count(const uint8_t cu)
  {
    return UTF8_CU_COUNTER::COUNTS [cu];
  }

  UTF8_CU_COUNTER()
    {
      for (uint_t i = 0; i < sizeof(COUNTS); ++i)
        COUNTS[i] = wh_utf8_cu_count(i);
    }
};

#endif

#endif /* WUTF_H_ */

