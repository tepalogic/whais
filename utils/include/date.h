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

#ifndef DATE_H_
#define DATE_H_

#include "whais.h"



#ifdef __cplusplus
  #define  BOOL_R     bool
  #define  FALSE_R    false
  #define  TRUE_R     true
#else
  #define  BOOL_R     bool_t
  #define  FALSE_R    FALSE
  #define  TRUE_R     TRUE
#endif


#define MNTH_DAYS_A  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
#define MNTH_YDAYS_A { 0,  31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}

#define DAYS_PER_YEAR   365



static BOOL_R
is_leap_year (const int year)
{
  if ((year % 4) == 0)
    {
      if ((year % 100) == 0)
        {
          if ((year % 400) == 0)
            return TRUE_R;

          else
            return FALSE_R;
        }
      else
        return TRUE_R;
    }
  else
    return FALSE_R;
}

#undef BOOL_R

#endif /* DATE_H_ */
