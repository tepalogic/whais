/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2014-2018  Iulian Popa

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

#include "utils/endianness.h"
#include "wod_dump.h"


namespace whais {
namespace wod {


static const uint_t MAX_INT64_LENGTH   = 25; //log(MAX_UINT64) = 19.34
static const uint_t MAX_RREAL_LENGTH   = 64; //log(MAX_UINT64) = 19.34


static const char*
into_ascii(uint64_t value, char* const dest, const bool_t isUnsigned = TRUE)
{
  const bool_t negative = isUnsigned ? FALSE : ((value & 0x8000000000000000ull) != 0);
  uint_t index = MAX_INT64_LENGTH - 1;

  dest[index--] = 0;

  if (negative)
    value *= -1;

  for (;; value /= 10)
  {
    dest[index] = (value % 10) + '0';
    if (value < 10)
      break;

    --index;
  }

  if (negative)
    dest[--index] = '-';

  return dest + index;
}

static const char*
fracint_to_ascii(uint64_t value, char* const dest, const uint64_t precision)
{
  uint_t index = MAX_INT64_LENGTH - 1;

  dest[index--] = 0;
  for (uint64_t p = 1; p < precision; p *= 10)
  {
    dest[index] = (value % 10) + '0';
    value /= 10;
    --index;
  }

  return dest + index + 1;
}

static inline void
int8_str_conv(char* const dest, const uint8_t value)
{
  const uint8_t hiDigit = (value >> 4) & 0x0F;
  const uint8_t loDigit = value & 0x0F;

  dest[0] = (hiDigit > 9) ? 'A' + (hiDigit - 10) : '0' + hiDigit;
  dest[1] = (loDigit > 9) ? 'A' + (loDigit - 10) : '0' + loDigit;
  dest[2] = 0;
}

static uint_t
wod_dec_w_na(const uint8_t* args, char* const op1, char* const op2)
{
  throw DumpException(_EXTRA(0), "Not a valid opcode encountered!");
  return 0;
}

static uint_t
wod_dec_w_no_ops(const uint8_t* args, char* const op1, char* const op2)
{
  op1[0] = op2[0] = 0;
  return 0;
}

static uint_t
wod_dec_w_ldc(const uint8_t* args, char* const op1, char* const op2)
{

  /* If is printable ASCII, then print the character. */
  if ((args[0] >= 0x20) && (args[0] <= 0x7E)
      && args[1] == 0
      && args[2] == 0
      && args[3] == 0)
  {
    op1[0] = '\'';
    op1[1] = args[0];
    op1[2] = '\'';
    op1[3] = 0;
  }
  else
  {
    int8_str_conv(op1, args[3]);
    int8_str_conv(op1 + 2, args[2]);
    int8_str_conv(op1 + 4, args[1]);
    int8_str_conv(op1 + 6, args[0]);
    strcat(op1, "h");
  }

  op2[0] = 0;

  return 4;
}

static uint_t
wod_dec_w_ldi8(const uint8_t* args, char* const op1, char* const op2)
{
  int8_str_conv(op1, args[0]);

  strcat(op1, "h");
  op2[0] = 0;

  return 1;
}

static uint_t
wod_dec_w_ldi16(const uint8_t* args, char* const op1, char* const op2)
{
  int8_str_conv(op1, args[1]);
  int8_str_conv(op1 + 2, args[0]);

  strcat(op1, "h");
  op2[0] = 0;

  return 2;
}

static uint_t
wod_dec_w_ldi32(const uint8_t* args, char* const op1,  char* const op2)
{
  int8_str_conv(op1, args[3]);
  int8_str_conv(op1 + 2, args[2]);
  int8_str_conv(op1 + 4, args[1]);
  int8_str_conv(op1 + 6, args[0]);

  strcat(op1, "h");
  op2[0] = 0;

  return 4;
}

static uint_t
wod_dec_w_ldi64(const uint8_t* pinArgs, char* const op1, char* const op2)
{
  int8_str_conv(op1, pinArgs[7]);
  int8_str_conv(op1 + 2, pinArgs[6]);
  int8_str_conv(op1 + 4, pinArgs[5]);
  int8_str_conv(op1 + 6, pinArgs[4]);
  int8_str_conv(op1 + 8, pinArgs[3]);
  int8_str_conv(op1 + 10, pinArgs[2]);
  int8_str_conv(op1 + 12, pinArgs[1]);
  int8_str_conv(op1 + 14, pinArgs[0]);

  strcat(op1, "h");
  op2[0] = 0;

  return 8;
}

static uint_t
wod_dec_w_ldd(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const uint8_t day   = args[0];
  const uint8_t month = args[1];
  const int16_t year  = _SC(int16_t, load_le_int16(args + 2));

  assert(day >= 1 && day <= 31);
  assert(month >= 1 && month <= 12);

  strcpy(op1, into_ascii(_SC(int64_t, year), t_str, FALSE));
  strcat(op1, "/");
  strcat(op1, into_ascii(month, t_str));
  strcat(op1, "/");
  strcat(op1, into_ascii(day, t_str));

  op2[0] = 0;

  return 4;
}

static uint_t
wod_dec_w_lddt(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const uint8_t sec   = args[0];
  const uint8_t min   = args[1];
  const uint8_t hour  = args[2];
  const uint8_t day   = args[3];
  const uint8_t month = args[4];
  const int16_t year  = _SC(int16_t, load_le_int16(args + 5));

  assert(sec < 60);
  assert(min < 60);
  assert(hour < 24);
  assert(day >= 1 && day <= 31);
  assert(month >= 1 && month <= 12);

  strcpy(op1, into_ascii(_SC(int16_t, year), t_str, FALSE));
  strcat(op1, "/");
  strcat(op1, into_ascii(month, t_str));
  strcat(op1, "/");
  strcat(op1, into_ascii(day, t_str));
  strcat(op1, " ");
  strcat(op1, into_ascii(hour, t_str));
  strcat(op1, ":");
  strcat(op1, into_ascii(min, t_str));
  strcat(op1, ":");
  strcat(op1, into_ascii(sec, t_str));

  op2[0] = 0;

  return 7;
}

static uint_t
wod_dec_w_ldht(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const uint32_t u_sec = load_le_int32(args);
  const uint8_t sec    = args[4];
  const uint8_t min    = args[5];
  const uint8_t hour   = args[6];
  const uint8_t day    = args[7];
  const uint8_t month  = args[8];
  const int16_t year   = _SC(int16_t, load_le_int16(args + 9));

  assert(sec < 60);
  assert(min < 60);
  assert(hour < 24);
  assert(day >= 1 && day <= 31);
  assert(month >= 1 && month <= 12);

  strcpy(op1, into_ascii(_SC(int64_t, year), t_str, FALSE));
  strcat(op1, "/");
  strcat(op1, into_ascii(month, t_str));
  strcat(op1, "/");
  strcat(op1, into_ascii(day, t_str));
  strcat(op1, " ");
  strcat(op1, into_ascii(hour, t_str));
  strcat(op1, ":");
  strcat(op1, into_ascii(min, t_str));
  strcat(op1, ":");
  strcat(op1, into_ascii(sec, t_str));

  strcat(op1, ".");
  strcat(op1, into_ascii(u_sec, t_str));

  op2[0] = 0;

  return 11;
}

static uint_t
wod_dec_w_ldrr(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_RREAL_LENGTH];

