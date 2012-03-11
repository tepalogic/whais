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

WBufferCompiledUnit::WBufferCompiledUnit (const D_UINT8*    buffer,
                                          D_UINT            buffer_size,
                                          WHC_MESSENGER     callback,
                                          WHC_MESSENGER_ARG context)
  : m_Handler (NULL)
{
  m_Handler = whc_hnd_create (_RC (const D_CHAR*, buffer), buffer_size, callback, context);

  if (m_Handler == NULL)
    throw WCompiledUnitException ("Failed to compile the buffer.", _EXTRA(0));
}

WBufferCompiledUnit::~WBufferCompiledUnit ()
{
  if (m_Handler != NULL)
    whc_hnd_destroy (m_Handler);
}

D_UINT
WBufferCompiledUnit::GetTypeInformationSize ()
{
  const D_UINT8* pDummy;
  const D_UINT   result = whc_get_typedec_pool (m_Handler, &pDummy);

  return result;
}

const D_UINT8*
WBufferCompiledUnit::RetriveTypeInformation ()
{
  const D_UINT8* pTypeInfo;

  whc_get_typedec_pool (m_Handler, &pTypeInfo);

  return pTypeInfo;
}

D_UINT
WBufferCompiledUnit::GetConstAreaSize ()
{
  const D_UINT8* pDummy;
  const D_UINT   result = whc_get_const_area (m_Handler, &pDummy);

  return result;
}

const D_UINT8*
WBufferCompiledUnit::RetrieveConstArea ()
{
  const D_UINT8* pConstArea;

  whc_get_const_area (m_Handler, &pConstArea);

  return pConstArea;
}

D_UINT
WBufferCompiledUnit::GetGlobalsCount ()
{
  return whc_get_globals_count (m_Handler);
}

D_UINT
WBufferCompiledUnit::GetProceduresCount ()
{
  return whc_get_procs_count (m_Handler);
}

