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

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#include "wod_dump.h"


#include "../../utils/include/le_converter.h"

static const D_UINT MAX_INT64_LENGTH = 25;	//log (MAX_UINT64) = 19.34

static const D_CHAR *
int_to_ascii (D_UINT64 value, D_CHAR * t_buffer, D_BOOL is_unsigned = TRUE)
{
  D_UINT i = MAX_INT64_LENGTH - 1;
  t_buffer[i--] = 0;
  const D_BOOL negative = is_unsigned ? FALSE : ((value >> 63) & 1) != 0;

  if (negative)
    value *= -1;

  for (;; value /= 10)
    {
      t_buffer[i] = (value % 10) + '0';
      if (value < 10)
	break;
      --i;
    }

  if (negative)
    t_buffer[--i] = '-';

  return t_buffer + i;
}

static inline void
int8_str_conv (D_CHAR * output, D_UINT8 value)
{
  const D_UINT8 hi_digit = (value >> 4) & 0x0F;
  const D_UINT8 lo_digit = value & 0x0F;

  output[0] = (hi_digit > 9) ? 'A' + (hi_digit - 10) : '0' + hi_digit;
  output[1] = (lo_digit > 9) ? 'A' + (lo_digit - 10) : '0' + lo_digit;
  output[2] = 0;
}

static D_UINT
wod_dec_w_na (const D_UINT8 * in_args, D_CHAR * operand_1, D_CHAR * operand_2)
{
  throw WDumpException ("Not a valid opcode encountered!", _EXTRA(0));
  return 0;
}

static D_UINT
wod_dec_w_ldnull (const D_UINT8 * in_args, D_CHAR * operand_1,
		  D_CHAR * operand_2)
{
  operand_1[0] = operand_2[0] = 0;

  return 0;
}

static D_UINT
wod_dec_w_ldc (const D_UINT8 * in_args, D_CHAR * operand_1,
	       D_CHAR * operand_2)
{
  //Does not support any extended character set yet!
  operand_1[0] = '\'';
  operand_1[1] = in_args[0];
  operand_1[2] = '\'';
  operand_1[3] = operand_2[0] = 0;
  return 1;
}

static D_UINT
wod_dec_w_ldi8 (const D_UINT8 * in_args, D_CHAR * operand_1,
		D_CHAR * operand_2)
{
  //Does not support any extended character set yet!
  int8_str_conv (operand_1, in_args[0]);
  strcat (operand_1, "h");
  operand_2[0] = 0;

  return 1;
}

static D_UINT
wod_dec_w_ldi16 (const D_UINT8 * in_args, D_CHAR * operand_1,
		 D_CHAR * operand_2)
{
  int8_str_conv (operand_1, in_args[1]);
  int8_str_conv (operand_1 + 2, in_args[0]);
  strcat (operand_1, "h");
  operand_2[0] = 0;

  return 2;
}

static D_UINT
wod_dec_w_ldi32 (const D_UINT8 * in_args, D_CHAR * operand_1,
		 D_CHAR * operand_2)
{
  int8_str_conv (operand_1, in_args[3]);
  int8_str_conv (operand_1 + 2, in_args[2]);
  int8_str_conv (operand_1 + 4, in_args[1]);
  int8_str_conv (operand_1 + 6, in_args[0]);
  strcat (operand_1, "h");
  operand_2[0] = 0;

  return 4;
}

static D_UINT
wod_dec_w_ldi64 (const D_UINT8 * in_args, D_CHAR * operand_1,
		 D_CHAR * operand_2)
{
  int8_str_conv (operand_1, in_args[7]);
  int8_str_conv (operand_1 + 2, in_args[6]);
  int8_str_conv (operand_1 + 4, in_args[5]);
  int8_str_conv (operand_1 + 6, in_args[4]);
  int8_str_conv (operand_1 + 8, in_args[3]);
  int8_str_conv (operand_1 + 10, in_args[2]);
  int8_str_conv (operand_1 + 12, in_args[1]);
  int8_str_conv (operand_1 + 14, in_args[0]);
  strcat (operand_1, "h");
  operand_2[0] = 0;

  return 8;
}

static D_UINT
wod_dec_w_ldd (const D_UINT8 * in_args, D_CHAR * operand_1,
	       D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];

  const D_UINT8 day = in_args[0];
  const D_UINT8 month = in_args[1];
  const D_INT16 year = _SC (D_INT16, from_le_int16 (in_args + 2));

  assert (day >= 1 && day <= 31);
  assert (month >= 1 && month <= 12);

  strcpy (operand_1, int_to_ascii (_SC (D_INT64, year), t_str, FALSE));
  strcat (operand_1, "/");
  strcat (operand_1, int_to_ascii (month, t_str));
  strcat (operand_1, "/");
  strcat (operand_1, int_to_ascii (day, t_str));

  operand_2[0] = 0;

  return 4;
}

