/******************************************************************************
UTILS - Common routines used trough WHISPER project
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
******************************************************************************/

#ifndef WUNICODE_H_
#define WUNICODE_H_

#include "whisper.h"



#ifdef __cplusplus
extern "C" {
#endif


bool_t
wh_is_lowercase (const uint32_t codePoint);


bool_t
wh_is_uppercase (const uint32_t codePoint);


uint32_t
wh_to_lowercase (const uint32_t codePoint);


uint32_t
wh_to_uppercase (const uint32_t codePoint);

int
wh_cmp_alphabetically (const uint32_t cp1, const uint32_t cp2);

uint32_t
wh_prev_char (const uint32_t codePoint);

uint32_t
wh_next_char (const uint32_t codePoint);

uint32_t
wh_to_base_letter (const uint32_t codePoint);

#ifdef __cplusplus
} /* exntern "C" */
#endif

#endif /* WUNICODE_H_ */

