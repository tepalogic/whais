/******************************************************************************
WHAISC - A compiler for whais programs
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
#include "utils/endianness.h"

#include "../whc/wo_format.h"



namespace whais {



WIFunctionalUnit::~WIFunctionalUnit ()
{
}


/////////////******CompiledBufferUnit********//////////////////////////////

CompiledBufferUnit::CompiledBufferUnit (const uint8_t*     buffer,
                                        uint_t             bufferSize,
                                        WH_MESSENGER       messenger,
                                        WH_MESSENGER_CTXT  messengerContext)
  : mHandler (NULL)
{
  mHandler = wh_compiler_load (_RC (const char*, buffer),
                               bufferSize,
                               messenger,
                               messengerContext);
  if (mHandler == NULL)
    throw FunctionalUnitException (_EXTRA (0), "Buffer could not be compiled.");
}


CompiledBufferUnit::~CompiledBufferUnit ()
{
  if (mHandler != NULL)
    wh_compiler_discard (mHandler);
}


uint_t
CompiledBufferUnit::TypeAreaSize ()
{
  const uint8_t* dummy;
  const uint_t   result = wh_unit_type_descriptors (mHandler, &dummy);

  return result;
}


const uint8_t*
CompiledBufferUnit::RetriveTypeArea ()
{
  const uint8_t* typePool = NULL;

  wh_unit_type_descriptors (mHandler, &typePool);

  return typePool;
}


uint_t
CompiledBufferUnit::ConstsAreaSize ()
{
  const uint8_t* dummy;
  const uint_t   result = wh_unit_constants (mHandler, &dummy);

  return result;
}


const uint8_t*
CompiledBufferUnit::RetrieveConstArea ()
{
  const uint8_t* constArea = NULL;

  wh_unit_constants (mHandler, &constArea);

  return constArea;
}


uint_t
CompiledBufferUnit::GlobalsCount ()
{
  return wh_unit_globals_count (mHandler);
}


uint_t
CompiledBufferUnit::ProceduresCount ()
{
  return wh_unit_procedures_count (mHandler);
}


uint_t
CompiledBufferUnit::GlobalNameLength (const uint_t id)
{
  WCompilerGlobalDesc globalDesc = {NULL, };

  if ( ! wh_unit_global (mHandler, id, &globalDesc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve global value (index: '%u') description.",
              id
                                    );
    }

  return globalDesc.nameLength;
}


const char*
CompiledBufferUnit::RetriveGlobalName (const uint_t id)
{
  WCompilerGlobalDesc globalDesc;

  if ( ! wh_unit_global (mHandler, id, &globalDesc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve global value (index: '%u') description.",
              id
                                    );
    }

  return globalDesc.name;
}


uint_t
CompiledBufferUnit::GlobalTypeOff (const uint_t id)
{
  WCompilerGlobalDesc globalDesc;

  if ( ! wh_unit_global (mHandler, id, &globalDesc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve global value (index: '%u') description.",
              id
                                    );
    }

  const uint8_t* temp;

  wh_unit_type_descriptors (mHandler, &temp);

  assert (temp && (temp <= globalDesc.type));

  return globalDesc.type - temp;

}


bool_t
CompiledBufferUnit::IsGlobalExternal (const uint_t id)
{
  WCompilerGlobalDesc globalDesc;

  if ( ! wh_unit_global (mHandler, id, &globalDesc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve global value (index: '%u') description.",
              id
                                    );
    }

  return (globalDesc.defined == FALSE);
}


uint_t
CompiledBufferUnit::ProcSyncStatementsCount (const uint_t id)
{
  WCompilerProcedureDesc desc;

  if ( ! wh_unit_procedure (mHandler, id, &desc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              id
                                    );
    }

  return desc.syncsCount;
}


uint_t
CompiledBufferUnit::ProcCodeAreaSize (const uint_t id)
{
  WCompilerProcedureDesc desc;

  if ( ! wh_unit_procedure (mHandler, id, &desc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              id
                                    );
    }

  return desc.codeSize;
}


