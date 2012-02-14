/******************************************************************************
WHISPERC - A compiler for whisper programs
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

#ifndef WOPCODES_H
#define WOPCODES_H

#include "whisper.h"

enum W_OPCODE
{
  /* one byte opcodes */
  W_NA = 0,			/* invalid opcode */

  /* W_LDxx put a value on the stack */
  W_LDNULL,
  W_LDC,
  W_LDI8,
  W_LDI16,
  W_LDI32,
  W_LDI64,
  W_LDD,
  W_LDDT,
  W_LDHT,
  W_LDR,
  W_LDT,
  W_LDBT,
  W_LDBF,
  W_LDLO8,
  W_LDLO16,
  W_LDLO32,
  W_LDGB8,
  W_LDGB16,
  W_LDGB32,
  W_CTS,

  W_STB,
  W_STC,
  W_STD,
  W_STDT,
  W_STHT,
  W_ST,
  W_STR,
  W_STRR,
  W_STT,
  W_STRO,
  W_STTA,
  W_STA,

  W_CALL,
  W_RET,

  /* W_ADDxx add the two value from the top of stack */
  W_ADD,
  W_ADDR,
  W_ADDRR,
  W_ADDT,

  /* W_ANDxx, logical/arithmetical and operator */
  W_AND,
  W_ANDB,

  /* W_DEC, decrement operator */
  W_DEC,

  /* W_DIVxx, divide operator */
  W_DIV,
  W_DIVR,
  W_DIVRR,

  /* W_EQxx, equality operator */
  W_EQ,
  W_EQB,
  W_EQC,
  W_EQD,
  W_EQDT,
  W_EQHT,
  W_EQR,
  W_EQRR,
  W_EQT,

  /* W_GExx, grater or equal operator */
  W_GE,
  W_GEC,
  W_GED,
  W_GEDT,
  W_GEHT,
  W_GER,
  W_GERR,
  W_GET,

  /* W_GT, grater than operator */
  W_GT,
  W_GTC,
  W_GTD,
  W_GTDT,
  W_GTHT,
  W_GTR,
  W_GTRR,
  W_GTT,

  /* W_INCxx, increment operator */
  W_INC,

  /* W_LE, less or equal operator */
  W_LE,
  W_LEC,
  W_LED,
  W_LEDT,
  W_LEHT,
  W_LER,
  W_LERR,
  W_LET,

  /* W_LT, less than operator */
  W_LT,
  W_LTC,
  W_LTD,
  W_LTDT,
  W_LTHT,
  W_LTR,
  W_LTRR,
  W_LTT,

  /* W_MODxx, modulo operator */
  W_MOD,

  /* W_MULxx, multiplication operator */
  W_MUL,
  W_MULR,
  W_MULRR,

  /* W_NExx,  unequal operator */
  W_NE,
  W_NEB,
  W_NEC,
  W_NED,
  W_NEDT,
  W_NEHT,
  W_NER,
  W_NERR,
  W_NET,

  /* W_NOTxx, negation operator or bit inversion operator */
  W_NOT,

  W_OR,
  W_ORB,

  /*W_SUBxx, subtract operator */
  W_SUB,
  W_SUBR,
  W_SUBRR,

  /* W_XORxx, xor operator */
  W_XOR,
  W_XORB,

  W_JF,
  W_JFC,
  W_JT,
  W_JTC,
  W_JMP,

  W_INDT,
  W_INDA,
  W_INDR,
  W_SELF,

  W_BSYNC,
  W_ESYNC,

  W_OP_END_MARK
};

#ifdef __cplusplus
extern "C"
{
#endif

unsigned int
whc_decode_opcode (const unsigned char *ins, enum W_OPCODE *opcode);

#ifdef __cplusplus
}
#endif

#endif				/* WOPCODES_H */
