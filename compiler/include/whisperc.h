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

#ifndef WHISPERC_H
#define WHISPERC_H

#include <stdarg.h>

#include "whisper.h"

#include "dbs/dbs_types.h"
#include "utils/wtypes.h"


typedef const void*     WH_COMPILED_UNIT;
typedef const void*     WH_COMPILED_UNIT_PROC;
typedef const void*     WH_MESSENGER_CTXT;


typedef void  (*WH_MESSENGER) (WH_MESSENGER_CTXT    context,
                               uint_t               buffPos,
                               uint_t               msgId,
                               uint_t               msgType,
                               const char*          message,
                               va_list              args);



#define WHC_IGNORE_BUFFER_POS        (uint_t)(-1)



typedef struct
{
  const char*           name;         /* Name of the global variable. */
  uint_t                nameLength;   /* Length of the name. Name is not
                                         null terminated. */
  const uint8_t*        type;         /* Compiled binary type description. */
  bool_t                defined;      /* Shortcut to specify if the compilation
                                         unit holds the definition of this
                                         global values, or it's a external
                                         declaration. */
} WCompilerGlobalDesc;


typedef struct
{
  const char*      name;        /* Name of the procedure. */
  uint_t           nameLength;  /* The name is not null terminated. */
  uint_t           paramsCount; /* Parameters count of this procedure. */
  uint_t           localsCount; /* Local variables used by this procedure
                                   (including the parameters variables). */
  uint_t           syncsCount;  /* Count of sync statements defined in
                                   this procedure. */
  uint_t           codeSize;    /* Compiled binary code size. */
  const uint8_t*   code;        /* Compiled binary code for this procedure. */
  bool_t           defined;     /* Shortcut flag to specify if this procedure
                                   is defined in this compilation unit, or it's
                                   an external declaration. */
} WCompilerProcedureDesc;



#ifdef __cplusplus
extern "C"
{
#endif

/* Load a buffer containing a WHISPER program. The supplied messenger is used
   to report compiler related information (e.g. errors, warnings, etc.). */
WH_COMPILED_UNIT
wh_compiler_load (const char*          sourceCode,
                  unsigned             sourceSize,
                  WH_MESSENGER         messenger,
                  WH_MESSENGER_CTXT    messengerContext);

/* Free the resources associated with a compiled unit. */
void
wh_compiler_discard (WH_COMPILED_UNIT hnd);

/* Get the globals count declared in a compiled unit. */
uint_t
wh_unit_globals_count (WH_COMPILED_UNIT hnd);

/* Get the description of global variable from a compiled unit. */
uint_t
wh_unit_global (WH_COMPILED_UNIT            hnd,
                const uint_t                id,
                WCompilerGlobalDesc* const  outDesription);

/* Get the procedures count declared in a compiled unit. */
uint_t
wh_unit_procedures_count (WH_COMPILED_UNIT hnd);

/* Get the description of a procedure from a compiled unit. */
uint_t
wh_unit_procedure (WH_COMPILED_UNIT              hnd,
                   const uint_t                  id,
                   WCompilerProcedureDesc* const outDesription);

/* Get a pointer to to the compiled binary type descriptors. */
uint_t
wh_unit_type_descriptors (WH_COMPILED_UNIT      hnd,
                         const uint8_t**        ppTypePool);

/* Get a pinter to the compiled binary constants area. */
uint_t
wh_unit_constants (WH_COMPILED_UNIT           hnd,
                   const uint8_t**            ppConstPool);

/* Get a procedure handler to get detailed information about
   it's components. */
WH_COMPILED_UNIT_PROC
wh_unit_procedure_get (WH_COMPILED_UNIT hnd, const uint_t id);

/* Release the resources associated a procedure handler. */
void
wh_unit_procedure_release (WH_COMPILED_UNIT            hnd,
                           WH_COMPILED_UNIT_PROC       proc);

/* Get the compiled binary type representation of the procedure's
   return type. */
const uint8_t*
wh_procedure_return_type (WH_COMPILED_UNIT           hnd,
                          WH_COMPILED_UNIT_PROC      proc);

/* Get the compiled binary type representation of a procedure's
   local variable */
const uint8_t*
wh_procedure_local_type (WH_COMPILED_UNIT           hnd,
                         WH_COMPILED_UNIT_PROC      proc,
                         const uint_t               id);

COMPILER_SHL void
wh_compiler_libver (uint_t* const   outMajor,
                    uint_t* const   outMinor);

/* Get the version of the language specification this library uses. */
COMPILER_SHL void
wh_compiler_language_ver (uint_t* const   outMajor,
                          uint_t* const   outMinor);

#ifdef __cplusplus
}       /* extern "C" */
#endif

#endif  /* WHISPERC_H */

