/******************************************************************************
UTILS - Common routines used trough WHAIS project
Copyright (C) 2009  Iulian Popa

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

#ifndef ENC_DES_H_
#define ENC_DES_H_

#include "whais.h"

#ifdef __cplusplus
extern "C" {
#endif


void
wh_buff_des_encode (const uint8_t* const     key,
                    uint8_t* const           buffer,
                    const uint_t             bufferSize);
void
wh_buff_des_decode (const uint8_t* const     key,
                    uint8_t* const           buffer,
                    const uint_t             bufferSize);

void
wh_buff_3des_encode (const uint8_t* const     key,
                     uint8_t* const           buffer,
                     const uint_t             bufferSize);

void
wh_buff_3des_decode (const uint8_t* const     key,
                     uint8_t* const           buffer,
                     const uint_t             bufferSize);

void
wh_prepare_des_keys (const uint8_t*           userKey,
                     const uint_t             keyLenght,
                     const bool_t             _3des,
                     uint64_t* const          outPreparedKeys);
void
wh_buff_des_encode_ex (const uint64_t* const    preparedKeys,
                       uint8_t* const           buffer,
                       const uint_t             bufferSize);
void
wh_buff_des_decode_ex (const uint64_t* const    preparedKeys,
                       uint8_t* const           buffer,
                       const uint_t             bufferSize);

void
wh_buff_3des_encode_ex (const uint64_t* const    preparedKeys,
                        uint8_t* const           buffer,
                        const uint_t             bufferSize);

void
wh_buff_3des_decode_ex (const uint64_t* const    preparedKeys,
                        uint8_t* const           buffer,
                        const uint_t             bufferSize);


#ifdef __cplusplus
}
#endif

#endif /* ENC_DES_H_ */