  int64_t int_part = load_le_int64(args);
  int64_t frac_part = load_le_int64(args + sizeof(uint64_t));

  if ((int_part < 0) || (frac_part < 0))
  {
    int_part = -int_part;
    frac_part = -frac_part;
    strcpy(op1, "-");
  }
  else
    strcpy(op1, "");

  strcat(op1, into_ascii(int_part, t_str, FALSE));
  strcat(op1, ".");
  strcat(op1, fracint_to_ascii(frac_part, t_str, W_LDRR_PRECISSION));

  return(sizeof(int64_t) + sizeof(uint64_t));
}

static const FDECODE_OPCODE wod_dec_w_ldt = wod_dec_w_ldi32;
static const FDECODE_OPCODE wod_dec_w_ldbt = wod_dec_w_no_ops;
static const FDECODE_OPCODE wod_dec_w_ldbf = wod_dec_w_no_ops;

static uint_t
wod_dec_w_ldlo8(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const uint8_t value = args[0];

  strcpy(op1, into_ascii(value, t_str));
  op2[0] = 0;

  return sizeof(uint8_t);
}

static uint_t
wod_dec_w_ldlo16(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const uint16_t value = load_le_int16(args);

  strcpy(op1, into_ascii(value, t_str));
  op2[0] = 0;

  return sizeof(uint16_t);
}

static uint_t
wod_dec_w_ldlo32(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const uint32_t value = load_le_int32(args);

  strcpy(op1, into_ascii(value, t_str));
  op2[0] = 0;

  return sizeof(uint32_t);
}

const FDECODE_OPCODE wod_dec_w_ldgb8  = wod_dec_w_ldlo8;
const FDECODE_OPCODE wod_dec_w_ldgb16 = wod_dec_w_ldlo16;
const FDECODE_OPCODE wod_dec_w_ldgb32 = wod_dec_w_ldlo32;

const FDECODE_OPCODE wod_dec_w_ldnull = wod_dec_w_ldi8;
const FDECODE_OPCODE wod_dec_w_cts    = wod_dec_w_ldi8;
const FDECODE_OPCODE wod_dec_w_stb    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stc    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_std    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stdt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stht   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sti8   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sti16  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sti32  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sti64  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_str    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_strr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stt    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stui8  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stui16 = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stui32 = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stui64 = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stta   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stf    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sta    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_stud   = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_inull  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_nnull  = wod_dec_w_no_ops;

static uint_t
wod_dec_w_call(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const uint32_t value = load_le_int32(args);

  strcpy(op1, into_ascii(value, t_str));
  op2[0] = 0;

  return sizeof(uint32_t);
}

const FDECODE_OPCODE wod_dec_w_ret    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_add    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_addrr  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_addt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_and    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_andb   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_div    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_divu   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_divr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_divrr  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eq     = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eqb    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eqc    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eqd    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eqdt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eqht   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eqrr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_eqt    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ge     = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_geu    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gec    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ged    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gedt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_geht   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gerr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gt     = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gtu    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gtc    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gtd    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gtdt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gtht   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_gtrr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_le     = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_leu    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_lec    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_led    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ledt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_leht   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_lerr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_lt     = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ltu    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ltc    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ltd    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ltdt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ltht   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ltrr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_mod    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_modu   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_mul    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_mulu   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_mulrr  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ne     = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_neb    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_nec    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ned    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_nedt   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_neht   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_nerr   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_net    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_not    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_notb   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_or     = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_orb    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sub    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_subrr  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_xor    = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_xorb   = wod_dec_w_no_ops;

static uint_t
wod_dec_w_jf(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_INT64_LENGTH];

