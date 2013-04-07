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

#ifndef WHISPER_TIME_H_
#define WHISPER_TIME_H_

typedef struct
{
  D_INT16  year;
  D_UINT8  month;
  D_UINT8  day;
  D_UINT8  hour;
  D_UINT8  min;
  D_UINT8  sec;
}WTime;


typedef D_UINT64 WTICKS;

#ifdef __cplusplus
extern "C"
{
#endif

WTime
wh_get_currtime ();

WTICKS
wh_msec_ticks ();

#ifdef __cplusplus
}
#endif

#endif /* WHISPER_TIME_H_ */
