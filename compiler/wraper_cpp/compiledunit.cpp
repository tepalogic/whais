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

#include "compiler/compiledunit.h"
#include "utils/le_converter.h"

#include "../whc/wo_format.h"

using namespace whisper;

/////////////******WBufferCompiledUnit********//////////////////////////////

WBufferCompiledUnit::WBufferCompiledUnit (const uint8_t*    pBuuffer,
                                          uint_t            bufferSize,
                                          WH_MESSENGER     messenger,
                                          WH_MESSENGER_CTXT messengerContext) :
  m_Handler (NULL)
{
  m_Handler = wh_compiler_load (_RC (const char*, pBuuffer),
                              bufferSize,
                              messenger,
                              messengerContext);
  if (m_Handler == NULL)
    throw WCompiledUnitException ("Failed to compile the buffer.", _EXTRA(0));
}

WBufferCompiledUnit::~WBufferCompiledUnit ()
{
  if (m_Handler != NULL)
    wh_compiler_discard (m_Handler);
}

uint_t
WBufferCompiledUnit::GetTypeInformationSize ()
{
  const uint8_t* pDummy;
  const uint_t   result = wh_unit_type_descriptors (m_Handler, &pDummy);

  return result;
}

const uint8_t*
WBufferCompiledUnit::RetriveTypeInformation ()
{
  const uint8_t* pTypeInfo = NULL;

  wh_unit_type_descriptors (m_Handler, &pTypeInfo);

  return pTypeInfo;
}

uint_t
WBufferCompiledUnit::GetConstAreaSize ()
{
  const uint8_t* pDummy;
  const uint_t   result = wh_unit_constants (m_Handler, &pDummy);

  return result;
}

const uint8_t*
WBufferCompiledUnit::RetrieveConstArea ()
{
  const uint8_t* pConstArea = NULL;

  wh_unit_constants (m_Handler, &pConstArea);

  return pConstArea;
}

uint_t
WBufferCompiledUnit::GetGlobalsCount ()
{
  return wh_unit_globals_count (m_Handler);
}

uint_t
WBufferCompiledUnit::GetProceduresCount ()
{
  return wh_unit_procedures_count (m_Handler);
}