D_UINT
WBufferCompiledUnit::GetGlobalNameLength (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;

  if (!whc_get_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  return globalDesc.name_len;
}

const D_CHAR*
WBufferCompiledUnit::RetriveGlobalName (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;

  if (!whc_get_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  return globalDesc.name;
}

D_UINT
WBufferCompiledUnit::GetGlobalTypeIndex (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;

  if (!whc_get_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const D_UINT8* pTemp;
  whc_get_typedec_pool (m_Handler, &pTemp);

  assert (pTemp && (pTemp <= globalDesc.type));

  return globalDesc.type - pTemp;

}

D_BOOL
WBufferCompiledUnit::IsGlobalExternal (D_UINT item)
{
  WHC_GLBVAR_DESC globalDesc;

  if (!whc_get_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  return (globalDesc.defined == FALSE);
}

D_UINT
WBufferCompiledUnit::GetProcSyncStatementsCount (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.syncs_count;
}

D_UINT
WBufferCompiledUnit::GetProcCodeAreaSize (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.instrs_size;
}

const D_UINT8*
WBufferCompiledUnit::RetriveProcCodeArea (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.instrs;
}

D_UINT
WBufferCompiledUnit::GetProcLocalsCount (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.locals_count;
}

D_UINT
WBufferCompiledUnit::GetProcParametersCount (D_UINT item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.param_count;
}

D_UINT
WBufferCompiledUnit::GetProcReturnTypeIndex (D_UINT proc_item)
{
  WHC_PROC_HANDLER hProc = whc_get_proc_hnd (m_Handler, proc_item);

  if (hProc == NULL)
    throw WCompiledUnitException ("Could not create the  procedure handle.", _EXTRA(0));

  const D_UINT8* pTemp;
  whc_get_typedec_pool (m_Handler, &pTemp);

  assert (pTemp && (pTemp <= whc_get_proc_rettype (m_Handler, hProc)));
  D_UINT result = whc_get_proc_rettype (m_Handler, hProc) - pTemp;

  whc_release_proc_hnd (m_Handler, hProc);

  return result;
}

D_UINT
WBufferCompiledUnit::GetProcNameSize (D_UINT proc_item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (m_Handler, proc_item, &desc))
    throw WCompiledUnitException ("Could not get the procedure's description.", _EXTRA(0));

  return desc.name_len;
}

const D_CHAR*
WBufferCompiledUnit::RetriveProcName (D_UINT proc_item)
{
  WHC_PROC_DESC desc;

  if (!whc_get_proc (m_Handler, proc_item, &desc))
    throw WCompiledUnitException ("Could not get procedure's description.", _EXTRA(0));

  return desc.name;
}

D_UINT
WBufferCompiledUnit::GetProcLocalTypeIndex (D_UINT item_proc, D_UINT item_local)
{
  if (item_local == 0)
    return GetProcReturnTypeIndex (item_proc);

  const D_UINT8* pLocalType;
  WHC_PROC_DESC  proc_desc;

  if (!whc_get_proc (m_Handler, item_proc, &proc_desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  WHC_PROC_HANDLER hProc = whc_get_proc_hnd (m_Handler, item_proc);

  if (hProc == NULL)
    throw WCompiledUnitException ("Could not create the  procedure handle.", _EXTRA(0));

  pLocalType = whc_get_local_type (m_Handler, hProc, item_local);

  whc_release_proc_hnd (m_Handler, hProc);
  if (pLocalType == NULL)
    throw WCompiledUnitException ("Could not get local variable type.", _EXTRA(0));

  const D_UINT8* pLocalDesc;
  whc_get_typedec_pool (m_Handler, &pLocalDesc);

  assert (pLocalDesc && (pLocalDesc <= pLocalType));
  return pLocalType - pLocalDesc;
}

D_BOOL
WBufferCompiledUnit::IsProcExternal (D_UINT item_proc)
{
  return (GetProcCodeAreaSize (item_proc) == 0);
}

/////////////******WFileCompiledUnit********//////////////////////////////

WFileCompiledUnit::WFileCompiledUnit (const D_CHAR * file_name)
  : m_File (file_name, WHC_FILEREAD),
    m_GlobalsCount (0),
    m_ProcsCount (0),
    m_TtySize (0),
    m_SymbolsSize (0),
    m_TypeInfo (NULL),
    m_Symbols (NULL),
    m_Globals (NULL),
    m_Procs (NULL),
    m_ProcData (NULL)
{
  ProcessHeader ();
  m_ProcData.reset (new PD_UINT8[m_ProcsCount]);

  for (D_UINT count = 0; count < m_ProcsCount; ++count)
    m_ProcData.get ()[count] = NULL;

}

WFileCompiledUnit::~WFileCompiledUnit ()
{
  for (D_UINT count = 0; count < m_ProcsCount; ++count)
    delete [] m_ProcData.get ()[count];
}

void
WFileCompiledUnit::ProcessHeader ()
{
  D_UINT8  t_buffer[WHC_TABLE_SIZE];
  D_UINT32 temp32;

  m_File.Seek (0, WHC_SEEK_BEGIN);
  m_File.Read (t_buffer, sizeof t_buffer);

  if ((t_buffer[0] != 'W') || (t_buffer[1] != 'O'))
    throw WCompiledUnitException ("Not an whisper object file!", _EXTRA(0));

  m_GlobalsCount = from_le_int32 (t_buffer + WHC_GLOBS_COUNT_OFF);
  m_ProcsCount   = from_le_int32 (t_buffer + WHC_PROCS_COUNT_OFF);

  temp32   = from_le_int32 (t_buffer + WHC_TYPEINFO_START_OFF);
  m_TtySize = from_le_int32 (t_buffer + WHC_TYPEINFO_SIZE_OFF);

  m_TypeInfo.reset (new D_UINT8[m_TtySize]);
  m_File.Seek (temp32, WHC_SEEK_BEGIN);
  m_File.Read (m_TypeInfo.get (), m_TtySize);

  temp32       = from_le_int32 (t_buffer + WHC_SYMTABLE_START_OFF);
  m_SymbolsSize = from_le_int32 (t_buffer + WHC_SYMTABLE_SIZE_OFF);

  m_Symbols.reset (new D_UINT8[m_SymbolsSize]);
  m_File.Seek (temp32, WHC_SEEK_BEGIN);
  m_File.Read (m_Symbols.get (), m_SymbolsSize);

  temp32         = from_le_int32 (t_buffer + WHC_CONSTAREA_START_OFF);
  m_ConstAreaSize = from_le_int32 (t_buffer + WHC_CONSTAREA_SIZE_OFF);

  m_ConstArea.reset (new D_UINT8[m_ConstAreaSize]);
  m_File.Seek (temp32, WHC_SEEK_BEGIN);
  m_File.Read (m_ConstArea.get (), m_ConstAreaSize);

  temp32 = (m_GlobalsCount * WHC_GLOBAL_ENTRY_SIZE) + (m_ProcsCount * WHC_PROC_ENTRY_SIZE);

  m_Globals.reset (new D_UINT8[m_GlobalsCount * WHC_GLOBAL_ENTRY_SIZE]);
  m_Procs.reset (new D_UINT8[m_ProcsCount * WHC_PROC_ENTRY_SIZE]);
  m_File.Seek ((-1 * _SC (D_INT64, temp32)), WHC_SEEK_END);
  m_File.Read (m_Globals.get (), m_GlobalsCount * WHC_GLOBS_COUNT_OFF);
  m_File.Read (m_Procs.get (), m_ProcsCount * WHC_PROC_ENTRY_SIZE);

}

void
WFileCompiledUnit::LoadProcInMemory (D_UINT proc_item)
{
  assert (proc_item < m_ProcsCount);

  if (m_ProcData.get ()[proc_item] != NULL)
    return;

  const D_UINT8* pProcEntry     = m_Procs.get () + (proc_item * WHC_PROC_ENTRY_SIZE);
  const D_UINT32 nlocals        = from_le_int16 (pProcEntry + WHC_PROC_ENTRY_NLOCAL_OFF);
  const D_UINT32 code_size      = from_le_int32 (pProcEntry + WHC_PROC_ENTRY_CODE_SIZE);
  const D_UINT32 body_pos       = from_le_int32 (pProcEntry + WHC_PROC_ENTRY_BODY_OFF);
  const D_UINT   proc_body_size = (WHC_PROC_BODY_LOCAL_ENTRY_SIZE * nlocals) +
                                  WHC_PROC_BODY_SYNCS_ENTRY_SYZE + code_size;

  m_ProcData.get ()[proc_item] = new D_UINT8[proc_body_size];
  m_File.Seek (body_pos, WHC_SEEK_BEGIN);
  m_File.Read (m_ProcData.get ()[proc_item], proc_body_size);
}

D_UINT WFileCompiledUnit::GetTypeInformationSize ()
{
  return m_TtySize;
}

const D_UINT8 *
WFileCompiledUnit::RetriveTypeInformation ()
{
  return m_TypeInfo.get ();
}

D_UINT WFileCompiledUnit::GetConstAreaSize ()
{
  return m_ConstAreaSize;
}

const D_UINT8 *
WFileCompiledUnit::RetrieveConstArea ()
{
  return m_ConstArea.get ();
}

D_UINT
WFileCompiledUnit::GetGlobalsCount ()
{
  return m_GlobalsCount;
}

D_UINT
WFileCompiledUnit::GetGlobalNameLength (D_UINT item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const D_UINT8* const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  D_UINT32             temp32    = from_le_int32 (pGlbEntry + WHC_GLB_ENTRY_NAME_OFF);

  assert (temp32 < m_SymbolsSize);
  return strlen (_RC (const D_CHAR *, m_Symbols.get ()) +temp32);
}

const D_CHAR*
WFileCompiledUnit::RetriveGlobalName (D_UINT item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const D_UINT8* const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  D_UINT32             temp32    = from_le_int32 (pGlbEntry + WHC_GLB_ENTRY_NAME_OFF);

  assert (temp32 < m_SymbolsSize);
  return _RC (const D_CHAR *, m_Symbols.get ()) +temp32;
}

D_UINT
WFileCompiledUnit::GetGlobalTypeIndex (D_UINT item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const D_UINT8 *const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  D_UINT32             temp32    = from_le_int32 (pGlbEntry + WHC_GLB_ENTRY_TYPE_OFF);

  temp32 &= ~EXTERN_MASK;
  assert (temp32 < m_TtySize);
  return temp32;
}

D_BOOL
WFileCompiledUnit::IsGlobalExternal (D_UINT item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const D_UINT8 *const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  D_UINT32             temp32    = from_le_int32 (pGlbEntry + WHC_GLB_ENTRY_TYPE_OFF);

  return (temp32 & EXTERN_MASK) != 0;
}

D_UINT
WFileCompiledUnit::GetProceduresCount ()
{
  return m_ProcsCount;
}

D_UINT
WFileCompiledUnit::GetProcSyncStatementsCount (D_UINT item)
{
  if (item >= m_ProcsCount)
    throw  WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  LoadProcInMemory (item);

  const D_UINT8 *const proc_data = m_ProcData.get ()[item];

  return proc_data[GetProcLocalsCount (item) * WHC_PROC_BODY_LOCAL_ENTRY_SIZE +
                   WHC_PROC_BODY_SYNCS_ENTRY_SYZE];
}

D_UINT
WFileCompiledUnit::GetProcCodeAreaSize (D_UINT item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return from_le_int32 (m_Procs.get () + (item * WHC_PROC_ENTRY_SIZE) + WHC_PROC_ENTRY_CODE_SIZE);
}

const D_UINT8*
WFileCompiledUnit::RetriveProcCodeArea (D_UINT item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  D_UINT8 nlocals = GetProcLocalsCount (item);

  LoadProcInMemory (item);

  return (m_ProcData.get ()[item] +
          (nlocals * WHC_PROC_BODY_LOCAL_ENTRY_SIZE) +
          WHC_PROC_BODY_SYNCS_ENTRY_SYZE);
}

D_UINT
WFileCompiledUnit::GetProcLocalsCount (D_UINT item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return from_le_int16 (m_Procs.get () +
                        (item * WHC_PROC_ENTRY_SIZE) +
                        WHC_PROC_ENTRY_NLOCAL_OFF);
}

D_UINT
WFileCompiledUnit::GetProcParametersCount (D_UINT item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return from_le_int16 (m_Procs.get () + (item * WHC_PROC_ENTRY_SIZE) + WHC_PROC_ENTRY_NPARMS_OFF);
}

D_UINT
WFileCompiledUnit::GetProcReturnTypeIndex (D_UINT proc_item)
{
  if (proc_item >= m_ProcsCount)
    throw  WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  const D_UINT32 temp32 = (from_le_int32 (m_Procs.get () +
                                          (proc_item * WHC_PROC_ENTRY_SIZE) +
                                          WHC_PROC_ENTRY_TYPE_OFF) &
                           ~EXTERN_MASK);
  assert (temp32 < m_TtySize);
  return temp32;
}

D_UINT
WFileCompiledUnit::GetProcNameSize (D_UINT proc_item)
{
  if (proc_item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  D_UINT temp32 = from_le_int32 (m_Procs.get () +
                                 (proc_item * WHC_PROC_ENTRY_SIZE) +
                                 WHC_PROC_ENTRY_NAME_OFF);
  assert (temp32 < m_SymbolsSize);
  return::strlen (_RC (const D_CHAR *, m_Symbols.get () + temp32));
}

const D_CHAR *
WFileCompiledUnit::RetriveProcName (D_UINT proc_item)
{
  if (proc_item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.",
        _EXTRA(0));

  D_UINT temp32 = from_le_int32 (m_Procs.get ()
      + (proc_item * WHC_PROC_ENTRY_SIZE) +
      WHC_PROC_ENTRY_NAME_OFF);
  assert (temp32 < m_SymbolsSize);
  return _RC (const D_CHAR *, m_Symbols.get () + temp32);
}

D_UINT
WFileCompiledUnit::GetProcLocalTypeIndex (D_UINT item_proc, D_UINT item_local)
{
  if (item_local == 0)
    return GetProcReturnTypeIndex (item_proc);

  if (item_local >= GetProcLocalsCount (item_proc))
    throw WCompiledUnitException ("Could not get local variable type.", _EXTRA(0));

  LoadProcInMemory (item_proc);

  const D_UINT8 *pProcEntry = m_ProcData.get ()[item_proc];
  pProcEntry += (item_local * WHC_PROC_BODY_LOCAL_ENTRY_SIZE);

  D_UINT temp32 = from_le_int32 (pProcEntry);
  assert (temp32 < m_TtySize);
  return temp32;
}

D_BOOL
WFileCompiledUnit::IsProcExternal (D_UINT item_proc)
{
  if (item_proc >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return (from_le_int32 (m_Procs.get () +
                         (item_proc * WHC_PROC_ENTRY_SIZE) +
                         WHC_PROC_ENTRY_TYPE_OFF) &
         (EXTERN_MASK)) != 0;
}
