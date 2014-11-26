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

#ifndef WRANDOM_H_
#define WRANDOM_H_

#include "whais.h"


#ifdef __cplusplus
extern "C" {
#endif



/* Get the seed used to generate the pseudo randoms. */
uint64_t
wh_rnd_seed();


/* Reset the seed to a new value. If the new value is 0 then use
 * the default one. */
void
wh_rnd_set_seed( uint64_t seed);


/* Generate a pseudo random. */
uint64_t
wh_rnd();


#ifdef __cplusplus
}
#endif

#endif /* WRANDOM_H_ */

