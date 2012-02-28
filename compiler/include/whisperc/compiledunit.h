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

class WICompiledUnit
{
public:
  virtual ~ WICompiledUnit ()
  {
  };

  virtual D_UINT GetTypeInformationSize () = 0;
  virtual const D_UINT8 *RetriveTypeInformation () = 0;
  virtual D_UINT GetConstAreaSize () = 0;
  virtual const D_UINT8 *RetrieveConstArea () = 0;

  virtual D_UINT GetGlobalsCount () = 0;
  virtual D_UINT GetGlobalNameLength (D_UINT item) = 0;
  virtual const D_CHAR* RetriveGlobalName (D_UINT item) = 0;
  virtual D_UINT GetGlobalTypeIndex (D_UINT item) = 0;
  virtual D_BOOL IsGlobalExternal (D_UINT item) = 0;

  virtual D_UINT GetProceduresCount () = 0;
  virtual D_UINT GetProcSyncStatementsCount (D_UINT item) = 0;
  virtual D_UINT GetProcCodeAreaSize (D_UINT item) = 0;
  virtual const D_UINT8 *RetriveProcCodeArea (D_UINT item) = 0;
  virtual D_UINT GetProcLocalsCount (D_UINT item) = 0;
  virtual D_UINT GetProcParametersCount (D_UINT item) = 0;
  virtual D_UINT GetProcReturnTypeIndex (D_UINT proc_item) = 0;
  virtual D_UINT GetProcNameSize (D_UINT proc_item) = 0;
  virtual const D_CHAR *RetriveProcName (D_UINT proc_item) = 0;
  virtual D_UINT GetProcLocalTypeIndex (D_UINT item_proc,
					D_UINT item_local) = 0;
  virtual D_BOOL IsProcExternal (D_UINT item_proc) = 0;
};

class WCompiledUnitException:public WException
{
public:
  WCompiledUnitException (const D_CHAR *message,
      const D_CHAR *file, D_UINT32 line, D_UINT32 extra)
  : WException (message, file, line, extra) {}
  virtual ~ WCompiledUnitException () {};

  virtual WException*     Clone () { return new WCompiledUnitException (*this); }
  virtual EXPCEPTION_TYPE GetType () { return UNIT_COMPILE_EXCEPTION; }
};

class WBufferCompiledUnit:public WICompiledUnit
{
public:
  WBufferCompiledUnit (const D_UINT8 * buffer,
		       D_UINT buffer_size,
		       WHC_MESSENGER callback, WHC_MESSENGER_ARG context);
  virtual ~ WBufferCompiledUnit ();

  virtual D_UINT GetTypeInformationSize ();
  virtual const D_UINT8 *RetriveTypeInformation ();
  virtual D_UINT GetConstAreaSize ();
  virtual const D_UINT8 *RetrieveConstArea ();

  virtual D_UINT GetGlobalsCount ();
  virtual D_UINT GetGlobalNameLength (D_UINT item);
  virtual const D_CHAR *RetriveGlobalName (D_UINT item);
  virtual D_UINT GetGlobalTypeIndex (D_UINT item);
  virtual D_BOOL IsGlobalExternal (D_UINT item);

  virtual D_UINT GetProceduresCount ();
  virtual D_UINT GetProcSyncStatementsCount (D_UINT item);
  virtual D_UINT GetProcCodeAreaSize (D_UINT item);
  virtual const D_UINT8 *RetriveProcCodeArea (D_UINT item);
  virtual D_UINT GetProcLocalsCount (D_UINT item);
  virtual D_UINT GetProcParametersCount (D_UINT item);
  virtual D_UINT GetProcReturnTypeIndex (D_UINT proc_item);
  virtual D_UINT GetProcNameSize (D_UINT proc_item);
  virtual const D_CHAR *RetriveProcName (D_UINT proc_item);
  virtual D_UINT GetProcLocalTypeIndex (D_UINT item_proc, D_UINT item_local);
  virtual D_BOOL IsProcExternal (D_UINT item_proc);

private:
    WBufferCompiledUnit (WBufferCompiledUnit &);
    WBufferCompiledUnit & operator= (WBufferCompiledUnit &);

private:
    WHC_HANDLER mHandler;
};

class WFileCompiledUnit : public WICompiledUnit
{

public:
  explicit WFileCompiledUnit (const D_CHAR * file_name);
  virtual ~ WFileCompiledUnit ();

  virtual D_UINT GetTypeInformationSize ();
  virtual const D_UINT8 *RetriveTypeInformation ();

  virtual D_UINT GetGlobalsCount ();
  virtual D_UINT GetGlobalNameLength (D_UINT item);
  virtual const D_CHAR *RetriveGlobalName (D_UINT item);
  virtual D_UINT GetGlobalTypeIndex (D_UINT item);
  virtual D_BOOL IsGlobalExternal (D_UINT item);
  virtual D_UINT GetConstAreaSize ();
  virtual const D_UINT8 *RetrieveConstArea ();

  virtual D_UINT GetProceduresCount ();
  virtual D_UINT GetProcSyncStatementsCount (D_UINT item);
  virtual D_UINT GetProcCodeAreaSize (D_UINT item);
  virtual const D_UINT8 *RetriveProcCodeArea (D_UINT item);
  virtual D_UINT GetProcLocalsCount (D_UINT item);
  virtual D_UINT GetProcParametersCount (D_UINT item);
  virtual D_UINT GetProcReturnTypeIndex (D_UINT proc_item);
  virtual D_UINT GetProcNameSize (D_UINT proc_item);
  virtual const D_CHAR *RetriveProcName (D_UINT proc_item);
  virtual D_UINT GetProcLocalTypeIndex (D_UINT item_proc, D_UINT item_local);
  virtual D_BOOL IsProcExternal (D_UINT item_proc);

private:
  typedef D_UINT8 *PD_UINT8;

  WFileCompiledUnit (WFileCompiledUnit &);
  WFileCompiledUnit & operator= (WFileCompiledUnit &);

  void ProcessHeader ();
  void LoadProcInMemory (D_UINT proc_item);

private:
  WFile mFile;
  D_UINT32 mGlobalsCount;
  D_UINT32 mProcsCount;
  D_UINT32 mTtySize;
  D_UINT32 mSymbolsSize;
  D_UINT32 mConstAreaSize;

  std::auto_ptr < D_UINT8 > mTypeInfo;
  std::auto_ptr < D_UINT8 > mSymbols;
  std::auto_ptr < D_UINT8 > mConstArea;
  std::auto_ptr < D_UINT8 > mGlobals;
  std::auto_ptr < D_UINT8 > mProcs;
  std::auto_ptr < PD_UINT8 > mProcData;
};

#endif /* COMPILEDUNIT_H_ */
