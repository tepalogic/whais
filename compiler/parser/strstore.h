/******************************************************************************
 WHAISC - A compiler for whais programs
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
/*
 * strstore.h - Declares a general purpose string storage to hold parsed text
 *              (e.g. the text resulted after parsing escape characters).
 */

#ifndef STRSTORE_H
#define STRSTORE_H

#include "whais.h"

/* A handler for a string store */
typedef void* StringStoreHnd;

StringStoreHnd
create_string_store();

void
release_string_store (StringStoreHnd* pStore);

char*
alloc_str (StringStoreHnd store, uint_t length);

#endif /* STRSTORE_H */