const uint8_t*
CompiledBufferUnit::RetriveProcCodeArea (const uint_t id)
{
  WCompilerProcedureDesc desc;

  if ( ! wh_unit_procedure (mHandler, id, &desc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              id
                                    );
    }

  return desc.code;
}


uint_t
CompiledBufferUnit::ProcLocalsCount (const uint_t id)
{
  WCompilerProcedureDesc desc;

  if ( ! wh_unit_procedure (mHandler, id, &desc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              id
                                    );
    }

  return desc.localsCount;
}


uint_t
CompiledBufferUnit::ProcParametersCount (const uint_t id)
{
  WCompilerProcedureDesc desc;

  if ( ! wh_unit_procedure (mHandler, id, &desc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              id
                                    );
    }

  return desc.paramsCount;
}


uint_t
CompiledBufferUnit::GetProcReturnTypeOff (const uint_t id)
{
  WH_COMPILED_UNIT_PROC hProc = wh_unit_procedure_get (mHandler, id);

  if (hProc == NULL)
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not obtain procedure (index: '%u') handle.",
              id
                                    );
    }

  const uint8_t* temp;
  wh_unit_type_descriptors (mHandler, &temp);

  assert (temp && (temp <= wh_procedure_return_type (mHandler, hProc)));

  uint_t result = wh_procedure_return_type (mHandler, hProc) - temp;

  wh_unit_procedure_release (mHandler, hProc);

  return result;
}


uint_t
CompiledBufferUnit::GetProcNameSize (const uint_t id)
{
  WCompilerProcedureDesc desc;

  if ( ! wh_unit_procedure (mHandler, id, &desc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              id
                                    );
    }

  return desc.nameLength;
}


const char*
CompiledBufferUnit::RetriveProcName (const uint_t id)
{
  WCompilerProcedureDesc desc;

  if ( ! wh_unit_procedure (mHandler, id, &desc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              id
                                    );
    }

  return desc.name;
}


uint_t
CompiledBufferUnit::GetProcLocalTypeOff (uint_t procId, uint_t localId)
{
  if (localId == 0)
    return GetProcReturnTypeOff (procId);

  const uint8_t*          localType;
  WCompilerProcedureDesc  procDesc;

  if ( ! wh_unit_procedure (mHandler, procId, &procDesc))
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not retrieve procedure (index: '%u') description.",
              procId
                                    );
    }

  WH_COMPILED_UNIT_PROC hProc = wh_unit_procedure_get (mHandler, procId);

  if (hProc == NULL)
    {
      throw FunctionalUnitException (
              _EXTRA (0),
              "Could not obtain procedure (index: '%u') handle.",
              procId
                                    );
    }

  localType = wh_procedure_local_type (mHandler, hProc, localId);

  wh_unit_procedure_release (mHandler, hProc);

  if (localType == NULL)
    {
      throw FunctionalUnitException (
          _EXTRA (0),
          "Cannot obtain the procedure's local value description "
            "(procedure index: %u, local index: %u).",
          procId,
          localId
                                    );
    }

  const uint8_t* localDesc;
  wh_unit_type_descriptors (mHandler, &localDesc);

  assert (localDesc && (localDesc <= localType));

  return localType - localDesc;
}


bool_t
CompiledBufferUnit::IsProcExternal (uint_t procId)
{
  return (ProcCodeAreaSize (procId) == 0);
}


/////////////******CompiledFileUnit********//////////////////////////////


CompiledFileUnit::CompiledFileUnit (const char* file)
  : mFile (file, WH_FILEREAD),
    mGlobalsCount (0),
    mProcsCount (0),
    mTypeAreaSize (0),
    mSymbolsSize (0),
    mTypeInfo (NULL),
    mSymbols (NULL),
    mGlobals (NULL),
    mProcs (NULL),
    mProcData (NULL)
{
  ProcessHeader ();

  mProcData.reset (new uint8_t*[mProcsCount]);

  for (uint_t count = 0; count < mProcsCount; ++count)
    mProcData.get ()[count] = NULL;

}


