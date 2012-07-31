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

#include "compiler/include/whisperc/wopcodes.h"

#include "pm_processor.h"

using namespace prima;

inline static D_UINT16
load_le_uint16 (const D_UINT8* const pSrc)
{
  D_UINT16 result = pSrc[1];
  result <<= 8; result |= pSrc[0];

  return result;
}

inline static D_UINT32
load_le_uint32 (const D_UINT8* const pSrc)
{
  D_UINT32 result = pSrc[3];
  result <<= 8; result |= pSrc[2];
  result <<= 8; result |= pSrc[1];
  result <<= 8; result |= pSrc[0];

  return result;
}

inline static D_UINT64
load_le_uint64 (const D_UINT8* const pSrc)
{
  D_UINT64 result = pSrc[7];
  result <<= 8; result |= pSrc[6];
  result <<= 8; result |= pSrc[5];
  result <<= 8; result |= pSrc[4];
  result <<= 8; result |= pSrc[3];
  result <<= 8; result |= pSrc[2];
  result <<= 8; result |= pSrc[1];
  result <<= 8; result |= pSrc[0];

  return result;
}

static void
op_func_ldnull (Processor& processor, D_INT64&)
{
  processor.GetStack ().Push ();
}

static void
op_func_ldc (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSChar value (load_le_uint32 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldi8 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt8 value (*pData);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT8);
}

static void
op_func_ldi16 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt16 value (load_le_uint16 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT16);
}

static void
op_func_ldi32 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt32 value (load_le_uint32 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldi64 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  DBSUInt64 value (load_le_uint64 (pData));

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT64);
}

static void
op_func_ldd (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT8 day   = pData[0];
  const D_UINT8 month = pData[1];
  const D_INT16 year  = load_le_uint16 (pData + 2);
  DBSDate value (year, month, day);

  processor.GetStack ().Push (value);

  offset += 2 + sizeof (D_UINT16);
}

static void
op_func_lddt (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT8 sec   = pData[0];
  const D_UINT8 min   = pData[1];
  const D_UINT8 hour  = pData[2];
  const D_UINT8 day   = pData[3];
  const D_UINT8 month = pData[4];
  const D_INT16 year  = load_le_uint16 (pData + 5);
  DBSDateTime value (year, month, day, hour, min, sec);

  processor.GetStack ().Push (value);

  offset += 5 + sizeof (D_UINT16);
}

static void
op_func_ldht (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT32 usec  = load_le_uint32 (pData);
  const D_UINT8  sec   = pData[4];
  const D_UINT8  min   = pData[5];
  const D_UINT8  hour  = pData[6];
  const D_UINT8  day   = pData[7];
  const D_UINT8  month = pData[8];
  const D_INT16  year  = load_le_uint16 (pData + 9);
  DBSHiresTime value (year, month, day, hour, min, sec, usec);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32) + 5 + sizeof (D_UINT16);
}

static void
op_func_ldr (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_INT64 intPart     = _SC (D_INT64, load_le_uint64 (pData));
  D_INT8        decExp      = *(pData + sizeof (D_UINT64));
  const bool    negativeExp = (decExp > 0) ? false : true;

  if (negativeExp)
    decExp *= -1;

  D_UINT64 fracPart = 1;
  while (decExp-- > 0)
    fracPart *= 10;

  RICHREAL_T basicValue = intPart;

  if (negativeExp)
    basicValue /= fracPart;
  else
    basicValue *= fracPart;

  DBSRichReal value (basicValue);
  processor.GetStack ().Push (value);

  offset += (sizeof (D_UINT8) + sizeof (D_UINT64));
}

static void
op_func_ldt (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT32 textOff  = load_le_uint32 (pData);
  const D_UINT8* pTextSrc = processor.GetUnit ().GetConstData (textOff);

  assert (pTextSrc != NULL);

  DBSText value (pTextSrc);
  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldbt (Processor& processor, D_INT64& offset)
{
  DBSBool value (true);
  processor.GetStack ().Push (value);
}

static void
op_func_ldbf (Processor& processor, D_INT64& offset)
{
  DBSBool value (false);
  processor.GetStack ().Push (value);
}

static void
op_func_ldlo8 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT8 localIndex = *pData;

  assert (localIndex < processor.LocalsCount ());

  StackValue& localValue = processor.GetStack ()[processor.StackBegin () +
                                                 localIndex];
  LocalOperand localOp (localValue);
  StackValue   value (localOp);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT8);
}