static D_UINT
wod_dec_w_lddt (const D_UINT8 * in_args, D_CHAR * operand_1,
		D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];

  const D_UINT8 sec = in_args[0];
  const D_UINT8 min = in_args[1];
  const D_UINT8 hour = in_args[2];
  const D_UINT8 day = in_args[3];
  const D_UINT8 month = in_args[4];
  const D_INT16 year = _SC (D_INT16, from_le_int16 (in_args + 5));

  assert (sec < 60);
  assert (min < 60);
  assert (hour < 24);
  assert (day >= 1 && day <= 31);
  assert (month >= 1 && month <= 12);

  strcpy (operand_1, int_to_ascii (_SC (D_INT16, year), t_str, FALSE));
  strcat (operand_1, "/");
  strcat (operand_1, int_to_ascii (month, t_str));
  strcat (operand_1, "/");
  strcat (operand_1, int_to_ascii (day, t_str));
  strcat (operand_1, " ");
  strcat (operand_1, int_to_ascii (hour, t_str));
  strcat (operand_1, ":");
  strcat (operand_1, int_to_ascii (min, t_str));
  strcat (operand_1, ":");
  strcat (operand_1, int_to_ascii (sec, t_str));

  operand_2[0] = 0;

  return 7;
}

static D_UINT
wod_dec_w_ldht (const D_UINT8 * in_args, D_CHAR * operand_1,
		D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];

  const D_UINT32 u_sec = from_le_int32 (in_args);
  const D_UINT8 sec = in_args[4];
  const D_UINT8 min = in_args[5];
  const D_UINT8 hour = in_args[6];
  const D_UINT8 day = in_args[7];
  const D_UINT8 month = in_args[8];
  const D_INT16 year = _SC (D_INT16, from_le_int16 (in_args + 9));

  assert (sec < 60);
  assert (min < 60);
  assert (hour < 24);
  assert (day >= 1 && day <= 31);
  assert (month >= 1 && month <= 12);

  strcpy (operand_1, int_to_ascii (_SC (D_INT64, year), t_str, FALSE));
  strcat (operand_1, "/");
  strcat (operand_1, int_to_ascii (month, t_str));
  strcat (operand_1, "/");
  strcat (operand_1, int_to_ascii (day, t_str));
  strcat (operand_1, " ");
  strcat (operand_1, int_to_ascii (hour, t_str));
  strcat (operand_1, ":");
  strcat (operand_1, int_to_ascii (min, t_str));
  strcat (operand_1, ":");
  strcat (operand_1, int_to_ascii (sec, t_str));

  strcat (operand_1, ".");
  strcat (operand_1, int_to_ascii (u_sec, t_str));

  operand_2[0] = 0;

  return 11;
}

static D_UINT
wod_dec_w_ldr (const D_UINT8 * in_args, D_CHAR * operand_1,
	       D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];
  D_UINT64 real_part = from_le_int64 (in_args);
  D_UINT64 frac_part = from_le_int64 (in_args + sizeof (D_UINT64));

  strcpy (operand_1, int_to_ascii (real_part, t_str, FALSE));
  strcat (operand_1, ".");

  if (frac_part == 0)
    strcat (operand_1, "0");
  else
    {
      D_UINT operand_pos = strlen (operand_1);
      while (frac_part != 0)
	{
	  const D_UINT8 t_value = ((frac_part >> 7) & 0xFF);
	  int8_str_conv (operand_1, t_value);
	  operand_pos += 2;
	  frac_part <<= 8;
	}
    }
  operand_2[0] = 0;
  return (2 * sizeof (D_UINT64));
}

static FDECODE_OPCODE wod_dec_w_ldt = wod_dec_w_ldi32;

const FDECODE_OPCODE wod_dec_w_ldbt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ldbf = wod_dec_w_ldnull;

static D_UINT
wod_dec_w_ldlo8 (const D_UINT8 * in_args, D_CHAR * operand_1,
		 D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];
  const D_UINT8 value = in_args[0];

  strcpy (operand_1, int_to_ascii (value, t_str));
  operand_2[0] = 0;

  return sizeof (D_UINT8);
}

static D_UINT
wod_dec_w_ldlo16 (const D_UINT8 * in_args, D_CHAR * operand_1,
		  D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];
  const D_UINT16 value = from_le_int16 (in_args);

  strcpy (operand_1, int_to_ascii (value, t_str));
  operand_2[0] = 0;

  return sizeof (D_UINT16);
}

static D_UINT
wod_dec_w_ldlo32 (const D_UINT8 * in_args, D_CHAR * operand_1,
		  D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];
  const D_UINT32 value = from_le_int32 (in_args);

  strcpy (operand_1, int_to_ascii (value, t_str));
  operand_2[0] = 0;

  return sizeof (D_UINT32);
}