CompiledFileUnit::~CompiledFileUnit ()
{
  for (uint_t count = 0; count < mProcsCount; ++count)
    delete [] mProcData.get ()[count];
}


void
CompiledFileUnit::ProcessHeader ()
{
  uint8_t  t_buffer[WHC_TABLE_SIZE];
  uint32_t temp32;

  mFile.Seek (0, WH_SEEK_BEGIN);
  mFile.Read (t_buffer, sizeof t_buffer);

  if ((t_buffer[0] != 'W') || (t_buffer[1] != 'O'))
    {
      throw FunctionalUnitException (
                  _EXTRA (0),
                  "File signature does not match a whais compiled object."
                                    );
    }


  mGlobalsCount = load_le_int32 (t_buffer + WHC_GLOBS_COUNT_OFF);
  mProcsCount   = load_le_int32 (t_buffer + WHC_PROCS_COUNT_OFF);

  temp32        = load_le_int32 (t_buffer + WHC_TYPEINFO_START_OFF);
  mTypeAreaSize = load_le_int32 (t_buffer + WHC_TYPEINFO_SIZE_OFF);

  mTypeInfo.reset (new uint8_t[mTypeAreaSize]);
  mFile.Seek (temp32, WH_SEEK_BEGIN);
  mFile.Read (mTypeInfo.get (), mTypeAreaSize);

  temp32       = load_le_int32 (t_buffer + WHC_SYMTABLE_START_OFF);
  mSymbolsSize = load_le_int32 (t_buffer + WHC_SYMTABLE_SIZE_OFF);

  mSymbols.reset (new uint8_t[mSymbolsSize]);
  mFile.Seek (temp32, WH_SEEK_BEGIN);
  mFile.Read (mSymbols.get (), mSymbolsSize);

  temp32         = load_le_int32 (t_buffer + WHC_CONSTAREA_START_OFF);
  mConstAreaSize = load_le_int32 (t_buffer + WHC_CONSTAREA_SIZE_OFF);

  mConstArea.reset (new uint8_t[mConstAreaSize]);
  mFile.Seek (temp32, WH_SEEK_BEGIN);
  mFile.Read (mConstArea.get (), mConstAreaSize);

  temp32 = (mGlobalsCount * WHC_GLOBAL_ENTRY_SIZE) +
             (mProcsCount * WHC_PROC_ENTRY_SIZE);

  mGlobals.reset (new uint8_t[mGlobalsCount * WHC_GLOBAL_ENTRY_SIZE]);
  mProcs.reset (new uint8_t[mProcsCount * WHC_PROC_ENTRY_SIZE]);

  mFile.Seek ((-1 * _SC (int64_t, temp32)), WH_SEEK_END);
  mFile.Read (mGlobals.get (), mGlobalsCount * WHC_GLOBS_COUNT_OFF);
  mFile.Read (mProcs.get (), mProcsCount * WHC_PROC_ENTRY_SIZE);
}


void
CompiledFileUnit::LoadProcInMemory (const uint_t id)
{
  assert (id < mProcsCount);

  if (mProcData.get ()[id] != NULL)
    return;

  const uint8_t* proc     = mProcs.get () + (id * WHC_PROC_ENTRY_SIZE);
  const uint32_t nlocals  = load_le_int16 (proc + WHC_PROC_ENTRY_NLOCAL_OFF);
  const uint32_t codeSize = load_le_int32 (proc + WHC_PROC_ENTRY_CODE_SIZE);
  const uint32_t bodyPos  = load_le_int32 (proc + WHC_PROC_ENTRY_BODY_OFF);
  const uint_t   bodySize = (WHC_PROC_BODY_LOCAL_ENTRY_SIZE * nlocals) +
                              WHC_PROC_BODY_SYNCS_ENTRY_SYZE + codeSize;

  mProcData.get ()[id] = new uint8_t[bodySize];
  mFile.Seek (bodyPos, WH_SEEK_BEGIN);
  mFile.Read (mProcData.get ()[id], bodySize);
}


