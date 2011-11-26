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
#include <assert.h>
#include <string.h>

#include "../include/whisperc/compiledunit.h"

#include "../../compiler/whc/wo_format.h"
#include "../include/le_converter.h"

/////////////******WBufferCompiledUnit********//////////////////////////////

WBufferCompiledUnit::WBufferCompiledUnit (const D_UINT8 * buffer,
    D_UINT buffer_size,
    WHC_MESSENGER callback,
    WHC_MESSENGER_ARG context)
:mHandler (NULL)
{
  mHandler = whc_hnd_create (_RC (const D_CHAR *, buffer),
      buffer_size, callback, context);
  if (mHandler == NULL)
    throw WCompiledUnitException ("Failed to compile the buffer.", _EXTRA(0));
}

WBufferCompiledUnit::~WBufferCompiledUnit ()
{
  if (mHandler != NULL)
    whc_hnd_destroy (mHandler);
}

D_UINT WBufferCompiledUnit::GetTypeInformationSize ()
{
  const D_UINT8 *dummy;
  const D_UINT result = whc_get_typedec_pool (mHandler, &dummy);

  return result;
}

const D_UINT8 *
WBufferCompiledUnit::RetriveTypeInformation ()
{
  const D_UINT8 *temp;
  whc_get_typedec_pool (mHandler, &temp);
  return temp;
}

D_UINT WBufferCompiledUnit::GetConsAreaSize ()
{
  const D_UINT8 *dummy;
  const D_UINT result = whc_get_const_area (mHandler, &dummy);

  return result;
}

const D_UINT8 *
WBufferCompiledUnit::RetrieveConstArea ()
{
  const D_UINT8 *temp;
  whc_get_const_area (mHandler, &temp);
  return temp;
}

D_UINT WBufferCompiledUnit::GetGlobalsCount ()
{
  return whc_get_globals_count (mHandler);
}

D_UINT WBufferCompiledUnit::GetProceduresCount ()
{
  return whc_get_procs_count (mHandler);
}

D_UINT WBufferCompiledUnit::GetGlobalNameLenght (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;
  if (!whc_get_global (mHandler, item, &globalDesc))
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));
  return globalDesc.name_len;
}

const D_CHAR *
WBufferCompiledUnit::RetriveGlobalName (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;
  if (!whc_get_global (mHandler, item, &globalDesc))
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));
  return globalDesc.name;
}

D_UINT WBufferCompiledUnit::GetGlobalTypeIndex (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;
  if (!whc_get_global (mHandler, item, &globalDesc))
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));

  const D_UINT8 * temp;
  whc_get_typedec_pool (mHandler, &temp);

  assert (temp && (temp <= globalDesc.type));
  return globalDesc.type - temp;

}

D_BOOL WBufferCompiledUnit::IsGlobalExternal (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;
  if (!whc_get_global (mHandler, item, &globalDesc))
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));
  return (globalDesc.defined == FALSE);
}

D_UINT WBufferCompiledUnit::GetProcSyncStatementsCount (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (mHandler, item, &desc))
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));
  return desc.syncs_count;

}

D_UINT WBufferCompiledUnit::GetProcCodeAreaSize (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (mHandler, item, &desc))
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));
  return desc.instrs_size;
}

const D_UINT8 *
WBufferCompiledUnit::RetriveProcCodeArea (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (mHandler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.",
        _EXTRA(0));
  return desc.instrs;
}

D_UINT WBufferCompiledUnit::GetProcLocalsCount (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (mHandler, item, &desc))
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));
  return desc.locals_count;
}

D_UINT WBufferCompiledUnit::GetProcParametersCount (D_UINT item)
{
  WHC_PROC_DESC
  desc;

  if (!whc_get_proc (mHandler, item, &desc))
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));
  return desc.param_count;
}

D_UINT WBufferCompiledUnit::GetProcReturnTypeIndex (D_UINT proc_item)
{
  WHC_PROC_HANDLER
  hProc = whc_get_proc_hnd (mHandler, proc_item);

  if (hProc == NULL)
    throw
    WCompiledUnitException ("Could not create the  procedure handle.",
        _EXTRA(0));

  const D_UINT8 *
  temp;
  whc_get_typedec_pool (mHandler, &temp);

  assert (temp && (temp <= whc_get_proc_rettype (mHandler, hProc)));
  D_UINT
  result = whc_get_proc_rettype (mHandler, hProc) - temp;
  whc_release_proc_hnd (mHandler, hProc);

  return result;
}

