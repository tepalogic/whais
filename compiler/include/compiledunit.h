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
#include "utils/include/wfile.h"

#ifndef COMPILEDUNIT_H_
#define COMPILEDUNIT_H_

namespace whisper
{

class COMPILER_SHL WICompiledUnit
{
public:
  WICompiledUnit ()
  {
  }

  virtual ~WICompiledUnit ()
  {
  };

  virtual uint_t         GetTypeInformationSize () = 0;
  virtual const uint8_t* RetriveTypeInformation () = 0;
  virtual uint_t         GetConstAreaSize () = 0;
  virtual const uint8_t* RetrieveConstArea () = 0;

  virtual uint_t         GetGlobalsCount () = 0;
  virtual uint_t         GetGlobalNameLength (uint_t item) = 0;
  virtual const char*  RetriveGlobalName (uint_t item) = 0;
  virtual uint_t         GetGlobalTypeIndex (uint_t item) = 0;
  virtual bool_t         IsGlobalExternal (uint_t item) = 0;

  virtual uint_t         GetProceduresCount () = 0;
  virtual uint_t         GetProcSyncStatementsCount (uint_t item) = 0;
  virtual uint_t         GetProcCodeAreaSize (uint_t item) = 0;
  virtual const uint8_t* RetriveProcCodeArea (uint_t item) = 0;
  virtual uint_t         GetProcLocalsCount (uint_t item) = 0;
  virtual uint_t         GetProcParametersCount (uint_t item) = 0;
  virtual uint_t         GetProcReturnTypeIndex (uint_t proc_item) = 0;
  virtual uint_t         GetProcNameSize (uint_t proc_item) = 0;
  virtual const char*  RetriveProcName (uint_t proc_item) = 0;
  virtual uint_t         GetProcLocalTypeIndex (uint_t item_proc,
                                                uint_t item_local) = 0;
  virtual bool_t         IsProcExternal (uint_t item_proc) = 0;
};

class COMPILER_SHL WCompiledUnitException : public Exception
{
public:
  WCompiledUnitException (const char* pMessage,
                          const char* pFile,
                          uint32_t      line,
                          uint32_t      extra)
    : Exception (pMessage, pFile, line, extra)
  {
  }

  virtual ~WCompiledUnitException ()
  {
  };

  virtual Exception*     Clone () const
  {
    return new WCompiledUnitException (*this);
  }
  virtual EXPCEPTION_TYPE Type () const { return UNIT_COMPILE_EXCEPTION; }
  virtual const char*   Description () const
  {
    return "Could not compile unit!";
  }
};

class COMPILER_SHL WBufferCompiledUnit : public WICompiledUnit
{
public:
  WBufferCompiledUnit (const uint8_t*    pBuffer,
                       uint_t            bufferSize,
                       WHC_MESSENGER     messenger,
                       WHC_MESSENGER_ARG messengerContext);
  virtual ~WBufferCompiledUnit ();

  virtual uint_t         GetTypeInformationSize ();
  virtual const uint8_t* RetriveTypeInformation ();
  virtual uint_t         GetConstAreaSize ();
  virtual const uint8_t* RetrieveConstArea ();

  virtual uint_t        GetGlobalsCount ();
  virtual uint_t        GetGlobalNameLength (uint_t item);
  virtual const char* RetriveGlobalName (uint_t item);
  virtual uint_t        GetGlobalTypeIndex (uint_t item);
  virtual bool_t        IsGlobalExternal (uint_t item);

  virtual uint_t         GetProceduresCount ();
  virtual uint_t         GetProcSyncStatementsCount (uint_t item);
  virtual uint_t         GetProcCodeAreaSize (uint_t item);
  virtual const uint8_t* RetriveProcCodeArea (uint_t item);
  virtual uint_t         GetProcLocalsCount (uint_t item);
  virtual uint_t         GetProcParametersCount (uint_t item);
  virtual uint_t         GetProcReturnTypeIndex (uint_t proc_item);
  virtual uint_t         GetProcNameSize (uint_t proc_item);
  virtual const char*  RetriveProcName (uint_t proc_item);
  virtual uint_t         GetProcLocalTypeIndex (uint_t item_proc,
                                                uint_t item_local);
  virtual bool_t         IsProcExternal (uint_t item_proc);

private:
    WBufferCompiledUnit (WBufferCompiledUnit&);
    WBufferCompiledUnit& operator= (WBufferCompiledUnit&);

private:
    WHC_HANDLER m_Handler;
};

class COMPILER_SHL WFileCompiledUnit : public WICompiledUnit
{

public:
  explicit WFileCompiledUnit (const char* pFileName);
  virtual ~WFileCompiledUnit ();

  virtual uint_t         GetTypeInformationSize ();
  virtual const uint8_t* RetriveTypeInformation ();

  virtual uint_t         GetGlobalsCount ();
  virtual uint_t         GetGlobalNameLength (uint_t item);
  virtual const char*  RetriveGlobalName (uint_t item);
  virtual uint_t         GetGlobalTypeIndex (uint_t item);
  virtual bool_t         IsGlobalExternal (uint_t item);
  virtual uint_t         GetConstAreaSize ();
  virtual const uint8_t* RetrieveConstArea ();

  virtual uint_t         GetProceduresCount ();
  virtual uint_t         GetProcSyncStatementsCount (uint_t item);
  virtual uint_t         GetProcCodeAreaSize (uint_t item);
  virtual const uint8_t* RetriveProcCodeArea (uint_t item);
  virtual uint_t         GetProcLocalsCount (uint_t item);
  virtual uint_t         GetProcParametersCount (uint_t item);
  virtual uint_t         GetProcReturnTypeIndex (uint_t proc_item);
  virtual uint_t         GetProcNameSize (uint_t proc_item);
  virtual const char*  RetriveProcName (uint_t proc_item);
  virtual uint_t         GetProcLocalTypeIndex (uint_t item_proc,
                                                uint_t item_local);
  virtual bool_t         IsProcExternal (uint_t item_proc);

private:
  WFileCompiledUnit (WFileCompiledUnit&);
  WFileCompiledUnit& operator= (WFileCompiledUnit&);

  void ProcessHeader ();
  void LoadProcInMemory (uint_t proc_item);

private:
#pragma warning (disable: 4251)
  WFile    m_File;
#pragma warning (default: 4251)
  uint32_t m_GlobalsCount;
  uint32_t m_ProcsCount;
  uint32_t m_TtySize;
  uint32_t m_SymbolsSize;
  uint32_t m_ConstAreaSize;

#pragma warning (disable: 4251)
  //Ignore this warning at the STL auto_ptr does not
  //have any static memebers
  std::auto_ptr<uint8_t>  m_TypeInfo;
  std::auto_ptr<uint8_t>  m_Symbols;
  std::auto_ptr<uint8_t>  m_ConstArea;
  std::auto_ptr<uint8_t>  m_Globals;
  std::auto_ptr<uint8_t>  m_Procs;
  std::auto_ptr<uint8_t*> m_ProcData;
#pragma warning (default: 4251)
};

} //namespace whisper

#endif /* COMPILEDUNIT_H_ */