const FDECODE_OPCODE wod_dec_w_ldgb8 = wod_dec_w_ldlo8;
const FDECODE_OPCODE wod_dec_w_ldgb16 = wod_dec_w_ldlo16;
const FDECODE_OPCODE wod_dec_w_ldgb32 = wod_dec_w_ldlo32;

const FDECODE_OPCODE wod_dec_w_cts = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_stb = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_stc = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_std = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_stdt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_stht = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_st = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_str = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_strr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_stt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_stro = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_stta = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_sta = wod_dec_w_ldnull;

static D_UINT
wod_dec_w_call (const D_UINT8 * in_args, D_CHAR * operand_1,
		D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];
  const D_UINT32 value = from_le_int32 (in_args);

  strcpy (operand_1, int_to_ascii (value, t_str));
  operand_2[0] = 0;

  return sizeof (D_UINT32);
}

const FDECODE_OPCODE wod_dec_w_ret = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_add = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_addr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_addrr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_addt = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_and = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_andb = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_dec = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_div = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_divr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_divrr = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_eq = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqb = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqc = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqd = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqdt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqht = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqrr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_eqt = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_ge = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gec = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ged = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gedt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_geht = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ger = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gerr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_get = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_gt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gtc = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gtd = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gtdt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gtht = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gtr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gtrr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_gtt = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_inc = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_le = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_lec = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_led = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ledt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_leht = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ler = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_lerr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_let = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_lt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ltc = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ltd = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ltdt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ltht = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ltr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ltrr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ltt = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_mod = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_mul = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_mulr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_mulrr = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_ne = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_neb = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_nec = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ned = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_nedt = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_neht = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_ner = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_nerr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_net = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_not = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_or = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_orb = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_sub = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_subr = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_subrr = wod_dec_w_ldnull;

const FDECODE_OPCODE wod_dec_w_xor = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_xorb = wod_dec_w_ldnull;

static D_UINT
wod_dec_w_jf (const D_UINT8 * in_args, D_CHAR * operand_1, D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];
  const D_INT32 temp = _SC (D_INT32, from_le_int32 (in_args));
  const D_INT64 value = temp;

  strcpy (operand_1, int_to_ascii (value, t_str, FALSE));
  operand_2[0] = 0;

  return sizeof (D_UINT32);
}

const FDECODE_OPCODE wod_dec_w_jfc = wod_dec_w_jf;
const FDECODE_OPCODE wod_dec_w_jt = wod_dec_w_jf;
const FDECODE_OPCODE wod_dec_w_jtc = wod_dec_w_jf;
const FDECODE_OPCODE wod_dec_w_jmp = wod_dec_w_jf;

const FDECODE_OPCODE wod_dec_w_ind = wod_dec_w_ldnull;
const FDECODE_OPCODE wod_dec_w_self = wod_dec_w_ldt;

static D_UINT
wod_dec_w_bsync (const D_UINT8 * in_args, D_CHAR * operand_1,
		 D_CHAR * operand_2)
{
  D_CHAR t_str[MAX_INT64_LENGTH];
  const D_UINT8 value = in_args[0];

  strcpy (operand_1, int_to_ascii (value, t_str));
  operand_2[0] = 0;

  return sizeof (D_UINT8);

}

const FDECODE_OPCODE wod_dec_w_esync = wod_dec_w_bsync;