uint_t CompiledFileUnit::TypeAreaSize ()
{
  return mTypeAreaSize;
}


const uint8_t*
CompiledFileUnit::RetriveTypeArea ()
{
  return mTypeInfo.get ();
}


uint_t CompiledFileUnit::ConstsAreaSize ()
{
  return mConstAreaSize;
}


const uint8_t*
CompiledFileUnit::RetrieveConstArea ()
{
  return mConstArea.get ();
}


uint_t
CompiledFileUnit::GlobalsCount ()
{
  return mGlobalsCount;
}


uint_t
CompiledFileUnit::GlobalNameLength (const uint_t id)
{
  if (id >= mGlobalsCount)
    {
      throw FunctionalUnitException (
                              _EXTRA (0),
                              "Global value index out of range (%d of %d).",
                              id,
                              mGlobalsCount
                                    );
    }

  const uint8_t* const global = mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * id);
  uint32_t             offset = load_le_int32 (global + WHC_GLB_ENTRY_NAME_OFF);

  assert (offset < mSymbolsSize);

  return strlen (_RC (const char *, mSymbols.get ()) + offset);
}


const char*
CompiledFileUnit::RetriveGlobalName (const uint_t id)
{
  if (id >= mGlobalsCount)
    {
      {
        throw FunctionalUnitException (
                                _EXTRA (0),
                                "Global value index out of range (%d of %d).",
                                id,
                                mGlobalsCount
                                      );
      }
    }

  const uint8_t* const global = mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * id);
  uint32_t             offset = load_le_int32 (global + WHC_GLB_ENTRY_NAME_OFF);

  assert (offset < mSymbolsSize);

  return _RC (const char *, mSymbols.get ()) + offset;
}

uint_t
CompiledFileUnit::GlobalTypeOff (const uint_t id)
{
  if (id >= mGlobalsCount)
    {
      throw FunctionalUnitException (
                              _EXTRA (0),
                              "Global value index out of range (%d of %d).",
                              id,
                              mGlobalsCount
                                    );
    }

  const uint8_t *const global = mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * id);
  uint32_t             offset = load_le_int32 (global + WHC_GLB_ENTRY_TYPE_OFF);

  offset &= ~EXTERN_MASK;

  assert (offset < mTypeAreaSize);

  return offset;
}

bool_t
CompiledFileUnit::IsGlobalExternal (const uint_t id)
{
  if (id >= mGlobalsCount)
    {
      throw FunctionalUnitException (
                              _EXTRA (0),
                              "Global value index out of range (%d of %d).",
                              id,
                              mGlobalsCount
                                    );
    }

  const uint8_t *const global = mGlobals.get () + (WHC_GLOBAL_ENTRY_SIZE * id);
  uint32_t             offset = load_le_int32 (global + WHC_GLB_ENTRY_TYPE_OFF);

  return (offset & EXTERN_MASK) != 0;
}

uint_t
CompiledFileUnit::ProceduresCount ()
{
  return mProcsCount;
}

uint_t
CompiledFileUnit::ProcSyncStatementsCount (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  LoadProcInMemory (id);

  const uint8_t *const proc = mProcData.get ()[id];

  return proc[ProcLocalsCount (id) * WHC_PROC_BODY_LOCAL_ENTRY_SIZE +
                WHC_PROC_BODY_SYNCS_ENTRY_SYZE];
}

uint_t
CompiledFileUnit::ProcCodeAreaSize (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  return load_le_int32 (mProcs.get () 			+
                          (id * WHC_PROC_ENTRY_SIZE) 	+
                          WHC_PROC_ENTRY_CODE_SIZE);
}

const uint8_t*
CompiledFileUnit::RetriveProcCodeArea (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  const uint_t nlocals = ProcLocalsCount (id);

  LoadProcInMemory (id);

  return (mProcData.get ()[id] +
            (nlocals * WHC_PROC_BODY_LOCAL_ENTRY_SIZE) +
            WHC_PROC_BODY_SYNCS_ENTRY_SYZE);
}