  const int32_t temp  = _SC(int32_t, load_le_int32(args));
  const int64_t value = temp;

  strcpy(op1, into_ascii(value, t_str, FALSE));
  op2[0] = 0;

  return sizeof(uint32_t);
}

const FDECODE_OPCODE wod_dec_w_jfc = wod_dec_w_jf;
const FDECODE_OPCODE wod_dec_w_jt  = wod_dec_w_jf;
const FDECODE_OPCODE wod_dec_w_jtc = wod_dec_w_jf;
const FDECODE_OPCODE wod_dec_w_jmp = wod_dec_w_jf;

const FDECODE_OPCODE wod_dec_w_indt  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_inda  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_indf  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_indta = wod_dec_w_ldlo32;
const FDECODE_OPCODE wod_dec_w_self  = wod_dec_w_ldlo32;

static uint_t
wod_dec_w_bsync(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_OP_STRING];

  const uint8_t value = args[0];

  strcpy(op1, into_ascii(value, t_str));
  op2[0] = 0;

  return sizeof(uint8_t);

}

const FDECODE_OPCODE wod_dec_w_esync = wod_dec_w_bsync;

const FDECODE_OPCODE wod_dec_w_sadd   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_saddrr = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_saddc  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_saddt  = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_ssub   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_ssubrr = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_smul   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_smulu  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_smulrr = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_sdiv   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sdivu  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sdivrr = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_smod   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_smodu  = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_sand  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sandb = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_sxor  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sxorb = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_sor  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_sorb = wod_dec_w_no_ops;

