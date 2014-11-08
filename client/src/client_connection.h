/******************************************************************************
WHAIS - An advanced database system
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

#ifndef CLIENT_CONNECTION_H_
#define CLIENT_CONNECTION_H_

#include "whais_connector.h"

#include "connector.h"

/* Helper functions to factor to code recieveing/sending frames durring
   connection autentication step. */

uint_t
read_raw_frame( struct INTERNAL_HANDLER* const pHnd,
                uint_t* const                  outFrameSize);

uint_t
write_raw_frame( struct INTERNAL_HANDLER* const pHnd,
                 const  uint_t                  frameSize);

#endif /* CLIENT_CONNECTION_H_ */
