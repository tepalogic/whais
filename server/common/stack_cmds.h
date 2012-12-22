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

D_UINT
cmd_pop_stack (ClientConnection& rConn, D_UINT16* const pDataOff);

D_UINT
cmd_push_stack (ClientConnection& rConn, D_UINT16* const pDataOff);

D_UINT
cmd_update_stack_top (ClientConnection& rConn, D_UINT16* const pDataOff);

D_UINT
cmd_read_basic_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT16* const   pDataOffset);

D_UINT
cmd_read_array_stack_top (ClientConnection& rConn,
                         StackValue&       topValue,
                         D_UINT64          hintPosition,
                         D_UINT16* const   pDataOffset);

D_UINT
cmd_read_text_stack_top (ClientConnection& rConn,
                         StackValue&       topValue,
                         D_UINT64          hintPosition,
                         D_UINT16* const   pDataOffset);

D_UINT
cmd_read_field_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT64          rowHint,
                          D_UINT64          hintPosition,
                          D_UINT16* const   pDataOffset);

D_UINT
cmd_read_table_stack_top (ClientConnection& rConn,
                          StackValue&       topValue,
                          D_UINT64          hintField,
                          D_UINT64          hintRow,
                          D_UINT64          hintPosition,
                          D_UINT16* const   pDataOffset);
#endif /* STACK_CMDS_H */

