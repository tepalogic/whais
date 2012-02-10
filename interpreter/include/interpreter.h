/******************************************************************************
 PASTRA - A light database one file system and more.
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

#ifndef INTERPRETER_H_
#define INTERPRETER_H_


#include "whisper.h"

#include "dbs/include/dbs_mgr.h"
#include "compiler/include/whisperc/compiledunit.h"


class InterpreterException : public WException
{
public:
  explicit InterpreterException (const D_CHAR*  message,
                                 const D_CHAR*  file,
                                 const D_UINT32 line,
                                 const D_UINT32 extra)
  : WException (message, file, line, extra) {}
  virtual ~InterpreterException () {};

  virtual InterpreterException* Clone () { return new InterpreterException (*this); }
  virtual EXPCEPTION_TYPE       GetType () { return INTERPRETER_EXCEPTION; }
};

class I_InterpreterHandler
{
public:
  I_InterpreterHandler ();
  virtual ~I_InterpreterHandler ();

  virtual void LoadCompiledUnit (WICompiledUnit& unit) = 0;

protected:
};




#endif /* INTERPRETER_H_ */
