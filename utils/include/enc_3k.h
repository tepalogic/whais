/******************************************************************************
UTILS - Common routines used trough WHISPER project
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

#ifndef ENC_3K_H_
#define ENC_3K_H_

#include "whisper.h"

#ifdef __cplusplus
extern "C" {
#endif

void
encrypt_3k_buffer (const D_UINT32       firstKing,
                   const D_UINT32       secondKing,
                   const D_UINT8* const key,
                   const D_UINT         keyLen,
                   D_UINT8*             buffer,
                   const D_UINT         bufferSize);

void
decrypt_3k_buffer (const D_UINT32       firstKing,
                   const D_UINT32       secondKing,
                   const D_UINT8* const key,
                   const D_UINT         keyLen,
                   D_UINT8*             buffer,
                   const D_UINT         bufferSize);

#ifdef __cplusplus
}
#endif

#endif /* ENC_3K_H_ */
