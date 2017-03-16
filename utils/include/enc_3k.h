/******************************************************************************
UTILS - Common routines used trough WHAIS project
Copyright(C) 2009  Iulian Popa

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

#ifndef ENC_3K_H_
#define ENC_3K_H_


#include "whais.h"


#ifdef __cplusplus
extern "C" {
#endif


void
wh_buff_3k_encode(const uint32_t firstKing,
                  const uint32_t secondKing,
                  const uint8_t* const key,
                  const uint_t keyLen,
                  uint8_t* const buffer,
                  const uint_t bufferSize);

void
wh_buff_3k_decode(const uint32_t firstKing,
                  const uint32_t secondKing,
                  const uint8_t* const key,
                  const uint_t keyLen,
                  uint8_t* const buffer,
                  const uint_t bufferSize);


#ifdef __cplusplus
}
#endif

#endif /* ENC_3K_H_ */