static void
op_func_ldlo16 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT16 localIndex = load_le_uint16 (pData);

  assert (localIndex < processor.LocalsCount ());

  StackValue& localValue = processor.GetStack ()[processor.StackBegin () +
                                                 localIndex];
  LocalOperand localOp (localValue);
  StackValue   value (localOp);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT16);
}

static void
op_func_ldlo32 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;
  const D_UINT32 localIndex = load_le_uint32 (pData);

  assert (localIndex < processor.LocalsCount ());

  StackValue& localValue = processor.GetStack ()[processor.StackBegin () +
                                                 localIndex];
  LocalOperand localOp (localValue);
  StackValue   value (localOp);

  processor.GetStack ().Push (value);

  offset += sizeof (D_UINT32);
}

static void
op_func_ldgb8 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT8  globalIndex = *pData;
  const D_UINT32 glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (D_UINT8);
}

static void
op_func_ldgb16 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT16 globalIndex = load_le_uint16 (pData);
  const D_UINT32 glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (D_UINT16);
}

static void
op_func_ldgb32 (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT32 globalIndex = load_le_uint32 (pData);
  const D_UINT32 glbId       = processor.GetUnit ().GetGlobalId (globalIndex);
  StackValue     glbValue    = processor.GetSession ().GetGlobalValue (glbId);

  processor.GetStack ().Push (glbValue);

  offset += sizeof (D_UINT32);
}

static void
op_func_cts (Processor& processor, D_INT64& offset)
{
  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          processor.GetStack ().Size ());

  processor.GetStack ().Pop (1);
}

template <class T> static void
op_func_stXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  I_Operand& dest   = stack[stackSize - 2].GetOperand ();

  T value;
  source.GetValue (value);
  dest.SetValue (value);

  stack.Pop (1);
}

static void
op_func_stta (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  I_Operand& dest   = stack[stackSize - 2].GetOperand ();

  if (source.IsNull ())
    {
      if (dest.IsNull() != false)
        {
          TableOperand& tableOp = _SC (TableOperand&, dest);
          TableOperand  newTabOp (tableOp.GetTableRef ().GetDBSHandler (),
                                  tableOp.GetTableRef ().GetTable ().Spawn());
          tableOp = newTabOp;
        }
    }
  else
    {
      TableOperand& srcTableOp  = _SC (TableOperand&, source);
      TableOperand& destTableOp = _SC (TableOperand&, dest);

      destTableOp = srcTableOp;
    }

  stack.Pop (1);
}

static void
op_func_stf (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  FieldOperand& src  = _SC (FieldOperand&, stack[stackSize - 1].GetOperand ());
  FieldOperand& dest = _SC (FieldOperand&, stack[stackSize - 2].GetOperand ());

  dest = src;

  stack.Pop (1);
}

static void
op_func_inull (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
           stackSize);

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  DBSBool result (source.IsNull ());

  stack.Pop (1);
  stack.Push (result);
}

static void
op_func_nnull (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
           stackSize);

  I_Operand& source = stack[stackSize - 1].GetOperand ();
  DBSBool result (! source.IsNull ());

  stack.Pop (1);
  stack.Push (result);
}

static void
op_func_call (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_UINT32 prcMgrId = load_le_uint32 (pData);
  offset += sizeof (D_UINT32);

  Session&      session         = processor.GetSession ();
  SessionStack& stack           = processor.GetStack ();
  const D_UINT  procLocalsCount = session.LocalsCount (prcMgrId);

  for (D_UINT32 localIndex = session.ArgsCount (prcMgrId) + 1;
       localIndex < procLocalsCount;
       ++localIndex)
    {
      StackValue localValue = session.ProcLocalValue (prcMgrId, localIndex);
      stack.Push (localValue);
    }

  Unit&                procUnit     = session.ProcUnit (prcMgrId);
  const D_UINT8* const pProcCode    = session.ProcCode (prcMgrId);
  const D_UINT32       procCodeSize = session.ProcCodeSize (prcMgrId);

  Processor procedure (session,
                       stack,
                       procUnit,
                       pProcCode,
                       procCodeSize,
                       procLocalsCount,
                       prcMgrId);
  procedure.Run ();
}

