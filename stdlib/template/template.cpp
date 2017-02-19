/******************************************************************************
 WSTDLIB - Standard mathemetically library for Whais.
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
 *****************************************************************************/
/******************************************************************************
 Template file containing a skeleton to write a a natve Whais library.
 *****************************************************************************/

#include <assert.h>

#include "whais.h"

#include "utils/wthread.h"
#include "stdlib/interface.h"

#include "math_constants.h"



using namespace whais;



static const WLIB_PROC_DESCRIPTION sgRegisteredProcs[] = {
    /* TODO: Add the procedures descriptors here. */
                                                         };

static const WLIB_DESCRIPTION sgLibraryDescription = {
    sizeof( sgRegisteredProcs) / sizeof( sgRegisteredProcs[0]),
    sgRegisteredProcs
                                                     };

static int      sgRefsCount = 0;
static Lock     sgShlLocker;
static bool_t   sgInited;



extern "C" {



SHL_EXPORT_SYMBOL WLIB_STATUS
wlib_start()
{
  LockRAII<Lock> syncHolder( sgShlLocker);

  assert( sgRefsCount >= 0);

  if (sgRefsCount == 0)
    {
      WLIB_STATUS status = WOP_OK;

      /* TODO: Add library initilization code here. */

      return status;
    }

  sgInited = true;
  sgRefsCount++;

  return WOP_OK;
}


SHL_EXPORT_SYMBOL WLIB_STATUS
wlib_end()
{
  LockRAII<Lock> syncHolder( sgShlLocker);

  assert( (! sgInited) || (sgRefsCount > 0));

  if (sgInited && (--sgRefsCount == 0))
    {
      sgInited = false;
      /* TODO: Add your library clean up code here. */
    }

  return WOP_OK;
}


SHL_EXPORT_SYMBOL const WLIB_DESCRIPTION*
wlib_describe()
{
  if (sgInited)
    return &sgLibraryDescription;

  return NULL;
}



} // extern "C"

