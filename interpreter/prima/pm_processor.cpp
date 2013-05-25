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

#include <assert.h>

#include "compiler/wopcodes.h"
#include "utils/le_converter.h"

#include "pm_processor.h"



namespace whisper {
namespace prima {


static void
op_func_ldnull (Processor& processor, int64_t&)
{
  processor.GetStack ().Push ();
}


static void
op_func_ldc (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  DChar value (load_le_int32 (data));

  processor.GetStack ().Push (value);

  offset += sizeof (uint32_t);
}


static void
op_func_ldi8 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  DUInt8 value (*data);

  processor.GetStack ().Push (value);

  offset += sizeof (uint8_t);
}


static void
op_func_ldi16 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  DUInt16 value (load_le_int16 (data));

  processor.GetStack ().Push (value);

  offset += sizeof (uint16_t);
}


static void
op_func_ldi32 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  DUInt32 value (load_le_int32 (data));

  processor.GetStack ().Push (value);

  offset += sizeof (uint32_t);
}


static void
op_func_ldi64 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  DUInt64 value (load_le_int64 (data));

  processor.GetStack ().Push (value);

  offset += sizeof (uint64_t);
}


static void
op_func_ldd (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  const uint8_t day   = data[0];
  const uint8_t month = data[1];
  const int16_t year  = load_le_int16 (data + 2);

  DDate value (year, month, day);

  processor.GetStack ().Push (value);

  offset += 2 + sizeof (uint16_t);
}


static void
op_func_lddt (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const uint8_t sec   = data[0];
  const uint8_t min   = data[1];
  const uint8_t hour  = data[2];
  const uint8_t day   = data[3];
  const uint8_t month = data[4];
  const int16_t year  = load_le_int16 (data + 5);

  DDateTime value (year, month, day, hour, min, sec);

  processor.GetStack ().Push (value);

  offset += 5 + sizeof (uint16_t);
}


static void
op_func_ldht (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const uint32_t usec  = load_le_int32 (data);
  const uint8_t  sec   = data[4];
  const uint8_t  min   = data[5];
  const uint8_t  hour  = data[6];
  const uint8_t  day   = data[7];
  const uint8_t  month = data[8];
  const int16_t  year  = load_le_int16 (data + 9);

  DHiresTime value (year, month, day, hour, min, sec, usec);

  processor.GetStack ().Push (value);

  offset += sizeof (uint32_t) + 5 + sizeof (uint16_t);
}


static void
op_func_ldrr (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  int64_t intPart  = load_le_int64 (data);
  int64_t fracPart = load_le_int64 (data + sizeof (uint64_t));

  const RICHREAL_T basicValue (intPart, fracPart, W_LDRR_PRECISSION);

  DRichReal value (basicValue);
  processor.GetStack ().Push (value);

  offset += (sizeof (uint64_t) + sizeof (uint64_t));
}


static void
op_func_ldt (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  const uint32_t textOff = load_le_int32 (data);
  const uint8_t* text    = processor.GetUnit ().GetConstData (textOff);

  assert (text != NULL);

  DText value (text);

  processor.GetStack ().Push (value);

  offset += sizeof (uint32_t);
}


static void
op_func_ldbt (Processor& processor, int64_t& offset)
{
  DBool value (true);

  processor.GetStack ().Push (value);
}


static void
op_func_ldbf (Processor& processor, int64_t& offset)
{
  DBool value (false);

  processor.GetStack ().Push (value);
}


static void
op_func_ldlo8 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const uint8_t localIndex = *data;

  assert (localIndex < processor.LocalsCount ());

  LocalOperand localOp (processor.GetStack(),
                        processor.StackBegin () + localIndex);

  processor.GetStack ().Push (StackValue (localOp));

  offset += sizeof (uint8_t);
}


