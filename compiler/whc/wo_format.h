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

#ifndef WO_FORMAT_H_
#define WO_FORMAT_H_

#include "whisper.h"

const D_UINT8 WH_SIGNATURE[] = { 'W', 'O' };

const D_INT8 WH_FFVER_MAJ = 1;
const D_INT8 WH_FFVER_MIN = 0;

const D_UINT WHC_SIGNATURE_OFF         = 0;
const D_UINT WHC_FORMATMMAJ_OFF        = 2;
const D_UINT WHC_FORMATMIN_OFF         = 3;
const D_UINT WHC_LANGVER_MAJ_OFF       = 4;
const D_UINT WHC_LANGVER_MIN_OFF       = 5;
const D_UINT WHC_GLOBS_COUNT_OFF       = 8;
const D_UINT WHC_PROCS_COUNT_OFF       = 12;
const D_UINT WHC_TYPEINFO_START_OFF    = 16;
const D_UINT WHC_TYPEINFO_SIZE_OFF     = 20;
const D_UINT WHC_SYMTABLE_START_OFF    = 24;
const D_UINT WHC_SYMTABLE_SIZE_OFF     = 28;
const D_UINT WHC_CONSTAREA_START_OFF   = 32;
const D_UINT WHC_CONSTAREA_SIZE_OFF    = 36;

const D_UINT   WHC_TABLE_SIZE          = 40;
const D_UINT32 EXTERN_MASK             = 0x80000000;

const D_UINT WHC_GLB_ENTRY_TYPE_OFF    = 0;
const D_UINT WHC_GLB_ENTRY_NAME_OFF    = 4;
const D_UINT WHC_GLOBAL_ENTRY_SIZE     = 8;

const D_UINT WHC_PROC_ENTRY_NAME_OFF   = 0;
const D_UINT WHC_PROC_ENTRY_BODY_OFF   = 4;
const D_UINT WHC_PROC_ENTRY_TYPE_OFF   = 8;
const D_UINT WHC_PROC_ENTRY_NLOCAL_OFF = 12;
const D_UINT WHC_PROC_ENTRY_NPARMS_OFF = 14;
const D_UINT WHC_PROC_ENTRY_CODE_SIZE  = 16;
const D_UINT WHC_PROC_ENTRY_SIZE       = 20;

const D_UINT WHC_PROC_BODY_LOCAL_ENTRY_SIZE = sizeof (D_UINT32);
const D_UINT WHC_PROC_BODY_SYNCS_ENTRY_SYZE = sizeof (D_UINT8);

#endif /* WO_FORMAT_H_ */