uint_t
CompiledFileUnit::ProcLocalsCount (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  return load_le_int16 (mProcs.get () +
                          (id * WHC_PROC_ENTRY_SIZE) +
                          WHC_PROC_ENTRY_NLOCAL_OFF);
}

uint_t
CompiledFileUnit::ProcParametersCount (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  return load_le_int16 (mProcs.get () +
                          (id * WHC_PROC_ENTRY_SIZE)
                          + WHC_PROC_ENTRY_NPARMS_OFF);
}

uint_t
CompiledFileUnit::GetProcReturnTypeOff (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  uint_t type = load_le_int32 (mProcs.get () +
                                 (id * WHC_PROC_ENTRY_SIZE) +
                                 WHC_PROC_ENTRY_TYPE_OFF);
  type &= ~EXTERN_MASK;

  assert (type < mTypeAreaSize);

  return type;
}

uint_t
CompiledFileUnit::GetProcNameSize (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  const uint_t offset = load_le_int32 (mProcs.get () +
                                         (id * WHC_PROC_ENTRY_SIZE) +
                                         WHC_PROC_ENTRY_NAME_OFF);
  assert (offset < mSymbolsSize);

  return strlen (_RC (const char*, mSymbols.get () + offset));
}

const char*
CompiledFileUnit::RetriveProcName (const uint_t id)
{
  if (id >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     id,
                                     mProcsCount);
    }

  const uint_t temp32 = load_le_int32 (mProcs.get () +
                                         (id * WHC_PROC_ENTRY_SIZE) +
                                         WHC_PROC_ENTRY_NAME_OFF);
  assert (temp32 < mSymbolsSize);

  return _RC (const char *, mSymbols.get () + temp32);
}

uint_t
CompiledFileUnit::GetProcLocalTypeOff (uint_t procId, uint_t localId)
{
  if (localId == 0)
    return GetProcReturnTypeOff (procId);

  if (localId >= ProcLocalsCount (procId))
    {
      throw FunctionalUnitException (
          _EXTRA (0),
          "Procedure (%d) local index out of range ( %d of %d).",
          procId,
          localId,
          ProcLocalsCount (procId),
          mProcsCount);
    }

  LoadProcInMemory (procId);

  const uint8_t* const entry = mProcData.get ()[procId] +
                                 (localId * WHC_PROC_BODY_LOCAL_ENTRY_SIZE);

  const uint_t offset = load_le_int32 (entry);
  assert (offset < mTypeAreaSize);

  return offset;
}

bool_t
CompiledFileUnit::IsProcExternal (uint_t procId)
{
  if (procId >= mProcsCount)
    {
      throw FunctionalUnitException (_EXTRA (0),
                                     "Procedure index out of range (%d of %d).",
                                     procId,
                                     mProcsCount);
    }

  return (load_le_int32 (mProcs.get () +
                           (procId * WHC_PROC_ENTRY_SIZE) +
                           WHC_PROC_ENTRY_TYPE_OFF) &
           EXTERN_MASK) != 0;
}




FunctionalUnitException::FunctionalUnitException (const uint32_t     code,
                                                  const char*        file,
                                                  const uint32_t     line,
                                                  const char*        fmtMsg,
                                                  ...)
  : Exception (code, file, line)
{
    if (fmtMsg != NULL)
      {
        va_list vl;

        va_start (vl, fmtMsg);
        this->Message (fmtMsg, vl);
        va_end (vl);
      }
}


Exception*
FunctionalUnitException::Clone () const
{
  return new FunctionalUnitException (*this);
}


EXCEPTION_TYPE
FunctionalUnitException::Type () const
{
  return FUNCTIONAL_UNIT_EXCEPTION;
}


const char*
FunctionalUnitException::Description () const
{
  return "A request to a functional unit has failed!";
}



} //namespace whais;

#if defined (ENABLE_MEMORY_TRACE) && defined (USE_COMPILER_SHL)
uint32_t WMemoryTracker::smInitCount = 0;
const char* WMemoryTracker::smModule = "COMPILER";
#endif
