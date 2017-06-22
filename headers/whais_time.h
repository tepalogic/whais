/******************************************************************************
WHAIS - An advanced database system
Copyright(C) 2008  Iulian Popa

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

#ifndef WHAIS_TIME_H_
#define WHAIS_TIME_H_

typedef struct
{
  int16_t  year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  min;
  uint8_t  sec;
  uint_t   usec;
}WTime;


typedef uint64_t WTICKS;

#ifdef __cplusplus
extern "C"
{
#endif

CUSTOM_SHL WTime 
wh_get_currtime();

CUSTOM_SHL WTICKS 
wh_msec_ticks();

#ifdef __cplusplus
}
#endif

#endif /* WHAIS_TIME_H_ */