uint_t
WBufferCompiledUnit::GetGlobalNameLength (uint_t item)
{
  WCompilerGlobalDesc globalDesc = {NULL, };

  if (!wh_unit_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  return globalDesc.nameLength;
}

const char*
WBufferCompiledUnit::RetriveGlobalName (uint_t item)
{
  WCompilerGlobalDesc globalDesc;

  if (!wh_unit_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  return globalDesc.name;
}

uint_t
WBufferCompiledUnit::GetGlobalTypeIndex (uint_t item)
{
  WCompilerGlobalDesc globalDesc;

  if (!wh_unit_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const uint8_t* pTemp;
  wh_unit_type_descriptors (m_Handler, &pTemp);

  assert (pTemp && (pTemp <= globalDesc.type));

  return globalDesc.type - pTemp;

}

bool_t
WBufferCompiledUnit::IsGlobalExternal (uint_t item)
{
  WCompilerGlobalDesc globalDesc;

  if (!wh_unit_global (m_Handler, item, &globalDesc))
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  return (globalDesc.defined == FALSE);
}

uint_t
WBufferCompiledUnit::GetProcSyncStatementsCount (uint_t item)
{
  WCompilerProcedureDesc desc;

  if (!wh_unit_procedure (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.syncsCount;
}

uint_t
WBufferCompiledUnit::GetProcCodeAreaSize (uint_t item)
{
  WCompilerProcedureDesc desc;

  if (!wh_unit_procedure (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.codeSize;
}

const uint8_t*
WBufferCompiledUnit::RetriveProcCodeArea (uint_t item)
{
  WCompilerProcedureDesc desc;

  if (!wh_unit_procedure (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.code;
}

uint_t
WBufferCompiledUnit::GetProcLocalsCount (uint_t item)
{
  WCompilerProcedureDesc desc;

  if (!wh_unit_procedure (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.localsCount;
}

uint_t
WBufferCompiledUnit::GetProcParametersCount (uint_t item)
{
  WCompilerProcedureDesc desc;

  if (!wh_unit_procedure (m_Handler, item, &desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return desc.paramsCount;
}

uint_t
WBufferCompiledUnit::GetProcReturnTypeIndex (uint_t proc_item)
{
  WH_COMPILED_UNIT_PROC hProc = wh_unit_procedure_get (m_Handler, proc_item);

  if (hProc == NULL)
    throw WCompiledUnitException ("Could not create the  procedure handle.", _EXTRA(0));

  const uint8_t* pTemp;
  wh_unit_type_descriptors (m_Handler, &pTemp);

  assert (pTemp && (pTemp <= wh_procedure_return_type (m_Handler, hProc)));
  uint_t result = wh_procedure_return_type (m_Handler, hProc) - pTemp;

  wh_unit_procedure_release (m_Handler, hProc);

  return result;
}

uint_t
WBufferCompiledUnit::GetProcNameSize (uint_t proc_item)
{
  WCompilerProcedureDesc desc;

  if (!wh_unit_procedure (m_Handler, proc_item, &desc))
    throw WCompiledUnitException ("Could not get the procedure's description.", _EXTRA(0));

  return desc.nameLength;
}

const char*
WBufferCompiledUnit::RetriveProcName (uint_t proc_item)
{
  WCompilerProcedureDesc desc;

  if (!wh_unit_procedure (m_Handler, proc_item, &desc))
    throw WCompiledUnitException ("Could not get procedure's description.", _EXTRA(0));

  return desc.name;
}

uint_t
WBufferCompiledUnit::GetProcLocalTypeIndex (uint_t item_proc, uint_t item_local)
{
  if (item_local == 0)
    return GetProcReturnTypeIndex (item_proc);

  const uint8_t* pLocalType;
  WCompilerProcedureDesc  proc_desc;

  if (!wh_unit_procedure (m_Handler, item_proc, &proc_desc))
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  WH_COMPILED_UNIT_PROC hProc = wh_unit_procedure_get (m_Handler, item_proc);

  if (hProc == NULL)
    throw WCompiledUnitException ("Could not create the  procedure handle.", _EXTRA(0));

  pLocalType = wh_procedure_local_type (m_Handler, hProc, item_local);

  wh_unit_procedure_release (m_Handler, hProc);
  if (pLocalType == NULL)
    throw WCompiledUnitException ("Could not get local variable type.", _EXTRA(0));

  const uint8_t* pLocalDesc;
  wh_unit_type_descriptors (m_Handler, &pLocalDesc);

  assert (pLocalDesc && (pLocalDesc <= pLocalType));
  return pLocalType - pLocalDesc;
}

bool_t
WBufferCompiledUnit::IsProcExternal (uint_t item_proc)
{
  return (GetProcCodeAreaSize (item_proc) == 0);
}

/////////////******WFileCompiledUnit********//////////////////////////////

WFileCompiledUnit::WFileCompiledUnit (const char * file_name) :
    m_File (file_name, WHC_FILEREAD),
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
  m_ProcData.reset (new uint8_t*[m_ProcsCount]);

  for (uint_t count = 0; count < m_ProcsCount; ++count)
    m_ProcData.get ()[count] = NULL;

}

WFileCompiledUnit::~WFileCompiledUnit ()
{
  for (uint_t count = 0; count < m_ProcsCount; ++count)
    delete [] m_ProcData.get ()[count];
}

void
WFileCompiledUnit::ProcessHeader ()
{
  uint8_t  t_buffer[WHC_TABLE_SIZE];
  uint32_t temp32;

  m_File.Seek (0, WHC_SEEK_BEGIN);
  m_File.Read (t_buffer, sizeof t_buffer);

  if ((t_buffer[0] != 'W') || (t_buffer[1] != 'O'))
    throw WCompiledUnitException ("Not an whisper object file!", _EXTRA(0));

  m_GlobalsCount = load_le_int32 (t_buffer + WHC_GLOBS_COUNT_OFF);
  m_ProcsCount   = load_le_int32 (t_buffer + WHC_PROCS_COUNT_OFF);

  temp32    = load_le_int32 (t_buffer + WHC_TYPEINFO_START_OFF);
  m_TtySize = load_le_int32 (t_buffer + WHC_TYPEINFO_SIZE_OFF);

  m_TypeInfo.reset (new uint8_t[m_TtySize]);
  m_File.Seek (temp32, WHC_SEEK_BEGIN);
  m_File.Read (m_TypeInfo.get (), m_TtySize);

  temp32        = load_le_int32 (t_buffer + WHC_SYMTABLE_START_OFF);
  m_SymbolsSize = load_le_int32 (t_buffer + WHC_SYMTABLE_SIZE_OFF);

  m_Symbols.reset (new uint8_t[m_SymbolsSize]);
  m_File.Seek (temp32, WHC_SEEK_BEGIN);
  m_File.Read (m_Symbols.get (), m_SymbolsSize);

  temp32         = load_le_int32 (t_buffer + WHC_CONSTAREA_START_OFF);
  m_ConstAreaSize = load_le_int32 (t_buffer + WHC_CONSTAREA_SIZE_OFF);

  m_ConstArea.reset (new uint8_t[m_ConstAreaSize]);
  m_File.Seek (temp32, WHC_SEEK_BEGIN);
  m_File.Read (m_ConstArea.get (), m_ConstAreaSize);

  temp32 = (m_GlobalsCount * WHC_GLOBAL_ENTRY_SIZE) + (m_ProcsCount * WHC_PROC_ENTRY_SIZE);

  m_Globals.reset (new uint8_t[m_GlobalsCount * WHC_GLOBAL_ENTRY_SIZE]);
  m_Procs.reset (new uint8_t[m_ProcsCount * WHC_PROC_ENTRY_SIZE]);
  m_File.Seek ((-1 * _SC (int64_t, temp32)), WHC_SEEK_END);
  m_File.Read (m_Globals.get (), m_GlobalsCount * WHC_GLOBS_COUNT_OFF);
  m_File.Read (m_Procs.get (), m_ProcsCount * WHC_PROC_ENTRY_SIZE);

}

void
WFileCompiledUnit::LoadProcInMemory (uint_t proc_item)
{
  assert (proc_item < m_ProcsCount);

  if (m_ProcData.get ()[proc_item] != NULL)
    return;

  const uint8_t* pProcEntry     = m_Procs.get () + (proc_item * WHC_PROC_ENTRY_SIZE);
  const uint32_t nlocals        = load_le_int16 (pProcEntry + WHC_PROC_ENTRY_NLOCAL_OFF);
  const uint32_t code_size      = load_le_int32 (pProcEntry + WHC_PROC_ENTRY_CODE_SIZE);
  const uint32_t body_pos       = load_le_int32 (pProcEntry + WHC_PROC_ENTRY_BODY_OFF);
  const uint_t   proc_body_size = (WHC_PROC_BODY_LOCAL_ENTRY_SIZE * nlocals) +
                                  WHC_PROC_BODY_SYNCS_ENTRY_SYZE + code_size;

  m_ProcData.get ()[proc_item] = new uint8_t[proc_body_size];
  m_File.Seek (body_pos, WHC_SEEK_BEGIN);
  m_File.Read (m_ProcData.get ()[proc_item], proc_body_size);
}

uint_t WFileCompiledUnit::GetTypeInformationSize ()
{
  return m_TtySize;
}

const uint8_t*
WFileCompiledUnit::RetriveTypeInformation ()
{
  return m_TypeInfo.get ();
}

uint_t WFileCompiledUnit::GetConstAreaSize ()
{
  return m_ConstAreaSize;
}

const uint8_t*
WFileCompiledUnit::RetrieveConstArea ()
{
  return m_ConstArea.get ();
}

uint_t
WFileCompiledUnit::GetGlobalsCount ()
{
  return m_GlobalsCount;
}

uint_t
WFileCompiledUnit::GetGlobalNameLength (uint_t item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const uint8_t* const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  uint32_t             temp32    = load_le_int32 (pGlbEntry + WHC_GLB_ENTRY_NAME_OFF);

  assert (temp32 < m_SymbolsSize);
  return strlen (_RC (const char *, m_Symbols.get ()) +temp32);
}

const char*
WFileCompiledUnit::RetriveGlobalName (uint_t item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const uint8_t* const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  uint32_t             temp32    = load_le_int32 (pGlbEntry + WHC_GLB_ENTRY_NAME_OFF);

  assert (temp32 < m_SymbolsSize);
  return _RC (const char *, m_Symbols.get ()) +temp32;
}

uint_t
WFileCompiledUnit::GetGlobalTypeIndex (uint_t item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const uint8_t *const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  uint32_t             temp32    = load_le_int32 (pGlbEntry + WHC_GLB_ENTRY_TYPE_OFF);

  temp32 &= ~EXTERN_MASK;
  assert (temp32 < m_TtySize);
  return temp32;
}

bool_t
WFileCompiledUnit::IsGlobalExternal (uint_t item)
{
  if (item >= m_GlobalsCount)
    throw WCompiledUnitException ("Could not get the global variable descriptor.", _EXTRA(0));

  const uint8_t *const pGlbEntry = m_Globals.get () + (WHC_GLOBAL_ENTRY_SIZE * item);
  uint32_t             temp32    = load_le_int32 (pGlbEntry + WHC_GLB_ENTRY_TYPE_OFF);

  return (temp32 & EXTERN_MASK) != 0;
}

uint_t
WFileCompiledUnit::GetProceduresCount ()
{
  return m_ProcsCount;
}

uint_t
WFileCompiledUnit::GetProcSyncStatementsCount (uint_t item)
{
  if (item >= m_ProcsCount)
    throw  WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  LoadProcInMemory (item);

  const uint8_t *const proc_data = m_ProcData.get ()[item];

  return proc_data[GetProcLocalsCount (item) * WHC_PROC_BODY_LOCAL_ENTRY_SIZE +
                   WHC_PROC_BODY_SYNCS_ENTRY_SYZE];
}

uint_t
WFileCompiledUnit::GetProcCodeAreaSize (uint_t item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return load_le_int32 (m_Procs.get () + (item * WHC_PROC_ENTRY_SIZE) + WHC_PROC_ENTRY_CODE_SIZE);
}

const uint8_t*
WFileCompiledUnit::RetriveProcCodeArea (uint_t item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  uint8_t nlocals = GetProcLocalsCount (item);

  LoadProcInMemory (item);

  return (m_ProcData.get ()[item] +
          (nlocals * WHC_PROC_BODY_LOCAL_ENTRY_SIZE) +
          WHC_PROC_BODY_SYNCS_ENTRY_SYZE);
}

uint_t
WFileCompiledUnit::GetProcLocalsCount (uint_t item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return load_le_int16 (m_Procs.get () +
                        (item * WHC_PROC_ENTRY_SIZE) +
                        WHC_PROC_ENTRY_NLOCAL_OFF);
}

uint_t
WFileCompiledUnit::GetProcParametersCount (uint_t item)
{
  if (item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return load_le_int16 (m_Procs.get () + (item * WHC_PROC_ENTRY_SIZE) + WHC_PROC_ENTRY_NPARMS_OFF);
}

uint_t
WFileCompiledUnit::GetProcReturnTypeIndex (uint_t proc_item)
{
  if (proc_item >= m_ProcsCount)
    throw  WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  const uint32_t temp32 = (load_le_int32 (m_Procs.get () +
                                          (proc_item * WHC_PROC_ENTRY_SIZE) +
                                          WHC_PROC_ENTRY_TYPE_OFF) &
                           ~EXTERN_MASK);
  assert (temp32 < m_TtySize);
  return temp32;
}

uint_t
WFileCompiledUnit::GetProcNameSize (uint_t proc_item)
{
  if (proc_item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  uint_t temp32 = load_le_int32 (m_Procs.get () +
                                 (proc_item * WHC_PROC_ENTRY_SIZE) +
                                 WHC_PROC_ENTRY_NAME_OFF);
  assert (temp32 < m_SymbolsSize);
  return::strlen (_RC (const char *, m_Symbols.get () + temp32));
}

const char*
WFileCompiledUnit::RetriveProcName (uint_t proc_item)
{
  if (proc_item >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.",
        _EXTRA(0));

  uint_t temp32 = load_le_int32 (m_Procs.get () +
                                 (proc_item * WHC_PROC_ENTRY_SIZE) +
                                 WHC_PROC_ENTRY_NAME_OFF);
  assert (temp32 < m_SymbolsSize);
  return _RC (const char *, m_Symbols.get () + temp32);
}

uint_t
WFileCompiledUnit::GetProcLocalTypeIndex (uint_t item_proc, uint_t item_local)
{
  if (item_local == 0)
    return GetProcReturnTypeIndex (item_proc);

  if (item_local >= GetProcLocalsCount (item_proc))
    throw WCompiledUnitException ("Could not get local variable type.", _EXTRA(0));

  LoadProcInMemory (item_proc);

  const uint8_t* pProcEntry = m_ProcData.get ()[item_proc];
  pProcEntry += (item_local * WHC_PROC_BODY_LOCAL_ENTRY_SIZE);

  uint_t temp32 = load_le_int32 (pProcEntry);
  assert (temp32 < m_TtySize);
  return temp32;
}

bool_t
WFileCompiledUnit::IsProcExternal (uint_t item_proc)
{
  if (item_proc >= m_ProcsCount)
    throw WCompiledUnitException ("Could not get procedure description.", _EXTRA(0));

  return (load_le_int32 (m_Procs.get () +
                         (item_proc * WHC_PROC_ENTRY_SIZE) +
                         WHC_PROC_ENTRY_TYPE_OFF) &
         (EXTERN_MASK)) != 0;
}


#if defined (ENABLE_MEMORY_TRACE) && defined (USE_COMPILER_SHL)
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "COMPILER";
#endif