const FDECODE_OPCODE wod_dec_w_itbf  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_itbl  = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_itn   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_itp   = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_itoff = wod_dec_w_no_ops;
const FDECODE_OPCODE wod_dec_w_fid   = wod_dec_w_no_ops;

static uint_t
wod_dec_w_carr(const uint8_t* args, char* const op1, char* const op2)
{
  char t_str[MAX_OP_STRING];

  strcpy(op1, into_ascii(args[0], t_str));
  strcpy(op2, into_ascii(load_le_int16(args + 1), t_str));

  return sizeof(uint8_t) + sizeof (uint16_t);
}

const FDECODE_OPCODE wod_dec_w_ajoin  = wod_dec_w_ldi8;
const FDECODE_OPCODE wod_dec_w_afout  = wod_dec_w_ldi8;
const FDECODE_OPCODE wod_dec_w_afin   = wod_dec_w_ldi8;


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
  wod_dec_w_ldrr,
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
  wod_dec_w_sti8,
  wod_dec_w_sti16,
  wod_dec_w_sti32,
  wod_dec_w_sti64,
  wod_dec_w_str,
  wod_dec_w_strr,
  wod_dec_w_stt,
  wod_dec_w_stui8,
  wod_dec_w_stui16,
  wod_dec_w_stui32,
  wod_dec_w_stui64,
  wod_dec_w_stta,
  wod_dec_w_stf,
  wod_dec_w_sta,
  wod_dec_w_stud,

  wod_dec_w_inull,
  wod_dec_w_nnull,

  wod_dec_w_call,
  wod_dec_w_ret,

  wod_dec_w_add,
  wod_dec_w_addrr,
  wod_dec_w_addt,

  wod_dec_w_and,
  wod_dec_w_andb,

  wod_dec_w_div,
  wod_dec_w_divu,
  wod_dec_w_divrr,

  wod_dec_w_eq,
  wod_dec_w_eqb,
  wod_dec_w_eqc,
  wod_dec_w_eqd,
  wod_dec_w_eqdt,
  wod_dec_w_eqht,
  wod_dec_w_eqrr,
  wod_dec_w_eqt,

  wod_dec_w_ge,
  wod_dec_w_geu,
  wod_dec_w_gec,
  wod_dec_w_ged,
  wod_dec_w_gedt,
  wod_dec_w_geht,
  wod_dec_w_gerr,

  wod_dec_w_gt,
  wod_dec_w_gtu,
  wod_dec_w_gtc,
  wod_dec_w_gtd,
  wod_dec_w_gtdt,
  wod_dec_w_gtht,
  wod_dec_w_gtrr,

  wod_dec_w_le,
  wod_dec_w_leu,
  wod_dec_w_lec,
  wod_dec_w_led,
  wod_dec_w_ledt,
  wod_dec_w_leht,
  wod_dec_w_lerr,

  wod_dec_w_lt,
  wod_dec_w_ltu,
  wod_dec_w_ltc,
  wod_dec_w_ltd,
  wod_dec_w_ltdt,
  wod_dec_w_ltht,
  wod_dec_w_ltrr,

  wod_dec_w_mod,
  wod_dec_w_modu,

  wod_dec_w_mul,
  wod_dec_w_mulu,
  wod_dec_w_mulrr,

  wod_dec_w_ne,
  wod_dec_w_neb,
  wod_dec_w_nec,
  wod_dec_w_ned,
  wod_dec_w_nedt,
  wod_dec_w_neht,
  wod_dec_w_nerr,
  wod_dec_w_net,

  wod_dec_w_not,
  wod_dec_w_notb,

  wod_dec_w_or,
  wod_dec_w_orb,

  wod_dec_w_sub,
  wod_dec_w_subrr,

  wod_dec_w_xor,
  wod_dec_w_xorb,

  wod_dec_w_jf,
  wod_dec_w_jfc,
  wod_dec_w_jt,
  wod_dec_w_jtc,
  wod_dec_w_jmp,

  wod_dec_w_indt,
  wod_dec_w_inda,
  wod_dec_w_indf,
  wod_dec_w_indta,
  wod_dec_w_self,

  wod_dec_w_bsync,
  wod_dec_w_esync,

  wod_dec_w_sadd,
  wod_dec_w_saddrr,
  wod_dec_w_saddc,
  wod_dec_w_saddt,

  wod_dec_w_ssub,
  wod_dec_w_ssubrr,

  wod_dec_w_smul,
  wod_dec_w_smulu,
  wod_dec_w_smulrr,

  wod_dec_w_sdiv,
  wod_dec_w_sdivu,
  wod_dec_w_sdivrr,

  wod_dec_w_smod,
  wod_dec_w_smodu,

  wod_dec_w_sand,
  wod_dec_w_sandb,

  wod_dec_w_sxor,
  wod_dec_w_sxorb,

  wod_dec_w_sor,
  wod_dec_w_sorb,

  wod_dec_w_itbf,
  wod_dec_w_itbl,
  wod_dec_w_itn,
  wod_dec_w_itp,
  wod_dec_w_itoff,
  wod_dec_w_fid,
  wod_dec_w_carr,

  wod_dec_w_ajoin,
  wod_dec_w_afout,
  wod_dec_w_afin
};