static void
op_func_ldlo16 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const uint16_t localIndex = load_le_int16 (data);

  assert (localIndex < processor.LocalsCount ());

  LocalOperand localOp (processor.GetStack(),
                        processor.StackBegin () + localIndex);

  processor.GetStack ().Push (StackValue (localOp));

  offset += sizeof (uint16_t);
}


static void
op_func_ldlo32 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const uint32_t localIndex = load_le_int32 (data);

  assert (localIndex < processor.LocalsCount ());

  LocalOperand localOp (processor.GetStack(),
                        processor.StackBegin () + localIndex);

  processor.GetStack ().Push (StackValue (localOp));

  offset += sizeof (uint32_t);
}


static void
op_func_ldgb8 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  const uint8_t  globalIndex = *data;
  const uint32_t glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (uint8_t);
}


static void
op_func_ldgb16 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  const uint16_t globalIndex = load_le_int16 (data);
  const uint32_t glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (uint16_t);
}


static void
op_func_ldgb32 (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  const uint32_t globalIndex = load_le_int32 (data);
  const uint32_t glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (uint32_t);
}


static void
op_func_cts (Processor& processor, int64_t& offset)
{
  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          processor.GetStack ().Size ());

  processor.GetStack ().Pop (1);
}


template <class T> static void
op_func_stXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& src  = stack[stackSize - 1].Operand ();
  IOperand& dest = stack[stackSize - 2].Operand ();

  T value;

  src.GetValue (value);
  dest.SetValue (value);

  stack.Pop (1);
}


static void
op_func_stta (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () -1 + 2) <=
          stackSize);

  BaseOperand& src  = _SC (BaseOperand&, stack[stackSize - 1].Operand ());
  BaseOperand& dest = _SC (BaseOperand&, stack[stackSize - 2].Operand ());

  dest.CopyTableOp (src.GetTableOp());

  stack.Pop (1);
}


static void
op_func_stf (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  BaseOperand& src  = _SC (BaseOperand&, stack[stackSize - 1].Operand ());
  BaseOperand& dest = _SC (BaseOperand&, stack[stackSize - 2].Operand ());

  dest.CopyFieldOp (src.GetFieldOp());

  stack.Pop (1);
}


static void
op_func_inull (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
           stackSize);

  IOperand& source = stack[stackSize - 1].Operand ();
  DBool result (source.IsNull ());

  stack.Pop (1);
  stack.Push (result);
}


static void
op_func_nnull (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
           stackSize);

  IOperand& source = stack[stackSize - 1].Operand ();
  DBool result (! source.IsNull ());

  stack.Pop (1);
  stack.Push (result);
}


static void
op_func_call (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  const uint32_t procId = load_le_int32 (data);

  offset += sizeof (uint32_t);

  Session&      session = processor.GetSession ();
  SessionStack& stack   = processor.GetStack ();

  Processor procedure (session, stack, procId);

  procedure.Run ();
}


static void
op_func_ret (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <= stackSize);

  StackValue result = stack[stackSize - 1].Operand().Duplicate ();

  stack.Pop (stackSize - processor.StackBegin());
  stack.Push (result);

  offset = processor.CodeSize (); //Signal the procedure return
}