D_UINT WBufferCompiledUnit::GetProcNameSize (D_UINT proc_item)
{
  WHC_PROC_DESC
  desc;

  if (!whc_get_proc (mHandler, proc_item, &desc))
    throw
    WCompiledUnitException ("Could not get the procedure's description.", _EXTRA(0));

  return desc.name_len;
}

const D_CHAR *
WBufferCompiledUnit::RetriveProcName (D_UINT proc_item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (mHandler, proc_item, &desc))
    throw WCompiledUnitException ("Could not get procedure's description.",
        _EXTRA(0));
  return desc.name;
}

D_UINT
WBufferCompiledUnit::GetProcLocalTypeIndex (D_UINT item_proc,
    D_UINT item_local)
{
  const D_UINT8 *local_type;
  WHC_PROC_DESC p_desc;

  if (!whc_get_proc (mHandler, item_proc, &p_desc))
    throw WCompiledUnitException ("Could not get procedure description.",
        _EXTRA(0));

  WHC_PROC_HANDLER hProc = whc_get_proc_hnd (mHandler, item_proc);

  if (hProc == NULL)
    throw WCompiledUnitException ("Could not create the  procedure handle.",
        _EXTRA(0));

  local_type = whc_get_local_type (mHandler, hProc, item_local);

  whc_release_proc_hnd (mHandler, hProc);
  if (local_type == NULL)
    throw WCompiledUnitException ("Could not get local variable type.",
        _EXTRA(0));

  const D_UINT8 *temp;
  whc_get_typedec_pool (mHandler, &temp);

  assert (temp && (temp <= local_type));
  return local_type - temp;
}

D_BOOL WBufferCompiledUnit::IsProcExternal (D_UINT item_proc)
{
  return (GetProcCodeAreaSize (item_proc) == 0);
}

/////////////******WFileCompiledUnit********//////////////////////////////

WFileCompiledUnit::WFileCompiledUnit (const D_CHAR * file_name)
:mFile (file_name, WHC_FILEREAD),
 mGlobalsCount (0),
 mProcsCount (0),
 mTtySize (0),
 mSymbolsSize (0),
 mTypeInfo (NULL),
 mSymbols (NULL),
 mGlobals (NULL),
 mProcs (NULL),
 mProcData (NULL)
{
  ProcessHeader ();
  mProcData.reset (new PD_UINT8[mProcsCount]);

  for (D_UINT count = 0; count < mProcsCount; ++count)
    mProcData.get ()[count] = NULL;

}

WFileCompiledUnit::~WFileCompiledUnit ()
{
  for (D_UINT count = 0; count < mProcsCount; ++count)
    delete[]mProcData.get ()[count];
}

void
WFileCompiledUnit::ProcessHeader ()
{
  D_UINT8 t_buffer[WHC_TABLE_SIZE];
  D_UINT32 temp32;

  mFile.Seek (0, WHC_SEEK_BEGIN);
  mFile.Read (t_buffer, sizeof t_buffer);

  if ((t_buffer[0] != 'W') || (t_buffer[1] != 'O'))
    throw WCompiledUnitException ("Not an whisper object file!", _EXTRA(0));

  mGlobalsCount = from_le_int32 (t_buffer + WHC_GLOBS_COUNT_OFF);
  mProcsCount = from_le_int32 (t_buffer + WHC_PROCS_COUNT_OFF);

  temp32 = from_le_int32 (t_buffer + WHC_TYPEINFO_START_OFF);
  mTtySize = from_le_int32 (t_buffer + WHC_TYPEINFO_SIZE_OFF);
  mTypeInfo.reset (new D_UINT8[mTtySize]);
  mFile.Seek (temp32, WHC_SEEK_BEGIN);
  mFile.Read (mTypeInfo.get (), mTtySize);

  temp32 = from_le_int32 (t_buffer + WHC_SYMTABLE_START_OFF);
  mSymbolsSize = from_le_int32 (t_buffer + WHC_SYMTABLE_SIZE_OFF);
  mSymbols.reset (new D_UINT8[mSymbolsSize]);
  mFile.Seek (temp32, WHC_SEEK_BEGIN);
  mFile.Read (mSymbols.get (), mSymbolsSize);

  temp32 = from_le_int32 (t_buffer + WHC_CONSTAREA_START_OFF);
  mConstAreaSize = from_le_int32 (t_buffer + WHC_CONSTAREA_SIZE_OFF);
  mConstArea.reset (new D_UINT8[mConstAreaSize]);
  mFile.Seek (temp32, WHC_SEEK_BEGIN);
  mFile.Read (mConstArea.get (), mConstAreaSize);

  temp32 = (mGlobalsCount * WHC_GLOBAL_ENTRY_SIZE)
        + (mProcsCount * WHC_PROC_ENTRY_SIZE);
  mGlobals.reset (new D_UINT8[mGlobalsCount * WHC_GLOBAL_ENTRY_SIZE]);
  mProcs.reset (new D_UINT8[mProcsCount * WHC_PROC_ENTRY_SIZE]);
  mFile.Seek ((-1 * _SC (D_INT64, temp32)), WHC_SEEK_END);
  mFile.Read (mGlobals.get (), mGlobalsCount * WHC_GLOBS_COUNT_OFF);
  mFile.Read (mProcs.get (), mProcsCount * WHC_PROC_ENTRY_SIZE);

}

