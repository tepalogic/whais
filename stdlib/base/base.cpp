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

#include <assert.h>

#include "whisper.h"

#include "utils/wthread.h"
#include "stdlib/interface.h"

#include "base_types.h"
#include "base_generics.h"
#include "base_constants.h"
#include "base_dates.h"
#include "base_text.h"
#include "base_arrays.h"
#include "base_fields.h"
#include "base_tables.h"



using namespace whisper;



static const WLIB_PROC_DESCRIPTION* sgRegisteredProcs[] = {
        //Basic procedures returning constants
                                                    &gProcPI,
                                                    &gProcSQRT2,
                                                    &gProcNE,

                                                    &gProcMinB,
                                                    &gProcMaxB,

                                                    &gProcMinC,
                                                    &gProcMaxC,

                                                    &gProcMinD,
                                                    &gProcMaxD,

                                                    &gProcMinDT,
                                                    &gProcMaxDT,

                                                    &gProcMinHT,
                                                    &gProcMaxHT,

                                                    &gProcMinI8,
                                                    &gProcMaxI8,

                                                    &gProcMinI16,
                                                    &gProcMaxI16,

                                                    &gProcMinI32,
                                                    &gProcMaxI32,

                                                    &gProcMinI64,
                                                    &gProcMaxI64,

                                                    &gProcMinU8,
                                                    &gProcMaxU8,

                                                    &gProcMinU16,
                                                    &gProcMaxU16,

                                                    &gProcMinU32,
                                                    &gProcMaxU32,

                                                    &gProcMinU64,
                                                    &gProcMaxU64,

                                                    &gProcMinR,
                                                    &gProcMaxR,

                                                    &gProcMinRR,
                                                    &gProcMaxRR,
          //Base generics procedures
                                                    &gProcPrevB,
                                                    &gProcNextB,

                                                    &gProcPrevC,
                                                    &gProcNextC,

                                                    &gProcPrevD,
                                                    &gProcNextD,

                                                    &gProcPrevDT,
                                                    &gProcNextDT,

                                                    &gProcPrevHT,
                                                    &gProcNextHT,

                                                    &gProcPrevI8,
                                                    &gProcNextI8,

                                                    &gProcPrevI16,
                                                    &gProcNextI16,

                                                    &gProcPrevI32,
                                                    &gProcNextI32,

                                                    &gProcPrevI64,
                                                    &gProcNextI64,

                                                    &gProcPrevU8,
                                                    &gProcNextU8,

                                                    &gProcPrevU16,
                                                    &gProcNextU16,

                                                    &gProcPrevU32,
                                                    &gProcNextU32,

                                                    &gProcPrevU64,
                                                    &gProcNextU64,

                                                    &gProcPrevR,
                                                    &gProcNextR,

                                                    &gProcPrevRR,
                                                    &gProcNextRR,

                                                    &gProcSleep,
                                                    &gProcLog,

                                                    &gProcCeil,
                                                    &gProcRound,
                                                    &gProcFloor,
                                                    &gProcAbs,

                        /* Date & time procedures. */

                                                    &gProcTicks,
                                                    &gProcNow,
                                                    &gProcTimeYear,
                                                    &gProcTimeMonth,
                                                    &gProcTimeDay,
                                                    &gProcTimeHours,
                                                    &gProcTimeMinutess,
                                                    &gProcTimeSeconds,
                                                    &gProcTimeMicroseconds,
                                                    &gProcDateDaysDiff,
                                                    &gProcDateDaysDelta,
                                                    &gProcDateSecondsDiff,
                                                    &gProcDateSecondsDelta,
                                                    &gProcDateMicrosecondsDiff,
                                                    &gProcDateMicrosecondsDelta,
                                                    &gProcNativeYear,
                                                    &gProcDateWeek,
                                                    &gProcLastDateOfWeek,
                                                    &gProcDayOfWeek,
                         /* Text procedures */
                                                    &gIsUpper,
                                                    &gIsLower,
                                                    &gIsDigit,
                                                    &gIsAlpha,
                                                    &gIsSpace,
                                                    &gIsPunct,
                                                    &gIsPrint,
                                                    &gUnicodeCP,
                                                    &gUpperChar,
                                                    &gLowerChar,
                                                    &gUpperText,
                                                    &gLowerText,
                                                    &gTextToUtf8,
                                                    &gTextFromUtf8,
                                                    &gTextToUtf16,
                                                    &gTextFromUtf16,
                                                    &gTextToCharArray,
                                                    &gTextFromCharArray,
                                                    &gTextCharsCount,
                                                    &gTextHash,
                                                    &gCharFind,
                                                    &gTextFind,
                                                    &gTextCompare,
                          /* Array procedures */
                                                    &gProcArrayCount,
                                                    &gProcArraySort,
                                                    &gProcArrayBinSearch,
                                                    &gProcArraySearch,
                                                    &gProcArrayMin,
                                                    &gProcArrayMax,
                                                    &gProcArrayAverage,
                                                    &gProcArrayIntersect,
                                                    &gProcArrayUnion,
                                                    &gProcArrayDiff,
                                                    &gProcArrayPushBack,
                                                    &gProcArrayTruncate,
                                                    &gProcArrayHash,
                          /* Field procedures */
                                                    &gProcFieldTable,
                                                    &gProcIsFielsIndexed,
                                                    &gProcFieldName,
                                                    &gProcFieldIndex,
                                                    &gProcFindValueRange,
                                                    &gProcFieldMinimum,
                                                    &gProcFieldMaximum,
                                                    &gProcFieldAverage,
                          /* Table procedures */
                                                    &gProcTableIsPersistent,
                                                    &gProcTableFieldsCount,
                                                    &gProcTableFieldByIndex,
                                                    &gProcTableFieldByName,
                                                    &gProcTableRowsCount,
                                                    &gProcTableAddRow,
                                                    &gProcTableFindRemovedRow,
                                                    &gProcTableRemoveRow,
                                                    &gProcTableExchangeRows
                                                          };