template <class DBS_T> static void
op_func_addXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DBS_T (firstOp.mValue + secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


static void
op_func_addt (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DText firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DText secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DText result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    {
      result = firstOp;
      result.Append (secondOp);
    }

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_andXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DBS_T (firstOp.mValue & secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_divXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  if (secondOp == DBS_T (0))
    throw InterException (NULL, _EXTRA (InterException::DIVIDE_BY_ZERO));

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DBS_T (firstOp.mValue / secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_eqXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBool result (firstOp == secondOp);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_geXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBool result ((firstOp < secondOp) == false);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_gtXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBool result (((firstOp < secondOp) || (firstOp == secondOp)) == false);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_leXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBool result ((firstOp < secondOp) || (firstOp == secondOp));

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_ltXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBool result (firstOp < secondOp);

  stack.Pop (2);
  stack.Push (result);
}


static void
op_func_mod (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DInt64 firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DInt64 secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  if (secondOp == DInt64 (0))
    throw InterException (NULL, _EXTRA (InterException::DIVIDE_BY_ZERO));

  DInt64 result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DInt64 (firstOp.mValue % secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_mulXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DBS_T (firstOp.mValue * secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_neXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBool result ((firstOp == secondOp) == false);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_notXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DBS_T operand;
  stack[stackSize - 1].Operand ().GetValue (operand);

  DBS_T result;
  if (operand.IsNull () == false)
    result = DBS_T (~operand.mValue);

  stack.Pop (1);
  stack.Push (result);
}


template <> void
op_func_notXX<DBool> (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DBool operand;
  stack[stackSize - 1].Operand ().GetValue (operand);

  DBool result;
  if (operand.IsNull () == false)
    result = DBool (! operand.mValue);

  stack.Pop (1);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_orXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DBS_T (firstOp.mValue | secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_subXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DBS_T (firstOp.mValue - secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_xorXX (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  DBS_T firstOp;
  stack[stackSize - 2].Operand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].Operand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;

  else if (secondOp.IsNull ())
    result = firstOp;

  else
    result = DBS_T (firstOp.mValue ^ secondOp.mValue);

  stack.Pop (2);
  stack.Push (result);
}


static void
op_func_jf (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DBool firstOp;
  stack[stackSize - 1].Operand ().GetValue (firstOp);

  if ((firstOp.IsNull() == false) && (firstOp.mValue == false))
    {
      const uint8_t* const data = processor.Code () +
                                    processor.CurrentOffset () +
                                    offset;

      const int32_t jmpOffset = _SC (int32_t, load_le_int32 (data));

      offset = jmpOffset;
    }
  else
    offset += sizeof (uint32_t);
}


static void
op_func_jfc (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DBool firstOp;
  stack[stackSize - 1].Operand ().GetValue (firstOp);
  stack.Pop (1);

  if ((firstOp.IsNull() == false) && (firstOp.mValue == false))
    {
      const uint8_t* const data = processor.Code () +
                                    processor.CurrentOffset () +
                                    offset;

      const int32_t jmpOffset = _SC (int32_t, load_le_int32 (data));
      offset = jmpOffset;
    }
  else
    offset += sizeof (uint32_t);
}


static void
op_func_jt (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DBool firstOp;
  stack[stackSize - 1].Operand ().GetValue (firstOp);

  if ((firstOp.IsNull() == false) && firstOp.mValue )
    {
      const uint8_t* const data = processor.Code () +
                                    processor.CurrentOffset () +
                                    offset;

      const int32_t jmpOffset = _SC (int32_t, load_le_int32 (data));
      offset = jmpOffset;
    }
  else
    offset += sizeof (uint32_t);
}


static void
op_func_jtc (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DBool firstOp;
  stack[stackSize - 1].Operand ().GetValue (firstOp);
  stack.Pop (1);

  if ((firstOp.IsNull() == false) && firstOp.mValue)
    {
      const uint8_t* const data = processor.Code () +
                                    processor.CurrentOffset () +
                                    offset;

      const int32_t jmpOffset = _SC (int32_t, load_le_int32 (data));
      offset = jmpOffset;
    }
  else
    offset += sizeof (uint32_t);
}


static void
op_func_jmp (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;

  const int32_t jmpOffset = _SC (int32_t, load_le_int32 (data));
  offset = jmpOffset;
}


template <uint_t EXCEPTION_CODE> void
op_func_ind (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DUInt64 index;
  stack[stackSize - 1].Operand ().GetValue (index);

  if (index.IsNull ())
    throw InterException (NULL, _EXTRA (EXCEPTION_CODE));

  StackValue result = stack[stackSize - 2].Operand ().GetValueAt (index.mValue);

  stack.Pop (2);
  stack.Push (result);
}


static void
op_func_indta (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  DUInt64 index;
  stack[stackSize - 1].Operand ().GetValue (index);

  if (index.IsNull ())
    throw InterException (NULL, _EXTRA (InterException::ROW_INDEX_NULL));

  BaseOperand& op = _SC (BaseOperand&, stack[stackSize - 2].Operand ());
  TableOperand tableOp = op.GetTableOp ();

  const uint8_t* const  pData = processor.Code () +
                                  processor.CurrentOffset () +
                                  offset;
  const uint32_t       textOff = load_le_int32 (pData);
  const uint8_t* const text    = processor.GetUnit ().GetConstData (textOff);

  offset += sizeof (uint32_t);

  FIELD_INDEX field = tableOp.GetTable ().RetrieveField (_RC (const char*,
                                                              text));
  FieldOperand fieldOp (tableOp, field);
  StackValue   result = fieldOp.GetValueAt (index.mValue);

  stack.Pop (2);
  stack.Push (result);
}


static void
op_func_self (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 1) <=
          stackSize);

  BaseOperand& op = _SC (BaseOperand&, stack[stackSize - 1].Operand ());
  TableOperand tableOp = op.GetTableOp ();

  const uint8_t* const  data = processor.Code () +
                                 processor.CurrentOffset () +
                                 offset;
  const uint32_t        textOff = load_le_int32 (data);
  const uint8_t* const  text    = processor.GetUnit ().GetConstData (textOff);

  offset += sizeof (uint32_t);

  FIELD_INDEX field = tableOp.GetTable ().RetrieveField (_RC (const char*,
                                                              text));
  FieldOperand fieldOp (tableOp, field);
  StackValue   result (fieldOp);

  stack.Pop (1);
  stack.Push (result);
}


static void
op_func_bsync (Processor& processor, int64_t& offset)
{
  const uint8_t* const  data = processor.Code () +
                                 processor.CurrentOffset () +
                               offset;
  const uint8_t syncStmt = *data;

  offset += sizeof (uint8_t);

  processor.AquireSync (syncStmt);
}


static void
op_func_esync (Processor& processor, int64_t& offset)
{
  const uint8_t* const data = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const uint8_t syncStmt = *data;

  offset += sizeof (uint8_t);

  processor.ReleaseSync (syncStmt);
}


template <class DBS_T> static void
op_func_sadd (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfAdd (delta);

  stack.Pop (1);
}


template <class DBS_T> static void
op_func_ssub (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfSub (delta);

  stack.Pop (1);
}


template <class DBS_T> static void
op_func_smul (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfMul (delta);

  stack.Pop (1);
}


template <class DBS_T> static void
op_func_sdiv (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfDiv (delta);

  stack.Pop (1);
}


template <class DBS_T> static void
op_func_smod (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfMod (delta);

  stack.Pop (1);
}


template <class DBS_T> static void
op_func_sand (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfAnd (delta);

  stack.Pop (1);
}


template <class DBS_T> static void
op_func_sxor (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfXor (delta);

  stack.Pop (1);
}


template <class DBS_T> static void
op_func_sor (Processor& processor, int64_t& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount () - 1 + 2) <=
          stackSize);

  IOperand& destOp = stack[stackSize - 2].Operand ();

  DBS_T delta;
  stack[stackSize - 1].Operand ().GetValue (delta);

  destOp.SelfOr (delta);

  stack.Pop (1);
}



typedef void (*OP_FUNC) (Processor& processor, int64_t& ioOffset);



static OP_FUNC operations[] = {
                                NULL,
                                op_func_ldnull,
                                op_func_ldc,
                                op_func_ldi8,
                                op_func_ldi16,
                                op_func_ldi32,
                                op_func_ldi64,
                                op_func_ldd,
                                op_func_lddt,
                                op_func_ldht,
                                op_func_ldrr,
                                op_func_ldt,
                                op_func_ldbt,
                                op_func_ldbf,
                                op_func_ldlo8,
                                op_func_ldlo16,
                                op_func_ldlo32,
                                op_func_ldgb8,
                                op_func_ldgb16,
                                op_func_ldgb32,
                                op_func_cts,

                                op_func_stXX<DBool>,
                                op_func_stXX<DChar>,
                                op_func_stXX<DDate>,
                                op_func_stXX<DDateTime>,
                                op_func_stXX<DHiresTime>,
                                op_func_stXX<DInt8>,
                                op_func_stXX<DInt16>,
                                op_func_stXX<DInt32>,
                                op_func_stXX<DInt64>,
                                op_func_stXX<DReal>,
                                op_func_stXX<DRichReal>,
                                op_func_stXX<DText>,
                                op_func_stXX<DUInt8>,
                                op_func_stXX<DUInt16>,
                                op_func_stXX<DUInt32>,
                                op_func_stXX<DUInt64>,
                                op_func_stta,
                                op_func_stf,
                                op_func_stXX<DArray>,

                                op_func_inull,
                                op_func_nnull,

                                op_func_call,
                                op_func_ret,

                                op_func_addXX<DInt64>,
                                op_func_addXX<DReal>,
                                op_func_addXX<DRichReal>,
                                op_func_addt,

                                op_func_andXX<DInt64>,
                                op_func_andXX<DBool>,

                                op_func_divXX<DInt64>,
                                op_func_divXX<DReal>,
                                op_func_divXX<DRichReal>,

                                op_func_eqXX<DInt64>,
                                op_func_eqXX<DBool>,
                                op_func_eqXX<DChar>,
                                op_func_eqXX<DDate>,
                                op_func_eqXX<DDateTime>,
                                op_func_eqXX<DHiresTime>,
                                op_func_eqXX<DReal>,
                                op_func_eqXX<DRichReal>,
                                op_func_eqXX<DText>,

                                op_func_geXX<DInt64>,
                                op_func_geXX<DChar>,
                                op_func_geXX<DDate>,
                                op_func_geXX<DDateTime>,
                                op_func_geXX<DHiresTime>,
                                op_func_geXX<DReal>,
                                op_func_geXX<DRichReal>,

                                op_func_gtXX<DInt64>,
                                op_func_gtXX<DChar>,
                                op_func_gtXX<DDate>,
                                op_func_gtXX<DDateTime>,
                                op_func_gtXX<DHiresTime>,
                                op_func_gtXX<DReal>,
                                op_func_gtXX<DRichReal>,

                                op_func_leXX<DInt64>,
                                op_func_leXX<DChar>,
                                op_func_leXX<DDate>,
                                op_func_leXX<DDateTime>,
                                op_func_leXX<DHiresTime>,
                                op_func_leXX<DReal>,
                                op_func_leXX<DRichReal>,

                                op_func_ltXX<DInt64>,
                                op_func_ltXX<DChar>,
                                op_func_ltXX<DDate>,
                                op_func_ltXX<DDateTime>,
                                op_func_ltXX<DHiresTime>,
                                op_func_ltXX<DReal>,
                                op_func_ltXX<DRichReal>,

                                op_func_mod,

                                op_func_mulXX<DInt64>,
                                op_func_mulXX<DReal>,
                                op_func_mulXX<DRichReal>,

                                op_func_neXX<DInt64>,
                                op_func_neXX<DBool>,
                                op_func_neXX<DChar>,
                                op_func_neXX<DDate>,
                                op_func_neXX<DDateTime>,
                                op_func_neXX<DHiresTime>,
                                op_func_neXX<DReal>,
                                op_func_neXX<DRichReal>,
                                op_func_neXX<DText>,

                                op_func_notXX<DInt64>,
                                op_func_notXX<DBool>,

                                op_func_orXX<DInt64>,
                                op_func_orXX<DBool>,

                                op_func_subXX<DInt64>,
                                op_func_subXX<DReal>,
                                op_func_subXX<DRichReal>,

                                op_func_xorXX<DInt64>,
                                op_func_xorXX<DBool>,

                                op_func_jf,
                                op_func_jfc,
                                op_func_jt,
                                op_func_jtc,
                                op_func_jmp,

                                op_func_ind<InterException::TEXT_INDEX_NULL>,
                                op_func_ind<InterException::ARRAY_INDEX_NULL>,
                                op_func_ind<InterException::ROW_INDEX_NULL>,
                                op_func_indta,
                                op_func_self,

                                op_func_bsync,
                                op_func_esync,

                                op_func_sadd<DInt64>,
                                op_func_sadd<DRichReal>,
                                op_func_sadd<DChar>,
                                op_func_sadd<DText>,

                                op_func_ssub<DInt64>,
                                op_func_ssub<DRichReal>,

                                op_func_smul<DInt64>,
                                op_func_smul<DRichReal>,

                                op_func_sdiv<DInt64>,
                                op_func_sdiv<DRichReal>,

                                op_func_smod<DInt64>,

                                op_func_sand<DInt64>,
                                op_func_sand<DBool>,

                                op_func_sxor<DInt64>,
                                op_func_sxor<DBool>,

                                op_func_sor<DInt64>,
                                op_func_sor<DBool>
                              };



Processor::Processor (Session&          session,
                      SessionStack&     stack,
                      const uint_t      procedureId)
  : mSession (session),
    mStack (stack),
    mProcUnit (mSession.ProcUnit (procedureId)),
    mCode (mSession.ProcCode (procedureId)),
    mCodeSize (mSession.ProcCodeSize (procedureId)),
    mCodePos (0),
    mLocalsCount (mSession.LocalsCount (procedureId)),
    mStackBegin (stack.Size () - mSession.ArgsCount (procedureId)),
    mProcId (procedureId),
    mAquiredSync (NO_INDEX)
{

  for (uint32_t localIndex = session.ArgsCount (mProcId) + 1;
       localIndex < mLocalsCount;
       ++localIndex)
    {
      StackValue localValue = session.ProcLocalValue (mProcId, localIndex);
      stack.Push (localValue);
    }

  //Count only procedure's parameters and local values,
  //but not the result value too.
  if ((mLocalsCount - 1) > stack.Size ())
    throw InterException (NULL, _EXTRA (InterException::STACK_CORRUPTED));
}


void
Processor::Run ()
{
  W_OPCODE opcode;

  try
  {
    while (mCodePos < mCodeSize)
      {
        int64_t offset = wh_compiler_decode_op (mCode + mCodePos, &opcode);

        assert (opcode < _SC (int, (sizeof operations / sizeof operations[0])));
        assert (opcode != 0);
        assert ((offset > 0) && (offset < 3));

        operations[opcode] (*this, offset);

        mCodePos += offset;

        assert ((mCodePos <= mCodeSize)
                || (_SC (uint64_t, offset) == mCodeSize));
      }
  }
  catch (...)
  {
      if (mAquiredSync != NO_INDEX)
        ReleaseSync (mAquiredSync);

      assert (mStack.Size () >= mStackBegin);

      throw;
  }

  //After a procedure execution, only the return value should be present
  //on the stack
  assert (mStack.Size () == (mStackBegin + 1));
}


void
Processor::AquireSync (const uint8_t sync)
{
  if (mAquiredSync != NO_INDEX)
    throw InterException (NULL, _EXTRA (InterException::NEESTED_SYNC_REQ));

  mSession.AquireProcSync (mProcId, sync);
  mAquiredSync = sync;
}


void
Processor::ReleaseSync (const uint8_t sync)
{
  if (mAquiredSync != sync)
    throw InterException (NULL, _EXTRA (InterException::SYNC_NOT_AQUIRED));

  mSession.ReleaseProcSync (mProcId, sync);
  mAquiredSync = NO_INDEX;
}


} //namespace prima
} //namespace whisper

