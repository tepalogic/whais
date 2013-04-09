/******************************************************************************
WHISPER - An advanced database system
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

#ifndef STACK_CMDS_H
#define STACK_CMDS_H

#include "whisper.h"

#include "connection.h"

uint_t
cmd_pop_stack (ClientConnection& rConn, uint_t* const pDataOff);

uint_t
cmd_push_stack (ClientConnection& rConn, uint_t* const pDataOff);

uint_t
cmd_update_stack_top (ClientConnection& rConn, uint_t* const pDataOff);

uint_t
cmd_read_basic_stack_top (ClientConnection& rConn,
                          StackValue&       value,
                          uint_t* const     pDataOffset);

uint_t
cmd_read_array_stack_top (ClientConnection& rConn,
                          StackValue&       value,
                          uint64_t          hintOffset,
                          uint_t* const     pDataOffset);

uint_t
cmd_read_text_stack_top (ClientConnection& rConn,
                         StackValue&       value,
                         const uint64_t    hintOffset,
                         uint_t* const     pDataOffset);

uint_t
cmd_read_field_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          uint64_t          hintRow,
                          uint64_t          hintArrayOff,
                          uint64_t          hintTextOff,
                          uint_t* const     pDataOffset);

uint_t
cmd_read_table_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          const uint_t      hintField,
                          uint64_t          hintRow,
                          uint64_t          hintArrayOff,
                          uint64_t          hintTextOff,
                          uint_t* const     pDataOffset);

#endif /* STACK_CMDS_H */

