/******************************************************************************
 WSTDLIB - Standard native libraries for a Whisper interpreter.
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

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "whisper.h"
#include "interpreter/operands.h"

namespace whisper {

// A type to hold the result of various library related calls.
typedef int32_t             WLIB_STATUS;


//Siganture of the function that actually implements the required
//behavior. Its paramters are on the top of the stack, a DBS handler is
//provided for extra fucntionality too. It the responsability of the function
//to clear the stack of its arguments and put the result on top of it.
typedef WLIB_STATUS (*WLIB_PROCEDURE) (SessionStack&, IDBSHandler&);

typedef const uint8_t* WLIB_PARAM_TYPE;

typedef struct {
    const char*             name;
    WLIB_PROCEDURE          code;
    uint32_t                localsCount;
    WLIB_PARAM_TYPE*        localsTypes;
} WLIB_PROC_DESCRIPTION;

//Describes the content of the library.
typedef struct {
    uint32_t                      procsCount;
    const WLIB_PROC_DESCRIPTION*  procsDescriptions;
} WLIB_DESCRIPTION;


// The signature of the function that should be called after the lib is loaded
// to allow the library initialisation.
typedef WLIB_STATUS (*WLIB_START_LIB_FUNC) ();

// The signature of the function that should be called before the library is
// unloaded to allow any clean up work to take place before clearing.
typedef WLIB_STATUS (*WLIB_END_LIB_FUNC) ();

// The signature of the function called to retreive the functions published
// by this library.
typedef const WLIB_DESCRIPTION* (*WLIB_DESC_LIB_FUNC) ();

// Constant string to specify the exported symbols.
static const char WSTDLIB_START_FUNC[]   = "wlib_start";
static const char WSTDLIB_END_FUNC[]     = "wlib_end";
static const char WSTDLIB_DESC_FUNC[]    = "wlib_describe";


} //namespace whisper

#endif /* INTERFACE_H_ */