void
WFileCompiledUnit::LoadProcInMemory (D_UINT proc_item)
{
  assert (proc_item < mProcsCount);

  if (mProcData.get ()[proc_item] != NULL)
    return;

  const D_UINT8 *proc_entry = mProcs.get ()
        + (proc_item * WHC_PROC_ENTRY_SIZE);
  const D_UINT32 nlocals =
      from_le_int16 (proc_entry + WHC_PROC_ENTRY_NLOCAL_OFF);
  const D_UINT32 code_size =
      from_le_int32 (proc_entry + WHC_PROC_ENTRY_CODE_SIZE);
  const D_UINT32 body_pos =
      from_le_int32 (proc_entry + WHC_PROC_ENTRY_BODY_OFF);

  const D_UINT proc_body_size = (WHC_PROC_BODY_LOCAL_ENTRY_SIZE * nlocals)
        + WHC_PROC_BODY_SYNCS_ENTRY_SYZE + code_size;

  mProcData.get ()[proc_item] = new D_UINT8[proc_body_size];
  mFile.Seek (body_pos, WHC_SEEK_BEGIN);
  mFile.Read (mProcData.get ()[proc_item], proc_body_size);
}

D_UINT WFileCompiledUnit::GetTypeInformationSize ()
{
  return mTtySize;
}

const D_UINT8 *
WFileCompiledUnit::RetriveTypeInformation ()
{
  return mTypeInfo.get ();
}

D_UINT WFileCompiledUnit::GetConsAreaSize ()
{
  return mConstAreaSize;
}

const D_UINT8 *
WFileCompiledUnit::RetrieveConstArea ()
{
  return mConstArea.get ();
}

D_UINT WFileCompiledUnit::GetGlobalsCount ()
{
  return mGlobalsCount;
}

D_UINT WFileCompiledUnit::GetGlobalNameLenght (D_UINT item)
{
  if (item >= mGlobalsCount)
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));
  const D_UINT8 *const
  glb_entry = mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);

  D_UINT32
  temp32 = from_le_int32 (glb_entry + WHC_GLB_ENTRY_NAME_OFF);
  assert (temp32 < mSymbolsSize);
  return::strlen (_RC (const D_CHAR *, mSymbols.get ()) +temp32);
}

const D_CHAR *
WFileCompiledUnit::RetriveGlobalName (D_UINT item)
{
  if (item >= mGlobalsCount)
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));
  const D_UINT8 *const glb_entry =
      mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);

  D_UINT32 temp32 = from_le_int32 (glb_entry + WHC_GLB_ENTRY_NAME_OFF);
  assert (temp32 < mSymbolsSize);
  return _RC (const D_CHAR *, mSymbols.get ()) +temp32;
}

D_UINT WFileCompiledUnit::GetGlobalTypeIndex (D_UINT item)
{
  if (item >= mGlobalsCount)
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));
  const D_UINT8 *const
  glb_entry = mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);

  D_UINT32
  temp32 = from_le_int32 (glb_entry + WHC_GLB_ENTRY_TYPE_OFF);
  temp32 &= ~EXTERN_MASK;
  assert (temp32 < mTtySize);
  return temp32;
}

D_BOOL WFileCompiledUnit::IsGlobalExternal (D_UINT item)
{
  if (item >= mGlobalsCount)
    throw
    WCompiledUnitException ("Could not get the global variable descriptor.",
        _EXTRA(0));
  const D_UINT8 *const
  glb_entry = mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);

  D_UINT32
  temp32 = from_le_int32 (glb_entry + WHC_GLB_ENTRY_TYPE_OFF);
  return (temp32 & EXTERN_MASK) != 0;
}

