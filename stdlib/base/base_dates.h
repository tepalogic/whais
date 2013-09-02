/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whisper.
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
 *****************************************************************************/

#ifndef BASE_DATES_H_
#define BASE_DATES_H_

#include "whisper.h"

#include "stdlib/interface.h"

extern whisper::WLIB_PROC_DESCRIPTION       gProcTicks;
extern whisper::WLIB_PROC_DESCRIPTION       gProcNow;
extern whisper::WLIB_PROC_DESCRIPTION       gProcTimeYear;
extern whisper::WLIB_PROC_DESCRIPTION       gProcTimeMonth;
extern whisper::WLIB_PROC_DESCRIPTION       gProcTimeDay;
extern whisper::WLIB_PROC_DESCRIPTION       gProcTimeHours;
extern whisper::WLIB_PROC_DESCRIPTION       gProcTimeMinutess;
extern whisper::WLIB_PROC_DESCRIPTION       gProcTimeSeconds;
extern whisper::WLIB_PROC_DESCRIPTION       gProcTimeMicroseconds;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDateDaysDiff;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDateDaysDelta;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDateSecondsDiff;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDateSecondsDelta;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDateMicrosecondsDiff;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDateMicrosecondsDelta;
extern whisper::WLIB_PROC_DESCRIPTION       gProcNativeYear;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDateWeek;
extern whisper::WLIB_PROC_DESCRIPTION       gProcLastDateOfWeek;
extern whisper::WLIB_PROC_DESCRIPTION       gProcDayOfWeek;


whisper::WLIB_STATUS
base_dates_init ();

#endif //BASE_DATES_H_

