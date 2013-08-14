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

#include "stdlib/interface.h"
#include "utils/wtypes.h"
#include "interpreter/interpreter.h"

#define MATH_SKIP_EXTERNS_DECLS
#include "math_constants.h"



using namespace whisper;

uint8_t gRichRealType[sizeof (TypeSpec)];

WLIB_PROC_DESCRIPTION     gProcPI;
WLIB_PROC_DESCRIPTION     gProcEM;
WLIB_PROC_DESCRIPTION     gProcGR;
WLIB_PROC_DESCRIPTION     gProcSQRT2;
WLIB_PROC_DESCRIPTION     gProcNE;



static WLIB_STATUS
get_PI (SessionStack& stack, IDBSHandler&)
{
  stack.Push (DRichReal (RICHREAL_T (3, 14159265358979ull, DBS_RICHREAL_PREC)));

  return WOP_OK;
}


static WLIB_STATUS
get_EM (SessionStack& stack, IDBSHandler&)
{

  stack.Push (DRichReal (RICHREAL_T (0, 57721566490153ull, DBS_RICHREAL_PREC)));
  return WOP_OK;
}


static WLIB_STATUS
get_GR (SessionStack& stack, IDBSHandler&)
{
  stack.Push (DRichReal (RICHREAL_T (1, 61803398874989ull, DBS_RICHREAL_PREC)));

  return WOP_OK;
}


static WLIB_STATUS
get_SQRT2 (SessionStack& stack, IDBSHandler&)
{
  stack.Push (DRichReal (RICHREAL_T (2, 41421356237309ull, DBS_RICHREAL_PREC)));

  return WOP_OK;
}


static WLIB_STATUS
get_NE (SessionStack& stack, IDBSHandler&)
{
  stack.Push (DRichReal (RICHREAL_T (2, 71828182845904ull, DBS_RICHREAL_PREC)));

  return WOP_OK;
}


WLIB_STATUS
math_constants_init ()
{
  static const uint8_t* procParamters[] = { gRichRealType };

  if (wh_define_basic_type (T_RICHREAL, _RC (TypeSpec*, gRichRealType)) <= 0)
    return WOP_UNKNOW;

  gProcPI.name             = "PI";
  gProcPI.localsCount      = 1;
  gProcPI.localsTypes      = procParamters;
  gProcPI.code             = get_PI;

  gProcEM.name             = "GAMMA";
  gProcEM.localsCount      = 1;
  gProcEM.localsTypes      = procParamters;
  gProcEM.code             = get_EM;

  gProcGR.name             = "PHY";
  gProcGR.localsCount      = 1;
  gProcGR.localsTypes      = procParamters;
  gProcGR.code             = get_GR;

  gProcSQRT2.name          = "SQRT2";
  gProcSQRT2.localsCount   = 1;
  gProcSQRT2.localsTypes   = procParamters;
  gProcSQRT2.code          = get_SQRT2;

  gProcNE.name             = "E";
  gProcNE.localsCount      = 1;
  gProcNE.localsTypes      = procParamters;
  gProcNE.code             = get_NE;

  return WOP_OK;
}