FDECODE_OPCODE wod_decode_table[] = {
  wod_dec_w_na,

  wod_dec_w_ldnull,
  wod_dec_w_ldc,
  wod_dec_w_ldi8,
  wod_dec_w_ldi16,
  wod_dec_w_ldi32,
  wod_dec_w_ldi64,
  wod_dec_w_ldd,
  wod_dec_w_lddt,
  wod_dec_w_ldht,
  wod_dec_w_ldr,
  wod_dec_w_ldt,
  wod_dec_w_ldbt,
  wod_dec_w_ldbf,
  wod_dec_w_ldlo8,
  wod_dec_w_ldlo16,
  wod_dec_w_ldlo32,
  wod_dec_w_ldgb8,
  wod_dec_w_ldgb16,
  wod_dec_w_ldgb32,
  wod_dec_w_cts,

  wod_dec_w_stb,
  wod_dec_w_stc,
  wod_dec_w_std,
  wod_dec_w_stdt,
  wod_dec_w_stht,
  wod_dec_w_st,
  wod_dec_w_str,
  wod_dec_w_strr,
  wod_dec_w_stt,
  wod_dec_w_stro,
  wod_dec_w_stta,
  wod_dec_w_sta,

  wod_dec_w_call,
  wod_dec_w_ret,

  wod_dec_w_add,
  wod_dec_w_addr,
  wod_dec_w_addrr,
  wod_dec_w_addt,

  wod_dec_w_and,
  wod_dec_w_andb,

  wod_dec_w_dec,

  wod_dec_w_div,
  wod_dec_w_divr,
  wod_dec_w_divrr,

  wod_dec_w_eq,
  wod_dec_w_eqb,
  wod_dec_w_eqc,
  wod_dec_w_eqd,
  wod_dec_w_eqdt,
  wod_dec_w_eqht,
  wod_dec_w_eqr,
  wod_dec_w_eqrr,
  wod_dec_w_eqt,

  wod_dec_w_ge,
  wod_dec_w_gec,
  wod_dec_w_ged,
  wod_dec_w_gedt,
  wod_dec_w_geht,
  wod_dec_w_ger,
  wod_dec_w_gerr,
  wod_dec_w_get,

  wod_dec_w_gt,
  wod_dec_w_gtc,
  wod_dec_w_gtd,
  wod_dec_w_gtdt,
  wod_dec_w_gtht,
  wod_dec_w_gtr,
  wod_dec_w_gtrr,
  wod_dec_w_gtt,

  wod_dec_w_inc,

  wod_dec_w_le,
  wod_dec_w_lec,
  wod_dec_w_led,
  wod_dec_w_ledt,
  wod_dec_w_leht,
  wod_dec_w_ler,
  wod_dec_w_lerr,
  wod_dec_w_let,

  wod_dec_w_lt,
  wod_dec_w_ltc,
  wod_dec_w_ltd,
  wod_dec_w_ltdt,
  wod_dec_w_ltht,
  wod_dec_w_ltr,
  wod_dec_w_ltrr,
  wod_dec_w_ltt,

  wod_dec_w_mod,

  wod_dec_w_mul,
  wod_dec_w_mulr,
  wod_dec_w_mulrr,

  wod_dec_w_ne,
  wod_dec_w_neb,
  wod_dec_w_nec,
  wod_dec_w_ned,
  wod_dec_w_nedt,
  wod_dec_w_neht,
  wod_dec_w_ner,
  wod_dec_w_nerr,
  wod_dec_w_net,

  wod_dec_w_not,

  wod_dec_w_or,
  wod_dec_w_orb,

  wod_dec_w_sub,
  wod_dec_w_subr,
  wod_dec_w_subrr,

  wod_dec_w_xor,
  wod_dec_w_xorb,

  wod_dec_w_jf,
  wod_dec_w_jfc,
  wod_dec_w_jt,
  wod_dec_w_jtc,
  wod_dec_w_jmp,

  wod_dec_w_ind,
  wod_dec_w_self,

  wod_dec_w_bsync,
  wod_dec_w_esync
};

const D_CHAR *wod_str_table[] = {
  "na",

  "ldnull",
  "ldc",
  "ldi8",
  "ldi16",
  "ldi32",
  "ldi64",
  "ldd",
  "lddt",
  "ldht",
  "ldr",
  "ldt",
  "ldbt",
  "ldbf",
  "ldlo8",
  "ldlo16",
  "ldlo32",
  "ldgb8",
  "ldgb16",
  "ldgb32",
  "cts",

  "stb",
  "stc",
  "std",
  "stdt",
  "stht",
  "st",
  "str",
  "strr",
  "stt",
  "stro",
  "stta",
  "sta",

  "call",
  "ret",

  "add",
  "addr",
  "addrr",
  "addt",

  "and",
  "andb",

  "dec",

  "div",
  "divr",
  "divrr",

  "eq",
  "eqb",
  "eqc",
  "eqd",
  "eqdt",
  "eqht",
  "eqr",
  "eqrr",
  "eqt",

  "ge",
  "gec",
  "ged",
  "gedt",
  "geht",
  "ger",
  "gerr",
  "get",

  "gt",
  "gtc",
  "gtd",
  "gtdt",
  "gtht",
  "gtr",
  "gtrr",
  "gtt",

  "inc",

  "le",
  "lec",
  "led",
  "ledt",
  "leht",
  "ler",
  "lerr",
  "let",

  "lt",
  "ltc",
  "ltd",
  "ltdt",
  "ltht",
  "ltr",
  "ltrr",
  "ltt",

  "mod",

  "mul",
  "mulr",
  "mulrr",

  "ne",
  "neb",
  "nec",
  "ned",
  "nedt",
  "neht",
  "ner",
  "nerr",
  "net",

  "not",

  "or",
  "orb",

  "sub",
  "subr",
  "subrr",

  "xor",
  "xorb",

  "jf",
  "jfc",
  "jt",
  "jtc",
  "jmp",

  "ind",
  "self",

  "bsync",
  "esync"
};
