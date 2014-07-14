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

#include "whisper.h"

#include "whisperc.h"
#include "utils/wfile.h"

#ifndef COMPILEDUNIT_H_
#define COMPILEDUNIT_H_



namespace whisper
{



class COMPILER_SHL WIFunctionalUnit
{
public:
  virtual ~WIFunctionalUnit ();

  virtual uint_t         TypeAreaSize () = 0;
  virtual const uint8_t* RetriveTypeArea () = 0;
  virtual uint_t         ConstsAreaSize () = 0;
  virtual const uint8_t* RetrieveConstArea () = 0;

  virtual uint_t         GlobalsCount () = 0;
  virtual uint_t         GlobalNameLength (const uint_t id) = 0;
  virtual const char*    RetriveGlobalName (const uint_t id) = 0;
  virtual uint_t         GlobalTypeOff (const uint_t id) = 0;
  virtual bool_t         IsGlobalExternal (const uint_t id) = 0;

  virtual uint_t         ProceduresCount () = 0;
  virtual uint_t         ProcSyncStatementsCount (const uint_t id) = 0;
  virtual uint_t         ProcCodeAreaSize (const uint_t id) = 0;
  virtual const uint8_t* RetriveProcCodeArea (const uint_t id) = 0;
  virtual uint_t         ProcLocalsCount (const uint_t id) = 0;
  virtual uint_t         ProcParametersCount (const uint_t id) = 0;
  virtual uint_t         GetProcReturnTypeOff (const uint_t id) = 0;
  virtual uint_t         GetProcNameSize (const uint_t id) = 0;
  virtual const char*    RetriveProcName (const uint_t id) = 0;
  virtual uint_t         GetProcLocalTypeOff (uint_t procId,
                                              uint_t localId) = 0;
  virtual bool_t         IsProcExternal (uint_t procId) = 0;
};



class COMPILER_SHL FunctionalUnitException : public Exception
{
public:
  FunctionalUnitException (const uint32_t     code,
                           const char*        file,
                           const uint32_t     line,
                           const char*        fmtMsg = NULL,
                           ...);

  virtual Exception* Clone () const;

  virtual EXCEPTION_TYPE Type () const;

  virtual const char* Description () const;
};


class COMPILER_SHL CompiledBufferUnit : public WIFunctionalUnit
{
public:
  CompiledBufferUnit (const uint8_t*    buffer,
                      uint_t            bufferSize,
                      WH_MESSENGER      messenger,
                      WH_MESSENGER_CTXT messengerContext);
  virtual ~CompiledBufferUnit ();

  virtual uint_t         TypeAreaSize ();
  virtual const uint8_t* RetriveTypeArea ();
  virtual uint_t         ConstsAreaSize ();
  virtual const uint8_t* RetrieveConstArea ();

  virtual uint_t        GlobalsCount ();
  virtual uint_t        GlobalNameLength (const uint_t id);
  virtual const char*   RetriveGlobalName (const uint_t id);
  virtual uint_t        GlobalTypeOff (const uint_t id);
  virtual bool_t        IsGlobalExternal (const uint_t id);

  virtual uint_t         ProceduresCount ();
  virtual uint_t         ProcSyncStatementsCount (const uint_t id);
  virtual uint_t         ProcCodeAreaSize (const uint_t id);
  virtual const uint8_t* RetriveProcCodeArea (const uint_t id);
  virtual uint_t         ProcLocalsCount (const uint_t id);
  virtual uint_t         ProcParametersCount (const uint_t id);
  virtual uint_t         GetProcReturnTypeOff (const uint_t id);
  virtual uint_t         GetProcNameSize (const uint_t id);
  virtual const char*    RetriveProcName (const uint_t id);
  virtual uint_t         GetProcLocalTypeOff (uint_t procId, uint_t localId);
  virtual bool_t         IsProcExternal (uint_t procId);

private:
    CompiledBufferUnit (CompiledBufferUnit&);
    CompiledBufferUnit& operator= (CompiledBufferUnit&);

private:
    WH_COMPILED_UNIT mHandler;
};



class COMPILER_SHL CompiledFileUnit : public WIFunctionalUnit
{

public:
  explicit CompiledFileUnit (const char* file);
  virtual ~CompiledFileUnit ();

  virtual uint_t         TypeAreaSize ();
  virtual const uint8_t* RetriveTypeArea ();

  virtual uint_t         GlobalsCount ();
  virtual uint_t         GlobalNameLength (const uint_t id);
  virtual const char*    RetriveGlobalName (const uint_t id);
  virtual uint_t         GlobalTypeOff (const uint_t id);
  virtual bool_t         IsGlobalExternal (const uint_t id);
  virtual uint_t         ConstsAreaSize ();
  virtual const uint8_t* RetrieveConstArea ();

  virtual uint_t         ProceduresCount ();
  virtual uint_t         ProcSyncStatementsCount (const uint_t id);
  virtual uint_t         ProcCodeAreaSize (const uint_t id);
  virtual const uint8_t* RetriveProcCodeArea (const uint_t id);
  virtual uint_t         ProcLocalsCount (const uint_t id);
  virtual uint_t         ProcParametersCount (const uint_t id);
  virtual uint_t         GetProcReturnTypeOff (const uint_t id);
  virtual uint_t         GetProcNameSize (const uint_t id);
  virtual const char*    RetriveProcName (const uint_t id);
  virtual uint_t         GetProcLocalTypeOff (uint_t procId, uint_t localId);
  virtual bool_t         IsProcExternal (uint_t procId);

private:
  CompiledFileUnit (CompiledFileUnit&);
  CompiledFileUnit& operator= (CompiledFileUnit&);

  void ProcessHeader ();
  void LoadProcInMemory (const uint_t id);

private:
#pragma warning (disable: 4251)
  File     mFile;
#pragma warning (default: 4251)
  uint32_t mGlobalsCount;
  uint32_t mProcsCount;
  uint32_t mTypeAreaSize;
  uint32_t mSymbolsSize;
  uint32_t mConstAreaSize;

#pragma warning (disable: 4251)
  //Ignore this warning at the STL auto_ptr does not
  //have any static memebers
  std::auto_ptr<uint8_t>  mTypeInfo;
  std::auto_ptr<uint8_t>  mSymbols;
  std::auto_ptr<uint8_t>  mConstArea;
  std::auto_ptr<uint8_t>  mGlobals;
  std::auto_ptr<uint8_t>  mProcs;
  std::auto_ptr<uint8_t*> mProcData;
#pragma warning (default: 4251)
};



} //namespace whisper



#endif /* COMPILEDUNIT_H_ */