static const WLIB_DESCRIPTION sgLibraryDescription = {
    sizeof (sgRegisteredProcs) / sizeof (sgRegisteredProcs[0]),
    sgRegisteredProcs
                                                     };

static int      sgRefsCount = 0;
static Lock     sgShlLocker;
static bool_t   sgInited;



extern "C" {



SHL_EXPORT_SYMBOL WLIB_STATUS
wlib_start ()
{
  LockRAII syncHolder (sgShlLocker);

  assert (sgRefsCount >= 0);

  if (sgRefsCount == 0)
    {
      WLIB_STATUS status;

      if (((status = base_types_init ()) != WOP_OK)
          || ((status = base_generics_init ()) != WOP_OK)
          || ((status = base_constants_init ()) != WOP_OK)
          || ((status = base_dates_init ()) != WOP_OK)
          || ((status = base_text_init ()) != WOP_OK)
          || ((status = base_arrays_init ()) != WOP_OK)
          || ((status = base_fields_init ()) != WOP_OK)
          || ((status = base_tables_init ()) != WOP_OK))
        {
          return status;
        }
    }

  sgInited = true;
  sgRefsCount++;

  return WOP_OK;
}


SHL_EXPORT_SYMBOL WLIB_STATUS
wlib_end ()
{
  LockRAII syncHolder (sgShlLocker);

  assert ((! sgInited) || (sgRefsCount > 0));

  if (sgInited)
    --sgRefsCount;

  sgInited = false;

  return WOP_OK;
}


SHL_EXPORT_SYMBOL const WLIB_DESCRIPTION*
wlib_describe ()
{
  if (sgInited)
    return &sgLibraryDescription;

  return NULL;
}



} // extern "C"

#ifdef ENABLE_MEMORY_TRACE
uint32_t    WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule    = "wnl_base";
#endif