const char *wod_str_table[] = {
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
  "ldrr",
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
  "sti8",
  "sti16",
  "sti32",
  "sti64",
  "str",
  "strr",
  "stt",
  "stui8",
  "stui16",
  "stui32",
  "stui64",
  "stta",
  "stf",
  "sta",
  "stud",

  "inull",
  "nnull",

  "call",
  "ret",

  "add",
  "addrr",
  "addt",

  "and",
  "andb",

  "div",
  "divu",
  "divrr",

  "eq",
  "eqb",
  "eqc",
  "eqd",
  "eqdt",
  "eqht",
  "eqrr",
  "eqt",

  "ge",
  "geu",
  "gec",
  "ged",
  "gedt",
  "geht",
  "gerr",

  "gt",
  "gtu",
  "gtc",
  "gtd",
  "gtdt",
  "gtht",
  "gtrr",

  "le",
  "leu",
  "lec",
  "led",
  "ledt",
  "leht",
  "lerr",

  "lt",
  "ltu",
  "ltc",
  "ltd",
  "ltdt",
  "ltht",
  "ltrr",

  "mod",
  "modu",

  "mul",
  "mulu",
  "mulrr",

  "ne",
  "neb",
  "nec",
  "ned",
  "nedt",
  "neht",
  "nerr",
  "net",

  "not",
  "notb",

  "or",
  "orb",

  "sub",
  "subrr",

  "xor",
  "xorb",

  "jf",
  "jfc",
  "jt",
  "jtc",
  "jmp",

  "indt",
  "inda",
  "indf",
  "indta",
  "self",

  "bsync",
  "esync",

  "sadd",
  "saddrr",
  "saddc",
  "saddt",

  "ssub",
  "ssubrr",

  "smul",
  "smulu",
  "smulrr",

  "sdiv",
  "sdivu",
  "sdivrr",

  "smod",
  "smodu",

  "sand",
  "sandb",

  "sxor",
  "sxorb",

  "sor",
  "sorb",

  "itf",
  "itl",
  "itn",
  "itp",
  "itoff",

  "fid",

  "carr",

  "adda",
  "suba",
  "moda",

  "ajoin",
  "afout",
  "afin"

};

} //namespace wod
} //namespace whais

