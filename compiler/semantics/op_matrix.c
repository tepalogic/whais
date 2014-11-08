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

#include "opcodes.h"
#include "vardecl.h"

const enum W_OPCODE add_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADDT, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADD, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_ADDRR, W_NA, W_NA},
  {W_NA, W_NA, W_ADDT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_ADDT, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE sub_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUB, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_SUBRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE mul_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MUL, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_MULRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE div_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIV, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_DIVRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE mod_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_MOD, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE less_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_LTC, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_LTD, W_LTDT, W_LTHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_LTDT, W_LTDT, W_LTHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_LTHT, W_LTHT, W_LTHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LT, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_LTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE less_eq_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_LEC, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_LED, W_LEDT, W_LEHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_LEDT, W_LEDT, W_LEHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_LEHT, W_LEHT, W_LEHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LE, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_LERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE greater_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_GTC, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_GTD, W_GTDT, W_GTHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_GTDT, W_GTDT, W_GTHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_GTHT, W_GTHT, W_GTHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GT, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_GTRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE greater_eq_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_GEC, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_GED, W_GEDT, W_GEHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_GEDT, W_GEDT, W_GEHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_GEHT, W_GEHT, W_GEHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GE, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_GERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE equals_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_EQB, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_EQC, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_EQD, W_EQD, W_EQD, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_EQD, W_EQDT, W_EQDT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_EQD, W_EQDT, W_EQHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_EQ, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQRR, W_EQRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQRR, W_EQRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_EQT, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE not_equals_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NEB, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NEC, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NED, W_NED, W_NED, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NED, W_NEDT, W_NEDT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NED, W_NEDT, W_NEHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NE, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NERR, W_NERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NERR, W_NERR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NET, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE or_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_ORB, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_OR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE xor_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_XORB, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_XOR, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};

const enum W_OPCODE and_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_ANDB, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_AND, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};


const enum W_OPCODE store_op[T_END_OF_TYPES][T_END_OF_TYPES] =
{
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_STB, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_STC, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_STD, W_STD, W_STD, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_STDT, W_STDT, W_STDT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_STHT, W_STDT, W_STHT, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STI8, W_STI8, W_STI8, W_STI8, W_STI8, W_STI8, W_STI8, W_STI8, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STI16, W_STI16, W_STI16, W_STI16, W_STI16, W_STI16, W_STI16, W_STI16, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STI32, W_STI32, W_STI32, W_STI32, W_STI32, W_STI32, W_STI32, W_STI32, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STI64, W_STI64, W_STI64, W_STI64, W_STI64, W_STI64, W_STI64, W_STI64, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STUI8, W_STUI8, W_STUI8, W_STUI8, W_STUI8, W_STUI8, W_STUI8, W_STUI8, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STUI16, W_STUI16, W_STUI16, W_STUI16, W_STUI16, W_STUI16, W_STUI16, W_STUI16, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STUI32, W_STUI32, W_STUI32, W_STUI32, W_STUI32, W_STUI32, W_STUI32, W_STUI32, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STUI64, W_STUI64, W_STUI64, W_STUI64, W_STUI64, W_STUI64, W_STUI64, W_STUI64, W_NA, W_NA, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STR, W_STR, W_STR, W_STR, W_STR, W_STR, W_STR, W_STR, W_STR, W_STR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STRR, W_STRR, W_STRR, W_STRR, W_STRR, W_STRR, W_STRR, W_STRR, W_STRR, W_STRR, W_NA, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_STT, W_NA},
  {W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA, W_NA}
};


static const uint16_t SECOND_BYTE_MARK = 0x80;


struct WOutputStream*
encode_opcode( struct WOutputStream* stream, const enum W_OPCODE opcode)
{
  uint16_t tempOpcode = opcode;

  if (tempOpcode >= SECOND_BYTE_MARK)
    {
      tempOpcode |= (SECOND_BYTE_MARK << 8);

     if (wh_ostream_wint8 (stream, (uint8_t)((tempOpcode >> 8) & 0xFF)) == NULL)
       return NULL;
     return wh_ostream_wint8 (stream, (uint8_t)(tempOpcode & 0xFF));
    }

  return wh_ostream_wint8 (stream, (uint8_t)tempOpcode);
}


enum W_OPCODE
decode_opcode( const uint8_t* instrs)
{
  if (*instrs & SECOND_BYTE_MARK)
    {
      uint16_t opcode = (*instrs++ & ~SECOND_BYTE_MARK);
      opcode += *instrs;

      return opcode;
    }

  return *instrs;
}


uint_t
wh_compiler_decode_op( const uint8_t* instrs, enum W_OPCODE* const outOpcode)
{
  if (*instrs & SECOND_BYTE_MARK)
    {
      uint16_t opcode = (*instrs++ & ~SECOND_BYTE_MARK);
      opcode += *instrs;

      *outOpcode  = opcode;

      return 2;
    }

  *outOpcode = *instrs;
  return 1;
}