D_UINT WFileCompiledUnit::GetProceduresCount ()
{
  return mProcsCount;
}

D_UINT WFileCompiledUnit::GetProcSyncStatementsCount (D_UINT item)
{
  if (item >= mProcsCount)
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  LoadProcInMemory (item);
  const D_UINT8 *const
  proc_data = mProcData.get ()[item];
  return proc_data[GetProcLocalsCount (item) * WHC_PROC_BODY_LOCAL_ENTRY_SIZE
                   + WHC_PROC_BODY_SYNCS_ENTRY_SYZE];
}

D_UINT WFileCompiledUnit::GetProcCodeAreaSize (D_UINT item)
{
  if (item >= mProcsCount)
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return from_le_int32 (mProcs.get ()
      + (item * WHC_PROC_ENTRY_SIZE) +
      WHC_PROC_ENTRY_CODE_SIZE);
}

const D_UINT8 *
WFileCompiledUnit::RetriveProcCodeArea (D_UINT item)
{
  if (item >= mProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.",
        _EXTRA(0));
  D_UINT8 nlocals = GetProcLocalsCount (item);

  LoadProcInMemory (item);
  return (mProcData.get ()[item] +
      (nlocals * WHC_PROC_BODY_LOCAL_ENTRY_SIZE) +
      WHC_PROC_BODY_SYNCS_ENTRY_SYZE);
}

D_UINT WFileCompiledUnit::GetProcLocalsCount (D_UINT item)
{
  if (item >= mProcsCount)
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return from_le_int16 (mProcs.get ()
      + (item * WHC_PROC_ENTRY_SIZE) +
      WHC_PROC_ENTRY_NLOCAL_OFF);
}

D_UINT WFileCompiledUnit::GetProcParametersCount (D_UINT item)
{
  if (item >= mProcsCount)
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return from_le_int16 (mProcs.get ()
      + (item * WHC_PROC_ENTRY_SIZE) +
      WHC_PROC_ENTRY_NPARMS_OFF);
}

D_UINT WFileCompiledUnit::GetProcReturnTypeIndex (D_UINT proc_item)
{
  if (proc_item >= mProcsCount)
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  const D_UINT32
  temp32 =
      (from_le_int32 (mProcs.get ()
          + (proc_item * WHC_PROC_ENTRY_SIZE) +
          WHC_PROC_ENTRY_TYPE_OFF) & (~EXTERN_MASK));
  assert (temp32 < mTtySize);
  return temp32;
}

D_UINT WFileCompiledUnit::GetProcNameSize (D_UINT proc_item)
{
  if (proc_item >= mProcsCount)
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  D_UINT
  temp32 = from_le_int32 (mProcs.get ()
      + (proc_item * WHC_PROC_ENTRY_SIZE) +
      WHC_PROC_ENTRY_NAME_OFF);
  assert (temp32 < mSymbolsSize);
  return::strlen (_RC (const D_CHAR *, mSymbols.get () + temp32));
}

const D_CHAR *
WFileCompiledUnit::RetriveProcName (D_UINT proc_item)
{
  if (proc_item >= mProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.",
        _EXTRA(0));

  D_UINT temp32 = from_le_int32 (mProcs.get ()
      + (proc_item * WHC_PROC_ENTRY_SIZE) +
      WHC_PROC_ENTRY_NAME_OFF);
  assert (temp32 < mSymbolsSize);
  return _RC (const D_CHAR *, mSymbols.get () + temp32);
}

D_UINT
WFileCompiledUnit::GetProcLocalTypeIndex (D_UINT item_proc,
    D_UINT item_local)
{

  if (item_local >= GetProcLocalsCount (item_proc))
    throw WCompiledUnitException ("Could not get local variable type.",
        _EXTRA(0));

  LoadProcInMemory (item_proc);

  const D_UINT8 *proc_entry = mProcData.get ()[item_proc];
  proc_entry += (item_local * WHC_PROC_BODY_LOCAL_ENTRY_SIZE);

  D_UINT temp32 = from_le_int32 (proc_entry);
  assert (temp32 < mTtySize);
  return temp32;
}

D_BOOL WFileCompiledUnit::IsProcExternal (D_UINT item_proc)
{
  if (item_proc >= mProcsCount)
    throw
    WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return
  (from_le_int32 (mProcs.get ()
      + (item_proc * WHC_PROC_ENTRY_SIZE) +
      WHC_PROC_ENTRY_TYPE_OFF) & (EXTERN_MASK)) != 0;
}