static void
op_func_ret (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) < stackSize);

  StackValue result = stack[stackSize - 1];

  stack.Pop (stackSize - processor.StackBegin());
  stack.Push (result);

  offset = processor.CodeSize (); //Signal the procedure return
}

template <class DBS_T> static void
op_func_addXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBS_T (firstOp.m_Value + secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

static void
op_func_addt (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBSText firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBSText secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBSText result;
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
op_func_andXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBS_T (firstOp.m_Value & secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_divXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  if (secondOp == DBS_T (0))
    throw InterException (NULL, _EXTRA (InterException::DIVIDE_BY_ZERO));

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBS_T (firstOp.m_Value / secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_eqXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBSBool result (firstOp == secondOp);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_geXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBSBool result ((firstOp < secondOp) == false);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_gtXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBSBool result (((firstOp < secondOp) || (firstOp == secondOp)) == false);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_leXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBSBool result ((firstOp < secondOp) || (firstOp == secondOp));

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_ltXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBSBool result (firstOp < secondOp);

  stack.Pop (2);
  stack.Push (result);
}

static void
op_func_mod (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBSInt64 firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBSInt64 secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  if (secondOp == DBSInt64 (0))
    throw InterException (NULL, _EXTRA (InterException::DIVIDE_BY_ZERO));

  DBSInt64 result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBSInt64 (firstOp.m_Value % secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_mulXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBS_T (firstOp.m_Value * secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_neXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBSBool result ((firstOp == secondOp) == false);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_notXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          stackSize);

  DBS_T operand;
  stack[stackSize - 1].GetOperand ().GetValue (operand);

  DBS_T result;
  if (operand.IsNull () == false)
    result = DBS_T (~operand.m_Value);

  stack.Pop (1);
  stack.Push (result);
}

template <> void
op_func_notXX<DBSBool> (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          stackSize);

  DBSBool operand;
  stack[stackSize - 1].GetOperand ().GetValue (operand);

  DBSBool result;
  if (operand.IsNull () == false)
    result = DBSBool (! operand.m_Value);

  stack.Pop (1);
  stack.Push (result);
}


template <class DBS_T> static void
op_func_orXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBS_T (firstOp.m_Value | secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_subXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBS_T (firstOp.m_Value - secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

template <class DBS_T> static void
op_func_xorXX (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBS_T firstOp;
  stack[stackSize - 2].GetOperand ().GetValue (firstOp);

  DBS_T secondOp;
  stack[stackSize - 1].GetOperand ().GetValue (secondOp);

  DBS_T result;
  if (firstOp.IsNull ())
    result = secondOp;
  else if (secondOp.IsNull ())
    result = firstOp;
  else
    result = DBS_T (firstOp.m_Value ^ secondOp.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

static void
op_func_jf (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          stackSize);

  DBSBool firstOp;
  stack[stackSize - 1].GetOperand ().GetValue (firstOp);

  if ((firstOp.IsNull() == false) && (firstOp.m_Value == false))
    {
      const D_UINT8* const pData = processor.Code () +
                                   processor.CurrentOffset () +
                                   offset;

      const D_INT32 jmpOffset = _SC (D_INT32, load_le_uint32 (pData));
      offset += jmpOffset;
    }
}

static void
op_func_jfc (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          stackSize);

  DBSBool firstOp;
  stack[stackSize - 1].GetOperand ().GetValue (firstOp);
  stack.Pop (1);

  if ((firstOp.IsNull() == false) && (firstOp.m_Value == false))
    {
      const D_UINT8* const pData = processor.Code () +
                                   processor.CurrentOffset () +
                                   offset;

      const D_INT32 jmpOffset = _SC (D_INT32, load_le_uint32 (pData));
      offset += jmpOffset;
    }
}

static void
op_func_jt (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          stackSize - 1);

  DBSBool firstOp;
  stack[stackSize - 1].GetOperand ().GetValue (firstOp);

  if ((firstOp.IsNull() == false) && (firstOp.m_Value != false))
    {
      const D_UINT8* const pData = processor.Code () +
                                   processor.CurrentOffset () +
                                   offset;

      const D_INT32 jmpOffset = _SC (D_INT32, load_le_uint32 (pData));
      offset += jmpOffset;
    }
}

static void
op_func_jtc (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          stackSize - 1);

  DBSBool firstOp;
  stack[stackSize - 1].GetOperand ().GetValue (firstOp);
  stack.Pop (1);

  if ((firstOp.IsNull() == false) && (firstOp.m_Value != false))
    {
      const D_UINT8* const pData = processor.Code () +
                                   processor.CurrentOffset () +
                                   offset;

      const D_INT32 jmpOffset = _SC (D_INT32, load_le_uint32 (pData));
      offset += jmpOffset;
    }
}

static void
op_func_jmp (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const pData = processor.Code () +
                               processor.CurrentOffset () +
                               offset;

  const D_INT32 jmpOffset = _SC (D_INT32, load_le_uint32 (pData));
  offset += jmpOffset;
}

template <D_UINT EXCEPTION_CODE> void
op_func_ind (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBSUInt64 index;
  stack[stackSize - 1].GetOperand ().GetValue (index);

  if (index.IsNull ())
    throw InterException (NULL, _EXTRA (EXCEPTION_CODE));

  StackValue result =
      stack[stackSize - 2].GetOperand ().GetValueAt (index.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

static void
op_func_indta (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  DBSUInt64 index;
  stack[stackSize - 1].GetOperand ().GetValue (index);

  if (index.IsNull ())
    throw InterException (NULL, _EXTRA (InterException::ROW_INDEX_NULL));

  TableOperand& tableOp = _SC (TableOperand&,
                                stack[stackSize - 2].GetOperand ());

  const D_UINT8* const  pData = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const D_UINT32        textOff  = load_le_uint32 (pData);
  const D_UINT8* const  pTextSrc = processor.GetUnit ().GetConstData (textOff);

  offset += sizeof (D_UINT32);

  FIELD_INDEX field = tableOp.GetTable ().GetFieldIndex (_RC (const D_CHAR*,
                                                              pTextSrc));

  FieldOperand fieldOp (tableOp, field);
  StackValue   result = fieldOp.GetValueAt (index.m_Value);

  stack.Pop (2);
  stack.Push (result);
}

static void
op_func_self (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          stackSize);

  TableOperand& tableOp = _SC (TableOperand&,
                                stack[stackSize - 1].GetOperand ());

  const D_UINT8* const  pData = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const D_UINT32        textOff  = load_le_uint32 (pData);
  const D_UINT8* const  pTextSrc = processor.GetUnit ().GetConstData (textOff);

  offset += sizeof (D_UINT32);

  FIELD_INDEX field = tableOp.GetTable ().GetFieldIndex (_RC (const D_CHAR*,
                                                              pTextSrc));

  FieldOperand fieldOp (tableOp, field);
  StackValue   result (fieldOp);

  stack.Pop (1);
  stack.Push (result);
}

static void
op_func_bsync (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const  pData = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const D_UINT8 syncStmt = *pData;
  offset += sizeof (D_UINT8);

  processor.AquireSync (syncStmt);
}

static void
op_func_esync (Processor& processor, D_INT64& offset)
{
  const D_UINT8* const  pData = processor.Code () +
                                processor.CurrentOffset () +
                                offset;
  const D_UINT8 syncStmt = *pData;
  offset += sizeof (D_UINT8);

  processor.ReleaseSync (syncStmt);
}

template <class DBS_T> static void
op_func_sadd (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfAdd (delta);

  stack.Pop (1);
}

template <class DBS_T> static void
op_func_ssub (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfSub (delta);

  stack.Pop (1);
}

template <class DBS_T> static void
op_func_smul (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfMul (delta);

  stack.Pop (1);
}

template <class DBS_T> static void
op_func_sdiv (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfDiv (delta);

  stack.Pop (1);
}

template <class DBS_T> static void
op_func_smod (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfMod (delta);

  stack.Pop (1);
}

template <class DBS_T> static void
op_func_sand (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfAnd (delta);

  stack.Pop (1);
}

template <class DBS_T> static void
op_func_sxor (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfXor (delta);

  stack.Pop (1);
}

template <class DBS_T> static void
op_func_sor (Processor& processor, D_INT64& offset)
{
  SessionStack& stack     = processor.GetStack ();
  const size_t  stackSize = stack.Size ();

  assert ((processor.StackBegin () + processor.LocalsCount ()) <
          (stackSize - 1));

  I_Operand& destOp = stack[stackSize - 2].GetOperand ();

  DBS_T delta;
  stack[stackSize - 1].GetOperand ().GetValue (delta);

  destOp.SelfOr (delta);

  stack.Pop (1);
}



typedef void (*OP_FUNC) (Processor& processor, D_INT64& ioOffset);

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
                                op_func_ldr,
                                op_func_ldt,
                                op_func_ldbt,
                                op_func_ldbf,
                                op_func_ldlo8,
                                op_func_ldlo16,
                                op_func_ldlo32,
                                op_func_ldgb8,
                                op_func_ldgb16,
                                op_func_ldgb32,
                                op_func_cts, //

                                op_func_stXX<DBSBool>,
                                op_func_stXX<DBSChar>,
                                op_func_stXX<DBSDate>,
                                op_func_stXX<DBSDateTime>,
                                op_func_stXX<DBSHiresTime>,
                                op_func_stXX<DBSInt8>,
                                op_func_stXX<DBSInt16>,
                                op_func_stXX<DBSInt32>,
                                op_func_stXX<DBSInt64>,
                                op_func_stXX<DBSReal>,
                                op_func_stXX<DBSRichReal>,
                                op_func_stXX<DBSText>,
                                op_func_stXX<DBSUInt8>,
                                op_func_stXX<DBSUInt16>,
                                op_func_stXX<DBSUInt32>,
                                op_func_stXX<DBSUInt64>,
                                op_func_stta,
                                op_func_stf,
                                op_func_stXX<DBSArray>,

                                op_func_inull, //
                                op_func_nnull, //

                                op_func_call, //
                                op_func_ret, //

                                op_func_addXX<DBSInt64>,
                                op_func_addXX<DBSReal>,
                                op_func_addXX<DBSRichReal>,
                                op_func_addt,

                                op_func_andXX<DBSInt64>,
                                op_func_andXX<DBSBool>,

                                op_func_divXX<DBSInt64>,
                                op_func_divXX<DBSReal>,
                                op_func_divXX<DBSRichReal>,

                                op_func_eqXX<DBSInt64>,
                                op_func_eqXX<DBSBool>,
                                op_func_eqXX<DBSChar>,
                                op_func_eqXX<DBSDate>,
                                op_func_eqXX<DBSDateTime>,
                                op_func_eqXX<DBSHiresTime>,
                                op_func_eqXX<DBSReal>,
                                op_func_eqXX<DBSRichReal>,
                                op_func_eqXX<DBSText>,

                                op_func_geXX<DBSInt64>,
                                op_func_geXX<DBSChar>,
                                op_func_geXX<DBSDate>,
                                op_func_geXX<DBSDateTime>,
                                op_func_geXX<DBSHiresTime>,
                                op_func_geXX<DBSReal>,
                                op_func_geXX<DBSRichReal>,

                                op_func_gtXX<DBSInt64>,
                                op_func_gtXX<DBSChar>,
                                op_func_gtXX<DBSDate>,
                                op_func_gtXX<DBSDateTime>,
                                op_func_gtXX<DBSHiresTime>,
                                op_func_gtXX<DBSReal>,
                                op_func_gtXX<DBSRichReal>,

                                op_func_leXX<DBSInt64>,
                                op_func_leXX<DBSChar>,
                                op_func_leXX<DBSDate>,
                                op_func_leXX<DBSDateTime>,
                                op_func_leXX<DBSHiresTime>,
                                op_func_leXX<DBSReal>,
                                op_func_leXX<DBSRichReal>,

                                op_func_ltXX<DBSInt64>,
                                op_func_ltXX<DBSChar>,
                                op_func_ltXX<DBSDate>,
                                op_func_ltXX<DBSDateTime>,
                                op_func_ltXX<DBSHiresTime>,
                                op_func_ltXX<DBSReal>,
                                op_func_ltXX<DBSRichReal>,

                                op_func_mod,

                                op_func_mulXX<DBSInt64>,
                                op_func_mulXX<DBSReal>,
                                op_func_mulXX<DBSRichReal>,

                                op_func_neXX<DBSInt64>,
                                op_func_neXX<DBSBool>,
                                op_func_neXX<DBSChar>,
                                op_func_neXX<DBSDate>,
                                op_func_neXX<DBSDateTime>,
                                op_func_neXX<DBSHiresTime>,
                                op_func_neXX<DBSReal>,
                                op_func_neXX<DBSRichReal>,
                                op_func_neXX<DBSText>,

                                op_func_notXX<DBSInt64>,
                                op_func_notXX<DBSBool>,

                                op_func_orXX<DBSInt64>,
                                op_func_orXX<DBSBool>,

                                op_func_subXX<DBSInt64>,
                                op_func_subXX<DBSReal>,
                                op_func_subXX<DBSRichReal>,

                                op_func_orXX<DBSInt64>,
                                op_func_orXX<DBSBool>,

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

                                op_func_sadd<DBSInt64>,
                                op_func_sadd<DBSRichReal>,
                                op_func_sadd<DBSChar>,
                                op_func_sadd<DBSText>,

                                op_func_ssub<DBSInt64>,
                                op_func_ssub<DBSRichReal>,

                                op_func_smul<DBSInt64>,
                                op_func_smul<DBSRichReal>,

                                op_func_sdiv<DBSInt64>,
                                op_func_sdiv<DBSRichReal>,

                                op_func_smod<DBSInt64>,

                                op_func_sand<DBSInt64>,
                                op_func_sand<DBSBool>,

                                op_func_sxor<DBSInt64>,
                                op_func_sxor<DBSBool>,

                                op_func_sor<DBSInt64>,
                                op_func_sor<DBSBool>
                              };

/////////////////////////////Processor/////////////////////////////////////////

void
Processor::Run ()
{
  W_OPCODE opcode;

  try
  {
    while (m_CodePos < m_CodeSize)
      {
        D_INT64 offset = whc_decode_opcode (m_pCode + m_CodePos, &opcode);

        assert (opcode < _SC (D_INT,
                              (sizeof operations / sizeof operations[0])));
        assert (opcode != 0);
        assert ((offset > 0) && (offset < 3));

        operations[opcode] (*this, offset);

        m_CodePos += offset;
        assert ((m_CodePos <= m_CodeSize)
                || (_SC (D_UINT64, offset) == m_CodeSize));
      }
  }
  catch (...)
  {
      if (m_AquiredSync != NO_INDEX)
        ReleaseSync (m_AquiredSync);

      assert (m_Stack.Size () >= m_StackBegin);

      throw;
  }

  //After a procedure execution, only the return value should be present
  //on the stack
  assert (m_Stack.Size () == (m_StackBegin + 1));
}

void
Processor::AquireSync (const D_UINT8 sync)
{
  if (m_AquiredSync != NO_INDEX)
    throw InterException (NULL, _EXTRA (InterException::NEESTED_SYNC_REQ));

  m_Session.AquireProcSync (m_ProcId, sync);
  m_AquiredSync = sync;
}

void
Processor::ReleaseSync (const D_UINT8 sync)
{
  if (m_AquiredSync != sync)
    throw InterException (NULL, _EXTRA (InterException::SYNC_NOT_AQUIRED));

  m_Session.ReleaseProcSync (m_ProcId, sync);
  m_AquiredSync = NO_INDEX;
}
